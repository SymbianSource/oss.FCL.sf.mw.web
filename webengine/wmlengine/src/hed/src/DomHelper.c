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


#include "nwx_defs.h"
#include "nw_hed_domhelper.h"

#include "nw_dom_element.h"
#include "nw_dom_domvisitor.h"
#include "nw_dom_text.h"
#include "nw_dom_document.h"
#include "nwx_string.h"
#include "nwx_url_utils.h"
#include "nwx_logger.h"
#include "TextReverse.h"
#include "nwx_settings.h"
#include <nwx_status.h>
#include "nwx_statuscodeconvert.h"
#include "nw_xhtml_xhtml_1_0_tokens.h"
#include "BrsrStatusCodes.h"
#include "wml_elm_attr.h"


/* ------------------------------------------------------------------------- *
   private constants
 * ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- *
   private static methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
static
NW_Bool NW_HED_DomHelper_ContainsPlainTextVariablesAndEntities (const NW_HED_DomHelper_t* domHelper,
                                                                const NW_String_t* value, 
                                                                NW_Uint32 encoding)
{
  NW_Uint16 charCount;
  NW_Uint16 i;
  NW_Byte* buff;
  NW_Int32 skip;
 

  NW_ASSERT (domHelper != NULL);
  NW_ASSERT (value != NULL);

  charCount = NW_String_getCharCount ((NW_String_t*) value, encoding);
  buff = value->storage;

  for (i = 0; i < charCount; i++) {
    NW_Ucs2 ucs2Char;

    skip = NW_String_readChar (buff, &ucs2Char, encoding);

    /* found a variable */
    if ((domHelper->context != NULL) && (ucs2Char == WAE_ASCII_DOLLAR)) {
      return NW_TRUE;
    }

    /* Found entity begin. There is support for loose entity i.e. entity 
     * not ending with (;). So, if this entity begin symbol is found then
     * let NW_HED_DomHelper_GetText_Resolve(...) to decide about the text string.
     * */

    if ((domHelper->entitySet != NULL) && (ucs2Char == WAE_URL_AMP_CHAR)) 
    {
      return NW_TRUE;
    }

    buff += skip;
  }

  return NW_FALSE;
}

/* ------------------------------------------------------------------------- */
static
NW_Int32 IsValidDigit (const NW_Ucs2 ch, 
                       NW_Int32 base) 
{
  switch (base) {
    case 10: 
      return NW_Str_Isdigit (ch);

    case 16:
      return NW_Str_Isxdigit (ch);

    default:
      return 0;
  }
}

/* ------------------------------------------------------------------------- */
static
NW_Bool GetNumEntityChar (const NW_Ucs2* instring, 
                          NW_Ucs2 *retchar, 
                          NW_Int32 base) 
{
  NW_Int32 result = 0;
  NW_Int32 prevResult;
  NW_Uint32 currDigit = 0;
  
  if (*instring == 0) {
    return NW_FALSE;
  }

  while (*instring != 0) {
    if (!(IsValidDigit (*instring, base)))
      return NW_FALSE;
  
    if (NW_Str_Isdigit (*instring)) {
      currDigit = *instring - WAE_ASCII_0;
      instring++;
    } 
    else {
      currDigit = NW_Str_ToUpper (*instring) - WAE_ASCII_UPPER_A + 10;
      instring++;
    }

    prevResult = result;
    result = result * base + currDigit;
    if (result < prevResult) {
      return NW_FALSE;
    }
  }
  if (result > 0xffff || result < 0x0) 
    return NW_FALSE;

  /* TODO: validate the entity number */
  *retchar = (NW_Ucs2) result;
  return NW_TRUE;
}


// Full support for MS Windows extensions to Latin-1.
// Technically these extensions should only be activated for pages
// marked "windows-1252" or "cp1252", but
// in the standard Microsoft way, these extensions infect hundreds of thousands
// of web pages.  Note that people with non-latin-1 Microsoft extensions
// are SOL.
//
// See: http://www.microsoft.com/globaldev/reference/WinCP.asp
//      http://www.bbsinc.com/iso8859.html
//      http://www.obviously.com/
//
// There may be better equivalents

// We only need this for entities. For non-entity text, we handle this in the text encoding.

static const NW_Ucs2 wml_windowsLatin1Extension[32] = {
    0x20AC, 0x0081, 0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021, // 80-87
    0x02C6, 0x2030, 0x0160, 0x2039, 0x0152, 0x008D, 0x017D, 0x008F, // 88-8F
    0x0090, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014, // 90-97
    0x02DC, 0x2122, 0x0161, 0x203A, 0x0153, 0x009D, 0x017E, 0x0178  // 98-9F
};

static NW_Ucs2 wml_fixUpChar(NW_Ucs2 c)
{
    if ((c & ~0x1F) != 0x0080)
        return c;
    return wml_windowsLatin1Extension[c - 0x80];
}


/* ------------------------------------------------------------------------- */
static 
TBrowserStatusCode ResolveEntity (const NW_HED_EntitySet_t* entitySet, 
                           NW_String_t* entityName, 
                           NW_Ucs2* entityChar)
{
  NW_Ucs2* rawEntity = NULL;

  NW_TRY (status) {
    const NW_Ucs2 EREF_HEXSIGN = 'x';
  
    /* get the raw entity */
    rawEntity = NW_Str_New (entityName->length);
    NW_THROW_OOM_ON_NULL (rawEntity, status);
    NW_Str_Strncpy (rawEntity, (NW_Ucs2*) entityName->storage, entityName->length);
    *(rawEntity + entityName->length)  = 0;

    /* check and validate for hex numeric char entity */
    if ((*(rawEntity) == WAE_URL_FRAG_CHAR) &&
        (*(rawEntity + 1) == EREF_HEXSIGN) &&
        (GetNumEntityChar(rawEntity + 2, entityChar, 16) != 0)) {
      NW_THROW_SUCCESS (status);
    }
  
    /* check and validate for dec numeric char entity */
    if((*rawEntity == WAE_URL_FRAG_CHAR) &&
       (GetNumEntityChar (rawEntity+1, entityChar, 10) != 0)) {
      NW_THROW_SUCCESS (status);
    }
  
    /* check if it is a named char entity */
    if ((*entityChar = NW_HED_EntitySet_GetEntityChar (entitySet, rawEntity)) != 0) { 
      NW_THROW_SUCCESS (status);
    }
  }

  NW_CATCH (status) { /* empty */ }

  NW_FINALLY {
    NW_Str_Delete (rawEntity);
    
    /* Map Windows Latin1 extensions to their Unicode equivalents */
    *entityChar = wml_fixUpChar(*entityChar);
    
    return status;
  } NW_END_TRY
}


/* ------------------------------------------------------------------------- */
static
NW_Ucs2* NW_HED_DomHelper_ResolveVariable (const NW_HED_Context_t* context,
                                           NW_Uint16 variableType,
                                           const NW_Ucs2* varName)
{
  NW_String_t vName;
  const NW_String_t* varValue;
  NW_Ucs2* vValue;

  /* get the value of the variable */
  if (StatusCodeConvert(NW_String_initialize (&vName, (void *) varName, HTTP_iso_10646_ucs_2)) != KBrsrSuccess) {
    return NULL;
  }

  varValue = NW_HED_Context_GetVariable(context, &vName);
  if (varValue == NULL) {
    return NULL;
  }

  switch (variableType) {
    /* escape the variable's value */
    case NW_WBXML_EXT_I_0:
    case NW_WBXML_EXT_T_0:
      vValue = NW_Str_New (NW_Url_EscapeStringLen ((NW_Ucs2 *) varValue->storage));
      if (vValue) {
        (void) NW_Url_EscapeString ((NW_Ucs2 *) varValue->storage, vValue);
      }
      break;

    /* unescape the variable's value */
    case NW_WBXML_EXT_I_1:
    case NW_WBXML_EXT_T_1:
      vValue = NW_Str_New (NW_Url_UnEscapeStringLen ((NW_Ucs2 *) varValue->storage));
      if (vValue) {
        (void) NW_Url_UnEscapeString ((NW_Ucs2 *) varValue->storage, vValue);
      }
      break;

    /* just copy the variable's value */
    default:
      NW_String_stringToUCS2Char (varValue, HTTP_iso_10646_ucs_2, &vValue);
      break;

  }

  return vValue;
}

/* ------------------------------------------------------------------------- */
static
NW_Bool
IsEndofVar (NW_Ucs2 varChar, 
            NW_Ucs2 endVar)
{
  if (NW_Str_Isalnum(varChar) || (varChar == WAE_ASCII_UNDERSCORE) ||
     ((varChar == WAE_ASCII_COLON) && (endVar == WAE_ASCII_RIGHT_BRACE))) {
    return NW_FALSE;
  }

  return NW_TRUE;
}

/* ------------------------------------------------------------------------- */
static
NW_Uint16
getVariableType (NW_Ucs2* conv)
{
  static const NW_Ucs2 DEPRECATED_CONV_NOESC[] = { 'n', 'o', 'e', 's', 'c', 0};
  static const NW_Ucs2 DEPRECATED_CONV_ESCAPE[] = { 'e', 's', 'c', 'a', 'p', 'e', 0};
  static const NW_Ucs2 DEPRECATED_CONV_UNESC[] = { 'u', 'n', 'e', 's', 'c', 0};
  NW_Ucs2 *pChar = conv;

  if(conv == NULL) {
    return 0;
  }

  if (NW_Str_Strnicmp (pChar, (NW_Ucs2 *) DEPRECATED_CONV_NOESC, 1) == 0) {
    if ((*(pChar + 1) != WAE_ASCII_NULL) &&
       (NW_Str_Strnicmp (pChar, (NW_Ucs2 *) DEPRECATED_CONV_NOESC, 5) != 0)) {
      return 0;
    }
    return NW_WBXML_EXT_I_2;
  }
  else if (NW_Str_Strnicmp (pChar, (NW_Ucs2 *) DEPRECATED_CONV_ESCAPE, 1) == 0)
  {
    if ((*(pChar + 1) != WAE_ASCII_NULL) &&
       (NW_Str_Strnicmp (pChar, (NW_Ucs2 *) DEPRECATED_CONV_ESCAPE, 6) != 0)) {
      return 0;
    }
    return NW_WBXML_EXT_I_0;
  }
  else if (NW_Str_Strnicmp (pChar, (NW_Ucs2 *) DEPRECATED_CONV_UNESC, 1) == 0)
  {
    if ((*(pChar + 1) != WAE_ASCII_NULL) &&
       (NW_Str_Strnicmp (pChar, (NW_Ucs2 *) DEPRECATED_CONV_UNESC, 5) != 0)) {
      return 0;
    }
    return NW_WBXML_EXT_I_1;
  }
  return 0;
}

/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode getVarValue (const NW_HED_Context_t* context,
                         NW_Ucs2* varName, 
                         NW_Ucs2** varValue,
                         NW_Ucs2** varEnd,
                         NW_Uint16 variableType
                         )
{
  NW_Ucs2 varRefEnd = ' ';
  NW_Ucs2* nameStart; 
  NW_Ucs2* tmpName = NULL;
  NW_Uint32 nameLen;
  TBrowserStatusCode status = KBrsrSuccess;

  /* skip $ char */
  nameStart = ++varName;

  /* is $$ */
  if (*varName == WAE_ASCII_DOLLAR) {
    varName++; /* skip second dollar */
    NW_THROWIF_NULL (*varValue = NW_Str_New(1));
    NW_Str_Strncpy (*varValue, nameStart , 1);
    *(*varValue + 1)  = 0;
    *varEnd = varName;

    return status;
  }
  
  /* is "$(" Variable ")" type variable */
  if (*varName == WAE_ASCII_LEFT_BRACE) {
    varRefEnd = WAE_ASCII_RIGHT_BRACE;
    nameStart = ++varName; /* skip left brace */

    while (!IsEndofVar (*varName, varRefEnd)) {
      varName++;
    }

    if (*varName == WAE_ASCII_RIGHT_BRACE) {
      NW_Ucs2* convStart = NULL;
      NW_Ucs2 convChar = 0;
      NW_Uint16 varType = 0;
      nameLen = varName - nameStart;

      NW_THROWIF_NULL (tmpName = NW_Str_New (nameLen));
      NW_Str_Strncpy (tmpName, nameStart, nameLen);
      *(tmpName + nameLen)  = 0;

      varName++; /* skip ) char */
      /* Support for conv and deprecated-conv */
      convStart = NW_Str_Strchr (tmpName, WAE_ASCII_COLON);
      if(convStart) {
        convChar = *convStart;
        *convStart = 0;
        convStart++;

        varType = getVariableType (convStart);
        if(varType) {
          *varValue = NW_HED_DomHelper_ResolveVariable(context, varType, tmpName);
        }

        /* unexpected conv char, copy as is */
        else {
          nameLen = varName - nameStart + 2; /* To include the "$(" chars */
          NW_THROWIF_NULL (*varValue = NW_Str_New (nameLen));
          NW_Str_Strncpy (*varValue, (nameStart - 2), nameLen);
          *(*varValue + nameLen)  = 0;
        }

        convStart--;
        *convStart = convChar;
      }
      else {
          *varValue = NW_HED_DomHelper_ResolveVariable (context, variableType, tmpName);
      }

      NW_Str_Delete(tmpName);
      *varEnd = varName;
      return status;
    }

    /* Found "$(" but no corresponding ")", so copy as is */
    nameLen = varName - nameStart + 2; /* To include the "$(" chars */
    NW_THROWIF_NULL (*varValue = NW_Str_New (nameLen));
    NW_Str_Strncpy (*varValue, (nameStart - 2), nameLen);
    *(*varValue + nameLen)  = 0;
    *varEnd = varName;
    return status;
  }

  /* is "$" Variable " " type variable */
  if(NW_Str_Isalpha (*varName) || (*varName == WAE_ASCII_UNDERSCORE)) {
    while (!IsEndofVar (*varName, varRefEnd)) {
      varName++;
    }

    nameLen = varName - nameStart;

    NW_THROWIF_NULL (tmpName = NW_Str_New (nameLen));
    NW_Str_Strncpy (tmpName, nameStart, nameLen);
    *(tmpName + nameLen)  = 0;
    *varValue = NW_HED_DomHelper_ResolveVariable (context, NW_WBXML_EXT_I_2, tmpName);
    NW_Str_Delete (tmpName);
    *varEnd = varName;
    return status;
  }

  /* If we reach here that means its just a $, copy as is */
  NW_THROWIF_NULL (*varValue = NW_Str_New (1));
  NW_Str_Strncpy (*varValue, varName - 1 , 1);
  *(*varValue + 1) = 0;
  *varEnd = varName;
  return status;

NW_CATCH_ERROR
  return KBrsrOutOfMemory;
}

/* ------------------------------------------------------------------------- */
static 
TBrowserStatusCode NW_HED_DomHelper_ResolveVariables (const NW_HED_Context_t* context,
                                               const NW_Ucs2* origText,                                              
                                                      NW_String_t* destString,
                                                      NW_Uint16 variableType
                                                      )
{
  NW_Ucs2* sourceWalker = NULL;
  NW_Ucs2* varStart = NULL;
  NW_Ucs2* varEnd = NULL;
  NW_Ucs2* varValue = NULL;
  NW_Bool done = NW_FALSE;
  NW_String_t tmpString;
  TBrowserStatusCode status = KBrsrDomNoStringReturned;

  /* Initialising the dest to NULL */
  destString->length = 0;
  destString->storage = NULL;
  tmpString.length = 0;
  tmpString.storage = NULL;

  /* Initialise sourceWalker to origText */
  sourceWalker = (NW_Ucs2*) origText;

  /* Itarate through sourceWalker to substitute all variables */
  while (!done) {
    /* Find a variable */
    varStart = NW_Str_Strchr (sourceWalker, WAE_ASCII_DOLLAR);

    /* Resolve variable */
    if(varStart) {
        NW_THROWIF_ERROR (status = getVarValue (context, varStart, &varValue, &varEnd, variableType));
    }

    /* append rest to the dest string, if no more variables */
    if(varStart == NULL) {
      if(destString != NULL) {
        tmpString.length =  NW_Str_Strsize (sourceWalker);
        tmpString.storage = (NW_Byte*) sourceWalker;
        NW_THROWIF_ERROR (status = NW_HED_DomHelper_CatString (destString, &tmpString, HTTP_iso_10646_ucs_2));
      }

      done = NW_TRUE;
      NW_Str_Delete (varValue);
      continue;
    }

    /* append the text before variable and the variable Value to the destSting */
    if(varStart != sourceWalker) {
      tmpString.length =  (varStart - sourceWalker) * sizeof (NW_Ucs2);
      tmpString.storage = (NW_Byte*) sourceWalker;
      NW_THROWIF_ERROR (status = NW_HED_DomHelper_CatString (destString, &tmpString, HTTP_iso_10646_ucs_2));
    }

    if(varValue && NW_Str_Strlen(varValue)) {
      tmpString.length =  NW_Str_Strlen (varValue) * sizeof (NW_Ucs2);
      tmpString.storage = (NW_Byte*) varValue;
      NW_THROWIF_ERROR (status = NW_HED_DomHelper_CatString (destString, &tmpString, HTTP_iso_10646_ucs_2));
      NW_Str_Delete (varValue);
      varValue = NULL;
    }

    /* set the sourceWlaker to the next iteration */
    sourceWalker = varEnd;
  }

  return status;

NW_CATCH_ERROR
  NW_Str_Delete (varValue);
  NW_String_deleteStorage (destString);
  destString->length = 0;
  destString->storage = NULL;
  if (status != KBrsrOutOfMemory) {
    status = KBrsrDomNoStringReturned;
  }

  return status;
}

/* ------------------------------------------------------------------------- */
static 
TBrowserStatusCode NW_HED_DomHelper_ResolveVariablesAndEntities (
                                           const NW_String_t* origText,
                                           const NW_HED_EntitySet_t* entitySet,
                                           const NW_HED_Context_t* context,
                                           NW_Uint32 encoding,
                                           NW_String_t *resolvedText,
                                           NW_Uint16 variableType)
{
  TBrowserStatusCode status;
  NW_Ucs2* ucs2String = NULL;
  NW_String_t tempString;

  NW_ASSERT (origText != NULL);
  NW_ASSERT (resolvedText != NULL);

  NW_String_initialize (resolvedText, NULL, 0);
  NW_String_initialize (&tempString, NULL, 0);

  /* convert the value to a ucs2-NW_String_t */
  NW_THROWIF_ERROR (status = StatusCodeConvert(NW_String_stringToUCS2Char (origText, encoding, &ucs2String)));

  /* perform enitity substitution */
  if (entitySet != NULL) {
    status = NW_HED_DomHelper_ResolveEntities (entitySet, ucs2String, &tempString);
    
    /* entities were found so update ucs2String with the resolved string */
    if (status == KBrsrSuccess) {
      NW_Str_Delete (ucs2String);
      ucs2String = NULL;

      NW_THROWIF_ERROR (status = StatusCodeConvert(NW_String_stringToUCS2Char (&tempString, HTTP_iso_10646_ucs_2, &ucs2String)));
      NW_String_deleteStorage (&tempString);
    }

    /* 
    ** if status equals KBrsrDomNoStringReturned no entities were found. In 
    ** which case ucs2String doesn't need to be updated.  All other errors cause
    ** an exception to be thrown.
    */
    else if (status != KBrsrDomNoStringReturned) {
      NW_THROW_ERROR();
    }
  }

  /* perform variable substitution */
  if (context != NULL) {
      status = NW_HED_DomHelper_ResolveVariables (context, ucs2String, &tempString, variableType);

    /* variables were found so update ucs2String with the resolved string */
    if (status == KBrsrSuccess) {
      NW_Str_Delete (ucs2String);
      ucs2String = NULL;

      NW_THROWIF_ERROR (status = StatusCodeConvert(NW_String_stringToUCS2Char (&tempString, encoding, &ucs2String)));
      NW_String_deleteStorage (&tempString);
    }

    /* 
    ** if status equals KBrsrDomNoStringReturned no variables were found. In 
    ** which case ucs2String doesn't need to be updated.  All other errors cause
    ** an exception to be thrown.
    */
    else if (status != KBrsrDomNoStringReturned) {
      NW_THROW_ERROR();
    }
  }
  
  /* update resolvedText */
  NW_String_initialize (resolvedText, ucs2String, HTTP_iso_10646_ucs_2);
  NW_String_setUserOwnsStorage (resolvedText);

  return KBrsrSuccess;

NW_CATCH_ERROR
  NW_String_deleteStorage (&tempString);
  NW_Str_Delete (ucs2String);

  return status;
}


/* ------------------------------------------------------------------------- */
/* 
** Extracts the text from the element. It is able to properly resolve variables
** & entities and handle multiple text items.  Upon success value holds the 
** result encoded as ucs2.
*/
static
TBrowserStatusCode NW_HED_DomHelper_GetText_Resolve (const NW_HED_DomHelper_t* domHelper,
                                              const NW_DOM_Node_t* element,
                                              NW_String_t* value)
{
  TBrowserStatusCode status;
  NW_DOM_TextItemIterator_t iterator;
  NW_DOM_TextItem_t item;
  NW_String_t nativeText;
  NW_Uint32 nativeEncoding;
  NW_Ucs2* ucs2String = NULL;

  NW_ASSERT (element != NULL);
  NW_ASSERT (value != NULL);

  nativeText.storage = NULL;
  nativeText.length = 0;

  /* init the result string */
  value->storage = NULL;
  value->length = 0;
  NW_String_setUserOwnsStorage (value);

  if (NW_DOM_Node_getNodeType ((NW_DOM_Node_t*) element) != NW_DOM_TEXT_NODE){
    return KBrsrFailure;
  }

  nativeEncoding = NW_HED_DomHelper_GetEncoding (element);

  /* init the text item iter */
  NW_THROWIF_ERROR (status = StatusCodeConvert(NW_DOM_TextNode_getTextItemIterator ((NW_DOM_Node_t*) element, &iterator)));

  /* build the text by appendding the testItems */ 
  NW_Mem_memset (&item, 0x00, sizeof (NW_DOM_TextItem_t));
  while (NW_DOM_TextItemIterator_getNextTextItem (&iterator, &item) == NW_STAT_WBXML_ITERATE_MORE) {
    NW_Uint32 encoding = nativeEncoding;

    /* entities should always be extracted with an ucs2 encoding */
    if (NW_DOM_TextItem_getType (&item) == NW_DOM_TEXT_ITEM_ENTITY) {
      encoding = HTTP_iso_10646_ucs_2;
    }

    /* don't include opaque data */
    if (NW_DOM_TextItem_getType (&item) == NW_DOM_TEXT_ITEM_OPAQUE) {
      continue;
    }

    /* get items's value as a string using the document's encoding */
    NW_THROWIF_ERROR (status = StatusCodeConvert(NW_DOM_TextItem_toString (&item, &nativeText, encoding))); 

    /* convert the value to a ucs2 string or copy it if it is already ucs2 */
    NW_THROWIF_ERROR (status = StatusCodeConvert(NW_String_stringToUCS2Char (&nativeText, encoding, &ucs2String)));
    NW_String_deleteStorage (&nativeText);

    /* if this item is a wbxml variable get the value -- we will append it instead */
    if(NW_DOM_TextItem_getType (&item) == NW_DOM_TEXT_ITEM_EXTENSION) {
      NW_Ucs2* varName;
      NW_String_t tmpStr;
      NW_Uint16 token;
        
      token = NW_DOM_TextItem_getExtension (&item, &tmpStr);

      varName = ucs2String;
      ucs2String = NW_HED_DomHelper_ResolveVariable (domHelper->context, (NW_Uint16) (token & 0xFF), varName);
      
      NW_Str_Delete (varName);
    }

    /* append the resolved string */
    NW_THROWIF_ERROR (status = NW_HED_DomHelper_CatUcs2String (value, ucs2String));

    NW_Str_Delete (ucs2String);
    ucs2String = NULL;

    NW_Mem_memset (&item, 0x00, sizeof (NW_DOM_TextItem_t));
  }

  /* if after building the text it is null just throw an "exception" so we can clean up below */
  if (value->storage == NULL) {
    status = KBrsrFailure;
    NW_THROW_ERROR ();
  }

  /* perform plain-text variable and entity substitution as needed */
  if (domHelper->resolvePlainTextVariablesAndEntities == NW_TRUE) {
    NW_String_t newValue;

    NW_THROWIF_ERROR (status = NW_HED_DomHelper_ResolveVariablesAndEntities (value, domHelper->entitySet, 
        domHelper->context, HTTP_iso_10646_ucs_2, &newValue, NW_WBXML_EXT_I_2));

    NW_String_deleteStorage (value);
    NW_String_initialize (value, newValue.storage, HTTP_iso_10646_ucs_2);
    NW_String_setUserOwnsStorage (value);
  }

  /* return the result */
  return KBrsrSuccess;

NW_CATCH_ERROR
  NW_String_deleteStorage (&nativeText);
  NW_Str_Delete (ucs2String);
  NW_String_deleteStorage (value);

  return status;
}

/* ------------------------------------------------------------------------- */
/* 
** Extracts the text from the element without copying the original data from
** the DOM.  This function returns KBrsrUnexpectedError if it isn't possible
** to extract the text without copying it -- in which case the caller should call 
** NW_HED_DomHelper_GetText_Resolve.
*/
static
TBrowserStatusCode NW_HED_DomHelper_GetText_NoResolve (const NW_HED_DomHelper_t* domHelper,
                                                const NW_DOM_Node_t* element,
                                                NW_String_t* value,
                                                NW_Uint32* encoding)
{
  NW_DOM_TextItemIterator_t iterator;
  NW_DOM_TextItem_t item;
  NW_Int32 itemCount = 0;

  NW_ASSERT (domHelper != NULL);
  NW_ASSERT (element != NULL);
  NW_ASSERT (value != NULL);
  NW_ASSERT (encoding != NULL);

  /* init the result string */
  NW_String_initialize (value, NULL, 0);

  NW_TRY (status) {
    if (NW_DOM_Node_getNodeType ((NW_DOM_Node_t*) element) != NW_DOM_TEXT_NODE){
      NW_THROW_UNEXPECTED (status);
    }

    /* set the encoding */
    *encoding = NW_HED_DomHelper_GetEncoding (element);

    /* init the text item iter */
    status = StatusCodeConvert(NW_DOM_TextNode_getTextItemIterator ((NW_DOM_Node_t*) element, &iterator));
    NW_THROW_ON_ERROR (status);

    /* extract the single text item -- reject complex elements */
    NW_Mem_memset (&item, 0x00, sizeof (NW_DOM_TextItem_t));
    while (NW_DOM_TextItemIterator_getNextTextItem (&iterator, &item) == NW_STAT_WBXML_ITERATE_MORE) {
      NW_DOM_TextItemType_t type;

      /* get the item's type */
      type = NW_DOM_TextItem_getType (&item);

      /* just ignore opaque data */
      if (type == NW_DOM_TEXT_ITEM_OPAQUE) {
        continue;
      }

      /* 
      ** reject the text element if it contains anything other than a string 
      ** item (ignoring opaque).  The caller must use NW_HED_DomHelper_GetText_Resolve instead.
      */
      if (type != NW_DOM_TEXT_ITEM_STRING) {
        NW_THROW_UNEXPECTED (status);
      }

      /* reject the text element if it contains more than one text item */
      if (itemCount > 0) {
        NW_THROW_UNEXPECTED (status);
      }
      else {
        itemCount++;
      }

      /* get items's value as a string using the document's encoding */
      status = StatusCodeConvert(NW_DOM_TextItem_toString (&item, value, *encoding));
      NW_THROW_ON_ERROR (status);

      NW_Mem_memset (&item, 0x00, sizeof (NW_DOM_TextItem_t));
    }

    /* if after building the text it is null just throw an "exception" */
    if (value->storage == NULL) {
      NW_THROW_UNEXPECTED (status);
    }

    /* 
    ** if the text element contains plain-text entities or variables the caller must use 
    ** NW_HED_DomHelper_GetText_Resolve instead.
    */
    if (domHelper->resolvePlainTextVariablesAndEntities == NW_TRUE) {
      if (NW_HED_DomHelper_ContainsPlainTextVariablesAndEntities (domHelper, 
          value, *encoding) == NW_TRUE) {
        NW_THROW_UNEXPECTED (status);
      }
    }
  }

  NW_CATCH (status) {
  }

  NW_FINALLY {
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode NW_HED_DomHelper_GetAttributeValueHelper (const NW_DOM_ElementNode_t* element, 
                                                      const NW_String_t* attributeName, 
                                                      const NW_HED_Context_t* context, 
                                                      NW_String_t* value)
{
  TBrowserStatusCode status;
  NW_String_t nativeText;
  NW_Uint32 nativeEncoding;
  NW_Ucs2* ucs2String = NULL;
  NW_DOM_AttrVal_t av;
  NW_DOM_AttributeHandle_t attributeHandle;

  nativeEncoding = NW_HED_DomHelper_GetEncoding (element);

  nativeText.storage = NULL;
  nativeText.length = 0;

  value->storage = NULL;
  value->length = 0;

  /* get the attribute's handle */
  NW_THROWIF_ERROR (status = StatusCodeConvert(NW_DOM_ElementNode_getAttributeHandleByName ((NW_DOM_ElementNode_t*) element,
      (NW_String_t*) attributeName, &attributeHandle)));

  /* 
  If this attribute's name includes the an attribute value prefix
  (e.g. name is something like "href=http://"), then the attribute's
  value prefix must be added.
  */
  status = StatusCodeConvert(NW_DOM_AttributeHandle_getValuePrefix (&attributeHandle, &nativeText));
  if (status == KBrsrSuccess) {
    value->storage = NULL;

    NW_THROWIF_ERROR (status = NW_HED_DomHelper_CatString (value, &nativeText, nativeEncoding));
    NW_String_deleteStorage(&nativeText);
  }
  else if (status != KBrsrDomNoValuePrefix) {
    status = KBrsrNotFound;
    NW_THROW_ERROR();
  }

  /* build the rest of the attribute's value */
  while (NW_DOM_AttributeHandle_getNextVal (&attributeHandle, &av)
         == NW_STAT_WBXML_ITERATE_MORE) {
    NW_Uint32 encoding = nativeEncoding;

    /* entities should always be extracted with an ucs2 encoding */
    if (NW_DOM_AttrVal_getType(&av) == NW_DOM_ATTR_VAL_ENTITY) {
      encoding = HTTP_iso_10646_ucs_2;
    }

    /* get the next value as a string using the document's encoding */
    NW_THROWIF_ERROR (status = StatusCodeConvert(NW_DOM_AttrVal_toString (&av, &nativeText, encoding))); 

    /* convert the value to a newly malloc'ed ucs2 string*/
    NW_THROWIF_ERROR (status = StatusCodeConvert(NW_String_stringToUCS2Char (&nativeText, encoding, &ucs2String)));
    NW_String_deleteStorage (&nativeText);

    /* if this item is a wbxml variable get the value -- we will append it instead */
    if(NW_DOM_AttrVal_getType (&av) == NW_DOM_ATTR_VAL_EXTENSION) {
      NW_Ucs2* varName;
      NW_String_t tmpStr;
      NW_Uint16 token; 
        
      token = NW_DOM_AttrVal_getExtension (&av, &tmpStr);

      varName = ucs2String;
      ucs2String = NW_HED_DomHelper_ResolveVariable (context, (NW_Uint16) (token & 0xFF), varName);
      
      NW_Str_Delete (varName);
    }

    /* append the resolved string */
    NW_THROWIF_ERROR (status = NW_HED_DomHelper_CatUcs2String (value, ucs2String));

    NW_Str_Delete (ucs2String);
    ucs2String = NULL;
  }

  if (value->storage == NULL)
    status = KBrsrDomNoStringReturned;
  else
    status = KBrsrSuccess;

  return status;

NW_CATCH_ERROR
  NW_String_deleteStorage (value);
  NW_String_deleteStorage (&nativeText);

  if (status != KBrsrOutOfMemory) {
    status = KBrsrDomNoStringReturned;
  }

  return status;
}

/* ------------------------------------------------------------------------- *
   final functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- 
  return values:  normal completion:
                  KBrsrSuccess if attribute found
                  KBrsrNotFound if attribute not found

                  abnormal completion:
                  other code if critical error
   ------------------------------------------------------------------------- */
TBrowserStatusCode NW_HED_DomHelper_ResolveEntities (const NW_HED_EntitySet_t* entitySet,
                                                     const NW_Ucs2* origText, 
                                                     NW_String_t* destString)
{
  NW_Ucs2* sourceWalker = NULL;
  NW_Ucs2* entStart = NULL;
  NW_Ucs2* entEnd = NULL;
  NW_String_t tmpString;
  NW_String_t entityName;
  NW_Ucs2 entityChar;
  NW_Bool done = NW_FALSE;
  NW_Bool looseEntity = NW_FALSE;
  NW_Bool resolve = NW_TRUE;
  TBrowserStatusCode status = KBrsrDomNoStringReturned;

  if (!origText) 
    return status;

  /* Initialise  destString to NULL */
  destString->length = 0;
  destString->storage = NULL;

  /* Initialise sourceWalker to origText */
  sourceWalker = (NW_Ucs2*) origText;

  /* Itarate through sourceWalker to substitute all entities. Let us allow the loose
   * entities. The loose entity means if semicolon (;) is missing at the end of entity.
   * But, the loose entity is still verified that it formed with correct syntax and is
   * found in the "entitySet" of xHTML.
   */

  while (!done) {
    /* initialise entityChar */
    entityChar = 0;

    /* Find an entity */
    entStart = NW_Str_Strchr (sourceWalker, WAE_URL_AMP_CHAR);
    if (entStart != NULL) {
      entEnd = NW_Str_Strchr (entStart, WAE_URL_PARAM_CHAR);
    }

    /* Append the rest to destSting, if there are no more entities */

    if (entStart == NULL ) 
    {
      if (destString->length) 
      {
        tmpString.length =  NW_Str_Strsize (sourceWalker);
        tmpString.storage = (NW_Byte*) sourceWalker;
        NW_THROWIF_ERROR (status = NW_HED_DomHelper_CatString (destString, &tmpString, HTTP_iso_10646_ucs_2));
      }

      done = NW_TRUE;
      continue;
    }

    if(entEnd != NULL)
    {
     /* Validate the correct syntax entity */
      NW_ASSERT(entEnd > entStart);
      entityName.length =  (entEnd - entStart - 1); /* just to hold characters between & and ; */
      entityName.storage = (NW_Byte*) (entStart + 1);  /* starting the char after & */ 
    }
    else
    {
     /* Check for loose syntax */
     
      looseEntity = NW_FALSE;
      
      for(entEnd=entStart+1;  NW_Str_Isalnum(*entEnd) ;entEnd++)
          {
          /* do nothing */
          }
      // in the case: src="http://a.com/leftwindow?OpenDocument&lang=en"
      // although &lang is an entity in entityset defs; it shouldn't be recognized as entity here
      if (entEnd && (entEnd[0] > 0x20))
        {
        resolve = NW_FALSE;
        }

      /* Check some thing is moved or not.  */

      if(entEnd > (entStart + 2) )
      {
       entityName.length =  (NW_Uint32) (entEnd - entStart) - 1; /* just to hold characters between & and non alphanumeric */
       
       entityName.storage = (NW_Byte*) (entStart + 1);  /* starting the char after & */ 
       looseEntity = NW_TRUE;
      }
      else
      {
       //Done
        if (destString->length) 
        {
         tmpString.length =  NW_Str_Strsize (sourceWalker);
         tmpString.storage = (NW_Byte*) sourceWalker;
         NW_THROWIF_ERROR (status = NW_HED_DomHelper_CatString (destString, &tmpString, HTTP_iso_10646_ucs_2));
        }

       done = NW_TRUE;
       continue;
      }
    }//end else
    


    if (resolve)
      {
    NW_THROWIF_ERROR (status = ResolveEntity (entitySet, &entityName, &entityChar));
      }

    /* append the text before entity and the entity value to the destSting */
    if(entStart != sourceWalker) {
      tmpString.length =  (entStart - sourceWalker) * sizeof (NW_Ucs2);
      tmpString.storage = (NW_Byte*) sourceWalker;
      NW_THROWIF_ERROR (status = NW_HED_DomHelper_CatString (destString, &tmpString, HTTP_iso_10646_ucs_2));
    }

    /* valid entity, copy entityChar, and set the sourceWalker to entEnd + 1 */
    if(entityChar) {
      tmpString.length =  sizeof (NW_Ucs2);
      tmpString.storage = (NW_Byte*) &entityChar;
      NW_THROWIF_ERROR (status = NW_HED_DomHelper_CatString (destString, &tmpString, HTTP_iso_10646_ucs_2));

      /* set the sourceWlaker to the next iteration */
      if(looseEntity == NW_FALSE)
      {
       sourceWalker = ++entEnd; /* skip ; char */
      }
      else
      {
       sourceWalker = entEnd; //Already at next character so don't skip
      }

    }

    /* invalid entity, copy &, and set the sourceWalker to entStart + 1 */
    else {
      tmpString.length =  sizeof (NW_Ucs2);
      tmpString.storage = (NW_Byte*) entStart;
      NW_THROWIF_ERROR (status = NW_HED_DomHelper_CatString (destString, &tmpString, HTTP_iso_10646_ucs_2));
      sourceWalker = ++entStart; /* skip & char of invalid entity */
    }
  }

  return status;

NW_CATCH_ERROR
  NW_String_deleteStorage (destString);
  if (status != KBrsrOutOfMemory) {
    status = KBrsrDomNoStringReturned;
  }

  return status;
}


/* ------------------------------------------------------------------------- 
  return values:  normal completion:
                  KBrsrSuccess if attribute found
                  KBrsrNotFound if attribute not found

                  abnormal completion:
                  other code if critical error
   ------------------------------------------------------------------------- */
TBrowserStatusCode NW_HED_DomHelper_GetAttributeValue (const NW_HED_DomHelper_t* domHelper,
                                                const NW_DOM_Node_t* element, 
                                                NW_Uint16 attribute, 
                                                NW_String_t *value)
{
  TBrowserStatusCode status;
  NW_String_t attrName;
  NW_DOM_NodeType_t nodeType;

  NW_ASSERT (element != NULL);
  NW_ASSERT (value != NULL);

  /* init the result string */
  value->storage = NULL;
  value->length = 0;
  NW_String_setUserOwnsStorage (value);

  attrName.storage = NULL;
  attrName.length = 0;

  nodeType = NW_DOM_Node_getNodeType ((NW_DOM_Node_t*) element);
  if ((nodeType != NW_DOM_PROCESSING_INSTRUCTION_NODE)
      && (nodeType != NW_DOM_ELEMENT_NODE)) {
    return KBrsrDomNodeTypeErr;
  }

  /* get the attribute as a string */
  NW_THROWIF_ERROR (status = NW_HED_DomHelper_GetAttributeName (element, attribute, &attrName));

  /* get the attribute value */
  status = NW_HED_DomHelper_GetAttributeValueHelper ((NW_DOM_ElementNode_t* ) element, &attrName, 
      domHelper->context, value);

  /* if after building the text it is null just throw an "exception" so we can clean up below */
  if (value->storage == NULL) {
    status = KBrsrNotFound;
    NW_THROW_ERROR ();
  }

  /* perform plain-text variable and entity substitution as needed */
  if (domHelper->resolvePlainTextVariablesAndEntities == NW_TRUE) {
    NW_String_t newValue;

    NW_Uint16 variableType = NW_WBXML_EXT_I_2;
    if (attribute == HREF_ATTR)
        {
        variableType = NW_WBXML_EXT_I_0;
        }
    NW_THROWIF_ERROR (status = NW_HED_DomHelper_ResolveVariablesAndEntities (value, domHelper->entitySet, 
        domHelper->context, HTTP_iso_10646_ucs_2, &newValue, variableType));

    NW_String_deleteStorage (value);
    NW_String_initialize (value, newValue.storage, HTTP_iso_10646_ucs_2);
    NW_String_setUserOwnsStorage (value);
  }

  /* clean up and return the result */
  NW_String_deleteStorage (&attrName);

  return KBrsrSuccess;

NW_CATCH_ERROR
  NW_String_deleteStorage (&attrName);
  NW_String_deleteStorage (value);

  return status;
}


TBrowserStatusCode NW_HED_DomHelper_GetHTMLAttributeValue (const NW_HED_DomHelper_t* domHelper,
                                                const NW_DOM_Node_t* element, 
                                                NW_Uint16 attributeToken, 
                                                NW_String_t *valueString)
{
  TBrowserStatusCode status;
  NW_DOM_NodeType_t nodeType;
  NW_DOM_AttributeHandle_t attributeHandle;
  NW_DOM_AttrVal_t av;
  NW_String_t str;
  NW_Uint32 encoding;

  NW_ASSERT (element != NULL);
  NW_ASSERT (valueString != NULL);

  /* init the result string */
  valueString->storage = NULL;
  valueString->length = 0;
  NW_String_setUserOwnsStorage (valueString);

  nodeType = NW_DOM_Node_getNodeType ((NW_DOM_Node_t*) element);
  if ((nodeType != NW_DOM_PROCESSING_INSTRUCTION_NODE)
      && (nodeType != NW_DOM_ELEMENT_NODE)) {
    return KBrsrDomNodeTypeErr;
  }

  status = StatusCodeConvert(NW_DOM_ElementNode_getAttributeHandleByToken((NW_DOM_Node_t*)element, attributeToken, &attributeHandle));
  if (status != KBrsrSuccess){
    return status;
  }
  encoding = NW_DOM_AttributeHandle_getEncoding(&attributeHandle);

  while (NW_DOM_AttributeHandle_getNextVal(&attributeHandle, &av) == NW_STAT_WBXML_ITERATE_MORE)
  {
    status = StatusCodeConvert(NW_DOM_AttrVal_toString(&av, &str, encoding)); 
    if (status != KBrsrSuccess)
      return KBrsrDomNoStringReturned;

    if ((valueString->length == 0) || (valueString->storage == NULL)){
      status = StatusCodeConvert(NW_String_deepCopy(valueString, &str));
    }
    else{
      status = StatusCodeConvert(NW_String_concatenate(valueString, &str, encoding));
    }
    NW_String_deleteStorage(&str);
    if (status == KBrsrOutOfMemory)
    {
      return KBrsrOutOfMemory;
    }
    if (status != KBrsrSuccess)
    {
      return KBrsrDomNoStringReturned;
    }
  }

  /* if after building the text it is null just throw an "exception" so we can clean up below */
/*
  if (valueString->storage == NULL) {
    status = KBrsrNotFound;
    NW_THROW_ERROR ();
  }
*/
  /* perform plain-text variable and entity substitution as needed */
  if (domHelper->resolvePlainTextVariablesAndEntities == NW_TRUE) {
    NW_String_t newValue;

    NW_THROWIF_ERROR (status = NW_HED_DomHelper_ResolveVariablesAndEntities (valueString, domHelper->entitySet, 
        domHelper->context, encoding, &newValue, NW_WBXML_EXT_I_2));
    NW_String_deleteStorage (valueString);
    NW_String_initialize (valueString, newValue.storage, HTTP_iso_10646_ucs_2);
    NW_String_setUserOwnsStorage (valueString);
  }

  return KBrsrSuccess;

NW_CATCH_ERROR
  NW_String_deleteStorage (valueString);

  return status;
}

/* ------------------------------------------------------------------------- 
  return values:  normal completion:
                  KBrsrSuccess if text found
                  KBrsrFailure if text == NULL

                  abnormal completion:
                  other code if critical error
   ------------------------------------------------------------------------- */
TBrowserStatusCode NW_HED_DomHelper_GetText (const NW_HED_DomHelper_t* domHelper,
                                             const NW_DOM_Node_t* element,
                                             NW_Bool forceUcs2Copy,
                                             NW_String_t* value,
                                             NW_Uint32* valueEncoding)
    {
    TBrowserStatusCode status;
    NW_String_t text;
    NW_Uint32 encoding = 0;
    
    NW_ASSERT (domHelper != NULL);
    NW_ASSERT (element != NULL);
    NW_ASSERT (value != NULL);
    
    /* init value */
    NW_String_initialize (&text, NULL, 0);
    
    /* 
    ** try to extracts the text from the element without copying the 
    ** original data from the DOM.  Because we must copy the text to resolve
    ** variables and entities this function returns with a failure if
    ** it contains them.
    */
    if (forceUcs2Copy == NW_FALSE) {
        status = NW_HED_DomHelper_GetText_NoResolve (domHelper, element,
            &text, &encoding);
        }
    else {
        /* set status to KBrsrUnexpectedError so the next block will execute */ 
        status = KBrsrUnexpectedError;
        }
    
        /* 
        ** if the above fails use the more sophisicated GetText_Resolve function
        ** to extract the text.  This function on success always returns a ucs2 
        ** encoded copy of the text.
    */
    if (status != KBrsrSuccess) {
        encoding = HTTP_iso_10646_ucs_2;
        status = NW_HED_DomHelper_GetText_Resolve (domHelper, element, &text);
        }
    
    if (status == KBrsrSuccess) 
        {
        if (valueEncoding != NULL) 
            {
            *valueEncoding = encoding;
            }
            
        // Force copy of the string
        if (NW_String_getUserOwnsStorage(&text))
            {
            NW_String_shallowCopy(value, &text);
            }
        else
            {
            status = NW_String_deepCopy(value, &text);
            }
        }
    
    return status;
    }

/* ------------------------------------------------------------------------- */
TBrowserStatusCode NW_HED_DomHelper_GetChildText (const NW_HED_DomHelper_t* domHelper,
                                           const NW_DOM_Node_t* element,
                                           NW_String_t* value)
{
  TBrowserStatusCode status = KBrsrOutOfMemory;
  NW_cXML_DOM_DOMVisitor_t* visitor = NULL;
  NW_DOM_Node_t* textNode = NULL;
  NW_String_t text;

  NW_ASSERT (domHelper != NULL);
  NW_ASSERT (element != NULL);
  NW_ASSERT (value != NULL);

  /* init the string */
  NW_String_initialize (value, NULL, 0);
  NW_String_setUserOwnsStorage (value);

  NW_String_initialize (&text, NULL, 0);

  /* create a dom visitor to extract text nodes */
  NW_THROWIF_NULL (visitor = NW_cXML_DOM_DOMVisitor_New ((NW_DOM_Node_t*) element));

  /* while there are text nodes */
  while ((textNode = NW_cXML_DOM_DOMVisitor_Next (visitor, NW_DOM_TEXT_NODE)) != NULL) {
    NW_Uint32 encoding;

    /* get the text */
    NW_THROWIF_ERROR (status = NW_HED_DomHelper_GetText (domHelper, textNode, 
        NW_TRUE, &text, &encoding))

    /* append the text */
    NW_THROWIF_ERROR (status = NW_HED_DomHelper_CatString (value, &text, encoding));

    /* discard the text */
    NW_String_deleteStorage (&text);
  }

  NW_Object_Delete (visitor);

  return KBrsrSuccess;

NW_CATCH_ERROR
  NW_String_deleteStorage (value);
  NW_String_deleteStorage (&text);
  NW_Object_Delete (visitor);

  return status;
}

/* ------------------------------------------------------------------------- */
NW_Bool NW_HED_DomHelper_AttributeEquals (const NW_HED_DomHelper_t* domHelper,
                                          const NW_DOM_Node_t* element, 
                                          NW_Uint16 attribute, 
                                          const NW_String_t* value) 
{
  NW_Bool match = NW_FALSE;
  NW_String_t attrValue;
 
  attrValue.storage = NULL;

  /* get the attribute as a ucs2 encoded string */
  if (NW_HED_DomHelper_GetAttributeValue (domHelper, element, attribute, &attrValue) == KBrsrSuccess) {
    match = (NW_Bool) NW_String_equals (value, &attrValue);
  }

  NW_String_deleteStorage (&attrValue);

  return match;
}

/* ------------------------------------------------------------------------- */
NW_Bool NW_HED_DomHelper_IsMatch (const NW_HED_DomHelper_t* domHelper,
                                  NW_DOM_Node_t* element,
                                  NW_Uint16 elementType,
                                  NW_Uint16 attribute,
                                  const NW_String_t* attributeMatchString)
{
  NW_Bool match = NW_TRUE;

  /* this is false if the types don't match */
  if (NW_HED_DomHelper_GetElementToken (element) != elementType) {
    match = NW_FALSE;
  }

  /* does the element have an attribute with the given value */
  else if (attributeMatchString != NULL) {
    match = NW_HED_DomHelper_AttributeEquals (domHelper, element, attribute, attributeMatchString);
  }

  return match;
}

/* ------------------------------------------------------------------------- */
NW_DOM_Node_t* NW_HED_DomHelper_FindElement (const NW_HED_DomHelper_t* domHelper,
                                             NW_DOM_Node_t* rootNode,
                                             NW_Int32 maxSearchDepth,
                                             NW_Uint16 elementType,
                                             NW_Uint16 attribute,
                                             const NW_String_t* attributeMatchString)
{
  NW_DOM_Node_t* childNode;
  NW_DOM_Node_t* descendantNode;

  /* does the root-node meet our criteria */
  if (NW_HED_DomHelper_IsMatch (domHelper, rootNode, elementType, attribute, attributeMatchString)) {
    return rootNode;
  }

  if (maxSearchDepth == 0) {
    return NULL;
  }

  /* does one of the children meet our criteria */
  for (childNode = NW_DOM_Node_getFirstChild (rootNode); childNode != NULL; 
      childNode = NW_DOM_Node_getNextSibling (childNode)) {

    if (NW_HED_DomHelper_IsMatch (domHelper, childNode, elementType, attribute, attributeMatchString)) {
      return childNode;
    }
  }

  /* does one of the children's ancestors meet our criteria */
  for (childNode = NW_DOM_Node_getFirstChild (rootNode); childNode != NULL; 
      childNode = NW_DOM_Node_getNextSibling (childNode)) {

    descendantNode = NW_HED_DomHelper_FindElement (domHelper, childNode, maxSearchDepth - 1,
        elementType, attribute, attributeMatchString);
    
    if (descendantNode != NULL) {
      return descendantNode;
    }
  }

  return NULL;
}

/* ------------------------------------------------------------------------- */
NW_Uint32 NW_HED_DomHelper_GetEncoding (const NW_DOM_Node_t* element)
{
  NW_DOM_DocumentNode_t *docNode;

  docNode = NW_DOM_Node_getOwnerDocument ((NW_DOM_Node_t*) element);

  return NW_DOM_DocumentNode_getCharacterEncoding (docNode);
}

/* ------------------------------------------------------------------------- */
NW_WBXML_Dictionary_t* NW_HED_DomHelper_GetDictionary (const NW_DOM_Node_t* element)
{
  NW_DOM_DocumentNode_t *docNode;

  docNode = NW_DOM_Node_getOwnerDocument ((NW_DOM_Node_t*) element);

  return NW_DOM_DocumentNode_getDictionary (docNode);
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode NW_HED_DomHelper_GetAttributeName (const NW_DOM_Node_t* element, 
                                               NW_Uint16 attribute, 
                                               NW_String_t *name)
{
  TBrowserStatusCode status;
  NW_String_UCS2Buff_t* ucs2Buff;

  NW_ASSERT (element != NULL);
  NW_ASSERT (name != NULL);

  ucs2Buff = NW_WBXML_Dictionary_getAttributeNameByToken (NW_HED_DomHelper_GetDictionary (element), 
      attribute);

  if (ucs2Buff != NULL) {
    status = StatusCodeConvert(NW_String_UCS2BuffToString (ucs2Buff, name, HTTP_iso_10646_ucs_2));
  }
  else {
    status = KBrsrFailure;
  }

  return status;
}

/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
NW_Uint16 NW_HED_DomHelper_GetElementToken (const NW_DOM_Node_t* element)
  {
  NW_Uint16 token;
  
  // Try to gracefully continue without crashing.
  if (!element) 
    {
    return 0;
    }
  
  /* get the basic token */
  token = NW_DOM_Node_getNodeToken ((NW_DOM_Node_t *) element);
  
  
  /* If it is Literal then there is no corresponding token in the Dictionary. 
  * Return this as "zzzunknown" token.
  */
  
  if (token == NW_WBXML_LITERAL) 
    {
    token = NW_XHTML_ElementToken_zzzunknown;  
    }
  
  return token;
  }

/* ------------------------------------------------------------------------- */
TBrowserStatusCode NW_HED_DomHelper_GetElementName (const NW_DOM_Node_t* element,
                                             NW_String_t* name)
{
  TBrowserStatusCode status;
  NW_String_t temp;

  if (NW_DOM_Node_getNodeType ((NW_DOM_Node_t*) element) != NW_DOM_ELEMENT_NODE) {
    return KBrsrFailure;
  }

  NW_String_initialize (&temp, NULL, 0);
  NW_String_initialize (name, NULL, 0);

  /* get the element's name */
  NW_THROWIF_ERROR (status = StatusCodeConvert(NW_DOM_ElementNode_getTagName((NW_DOM_Node_t*) element, &temp)));

  /* convert it to ucs2 */
  NW_THROWIF_ERROR (status = NW_HED_DomHelper_CatString (name, 
      &temp, NW_HED_DomHelper_GetEncoding (element)));

  NW_String_deleteStorage (&temp);
  return KBrsrSuccess;

NW_CATCH_ERROR
  NW_String_deleteStorage (&temp);
  NW_String_deleteStorage (name);
  
  return status;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode NW_HED_DomHelper_PrintTree (const NW_HED_DomHelper_t* domHelper,
                                        const NW_DOM_Node_t* rootNode)
{
  TBrowserStatusCode status = KBrsrOutOfMemory;
  NW_cXML_DOM_DOMVisitor_t* visitor = NULL;
  NW_DOM_Node_t* domNode;
  NW_DOM_NodeType_t domType;
  NW_String_t nodeName;
  NW_Ucs2* name = NULL;
  NW_Ucs2* type = NULL;
  NW_DOM_Node_t* parentNode = NULL;

  if (parentNode) {} //fix warning

  NW_ASSERT (domHelper != NULL);
  NW_ASSERT (rootNode != NULL);

  NW_String_initialize (&nodeName, NULL, 0);

  NW_LOG0(NW_LOG_LEVEL3, "\n\n** Print DOM Tree **");

  /* create a dom visitor */
  NW_THROWIF_NULL (visitor = NW_cXML_DOM_DOMVisitor_New ((NW_DOM_Node_t*) rootNode));

  /* print the tree */
  while ((domNode = NW_cXML_DOM_DOMVisitor_Next (visitor, /*NW_DOM_ELEMENT_NODE | 
      NW_DOM_TEXT_NODE*/ 255)) != NULL) {

    domType = NW_DOM_Node_getNodeType (domNode);

    /* if it is a text-node extract the text */
    if (domType == NW_DOM_TEXT_NODE) {
      NW_Uint16 i;

      status = NW_HED_DomHelper_GetText (domHelper,
          domNode, NW_TRUE, &nodeName, NULL);
      name = ((NW_Ucs2 *) nodeName.storage);

      /* trunc the string */
      if (NW_Str_Strlen (name) > 10) {
        name[9] = '\0';
      }

      /* remove /r & /n */
      for (i = 0; i < NW_Str_Strlen (name); i++) {
        if ((name[i] == '\r') || (name[i] == '\n')) {
          name[i] = ' ';
        }
      }
    }

    /* otherwise get the element's name */
    else {
      status = NW_HED_DomHelper_GetElementName (domNode, &nodeName);
      if (status == KBrsrSuccess)
      name = ((NW_Ucs2 *) nodeName.storage);
    }

    switch (domType) {
      case 1:
        type = NW_Str_NewcpyConst("tag"); break;
      case 2:
        type = NW_Str_NewcpyConst("text"); break;
      case 4:
        type = NW_Str_NewcpyConst("cdata"); break;
      case 8:
        type = NW_Str_NewcpyConst("pi"); break;
      case 16:
        type = NW_Str_NewcpyConst("comment"); break;
      case 32:
        type = NW_Str_NewcpyConst("doc"); break;
      default:
        type = NW_Str_NewcpyConst("unknown"); break;
    }

    /* log it and clean up */
    parentNode = NW_DOM_Node_getParentNode(domNode) ;
    if (name)
    {
      NW_LOG7(NW_LOG_LEVEL3, "Node: %x, type %s, name: %s, Offset:%x, first_child:%x, next_sib:%x, parent: %x", 
      domNode, type, name, domNode->source_offset, domNode->first_child, domNode->next_sibling, parentNode);
    NW_String_deleteStorage (&nodeName);
    }else
    {
      NW_LOG6(NW_LOG_LEVEL3, "Node: %x, type %s, Offset: %x, first_child:%x, next_sib:%x, parent: %x",
      domNode, type, domNode->source_offset, domNode->first_child, domNode->next_sibling, parentNode);
    }
    NW_Str_Delete(type);
  }
  NW_Object_Delete (visitor);

  return KBrsrSuccess;

NW_CATCH_ERROR
  NW_String_deleteStorage (&nodeName);
  NW_Object_Delete (visitor);
  NW_Str_Delete(type);
  return status;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode NW_HED_DomHelper_CatString (NW_String_t* destString, 
                                        const NW_String_t* srcString, 
                                        NW_Uint32 encoding)
{
  TBrowserStatusCode status;
  NW_String_t src;
  NW_Bool deleteStorage = NW_FALSE;

  NW_ASSERT (destString != NULL);
  NW_ASSERT (srcString != NULL);

  if (srcString->storage == NULL) {
    return KBrsrSuccess;
  }

  /* if encoding is not ucs2 convert it to ucs2 before appendding it */
  if (encoding != HTTP_iso_10646_ucs_2) {
    NW_Ucs2* ucs2String;

    if ((status = StatusCodeConvert(NW_String_stringToUCS2Char (srcString, encoding, &ucs2String))) == KBrsrSuccess) {
      NW_String_initialize (&src, (void *) ucs2String, HTTP_iso_10646_ucs_2);
      NW_String_setUserOwnsStorage (&src);

      deleteStorage = NW_TRUE;
    }
    else {
      return status;
    }
  }
  else {
    NW_String_shallowCopy(&src, (NW_String_t *) srcString);
  }

  /* append the string, copy it if destString is empty */
  if ((destString->length == 0) || (destString->storage == NULL)){
    status = StatusCodeConvert(NW_String_deepCopy (destString, &src));
  }
  else {
    status = StatusCodeConvert(NW_String_concatenate (destString, &src, HTTP_iso_10646_ucs_2));
  }

  /* delete the ucs2 version */
  if (deleteStorage == NW_TRUE) {
    NW_String_deleteStorage (&src);
  }

  return status;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode NW_HED_DomHelper_CatUcs2String (NW_String_t* destString, 
                                            const NW_Ucs2* srcString)
{
  TBrowserStatusCode status;
  NW_String_t src;

  if ((status = StatusCodeConvert(NW_String_initialize (&src, (void *) srcString, HTTP_iso_10646_ucs_2))) == KBrsrSuccess) {
    status = NW_HED_DomHelper_CatString (destString, &src, HTTP_iso_10646_ucs_2);
  }

  return status;
}


/* ------------------------------------------------------------------------- */
// the function returns  TBrowserStatusCode KBrsrSuccess- Attribute is found.
//                                          KBrsrNotFound - Attribute is not found.
//                                          KBrsrDomNodeTypeErr - not a DOM node
// if the attributeToken exists on a element

TBrowserStatusCode  NW_HED_DomHelper_HasAttribute(const NW_DOM_Node_t* element, 
                                                NW_Uint16 attributeToken)
  {
  TBrowserStatusCode status;
  NW_DOM_NodeType_t nodeType;
  NW_DOM_AttributeHandle_t attributeHandle;

  nodeType = NW_DOM_Node_getNodeType ((NW_DOM_Node_t*) element);
  if ((nodeType != NW_DOM_PROCESSING_INSTRUCTION_NODE)
      && (nodeType != NW_DOM_ELEMENT_NODE)) {
    return KBrsrDomNodeTypeErr;
  }
  status = StatusCodeConvert(NW_DOM_ElementNode_getAttributeHandleByToken((NW_DOM_Node_t*)element, attributeToken, &attributeHandle));
  return status;
  }
