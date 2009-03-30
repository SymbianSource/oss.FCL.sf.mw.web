/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies).
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


#include "nw_wml_core.h"
#include "wml_url_utils.h"
#include "wml_task.h"
#include "nw_wml1x_wml1xformliaisoni.h" 
#include "nw_wml1x_wml1xcontenthandler.h"
#include "nw_adt_resizablevector.h"
#include "NW_Text_Abstract.h"
#include "nw_hed_documentroot.h"
#include "wml_elm_attr.h"
#include "BrsrStatusCodes.h"
#include "wml_ops.h"

/* ------------------------------------------------------------------------- *
   private types
 * ------------------------------------------------------------------------- */

typedef enum {
  NW_Wml1x_FormControlType_None,
  NW_Wml1x_FormControlType_InputPassword, /* <input type=password> */
  NW_Wml1x_FormControlType_InputText,     /* <input type=text> */
  NW_Wml1x_FormControlType_Option         /* <option> */
} NW_Wml1x_FormControlType_t;


/* ------------------------------------------------------------------------- *
   private methods
 * ------------------------------------------------------------------------- */

static
NW_Wml1x_FormControlType_t
NW_Wml1x_FormLiaison_GetControlType(NW_Wml1x_FormLiaison_t  *thisObj,
                                    void                    *elemId);

static
NW_Wml1x_FormControlType_t
NW_Wml1x_FormLiaison_GetInputType(NW_Wml1x_FormLiaison_t  *thisObj,
                                  void                    *elemId);


static NW_Ucs2* FormatGetMask(const NW_Ucs2 *format);

/*****************************************************************

  Name:         NW_Wml1x_FormLiaison_GetControlType()

  Description:

  Parameters:

  Algorithm:

  Return Value:

*****************************************************************/
static
NW_Wml1x_FormControlType_t
NW_Wml1x_FormLiaison_GetControlType(NW_Wml1x_FormLiaison_t  *thisObj,
                                    void                    *elemId)
{
  NW_Wml1x_FormControlType_t  type = NW_Wml1x_FormControlType_None;
  NW_Wml_ElType_e             elType = UNKNOWN_ELEMENT;
  NW_Wml_Element_t            *el;
  NW_Uint16                   elementIdAsUint16;

  elementIdAsUint16 = NW_UINT16_CAST ((NW_Uint32) elemId);  /* NOTE: this assumes (void*) is 32 bits */

  (void) NW_Wml_GetElementType(&(thisObj->contentHandler->wmlInterpreter),
															 elementIdAsUint16,
															 &elType,
															 &el);

  switch (elType) {
  case INPUT_ELEMENT:
    type = NW_Wml1x_FormLiaison_GetInputType(thisObj, elemId);
    break;
  case OPTION_ELEMENT:
    type = NW_Wml1x_FormControlType_Option;
    break;
  default:
    type = NW_Wml1x_FormControlType_None;
    break;
  }

  return type;
}

/*****************************************************************

  Name:         NW_Wml1x_FormLiaison_GetInputType()

  Description:

  Parameters:

  Algorithm:

  Return Value:

*****************************************************************/
static
NW_Wml1x_FormControlType_t
NW_Wml1x_FormLiaison_GetInputType(NW_Wml1x_FormLiaison_t  *thisObj,
                                  void                    *elemId)
{
  /* default to <input type=text> */
  NW_Wml1x_FormControlType_t  type = NW_Wml1x_FormControlType_InputText;
  NW_Ucs2                     *retString = NULL;

  static const NW_Ucs2 passwordTag[] = {'p','a','s','s','w','o','r','d'};
  NW_Uint16                   elementIdAsUint16;

  elementIdAsUint16 = NW_UINT16_CAST ((NW_Uint32) elemId);  /* NOTE: this assumes (void*) is 32 bits */

  (void) NW_Wml_GetAttribute (&(thisObj->contentHandler->wmlInterpreter),
		                          elementIdAsUint16,
															TYPE_ATTR,
															&retString);

  if (retString != NULL)
  {
    if (NW_Mem_memcmp(retString,
                     passwordTag,
                     sizeof(passwordTag)) == 0)
    {
      type = NW_Wml1x_FormControlType_InputPassword;
    }

    NW_Mem_Free(retString);
  }

  return type;
}



/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_Wml1x_FormLiaison_Class_t NW_Wml1x_FormLiaison_Class = {
  { /* NW_Object_Core          */
    /* super                   */ &NW_FBox_FormLiaison_Class,
    /* queryInterface          */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base          */
    /* interfaceList           */ NULL
  },
  { /* NW_Object_Dynamic       */
    /* instanceSize            */ sizeof (NW_Wml1x_FormLiaison_t),
    /* construct               */ _NW_Wml1x_FormLiaison_Construct,
    /* destruct                */ _NW_Wml1x_FormLiaison_Destruct
  },
  { /* NW_FBox_FormLiaison     */
    /* getWBXMLVersion         */ _NW_Wml1x_FormLiaison_GetWBXMLVersion,
    /* addControl              */ NULL,
    /* setInitialStringValue   */ _NW_Wml1x_FormLiaison_SetInitialStringValue,
    /* setStringValue          */ _NW_Wml1x_FormLiaison_SetStringValue,
    /* validateStringValue     */ _NW_Wml1x_FormLiaison_ValidateStringValue,
    /* getStringValue          */ _NW_Wml1x_FormLiaison_GetStringValue,
    /* getStringName           */ _NW_Wml1x_FormLiaison_GetStringName,
    /* getStringTitle          */ _NW_Wml1x_FormLiaison_GetStringTitle,
    /* setBoolValue            */ _NW_Wml1x_FormLiaison_SetBoolValue,
    /* resetRadioValue         */ NULL,
    /* toggleBoolValue         */ _NW_Wml1x_FormLiaison_ToggleBoolValue,
    /* getBoolValue            */ _NW_Wml1x_FormLiaison_GetBoolValue,
    /* reset                   */ NULL,
    /* submit                  */ NULL,
    /* focus                   */ NULL,
    /* isOptionMultiple        */ NULL,
    /* isOptionSelected        */ NULL,
    /* optionHasOnPick         */ _NW_Wml1x_FormLiaison_OptionHasOnPick,
    /* isLocalNavOnPick        */ _NW_Wml1x_FormLiaison_IsLocalNavOnPick,
    /* getInitialValue         */ NULL,
    /* getDocRoot              */ _NW_Wml1x_FormLiaison_GetDocRoot,
    /* delegateEcmaEvent       */ _NW_Wml1x_FormLiaison_DelegateEcmaEvent,
   },
  { /* NW_Wml1x_FormLiaison    */
    /* unused                  */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/*****************************************************************

  Name:         _NW_Wml1x_FormLiaison_Construct()

  Description:

  Parameters:

  Algorithm:

  Return Value: KBrsrSuccess
                or, return value from superclass constructor

*****************************************************************/
TBrowserStatusCode
_NW_Wml1x_FormLiaison_Construct (NW_Object_Dynamic_t  *dynamicObject,
                                 va_list              *argp)
{
  NW_Wml1x_FormLiaison_t  *thisObj;

  /* parameter assertions */
  NW_ASSERT (dynamicObject != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject,
                                     &NW_Wml1x_FormLiaison_Class));
  NW_ASSERT (argp != NULL);

  /* for convenience */
  thisObj = NW_Wml1x_FormLiaisonOf (dynamicObject);

  /* initialize our member variables */
  thisObj->contentHandler = va_arg (*argp, NW_Wml1x_ContentHandler_t*);

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
void
_NW_Wml1x_FormLiaison_Destruct(NW_Object_Dynamic_t* dynamicObject)
{
  NW_Wml1x_FormLiaison_t  *thisObj;

  /* for convenience */
  thisObj = NW_Wml1x_FormLiaisonOf (dynamicObject);

  NW_Object_Delete (thisObj->lastStringValue);
}

/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */

/*****************************************************************

  Name:         _NW_Wml1x_FormLiaison_GetWBXMLVersion()

  Description:  The WBXML version of the WML document is retrieved and translated
                to an enum as defined by TWapDocType in "rb_tempest\fbox\include\
                CoreWrapper.h"

  Parameters:   aWapDocType is an "out" parameter, representative of spec level of
                conformance with WAPFORUM WML the gateway encoder used to
                encode the content from the origin server.

  Algorithm:    Using NW_FBox_FormLiaison_t, look up the version field
                maintained under tinyparser/domtree/doc/version.

  Return Value: KBrsrSuccess, if pointers valid and a value exists
                otherwise, return KBrsrFailure

*****************************************************************/
TBrowserStatusCode
_NW_Wml1x_FormLiaison_GetWBXMLVersion(NW_FBox_FormLiaison_t*  thisObj,
                                      NW_Uint32*               wmlVersion)
{
  NW_Wml1x_FormLiaison_t  *formLiaison;
  TBrowserStatusCode             status = KBrsrSuccess;

  formLiaison = NW_Wml1x_FormLiaisonOf(thisObj);
  if (formLiaison == NULL)
  {
    status = KBrsrFailure;
  }
  else
  {
    status = NW_Wml_GetPublicId(&(formLiaison->contentHandler->wmlInterpreter),
                                wmlVersion );
  }
  return status;
}
/*****************************************************************

  Name:         _NW_Wml1x_FormLiaison_SetInitialStringValue()

  Description:  Sets the initial value for the input form control from the content.
                Currently, not used in WML, so this is just a stub.

  Return Value: KBrsrSuccess 

*****************************************************************/
TBrowserStatusCode
_NW_Wml1x_FormLiaison_SetInitialStringValue(NW_FBox_FormLiaison_t  *thisObj,
                                            void                   *elemId,
																						NW_Text_Length_t			 maxChars)
{
  NW_REQUIRED_PARAM( thisObj );
  NW_REQUIRED_PARAM( elemId );
  NW_REQUIRED_PARAM( maxChars );

  return KBrsrSuccess;
}

//*****************************************************************
//
//  Name:         _NW_Wml1x_FormLiaison_SetStringValue()
//
//  Description:
//
//  Parameters:   elemId is the actual element Id, not a pointer to anything
//                  so some fancy casting is necessary to avoid confusing the compiler
//                value is a pointer to a text object which we are responsible 
//                  for deleting when we're done with it.
//
//  Algorithm:    Using the elemId, look up the name of the variable to be set.
//                Convert the passed-in value to a Ucs2 string.
//                If NW_Evt_HandleInputEvent() allows the value,
//                  use NW_Wml_SetVar() to save the value on the variable.
//                Else 
//                  notify the user
//
//  Return Value: KBrsrFailure if elemId not an Input element;
//                otherwise, return value from
//
//*****************************************************************/
TBrowserStatusCode
_NW_Wml1x_FormLiaison_SetStringValue(NW_FBox_FormLiaison_t  *thisObj,
                                    void                    *elemId,
                                    NW_Text_t               *value)
  {
  TBrowserStatusCode      status = KBrsrSuccess;
  NW_Wml1x_FormLiaison_t  *formLiaison;
  NW_Ucs2                 *varName = NULL;
  NW_Ucs2                 *varValue = NULL;
  NW_Bool                 freeValue;
  NW_Uint16               elementIdAsUint16;
  NW_Wml_ElType_e         elType = UNKNOWN_ELEMENT;
  NW_Wml_Element_t        *el;
  NW_Bool                 validateOk;

  elementIdAsUint16 = NW_UINT16_CAST ((NW_Uint32) elemId);  // NOTE: this assumes (void*) is 32 bits 

  formLiaison = NW_Wml1x_FormLiaisonOf(thisObj);

  switch (NW_Wml1x_FormLiaison_GetControlType(formLiaison, elemId))
       {
       // Password input boxes ignore format attribute, so no longer need to validate input box value
       // against format.  Simply use and save whatever user has entered.
       case NW_Wml1x_FormControlType_InputPassword:  
       varValue = 
           (NW_Ucs2*) NW_Text_GetUCS2Buffer (value,
                                         NW_Text_Flags_NullTerminated | NW_Text_Flags_Aligned,
                                         NULL,
                                         &freeValue);
 
       // lookup name 
       status = NW_Wml_GetAttribute (&(formLiaison->contentHandler->wmlInterpreter),
                                        elementIdAsUint16,
                                        NAME_ATTR,
                                        &varName);
       if (status == KBrsrSuccess)
           {
           // password content no longer must match a format, that is to say, formats are now
           // ignored for password fields, so simply set the variable in the formLiaison.
           status = NW_Wml_SetVar (&(formLiaison->contentHandler->wmlInterpreter),
                                    varName,
                                    varValue);
           }
       // free the name string 
       NW_Str_Delete(varName);

       if (freeValue) 
           {
           NW_Str_Delete(varValue);
           }
       break;

       case NW_Wml1x_FormControlType_InputText:    
       varValue = 
          (NW_Ucs2*) NW_Text_GetUCS2Buffer (value,
                                        NW_Text_Flags_NullTerminated | NW_Text_Flags_Aligned,
                                        NULL,
                                        &freeValue);

       // perform WML processing on new varValue
       status = NW_Wml_GetElementType(&(formLiaison->contentHandler->wmlInterpreter),
                                        elementIdAsUint16,
                                        &elType,
                                        &el);

       if (status == KBrsrSuccess)
           {
           // lookup name 
           status = NW_Wml_GetAttribute (&(formLiaison->contentHandler->wmlInterpreter),
                                        elementIdAsUint16,
                                        NAME_ATTR,
                                        &varName);

       // setting NW_FALSE for the update variable and sending the address of a 
       // boolean to receive the results of the validate operation, allows us to
       // non-destructively find out if the input matches the input mask (if any)
       //
       status = NW_Wml_HandleInputEvent(&(formLiaison->contentHandler->wmlInterpreter),
                                       el,
                                       varValue,
                                       NW_FALSE,
                                       &validateOk);
       if (validateOk)
           {
           status = NW_Wml_SetVar (&(formLiaison->contentHandler->wmlInterpreter),
                                       varName,
                                       varValue);
           }
       else
           {
           status = KBrsrWmlbrowserInputNonconformingToMask;
           }

       // free the name string
       NW_Str_Delete(varName); 
       }

       if (freeValue) 
           {
           NW_Str_Delete(varValue);
           }
       break;

       default:
       //
       // cannot set value; the input element is not text type
       //
       status = KBrsrFailure;
       break;
       }

  NW_Object_Delete(value);
  return status;
  }


/*****************************************************************

  Name:         _NW_Wml1x_FormLiaison_ValidateStringValue()

  Description:

  Parameters:   elemId is the actual element Id, not a pointer to anything
                  so some fancy casting is necessary to avoid confusing the compiler
                value is a pointer to a text object which we are responsible 
                  for deleting when we're done with it.

  Algorithm:    Using the elemId, look up the name of the variable to be set.
                If NW_WmlInput_ValidateValue() disallows the value,
                  notify the user

  Return Value: KBrsrFailure if elemId not an Input element;
                otherwise, return value from NW_WmlInput_ValidateValue()

*****************************************************************/
TBrowserStatusCode
_NW_Wml1x_FormLiaison_ValidateStringValue (NW_FBox_FormLiaison_t* thisObj,
                                           void* elemId,
                                           const NW_Text_t* value)
{
  TBrowserStatusCode             status        = KBrsrSuccess;
  NW_Wml1x_FormLiaison_t  *formLiaison;
  NW_Uint32               publicId;
  NW_Ucs2                 *strValue     = NULL;
  NW_Ucs2                 *format       = NULL;
  NW_Ucs2                 *emptyok_str  = NULL;
  NW_Ucs2                 *maxLengthStr = NULL;
  NW_WmlInput_EmptyOk_t   emptyok       = NW_WmlInput_EmptyOk_None;
  NW_Bool                 freeValue     = NW_FALSE;
  NW_Uint16               elementIdAsUint16;
  NW_Bool                 validateOk    = NW_FALSE;

  elementIdAsUint16 = NW_UINT16_CAST ((NW_Uint32) elemId);  /* NOTE: this assumes (void*) is 32 bits */

  formLiaison = NW_Wml1x_FormLiaisonOf(thisObj);

  switch (NW_Wml1x_FormLiaison_GetControlType(formLiaison, elemId))
  {
   case NW_Wml1x_FormControlType_InputPassword: /* FALL THROUGH */
   case NW_Wml1x_FormControlType_InputText:     /* FALL THROUGH */

    /* get string from NW_Text_UCS2 object */
    strValue = (NW_Ucs2*) NW_Text_GetUCS2Buffer (value,
                                    NW_Text_Flags_NullTerminated | NW_Text_Flags_Aligned,
                                    NULL,
                                    &freeValue);

    /* get FORMAT attr value */
    status = NW_Wml_GetAttribute(&(formLiaison->contentHandler->wmlInterpreter),
                                elementIdAsUint16,
                                FORMAT_ATTR,
                                &format);
    if (status != KBrsrSuccess) {
      goto cleanup;
    }

    /* get EMPTYOK attr value */
    status = NW_Wml_GetAttribute(&(formLiaison->contentHandler->wmlInterpreter),
                                elementIdAsUint16,
                                EMPTYOK_ATTR,
                                &emptyok_str);
    if (status != KBrsrSuccess) {
      goto cleanup;
    }

    status = NW_Wml_GetAttribute(&(formLiaison->contentHandler->wmlInterpreter),
                                elementIdAsUint16,
                                MAXLENGTH_ATTR,
                                &maxLengthStr);
    if (status != KBrsrSuccess) {
      goto cleanup;
    }

    /* maxLengthStr set to NULL if attr not found */
    if (maxLengthStr != NULL) {
			NW_Int32 maxLength = -1; /* Initialize to invalid length */
			if(NW_Str_StrIsValidLength(maxLengthStr))
			{
				maxLength = NW_Str_Atoi(maxLengthStr);
			}
			if(maxLength < 0)
			{
				break;
			}
      else if (NW_Str_Strlen(strValue) > ((NW_Uint32)maxLength))
			{
        validateOk = NW_FALSE;
        break;
      }
    }

    emptyok = NW_WmlInput_EmptyOk_None;
    /* if no format is specified, default for format is *M and emptyok=true.  (WML-191, section 11.6.3 - emptyok) */
    if (emptyok_str == NULL && (format == NULL || *format == NULL)) 
    {
      emptyok = NW_WmlInput_EmptyOk_True;
    }
    else
    {
      if ( emptyok_str != NULL) {  
        if (NW_Str_StrcmpConst(emptyok_str, WAE_ASC_TRUE_STR) == 0) {
          emptyok = NW_WmlInput_EmptyOk_True;
        } else if (NW_Str_StrcmpConst(emptyok_str, WAE_ASC_FALSE_STR) == 0) {
          emptyok = NW_WmlInput_EmptyOk_False;
        } 
      }
    }
    

    /* get Document Public Id value */
    status = NW_Wml_GetPublicId(&(formLiaison->contentHandler->wmlInterpreter),
                                &publicId);
    if (status != KBrsrSuccess) {
      goto cleanup;
    }

    /* Do a full string validatation because this should be called after
       input dialog is closed.  jwild 26-Jan-2002. */
    validateOk = NW_WmlInput_ValidateValue(strValue,
                                           format,
                                           emptyok,
                                           NW_FALSE, /* Full string validation */
                                           publicId);

    break;

   default:
    /*
     * cannot set value; the input element is not text type.  No need to
     * warn user about invalid input.
     */
    validateOk = NW_TRUE;
    status = KBrsrFailure;
    break;
  }

  if (validateOk)
  {
    status = KBrsrSuccess;
  }
  else
  {
    status = KBrsrWmlbrowserInputNonconformingToMask;
  }


cleanup:
  if (freeValue) {
    NW_Str_Delete(strValue);
  }

  NW_Str_Delete(format);
  NW_Str_Delete(emptyok_str);
  NW_Str_Delete(maxLengthStr);

  return status;

}


//*****************************************************************
//
//  Name:         _NW_Wml1x_FormLiaison_GetStringValue()
//
//  Description:
//
//  Parameters:   out parameters: valueOut = initial value | format
//                                initialValueUsed = "value" attribute present
//
//  Algorithm:    Using the elemId, look up the name of the variable to get.
//                Use NW_Wml_GetVar() to retrieve the value of the variable.
//                Convert the value to a NW_Text_t object and set valueOut
//                to point to that object.
//
//  Return Value: KBrsrFailure if elemId not an Input element;
//                otherwise, return value from
//
//*****************************************************************/
TBrowserStatusCode
_NW_Wml1x_FormLiaison_GetStringValue (NW_FBox_FormLiaison_t  *thisObj,
                                      void                   *elemId,
                                      NW_Text_t             **valueOut,
                                      NW_Bool                *initialValueUsed)
  {
  TBrowserStatusCode             status = KBrsrSuccess;
  NW_Wml1x_FormLiaison_t  *formLiaison;
  NW_Ucs2                 *varName = NULL;
  NW_Ucs2                 *retString = NULL;
  NW_Uint32               publicId = 0;
  NW_Ucs2                 *emptyok_str  = NULL;
  NW_Ucs2                 *maxLengthStr = NULL;
  NW_WmlInput_EmptyOk_t   emptyok       = NW_WmlInput_EmptyOk_None;
  NW_Ucs2                 *format = NULL;
  NW_Ucs2                 *formatMask = NULL;
  NW_Uint16               elementIdAsUint16;

  elementIdAsUint16 = NW_UINT16_CAST ((NW_Uint32) elemId);  // NOTE: this assumes (void*) is 32 bits 

  formLiaison = NW_Wml1x_FormLiaisonOf(thisObj);
  
  NW_Object_Delete (formLiaison->lastStringValue);
  formLiaison->lastStringValue = NULL;

  switch (NW_Wml1x_FormLiaison_GetControlType(formLiaison, elemId))
  {
  case NW_Wml1x_FormControlType_InputPassword:
    // Password input boxes ignore format attribute, so there is no validation
    // to perform.
    // lookup name
    status = NW_Wml_GetAttribute (&(formLiaison->contentHandler->wmlInterpreter),
                                 elementIdAsUint16,
                                 NAME_ATTR,
                                 &varName);
    if (status == KBrsrOutOfMemory)
       {
       return status;   
       }
    
    if (varName != NULL)
       {
       // get value 
       status = NW_Wml_GetVar (&(formLiaison->contentHandler->wmlInterpreter),
                               varName,
                               &retString);

       // deal with the failure code returned by GetVar() when the variable is empty 
       if (status == KBrsrFailure)
           {
           status = KBrsrSuccess;
           }

       NW_Mem_Free (varName);
       varName = NULL;
       }

    
    if (retString != NULL)
       {   
       // get maxLengthStr value 
       status = NW_Wml_GetAttribute(&(formLiaison->contentHandler->wmlInterpreter),
                                 elementIdAsUint16,
                                 MAXLENGTH_ATTR,
                                 &maxLengthStr);
       if (status == KBrsrOutOfMemory)
           {
           return status;    
           }
      
       // maxLengthStr set to NULL if attr not found 
       if (maxLengthStr != NULL)
           {
           NW_Int32 maxLength = -1; // Initialize to invalid length
           if(NW_Str_StrIsValidLength(maxLengthStr))
               {
               maxLength = NW_Str_Atoi(maxLengthStr);
               }
           if (maxLength < 0)
               {
               NW_Mem_Free(maxLengthStr);
               break;
               }
           }    
       emptyok = NW_WmlInput_EmptyOk_True;
       }
       *initialValueUsed = (NW_Bool) ( (retString) ? NW_TRUE : NW_FALSE );
       // if value is not available, create an empty string value 
       if (retString == NULL) 
           {
           retString = NW_Str_NewcpyConst( "" );
           }
       if (retString != NULL)
           {
           // convert to text object 
           formLiaison->lastStringValue = (NW_Text_t*)
                NW_Text_UCS2_New (retString, NW_Str_Strlen(retString), NW_Text_Flags_TakeOwnership);
           if (formLiaison->lastStringValue == NULL)
               {
               NW_Mem_Free(retString);
               status = KBrsrOutOfMemory;
               }
           retString = NULL;
           }
       else
           {
           status = KBrsrOutOfMemory;
           }
       break;


  case NW_Wml1x_FormControlType_InputText:    
    // Retrieve var name, var value and format used and validate value against format.
    // lookup name 
    status = NW_Wml_GetAttribute (&(formLiaison->contentHandler->wmlInterpreter),
                                 elementIdAsUint16,
                                 NAME_ATTR,
                                 &varName);
    if (status == KBrsrOutOfMemory)
        {
        return status; 
        }
     
    // get FORMAT attr value 
    status = NW_Wml_GetAttribute(&(formLiaison->contentHandler->wmlInterpreter),
                                elementIdAsUint16,
                                FORMAT_ATTR,
                                &format);
    if (status == KBrsrOutOfMemory)
        {
        return status;     
        }
    
    if (varName != NULL)
        {
        // get value 
        status = NW_Wml_GetVar (&(formLiaison->contentHandler->wmlInterpreter),
                                varName,
                                &retString);

        // deal with the failure code returned by GetVar() when the variable is empty 
        if (status == KBrsrFailure)
            {
            status = KBrsrSuccess;
            }

        NW_Mem_Free (varName);
        varName = NULL;
        }

    
    if (retString != NULL)
       {
       // get EMPTYOK attr value 
       status = NW_Wml_GetAttribute(&(formLiaison->contentHandler->wmlInterpreter),
                                  elementIdAsUint16,
                                  EMPTYOK_ATTR,
                                  &emptyok_str);
       if (status == KBrsrOutOfMemory)
            {
            return status;    
            }
       // get Document Public Id value 
       status = NW_Wml_GetPublicId(&(formLiaison->contentHandler->wmlInterpreter),
                                  &publicId);    
       // get maxLengthStr value 
       status = NW_Wml_GetAttribute(&(formLiaison->contentHandler->wmlInterpreter),
                                  elementIdAsUint16,
                                  MAXLENGTH_ATTR,
                                  &maxLengthStr);
       if (status == KBrsrOutOfMemory)
           {
           return status;    
           }      
       // maxLengthStr set to NULL if attr not found 
       if (maxLengthStr != NULL) 
           {
				   NW_Int32 maxLength = -1; // Initialize to invalid length
				   if(NW_Str_StrIsValidLength(maxLengthStr))
				       {
					     maxLength = NW_Str_Atoi(maxLengthStr);
				       }
           if (maxLength < 0)
				       {
               break;
               }
           // to make WML and XHTML behaviour consistent, if the defined maxlength is less than the length
           //  of the value string do not display any initial string.  See the Series 60 XHTML UI SPEC,
           //  section 9.6.4 for additional information regarding the "value" and "maxlen" attribute.
           if (maxLength > NULL && maxLength < (NW_Int32)NW_Str_Strlen( retString ))
               {
               NW_Str_Delete(retString);
               retString = NULL;
               }
           }
    
       emptyok = NW_WmlInput_EmptyOk_None;
       // if no format is specified, default for format is *M and emptyok=true.  (WML-191, section 11.6.3 - emptyok) 
       if (emptyok_str == NULL && (format == NULL || *format == NULL)) 
           {
           emptyok = NW_WmlInput_EmptyOk_True;
           }
       else
           {
           if ( emptyok_str != NULL) 
               {  
               if (NW_Str_StrcmpConst(emptyok_str, WAE_ASC_TRUE_STR) == 0) 
                   {
                   emptyok = NW_WmlInput_EmptyOk_True;
                   }
               else if (NW_Str_StrcmpConst(emptyok_str, WAE_ASC_FALSE_STR) == 0) 
                   {
                   emptyok = NW_WmlInput_EmptyOk_False;
                   } 
               }
           }

       }

       *initialValueUsed = (NW_Bool) ( (retString) ? NW_TRUE : NW_FALSE );

       if ((format !=NULL) && (retString == NULL))
           {
           if ((formatMask = FormatGetMask(format)) != NULL) 
               {
               retString = formatMask;
               }
           }
       else
           {
           if ( NW_WmlInput_ValidateValue(retString, format, emptyok, NW_FALSE, publicId) == NW_FALSE) 
               {
               if (format && (formatMask = FormatGetMask(format)) != NULL) 
                   {
                   NW_Str_Delete(retString);
                   retString = formatMask;
                   }
               else
                   {
                   NW_Str_Delete(retString);
                   retString = NULL;
                   }
               }
           }
   
       // if value is not available, create an empty string value 
       if (retString == NULL) 
           {
           retString = NW_Str_NewcpyConst( "" );
           }
       if (retString != NULL)
           {
           // convert to text object 
           formLiaison->lastStringValue = (NW_Text_t*)
                NW_Text_UCS2_New (retString, NW_Str_Strlen(retString), NW_Text_Flags_TakeOwnership);
           retString = NULL;
           }
       else
           {
           status = KBrsrOutOfMemory;
           }
    break;

    default:
        //
        // cannot get value; the input element is not text type
        //
        status = KBrsrFailure;
        break;
  }

  if (valueOut != NULL)
      {
      *valueOut = formLiaison->lastStringValue;
      }

  NW_Str_Delete(retString);
  NW_Str_Delete(format);
  NW_Str_Delete(emptyok_str);
  NW_Str_Delete(maxLengthStr);

  return status;
  }

/*****************************************************************

  Name:         _NW_Wml1x_FormLiaison_GetStringName()

  Description:

  Parameters:

  Algorithm:    Using the elemId, look up the name of the variable.
                Convert the name to a NW_Text_t object and set nameOut
                to point to that object.

  Return Value: KBrsrFailure if elemId not an Input element;
                otherwise, return value from

*****************************************************************/
TBrowserStatusCode
_NW_Wml1x_FormLiaison_GetStringName(NW_FBox_FormLiaison_t  *thisObj,
                                    void                   *elemId,
                                    NW_Ucs2             **nameOut)
{
  TBrowserStatusCode             status = KBrsrSuccess;
  NW_Wml1x_FormLiaison_t  *formLiaison;
  NW_Ucs2                 *varName = NULL;
  NW_Uint16               elementIdAsUint16;

  elementIdAsUint16 = NW_UINT16_CAST ((NW_Uint32) elemId);  /* NOTE: this assumes (void*) is 32 bits */

  formLiaison = NW_Wml1x_FormLiaisonOf(thisObj);
  
  /* lookup name */
  status = NW_Wml_GetAttribute (&(formLiaison->contentHandler->wmlInterpreter),
    elementIdAsUint16,
    NAME_ATTR,
    &varName);
  
  if (varName == NULL)
  {
    status = KBrsrFailure;
  }
  *nameOut = varName;

  return status;
}

/*****************************************************************

  Name:         _NW_Wml1x_FormLiaison_GetStringTitle()

  Description:

  Parameters:

  Algorithm:    Using the elemId, look up the name of the variable.
                Convert the name to a NW_Text_t object and set nameOut
                to point to that object.

  Return Value: KBrsrFailure if elemId not an Input element;
                otherwise, return value from

*****************************************************************/
TBrowserStatusCode
_NW_Wml1x_FormLiaison_GetStringTitle(NW_FBox_FormLiaison_t  *thisObj,
                                    void                    *elemId,
                                    NW_Ucs2                **titleOut)
{
  TBrowserStatusCode             status = KBrsrSuccess;
  NW_Wml1x_FormLiaison_t  *formLiaison;
  NW_Ucs2                 *varTitle = NULL;
  NW_Uint16               elementIdAsUint16;

  elementIdAsUint16 = NW_UINT16_CAST ((NW_Uint32) elemId);  /* NOTE: this assumes (void*) is 32 bits */

  formLiaison = NW_Wml1x_FormLiaisonOf(thisObj);
  
  /* lookup name */
  status = NW_Wml_GetAttribute (&(formLiaison->contentHandler->wmlInterpreter),
    elementIdAsUint16,
    TITLE_ATTR,
    &varTitle);
  
  if (varTitle == NULL)
  {
    status = KBrsrFailure;
  }
  *titleOut = varTitle;

  return status;
}

/*****************************************************************

  Name:         _NW_Wml1x_FormLiaison_SetBoolValue()

  Description:

  Parameters:

  Algorithm:

  Return Value: KBrsrFailure if elemId not an option element;
                otherwise, return value from NW_UI_SetOptState()

*****************************************************************/
TBrowserStatusCode
_NW_Wml1x_FormLiaison_SetBoolValue (NW_FBox_FormLiaison_t  *thisObj,
                                    void                   *elemId,
                                    NW_Bool                *value)
{
  TBrowserStatusCode             status;
  NW_Wml1x_FormLiaison_t         *formLiaison;
  NW_Uint16                      elementIdAsUint16;
  NW_Wml_ElType_e                elType = UNKNOWN_ELEMENT;
  NW_Wml_Element_t               *el;
  NW_HED_DocumentRoot_t          *docRoot; 

  elementIdAsUint16 = NW_UINT16_CAST ((NW_Uint32) elemId);  /* NOTE: this assumes (void*) is 32 bits */

  formLiaison = NW_Wml1x_FormLiaisonOf(thisObj);

  switch (NW_Wml1x_FormLiaison_GetControlType(formLiaison, elemId))
  {
  case NW_Wml1x_FormControlType_Option:
    /* TODO:  is there a better way to access the setOptState() api function? */
    status = (formLiaison->contentHandler->wmlInterpreter.wml_api->display->setOptState)
                  ((formLiaison->contentHandler),
                    elementIdAsUint16,
                    *value);

    /* perform WML processing on new option value */
    status = NW_Wml_GetElementType(&(formLiaison->contentHandler->wmlInterpreter),
                                   elementIdAsUint16,
                                   &elType,
                                   &el);
    if (status == KBrsrSuccess)
    {
      status = NW_Wml_HandleOptionEvent(&(formLiaison->contentHandler->wmlInterpreter), el);
      if (status == KBrsrWmlbrowserOnPickEx)
      {
        /* an onpick event was associated with the option whose state was
        ** just changed so send a message to close the dialog where the
        ** option was changed, since its context may be no longer valid
        */
        docRoot = (NW_HED_DocumentRoot_t*)
          NW_HED_DocumentNode_GetRootNode (formLiaison->contentHandler);

        /* for ISA, does a CallClass((objref) &SelectList, PM_SELECTLIST_CLEANUP, 0, 0, 0); */
        if (docRoot->appServices->selectListApi.cleanup != NULL)
        {
          (docRoot->appServices->selectListApi.cleanup)();
        }
      }
    }

    break;

  default:
    /*
     * cannot set value; the element is not option type,
     */
    status = KBrsrFailure;
    break;
  }

  return status;
}

/*****************************************************************

  Name:         _NW_Wml1x_FormLiaison_ToggleBoolValue()

  Description:

  Parameters:

  Algorithm:

  Return Value: KBrsrFailure if elemId not an option element;
                KBrsrSuccess otherwise

*****************************************************************/
TBrowserStatusCode
_NW_Wml1x_FormLiaison_ToggleBoolValue(NW_FBox_FormLiaison_t *thisObj,
                                     void                   *elemId)
{
  NW_Bool value;
  TBrowserStatusCode status = KBrsrSuccess;

  if ((status = NW_FBox_FormLiaison_GetBoolValue(thisObj, elemId, &value)) !=
      KBrsrSuccess)
  {
    return status;
  }

  value = (NW_Bool) !value;
  status = NW_FBox_FormLiaison_SetBoolValue(thisObj,
                                            elemId,
                                            &value);

  return status;
}

/*****************************************************************

  Name:         _NW_Wml1x_FormLiaison_GetBoolValue()

  Description:

  Parameters:

  Algorithm:

  Return Value: KBrsrFailure if elemId not an option element;
                KBrsrSuccess otherwise

*****************************************************************/
TBrowserStatusCode
_NW_Wml1x_FormLiaison_GetBoolValue(NW_FBox_FormLiaison_t  *thisObj,
                                  void                    *elemId,
                                  NW_Bool                 *valueOut)
{
  TBrowserStatusCode             status = KBrsrSuccess;
  NW_Wml1x_FormLiaison_t  *formLiaison;
  NW_Uint16               elementIdAsUint16;

  elementIdAsUint16 = NW_UINT16_CAST ((NW_Uint32) elemId);  /* NOTE: this assumes (void*) is 32 bits */

  formLiaison = NW_Wml1x_FormLiaisonOf(thisObj);

  switch (NW_Wml1x_FormLiaison_GetControlType(formLiaison, elemId))
  {
  case NW_Wml1x_FormControlType_Option:
    /* TODO:  is there a better way to access the getOptState() api function? */
    status = (formLiaison->contentHandler->wmlInterpreter.wml_api->display->getOptState)
                  ((formLiaison->contentHandler),
                    elementIdAsUint16,
                    valueOut);
    break;

  default:
    status = KBrsrFailure;
    break;
  }

  return status;
}


/*****************************************************************

  Name:         _NW_Wml1x_FormLiaison_OptionHasOnPick()

  Description:

  Parameters:

  Algorithm:

  Return Value: NW_TRUE if elemId has OnPick propert;
                NW_FALSE otherwise

*****************************************************************/
NW_Bool
_NW_Wml1x_FormLiaison_OptionHasOnPick(NW_FBox_FormLiaison_t* thisObj,
                                      void* elemId)
{
  TBrowserStatusCode             status;
  NW_Wml1x_FormLiaison_t  *formLiaison;
  NW_Uint16               elementIdAsUint16;

  elementIdAsUint16 = NW_UINT16_CAST ((NW_Uint32) elemId);  /* NOTE: this assumes (void*) is 32 bits */

  formLiaison = NW_Wml1x_FormLiaisonOf(thisObj);

  NW_ASSERT(NW_Wml1x_FormLiaison_GetControlType(formLiaison, elemId) == NW_Wml1x_FormControlType_Option);

  status = NW_Wml_HasOnpick(&(formLiaison->contentHandler->wmlInterpreter), elementIdAsUint16);

  if (status == KBrsrSuccess)
  {
    return NW_TRUE;
  }

  return NW_FALSE;
}


/*****************************************************************

  Name:         _NW_Wml1x_FormLiaison_IsLocalNavOnPick()

  Description:  Determine if ONPICK attribute points to a "local"
                card within the same deck as the current card.

  Parameters:   NW_FBox_FormLiaison_t* thisObj - used to derive WML context
                void* elemId - id number of OPTION element, needed for GetAttribute
                               subroutine to obtain ONPICK attribute value.

  Algorithm:

  Return Value: NW_TRUE if elemId has OnPick property and nav is considered local;
                NW_FALSE otherwise

*****************************************************************/
NW_Bool
_NW_Wml1x_FormLiaison_IsLocalNavOnPick(NW_FBox_FormLiaison_t* thisObj,
                                      void* elemId)
{
  NW_Uint16                   elementIdAsUint16;
  NW_Ucs2                     *retString = NULL;  // returned URL
  NW_Ucs2                     *target_url = NULL;
  NW_Wml1x_FormLiaison_t      *formLiaison;
  NW_DeckDecoderIter_t        iter;
  NW_Wml_Element_t*           option_el = 0;
  NW_Wml_Element_t            new_el;
  NW_Wml_ElType_e             el_type;
  TBrowserStatusCode          status = NW_FALSE;
  NW_Wml_t*                   wmlObj = NULL;

  elementIdAsUint16 = NW_UINT16_CAST ((NW_Uint32) elemId);  /* NOTE: this assumes (void*) is 32 bits */
  formLiaison = NW_Wml1x_FormLiaisonOf(thisObj);
  wmlObj = &(formLiaison->contentHandler->wmlInterpreter);
  (void) NW_Wml_GetAttribute (wmlObj, elementIdAsUint16, ONPICK_ATTR, &retString);

  if (retString)
  {
	status = NW_Wml_GetFullUrl(wmlObj, retString, &target_url);
    NW_Mem_Free(retString);
    if (status != KBrsrSuccess && status != KBrsrFailure)
    {
      /* FAILURE does not imply a fatal error */
      return NW_FALSE;
    }
  }
  else
  {  /* if no "onpick" attr found, try for "onevent" element with "type=onpick" used in the option list */
    if ((status = NW_Wml_GetElementType(wmlObj, elementIdAsUint16, &el_type, &option_el)) != KBrsrSuccess)
	  {
         return NW_FALSE;
	  }

    /* look for a task element */
    if (NW_DeckDecoderIter_InitializeAll_Children(&iter, wmlObj->decoder, option_el) != KBrsrSuccess)
    {
	    return NW_FALSE;
    }

    while ( NW_DeckDecoderIter_GetNextElement(&iter, &new_el) )
    {
	    NW_Wml_ElType_e el_type;

	    NW_DeckDecoder_GetType(wmlObj->decoder, &new_el, &el_type);

   		if (el_type == ONEVENT_ELEMENT)
   		{
   		   if (NW_DeckDecoder_HasAttributeWithValue (wmlObj->decoder, &new_el, TYPE_ATTR,
                    WAE_ASC_ONPICK_STR, wmlObj->var_list))
   		   {
		        while ( NW_DeckDecoderIter_GetNextElement(&iter, &new_el) )
		        {
			         NW_DeckDecoder_GetType(wmlObj->decoder, &new_el, &el_type);

			         if (el_type == GO_ELEMENT)
			         {
			            break;
			         }
		        }

                /* GetHrefAttr & FetchUrl handle 12.5.1.2 */
			    status = NW_Wml_GetHrefAttr(wmlObj, &new_el, &target_url);

    		    if (el_type == GO_ELEMENT)
			      {			     
			         if (status != KBrsrSuccess && status != KBrsrFailure)
			         {
			            /* FAILURE does not imply a fatal error */
			            return NW_FALSE;
			         }
			      }
                else // finished the inner while loop without finding a GO_ELEMENT
                  {
                    if (target_url == NULL)
                    {
                        return NW_TRUE;
                    }
                    else
                    {
                        break;
                    }
                  }
		     } //end if (NW_DeckDecoder......

        }//end if (el_type == ONEVENT_ELEMENT)

    }//end while
    
  } //end else

  if (target_url != NULL)
  {
    status = NW_Wml_IsCardInCurrentDeck(wmlObj, target_url);
    NW_Mem_Free(target_url);
    
    if (status == KBrsrSuccess)
    {
      return NW_TRUE;
    }
  }

  return NW_FALSE;
}

TBrowserStatusCode
_NW_Wml1x_FormLiaison_GetDocRoot(NW_FBox_FormLiaison_t  *thisObj,
                                    NW_HED_DocumentRoot_t **docRoot)
{
  NW_Wml1x_FormLiaison_t  *wml1xFormLiaison ;
  NW_ASSERT (docRoot != NULL);
  wml1xFormLiaison = NW_Wml1x_FormLiaisonOf(thisObj);
  *docRoot =
    (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode (wml1xFormLiaison->contentHandler);
  return KBrsrSuccess;
}


TBrowserStatusCode
_NW_Wml1x_FormLiaison_DelegateEcmaEvent(NW_FBox_FormLiaison_t *thisObj,
                                        void* eventNode,
                                        NW_ECMA_Evt_Type_t ecmaEvent)

{
  NW_REQUIRED_PARAM( thisObj );
  NW_REQUIRED_PARAM( eventNode );
  NW_REQUIRED_PARAM( ecmaEvent );
  return KBrsrSuccess;
}


/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/*****************************************************************

  Name:         NW_Wml1x_FormLiaison_New()

  Description:

  Parameters:

  Algorithm:

  Return Value:

*****************************************************************/
NW_Wml1x_FormLiaison_t*
NW_Wml1x_FormLiaison_New (NW_Wml1x_ContentHandler_t *contentHandler)
{
  return (NW_Wml1x_FormLiaison_t*)
    NW_Object_New (&NW_Wml1x_FormLiaison_Class, contentHandler);
}


static NW_Ucs2* FormatGetMask(const NW_Ucs2 *format) 
{
 
  NW_Ucs2 *format_mask = NULL ;
  NW_Int32 countM=0, countF=0;
  NW_Int32 lastStatic;
  
  format_mask = NW_Str_New(NW_Str_Strlen (format));
  if (format_mask == NULL) {
    return NULL;
  }

  /* find the last static character */
  lastStatic = -1;
  countF = 0;
  while (format && format[countF] ) {
    if (format[countF] == '\\') {
      lastStatic=countF;
    }
    countF++;
  }

  /* if there are static characters - create mask */
  if (lastStatic != -1) {
    countF = 0;
    while (format && format[countF] && countF<=lastStatic) {
    if (format[countF++] == '\\') {
      format_mask[countM++] = format[countF++];
    } 
      else {
        format_mask[countM++] = ' ';
      }
  }

    format_mask[countM] = '\0';
  }
  else {
    NW_Str_Delete(format_mask);
    format_mask = NULL;
  }

  return format_mask;
}



