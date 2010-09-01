/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies).
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


#include "nw_basicforms_xhtmlcontrolseti.h"
#include "nw_adt_resizablevector.h"
#include "nw_lmgr_box.h"
#include "BrsrStatusCodes.h"

#if 0
#if (defined(_DEBUG)) || (defined(_ENABLE_LOGS))
#define XFL_LOGGING 1
#include <FLogger.h>

    _LIT(KLogFileName, "XhtmlFormLiaison.log");
    _LIT(KLogDirName, "Browser");
#endif
#endif
 /* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_XHTML_ControlSet_Class_t NW_XHTML_ControlSet_Class = {
  { /* NW_Object_Core      */
    /* super               */ &NW_Object_Dynamic_Class,
    /* queryInterface      */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base      */
    /* interfaceList       */ NULL
  },
  { /* NW_Object_Dynamic   */
    /* instanceSize        */ sizeof (NW_XHTML_ControlSet_t),
    /* construct           */ _NW_XHTML_ControlSet_Construct,
    /* destruct            */ _NW_XHTML_ControlSet_Destruct
  },
  { /* NW_XHTML_ControlSet */
    /* unused              */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_ControlSet_Construct (NW_Object_Dynamic_t* dynamicObject,
                                va_list* argp)
{
  NW_XHTML_ControlSet_t* thisObj;
  NW_ADT_Vector_Metric_t capacity;

  /* parameter assertions */
  NW_ASSERT (dynamicObject != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject,
                                     &NW_XHTML_ControlSet_Class));
  NW_ASSERT (argp != NULL);

  /* for convenience */
  thisObj = NW_XHTML_ControlSetOf (dynamicObject);

  /* initialize member variables */
  thisObj->valueVector = va_arg (*argp, NW_ADT_DynamicVector_t*);
  capacity = NW_ADT_DynamicVector_GetCapacity (thisObj->valueVector);
  thisObj->nodeVector  = (NW_ADT_DynamicVector_t*)
    NW_ADT_ResizableVector_New (sizeof (NW_DOM_ElementNode_t*), capacity, 5);

  if(thisObj->nodeVector == NULL) {
    return KBrsrOutOfMemory;
  }
  thisObj->boxVector  = (NW_ADT_DynamicVector_t*)
    NW_ADT_ResizableVector_New (sizeof (NW_LMgr_Box_t*), capacity, 5);
  if(thisObj->boxVector == NULL) {
    return KBrsrOutOfMemory;
  }

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
void
_NW_XHTML_ControlSet_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_XHTML_ControlSet_t* thisObj;

  /* parameter assertions */
  NW_ASSERT (dynamicObject != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject,
                                     &NW_XHTML_ControlSet_Class));

  thisObj = NW_XHTML_ControlSetOf (dynamicObject);

  /* release our resources; notice that nodeVector is NULL at thisObj point
     if _NW_XHTML_ControlSet_GetValueVector has been called */
  NW_Object_Delete (thisObj->valueVector);
  NW_Object_Delete (thisObj->nodeVector);
  NW_Object_Delete (thisObj->boxVector);
}

/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_ControlSet_AddNode (NW_XHTML_ControlSet_t* thisObj,
                              NW_DOM_ElementNode_t* node,
                              NW_ADT_Vector_Metric_t* index)
{
  /* parameter assertions */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_XHTML_ControlSet_Class));
  NW_ASSERT (node != NULL);
  NW_ASSERT (index != NULL);

  /* insert the node and update the index to reflect the actual position
     within the vector */
    if (NW_ADT_DynamicVector_InsertAt (thisObj->nodeVector, &node,
                                     NW_ADT_Vector_AtEnd) == NULL) {
      return KBrsrOutOfMemory;
    }
  *index = (NW_ADT_Vector_Metric_t)
    (NW_ADT_Vector_GetSize (thisObj->nodeVector) - 1);

  #if defined(XFL_LOGGING)
      {
	  TBuf16<300> buf;
      NW_String_t *node_name =NW_String_new();;
      NW_DOM_Node_getNodeName(node, node_name); 

    //  NW_Uint8* text = NW_TinyTree_EBuffer_AddressAt(node->tree->ebuffer, node->source_offset);

      buf.Format( _L("XhtmlFormLiaison::AddNode index=%d, node=%x, node_name=%s\n"), *index, node, 
          node_name->storage);
      RFileLogger::Write( KLogDirName, KLogFileName, EFileLoggingModeAppend, buf );
      NW_String_delete(node_name);
      }
#endif

  /* if this is a new node, add it to the end of the nodeVector list */
      if (*index == NW_ADT_Vector_GetSize (thisObj->valueVector)) {
          void* value;
          
          /* Make sure we don't erase a previous value if it exists. */
          if ( NW_ADT_Vector_GetSize (thisObj->nodeVector) >
              NW_ADT_Vector_GetSize (thisObj->valueVector) ) {
              /* Added a new node that doesn't have a value.  Set to NULL. */
              value = NULL;
              if (NW_ADT_DynamicVector_InsertAt (thisObj->valueVector, &value,
                  *index) == NULL) {
                  return KBrsrOutOfMemory;
                  }
              }
          }

      if (*index == NW_ADT_Vector_GetSize (thisObj->boxVector)) {
          void* box;
          
          /* Make sure we don't erase a previous value if it exists. */
          if ( NW_ADT_Vector_GetSize (thisObj->nodeVector) >
              NW_ADT_Vector_GetSize (thisObj->boxVector) ) {
              /* Added a new node that doesn't have a box.  Set to NULL. */
              box = NULL;
              if (NW_ADT_DynamicVector_InsertAt (thisObj->boxVector, &box,
                  *index) == NULL) {
                  return KBrsrOutOfMemory;
                  }
              }
  
          }
  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_ControlSet_SetValue (NW_XHTML_ControlSet_t* thisObj,
                               NW_DOM_ElementNode_t* node,
                               NW_Object_t* value)
{
  NW_ADT_Vector_Metric_t index;
  NW_Object_t** entry;

  /* parameter assertions */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_XHTML_ControlSet_Class));
  NW_ASSERT (node != NULL);

  /* ensure that the node exists */
  index = NW_ADT_Vector_GetElementIndex (thisObj->nodeVector, &node);
  if (index == NW_ADT_Vector_AtEnd) {
    return KBrsrFailure;
  }

  /* retrieve the pointer to the entry */
  entry = NW_ADT_Vector_ElementAt (thisObj->valueVector, index);
  if (entry == NULL) {
    return KBrsrFailure;
  }

  /* if the old value is not the same as the new, delete it and install the 
     new */
  if(*entry != value) {
    NW_Object_Delete (*entry);
  }
  *entry = value;

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
NW_Object_t*
_NW_XHTML_ControlSet_GetValue (NW_XHTML_ControlSet_t* thisObj,
                               NW_DOM_ElementNode_t* node)
{
  NW_ADT_Vector_Metric_t index;

  /* parameter assertions */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_XHTML_ControlSet_Class));
  NW_ASSERT (node != NULL);

  /* get the node's position in the nodeVector list */
  index = NW_ADT_Vector_GetElementIndex (thisObj->nodeVector, &node);
  if (index != NW_ADT_Vector_AtEnd) {
    NW_Object_t** value;

    /* retrieve the value from the valueVector list at the same index */
    value = NW_ADT_Vector_ElementAt (thisObj->valueVector, index);
    if (value != NULL) {
      return *value;
    }
  }

  return NULL;
}

/* ------------------------------------------------------------------------- */
NW_DOM_ElementNode_t*
_NW_XHTML_ControlSet_GetControl (NW_XHTML_ControlSet_t* thisObj,
                                 NW_ADT_Vector_Metric_t index)
{
  NW_Object_t** node;

  /* parameter assertions */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_XHTML_ControlSet_Class));
  NW_ASSERT (index < NW_ADT_Vector_GetSize (thisObj->nodeVector));

  node = NW_ADT_Vector_ElementAt (thisObj->nodeVector, index);
  if (node != NULL) {
    return (NW_DOM_ElementNode_t*) *node;
  }
  return NULL;
}

/* ------------------------------------------------------------------------- */
NW_ADT_DynamicVector_t*
_NW_XHTML_ControlSet_GetValueVector (NW_XHTML_ControlSet_t* thisObj)
{
  NW_ADT_DynamicVector_t* valueVector;

  /* parameter assertions */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_XHTML_ControlSet_Class));

  valueVector = thisObj->valueVector;
  if (valueVector != NULL) {
    /* set our valueVector to NULL to make sure we won't delete it */
    thisObj->valueVector = NULL;
  }

  return valueVector;
}


/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_XHTML_ControlSet_t*
NW_XHTML_ControlSet_New (NW_ADT_DynamicVector_t* valueVector)
{
  return (NW_XHTML_ControlSet_t*)
    NW_Object_New (&NW_XHTML_ControlSet_Class, valueVector);
}
