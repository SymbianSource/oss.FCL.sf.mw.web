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


#include "nw_gdi_utils.h"

NW_Bool
NW_GDI_Rectangle_Contains (const NW_GDI_Rectangle_t* rect,
                           const NW_GDI_Point2D_t* point)
{
  if ((point->x >= rect->point.x) && (point->x < rect->point.x + rect->dimension.width) &&
    (point->y >= rect->point.y) && (point->y < rect->point.y + rect->dimension.height)) {
    return NW_TRUE;
  }
  else {
    return NW_FALSE;
  }
}

NW_Bool
NW_GDI_Rectangle_Cross (const NW_GDI_Rectangle_t* rect1,
                        const NW_GDI_Rectangle_t* rect2,
                        NW_GDI_Rectangle_t* result)
{
  const NW_GDI_Rectangle_t* temp;
  NW_GDI_Metric_t x1_left;
  NW_GDI_Metric_t x1_right;
  NW_GDI_Metric_t y1_bottom;
  NW_GDI_Metric_t x2_left;
  NW_GDI_Metric_t x2_right;
  NW_GDI_Metric_t y2_top;
  NW_GDI_Metric_t y2_bottom;
  NW_GDI_Metric_t r_left;
  NW_GDI_Metric_t r_right;
  NW_GDI_Metric_t r_top;
  NW_GDI_Metric_t r_bottom;

  if (result != NULL) {
    NW_Mem_memset(result, 0, sizeof(NW_GDI_Rectangle_t));
  }

  if ((rect1->dimension.width==0 && rect1->dimension.height==0) ||
      (rect2->dimension.width==0 && rect2->dimension.height==0)) {
    return NW_FALSE;
  }

  if (rect1->point.y > rect2->point.y) {
    temp = rect1;
    rect1 = rect2;
    rect2 = temp;
  }

  x1_left = rect1->point.x; 
  x1_right = (NW_GDI_Metric_t)(rect1->point.x + rect1->dimension.width - 1);
  y1_bottom = (NW_GDI_Metric_t)(rect1->point.y + rect1->dimension.height - 1);

  x2_left = rect2->point.x; 
  x2_right = (NW_GDI_Metric_t)(rect2->point.x + rect2->dimension.width - 1);
  y2_top = rect2->point.y; 
  y2_bottom = (NW_GDI_Metric_t)(rect2->point.y + rect2->dimension.height - 1);

  if (x1_left < x2_left) {
    if (x1_right < x2_left) {
      return NW_FALSE;
    }
    else {
      r_left = x2_left;
    }

    if (x2_right < x1_right) {
      r_right = x2_right;
    }
    else {
      r_right = x1_right;
    }
  }
  else {
    if (x1_left > x2_right) {
      return NW_FALSE;
    }
    else {
      r_left = x1_left;
    }

    if (x1_right < x2_right) {
      r_right = x1_right;
    }
    else {
      r_right = x2_right;
    }
  }

  /* Get the vertical intersection */
  if (y1_bottom < y2_top) {
    return NW_FALSE;
  }
  else {
    r_top = y2_top;
  }

  if (y2_bottom < y1_bottom) {
    r_bottom = y2_bottom;
  }
  else {
    r_bottom = y1_bottom;
  }
  
  if (result != NULL) {
    result->point.x = r_left;
    result->point.y = r_top;
    result->dimension.width = (NW_GDI_Metric_t)(r_right - r_left + 1);
    result->dimension.height = (NW_GDI_Metric_t)(r_bottom - r_top + 1);
  }

  return NW_TRUE;
}

void
NW_GDI_Rectangle_Add (const NW_GDI_Rectangle_t* rect1,
                      const NW_GDI_Rectangle_t* rect2,
                      NW_GDI_Rectangle_t* result)
{
  if (result) {
    *result = *rect1;

    if (rect2->point.x < rect1->point.x) {
      result->point.x = rect1->point.x;
    }
    if (rect2->point.y < rect1->point.y) {
      result->point.y = rect1->point.y;
    }
    if (rect2->point.x + rect2->dimension.width > 
          rect1->point.x + rect1->dimension.width) {
      result->dimension.width = (NW_GDI_Metric_t)(rect2->point.x + rect2->dimension.width - result->point.x);
    }
    if (rect2->point.y + rect2->dimension.height > 
          rect1->point.y + rect1->dimension.height) {
      result->dimension.height = (NW_GDI_Metric_t)(rect2->point.y + rect2->dimension.height - result->point.y);
    }
  }
}


