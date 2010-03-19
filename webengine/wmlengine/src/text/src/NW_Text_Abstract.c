/*
* Copyright (c) 2000 - 2001 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#include "NW_Text_AbstractI.h"

#include "nw_text_ucs2.h"
#include "nw_text_utf8.h"
#include "nw_text_latin1.h"
#include "nw_text_ascii.h"
#include "nwx_string.h"
#include "nw_string_string.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_Text_Abstract_Class_t NW_Text_Abstract_Class = {
  { /* NW_Object_Core     */
    /* super              */ &NW_Object_Dynamic_Class,
    /* queryInterface     */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base     */
    /* interfaceList      */ NULL
  },
  { /* NW_Object_Dynamic  */
    /* instanceSize       */ sizeof (NW_Text_Abstract_t),
    /* construct          */ _NW_Text_Abstract_Construct,
    /* destruct           */ _NW_Text_Abstract_Destruct
  },
  { /* NW_Text_Abstract   */
    /* createIterator     */ NULL,
    /* getStorageSize     */ NULL,
    /* getCharacterAt     */ NULL,
    /* getUCS2Buffer      */ NULL,
    /* getSpaceAfter      */ _NW_Text_Abstract_GetSpaceAfter
  }
};

/* ------------------------------------------------------------------------- *
   private methods
 * ------------------------------------------------------------------------- */

static
NW_Int32
NW_Text_Abstract_CompressCharacter(NW_Ucs2 character, NW_Bool retainLineFeeds)
{
  if (retainLineFeeds) {
    return (NW_Str_Isspace (character) && (!NW_Str_Isnewline (character)));
  }
  else {
    return NW_Str_Isspace (character);
  }
}

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Text_Abstract_Construct (NW_Object_Dynamic_t* dynamicObject,
                             va_list* argList)
{
  NW_Text_Abstract_t* thisObj;
  TBrowserStatusCode status;
  void* storage;
  NW_Text_Length_t characterCount;
  NW_Uint16 flags;

  /* parameter assertion block */
  NW_ASSERT (dynamicObject != NULL);
  NW_ASSERT (argList != NULL);

  /* for convenience */
  thisObj = NW_Text_AbstractOf (dynamicObject);

  /* initialize our member variables */
  storage = va_arg (*argList, void*);
  characterCount = va_arg (*argList, NW_Text_Length_t);
  flags = (NW_Uint16)va_arg (*argList, NW_Uint32);
  status =
    NW_Text_Abstract_SetStorage (thisObj, storage, characterCount, flags);
  if (status != KBrsrSuccess) {
    return status;
  }

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
void
_NW_Text_Abstract_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_Text_Abstract_t* thisObj;

  /* parameter assertion block */
  NW_ASSERT (dynamicObject != NULL);

  /* for convenience */
  thisObj = NW_Text_AbstractOf (dynamicObject);

  /* if we own the storage, release it */
  if (thisObj->flags & NW_Text_Flags_TakeOwnership) {
    NW_Mem_Free ((void*) thisObj->storage);
  }
}

/* ------------------------------------------------------------------------- */
NW_Text_Length_t
_NW_Text_Abstract_GetSpaceAfter (const NW_Text_Abstract_t* thisObj,
                                 NW_Text_Length_t index)
{
  NW_REQUIRED_PARAM(thisObj);
  return index;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Text_Abstract_CalcCharacterCount (NW_Text_Abstract_t* thisObj)
{
  NW_Ucs4 character;
  NW_Text_Iterator_t* iterator = NULL;

  NW_TRY (status) {
    if (thisObj->characterCount == 0) {
      thisObj->characterCount = (NW_Text_Length_t) ~0;
      status = NW_Text_CreateIterator (thisObj, NW_Text_Direction_Forward, &iterator);
      NW_THROW_ON_ERROR (status);

      while ((status = NW_Text_Iterator_GetNext (iterator, &character)) == 
          KBrsrSuccess && character != '\0')
      {
        /* Nothing needed... */
      }

      NW_THROW_ON_ERROR (status);

      thisObj->characterCount =
        (NW_Text_Length_t) (NW_Text_Iterator_GetIndex (iterator) - 1);
      thisObj->flags |= (NW_Uint16) NW_Text_Flags_NullTerminated;
    } else { /* determine if the string is null terminated */
      character = NW_Text_GetAt (thisObj, thisObj->characterCount);
      if (character == '\0') {
        thisObj->flags |= (NW_Uint16) NW_Text_Flags_NullTerminated;
      }
    }
  }

  NW_CATCH (status) {
    thisObj->characterCount = 0;
  }

  NW_FINALLY {
    NW_Object_Delete (iterator);
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Text_Abstract_SetStorage (NW_Text_Abstract_t* thisObj,
                              void* storage,
                              NW_Text_Length_t characterCount,
                              NW_Uint16 flags)
{
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_Text_Abstract_Class));

  NW_TRY (status) {
    NW_Uint32 storageSize;

    /* if the old storage is under our ownership, we must deallocate it first */
    if (thisObj->flags & NW_Text_Flags_TakeOwnership) {
      NW_Mem_Free ((void*) thisObj->storage);
    }
    thisObj->flags = 0;

    /* if the supplied pointer is NULL, we have no more work to do */
    thisObj->storage = storage;
    if (thisObj->storage == NULL) {
      thisObj->characterCount = 0;
      NW_THROW_SUCCESS (status);
    }

    /* determine the character count */
    thisObj->characterCount = characterCount;
    status = NW_Text_Abstract_CalcCharacterCount (thisObj);
    NW_THROW_ON_ERROR (status);

    /* if we are not directed to make a copy of the source, we simply assign the
       pointer and set the flags appropriately */
    if (!(flags & NW_Text_Flags_Copy)) {
      thisObj->flags |= (NW_Uint16) (flags & NW_Text_Flags_TakeOwnership);
      return KBrsrSuccess;
    }

    /* if the caller directed us to make a copy, we must do so now */
    storageSize = NW_Text_Abstract_GetStorageSize (thisObj);
    thisObj->storage = NW_Mem_Malloc (storageSize + sizeof (NW_Ucs4));
    if (thisObj->storage == NULL) {
      return KBrsrOutOfMemory;
    }
    (void) NW_Mem_memcpy (thisObj->storage, storage, storageSize);
    (void) NW_Mem_memset ((char*) thisObj->storage + storageSize, 0,
                          sizeof (NW_Ucs4));
    thisObj->flags |=
      (NW_Uint16) (NW_Text_Flags_TakeOwnership | NW_Text_Flags_NullTerminated);
  }

  NW_CATCH (status) { /* empty */ }

  NW_FINALLY {
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Text_Abstract_SetCharacterCount (NW_Text_Abstract_t* thisObj,
                                     NW_Text_Length_t characterCount)
{
  thisObj->characterCount = characterCount;
  thisObj->flags &= (NW_Uint16) ~NW_Text_Flags_NullTerminated;
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
NW_Ucs2*
_NW_Text_Abstract_GetUCS2Buffer (const NW_Text_Abstract_t* thisObj,
                                 NW_Uint16 flags,
                                 NW_Text_Length_t* characterCount,
                                 NW_Bool* freeNeeded)
{
  NW_Ucs2* buffer = NULL;
  NW_Text_Iterator_t* iterator = NULL;

  NW_TRY(status) {
    NW_Ucs4 character;
    NW_Text_Length_t dstIndex;
    NW_Bool retainLineFeeds = (NW_Bool) ((thisObj->flags & NW_Text_Flags_RetainLineFeeds) ||
           (flags & NW_Text_Flags_RetainLineFeeds));


    /* To reduce the amount of processing, we allocate for the worst-case
       buffer in which no spaces were compressed and a terminating '\0' has
       to be added.  Given that the resulting string is only temporary storage, 
       this does not matter much */
    buffer = (NW_Ucs2*) NW_Mem_Malloc ((thisObj->characterCount + 1) * sizeof (NW_Ucs2));
    NW_THROW_OOM_ON_NULL (buffer, status);

    status =
      NW_Text_CreateIterator (thisObj, NW_Text_Direction_Forward, &iterator);
    NW_THROW_ON_ERROR (status);
    dstIndex = 0;

    while ((status =
              NW_Text_Iterator_GetNext (iterator,
                                        &character)) == KBrsrSuccess) {
      /* check to see if we've been asked to compress whitespace */
      if ((thisObj->flags & NW_Text_Flags_CompressWhitespace ||
           flags & NW_Text_Flags_CompressWhitespace) &&
           NW_Text_Abstract_CompressCharacter((NW_Ucs2) character, retainLineFeeds)) {

        /* we need to preserve the leading whitespace */
        buffer[dstIndex++] = 0x0020; /* ' ' */

        /* skip all consecutive whitespace characters */
        do {
          status = NW_Text_Iterator_GetNext (iterator, &character);
        } while (status == KBrsrSuccess &&
                 NW_Text_Abstract_CompressCharacter((NW_Ucs2) character, retainLineFeeds));                                                     

        if (status == KBrsrIterateDone) {
          break;
        }
        NW_THROW_ON_ERROR (status);
      }

      buffer[dstIndex++] = (NW_Ucs2) character;
    }

    /* Append a '\0' terminator */
    buffer[dstIndex] = '\0';

    /* update the 'out' parameters */
    if (characterCount != NULL) {
      *characterCount = dstIndex;
    }
    if (freeNeeded != NULL) {
      *freeNeeded = NW_TRUE;
    }
  }
  
  NW_CATCH (status) {
    NW_Mem_Free (buffer);
    buffer = NULL;
  }

  NW_FINALLY {
    NW_Object_Delete (iterator);

    return buffer;
  } NW_END_TRY;
}

/* ------------------------------------------------------------------------- */
NW_Text_Abstract_t*
_NW_Text_Abstract_Copy (const NW_Text_Abstract_t* thisObj,
                        NW_Bool deepCopy)
{
  return (NW_Text_Abstract_t*) NW_Object_New (NW_Object_GetClass (thisObj), 
      thisObj->storage, thisObj->characterCount, deepCopy ? NW_Text_Flags_Copy : 0);
}

/* ------------------------------------------------------------------------- */
NW_Int32
_NW_Text_Abstract_PartialCompare (const NW_Text_Abstract_t* thisObj,
                                  const NW_Text_Abstract_t* rhsText,
                                  NW_Text_Length_t numCharacters)
{
  TBrowserStatusCode status;
  NW_Text_Iterator_t* lhsIterator;
  NW_Ucs4 lhsCharacter;
  NW_Text_Iterator_t* rhsIterator;
  NW_Ucs4 rhsCharacter;

  /* ensure that the two text objects character counts are of least
     'numCharacters' */
  if (thisObj->characterCount < numCharacters ||
      rhsText->characterCount < numCharacters) {
    return thisObj->characterCount - rhsText->characterCount;
  }

  /* instantiate the iterators */
  status = NW_Text_CreateIterator (thisObj, NW_Text_Direction_Forward, &lhsIterator);
  if (status != KBrsrSuccess) {
    return -1;
  }
  status = NW_Text_CreateIterator (rhsText, NW_Text_Direction_Forward, &rhsIterator);
  if (status != KBrsrSuccess) {
    NW_Object_Delete (rhsIterator);
    return -1;
  }

  /* iterate through the text objects a character at a time */
  while ((status =
            NW_Text_Iterator_GetNext (lhsIterator,
                                      &lhsCharacter)) == KBrsrSuccess &&
         (status =
            NW_Text_Iterator_GetNext (rhsIterator,
                                      &rhsCharacter)) == KBrsrSuccess) {
    if (lhsCharacter != rhsCharacter) {
      NW_Object_Delete (lhsIterator);
      NW_Object_Delete (rhsIterator);
      return lhsCharacter - rhsCharacter;
    }
  }

  /* the two text objects point to the same Unicode string for the first
     number of characters specified by 'numCharacters' */
     
  if(lhsIterator != NULL)
      {
      NW_Object_Delete (lhsIterator);
      }
  if(rhsIterator != NULL)
      {
      NW_Object_Delete (rhsIterator);
      }
  return 0;
}

/* ------------------------------------------------------------------------- */
NW_Int32
_NW_Text_Abstract_Compare (const NW_Text_Abstract_t* thisObj,
                           const NW_Text_Abstract_t* rhsText)
{
  /* ensure that the two text objects are of equal length */
  if (thisObj->characterCount != rhsText->characterCount) {
    return thisObj->characterCount - rhsText->characterCount;
  }
  return NW_Text_PartialCompare (thisObj, rhsText, thisObj->characterCount);
}

/* ------------------------------------------------------------------------- */
NW_Int32
_NW_Text_Abstract_PartialASCIICompare (const NW_Text_Abstract_t* thisObj,
                                       const NW_Uint8* rhsString,
                                       NW_Text_Length_t numCharacters)
{
  NW_Text_Length_t index;

  /* iterate through the text object and string a character at a time */
  for (index = 0; index < numCharacters; index++) {
    NW_Ucs4 lhsCharacter;

    /* ensure that we don't overrun the text buffer */
    if (index == thisObj->characterCount) {
      return -1;
    }

    /* compare the characters */
    lhsCharacter = NW_Text_GetAt (thisObj, index);
    if (lhsCharacter != rhsString[index]) {
      return lhsCharacter - rhsString[index];
    }
  }

  /* the two text objects point to the same Unicode string for the first
     number of characters specified by 'numCharacters' */
  return 0;
}

/* ------------------------------------------------------------------------- */
NW_Int32
_NW_Text_Abstract_ASCIICompare (const NW_Text_Abstract_t* thisObj,
                                const NW_Uint8* rhsString)
{
  /* note that there is no need to compare the text character count with the
     string length as the PartialASCIICompare will do the right lexical
     "thing" if they objects are of unequal length */
  return NW_Text_PartialCompare (thisObj, rhsString, thisObj->characterCount);
}

/* ------------------------------------------------------------------------- */
NW_Text_Abstract_t*
_NW_Text_Abstract_Split (NW_Text_Abstract_t* thisObj,
                         NW_Text_Length_t index)
{
  NW_Text_Abstract_t* newText;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_Text_Abstract_Class));
  NW_ASSERT (index < NW_Text_GetCharCount (thisObj));

  /* create the new Text object */
  newText = (NW_Text_t*)
    NW_Object_New (NW_Object_GetClass (thisObj), thisObj->storage, index,
                   thisObj->flags & NW_Text_Flags_TakeOwnership);
  if (newText == NULL) {
    return NULL;
  }

  /* update our member variables */
  thisObj->storage =
    (NW_Uint8*) thisObj->storage + NW_Text_Abstract_GetStorageSize (newText);
  thisObj->characterCount =
    (NW_Text_Length_t) (thisObj->characterCount - index);
  thisObj->flags &= ~NW_Text_Flags_TakeOwnership;

  /* successful completion */
  return newText;
}

/* ------------------------------------------------------------------------- */
NW_Text_Abstract_t*
_NW_Text_Abstract_SplitOnSpace (NW_Text_Abstract_t* thisObj,
                                NW_Text_Length_t index,
                                NW_Text_Length_t *actualIndex)
{
  NW_Text_Abstract_t* newText;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_Text_Abstract_Class));
  NW_ASSERT (index < NW_Text_GetCharCount (thisObj));
  NW_ASSERT(actualIndex != NULL);

  *actualIndex = NW_Text_GetSpaceAfter(thisObj, index);
  if (*actualIndex == NW_Text_GetCharCount (thisObj))
    return NULL;

  /* create the new Text object */
  newText = (NW_Text_t*)
    NW_Object_New (NW_Object_GetClass (thisObj), thisObj->storage, *actualIndex,
                   thisObj->flags & NW_Text_Flags_TakeOwnership);
  if (newText == NULL) {
    return NULL;
  }

  /* update our member variables */
  thisObj->storage =
    (NW_Uint8*) thisObj->storage + NW_Text_Abstract_GetStorageSize (newText);
  thisObj->characterCount =
    (NW_Text_Length_t) (thisObj->characterCount - (*actualIndex));
  thisObj->flags &= ~NW_Text_Flags_TakeOwnership;

  /* successful completion */
  return newText;
}

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_Text_Abstract_t*
NW_Text_New (NW_Uint32 encoding,
             void* storage,
             NW_Text_Length_t characterCount,
             NW_Uint16 flags)
{
  switch (encoding) {
    case HTTP_iso_10646_ucs_2:
      return (NW_Text_Abstract_t*)
        NW_Text_UCS2_New (storage, characterCount, flags);

    case HTTP_utf_8:
      return (NW_Text_Abstract_t*)
        NW_Text_UTF8_New (storage, characterCount, flags);

    case HTTP_iso_8859_1:
      return (NW_Text_Abstract_t*)
        NW_Text_Latin1_New (storage, characterCount, flags);

    case HTTP_us_ascii:
      return (NW_Text_Abstract_t*)
        NW_Text_ASCII_New (storage, characterCount, flags);

    default:
      /* unsupported encoding */
      NW_ASSERT(encoding == 0); /* force debug to stop on error */
      /* if given ownership of storage but failed to create an object, delete the storage */
      if (storage != NULL && flags & NW_Text_Flags_TakeOwnership) {
        NW_Mem_Free (storage);
      }
      return NULL;
  }
}

/* ------------------------------------------------------------------------- */
NW_Text_Abstract_t* 
NW_Text_NewFromStringAndDeleteString (NW_String_t* stringStr, 
                                      NW_Uint32 encoding)
{
  NW_Text_Abstract_t* textAbstract;

  /* parameter assertion block */
  NW_ASSERT(stringStr != NULL);

  /* */
  textAbstract =
    NW_Text_New (encoding, NW_String_getStorage (stringStr),
                 NW_String_getCharCount (stringStr, encoding),
                 (NW_Uint8) (NW_String_getUserOwnsStorage (stringStr) ?
                               NW_Text_Flags_TakeOwnership : 0));

  /* Storage was either owned and ownership passed to the NW_Text_Abstract_t or
     it wasn't owned.  Either way clear the owned bit, so storage
     won't be freed. */
  NW_String_clearUserOwnsStorage(stringStr);
  NW_String_delete (stringStr);

  return textAbstract;
}

/* ------------------------------------------------------------------------- */
NW_Text_t*
NW_Text_Concatenate(NW_Text_t* val1, NW_Text_t* val2)
    {
    NW_Ucs2* storage1 = NULL;
    NW_Text_Length_t length1;
    NW_Bool freeNeeded1 = NW_FALSE;
    NW_Text_t* newText = NULL;
    NW_Ucs2* storage2 = NULL;
    NW_Text_Length_t length2;
    NW_Bool freeNeeded2 = NW_FALSE;
    
    storage1 = NW_Text_GetUCS2Buffer (val1, NW_Text_Flags_NullTerminated |
                                  NW_Text_Flags_CompressWhitespace, &length1, &freeNeeded1);
    storage2 =
    NW_Text_GetUCS2Buffer (val2, NW_Text_Flags_NullTerminated | NW_Text_Flags_CompressWhitespace,
                           &length2, &freeNeeded2);
    if (storage1 && storage2)
        {
        NW_Ucs2* newStr = NW_Str_Newcat(storage1, storage2);
        if (newStr)
            {                   
            newText = NW_Text_New (HTTP_iso_10646_ucs_2, newStr, NW_Str_Strlen(newStr), NW_Text_Flags_TakeOwnership);
            if (!newText)
                {
                NW_Str_Delete(newStr);
                }
            }
        }
    if (freeNeeded1)
        {
        NW_Str_Delete(storage1);
        }
    if (freeNeeded2)
        {
        NW_Str_Delete(storage2);
        }
    return newText;
    }
