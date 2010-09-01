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


#include "nw_adt_resizableobjectvectori.h"

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_ADT_ResizableObjectVector_Class_t NW_ADT_ResizableObjectVector_Class = {
  { /* NW_Object_Core               */
    /* super                        */ &NW_ADT_ResizableVector_Class,
    /* queryInterface               */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base               */
    /* interfaceList                */ NULL
  },
  { /* NW_Object_Dynamic            */
    /* instanceSize                 */ sizeof (NW_ADT_ResizableVector_t),
    /* construct                    */ _NW_ADT_ResizableVector_Construct,
    /* destruct                     */ _NW_ADT_ResizableObjectVector_Destruct
  },
  { /* NW_ADT_Vector		    */
    /* getElementSize               */ _NW_ADT_DynamicVector_GetElementSize,
    /* getSize                      */ _NW_ADT_DynamicVector_GetSize,
    /* addressAt		    */ _NW_ADT_ResizableVector_AddressAt
  },
  { /* NW_ADT_DynamicVector         */
    /* resizeCapacity               */ _NW_ADT_ResizableVector_ResizeCapacity,
    /* moveElements		    */ _NW_ADT_DynamicVector_MoveElements,
    /* clear                        */ _NW_ADT_DynamicVector_Clear
  },
  { /* NW_ADT_ResizableVector	    */
    /* unused                       */ NW_Object_Unused
  },
  { /* NW_ADT_ResizableObjectVector */
    /* unused                       */ NW_Object_Unused
  },
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
void
_NW_ADT_ResizableObjectVector_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_ADT_ResizableObjectVector_t* thisObj;
  NW_ADT_Vector_Metric_t index;

  /* for convenience */
  thisObj = NW_ADT_ResizableObjectVectorOf (dynamicObject);

  /* release the dynamic objects pointed by the object array */
  for (index = 0; index < NW_ADT_Vector_GetSize (thisObj); index++) {
    NW_Object_t* object;

    object = *(NW_Object_t**) NW_ADT_Vector_ElementAt (thisObj, index);
    NW_Object_Delete (object);
  }
}

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_ADT_ResizableObjectVector_t*
NW_ADT_ResizableObjectVector_New (NW_ADT_Vector_Metric_t initialCapacity,
                                  NW_ADT_Vector_Metric_t capacityIncrement)
{
  /* make the vector such that it holds pointers to NW_Object_t */
  return (NW_ADT_ResizableObjectVector_t*)
    NW_Object_New (&NW_ADT_ResizableObjectVector_Class, sizeof (NW_Object_t*),
                   initialCapacity, capacityIncrement);
}
