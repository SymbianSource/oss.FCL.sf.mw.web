/*
* Copyright (c) 2000 - 2003 Nokia Corporation and/or its subsidiary(-ies).
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

#include "nw_frame_frameelementhandleri.h"
#include "nw_xhtml_xhtmlcontenthandler.h"
#include "nw_hed_hedeventhandler.h"
#include "nw_hed_documentroot.h"
#include "nw_lmgr_activecontainerbox.h"
#include "nw_lmgr_textbox.h"
#include "nw_lmgr_breakbox.h"
#include "nw_evt_activateevent.h"
#include "nw_evt_focusevent.h"
#include "nw_xhtml_xhtml_1_0_tokens.h"
#include "nwx_url_utils.h"
#include "nwx_settings.h"
#include "nwx_http_defs.h"

/* ------------------------------------------------------------------------- *
   private method
 * ------------------------------------------------------------------------- */

 /*
 Get the URL in base tag defined in header, this value can be used to combine with 
 relative URL to figure out the absolute URL. 
*/
static TBrowserStatusCode
NW_XHTML_frameElementHandler_getBaseTagUrl (NW_XHTML_ContentHandler_t* contentHandler,
                                          NW_Text_t** srcVal);

/* ------------------------------------------------------------------------- */
static  
NW_LMgr_Box_t*
NW_XHTML_frameElementHandler_ImodeCreateBox(const NW_XHTML_ElementHandler_t* elementHandler,
                                        NW_XHTML_ContentHandler_t* contentHandler,
                                        NW_DOM_ElementNode_t* elementNode)
{
  NW_Text_t* srcVal = NULL;
  NW_LMgr_ActiveContainerBox_t* box = NULL;
  NW_Ucs2* storage = NULL;
  NW_Uint8 freeNeeded;
  NW_Text_Length_t charCount;
  NW_HED_EventHandler_t* eventHandler = NULL;

  NW_REQUIRED_PARAM(elementHandler);

  // find the 'src' attribute and get its string value
  if (NW_DOM_ElementNode_getTagToken(elementNode) == NW_XHTML_ElementToken_frame)
  {
    NW_XHTML_GetDOMAttribute (contentHandler, 
                              elementNode, 
                              NW_XHTML_AttributeToken_src, &srcVal);
  
    // if there is no 'src' value, see if an 'href' value is specified
    if (!srcVal)
    {
      NW_XHTML_GetDOMAttribute (contentHandler, 
                                elementNode, 
                                NW_XHTML_AttributeToken_href, &srcVal);
    }
     
    if (!srcVal)
    { 
      NW_XHTML_frameElementHandler_getBaseTagUrl (contentHandler, &srcVal);
    }
  }

  if (srcVal)
  {
    NW_Ucs4 character;
    NW_Url_Schema_t scheme = NW_SCHEMA_INVALID;
    TBrowserStatusCode schemeStatus = KBrsrSuccess;
    
    storage = (NW_Ucs2*)
      NW_Text_GetUCS2Buffer (srcVal, NW_Text_Flags_Aligned, &charCount,
                             &freeNeeded);

    if (!storage ) 
    {
      NW_Object_Delete(srcVal);
      return NULL;
    }

    // we do not want to handle frame links within a page (i.e. src values that start with '#')
    character = NW_Text_GetAt( srcVal, 0 );

    if( character == '#' ) 
    {
        NW_Object_Delete(srcVal);
        if (freeNeeded){
          NW_Str_Delete(storage);
        }
      return NULL;
    }

    /* At this point, only interested if there is a scheme present.  Philosophy is
       to present EVERYTHING to the user - except bad TEL/MAILTO links.  If there is a problem
       with a URL, it should be caught later when the anchor is selected/used. */
    schemeStatus = NW_Url_GetScheme(storage, &scheme);

    /* We don't want to display bad links, but other schemes are okay. */
    if (schemeStatus == KBrsrMalformedUrl) 
    {
      box = NULL;
    }
    else
    {
      eventHandler = 
        NW_HED_EventHandler_New (NW_HED_DocumentNodeOf (contentHandler), elementNode);
      box = NW_LMgr_ActiveContainerBox_New(
          0, NW_LMgr_EventHandlerOf (eventHandler), NW_LMgr_ActionType_OpenLink);
    }

    /* If elementNode has no children, then insert a box for the link text from the url. */
    if (box && (NW_DOM_Node_getFirstChild(elementNode) == NULL)) 
    {
      TBrowserStatusCode status; 
      NW_Text_t* linkText = NULL;
      NW_Text_t* text = NULL;
      NW_LMgr_TextBox_t* linkTextBox;
      NW_LMgr_Box_t* breakBox = NULL;

      NW_Uint16 lineTextArray[4] = 
      { NW_XHTML_AttributeToken_id,
        NW_XHTML_AttributeToken_name,
        NW_XHTML_AttributeToken_src,
        NW_XHTML_AttributeToken_title
      };

      /* For frames, link text needs to be taken from either "id", 
         "name" or "src" attributes, in this order of priority */
      NW_Uint8 i;

      for (i = 0; i < 4; i++)
      {
        NW_XHTML_GetDOMAttribute (contentHandler, 
                                  elementNode, 
                                  lineTextArray[i], 
                                  &text);
        // ignore empty text too
        if( text && text->characterCount > 0 )
        { 
          break;
        }
      }
         
      if (!text)
      {
        NW_Object_Delete(srcVal);
        if (freeNeeded){
          NW_Str_Delete(storage);
        }
        return NULL;
      }

      linkText = NW_Text_Copy(text, NW_TRUE);  /* deep copy */
      NW_Object_Delete(text);
      text = NULL;
      linkTextBox = NW_LMgr_TextBox_New (0, linkText);
      status = NW_LMgr_ContainerBox_AddChild (NW_LMgr_ContainerBoxOf(box), NW_LMgr_BoxOf(linkTextBox));
      if (status != KBrsrSuccess)
      {
        NW_Object_Delete(linkTextBox);    
      }

      breakBox = (NW_LMgr_Box_t*)NW_LMgr_BreakBox_New ((NW_ADT_Vector_Metric_t)1);
      status = NW_LMgr_ContainerBox_AddChild (NW_LMgr_ContainerBoxOf(box), NW_LMgr_BoxOf(breakBox));
 
      if (status != KBrsrSuccess)
      {
        NW_Object_Delete(breakBox);    
      }    
    }

    NW_Object_Delete(srcVal);
    if (freeNeeded){
      NW_Str_Delete(storage);
    }
  }
  if (!box){
    return NULL;
  }
  return NW_LMgr_BoxOf(box);
}

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_XHTML_frameElementHandler_Class_t NW_XHTML_frameElementHandler_Class = {
  { /* NW_Object_Core              */
    /* super                       */ &NW_XHTML_ElementHandler_Class,
    /* queryInterface              */ _NW_Object_Core_QueryInterface
  },
  { /* NW_XHTML_ElementHandler     */
    /* initialize                  */ _NW_XHTML_ElementHandler_Initialize,
    /* createBoxTree               */ _NW_XHTML_frameElementHandler_CreateBoxTree,
    /* processEvent                */ _NW_XHTML_frameElementHandler_ProcessEvent
  },
  { /* NW_XHTML_frameElementHandler    */
    /* unused                      */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   singleton definition
 * ------------------------------------------------------------------------- */

const NW_XHTML_frameElementHandler_t NW_XHTML_frameElementHandler = {
  { { &NW_XHTML_frameElementHandler_Class } }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_frameElementHandler_CreateBoxTree (const NW_XHTML_ElementHandler_t* elementHandler,
                                             NW_XHTML_ContentHandler_t* contentHandler,
                                             NW_DOM_ElementNode_t* elementNode,
                                             NW_LMgr_ContainerBox_t* parentBox)
{
  NW_LMgr_Box_t* box = NULL;
  
  NW_TRY (status) 
  {
    box = NW_XHTML_frameElementHandler_ImodeCreateBox(elementHandler, contentHandler, elementNode);

    if (!box)
    {
      // There was no src attribute, so just return
      NW_THROW_SUCCESS(status);
    }

    // Add the box to the tree 
    status = NW_LMgr_ContainerBox_AddChild (parentBox, box);
    _NW_THROW_ON_ERROR (status);
  
    // Apply common attributes and styles 
    status = NW_XHTML_ElementHandler_ApplyStyles (elementHandler, contentHandler, elementNode, &box, NULL);
    NW_THROW_ON (status, KBrsrOutOfMemory);
  
    // Apply Styles will take care of replacing container box if display was block, list-item or marquee.
    // In case of 'display:none' or an error it removes the container box. That's why we check box
    if (!box)
    {
      NW_THROW_SUCCESS(status);
    }

    parentBox = NW_LMgr_ContainerBoxOf (box);

    // Invoke our superclass for completion
    status = _NW_XHTML_ElementHandler_CreateBoxTree(elementHandler, contentHandler, elementNode, parentBox);
   }
   NW_CATCH (status) 
   {
     if(box) 
     {
        NW_Object_Delete(box);
     }
   }
   NW_FINALLY 
   {
     return status;
   } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
NW_Uint8
_NW_XHTML_frameElementHandler_ProcessEvent (const NW_XHTML_ElementHandler_t* elementHandler,
                                            NW_XHTML_ContentHandler_t* contentHandler,
                                            NW_DOM_ElementNode_t* elementNode,
                                            NW_LMgr_Box_t* box,
                                            NW_Evt_Event_t* event)
{
  NW_Text_t* url = NULL;
  TBrowserStatusCode status;

  NW_REQUIRED_PARAM(elementHandler);
  NW_REQUIRED_PARAM(box);

  // we are really only interested in the following events
  if ((NW_Object_Core_GetClass (event) != &NW_Evt_ActivateEvent_Class) &&
      (NW_Object_Core_GetClass (event) != &NW_Evt_FocusEvent_Class))
  {
    return NW_LMgr_EventNotAbsorbed;
  }

  if(NW_Object_Core_GetClass (event) == &NW_Evt_FocusEvent_Class) 
  {
    return NW_LMgr_EventAbsorbed;
  }
  
  // find the 'src' attribute and get its string value
  NW_XHTML_GetDOMAttribute (contentHandler, 
                            elementNode, 
                            NW_XHTML_AttributeToken_src, &url);

  // if there is no 'src' value, see if an 'href' value is specified
  if (!url)
  {
    NW_XHTML_GetDOMAttribute (contentHandler, 
                              elementNode, 
                              NW_XHTML_AttributeToken_href, &url);
  }
   
  if (!url)
  { 
    NW_XHTML_frameElementHandler_getBaseTagUrl (contentHandler, &url);
  }
 

  if (!url) 
  {
    return NW_LMgr_EventAbsorbed;
  }

  // load the content
  status = NW_XHTML_ContentHandler_StartLoad (contentHandler, NW_TextOf (url), 
      NW_HED_UrlRequest_Reason_DocLoad, elementNode, NW_UrlRequest_Type_Any);

  NW_Object_Delete(url);

  if (status != KBrsrSuccess) 
  {
    NW_HED_DocumentNode_HandleError (contentHandler,
        NW_HED_ContentHandler_GetUrlRequest (contentHandler), 
        BRSR_STAT_CLASS_GENERAL, (NW_WaeError_t) status);
  }

  // we absorbed the event!
  return NW_LMgr_EventAbsorbed;
}

//
// Get the URL in base tag defined in header, this value can be used to combine with 
// relative URL to figure out the absolute URL. 
//
static TBrowserStatusCode
NW_XHTML_frameElementHandler_getBaseTagUrl (NW_XHTML_ContentHandler_t* contentHandler,
                                          NW_Text_t** srcVal)
{
  NW_DOM_Node_t* baseElement;
  NW_String_t href;

  NW_ASSERT (srcVal);
  
  // get the base url as found in the "base" element
  baseElement = NW_HED_DomHelper_FindElement (&contentHandler->domHelper, NW_HED_DomTree_GetRootNode (
    contentHandler->domTree), 3, NW_XHTML_ElementToken_base, 0, NULL);
  
  if (baseElement) 
  {
    TBrowserStatusCode status = NW_HED_DomHelper_GetAttributeValue (&contentHandler->domHelper, baseElement, 
      NW_XHTML_AttributeToken_href, &href);
    
    if (status == KBrsrSuccess) 
    {
      *srcVal = (NW_Text_t*) NW_Text_UCS2_New (href.storage, 0, NW_Text_Flags_TakeOwnership);
    }
  } 
  return KBrsrSuccess;
}
