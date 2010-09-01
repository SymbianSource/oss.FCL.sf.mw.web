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


#ifndef NW_HED_DOCUMENTNODEI_H
#define NW_HED_DOCUMENTNODEI_H

#include "nw_object_dynamici.h"
#include "nw_hed_documentnode.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   method implementation prototypes
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_DocumentNode_Construct (NW_Object_Dynamic_t* dynamicObject,
                                va_list* argp);

NW_HED_EXPORT
void
_NW_HED_DocumentNode_Destruct (NW_Object_Dynamic_t* dynamicObject);

NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_DocumentNode_Cancel (NW_HED_DocumentNode_t* documentNode, 
							 NW_HED_CancelType_t cancelType);

NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_DocumentNode_Initialize( NW_HED_DocumentNode_t* documentNode,
                                 TBrowserStatusCode aInitStatus );

NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_DocumentNode_NodeChanged (NW_HED_DocumentNode_t* documentNode,
                                  NW_HED_DocumentNode_t* childNode);

NW_HED_EXPORT
NW_Uint8
_NW_HED_DocumentNode_ProcessEvent (NW_HED_DocumentNode_t* documentNode,
                                   NW_LMgr_Box_t* box,
                                   NW_Evt_Event_t* event,
                                   void* context);
NW_HED_EXPORT
void
_NW_HED_DocumentNode_HandleError (NW_HED_DocumentNode_t* documentNode,
                                  const NW_HED_UrlRequest_t *urlRequest,
                                  NW_Int16 errorClass,
                                  NW_WaeError_t error);

NW_HED_EXPORT
void
_NW_HED_DocumentNode_Suspend (NW_HED_DocumentNode_t* documentNode,
                              NW_Bool aDeleteBoxTree);

NW_HED_EXPORT
void
_NW_HED_DocumentNode_Resume (NW_HED_DocumentNode_t* documentNode);

// The function is called when a load issued by the document node is complete
NW_HED_EXPORT
void
_NW_HED_DocumentNode_HandleLoadComplete(NW_HED_DocumentNode_t* documentNode,
                                        const NW_HED_UrlRequest_t *urlRequest,
                                        NW_Int16 errorClass,
                                        TBrowserStatusCode error,
                                        NW_Uint16 transactionId,
                                        void* context );



NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_DocumentNode_IntraPageNavigationCompleted (NW_HED_DocumentNode_t* documentNode);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_HED_DOCUMENTNODEI_H */
