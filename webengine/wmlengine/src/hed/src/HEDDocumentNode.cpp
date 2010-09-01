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


#include "nw_hed_documentnodei.h"

#include "nw_hed_documentroot.h"
#include "nw_text_ucs2.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_HED_DocumentNode_Class_t NW_HED_DocumentNode_Class = {
  { /* NW_Object_Core               */
    /* super                        */ &NW_Object_Dynamic_Class,
    /* queryInterface               */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base               */
    /* interfaceList                */ NULL
  },
  { /* NW_Object_Dynamic            */
    /* instanceSize                 */ sizeof (NW_HED_DocumentNode_t),
    /* construct                    */ _NW_HED_DocumentNode_Construct,
    /* destruct                     */ _NW_HED_DocumentNode_Destruct
  },
  { /* NW_HED_DocumentNode	    */
    /* cancel                       */ _NW_HED_DocumentNode_Cancel,
    /* partialLoadCallback	        */ NULL,
    /* initialize                   */ _NW_HED_DocumentNode_Initialize,
    /* nodeChanged                  */ _NW_HED_DocumentNode_NodeChanged,
    /* getBoxTree                   */ NULL,
    /* processEvent                 */ _NW_HED_DocumentNode_ProcessEvent,
    /* handleError                  */ _NW_HED_DocumentNode_HandleError,
    /* suspend                      */ _NW_HED_DocumentNode_Suspend,
    /* resume                       */ _NW_HED_DocumentNode_Resume,
    /* allLoadCompleted             */ NULL,
    /* intraPageNavigationCompleted */ _NW_HED_DocumentNode_IntraPageNavigationCompleted,
    /* loseFocus                    */ NULL,
    /* gainFocus                    */ NULL,
    /* handleLoadComplete           */ _NW_HED_DocumentNode_HandleLoadComplete,
  }
};

/* ------------------------------------------------------------------------- *
   NW_Object_Dynamic methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_DocumentNode_Construct (NW_Object_Dynamic_t* dynamicObject,
                                va_list* argList)
{
  NW_HED_DocumentNode_t* thisObj;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject,
                                     &NW_HED_DocumentNode_Class));
  NW_ASSERT (argList != NULL);

  /* for convenience */
  thisObj = NW_HED_DocumentNodeOf (dynamicObject);

  /* initialize our members */
  thisObj->parent = va_arg (*argList, NW_HED_DocumentNode_t*);

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
void
_NW_HED_DocumentNode_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_HED_DocumentNode_t* thisObj;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject,
                                     &NW_HED_DocumentNode_Class));

  /* for convenience */
  thisObj = NW_HED_DocumentNodeOf (dynamicObject);

  /* if this node is connected to a document tree, we must remove it from
     that tree before deleting it */
  if (thisObj->parent != NULL) {
    NW_HED_DocumentNode_t* rootNode;
    NW_HED_ICompositeNode_t* compositeNode;

    /* delete all outstanding requests associated with this node */
    rootNode = NW_HED_DocumentNode_GetRootNode (thisObj);

    /* Detach the node from its parent first. It's important because in Destructor
       we should not cast the node back to it's subclass, e.g. XHTML CH */
    compositeNode = (NW_HED_ICompositeNode_t*)
      NW_Object_QueryInterface (thisObj->parent, &NW_HED_ICompositeNode_Class);
    NW_ASSERT (compositeNode != NULL);
    (void) NW_HED_ICompositeNode_RemoveChild (compositeNode, thisObj);

    /* delete all outstanding requests associated with this node */
    if (NW_Object_IsInstanceOf (rootNode, &NW_HED_DocumentRoot_Class)) {
      NW_HED_DocumentRoot_DeleteRequests (rootNode, thisObj);
    }
  }
}

/* ------------------------------------------------------------------------- *
   NW_HED_DocumentNode public/protected methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_DocumentNode_Cancel (NW_HED_DocumentNode_t* documentNode, 
			     NW_HED_CancelType_t cancelType)
{
  /* avoid "unreferenced formal parameter" warnings  */
  (void) documentNode;
  NW_REQUIRED_PARAM(cancelType);

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (documentNode,
                                     &NW_HED_DocumentNode_Class));
  return KBrsrSuccess;
}


/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_DocumentNode_Initialize( NW_HED_DocumentNode_t* documentNode,
    TBrowserStatusCode aInitStatus )
{
  TBrowserStatusCode status;

  NW_REQUIRED_PARAM( aInitStatus );

  /* now that the content has been loaded and initialization has taken place,
     we are ready for action - we need to notify out parent of this */
  status = NW_HED_DocumentNode_NodeChanged (documentNode, documentNode);
  if(status != KBrsrSuccess)
  {
    return status;
  }
  /* successful completion */
  return KBrsrSuccess;
}


/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_DocumentNode_NodeChanged (NW_HED_DocumentNode_t* documentNode,
                                  NW_HED_DocumentNode_t* childNode)
{
  /* avoid 'unreferenced formal parameter' warnings */
  (void) childNode;

  /* notify our parent */
  return NW_HED_DocumentNode_NodeChanged (documentNode->parent, documentNode);
}

/* ------------------------------------------------------------------------- */
NW_Uint8
_NW_HED_DocumentNode_ProcessEvent (NW_HED_DocumentNode_t* documentNode,
                                   NW_LMgr_Box_t* box,
                                   NW_Evt_Event_t* event,
                                   void* context)
{
  /* avoid 'unreferenced formal parameter' warnings */
  (void) documentNode;
  (void) box;
  (void) event;
  (void) context;

  /* we didn't absorb the event */
  return 0;
}

/* ------------------------------------------------------------------------- */
void
_NW_HED_DocumentNode_HandleError (NW_HED_DocumentNode_t* documentNode,
                                  const NW_HED_UrlRequest_t *urlRequest,
                                  NW_Int16 errorClass,
                                  NW_WaeError_t error)
{
  /* 
  ** TODO this doesn't work if the node is deeper than one level --
  ** (eg. DocumentRoot -> XHTML -> Image)
  */

  /* notify our parent */
  if (documentNode->parent != NULL) {
    NW_HED_DocumentNode_HandleError (documentNode->parent, urlRequest,
                                     errorClass, error);
  }
}

/* ------------------------------------------------------------------------- */
void
_NW_HED_DocumentNode_Suspend (NW_HED_DocumentNode_t* thisObj, 
                              NW_Bool aDestroyBoxTree)
{
  /* avoid "unreferenced formal parameter" warnings  */
  (void) aDestroyBoxTree;

  NW_HED_DocumentNode_t* rootNode;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_HED_DocumentNode_Class));

  /* delete all outstanding requests associated with this node */
  rootNode = NW_HED_DocumentNode_GetRootNode (thisObj);
  if (NW_Object_IsInstanceOf (rootNode, &NW_HED_DocumentRoot_Class)) 
    {
    NW_HED_DocumentRoot_DeleteRequests (rootNode, thisObj);
    }
}

/* ------------------------------------------------------------------------- */
void
_NW_HED_DocumentNode_Resume (NW_HED_DocumentNode_t* thisObj)
{
  /* avoid unreferenced formal parameter warnings */
  (void) thisObj;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_HED_DocumentNode_Class));
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_DocumentNode_IntraPageNavigationCompleted (NW_HED_DocumentNode_t* documentNode)
{
  /* avoid "unreferenced formal parameter" warnings  */
  (void) documentNode;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (documentNode,
                                     &NW_HED_DocumentNode_Class));
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
NW_HED_DocumentNode_t*
_NW_HED_DocumentNode_GetRootNode (NW_HED_DocumentNode_t* thisObj)
{
  /* find the topmost DocumentNode - this should be the DocumentRoot */
  while (thisObj->parent != NULL) {
    thisObj = thisObj->parent;
  }
  return thisObj;
}


/* ------------------------------------------------------------------------- */
// The function is called when a load issued by the document node is complete
void
_NW_HED_DocumentNode_HandleLoadComplete(NW_HED_DocumentNode_t* documentNode,
                                        const NW_HED_UrlRequest_t *urlRequest,
                                        NW_Int16 errorClass,
                                        TBrowserStatusCode error,
                                        NW_Uint16 transactionId,
                                        void* context )
{
  // default implementation none
  NW_REQUIRED_PARAM(documentNode);
  NW_REQUIRED_PARAM(urlRequest);
  NW_REQUIRED_PARAM(errorClass);
  NW_REQUIRED_PARAM(error);
  NW_REQUIRED_PARAM(transactionId);
  NW_REQUIRED_PARAM(context);
}

