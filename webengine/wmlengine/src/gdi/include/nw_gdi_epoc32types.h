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


#ifndef NW_GDI_EPOC32TYPES_H
#define NW_GDI_EPOC32TYPES_H

#include "nwx_defs.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- */
typedef NW_Int32 NW_GDI_Metric_t;

typedef enum NW_GDI_Pattern_e {
  NW_GDI_Pattern_None, /* CGraphicsContext::ENullPen */
  NW_GDI_Pattern_Solid, /* CGraphicsContext::ESolidPen */
  NW_GDI_Pattern_Dotted, /* CGraphicsContext::EDottedPen */
  NW_GDI_Pattern_Dashed, /* CGraphicsContext::EDashedPen */
} NW_GDI_Pattern_t;

typedef enum NW_GDI_PaintMode_e {
  NW_GDI_PaintMode_Copy=0x20, /* CGraphicsContext::EDrawModePEN */
  NW_GDI_PaintMode_Or,
  NW_GDI_PaintMode_XOr,
  NW_GDI_PaintMode_And,
  NW_GDI_PaintMode_Not,
  NW_GDI_PaintMode_Blink
} NW_GDI_PaintMode_t;

typedef enum NW_GDI_TextDirection_e {
  NW_GDI_TextDirection_Horizontal,
  NW_GDI_TextDirection_HorizontalUpDown,
  NW_GDI_TextDirection_VerticalTopDown,
  NW_GDI_TextDirection_VerticalBottomUp
} NW_GDI_TextDirection_t;


#ifdef __cplusplus
} // extern "C"
#endif /* __cplusplus */

#endif /* NW_GDI_EPOC32TYPES_H */
