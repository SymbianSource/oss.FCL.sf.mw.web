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

#include "nw_xhtml_telephonyservicesi.h"
#include "nwx_string.h"
#include "nw_hed_documentroot.h"
#include "NW_Text_Abstract.h"
#include "nw_text_iterator.h"
#include "nw_wmlscriptch_wmlscriptcontenthandler.h"  /* The load context */
#include "nwx_http_defs.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_XHTML_TelephonyServices_Class_t NW_XHTML_TelephonyServices_Class = {
  { /* NW_Object_Core             */
    /* super                      */ &NW_Object_Core_Class,
    /* queryInterface             */ _NW_Object_Core_QueryInterface
  },
  { /* NW_XHTML_TelephonyServices */
    /* unused                     */ 0
  }
};


/* ------------------------------------------------------------------------- */
const NW_XHTML_TelephonyServices_t NW_XHTML_TelephonyServices = {
  { &NW_XHTML_TelephonyServices_Class }
};

/* ------------------------------------------------------------------------- */

static TBrowserStatusCode
TelephonyServices_StartLoad(NW_XHTML_ContentHandler_t* contentHandler,
                            NW_Ucs2* url)
{
  NW_Text_t *urlText = NULL;
  TBrowserStatusCode status = KBrsrSuccess;

  NW_ASSERT(url != NULL);
  NW_ASSERT(contentHandler != NULL);
  urlText = NW_Text_New (HTTP_iso_10646_ucs_2, url, (NW_Text_Length_t)NW_Str_Strlen(url), 0);
  if (urlText == NULL)
  {
      return KBrsrOutOfMemory;
  }
  /* Send the new url to the loaders */
  status = NW_XHTML_ContentHandler_StartLoad (contentHandler, NW_TextOf (urlText), 
    NW_HED_UrlRequest_Reason_DocLoad, (void *) &NW_XHTML_TelephonyServices_Class,
    NW_UrlRequest_Type_Any);
  /* If the load request was dispatched to the WTAI handler, 
     the response is KBrsrHedContentDispatched */
  if (status == KBrsrHedContentDispatched)
  {
      status = KBrsrSuccess;
  }

  NW_Object_Delete(urlText);
  return status;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_XHTML_TelephonyServices_MakeCall(NW_XHTML_TelephonyServices_t* telServices,
                                    NW_XHTML_ContentHandler_t* contentHandler,
                                    NW_Text_t* number,
                                    NW_Bool cti)
{
  NW_Ucs2* ucs2Number = NULL;
  NW_Ucs2* storage = NULL;
  const char* const makeCallUrlPrefix = "wtai://wp/mc;";
  NW_Text_Length_t charCount;
  NW_Uint8 freeNeeded = NW_FALSE;

  NW_REQUIRED_PARAM(telServices);

  NW_TRY (status ) {
    storage = (NW_Ucs2*)
      NW_Text_GetUCS2Buffer (number, NW_Text_Flags_Aligned, &charCount, &freeNeeded);
    NW_THROW_OOM_ON_NULL (storage, status);

    if (cti){
      ucs2Number = (NW_Ucs2*) NW_Mem_Malloc((NW_Asc_strlen(makeCallUrlPrefix) + NW_Str_Strlen(storage) + 1) * sizeof(NW_Ucs2));
      NW_THROW_OOM_ON_NULL (ucs2Number, status);
      NW_Str_StrcpyConst(ucs2Number, makeCallUrlPrefix);
      NW_Str_Strcat(ucs2Number, storage);
    }
    else{ // skip the scheme part
      ucs2Number = (NW_Ucs2*) NW_Mem_Malloc((NW_Asc_strlen(makeCallUrlPrefix) + NW_Str_Strlen(storage) - 3) * sizeof(NW_Ucs2));
      NW_THROW_OOM_ON_NULL (ucs2Number, status);
      NW_Str_StrcpyConst(ucs2Number, makeCallUrlPrefix);
      NW_Str_Strcat(ucs2Number, storage + 4);
    }
    /* Send the load request */
    status = TelephonyServices_StartLoad(contentHandler, ucs2Number);
  }
  NW_CATCH (status) {
  }
  NW_FINALLY {
    if ((storage != NULL) && freeNeeded){
      NW_Mem_Free(storage);
    }
    NW_Mem_Free(ucs2Number);
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_XHTML_TelephonyServices_SaveToPhoneBook(NW_XHTML_TelephonyServices_t* telServices,
                                           NW_XHTML_ContentHandler_t* contentHandler,
                                           NW_Text_t* number,
                                           NW_Text_t* name,
                                           NW_Text_t* email,
                                           NW_Bool cti)
{
  NW_Ucs2* storage = NULL;
  NW_Text_Length_t charCount;
  NW_Uint8 freeNeeded = NW_FALSE;
  NW_Url_Schema_t scheme;
  NW_Ucs2* ucs2Number = NULL;
  NW_Ucs2* ucs2Name = NULL;
  NW_Ucs2* ucs2Email = NULL;
  NW_Ucs2* url = NULL;
  const char* const makeCallUrlPrefix = "wtai://wp/ap;";
  const char* const numberNameSeparator = ";";
  NW_Uint32 len = NW_Asc_strlen(makeCallUrlPrefix) + 3; // NULL terminator and ';' separators

  NW_REQUIRED_PARAM(telServices);

  NW_TRY (status) {

    NW_THROW_ON_NULL (number, status, KBrsrFailure);
    storage = (NW_Ucs2*)
      NW_Text_GetUCS2Buffer (number, NW_Text_Flags_Aligned, &charCount, &freeNeeded);
    NW_THROW_OOM_ON_NULL (storage, status);

    (void) NW_Url_GetScheme(storage, &scheme);
  
    ucs2Number = NULL;
    ucs2Name = NULL;
    ucs2Email = NULL;

    /* to handle cti */
    if(cti)
    {
      ucs2Number = NW_Str_Newcpy(storage);
    }
    else /* handle tel:// scheme */
    {
      ucs2Number = NW_Str_Newcpy(storage+4);      
    }
    NW_THROW_OOM_ON_NULL (ucs2Number, status);

    len += NW_Str_Strlen(ucs2Number);

    if (freeNeeded){
      NW_Mem_Free(storage);
      storage = NULL;
    }

    if (name)
    {
      storage = (NW_Ucs2*)
        NW_Text_GetUCS2Buffer (name, NW_Text_Flags_Aligned, &charCount, &freeNeeded);
      NW_THROW_OOM_ON_NULL (storage, status);

      if (freeNeeded){
        ucs2Name = storage;
      }
      else{
        ucs2Name = NW_Str_Newcpy(storage);
        NW_THROW_OOM_ON_NULL (ucs2Name, status);
      }
      len += NW_Str_Strlen(ucs2Name);
    }

    if (email)
    {
      storage = (NW_Ucs2*)
        NW_Text_GetUCS2Buffer (email, NW_Text_Flags_Aligned, &charCount, &freeNeeded);
      NW_THROW_OOM_ON_NULL (storage, status);
	  
      if (freeNeeded){
        ucs2Email = storage;
        storage = NULL;
      }
      else{
        ucs2Email = NW_Str_Newcpy(storage);
        NW_THROW_OOM_ON_NULL (ucs2Email, status);
      }
      len += NW_Str_Strlen(ucs2Email);
    }

    url = (NW_Ucs2*) NW_Mem_Malloc(len * sizeof(NW_Ucs2));
    NW_THROW_OOM_ON_NULL (url, status);
    NW_Str_StrcpyConst(url, makeCallUrlPrefix);
    NW_Str_Strcat(url, ucs2Number);
    NW_Str_StrcatConst(url, numberNameSeparator);
    if (ucs2Name != NULL)
    {
      NW_Str_Strcat(url, ucs2Name);
    }
    NW_Str_StrcatConst(url, numberNameSeparator);
    if (ucs2Email != NULL)
    {
      NW_Str_Strcat(url, ucs2Email);
    }
    /* Send the load request */
    status = TelephonyServices_StartLoad(contentHandler, url);
  }
  NW_CATCH (status) {
  }
  NW_FINALLY {
    if ((storage != NULL) && freeNeeded){
      NW_Mem_Free(storage);
    }
    NW_Mem_Free(ucs2Number);
    if (ucs2Name != NULL)
      NW_Mem_Free(ucs2Name);
    if (ucs2Email != NULL)
      NW_Mem_Free(ucs2Email);
    if (url != NULL)
      NW_Mem_Free(url);

    return status;
  } NW_END_TRY
}

// ----------------------------------------------------------------------------
// NW_XHTML_TelephonyServices_ParseNumber
//
// Parse the telephone number from the wtai or tel scheme. Also check phone
// number from cti attribute, if present with tel scheme and aCti = NW_TRUE.
//
// WTAI cases: <a href="wtai://wp/mc;6175551234;!retValue">
//             <a href="wtai://wp/mc;6175551234">
//             <a href="wtai://wp/sd;123*987!retValue">
//             <a href="wtai://wp/ap;617551234;Joe Smith!returnValue">
// TEL cases:  <a href="tel:6175551234">
//             <a href="tel:6175551234 cti=7815554321/67#,89*">
// 
// Returns NW_TRUE if a valid number, NW_FALSE if not a valid number. 
// ----------------------------------------------------------------------------
NW_Bool
NW_XHTML_TelephonyServices_ParseNumber(NW_XHTML_TelephonyServices_t* aTelServices,
                                       NW_Ucs2* aNumber,
                                       NW_Bool aCti)
    {
    NW_REQUIRED_PARAM(aTelServices);
    NW_Bool retVal = NW_FALSE;
    NW_Ucs2 ch;
    NW_Ucs2* unEscapedStr = NULL;
    NW_Int32 charCount = 0;
    NW_Int32 digitCount = 0;
    
    if (aNumber == NULL)
       {
       NW_FINISHED
       }

    /* unescape the phone number */
    unEscapedStr = NW_Str_New(NW_Url_UnEscapeStringLen(aNumber));
    if (unEscapedStr == NULL)
       {       // not able to allocate memory
       NW_FINISHED
       }

    if (NW_Url_UnEscapeString(aNumber, unEscapedStr) == NULL)
       {      // not able to unescape the phone number string
       NW_FINISHED 
       }

    ch = *unEscapedStr;

    // Allow numbers to begin with the "+" char.
    if (ch == '+')
       {
       charCount++;
       ch = unEscapedStr[charCount];
       }
  
    // Parse the phone number, until we reach the end
    while (ch != 0)
        {
        // Do we have a valid digit
        if (NW_Str_Isdigit(ch))
            {
            charCount++;
            ch = unEscapedStr[charCount];
            digitCount++;
            }
        // Do we have a valid non-digit character
        else if (NW_Str_Isspace(ch) || 
                 (ch == '#') || 
                 (ch == '*') ||
                 (ch == '.') ||
                 (ch == '(') ||
                 (ch == ')') ||
                 (ch == '-') ||
                 (ch == ','))
            {      
            charCount++;
            ch = unEscapedStr[charCount];
            }
        // Do we have a returnValue delimiter "!", or a parameter delimiter ";".
        // We are at the end of the phone number.
        // For example: wtai:\\wp\ap;6175551234;Joe Smith!retValue
        else if ((ch == '!') || (ch ==';'))
            {
            break;
            }
        // Do we have a character that is only valid in a cti number
        else if (ch == '/')
            {
            // If this a cti number, break and continue processing as cti number
            if (aCti)
                {
                break;
                }
            // We have a non-cti number with invalid characters
            else
                {
                NW_FINISHED
                }
            }
        // We have an invalid character in the number, before we came to the end
        else
            {
            NW_FINISHED
            }
        }   // end of while loop 

    // Is our number an invalid length
    if ((digitCount < 1) || (digitCount > 24))
        {
        NW_FINISHED
        }

    // If this is not a cti number, we are done
    if (!aCti)
        {
        retVal =  NW_TRUE;
        NW_FINISHED
        }
    
    // This is a cti number, continue parsing the output with cti specs
    while (ch != 0)
        {
        // Do we have a valid cti character
        if (NW_Str_Isdigit(ch) ||
            (ch == '#') || 
            (ch == '*') ||
            (ch == ','))
            {      
            charCount++;
            ch = aNumber[charCount];
            }
        // Do we have a slash, check if consecutive slashes
        else if (ch == '/')   
            {
            charCount++;
            ch = unEscapedStr[charCount];
            // Consecutive "/" (pause) are not allowed according to NTT DoCoMo's 
            // "I-Mode and C-Html difference" document (section 3), also wtai spec.
            if (ch == '/')
                {
                NW_FINISHED
                }
            } 
        // We have an invalid character in the cti number, before we came to end
        else
            {
            NW_FINISHED
            }
        }   // end of while loop

    retVal = NW_TRUE;

    NW_FINALLY
        {
        NW_Str_Delete (unEscapedStr);
        return retVal;
        }
    }

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_XHTML_TelephonyServices_Mailto_SaveToPhoneBook(NW_XHTML_TelephonyServices_t* telServices,
                                                  NW_XHTML_ContentHandler_t* contentHandler,
                                                  NW_Text_t* email,
                                                  NW_Text_t* name,
                                                  NW_Bool isEmail)
{
  NW_Ucs2* storage = NULL;
  NW_Text_Length_t charCount;
  NW_Uint8 freeNeeded = NW_FALSE;
  NW_Ucs2* ucs2Name = NULL;
  NW_Ucs2* ucs2Email = NULL;
  NW_Int32 ucs2EmailLength;

  const char* const makeCallUrlPrefix = "wtai://wp/ap;;"; // No Number
  const char* const numberNameSeparator = ";";
  ucs2EmailLength = (NW_Asc_strlen(makeCallUrlPrefix) + NW_Asc_strlen(numberNameSeparator))
                    * sizeof(NW_Ucs2);

  NW_REQUIRED_PARAM(telServices);

  NW_TRY (status) {
    if (name)
    {
      storage = (NW_Ucs2*)
        NW_Text_GetUCS2Buffer (name, NW_Text_Flags_Aligned, &charCount, &freeNeeded);
      NW_THROW_OOM_ON_NULL (storage, status);

      ucs2Name = NW_Str_Newcpy(storage);
      NW_THROW_OOM_ON_NULL (ucs2Name, status);
      if (freeNeeded){
        NW_Mem_Free(storage);
        storage = NULL;
        freeNeeded = NW_FALSE;
      }

      ucs2EmailLength += (NW_Str_Strlen(ucs2Name) * sizeof(NW_Ucs2));
    }

    NW_THROW_ON_NULL (email, status, KBrsrFailure);
    storage = (NW_Ucs2*)
      NW_Text_GetUCS2Buffer (email, NW_Text_Flags_Aligned, &charCount, &freeNeeded);
    NW_THROW_OOM_ON_NULL (storage, status);

    /* handle mailto: scheme */
    if(isEmail)
    {
        ucs2EmailLength += ((NW_Str_Strlen(storage) + 1)* sizeof(NW_Ucs2));
    }
    else
    {
        ucs2EmailLength += ((NW_Str_Strlen(storage) - 6) * sizeof(NW_Ucs2));
    }
    ucs2Email = (NW_Ucs2*) NW_Mem_Malloc(ucs2EmailLength);
    NW_THROW_OOM_ON_NULL (ucs2Email, status);
    /* Create a new URL containing the wtai scheme and the e-mail address */
    NW_Str_StrcpyConst(ucs2Email, makeCallUrlPrefix);

    if (ucs2Name != NULL)
    {
      NW_Str_Strcat(ucs2Email, ucs2Name);
    }
    NW_Str_StrcatConst(ucs2Email, numberNameSeparator);

    if(isEmail)
    {
        NW_Str_Strcat(ucs2Email, storage );
    }
    else
    {
        NW_Str_Strcat(ucs2Email, storage + 7);
    }

    /* Send the load request */
    status = TelephonyServices_StartLoad(contentHandler, ucs2Email);
  }
  NW_CATCH (status) {
  }
  NW_FINALLY {
    if ((storage != NULL) && freeNeeded){
      NW_Mem_Free(storage);
    }
    if (ucs2Name != NULL){
      NW_Mem_Free(ucs2Name);
    }
    NW_Mem_Free(ucs2Email);
    return status;
  } NW_END_TRY
}
