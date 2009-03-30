/*
* Copyright (c) 2000 - 2003 Nokia Corporation and/or its subsidiary(-ies).
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


#include "nw_lmgr_slavepropertylisti.h"
#include "nw_lmgr_simplepropertylisti.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_LMgr_SlavePropertyList_Class_t NW_LMgr_SlavePropertyList_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_LMgr_PropertyList_Class,
    /* queryInterface            */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base            */
    /* interfaceList             */ NULL
  },
  { /* NW_Object_Dynamic         */
    /* instanceSize              */ sizeof (NW_LMgr_SlavePropertyList_t),
    /* construct                 */ _NW_LMgr_SlavePropertyList_Construct,
    /* destruct                  */ NULL
  },
  { /* NW_LMgr_PropertyList      */
    /* get                       */ _NW_LMgr_SlavePropertyList_Get,
    /* set                       */ _NW_LMgr_SlavePropertyList_Set,
    /* remove                    */ _NW_LMgr_SlavePropertyList_Remove,
    /* clone                     */ _NW_LMgr_SlavePropertyList_Clone,
    /* clear                     */ _NW_LMgr_SlavePropertyList_Clear
  },
  { /* NW_LMgr_SlavePropertyList */
    /* unused                    */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */


/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_LMgr_SlavePropertyList_Construct (NW_Object_Dynamic_t* dynamicObject,
                                      va_list* argp)
{
  NW_LMgr_SlavePropertyList_t* thisObj;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject,
                                     &NW_LMgr_SlavePropertyList_Class));
  NW_ASSERT (argp != NULL);

  /* for convenience */
  thisObj = NW_LMgr_SlavePropertyListOf (dynamicObject);

  /* initialize the member variables */
  thisObj->siblingBox = va_arg (*argp, NW_LMgr_Box_t*);
  if (thisObj->siblingBox){
    NW_ASSERT (NW_Object_IsInstanceOf (thisObj->siblingBox,
                                       &NW_LMgr_Box_Class));
  }

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_LMgr_SlavePropertyList_Get (const NW_LMgr_PropertyList_t* propertyList,
                               NW_LMgr_PropertyName_t key,
                               NW_LMgr_PropertyList_Entry_t* entry)
{
  NW_LMgr_Box_t* masterBox;
  NW_LMgr_SlavePropertyList_t* slavePropList;
  NW_LMgr_PropertyList_t* siblingPropList;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (propertyList,
                                     &NW_LMgr_SlavePropertyList_Class));

  slavePropList = NW_LMgr_SlavePropertyListOf(propertyList);

  NW_ASSERT(slavePropList->siblingBox);
  siblingPropList = NW_LMgr_Box_PropListGet(slavePropList->siblingBox);
  NW_ASSERT(siblingPropList);

  switch (key)
  {
  case NW_CSS_Prop_sibling:
    entry->type = NW_ADT_ValueType_Object | NW_CSS_ValueType_Copy;
    entry->value.object = slavePropList->siblingBox;
    return KBrsrSuccess;

  case NW_CSS_Prop_rightPadding:
    entry->type = NW_ADT_ValueType_Integer;
    entry->value.integer = 0;
    return KBrsrSuccess;

  case NW_CSS_Prop_rightMargin:
    entry->type = NW_ADT_ValueType_Integer;
    entry->value.integer = 0;
    return KBrsrSuccess;

  case NW_CSS_Prop_rightBorderStyle:
    entry->type = NW_CSS_ValueType_Token;
    entry->value.integer = NW_CSS_PropValue_none;
    return KBrsrSuccess;

  case NW_CSS_Prop_leftPadding:
    if (NW_Object_IsInstanceOf(siblingPropList, &NW_LMgr_SlavePropertyList_Class)){    
      entry->type = NW_ADT_ValueType_Integer;
      entry->value.integer = 0;
      return KBrsrSuccess;
    }
    break;

  case NW_CSS_Prop_leftMargin:
    if (NW_Object_IsInstanceOf(siblingPropList, &NW_LMgr_SlavePropertyList_Class)){
      entry->type = NW_ADT_ValueType_Integer;
      entry->value.integer = 0;
      return KBrsrSuccess;
    }
    break;

  case NW_CSS_Prop_leftBorderStyle:
    if (NW_Object_IsInstanceOf(siblingPropList, &NW_LMgr_SlavePropertyList_Class)){
      entry->type = NW_CSS_ValueType_Token;
      entry->value.integer = NW_CSS_PropValue_none;
      return KBrsrSuccess;
    }
    break;

  default:
    break;
  }

  masterBox = NW_LMgr_SlavePropertyList_GetMasterBox (propertyList);
  if (masterBox == NULL){
    return NULL;
  }
  propertyList = NW_LMgr_Box_PropListGet (masterBox);
  if (propertyList == NULL){
    return NULL;
  }
  // return NW_LMgr_PropertyList_Get (propertyList, key, entry); */ 
  return NW_LMgr_SimplePropertyList_MapLookup( 
    NW_LMgr_SimplePropertyListOf( propertyList ), key, entry );
  }

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_LMgr_SlavePropertyList_Set (NW_LMgr_PropertyList_t* propertyList,
                               NW_LMgr_PropertyName_t key,
                               const NW_LMgr_PropertyList_Entry_t* entry)
{
  NW_LMgr_Box_t* masterBox;
  NW_LMgr_SlavePropertyList_t* slavePropList;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (propertyList,
                                     &NW_LMgr_SlavePropertyList_Class));

  slavePropList = NW_LMgr_SlavePropertyListOf(propertyList);

  if (key == NW_CSS_Prop_sibling){
    slavePropList->siblingBox = (NW_LMgr_Box_t *) entry->value.object;
    return KBrsrSuccess;
  }

  masterBox = NW_LMgr_SlavePropertyList_GetMasterBox (propertyList);
  propertyList = NW_LMgr_Box_PropListGet (masterBox);
  return NW_LMgr_PropertyList_Set (propertyList, key, entry);
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_LMgr_SlavePropertyList_Remove (NW_LMgr_PropertyList_t* propertyList,
                                  NW_LMgr_PropertyName_t key)
{
  NW_LMgr_Box_t* masterBox;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (propertyList,
                                     &NW_LMgr_SlavePropertyList_Class));

  masterBox = NW_LMgr_SlavePropertyList_GetMasterBox (propertyList);
  propertyList = NW_LMgr_Box_PropListGet (masterBox);
  return NW_LMgr_PropertyList_Remove (propertyList, key);
}

/* ------------------------------------------------------------------------- */
NW_LMgr_PropertyList_t*
_NW_LMgr_SlavePropertyList_Clone(const NW_LMgr_PropertyList_t* propertyList)
{
  NW_LMgr_PropertyList_t* slavePropertyList = NULL;
  NW_LMgr_Box_t* masterBox = NW_LMgr_SlavePropertyList_GetMasterBox(propertyList);
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (masterBox->propList,
                                     &NW_LMgr_SimplePropertyList_Class));
  slavePropertyList = (NW_LMgr_PropertyList_t*)
                      NW_LMgr_SlavePropertyList_New(masterBox);
  return slavePropertyList;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_LMgr_SlavePropertyList_Clear (NW_LMgr_PropertyList_t* propertyList)
{
  NW_LMgr_Box_t* masterBox;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (propertyList,
                                     &NW_LMgr_SlavePropertyList_Class));

  masterBox = NW_LMgr_SlavePropertyList_GetMasterBox (propertyList);
  propertyList = NW_LMgr_Box_PropListGet (masterBox);
  return NW_LMgr_PropertyList_Clear (propertyList);
}

/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_LMgr_Box_t*
_NW_LMgr_SlavePropertyList_GetMasterBox (const NW_LMgr_SlavePropertyList_t* thisObj)
{
  NW_LMgr_Box_t* siblingBox;
  const NW_LMgr_PropertyList_t* propertyList;

  siblingBox = NULL;
  propertyList = NW_LMgr_PropertyListOf (thisObj);
  do {
    siblingBox = NW_LMgr_SlavePropertyList_GetSiblingBox (propertyList);
    propertyList = NW_LMgr_Box_PropListGet (siblingBox);
  } while (!NW_Object_IsClass (propertyList, &NW_LMgr_SimplePropertyList_Class));
  return siblingBox;
}

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_LMgr_Box_t*
NW_LMgr_SlavePropertyList_GetFirstBox (const NW_LMgr_SlavePropertyList_t* thisObj)
{
  NW_LMgr_Box_t* siblingBox, *previousBox;
  const NW_LMgr_PropertyList_t* propertyList;

  siblingBox = NULL;
  previousBox = NULL;
  propertyList = NW_LMgr_PropertyListOf (thisObj);
  do {
    previousBox = siblingBox;
    siblingBox = NW_LMgr_SlavePropertyList_GetSiblingBox (propertyList);
    propertyList = NW_LMgr_Box_PropListGet (siblingBox);
  } while (!NW_Object_IsClass (propertyList, &NW_LMgr_SimplePropertyList_Class));
  return previousBox;
}

/* ------------------------------------------------------------------------- */
NW_LMgr_SlavePropertyList_t*
NW_LMgr_SlavePropertyList_New (NW_LMgr_Box_t* siblingBox)
{
  return (NW_LMgr_SlavePropertyList_t*)
    NW_Object_New (&NW_LMgr_SlavePropertyList_Class, siblingBox);
}
