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


#ifndef NW_CSS_PROCESSORI_H
#define NW_CSS_PROCESSORI_H

#include "nw_object_dynamici.h"
#include "nw_css_processor.h"
#include "nw_image_iimageobserveri.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  


/* ------------------------------------------------------------------------- *
   protected types
 * ------------------------------------------------------------------------- */

/* Note: use "volatile" keyword to suppress compiler optimizer bug */
/*       which changes store/load operation into a data-type cast */
typedef volatile union NW_CSS_Processor_LoadData_u{
  TInt bufferIndex;
  NW_Text_t* url;
}NW_CSS_Processor_LoadData_t;

typedef struct NW_CSS_Processor_PendingLoad_s {
  NW_Bool isSavedDeck;
  NW_Bool isHistLoad;
  NW_Bool isReLoad;
  NW_Text_t* url;
  TInt cssBufferIndex;
} NW_CSS_Processor_PendingLoad_t;

typedef struct NW_CSS_Processor_LoadContext_s{
  NW_Uint8 loadType;
  NW_Int32 bufferIndex;
}NW_CSS_Processor_LoadContext_t;

enum NW_CSS_Processor_Type_e{
  NW_CSS_Processor_LoadImage = 1,
  NW_CSS_Processor_LoadCSS  
};

/* ------------------------------------------------------------------------- *
   protected global data
 * ------------------------------------------------------------------------- */
NW_CSS_EXPORT const NW_HED_ILoadRecipient_Class_t NW_CSS_Processor_ILoadRecipient_Class;
NW_CSS_EXPORT const NW_Image_IImageObserver_Class_t NW_CSS_Processor_IImageObserver_Class;
NW_CSS_EXPORT const NW_Object_Class_t* const NW_CSS_Processor_InterfaceList[];

/* ------------------------------------------------------------------------- *
   virtual methods implementation declaration
 * ------------------------------------------------------------------------- */

NW_CSS_EXPORT
TBrowserStatusCode
_NW_CSS_Processor_Construct (NW_Object_Dynamic_t* dynamicObject,
                            va_list* argp);

NW_CSS_EXPORT
void
_NW_CSS_Processor_Destruct (NW_Object_Dynamic_t* dynamicObject);

NW_CSS_EXPORT
TBrowserStatusCode  
_NW_CSS_Processor_ILoadRecipient_ProcessPartialLoad(NW_HED_ILoadRecipient_t* loadListener,
                                                    TBrowserStatusCode loadStatus,
                                                    NW_Uint16 transactionId,
                                                    NW_Int32 chunkIndex,
                                                    NW_Url_Resp_t* response,
                                                    NW_HED_UrlRequest_t* urlRequest,
                                                    void* clientData);
extern
void
_NW_CSS_Processor_IImageObserver_ImageOpened( NW_Image_IImageObserver_t* imageObserver,
                                              NW_Image_AbstractImage_t* abstractImage,
                                              NW_Bool failed,
                                              NW_Int16 suppressDupReformats );

extern
void
_NW_CSS_Processor_IImageObserver_SizeChanged( NW_Image_IImageObserver_t* imageObserver,
                                              NW_Image_AbstractImage_t* abstractImage );

extern
void
_NW_CSS_Processor_IImageObserver_ImageOpeningStarted( NW_Image_IImageObserver_t* imageObserver,
                                                      NW_Image_AbstractImage_t* abstractImage );

extern
void
_NW_CSS_Processor_IImageObserver_ImageDecoded( NW_Image_IImageObserver_t* imageObserver,
                                               NW_Image_AbstractImage_t* abstractImage,
                                               NW_Bool failed);

extern
void
_NW_CSS_Processor_IImageObserver_ImageDestroyed( NW_Image_IImageObserver_t* imageObserver,
                                                 NW_Image_AbstractImage_t* abstractImage );


NW_Bool
_NW_CSS_Processor_IImageObserver_IsVisible(NW_Image_IImageObserver_t* aImageObserver);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_CSS_PROCESSORI_H */


