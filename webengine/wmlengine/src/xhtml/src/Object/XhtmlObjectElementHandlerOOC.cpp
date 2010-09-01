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
* Description:  Element handler for object.  This file contains two approaches to 
*                handle object tags, via the Netscape plugin interface and extern
*				 applications.
*
*/


// INCLUDE FILES

#include <e32std.h>

#include "BrsrStatusCodes.h"
#include "nw_xhtml_xhtmlcontenthandler.h"
#include "XhtmlObjectElementHandlerOOCi.h"
#include "XhtmlObjectElementHandler.h"

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_XHTML_ObjectElementHandler_CreateImpl(const NW_XHTML_ElementHandler_t* aElementHandler,
        NW_XHTML_ContentHandler_t* aContentHandler, NW_DOM_ElementNode_t* aElementNode)
    {
    TBrowserStatusCode      status;
    CXHTMLObjectElementHandler* handler = NULL;

    NW_ASSERT(aElementHandler != NULL);
    NW_ASSERT(aContentHandler != NULL);
    NW_ASSERT(aElementNode != NULL);

    // Create the ObjectElementHandler instance
    TRAPD(err, handler = CXHTMLObjectElementHandler::NewL(aElementHandler, 
            aContentHandler, aElementNode));

    if (err != KErrNone) 
        {
        return err;
        }

    // Associate the handler with the aElementNode.
    status = NW_XHTML_ContentHandler_SetElementHandlerInstance(aContentHandler,
            aElementNode, handler);

    if (status != KBrsrSuccess)
        {
        delete handler;
        }

    return status;
    }
        
/* ------------------------------------------------------------------------- */
CXHTMLObjectElementHandler*
NW_XHTML_ObjectElementHandler_GetImpl(NW_XHTML_ContentHandler_t* aContentHandler,
        NW_DOM_ElementNode_t* aElementNode)
    {
    CXHTMLObjectElementHandler*  handler = NULL;
    NW_DOM_Node_t*               node = NULL;

    NW_ASSERT(aContentHandler != NULL);
    NW_ASSERT(aElementNode != NULL);

    node = aElementNode;
    do
        {
        // Get the instance associated with the node
        handler = (CXHTMLObjectElementHandler*) NW_XHTML_ContentHandler_GetElementHandlerInstance(
                aContentHandler, node);

        if (handler == NULL)
          {
          node = NW_DOM_Node_getParentNode(node);

          // Disregard the parent unless its a object element.
          if (node != NULL)
            {
            NW_Uint16  token;

            token = NW_HED_DomHelper_GetElementToken(node);
            if (token != NW_XHTML_ElementToken_object)
              {
              node = NULL;
              }
            }
          }
        } while ((handler == NULL) && (node != NULL));

    return handler;
    }

/* ------------------------------------------------------------------------- *
    class definition
* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_XHTML_ObjectElementHandler_Class_t NW_XHTML_ObjectElementHandler_Class = 
{
    { /* NW_Object_Core                */
        /* super                       */ &NW_XHTML_ElementHandler_Class,
        /* queryInterface              */ _NW_Object_Core_QueryInterface
    },
    { /* NW_XHTML_ElementHandler       */
        /* initialize                  */ _NW_XHTML_ObjectElementHandler_Initialize,   
        /* createBoxTree               */ _NW_XHTML_ObjectElementHandler_CreateBoxTree,                                   
        /* processEvent                */ _NW_XHTML_ObjectElementHandler_ProcessEvent
    },
    { /* NW_XHTML_ObjectElementHandler */
        /* unused                      */ NW_Object_Unused
    }
};

/* ------------------------------------------------------------------------- *
    singleton definition
* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_XHTML_ObjectElementHandler_t NW_XHTML_ObjectElementHandler = 
{
    { { &NW_XHTML_ObjectElementHandler_Class } }
};

/* ------------------------------------------------------------------------- *
    virtual methods
* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_ObjectElementHandler_Initialize (const NW_XHTML_ElementHandler_t* aElementHandler,
        NW_XHTML_ContentHandler_t* aContentHandler, NW_DOM_ElementNode_t* aElementNode)
    {
    TBrowserStatusCode status = KBrsrSuccess;

    NW_ASSERT(aElementHandler != NULL);
    NW_ASSERT(aContentHandler != NULL);
    NW_ASSERT(aElementNode != NULL);

    // Create the ObjectElementHandler instance.
    if (!NW_XHTML_ObjectElementHandler_GetImpl(aContentHandler, aElementNode))
        {
        status = NW_XHTML_ObjectElementHandler_CreateImpl(aElementHandler, aContentHandler, 
		    	aElementNode);

        if (status != KBrsrSuccess) 
            {
            return status;
            }
        }

    // Pass it on to the implementation
    CXHTMLObjectElementHandler* imp;

    imp = NW_XHTML_ObjectElementHandler_GetImpl(aContentHandler, aElementNode);
    NW_ASSERT(imp != NULL);

    return imp->Initialize();
    }

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_ObjectElementHandler_CreateBoxTree (const NW_XHTML_ElementHandler_t* aElementHandler,
    NW_XHTML_ContentHandler_t* aContentHandler, NW_DOM_ElementNode_t* aElementNode,
    NW_LMgr_ContainerBox_t* parentBox)
    {
    NW_REQUIRED_PARAM(aElementHandler);

    NW_ASSERT(aContentHandler != NULL);
    NW_ASSERT(aElementNode != NULL);
    NW_ASSERT(parentBox != NULL);

    // Pass it on to the implementation
    CXHTMLObjectElementHandler* imp;

    imp = NW_XHTML_ObjectElementHandler_GetImpl(aContentHandler, aElementNode);
    NW_ASSERT(imp != NULL);

    return imp->CreateBoxTree(*parentBox);
    }

/* ------------------------------------------------------------------------- */
NW_Uint8
_NW_XHTML_ObjectElementHandler_ProcessEvent (const NW_XHTML_ElementHandler_t* aElementHandler,
        NW_XHTML_ContentHandler_t* aContentHandler, NW_DOM_ElementNode_t* aElementNode,
        NW_LMgr_Box_t* aBox, NW_Evt_Event_t* aEvent)
    {
    NW_REQUIRED_PARAM(aElementHandler);

    NW_ASSERT(aContentHandler != NULL);
    NW_ASSERT(aElementNode != NULL);
    NW_ASSERT(aBox != NULL);
    NW_ASSERT(aEvent != NULL);

    // Pass it on to the implementation
    CXHTMLObjectElementHandler* imp;

    imp = NW_XHTML_ObjectElementHandler_GetImpl(aContentHandler, aElementNode);
    NW_ASSERT(imp != NULL);

    return imp->ProcessEvent(*aBox, *aEvent);
    }

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_XHTML_ObjectElementHandler_ApplyInfoProperties (const NW_XHTML_ElementHandler_t* aElementHandler,
        NW_XHTML_ContentHandler_t* aContentHandler, NW_DOM_ElementNode_t* aElementNode,
        NW_LMgr_Box_t* aBox)
    {
    NW_REQUIRED_PARAM(aElementHandler);

    NW_ASSERT(aContentHandler != NULL);
    NW_ASSERT(aElementNode != NULL);
    NW_ASSERT(aBox != NULL);

    // Pass it on to the implementation
    CXHTMLObjectElementHandler* imp;

    imp = NW_XHTML_ObjectElementHandler_GetImpl(aContentHandler, aElementNode);
    NW_ASSERT(imp != NULL);

    return imp->ApplyInfoProperties(*aBox);
    }
