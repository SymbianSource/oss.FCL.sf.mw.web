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


#ifndef NW_GDI_TYPES_H
#define NW_GDI_TYPES_H

#include "nwx_defs.h"
#include "nw_gdi_epoc32types.h"
#include "nwx_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* ------------------------------------------------------------------------- */
typedef NW_Uint32 NW_GDI_Color_t;
enum {
  NW_GDI_Color_Black       = 0x00000000,
  NW_GDI_Color_Red         = 0x00ff0000,
  NW_GDI_Color_Green       = 0x0000ff00,
  NW_GDI_Color_Blue        = 0x000000ff,
  NW_GDI_Color_White       = 0x00ffffff,
  NW_GDI_Color_Transparent = 0x01000000,

};

#define NW_GDI_RGB(r,g,b) ((NW_GDI_Color_t)(((NW_Byte)(b)|((NW_Word)((NW_Byte) \
                            (g))<<8))|(((NW_Dword)(NW_Byte)(r))<<16)))
#define NW_GDI_GetRValue(rgb) ((NW_Byte)((rgb)>>16))
#define NW_GDI_GetGValue(rgb) ((NW_Byte)(((NW_Word)(rgb)) >> 8))
#define NW_GDI_GetBValue(rgb) ((NW_Byte)(rgb))

typedef struct NW_GDI_ColorPalette_s {
  NW_GDI_Color_t *colors;
	NW_Int32 numColors;
} NW_GDI_ColorPalette_t;

typedef struct NW_GDI_Point2D_s {
  NW_GDI_Metric_t x;
  NW_GDI_Metric_t y;
} NW_GDI_Point2D_t, NW_GDI_Delta_t;

typedef struct NW_GDI_Dimension2D_s {
  NW_GDI_Metric_t width;
  NW_GDI_Metric_t height;
} NW_GDI_Dimension2D_t;

typedef struct NW_GDI_Dimension3D_s {
  NW_GDI_Metric_t width;
  NW_GDI_Metric_t height;
  NW_GDI_Metric_t depth;
} NW_GDI_Dimension3D_t;

typedef struct NW_GDI_Rectangle_s {
  NW_GDI_Point2D_t point;
  NW_GDI_Dimension2D_t dimension;
} NW_GDI_Rectangle_t;

typedef enum NW_GDI_FlowDirection_e {
  NW_GDI_FlowDirection_LeftRight,
  NW_GDI_FlowDirection_RightLeft,
  NW_GDI_FlowDirection_TopDown,
  NW_GDI_FlowDirection_BottomUp,
  NW_GDI_FlowDirection_Unknown
} NW_GDI_FlowDirection_t;

typedef enum NW_GDI_Image_ConvertToNativeFormat_e {
  NW_GDI_Image_ConversionNotRequired = 0,
  NW_GDI_Image_ConversionRequired
} NW_GDI_Image_ConvertToNativeFormat_t;

typedef enum NW_GDI_SplitModes_e {
  NW_GDI_SplitMode_WordWrap,
  NW_GDI_SplitMode_Clip,
  NW_GDI_SplitMode_Truncate,
  NW_GDI_SplitMode_Newline
} NW_GDI_SplitFlags_t;

typedef enum NW_GDI_TextDecoration_e {
	NW_GDI_TextDecoration_None = 0x00,
  NW_GDI_TextDecoration_Underline = 0x01,
  NW_GDI_TextDecoration_Overline = 0x02,
  NW_GDI_TextDecoration_Strikeout = 0x04
} NW_GDI_TextDecoration_t;

typedef struct NW_GDI_ImageBlock_s {
  NW_GDI_Dimension3D_t size;
  const void* data;
  const void* mask;
  NW_Bool invertedMask;
  NW_Buffer_t colPalette;
  NW_Bool isTransparent;
  NW_GDI_Color_t transparencyIndex;
  NW_GDI_Image_ConvertToNativeFormat_t conversionStatus;
} NW_GDI_ImageBlock_t;


#ifdef __cplusplus
} /* extern "C" { */
#endif /* __cplusplus */

#endif /* NW_GDI_TYPES_H */
