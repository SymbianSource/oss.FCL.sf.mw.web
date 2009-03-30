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


#ifndef _BRSR_TYPES_H_
#define _BRSR_TYPES_H_

// Forward declarations
class CGDIDeviceContext;
class CGDIFont;
class CView;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
typedef unsigned char           NW_Bool;        /* NW_FALSE, NW_TRUE  */
typedef signed   short          NW_Int16;       /* -32768 ... +32767  */
typedef signed   long           NW_Int32;       /* -2147483648 ... +2147483647 */
typedef unsigned char           NW_Uint8;       /* 0 ... 255          */
typedef unsigned short          NW_Uint16;      /* 0 ... 65535        */
typedef unsigned long           NW_Uint32;      /* 0 ... 4294967295   */
typedef NW_Int32      NW_WaeError_t;

typedef NW_Uint16 NW_ADT_Vector_Metric_t;

struct NW_ADT_DynamicVector_s;
typedef struct NW_ADT_DynamicVector_s NW_ADT_DynamicVector_t;

struct NW_Dlg_SelectData_s;
typedef struct NW_Dlg_SelectData_s NW_Dlg_SelectData_t;

struct NW_Evt_Event_s;
typedef struct NW_Evt_Event_s NW_Evt_Event_t;

struct NW_Evt_KeyEvent_s;
typedef struct NW_Evt_KeyEvent_s NW_Evt_KeyEvent_t;

struct NW_Evt_ScrollEvent_s;
typedef struct NW_Evt_ScrollEvent_s NW_Evt_ScrollEvent_t;

struct NW_Evt_TabEvent_s;
typedef struct NW_Evt_TabEvent_s NW_Evt_TabEvent_t;

typedef NW_Int32 NW_GDI_Metric_t;

struct NW_GDI_Rectangle_s;
typedef struct NW_GDI_Rectangle_s NW_GDI_Rectangle_t;

struct NW_HED_DocumentNode_s;
typedef struct NW_HED_DocumentNode_s NW_HED_DocumentNode_t;

struct NW_HED_DocumentRoot_s;
typedef struct NW_HED_DocumentRoot_s NW_HED_DocumentRoot_t;

struct NW_HED_UrlRequest_s;
typedef struct NW_HED_UrlRequest_s NW_HED_UrlRequest_t;

struct NW_LMgr_Box_s;
typedef struct NW_LMgr_Box_s NW_LMgr_Box_t;

struct NW_LMgr_BoxVisitor_s;
typedef struct NW_LMgr_BoxVisitor_s NW_LMgr_BoxVisitor_t;

struct NW_LMgr_FlowBox_s;
typedef struct NW_LMgr_FlowBox_s NW_LMgr_FlowBox_t;

struct NW_LMgr_RootBox_s;
typedef struct NW_LMgr_RootBox_s NW_LMgr_RootBox_t;

struct NW_Msg_Message_s;
typedef struct NW_Msg_Message_s NW_Msg_Message_t;

struct NW_MVC_ILoadListener_s;
typedef struct NW_MVC_ILoadListener_s NW_MVC_ILoadListener_t;

struct NW_Text_Abstract_s;
typedef struct NW_Text_Abstract_s NW_Text_t;

typedef NW_Uint32 NW_Text_Length_t;

struct NW_Url_Resp_s;
typedef NW_Url_Resp_s NW_Url_Resp_t;

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* __BRSR_TYPES_H_ */
