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


#ifndef NW_HED_APPSERVICES_H
#define NW_HED_APPSERVICES_H

#ifndef __E32DEF_H__
// Undef NULL is necessary to prevent un-necessary warnings.  <time.h> also
// defines NULL if not defined already.  Therefore, if time.h has already
// been included in the source file before this header file then warnings 
// would otherwise occur.
#ifdef NULL
#undef NULL
#endif
#include <e32def.h>
#endif

#include "nw_errnotify.h"
#include "nw_scrproxy.h"
#include "nw_wml_api.h"
#include "nw_wae.h"
#include "nw_evlog_api.h"
#include "nwx_url_utils.h"
#include "BrsrStatusCodes.h"
//#include "urlloader_urlloaderint.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct NW_String_String_s;
typedef struct NW_String_String_s NW_String_t;

typedef struct _NW_HED_AppServices_s NW_HED_AppServices_t;

typedef struct NW_AppServices_PictureViewAPI_s NW_AppServices_PictureViewAPI_t;

typedef struct NW_AppServices_SoftKeyAPI_s NW_AppServices_SoftKeyAPI_t;

typedef struct NW_AppServices_SelectListAPI_s NW_AppServices_SelectListAPI_t;

typedef struct NW_AppServices_BrowserAppAPI_s NW_AppServices_BrowserAppAPI_t;

typedef struct NW_AppServices_ImageViewAPI_s NW_AppServices_ImageViewAPI_t;

typedef struct NW_AppServices_GetLocalizedStringAPI_s NW_AppServices_GetLocalizedStringAPI_t;

typedef struct NW_AppServices_CharacterConversionAPI_s NW_AppServices_CharacterConversionAPI_t;

typedef struct NW_AppServices_EcmaScriptPI_s NW_AppServices_EcmaScriptPI_t;

typedef struct NW_AppServices_FormBox_s NW_AppServices_FormBox_t;

typedef struct NW_AppServices_SecurityNotes_s NW_AppServices_SecurityNotes_t;

typedef struct NW_AppServices_ObjectDialog_s NW_AppServices_ObjectDialog_t;

/*
**-------------------------------------------------------------------------
**  PictureViewer API
**-------------------------------------------------------------------------
*/
struct NW_AppServices_PictureViewAPI_s
{
  /* initialize the dynamic item list */
  void (*initDynList)(void *ctx);
  /* Add item to the dynamic item list */
  void (*addItem)(NW_Uint32* image);

  };

/*
**-------------------------------------------------------------------------
**  SoftKey API
**-------------------------------------------------------------------------
*/
typedef enum {
  NW_SOFTKEY_OK,
  NW_SOFTKEY_CLEAR,
  NW_SOFTKEY_OPTIONS,
  NW_SOFTKEY_BACK,
  NW_SOFTKEY_CANCEL
} NW_SoftKeyText_t;

struct NW_AppServices_SoftKeyAPI_s
{
  /* Set text of LeftSoftKey */
  void (*setLeft)(NW_SoftKeyText_t softKeyText);
  /* Set text of RightSoftKey */
  void (*setRight)(NW_SoftKeyText_t softKeyText);
};

/*

**-------------------------------------------------------------------------
**  SelectList API
**-------------------------------------------------------------------------
*/
struct NW_AppServices_SelectListAPI_s
{
  /* Stop select list panel */
  void (*cleanup)(void);
};

/*-------------------------------------------------------------------------
 *  Telephony API
 *-------------------------------------------------------------------------
 */
struct NW_AppServices_TelephonyAPI_s
{
  /* Make Call - a valid number should be sent */
  TBrowserStatusCode (*makeCall)(const NW_Ucs2* number);
  /* add phone book entry */
  TBrowserStatusCode (*addPBEntry)(const NW_Ucs2* number, const NW_Ucs2* name, const NW_Ucs2* email);
};

/*-------------------------------------------------------------------------
 *  Telephony API
 *-------------------------------------------------------------------------
 */
struct NW_AppServices_BrowserAppAPI_s
{
  /* Exit the browser application */
  TBrowserStatusCode (*exit)(void *ctx);
  TInt  (*SwitchToImgMapView)(void);
  TInt  (*SwitchFromImgMapViewIfNeeded)(void);
  TBool (*IsImageMapView)(void);
};

/*
**-------------------------------------------------------------------------
**  ImageViewer API
**-------------------------------------------------------------------------
*/
struct NW_AppServices_ImageViewAPI_s
{
  /* View the focussed image */
  void (*imageView)(void* rawData, NW_Int32 length, NW_Bool isWbmp);

};

/*
**-------------------------------------------------------------------------
**  GetLocalizedString API
**-------------------------------------------------------------------------
*/
struct NW_AppServices_GetLocalizedStringAPI_s
{
  /* Retrieve localized string based on type */
  NW_Ucs2* (*getLocalizedString)(NW_Uint32 type);

};

/*
**-------------------------------------------------------------------------
**  Character Conversion API
**-------------------------------------------------------------------------
*/
struct NW_AppServices_CharacterConversionAPI_s
{
  /* Converts the respo */
  TBrowserStatusCode (*convertFromForeignCharSet)(void* ctx, NW_Url_Resp_t* response);
  TBrowserStatusCode (*convertToForeignCharSet)(void* ctx, NW_String_t* text);
  TBrowserStatusCode (*convertFromForeignChunk)(void* ctx, NW_Uint32 foreignEncoding, void* inBuf, 
    TInt * numUnconvertible, TInt* indexFirstUnconvertible, void** outBuf);
};

/*
**-------------------------------------------------------------------------
**  EcmaScript API
**-------------------------------------------------------------------------
*/
struct NW_AppServices_EcmaScriptPI_s
{
  void (*WindowAlert)(void* browserApp_Ctx, NW_Ucs2* msg);
  NW_Bool (*WindowConfirm)(void* browserApp_Ctx, NW_Ucs2* msg, NW_Ucs2* yesMsg, NW_Ucs2* noMsg);
  NW_Ucs2* (*WindowPrompt)(void* browserApp_Ctx, NW_Ucs2* msg, NW_Ucs2* defaultVal);
  void (*WindowOpen)(void* browserApp_Ctx, NW_Ucs2* url, NW_Ucs2* target, NW_Uint32 reason);
};

/*
**-------------------------------------------------------------------------
**  FormBox API
**-------------------------------------------------------------------------
*/

struct NW_AppServices_FormBox_s
{
  TBrowserStatusCode (*InputInvalid)(void* aCtx, TInt aMin);
};

/*
**-------------------------------------------------------------------------
**  SecurityNotes API
**-------------------------------------------------------------------------
*/

struct NW_AppServices_SecurityNotes_s
{
  TBrowserStatusCode (*AboutToLoadPage)(NW_Uint32 noteType);
};

/*
**-------------------------------------------------------------------------
**  ObjectDialog API
**-------------------------------------------------------------------------
*/

struct NW_AppServices_ObjectDialog_s
{
  NW_Bool (*ShowObjectDialog)();
};


/*
**-------------------------------------------------------------------------
**  Application Services API
**-------------------------------------------------------------------------
*/
struct _NW_HED_AppServices_s {

  /* TODO:  insert other relevant APIs */  
  /* ... */

  /* Script Dialog API */
  NW_Scr_DialogApi_t scrDlgApi;
  
  /* Error Notification API */
  NW_ErrorApi_t errorApi;

  /* WTAI API */
  NW_WtaiApi_t wtaiApi;

  /* Generic Dialog API */
  NW_GenDlgApi_t genDlgApi;

  /* PictureView API */
  NW_AppServices_PictureViewAPI_t pictureViewApi;

  /* WaitNotePanel API */
  NW_UrlLoadProgressApi_t loadProgress_api;
  
  /* Event Log API */
  NW_EvLogApi_t evLogApi;

  /* SoftKey API */
  NW_AppServices_SoftKeyAPI_t softKeyApi;

  /* Userredirection Resp API */
  NW_UserRedirectionApi_t userRedirection;

  /* Select List API */
  NW_AppServices_SelectListAPI_t selectListApi;

  /* Script Suspend/Resume Query API */
  NW_Scr_SuspResQueryApi_t scriptSuspResApi;
 
  /* BrowserApp API */
  NW_AppServices_BrowserAppAPI_t browserAppApi;

  /* ImageView API */
  NW_AppServices_ImageViewAPI_t imageViewAPI;

  /* Localized UI strings API */
  NW_AppServices_GetLocalizedStringAPI_t getLocalizedStringAPI;

  /* Character Conversion API */
  NW_AppServices_CharacterConversionAPI_t characterConversionAPI;

  /* EcmaScript API */
  NW_AppServices_EcmaScriptPI_t ecmaScriptAPI;

  /* Fbox callbacks */
  NW_AppServices_FormBox_t  formBoxCallbacks;

  /* Security Notes */
  NW_AppServices_SecurityNotes_t securityNotes;

  /* Object Dialog */
  NW_AppServices_ObjectDialog_t objectDialog;

};



#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_HED_APPSERVICES_H */
