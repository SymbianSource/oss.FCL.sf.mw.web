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


#include "nw_adt_map.h"
#include "nw_adt_resizablevector.h"

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_ADT_Map_t*
NW_ADT_ResizableMap_New (NW_Uint32 keySize,
                         NW_Uint32 elementSize,
                         NW_ADT_Vector_Metric_t initialCapacity,
                         NW_ADT_Vector_Metric_t capacityIncrement)
{
  NW_ADT_DynamicVector_t* vector;
  NW_ADT_Map_t* map;

  /* parameter assertions */
  NW_ASSERT (keySize != 0);
  NW_ASSERT (elementSize != 0);
  NW_ASSERT (initialCapacity != 0);
  NW_ASSERT (capacityIncrement != 0);

  /* create the vector */
  vector = (NW_ADT_DynamicVector_t*)
    NW_ADT_ResizableVector_New ((NW_ADT_Vector_Metric_t) (keySize + elementSize),
                                initialCapacity, capacityIncrement);
  if (vector == NULL) {
    return NULL;
  }

  /* create the map */
  map = NW_ADT_Map_New (keySize, elementSize, vector);
  if (map == NULL) {
    NW_Object_Delete (vector);
  }
  return map;
}
