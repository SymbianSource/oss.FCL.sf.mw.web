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


#ifndef NW_TEXTUTILS_H
#define NW_TEXTUTILS_H

#include "nw_text_ucs2.h"
#include "nw_gdi_types.h"
#include "BrsrTypes.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */

/* Constants */

#define NW_LMgr_Text_Flags_SplitAtSpace    0x01
#define NW_LMgr_Text_Flags_SplitAtHyphen   0x02
#define NW_LMgr_Text_Flags_SplitAtNewline  0x04
#define NW_LMgr_Text_Flags_SplitCompressed 0x08
#define NW_LMgr_Text_Flags_WordWrap        0x10
#define NW_LMgr_Text_Flags_Truncate        0x20

/* Function prototypes */
  
NW_Text_UCS2_t*
NW_LMgr_Text_GetDisplayText (NW_Text_t* text, NW_Bool compressWS);

TBrowserStatusCode
NW_LMgr_Text_DrawText(NW_GDI_Point2D_t *point,
                      NW_Text_t *text,
                      NW_Bool compressWS,
                      CGDIDeviceContext *deviceContext, NW_Uint16 flag);
  
TBrowserStatusCode
NW_LMgr_Text_SplitUnder(NW_Text_t *text,
                        NW_GDI_Metric_t space, 
                        NW_Text_t* *split,
                        NW_Uint8 flags,
                        CGDIDeviceContext* deviceContext);

TBrowserStatusCode
NW_LMgr_Text_SplitExact(NW_Text_t *text,
                        NW_GDI_Metric_t space, 
                        NW_Text_t **split,
                        NW_Uint8 flags,
                        CGDIDeviceContext* deviceContext);

TBrowserStatusCode
NW_LMgr_Text_SplitAt(NW_Text_t *text,
                     NW_Text_t **split,
                     NW_Uint8 flags);

TBrowserStatusCode
NW_LMgr_Text_GetSubstringExtent(NW_Text_t *text, 
                                NW_Text_Length_t index,
                                NW_Bool compressWS,
                                NW_GDI_Dimension2D_t *extent,
                                CGDIDeviceContext *deviceContext);

TBrowserStatusCode
NW_LMgr_Text_GetExtent(NW_Text_t *text,
                       NW_Bool compressWS,
                       NW_GDI_Dimension2D_t *extent,
                       CGDIDeviceContext *deviceContext);

TBrowserStatusCode
NW_LMgr_Text_SplitAtNewLine (NW_Text_t *text,
                             NW_GDI_Metric_t space,
                             NW_Text_t **split,
                             CGDIDeviceContext* deviceContext);

NW_Text_t*
NW_LMgr_Text_NonprintableToSpace (NW_Text_t* text);

TBrowserStatusCode
NW_LMgr_Text_BreakText(NW_Text_t *text, 
                       NW_Text_t *trunctStr,
                       NW_GDI_Metric_t space,
                       NW_Uint8 flags,
                       CGDIDeviceContext  *deviceContext,
                       NW_Text_Length_t        *length);
NW_Bool
NW_LMgr_Text_CanSplit(NW_Ucs2 ch, NW_Uint8 flags);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_TEXTUTILS_H */
