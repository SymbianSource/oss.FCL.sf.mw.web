/*
* Copyright (c) 2000-2002 Nokia Corporation and/or its subsidiary(-ies).
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

#include "nwx_string.h"
#include "nw_wml_core.h"
#include "nw_markup_wmlvalidatori.h"
#include "wml_task.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_Markup_WmlValidator_Class_t NW_Markup_WmlValidator_Class = {
  { /* NW_Object_Core    */
    /* super             */ &NW_FBox_Validator_Class,
    /* queryInterface    */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base    */
    /* interfaceList     */ NULL
  },
  { /* NW_Object_Dynamic */
    /* instanceSize      */ sizeof (NW_Markup_WmlValidator_t),
    /* construct         */ _NW_Markup_WmlValidator_Construct,
    /* destruct          */ _NW_Markup_WmlValidator_Destruct
  },
  { /* NW_FBox_Validator */
    /* validate          */ _NW_Markup_WmlValidator_Validate,
    /* getFormat         */ _NW_Markup_WmlValidator_GetFormat
  },
  { /* NW_Markup_WmlValidator */
    /* unused                      */ NW_Object_Unused
  }
};


/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Markup_WmlValidator_Construct (NW_Object_Dynamic_t* dynamicObject,
                                   va_list* argp)
{
  NW_Markup_WmlValidator_t* thisObj;
  NW_Text_t* format;

  /* parameter assertions */
  NW_ASSERT (dynamicObject != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject,
                                     &NW_Markup_WmlValidator_Class));
  NW_ASSERT (argp != NULL);

  /* for convenience */
  thisObj = NW_Markup_WmlValidatorOf (dynamicObject);

  /* No NW_Fbox_Validator superclass constructor to call. */

  thisObj->formatUcs2 = NULL;
  thisObj->mode = NW_FBox_Validator_Mode_None;
  thisObj->emptyOk = NW_FBox_Validator_EmptyOk_None;
  /* Default is to do a full string match, not prefix. */
  thisObj->fullMatch = NW_TRUE;   

  /* initialize the member variables */
  format = va_arg (*argp, NW_Text_t*);
  if ( format != NULL ) {
    if ( NW_Text_GetCharCount(format) >= 1 ) {
      NW_Ucs4 ch;

      ch = NW_Text_GetAt( format, 0 );
      if ( ch != 0 ) {
        /* Is not an empty string. Take a copy of it.  
           Otherwise leave as null pointer. */
        thisObj->formatUcs2 = NW_Text_GetUCS2Buffer(format, 
                                                    NW_Text_Flags_NullTerminated | 
                                                      NW_Text_Flags_Aligned |
                                                      NW_Text_Flags_Copy,
                                                    NULL, NULL);
      }
    }
  }


  thisObj->mode = (NW_FBox_Validator_Mode_t)va_arg (*argp, NW_Uint32);
  thisObj->emptyOk = (NW_FBox_Validator_EmptyOk_t)va_arg (*argp, NW_Uint32);
  thisObj->fullMatch = (NW_Bool)va_arg (*argp, NW_Uint32);

  /* successful completion */
  return KBrsrSuccess;

}

/* ------------------------------------------------------------------------- */
void
_NW_Markup_WmlValidator_Destruct(NW_Object_Dynamic_t* dynamicObject)
{
  NW_Markup_WmlValidator_t* thisObj;

  /* for convenience */
  thisObj = NW_Markup_WmlValidatorOf (dynamicObject);

  /* Destroy the format */
  NW_Str_Delete((NW_Ucs2*) thisObj->formatUcs2);

  /* No NW_Fbox_Validator superclass destructor to call. */

  return;
}

/* --------------------------------------------------------------------------*/
TBrowserStatusCode
_NW_Markup_WmlValidator_Validate(NW_FBox_Validator_t* validator,
                                 const NW_Text_t* string)
{
  TBrowserStatusCode status;
  NW_Bool stringFreeNeeded = NW_FALSE;
  NW_Ucs2* stringStr;
  NW_Markup_WmlValidator_t* thisObj;
  NW_Bool prefixMatch;
  NW_WmlInput_EmptyOk_t emptyOk;

  NW_ASSERT(validator != NULL);
  NW_ASSERT(NW_Object_IsInstanceOf(validator, &NW_Markup_WmlValidator_Class));

  /* For convenience. */
  thisObj = NW_Markup_WmlValidatorOf( validator );

  if (string != NULL) {
    stringStr = NW_Text_GetUCS2Buffer(string, 
                                      NW_Text_Flags_NullTerminated | 
                                         NW_Text_Flags_Aligned,
                                      NULL, &stringFreeNeeded);
  } else {
    stringStr = NULL;
  }

  /* Convert emptyOk to values for NW_WmlInput_ValidateValue. */
  if (thisObj->emptyOk == NW_FBox_Validator_EmptyOk_True) {
    emptyOk = NW_WmlInput_EmptyOk_True;
  } else if (thisObj->emptyOk == NW_FBox_Validator_EmptyOk_False) {
    emptyOk = NW_WmlInput_EmptyOk_False;
  } else {
    emptyOk = NW_WmlInput_EmptyOk_None;
  }

  prefixMatch = (NW_Bool)(!thisObj->fullMatch); /* True when prefix match, so complement */ 
  if  ( NW_WmlInput_ValidateValue(stringStr, (NW_Ucs2*) thisObj->formatUcs2,
                                  emptyOk,
                                  prefixMatch, 
                                  0) ) {
    status = KBrsrSuccess;
  } else {
    status = KBrsrWmlbrowserInputNonconformingToMask;
  }

  if ( stringFreeNeeded ) {
    NW_Str_Delete( stringStr );
    stringStr = NULL;
  }

  return status;
}

/* --------------------------------------------------------------------------*/
TBrowserStatusCode
_NW_Markup_WmlValidator_GetFormat(NW_FBox_Validator_t* validator,
                                  const NW_Ucs2** format,
                                  NW_FBox_Validator_Mode_t* mode,
                                  NW_FBox_Validator_EmptyOk_t* emptyOk)
{
  NW_Markup_WmlValidator_t* thisObj;

  NW_ASSERT( validator != NULL );
  NW_ASSERT( format != NULL );
  NW_ASSERT( mode != NULL );
  NW_ASSERT( emptyOk != NULL );
  NW_ASSERT(NW_Object_IsInstanceOf(validator, &NW_Markup_WmlValidator_Class));

  /* for convenience */
  thisObj = NW_Markup_WmlValidatorOf(validator);

  *format = thisObj->formatUcs2;
  *mode = thisObj->mode;
  *emptyOk = thisObj->emptyOk;

  /* make sure the format string is valid before returning */
  if ((*format != NULL) && ( ! NW_WmlInput_ValidateFormat(*format)))
  {
    *format = NULL;
  }

  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- *
   convenience methods
 * ------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------*/
NW_Markup_WmlValidator_t*
NW_Markup_WmlValidator_New (const NW_Text_t* format,
                            const NW_FBox_Validator_Mode_t  mode,
                            const NW_FBox_Validator_EmptyOk_t emptyOk,
                            const NW_Bool fullMatch)
{
  return (NW_Markup_WmlValidator_t*)
    NW_Object_New (&NW_Markup_WmlValidator_Class, format, (NW_Uint32)mode, (NW_Uint32)emptyOk, (NW_Uint32)fullMatch);
}


