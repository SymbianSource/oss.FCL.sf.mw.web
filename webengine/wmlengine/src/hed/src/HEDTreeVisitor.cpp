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


#include "nw_hed_treevisitori.h"
#include "nw_hed_compositecontenthandler.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_HED_TreeVisitor_Class_t NW_HED_TreeVisitor_Class = {
  { /* NW_Object_Core     */
    /* super              */ &NW_Object_Dynamic_Class,
    /* queryInterface     */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base     */
    /* interfaceList      */ NULL
  },
  { /* NW_Object_Dynamic  */
    /* instanceSize       */ sizeof (NW_HED_TreeVisitor_t),
    /* construct          */ _NW_HED_TreeVisitor_Construct,
    /* destruct           */ _NW_HED_TreeVisitor_Destruct
  },
  { /* NW_HED_TreeVisitor */
    /* unused             */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   NW_Object_Dynamic methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_TreeVisitor_Construct (NW_Object_Dynamic_t* dynamicObject,
                               va_list* argList)
{
  NW_HED_TreeVisitor_t* thisObj;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject,
                                     &NW_HED_TreeVisitor_Class));
  NW_ASSERT (argList != NULL);

  /* for convenience */
  thisObj = NW_HED_TreeVisitorOf (dynamicObject);

  /* initialize the member variables */
  thisObj->rootNode = va_arg (*argList, NW_HED_DocumentNode_t*);
  if (!NW_Object_IsInstanceOf (thisObj->rootNode,
                               &NW_HED_DocumentNode_Class)) {
    return KBrsrBadInputParam;
  }
  thisObj->currentNode = thisObj->rootNode;

  return KBrsrSuccess;
}

void
_NW_HED_TreeVisitor_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
   NW_HED_TreeVisitor_t* thisObj;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject,
                                     &NW_HED_TreeVisitor_Class));

  /* for convenience */
  thisObj = NW_HED_TreeVisitorOf (dynamicObject);

  if (thisObj->childIterator != NULL) {
      NW_Object_Delete (thisObj->childIterator);
      thisObj->childIterator = NULL;
      }
}
/* ------------------------------------------------------------------------- *
   NW_HED_TreeVisitor methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_Bool
_NW_HED_TreeVisitor_HasNext (NW_HED_TreeVisitor_t* thisObj)
{
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_HED_TreeVisitor_Class));

  return (NW_Bool) (thisObj->currentNode == NULL ? NW_FALSE : NW_TRUE);
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_TreeVisitor_GetNext (NW_HED_TreeVisitor_t* thisObj,
                             NW_HED_DocumentNode_t** documentNode)
{
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_HED_TreeVisitor_Class));
  NW_ASSERT (documentNode != NULL);

  NW_TRY (status) {

    /* set the 'documentNode' with the current node */
    NW_THROW_ON_NULL (thisObj->currentNode, status, KBrsrNotFound);
    *documentNode = thisObj->currentNode;

    /* does the node have children */
    status =
      NW_HED_TreeVisitor_NextSibling (thisObj, thisObj->currentNode, NULL);
    if (status != KBrsrNotFound) {
      NW_THROW_ON_ERROR (status);
      NW_THROW_SUCCESS (status);
    }

    /* climb the tree looking for siblings */
    while (status != KBrsrSuccess) {
      NW_HED_DocumentNode_t* parentNode;

      /* we're done if the current node is the root node */
      if (thisObj->currentNode == thisObj->rootNode) {
        thisObj->currentNode = NULL;
        break;
      }

      /* get the next sibling */
      parentNode = NW_HED_DocumentNode_GetParentNode (thisObj->currentNode);
      status = NW_HED_TreeVisitor_NextSibling (thisObj, parentNode,
                                               thisObj->currentNode);
      if (status != KBrsrSuccess) {
        if (status != KBrsrNotFound) {
          NW_THROW (status);
        }

        thisObj->currentNode = parentNode;
      }
    }

  } NW_CATCH (status) {
  } NW_FINALLY {
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_TreeVisitor_NextSibling (NW_HED_TreeVisitor_t* thisObj,
                                 NW_HED_DocumentNode_t* parentNode,
                                 NW_HED_DocumentNode_t* node)
{
  NW_TRY (status) {
    NW_HED_DocumentNode_t* documentNode;

    /* if there is no childIterator cached, we need to get one from our
       parent */
    if (thisObj->childIterator == NULL) {
      NW_HED_ICompositeNode_t* compositeNode;

      /* get the childIterator */
      compositeNode = (NW_HED_ICompositeNode_t*)
        NW_Object_QueryInterface (parentNode, &NW_HED_ICompositeNode_Class);
      NW_ASSERT (compositeNode != NULL);
      status = NW_HED_ICompositeNode_GetChildren (compositeNode,
                                                 &thisObj->childIterator);
      NW_THROW_ON_ERROR (status);

      /* find out position in the iterator */
      if (node != NULL) {
        while (NW_ADT_Iterator_HasMoreElements (thisObj->childIterator)) {

          status = NW_ADT_Iterator_GetNextElement (thisObj->childIterator,
                                                   &documentNode);
          NW_THROW_ON_ERROR (status);
          if (documentNode == node) {
            break;
          }
        }
      }
    }

    /* do we have any siblings? */
    status =
      NW_ADT_Iterator_GetNextElement (thisObj->childIterator, &documentNode);
    if (status == KBrsrNotFound) {
      NW_Object_Delete (thisObj->childIterator);
      thisObj->childIterator = NULL;
      NW_THROW (status);
    }
    NW_THROW_ON_ERROR (status);

    /* set the current node */
    thisObj->currentNode = documentNode;

  } NW_CATCH (status) {
  } NW_FINALLY {
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_TreeVisitor_Initialize (NW_HED_TreeVisitor_t* treeVisitor,
                                NW_HED_DocumentNode_t* rootNode)
{
  return NW_Object_Initialize (&NW_HED_TreeVisitor_Class, treeVisitor,
                               rootNode);
}

/* ------------------------------------------------------------------------- */
NW_HED_TreeVisitor_t*
_NW_HED_TreeVisitor_New (NW_HED_DocumentNode_t* rootNode)
{
  return (NW_HED_TreeVisitor_t*)
    NW_Object_New (&NW_HED_TreeVisitor_Class, rootNode);
}
