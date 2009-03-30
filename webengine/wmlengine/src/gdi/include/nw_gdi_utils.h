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


#ifndef NW_GDI_UTILS_H
#define NW_GDI_UTILS_H

#include "nw_gdi_types.h"
#include "NW_GDI_EXPORT.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/* ------------------------------------------------------------------------- */
NW_GDI_EXPORT
NW_Bool
NW_GDI_Rectangle_Contains (const NW_GDI_Rectangle_t* rect,
                           const NW_GDI_Point2D_t* point);

/**
* NW_GDI_Rectangle_Cross
* @description This method is useful to see if two rectangles overlap.  There
* are 3 cases to consider:
* (1) The rectangles do not overlap
* (2) The rectangles partially overlap
* (3) One rectangle completely encloses the other rectangle
* Case (1): If the rectangles do not overlap, NW_GDI_Rectangle_Cross will return
*  NW_FALSE.
*
*   ________________
*   |              |  ________    
*   |       1      |  |   2  |
*   |              |  |______|
*   |______________|
*
* Case (2): If the rectangles partially overlap, NW_GDI_Rectangle_Cross will
*  return NW_TRUE, and the resulting rectangle will be the overlapping intersection
*  of the two rectangles. So as in the example below, the result rectangle will
*  contain the intersection of rectangle #1 and rectangle #2, which is rectangle #3.
*   
*   ________________
*   |            __|___
*   |       1   |3 | 2 |
*   |           |__|___|
*   |______________|
*
* Case (3): One rectangle completely encloses the other rectangle, NW_GDI_Rectangle_Cross 
*  will return NW_TRUE, and the resulting rectangle will be the portion of the 
*  overlap, which in this case will be the enclosed rectangle. So in the example
*  below, the intesection of rectangle #1 and rectangle #2, is rectangle #2.
*
*   ---------------
*   |      _____   |
*   | 1   | 2   |  |
*   |     |_____|  |
*   |              |
*   ---------------
*
* @param const NW_GDI_Rectangle_t* rect1: One of the rectangles compare for overlap
* @param const NW_GDI_Rectangle_t* rect2: The other rectangle to compare for overlap
* @param NW_GDI_Rectangle_t* result: If there is a overlap, the result is the intersecting
*  rectangle.
* @return NW_Bool: NW_TRUE if there is an overlap. NW_FALSE if there is no overlap
*/
NW_GDI_EXPORT
NW_Bool
NW_GDI_Rectangle_Cross (const NW_GDI_Rectangle_t* rect1,
                        const NW_GDI_Rectangle_t* rect2,
                        NW_GDI_Rectangle_t* result);

NW_GDI_EXPORT
void
NW_GDI_Rectangle_Add (const NW_GDI_Rectangle_t* rect1,
                      const NW_GDI_Rectangle_t* rect2,
                      NW_GDI_Rectangle_t* result);

#define NW_VerticalLayout_MinImageWidth1 4
#define NW_VerticalLayout_MinImageHeight1 4
#define NW_VerticalLayout_MinImageWidth2 26 
#define NW_VerticalLayout_MinImageHeight2 19

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_GDI_UTILS_H */
