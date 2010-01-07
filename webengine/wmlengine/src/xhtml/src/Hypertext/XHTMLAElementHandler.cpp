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


#include "nw_hypertext_aelementhandleri.h"
#include "nw_xhtml_xhtmlcontenthandler.h"
#include "nw_hed_hedeventhandler.h"
#include "nw_hed_documentroot.h"
#include "nw_lmgr_activecontainerbox.h"
#include "nw_lmgr_textbox.h"
#include "nw_evt_activateevent.h"
#include "nw_evt_accesskeyevent.h"
#include "nw_xhtml_savetophonebkevent.h"
#include "nw_evt_focusevent.h"
#include "nw_xhtml_xhtml_1_0_tokens.h"
#include "nw_xhtml_telephonyservices.h"
#include "nw_xhtml_textelementhandler.h"
#include "nw_image_imgelementhandler.h"
#include "nw_wmlscriptch_wmlscriptcontenthandler.h"  /* to run Wml Scripts */
#include "nwx_url_utils.h"
#include "nwx_http_defs.h"
//#include "nw_ecma_contenthandler.h"
#include "BrsrStatusCodes.h"
#include "nwx_logger.h"

/* ------------------------------------------------------------------------- */
static  
NW_LMgr_Box_t*
NW_XHTML_aElementHandler_ImodeCreateBox(const NW_XHTML_ElementHandler_t* elementHandler,
                                        NW_XHTML_ContentHandler_t* contentHandler,
                                        NW_DOM_ElementNode_t* elementNode)
    {
    NW_Text_t* ctiVal = NULL;
    NW_Text_t* hrefVal = NULL;
    NW_Text_t* nameVal = NULL;
    NW_LMgr_ActiveContainerBox_t* box = NULL;
    NW_Bool validNumber = NW_FALSE;
    NW_Ucs2* storage = NULL;
    NW_Uint8 freeNeeded = NW_FALSE;
    NW_Text_Length_t charCount;
    NW_XHTML_TelephonyServices_t* telServices = NULL;
    NW_HED_EventHandler_t* eventHandler = NULL;
    NW_Ucs2* newUrl = NULL;
    NW_Ucs2 *chReturns = NULL;
    NW_Bool createBox = NW_FALSE;
    NW_LMgr_ActiveContainerBox_ActionType_t actionType = NW_LMgr_ActionType_OpenLink;
    
    NW_REQUIRED_PARAM(elementHandler);
    
    telServices = (NW_XHTML_TelephonyServices_t*)(&NW_XHTML_TelephonyServices);

    // Find the 'name' attribute and get its string value. The name is used below
    // if HREF is not specified and it is also used for ECMA script processing.
    NW_XHTML_GetDOMAttribute (contentHandler, elementNode, NW_XHTML_AttributeToken_name, &nameVal);
    
    /* find the 'cti' attribute and get its string value */
    NW_XHTML_GetDOMAttribute(contentHandler, elementNode,
                             NW_XHTML_AttributeToken_cti, &ctiVal);
    if (ctiVal)
        {
        storage = (NW_Ucs2*) NW_Text_GetUCS2Buffer(ctiVal, NW_Text_Flags_Aligned,&charCount,
                                                   &freeNeeded);
        if (!storage)
            {
            goto cleanup;
            }
        
        validNumber = NW_XHTML_TelephonyServices_ParseNumber(telServices, storage, NW_TRUE);

        if (storage && freeNeeded)
            {
            NW_Str_Delete(storage);
            storage = NULL;
            }

        if (validNumber)
            {
            eventHandler = NW_HED_EventHandler_New(NW_HED_DocumentNodeOf (contentHandler), elementNode);
            box = NW_LMgr_ActiveContainerBox_New(0, NW_LMgr_EventHandlerOf (eventHandler), NW_LMgr_ActionType_DialNumber);
            if (box)
                {
                NW_LMgr_ActiveContainerBox_SetName(box, nameVal, NW_TRUE);
                nameVal = NULL; // ownership of name object was transferred to box
                }
            goto cleanup;
            }
        }

    /* find the 'href' attribute and get its string value */
    NW_XHTML_GetDOMAttribute (contentHandler, elementNode, NW_XHTML_AttributeToken_href, &hrefVal);
    if (hrefVal == NULL)
        {
        /* if href is not specified in <a>, use base tag url, unless a name attribute is specified. 
        Because if a name tag is specifed but not an href, this <a> element means to be only a fragment
        identifier. */
        if (nameVal == NULL)
            {
            NW_XHTML_aElementHandler_getBaseTagUrl (contentHandler, &hrefVal);
            }
        }

    if (hrefVal)
        {
        NW_Url_Schema_t scheme = NW_SCHEMA_INVALID;
        NW_Ucs2* number;

        storage = (NW_Ucs2*) NW_Text_GetUCS2Buffer(hrefVal, NW_Text_Flags_Aligned, &charCount,
                                                   &freeNeeded);
        if (!storage)
            {
            goto cleanup;
            }
        
        /* At this point, only interested if a scheme is present, not if the URL 
        complies with RFC2396.  If there is a problem with the URL, it will be 
        caught later, when the anchor is selected.  The philosophy is to present
        everything to the user, except scheme=NW_SCHEMA_INVALID for TEL: and MAILTO:
        */
        NW_Url_GetSchemeNoValidation(storage, &scheme);

        // remove cariage return and new line from url
        chReturns = NW_Str_New (2);
        chReturns[0] = 0x0D;
        chReturns[1] = 0x0A;
        chReturns[2] = '\0';

        NW_LOG1( NW_LOG_OFF, "before removing, storage: %s", storage );
        newUrl = NW_Str_New(NW_Url_RemoveCharsLen(storage, chReturns));
        if (!newUrl)
            {
            goto cleanup;
            }
        NW_Url_RemoveChars(storage, newUrl, chReturns);

        // Validate the TEL:, WTAI:, MAILTO: schemes, all other schemes just get created.
        // Also, set the corresponding action type for the shemes.
        if (scheme == NW_SCHEMA_TEL)
            {
            // For "TEL:" scheme, verify the number, i.e. "TEL:8005551234"
            number = newUrl + 4;
            validNumber = NW_XHTML_TelephonyServices_ParseNumber(telServices, number, NW_FALSE);
            actionType = NW_LMgr_ActionType_DialNumber;
            // Create box for the anchor <a> link if and only if "TEL:" number format is valid.
            createBox = validNumber;
            }
        else if (scheme == NW_SCHEMA_WTAI)
            {
            // For "WTAI:" scheme, verify the number, i.e. "WTAI://wp/mc;8005551234"
            number = newUrl + 13;
            validNumber = NW_XHTML_TelephonyServices_ParseNumber(telServices, number, NW_FALSE);
            actionType = NW_LMgr_ActionType_DialNumber;
            // Create the box for the anchor <a> link if and only if "WTAI://wp/mc" number format is valid.
            createBox = validNumber;
            }
        else if (scheme == NW_SCHEMA_MAILTO)
            {
            // For "MAILTO" scheme, no verification. Assume it is valid.
            actionType = NW_LMgr_ActionType_SendMail;
            // Since there is no validation, always create the box. 
            createBox = NW_TRUE;
            }
        else
            {
            // Create an anchor for all other schemas (http, https, file,
            // etc...), including:
            //  NW_SCHEMA_UNKNOWN - Unknown schemes contain a colon, but aren't
            //      in the limited supported list. They need to be displayed
            //      anyhow, because some application might support it (plugin,
            //      sms, mms, etc...).
            //  NW_SCHEMA_INVALID - Probably a relative link.
            actionType = NW_LMgr_ActionType_OpenLink;
            // Always create the box.
            createBox = NW_TRUE;
            }

        if (createBox)
            {
            // Create the box for the anchor <a> link.
            eventHandler = NW_HED_EventHandler_New (NW_HED_DocumentNodeOf (contentHandler), elementNode);
            box = NW_LMgr_ActiveContainerBox_New(0, NW_LMgr_EventHandlerOf (eventHandler), actionType);
            if (box)
                {
                NW_LMgr_ActiveContainerBox_SetName(box, nameVal, NW_TRUE);
                nameVal = NULL; // ownership of name object was transferred to box
                }
            }

        }   // end of if (hrefVal)
        
      if (box)
        {
        box->hrefAttr = NW_Text_New( HTTP_iso_10646_ucs_2, (void*)newUrl, NW_Str_Strlen(newUrl), 
                                     NW_Text_Flags_NullTerminated | NW_Text_Flags_Aligned | NW_Text_Flags_Copy );
        if (!box->hrefAttr)
            {
            NW_Object_Delete(box);
            box = NULL;
            }
        }

cleanup:
        if (hrefVal)
            {
            NW_Object_Delete(hrefVal);      
            }
        if (storage && freeNeeded)
            {
            NW_Str_Delete(storage);
            }
        if (newUrl)
            {
            NW_Str_Delete(newUrl);
            }        
        if (chReturns)
            {
            NW_Str_Delete(chReturns);
            }
        if (nameVal)
            {
            NW_Object_Delete(nameVal);
            }
        if (ctiVal)
            {
            NW_Object_Delete(ctiVal);
            }

        return (box) ? NW_LMgr_BoxOf(box) : NULL;
}

/* -------------------------------------------------------------------------
  ------------------------------------------------------------------------- */
static  
NW_LMgr_Box_t*
NW_XHTML_aElementHandler_CreateBox(const NW_XHTML_ElementHandler_t* elementHandler,
                                   NW_XHTML_ContentHandler_t* contentHandler,
                                   NW_DOM_ElementNode_t* elementNode)
{
  NW_Text_t* hrefVal = NULL;
  NW_Text_t* nameVal = NULL;
  NW_LMgr_ActiveContainerBox_t* box = NULL;
  NW_HED_EventHandler_t* eventHandler = NULL;
  NW_Ucs2* storage = NULL;
  NW_Uint8 freeNeeded = NW_FALSE;

  NW_REQUIRED_PARAM(elementHandler);

  // find the 'name' attribute.
  NW_XHTML_GetDOMAttribute (contentHandler, elementNode,
                            NW_XHTML_AttributeToken_name, &nameVal);
  
  /* find the 'href' attribute. */
  NW_XHTML_GetDOMAttribute (contentHandler, elementNode, 
                            NW_XHTML_AttributeToken_href, &hrefVal);
  if (hrefVal == NULL)
  {
    NW_XHTML_aElementHandler_getBaseTagUrl(contentHandler, &hrefVal);
  }
  if (hrefVal != NULL)
  {
    NW_Url_Schema_t scheme = NW_SCHEMA_INVALID;
    TBrowserStatusCode schemeStatus = KBrsrSuccess;
    NW_Text_Length_t charCount;
  
    /* There is an href attribute, so make it an active box. */

    storage = (NW_Ucs2*)
      NW_Text_GetUCS2Buffer (hrefVal, NW_Text_Flags_Aligned, &charCount,
                             &freeNeeded);
    if (!storage)
    {
      goto cleanup;
    }

    /* At this point, only interested if a scheme is present, not that the URL does not
       comply with RFC2396.  If there is a problem with the URL, it will be caught later
       when the anchor is selected/used.  The philosophy is to present everything to the user. */
    schemeStatus = NW_Url_GetScheme(storage, &scheme);

    if (scheme == NW_SCHEMA_INVALID && schemeStatus == KBrsrMalformedUrl)
    {
      goto cleanup;
    }

    eventHandler = 
        NW_HED_EventHandler_New (NW_HED_DocumentNodeOf (contentHandler), elementNode);
    box = NW_LMgr_ActiveContainerBox_New(0, 
                                         NW_LMgr_EventHandlerOf(eventHandler),
                                         NW_LMgr_ActionType_OpenLink);
    if (box)
    {
      NW_LMgr_ActiveContainerBox_SetName(box, nameVal, NW_TRUE);
      nameVal = NULL; // ownership of name object was transferred to box
    }
  }

cleanup:
    if (hrefVal)
        {
        NW_Object_Delete(hrefVal);      
        }
    if (storage && freeNeeded)
        {
        NW_Str_Delete(storage);
        }
    if (nameVal)
        {
        NW_Object_Delete(nameVal);
        }

    return (box) ? NW_LMgr_BoxOf(box) : NULL;
}

/* -------------------------------------------------------------------------
NW_XHTML_aElementHandler_HandleIModeEvents
------------------------------------------------------------------------- */
static  
NW_Uint8
NW_XHTML_aElementHandler_HandleIModeEvents(const NW_XHTML_ElementHandler_t* elementHandler,
                                           NW_XHTML_ContentHandler_t* contentHandler,
                                           NW_DOM_ElementNode_t* elementNode,
                                           NW_Evt_Event_t* event)
    {
    NW_Text_t* url = NULL;
    NW_Text_t* cti = NULL;
    NW_Bool isCti = NW_FALSE;
    TBrowserStatusCode status = KBrsrFailure;
    NW_Url_Schema_t scheme = NW_SCHEMA_INVALID;
    NW_Ucs2* storage = NULL;
    NW_Uint8 freeNeeded = 0;
    NW_Text_Length_t charCount = 0;
    NW_XHTML_TelephonyServices_t* telServices;
    
    NW_REQUIRED_PARAM(elementHandler);
    
    telServices = (NW_XHTML_TelephonyServices_t*)(&NW_XHTML_TelephonyServices);
    
    /* find the 'cti' attribute and get its string value */
    NW_XHTML_GetDOMAttribute (contentHandler, elementNode, 
                              NW_XHTML_AttributeToken_cti, &cti);
    
    /*In models that support this function, the cti attribute takes
    precedence over the href attribute. Models that do not support this
    tag will not understand the cti attribute and will therefore use the
    href attribute. */
    if (cti == NULL)
        {
        /* find the 'href' attribute and get its string value */
        NW_XHTML_GetDOMAttribute (contentHandler, elementNode, 
                                  NW_XHTML_AttributeToken_href, &url);
        if (url == NULL)
            {
            NW_XHTML_aElementHandler_getBaseTagUrl(contentHandler, &url);
            }
        if (url == NULL)
            {
            return NW_LMgr_EventAbsorbed;
            }
        storage = (NW_Ucs2*)NW_Text_GetUCS2Buffer(url, NW_Text_Flags_Aligned,
                                                  &charCount, &freeNeeded);
        NW_Url_GetSchemeNoValidation(storage, &scheme);
        }
    
    /* handle Save To Phonebook event */
    if (NW_Object_Core_GetClass (event) == &NW_XHTML_SaveToPhoneBkEvent_Class)
        {
        if (scheme == NW_SCHEMA_MAILTO)
            {
            NW_Text_t* email = NULL;
            NW_Text_t* name = NULL;
            NW_XHTML_GetDOMAttribute (contentHandler, elementNode, 
                                      NW_XHTML_AttributeToken_email, &email);
            NW_XHTML_GetDOMAttribute (contentHandler, elementNode, 
                                      NW_XHTML_AttributeToken_memoryname, &name);
            
            if (email == NULL)
                {
                status = NW_XHTML_TelephonyServices_Mailto_SaveToPhoneBook(telServices, contentHandler,
                                                                           url, name, NW_FALSE);
                }
            else
                {
                status = NW_XHTML_TelephonyServices_Mailto_SaveToPhoneBook(telServices, contentHandler,
                                                                           email, name, NW_TRUE);
                }
            NW_Object_Delete(url);
            NW_Object_Delete(email);
            NW_Object_Delete(name);

            return NW_LMgr_EventAbsorbed;
            }
        else
            /*if (NW_Object_Core_GetClass (event) == &NW_XHTML_SaveToPhoneBkEvent_Class)*/
            {
            NW_Text_t* name = NULL;
            NW_Text_t* email = NULL;
            
            status = NW_XHTML_GetDOMAttribute(contentHandler, elementNode, 
                                              NW_XHTML_AttributeToken_telbook, &name);
            
            if (status != KBrsrSuccess) 
                {
                NW_XHTML_GetDOMAttribute(contentHandler, elementNode, 
                                         NW_XHTML_AttributeToken_memoryname, &name);
                }
            
            NW_XHTML_GetDOMAttribute(contentHandler, elementNode, 
                                     NW_XHTML_AttributeToken_email, &email);
            if (cti != NULL)
                {
                isCti = NW_TRUE;
                status = NW_XHTML_TelephonyServices_SaveToPhoneBook(telServices, contentHandler,
                                                                    cti, name, email, isCti);
                }
            else
                {
                status = NW_XHTML_TelephonyServices_SaveToPhoneBook(telServices, contentHandler,
                                                                    url, name, email, isCti);
                }
            
            NW_Object_Delete(cti);
            NW_Object_Delete(url);
            NW_Object_Delete(name);
            NW_Object_Delete(email);

            return NW_LMgr_EventAbsorbed;
            }
        }
    
    if (freeNeeded) 
        {
        NW_Str_Delete(storage);
        }
    
    /* handle ActiveEvent/AccessKeyEvent */
    if (cti != NULL) /* if cti attribute is specified use cti to make call */
        {
        NW_Object_Delete(url);   /* cti will be used.  url is not needed. */
        url = NULL;
        
        status = NW_XHTML_TelephonyServices_MakeCall(telServices, contentHandler, cti, NW_TRUE);
        if (status != KBrsrSuccess) 
            {
            NW_HED_DocumentNode_HandleError(contentHandler,
                                            NW_HED_ContentHandler_GetUrlRequest(contentHandler), 
                                            BRSR_STAT_CLASS_GENERAL, (NW_WaeError_t)status);                                  
            }
        NW_Object_Delete(cti);

        return NW_LMgr_EventAbsorbed;
        }
    
    /* handle tel:// scheme */
    if (scheme == NW_SCHEMA_TEL)
        {
        status = NW_XHTML_TelephonyServices_MakeCall(telServices, contentHandler, url, NW_FALSE);
        if (status != KBrsrSuccess)
            {
            NW_HED_DocumentNode_HandleError(contentHandler,
                                            NW_HED_ContentHandler_GetUrlRequest(contentHandler), 
                                            BRSR_STAT_CLASS_GENERAL, (NW_WaeError_t)status);                                  
            }
        NW_Object_Delete(url);

        return NW_LMgr_EventAbsorbed;
        }
    
    NW_Object_Delete(url);

    return NW_LMgr_EventNotAbsorbed;
}

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_XHTML_aElementHandler_Class_t NW_XHTML_aElementHandler_Class = {
  { /* NW_Object_Core              */
    /* super                       */ &NW_XHTML_ElementHandler_Class,
    /* queryInterface              */ _NW_Object_Core_QueryInterface
  },
  { /* NW_XHTML_ElementHandler     */
    /* initialize                  */ _NW_XHTML_aElementHandler_Initialize,
    /* createBoxTree               */ _NW_XHTML_aElementHandler_CreateBoxTree,
    /* processEvent                */ _NW_XHTML_aElementHandler_ProcessEvent
  },
  { /* NW_XHTML_aElementHandler    */
    /* unused                      */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   singleton definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_XHTML_aElementHandler_t NW_XHTML_aElementHandler = {
  { { &NW_XHTML_aElementHandler_Class } }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_aElementHandler_Initialize (const NW_XHTML_ElementHandler_t* elementHandler,
                                          NW_XHTML_ContentHandler_t* contentHandler,
                                          NW_DOM_ElementNode_t* elementNode)
{
  NW_REQUIRED_PARAM(elementHandler);

  // special handling if a is the last part of the chunk: 
  // leave select to be processed in the next chunk
  // in case this is the last chunk, we need to process a in this chunk
  NW_XHTML_ElementHandler_NextSiblingExists (contentHandler, elementNode, NW_TRUE);

  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_aElementHandler_CreateBoxTree (const NW_XHTML_ElementHandler_t* elementHandler,
                                         NW_XHTML_ContentHandler_t* contentHandler,
                                         NW_DOM_ElementNode_t* elementNode,
                                         NW_LMgr_ContainerBox_t* parentBox)
{
  NW_LMgr_Box_t* box = NULL;
  NW_Text_t* url = NULL;
  NW_LMgr_TextBox_t* textBox = NULL;

  NW_TRY (status) 
  {
    // special handling if a is the last node of the chunck: 
    // leave it to be processed in the next chunck
    if (!NW_XHTML_ElementHandler_NextSiblingExists (contentHandler, elementNode, NW_FALSE))
    {
      NW_THROW_SUCCESS (status);
    }
    
    if ( NW_XHTML_ContentHandler_GetHtmlSubset(contentHandler) == 
        NW_XHTML_CONTENTHANDLER_IMODEHTMLEXT)
    {
      box = NW_XHTML_aElementHandler_ImodeCreateBox(elementHandler, contentHandler,
                                                    elementNode);
    }
    else
    {
      box = NW_XHTML_aElementHandler_CreateBox(elementHandler, contentHandler,
                        elementNode);
    }
    if (box == NULL)
    {
      /* either tel was invalid or there was no href attribute */
      box = (NW_LMgr_Box_t*)NW_LMgr_ContainerBox_New(0);
      NW_THROW_OOM_ON_NULL (box, status);
    }

      /* Add the box to the tree */
    status = NW_LMgr_ContainerBox_AddChild (parentBox, box);
    _NW_THROW_ON_ERROR (status);

    /* apply common attributes and styles */
    status = NW_XHTML_ElementHandler_ApplyStyles (elementHandler, contentHandler, elementNode, &box, NULL);

    NW_THROW_ON (status, KBrsrOutOfMemory);

    // Apply Styles will take care of replacing container box if display was block, list-item or marquee.
    // In case of 'display:none' or an error it removes the container box. That's why we check box
    if (box == NULL)
    {
      NW_THROW_SUCCESS (status);
    }

    parentBox = NW_LMgr_ContainerBoxOf (box);

    /* invoke our superclass for completion */
    status = _NW_XHTML_ElementHandler_CreateBoxTree(elementHandler, contentHandler, elementNode, parentBox);

    // if the <a href="foo.html"></a> is empty, then add the href as link text.
    if( parentBox && NW_Object_IsInstanceOf( parentBox, &NW_LMgr_ActiveContainerBox_Class ) )
      {
      // child count = 0 means that we have not added any textbox to this container yet. add the href then.
      if( NW_LMgr_ContainerBox_GetChildCount( parentBox ) == 0 && NW_LMgr_ActiveContainerBoxOf( parentBox )->hrefAttr )
        {
        url = NW_Text_Copy( NW_LMgr_ActiveContainerBoxOf( parentBox )->hrefAttr, NW_TRUE );
        NW_THROW_OOM_ON_NULL( url, status );
        //
        // create the TextBox and insert it into our parent 
        textBox = (NW_LMgr_TextBox_t*)NW_LMgr_TextBox_New( 0, url );
        NW_THROW_OOM_ON_NULL( textBox, status );
        url = NULL;

        status = NW_LMgr_ContainerBox_AddChild( parentBox, NW_LMgr_BoxOf( textBox ) );
        _NW_THROW_ON_ERROR( status );
        textBox = NULL;
        }
      }
    }
  NW_CATCH (status) 
  {
    if(box) 
    {
      NW_Object_Delete(box);
    }
    NW_Object_Delete( url );
    NW_Object_Delete( textBox );
  }
  NW_FINALLY 
  {
   return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */

NW_Uint8
_NW_XHTML_aElementHandler_ProcessEvent (const NW_XHTML_ElementHandler_t* elementHandler,
                                        NW_XHTML_ContentHandler_t* contentHandler,
                                        NW_DOM_ElementNode_t* elementNode,
                                        NW_LMgr_Box_t* box,
                                        NW_Evt_Event_t* event)
{
  NW_Text_t* url = NULL;
  TBrowserStatusCode status = KBrsrNotFound;
  NW_HED_DocumentRoot_t* documentRoot = 
    (NW_HED_DocumentRoot_t*)NW_HED_DocumentNode_GetRootNode (contentHandler);

  NW_REQUIRED_PARAM(box);

  /* we are really only interested in the following list of events */
  if ((NW_Object_Core_GetClass (event) != &NW_Evt_ActivateEvent_Class) &&
      (NW_Object_Core_GetClass (event) != &NW_Evt_AccessKeyEvent_Class) &&
      (NW_Object_Core_GetClass (event) != &NW_Evt_FocusEvent_Class) &&
      (NW_Object_Core_GetClass (event) != &NW_XHTML_SaveToPhoneBkEvent_Class))
  {
    return NW_LMgr_EventNotAbsorbed;
  }

  if(NW_Object_Core_GetClass (event) == &NW_Evt_FocusEvent_Class) {
    return NW_LMgr_EventAbsorbed;
  }

  /* Process IMODE events */
  if (NW_XHTML_ContentHandler_GetHtmlSubset(contentHandler) == 
       NW_XHTML_CONTENTHANDLER_IMODEHTMLEXT)
  {
    if(NW_XHTML_aElementHandler_HandleIModeEvents(elementHandler, contentHandler, 
                                               elementNode, event) == NW_LMgr_EventAbsorbed)
      return NW_LMgr_EventAbsorbed;
  }

  if(NW_LMgr_ActiveContainerBoxOf(box)->hrefAttr != NULL)
  {
    url = NW_Text_Copy(NW_LMgr_ActiveContainerBoxOf(box)->hrefAttr, NW_TRUE);
  }
  else
  {
    /* find the 'href' attribute and get its string value */
    NW_XHTML_GetDOMAttribute (contentHandler, 
                                  elementNode, 
                                  NW_XHTML_AttributeToken_href, &url);
  }

  if (url == NULL)
  {
    NW_XHTML_aElementHandler_getBaseTagUrl(contentHandler, &url);
  }
  if (url == NULL) {
    return NW_LMgr_EventAbsorbed;
  }


  if(status == KBrsrNotFound)
  {
    // if the url starts with javascript: then delegate the activate event to ecmaengine
    if(NW_Str_Strnicmp((const NW_Ucs2*) url->storage, (const NW_Ucs2*) NW_STR("javascript:"), 11) == 0)     {
      NW_Bool urlTextFreeNeeded;
      NW_Text_Length_t urlTextLength;
      NW_Ucs2* urlStorage;
      urlStorage = NW_Text_GetUCS2Buffer (url, 0, &urlTextLength, &urlTextFreeNeeded);
      if(urlTextFreeNeeded){
        NW_Mem_Free ((NW_Ucs2*) urlStorage);
      }                       
    }
    else
        {
#if !defined(__SERIES60_28__) && !defined(__SERIES60_27__)
        NW_Text_t* target = NULL;
        NW_XHTML_GetDOMAttribute (contentHandler, elementNode, NW_XHTML_AttributeToken_target, &target);
        TBool newWindow = EFalse;
		if (target != NULL && target->characterCount == 0)
        	{
        	NW_Object_Delete(target);
        	target = NULL;
        	}
        if (target == NULL)
        	{
        	NW_XHTML_ContentHandler_GetBaseTagTarget (contentHandler, &target);
        	}
        if (target != NULL)
            {
            // If the target attribute exists, and it is not an empty string, and 
            // it is not "_self", open the url in a new window
            NW_Bool targetTextFreeNeeded = NW_FALSE;;
            NW_Ucs2* targetStorage;
            NW_Text_Length_t targetTextLength;
            _LIT(KSelf, "_self");
            targetStorage = NW_Text_GetUCS2Buffer (target, NW_Text_Flags_NullTerminated,
                &targetTextLength, &targetTextFreeNeeded);
            // Check that the target attribute is not empty
            TPtrC targetPtr(targetStorage, targetTextLength);
            if (targetTextLength > 0 && targetPtr.CompareF(KSelf) != 0)
                {
                status = NW_XHTML_ContentHandler_WindowOpen(contentHandler, url, target);
                newWindow = ETrue;
                }
            if (targetTextFreeNeeded)
                {
                NW_Mem_Free(targetStorage);
                }
            NW_Object_Delete(target);
            }
        if (!newWindow)
#endif
            {
              // copy the Url on to the content handler 
              NW_XHTML_ContentHandler_SetReferrerUrl(contentHandler);
              // mailto and wtai scheme are now being processed by the default scheme dispatcher
              /* load the content */
              status = NW_XHTML_ContentHandler_StartLoad (contentHandler, NW_TextOf (url), 
              NW_HED_UrlRequest_Reason_DocLoad, (void *) &NW_WmlScript_ContentHandler_Class, 
              NW_UrlRequest_Type_Any);
            }
        }
  }

  NW_Object_Delete(url);

  if (status != KBrsrSuccess && status != KBrsrHedContentDispatched) {
    NW_HED_DocumentNode_HandleError (contentHandler,
        NW_HED_ContentHandler_GetUrlRequest (contentHandler), 
        BRSR_STAT_CLASS_GENERAL, (NW_WaeError_t) status);
  }

  /* we absorbed the event! */
  return NW_LMgr_EventAbsorbed;
}

/*
 Get the URL in base tag defined in header, this value can be used to combine with 
 relative URL to figure out the absolute URL. 
*/
TBrowserStatusCode
NW_XHTML_aElementHandler_getBaseTagUrl (NW_XHTML_ContentHandler_t* contentHandler,
                                          NW_Text_t** hrefVal)
{
  NW_DOM_Node_t* baseElement;
  NW_String_t href;

  NW_ASSERT (hrefVal);
  
  /* get the base url as found in the "base" element */
  baseElement = NW_HED_DomHelper_FindElement (&contentHandler->domHelper, NW_HED_DomTree_GetRootNode (
    contentHandler->domTree), 3, NW_XHTML_ElementToken_base, 0, NULL);
  
  if (baseElement != NULL) {
    TBrowserStatusCode status = NW_HED_DomHelper_GetAttributeValue (&contentHandler->domHelper, baseElement, 
      NW_XHTML_AttributeToken_href, &href);
    
    if (status == KBrsrSuccess) {
      *hrefVal = (NW_Text_t*) NW_Text_UCS2_New (href.storage, 0, NW_Text_Flags_TakeOwnership);
    }
  } 
  return KBrsrSuccess;
}
