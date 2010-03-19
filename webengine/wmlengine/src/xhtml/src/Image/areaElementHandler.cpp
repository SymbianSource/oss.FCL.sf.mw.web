/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Element handler for the <area> tag
*
*/


// INCLUDE FILES
#include "nw_image_areaelementhandleri.h"
#include "nw_xhtml_xhtmlcontenthandler.h"
#include "nw_hed_hedeventhandler.h"
#include "nw_hed_documentroot.h"
#include "nw_evt_activateevent.h"
#include "nw_evt_accesskeyevent.h"
#include "nw_evt_focusevent.h"
#include "nw_xhtml_xhtml_1_0_tokens.h"
#include "nw_image_imgelementhandler.h"
#include "nwx_settings.h"
#include "nwx_http_defs.h"
#include "nw_lmgr_rootbox.h"
#include "nw_lmgr_areabox.h"
#include "nw_lmgr_imagemapbox.h"
//#include "nw_ecma_contenthandler.h"
#include "nw_hypertext_aelementhandler.h"
/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

// -----------------------------------------------------------------------------
const
NW_XHTML_areaElementHandler_Class_t NW_XHTML_areaElementHandler_Class =
    {
        { /* NW_Object_Core              */
            /* super                       */ &NW_XHTML_ElementHandler_Class,
            /* queryInterface              */ _NW_Object_Core_QueryInterface
        },
        { /* NW_XHTML_ElementHandler     */
            /* initialize                  */ _NW_XHTML_ElementHandler_Initialize,
            /* createBoxTree               */ _NW_XHTML_ElementHandler_CreateBoxTree,
            /* processEvent                */ _NW_XHTML_areaElementHandler_ProcessEvent
        },
        { /* NW_XHTML_areaElementHandler    */
            /* unused                      */ NW_Object_Unused
        }
    };

/* ------------------------------------------------------------------------- *
   singleton definition
 * ------------------------------------------------------------------------- */

// -----------------------------------------------------------------------------
const NW_XHTML_areaElementHandler_t NW_XHTML_areaElementHandler =
    {
        {
            {
                &NW_XHTML_areaElementHandler_Class
            }
        }
    };

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

// -----------------------------------------------------------------------------
NW_Uint8
_NW_XHTML_areaElementHandler_ProcessEvent (const NW_XHTML_ElementHandler_t* elementHandler,
        NW_XHTML_ContentHandler_t* contentHandler,
        NW_DOM_ElementNode_t* elementNode,
        NW_LMgr_Box_t* box,
        NW_Evt_Event_t* event)
    {
    NW_Text_t* url = NULL;
    TBrowserStatusCode status (KBrsrFailure);
    NW_LMgr_ImageMapBox_t* imageMapBox = NULL;
    NW_LMgr_RootBox_t* rootBox = NULL;
    NW_HED_AppServices_t* appServices = NULL;

    NW_REQUIRED_PARAM(elementHandler);
    NW_REQUIRED_PARAM(box);

    // we are really only interested in the following events
    if ((NW_Object_Core_GetClass (event) != &NW_Evt_ActivateEvent_Class) &&
        (NW_Object_Core_GetClass (event) != &NW_Evt_AccessKeyEvent_Class) &&
        (NW_Object_Core_GetClass (event) != &NW_Evt_FocusEvent_Class))
        {
        return NW_LMgr_EventNotAbsorbed;
        }

    if (NW_Object_Core_GetClass (event) == &NW_Evt_FocusEvent_Class)
        {
        return NW_LMgr_EventAbsorbed;
        }

    NW_HED_DocumentRoot_t* documentRoot = 
      (NW_HED_DocumentRoot_t*)NW_HED_DocumentNode_GetRootNode (contentHandler);


    // if OnClick attribute is present , then the event should be processed and
    // then href attribute is processed.
    //R->ecma
    //status = NW_Ecma_HandleEvent(documentRoot, elementNode, box, 
    //                            NW_Ecma_Evt_OnClick,ELinkNode);

    if (status == KBrsrSuccess)
        {
        return NW_LMgr_EventAbsorbed;
        }

    // find the 'href' attribute and get its string value
    NW_XHTML_GetDOMAttribute (contentHandler,
                              elementNode,
                              NW_XHTML_AttributeToken_href, &url);


    if (!url)
        {
        NW_XHTML_aElementHandler_getBaseTagUrl(contentHandler, &url);
        }

    if (!url)
        {
        return NW_LMgr_EventAbsorbed;
        }


    // if the script is disabled or OnClick attr is not present then status is KBrsrNotFound
    // if the url starts with javascript: then delegate the activate event to ecmaengine
    if (NW_Str_Strncmp((const NW_Ucs2*) url->storage, (const NW_Ucs2*) NW_STR("javascript:"), 11) == 0)
        {
        NW_Bool urlTextFreeNeeded;
        NW_Text_Length_t urlTextLength;
        NW_Ucs2* urlStorage;
        urlStorage = NW_Text_GetUCS2Buffer (url, 0, &urlTextLength, &urlTextFreeNeeded);
        //R->ecma
        //status = NW_Ecma_ExecuteEvent(documentRoot , elementNode, box, urlStorage , 
        //                              NW_Ecma_Evt_OnClick,ELinkNode);
        if (urlTextFreeNeeded)
            {
            NW_Mem_Free ((NW_Ucs2*) urlStorage);
            }
        }
    if(status != KBrsrSuccess)
        {
         // reset the status    
         status = KBrsrSuccess;
        // call SwitchFromImgMapViewIfNeeded in case we are accessing the area box
        // from the image map view and need to switch back to the normal view before loading
        // the new page
        imageMapBox = NW_LMgr_AreaBox_GetParentImageMapBox(box);
        NW_ASSERT(imageMapBox);

        rootBox = NW_LMgr_Box_GetRootBox(imageMapBox);
        NW_ASSERT(rootBox);

        appServices = rootBox->appServices;
        NW_ASSERT(appServices);

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
	        appServices->browserAppApi.SwitchFromImgMapViewIfNeeded();

	        // load the content
	        status = NW_XHTML_ContentHandler_StartLoad (contentHandler, NW_TextOf (url),
	                 NW_HED_UrlRequest_Reason_DocLoad, elementNode, NW_UrlRequest_Type_Any);

	        if (status != KBrsrSuccess)
	            {
	            NW_HED_DocumentNode_HandleError (contentHandler,
	                                             NW_HED_ContentHandler_GetUrlRequest (contentHandler),
	                                             BRSR_STAT_CLASS_GENERAL, (NW_WaeError_t) status);
	            }
			}// if (!newWindow)
        }

    NW_Object_Delete(url);

    // we absorbed the event!
    return NW_LMgr_EventAbsorbed;
    }


