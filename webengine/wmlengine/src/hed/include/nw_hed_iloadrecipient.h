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


#ifndef NW_HED_ILOADRECIPIENT_H
#define NW_HED_ILOADRECIPIENT_H

#include "nw_object_interface.h"
#include "NW_Text_Abstract.h"
#include "nw_hed_urlrequest.h"
#include <urlloader_urlresponse.h>
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  


/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_HED_ILoadRecipient_Class_s NW_HED_ILoadRecipient_Class_t;
typedef struct NW_HED_ILoadRecipient_s NW_HED_ILoadRecipient_t;

/* ------------------------------------------------------------------------- *
   method type prototypes
 * ------------------------------------------------------------------------- */
typedef
TBrowserStatusCode
(*NW_HED_ILoadRecipient_ProcessPartialLoad_t) (NW_HED_ILoadRecipient_t* loadRecipient,
                                        TBrowserStatusCode loadStatus,
                                        NW_Uint16 transactionId,
                                        NW_Int32 chunkIndex,
                                        NW_Url_Resp_t* response,
                                        NW_HED_UrlRequest_t* urlRequest,
                                        void* clientData);

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_HED_ILoadRecipient_ClassPart_s {
   NW_HED_ILoadRecipient_ProcessPartialLoad_t processPartialLoad;
} NW_HED_ILoadRecipient_ClassPart_t;

struct NW_HED_ILoadRecipient_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Interface_ClassPart_t NW_Object_Interface;
  NW_HED_ILoadRecipient_ClassPart_t NW_HED_ILoadRecipient;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_HED_ILoadRecipient_s {
  NW_Object_Interface_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_HED_ILoadRecipient_GetClassPart(_object) \
  (NW_Object_GetClassPart (_object, NW_HED_ILoadRecipient))

#define NW_HED_ILoadRecipientOf(_object) \
  (NW_Object_Cast (_object, NW_HED_ILoadRecipient))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT const NW_Object_Core_Class_t NW_HED_ILoadRecipient_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_HED_ILoadRecipient_ProcessPartialLoad(_loadRecipient, _loadStatus, _transactionId, _chunkIndex, _response, _urlRequest, _clientData) \
  (NW_Object_Invoke ((_loadRecipient), NW_HED_ILoadRecipient, processPartialLoad) \
    ((_loadRecipient), (_loadStatus), (_transactionId), (_chunkIndex), (_response), (_urlRequest), \
     (_clientData)))


#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_HED_ILOADRECIPIENT_H */
