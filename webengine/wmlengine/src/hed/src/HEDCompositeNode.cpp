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


#include "nw_hed_compositenodei.h"
#include "nw_hed_childiterator.h"
#include "nw_object_interface.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_HED_CompositeNode_Class_t NW_HED_CompositeNode_Class = {
  { /* NW_Object_Core       */
    /* super                */ &NW_Object_Aggregate_Class,
    /* querySecondary       */ _NW_Object_Core_QuerySecondary
  },
  { /* NW_Object_Secondary  */
    /* offset               */ 0
  },
  { /* NW_Object_Aggregate  */
    /* secondaryList        */ NW_HED_CompositeNode_InterfaceList,
    /* construct            */ NULL,
    /* destruct             */ _NW_HED_CompositeNode_Destruct
  },
  { /* NW_HED_CompositeNode */
    /* addChild             */ _NW_HED_CompositeNode_AddChild
  }
};

/* ------------------------------------------------------------------------- */
const NW_Object_Class_t* const NW_HED_CompositeNode_InterfaceList[] = {
  &NW_HED_CompositeNode_ICompositeNode_Class,
  NULL
};

/* ------------------------------------------------------------------------- */
const NW_HED_ICompositeNode_Class_t NW_HED_CompositeNode_ICompositeNode_Class = {
  { /* NW_Object_Core        */
    /* super                 */ &NW_HED_ICompositeNode_Class,
    /* querySecondary        */ _NW_Object_Core_QuerySecondary
  },
  { /* NW_Object_Secondary   */
    /* offset                */ offsetof (NW_HED_CompositeNode_t, NW_HED_ICompositeNode)
  },
  { /* NW_HED_ICompositeNode */
    /* removeChild           */ _NW_HED_CompositeNode_ICompositeNode_RemoveChild,
    /* getChildren           */ _NW_HED_CompositeNode_ICompositeNode_GetChildren
  }
};

/* ------------------------------------------------------------------------- *
   NW_Object_Aggregate methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
void
_NW_HED_CompositeNode_Destruct (NW_Object_Aggregate_t* aggregate)
{
  NW_HED_CompositeNode_t* thisObj;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (aggregate,
                                     &NW_HED_CompositeNode_Class));

  /* for convenience */
  thisObj = NW_HED_CompositeNodeOf (aggregate);

  /* delete all the children */
  NW_HED_CompositeNode_DeleteChildren(thisObj);
}

/* ------------------------------------------------------------------------- *
   NW_HED_ICompositeNode method implemtations
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_CompositeNode_ICompositeNode_RemoveChild (NW_HED_ICompositeNode_t* icompositeNode,
                                                  NW_HED_DocumentNode_t* childNode)
{
  NW_HED_CompositeNode_t* thisObj;
  NW_ADT_MapIterator_t iterator;
  
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (icompositeNode,
                                     &NW_HED_CompositeNode_ICompositeNode_Class));

  /* for convenience */
  thisObj = (NW_HED_CompositeNode_t *) NW_Object_Interface_GetImplementer (icompositeNode);
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_HED_CompositeNode_Class));
  
  NW_Object_Invalidate (&iterator);
  NW_TRY (status) {
    /* there is nothing we can do if the child map is not allocated */
    NW_THROW_ON_NULL (thisObj->children, status, KBrsrNotFound);

    /* create an child iterator */
    status = NW_ADT_MapIterator_Initialize (&iterator, thisObj->children);
    NW_THROW_ON_ERROR (status);
  
    while (NW_ADT_Iterator_HasMoreElements (&iterator)) {
      void* key;
      NW_HED_DocumentNode_t* node;

      status = NW_ADT_Iterator_GetNextElement (&iterator, &key);
      NW_ASSERT (status == KBrsrSuccess);
      status = NW_ADT_Map_Get (thisObj->children, &key, &node);
      NW_THROW_ON_ERROR (status);

      if (node == childNode) {
        childNode->parent = NULL;
        status = NW_ADT_Map_Remove (thisObj->children, &key);
        NW_THROW_ON_ERROR (status);
        break;
      }
    }
  }

  NW_CATCH (status) { /* empty */ }

  NW_FINALLY {
    NW_Object_Terminate (&iterator);
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_CompositeNode_ICompositeNode_GetChildren (NW_HED_ICompositeNode_t* icompositeNode,
                                                  NW_ADT_Iterator_t** childIterator)
{
  NW_HED_CompositeNode_t* thisObj;
  NW_HED_ChildIterator_t* iterator;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (icompositeNode,
                                     &NW_HED_CompositeNode_ICompositeNode_Class));
  NW_ASSERT (childIterator != NULL);

  /* for convenience */
  thisObj = (NW_HED_CompositeNode_t*) NW_Object_Interface_GetImplementer (icompositeNode);
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_HED_CompositeNode_Class));
  
  /* make sure we do in fact have children */
  if (thisObj->children == NULL) {
    return KBrsrNotFound;
  }

  /* instantiate a new NW_HED_ChildIterator */
  iterator = NW_HED_ChildIterator_New (thisObj->children);
  if (iterator == NULL) {
    return KBrsrOutOfMemory;
  }

  /* successful completion */
  *childIterator = (NW_ADT_Iterator_t*) iterator;
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- *
   NW_HED_CompositeNode methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_CompositeNode_AddChild (NW_HED_CompositeNode_t* thisObj,
                                NW_HED_DocumentNode_t* childNode,
                                void* context)
{
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj,
                                     &NW_HED_CompositeNode_Class));
  NW_ASSERT (NW_Object_IsInstanceOf (childNode,
                                     &NW_HED_DocumentNode_Class));
  
  NW_TRY (status) {
    /* create the 'children' map on-demand */
    if (thisObj->children == NULL) {
      thisObj->children =
        NW_ADT_ResizableMap_New (sizeof (void*),
                                 sizeof (NW_HED_DocumentNode_t*), 1, 1);
      NW_THROW_OOM_ON_NULL (thisObj->children, status);
    }
  
    /* add the child */
    status = NW_ADT_Map_Set (thisObj->children, &context, &childNode);
    NW_THROW_ON_ERROR (status);
  }

  NW_CATCH (status) { /* empty */ }

  NW_FINALLY {
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
NW_HED_DocumentNode_t*
_NW_HED_CompositeNode_LookupChild (NW_HED_CompositeNode_t* thisObj,
                                   void* key)
{
  NW_HED_DocumentNode_t* childNode = NULL;
  
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj,
                                     &NW_HED_CompositeNode_Class));
  
  NW_TRY (status) {
    /* there is nothing we can do if the child map is not allocated */
    NW_THROW_ON_NULL (thisObj->children, status, KBrsrNotFound);

    childNode = NULL;
    status = NW_ADT_Map_Get (thisObj->children, &key, &childNode);
    NW_THROW_ON (status, KBrsrNotFound);
    NW_THROW_ON_ERROR (status);
  }

  NW_CATCH (status) { /* empty */ }

  NW_FINALLY {
    return childNode;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_CompositeNode_DeleteChildren (NW_HED_CompositeNode_t* thisObj)
{
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_HED_CompositeNode_Class));
  
  NW_TRY (status) {
    NW_ADT_Iterator_t* iterator;

    status = NW_HED_CompositeNode_GetChildren (thisObj, &iterator);
    if (status != KBrsrSuccess) {
      if (status != KBrsrNotFound) {
        NW_THROW (status);
      }
    } else {
      /* delete each child */
      while (NW_ADT_Iterator_HasMoreElements (iterator)) {
        NW_HED_DocumentNode_t* childNode;
  
        status = NW_ADT_Iterator_GetNextElement (iterator, &childNode);
        NW_ASSERT (status == KBrsrSuccess);

        childNode->parent = NULL;
        NW_Object_Delete (childNode);
      }
  
      NW_Object_Delete (iterator);
    }
  
    /* delete the map */
    NW_Object_Delete (thisObj->children);
    thisObj->children = NULL;
  }

  NW_CATCH (status) { /* empty */ }

  NW_FINALLY { 
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_CompositeNode_DeleteChild (NW_HED_CompositeNode_t* thisObj,
                                   void* key)
{
  NW_HED_DocumentNode_t* childNode;
  
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_HED_CompositeNode_Class));
  
  NW_TRY (status) {
    /* there is nothing we can do if the child map is not allocated */
    NW_THROW_ON_NULL (thisObj->children, status, KBrsrNotFound);

    /* find the child */
    childNode = NW_HED_CompositeNode_LookupChild (thisObj, key);
    NW_THROW_ON_NULL (childNode, status, KBrsrNotFound);

    /* remove the child from the map */
    status = NW_HED_CompositeNode_RemoveChild (thisObj, childNode);
    NW_ASSERT (status == KBrsrSuccess);
  
    /* delete the node */
    NW_Object_Delete (childNode);
  }

  NW_CATCH (status) { /* empty */ }

  NW_FINALLY {
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_CompositeNode_RemoveChild (NW_HED_CompositeNode_t* thisObj,
                                   NW_HED_DocumentNode_t* childNode)
{
  NW_HED_ICompositeNode_t* icompositeNode;

  /* get the interface implementation and invoke that */
  icompositeNode = (NW_HED_ICompositeNode_t*)
    NW_Object_QueryInterface (thisObj, &NW_HED_ICompositeNode_Class);
  NW_ASSERT (icompositeNode != NULL);
  return NW_HED_ICompositeNode_RemoveChild (icompositeNode, childNode);
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_CompositeNode_GetChildren (NW_HED_CompositeNode_t* thisObj,
                                   NW_ADT_Iterator_t** childIterator)
{
  NW_HED_ICompositeNode_t* icompositeNode;

  /* get the interface implementation and invoke that */
  icompositeNode = (NW_HED_ICompositeNode_t*)
    NW_Object_QueryInterface (thisObj, &NW_HED_ICompositeNode_Class);
  NW_ASSERT (icompositeNode != NULL);
  return NW_HED_ICompositeNode_GetChildren (icompositeNode, childIterator);
}
