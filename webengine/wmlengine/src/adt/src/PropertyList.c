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


#include "nw_adt_propertylisti.h"

#include "nw_adt_mapiterator.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_ADT_PropertyList_Class_t NW_ADT_PropertyList_Class = {
  { /* NW_Object_Core          */
    /* super                   */ &NW_ADT_ValueList_Class,
    /* queryInterface          */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base          */
    /* interfaceList           */ NULL
  },
  { /* NW_Object_Dynamic       */
    /* instanceSize            */ sizeof (NW_ADT_PropertyList_t),
    /* construct               */ _NW_ADT_ValueList_Construct,
    /* destruct                */ NULL
  },
  { /* NW_ADT_ValueList        */
    /* unused                  */ NW_Object_Unused
  },
  { /* NW_ADT_PropertyList     */
    /* unused                  */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode 
NW_ADT_PropertyList_Get (const NW_ADT_PropertyList_t* thisObj,
                         NW_ADT_PropertyName_t name,
                         NW_ADT_Property_t* property)
{
  return NW_ADT_ValueList_Get (NW_ADT_ValueListOf (thisObj), &name, property);
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_ADT_PropertyList_Set (const NW_ADT_PropertyList_t* thisObj,
                         NW_ADT_PropertyName_t name,
                         const NW_ADT_Property_t* property)
{
  return NW_ADT_ValueList_Set (NW_ADT_ValueListOf (thisObj), &name, property);
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_ADT_PropertyList_Remove (const NW_ADT_PropertyList_t* thisObj,
                            NW_ADT_PropertyName_t name)
{
  return NW_ADT_ValueList_Remove (NW_ADT_ValueListOf (thisObj), &name);
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_ADT_PropertyList_Copy (const NW_ADT_PropertyList_t* thisObj,
                          NW_ADT_PropertyList_t* dest)
{
  return NW_ADT_ValueList_Copy (NW_ADT_ValueListOf (thisObj), NW_ADT_ValueListOf (dest));
}

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_ADT_PropertyList_t*
NW_ADT_PropertyList_New (NW_ADT_Vector_Metric_t capacity,
                         NW_ADT_Vector_Metric_t increment)
{
  return (NW_ADT_PropertyList_t*)
    NW_Object_New (&NW_ADT_PropertyList_Class, sizeof (NW_ADT_PropertyName_t),
                   capacity, increment);
}
