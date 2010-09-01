/*
* Copyright (c) 2000 - 2004 Nokia Corporation and/or its subsidiary(-ies).
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

#include "nw_lmgr_containerboxi.h"
#include "nw_lmgr_box.h"
#include "nw_lmgr_flowbox.h"
#include "nw_lmgr_rootbox.h"
#include "nw_lmgr_splittextbox.h"
#include "nw_lmgr_statictablebox.h"
#include "nw_lmgr_statictablerowbox.h"
#include "nw_lmgr_statictablecellbox.h"
#include "nw_lmgr_activecontainerbox.h"
#include "nw_lmgr_cssproperties.h"
#include "nw_adt_resizablevector.h"
#include "nw_lmgr_eventhandler.h"
#include "GDIDeviceContext.h"
#include "nw_lmgr_slavepropertylist.h"
#include "nw_adt_vectoriterator.h"
#include "BrsrStatusCodes.h"
#include "BoxRender.h"
#include "nwx_logger.h"
#include "nwx_settings.h"
#include "ObjectUtils.h"
#include "nw_lmgr_posflowbox.h"

/* ------------------------------------------------------------------------- *
   private final methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

const
NW_LMgr_ContainerBox_Class_t  NW_LMgr_ContainerBox_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_LMgr_Box_Class,
    /* queryInterface            */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base            */
    /* interfaceList             */ NULL
  },
  { /* NW_Object_Dynamic         */
    /* instanceSize              */ sizeof (NW_LMgr_ContainerBox_t),
    /* construct                 */ _NW_LMgr_ContainerBox_Construct,
    /* destruct                  */ _NW_LMgr_ContainerBox_Destruct
  },
  { /* NW_LMgr_Box               */
    /* split                     */ _NW_LMgr_Box_Split,
    /* resize                    */ _NW_LMgr_ContainerBox_Resize,
    /* postResize                */ _NW_LMgr_Box_PostResize,
    /* getMinimumContentSize     */ _NW_LMgr_Box_GetMinimumContentSize,
    /* hasFixedContentSize       */ _NW_LMgr_Box_HasFixedContentSize,
    /* constrain                 */ _NW_LMgr_Box_Constrain,
    /* draw                      */ _NW_LMgr_Box_Draw,
    /* render                    */ _NW_LMgr_ContainerBox_Render,
    /* getBaseline               */ _NW_LMgr_ContainerBox_GetBaseline,
    /* shift                     */ _NW_LMgr_ContainerBox_Shift,
    /* clone                     */ _NW_LMgr_ContainerBox_Clone
  },
  { /* NW_LMgr_ContainerBox      */
    /* unused                    */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_ContainerBox_Construct
 * Description:  The constructor.
 * Returns:      KBrsrSuccess, KBrsrOutOfMemory.
 */

TBrowserStatusCode
_NW_LMgr_ContainerBox_Construct (NW_Object_Dynamic_t* dynamicObject,
                                 va_list* argp)
{
  NW_LMgr_ContainerBox_t* container;

  NW_TRY(status) {
  
    /* for convenience */
    container = NW_LMgr_ContainerBoxOf (dynamicObject);
  
    /* invoke our superclass constructor */
    status = _NW_LMgr_Box_Construct (dynamicObject, argp);
    _NW_THROW_ON_ERROR(status);
  
    /* initialize the member variables */
    container->children = (NW_ADT_DynamicVector_t*)
      NW_ADT_ResizableVector_New (sizeof (NW_LMgr_Box_t*), 0, 5 );
    NW_THROW_OOM_ON_NULL(container->children, status);
  }
  NW_CATCH(status) {
  }
  NW_FINALLY {
    return status;
  }
  NW_END_TRY
}

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_ContainerBox_Destruct
 * Description:  The destructor.
 * Returns:      void
 */

void
_NW_LMgr_ContainerBox_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_LMgr_ContainerBox_t* container;
  NW_ADT_Vector_Metric_t numChildren;
  NW_Int32 index;
  
  /* for convenience */
  container = NW_LMgr_ContainerBoxOf (dynamicObject);
  
  if (container->children != NULL) {
    /* before deleting the object, we must delete any children
       contained by us */
    numChildren =
      NW_ADT_Vector_GetSize (container->children);
    for (index = numChildren - 1; index >= 0; index--) {
      NW_LMgr_Box_t* child;
    
      child = *(NW_LMgr_Box_t**)
        NW_ADT_Vector_ElementAt (container->children,
                                 (NW_ADT_Vector_Metric_t) index);
      NW_Object_Delete (child);
    }
  
    /* release our resources */
    NW_Object_Delete (container->children);
  } 
}


TBrowserStatusCode
_NW_LMgr_ContainerBox_Resize( NW_LMgr_Box_t* box, NW_LMgr_FormatContext_t* context )
{
  TBrowserStatusCode status;
  NW_LMgr_ContainerBox_t* container; 

  NW_REQUIRED_PARAM( context );
  NW_ASSERT( box != NULL );

  container = NW_LMgr_ContainerBoxOf( box );
  status = NW_LMgr_ContainerBox_ResizeToChildren( container );
  if (NW_Object_IsInstanceOf(NW_LMgr_BoxOf(NW_LMgr_Box_GetParent(box)),& NW_LMgr_PosFlowBox_Class))
      {
      NW_GDI_Dimension2D_t size;
      NW_LMgr_Box_GetSizeProperties(box, &size);
      if (size.width > box->iFormatBounds.dimension.width)
          {
          box->iFormatBounds.dimension.width = size.width;
          }

      if (size.height > box->iFormatBounds.dimension.height)
          {
          box->iFormatBounds.dimension.height = size.height;
          }
      }
  return status; 
}

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_ContainerBox_Shift
 * Description:  ContainerBox::Shift shifts the container and the subtree
 *               below it.
 * Returns:      KBrsrSuccess, KBrsrOutOfMemory.
 */

TBrowserStatusCode
_NW_LMgr_ContainerBox_Shift (NW_LMgr_Box_t* box, 
                             NW_GDI_Point2D_t *delta)
{
  NW_LMgr_BoxVisitor_t * visitor = NULL;
  NW_LMgr_Box_t *child;

  NW_TRY(status) {

    NW_ASSERT(box != NULL);

    visitor = NW_LMgr_ContainerBox_GetBoxVisitor(box);
    NW_THROW_OOM_ON_NULL(visitor, status);
 
    /* Invoke base-class shift on this box and all children */

    while ((child = NW_LMgr_BoxVisitor_NextBox(visitor, NULL)) != NULL) {
      status = NW_LMgr_Box_Class.NW_LMgr_Box.shift(child, delta);
      _NW_THROW_ON_ERROR(status);
    }
  }
  NW_CATCH(status) {
  }
  NW_FINALLY {
    NW_Object_Delete(visitor);
    return status;
  }
  NW_END_TRY
}

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_ContainerBox_Clone
 * Description:  Clones of containers only inherit the property list.  They
 *               also have their "Clone" flag set.  The children are not 
 *               copied into the new box.
 * Returns:      The new box, or NULL in case of an error.
 */
NW_LMgr_Box_t*
_NW_LMgr_ContainerBox_Clone (NW_LMgr_Box_t* box)
{
  NW_LMgr_Box_t *newBox = NULL;

  /* parameter assertion block */
  NW_ASSERT(NW_Object_IsInstanceOf (box, &NW_LMgr_ContainerBox_Class));

  /* Create the new box */
  newBox = (NW_LMgr_Box_t*) NW_LMgr_ContainerBox_New (0);
  if (newBox == NULL) {
    return NULL;
  }

  /* Copy the property list, if one exists */
  if (box->propList != NULL) {
    newBox->propList = NW_LMgr_PropertyList_Clone (box->propList);
  }
  else{
    /* instantiate a new SimplePropertyList object */
    newBox->propList = (NW_LMgr_PropertyList_t*)
              NW_LMgr_SlavePropertyList_New (NULL);
  }
  if (newBox->propList == NULL) {
    NW_Object_Delete (newBox);
    return NULL;
  }

  return newBox;
}

/* ------------------------------------------------------------------------- *
   public/protected final methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_LMgr_ContainerBox_Collapse (NW_LMgr_ContainerBox_t* thisObj)
{
  NW_LMgr_PropertyList_t* propertyList;
  NW_LMgr_Property_t prop;
  NW_LMgr_ContainerBox_t* siblingBox;
  NW_LMgr_Box_t* childBox;
  TBrowserStatusCode status = KBrsrSuccess;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_LMgr_ContainerBox_Class));

  propertyList = NW_LMgr_Box_PropListGet(NW_LMgr_BoxOf(thisObj));
  if (propertyList == NULL){
    return KBrsrSuccess;
  }

  NW_ASSERT (NW_Object_IsInstanceOf (propertyList, &NW_LMgr_PropertyList_Class));

  /* the master box will take care of coolapsing the slave boxes */
  if (NW_Object_IsInstanceOf(propertyList, &NW_LMgr_SlavePropertyList_Class)){
    return KBrsrSuccess;
  }

  /* if the propertylist is not slave propertylist, it definitely is a simple
     property list, and this box is a master box - so start collapse
   */

  /* implement collapse */

  /* Get sibling box */
  prop.value.object = NULL;
  (void)NW_LMgr_PropertyList_Get(propertyList, NW_CSS_Prop_sibling, &prop);
  if (prop.value.object == NULL){
    return KBrsrSuccess;
  }
  
  while(prop.value.object)
  {
    NW_ADT_Vector_Metric_t childCount;

    siblingBox = NW_LMgr_ContainerBoxOf(prop.value.object);

    /* This condition should never happen unless sibling boxes are not created
    properly. Should it be an ASSERT instead?? */
    if (!NW_Object_IsInstanceOf(NW_LMgr_BoxOf(siblingBox)->propList, 
                               &NW_LMgr_SlavePropertyList_Class))
    {
      return KBrsrFailure;
    }

    /* Insert your sibling's children in master container box */
    /* PROBLEM: Make Container_GetChild recognize NW_ADT_Vector_AtEnd */
    childCount = NW_LMgr_ContainerBox_GetChildCount(siblingBox);
    while (childCount > 0)
    {
      childCount--;
      childBox = NW_LMgr_ContainerBox_GetChild(siblingBox, childCount);
      NW_ASSERT(childBox);
       
      /* Insert ChildAt will remove the child from the old parent 
         which is the sibling box
         */
      /* The following method should be more efficient - we get the child
         from the old box, and then again search for it to remove it. 
         RemoveChild should return a box, so that we can use that box to 
         attach it somewhere else */
      status = NW_LMgr_ContainerBox_InsertChildAt(thisObj, childBox, 0);
      if (status != KBrsrSuccess) {
        return status;
      }

    }
    
    /* Get next sibling box */
    propertyList = NW_LMgr_Box_PropListGet(NW_LMgr_BoxOf(siblingBox));
    NW_ASSERT (NW_Object_IsInstanceOf (propertyList, &NW_LMgr_PropertyList_Class));

    prop.value.object = NULL;
    (void)NW_LMgr_PropertyList_Get(propertyList, NW_CSS_Prop_sibling, &prop);

    /* delete the sibling box */
    NW_Object_Delete(siblingBox);

    /* If we come back in a loop to the master box, then stop */
    if (prop.value.object == thisObj)
      break;
  }
  /* We also want to remove the sibling property on the master box,
     since we are collapsing the split containers */
  (void)NW_LMgr_Box_RemoveProperty(NW_LMgr_BoxOf(thisObj), NW_CSS_Prop_sibling);

  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_ContainerBox_InsertChildAt     
 * Description:  Inserts a child at the given index.  If a child is already
 *               attached to a parent, it detaches it first.
 * Returns:      KBrsrSuccess if successful, KBrsrFailure if the index 
 *               is out of bounds, KBrsrOutOfMemory.
 */

TBrowserStatusCode
NW_LMgr_ContainerBox_InsertChildAt (NW_LMgr_ContainerBox_t* container,
                                    NW_LMgr_Box_t* child,
                                    NW_ADT_Vector_Metric_t where)
{
  NW_LMgr_ContainerBox_t* parent;

  NW_TRY(status) {

    NW_ASSERT(child != NULL);

    /* Remove the child from its existing parent, if necessary */
    parent = NW_LMgr_Box_GetParent (child);
    if (parent != NULL) {
      status = NW_LMgr_ContainerBox_RemoveChild (parent, child);
      _NW_THROW_ON_ERROR(status);
    }
  
    /* Insert the child into our 'children' vector */
    NW_THROW_OOM_ON_NULL(NW_ADT_DynamicVector_InsertAt(container->children,
                                                       &child, where),
                         status);

    /* Set the parent field of the Box - note that we do this as
       a 'friend' class */
    NW_LMgr_Box_SetParent (child, container);
  }
  NW_CATCH(status) {
  }
  NW_FINALLY {
    return status;
  }
  NW_END_TRY
}

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_ContainerBox_AddChild   
 * Description:  Adds a child to the end of the container.
 * Returns:      KBrsrSuccess, KBrsrOutOfMemory.
 */

TBrowserStatusCode
NW_LMgr_ContainerBox_AddChild (NW_LMgr_ContainerBox_t* container,
                               NW_LMgr_Box_t* child)
{
  return NW_LMgr_ContainerBox_InsertChildAt (container, child,
                                             NW_ADT_Vector_AtEnd);
}

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_ContainerBox_InsertChild
 * Description:  Inserts a child immediately before the box pointed to by
 *               "where".
 *               If "where" is not a child of the container, KBrsrFailure
 *               is returned.
 * Returns:      KBrsrSuccess, KBrsrFailure, KBrsrOutOfMemory.
 */

TBrowserStatusCode
NW_LMgr_ContainerBox_InsertChild (NW_LMgr_ContainerBox_t* container,
                                  NW_LMgr_Box_t* child,
                                  NW_LMgr_Box_t* where)
{
  NW_ADT_Vector_Metric_t index;

  index = NW_ADT_Vector_GetElementIndex (container->children,
                                         &where);
  if (index == NW_ADT_Vector_AtEnd) {
    return KBrsrFailure;
  }
  return NW_LMgr_ContainerBox_InsertChildAt (container, child, index);
}

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_ContainerBox_InsertChildAft
 * Description:  Inserts a child immediately following the box pointed to by 
 *               "where".  If "where" is not a child of the container, 
 *               KBrsrFailure is returned.
 * Returns:      KBrsrSuccess, KBrsrFailure, KBrsrOutOfMemory.
 */

TBrowserStatusCode
NW_LMgr_ContainerBox_InsertChildAft (NW_LMgr_ContainerBox_t* container,
                                     NW_LMgr_Box_t* child,
                                     NW_LMgr_Box_t* where)
{
  NW_ADT_Vector_Metric_t index;

  index = NW_ADT_Vector_GetElementIndex (container->children,
                                         &where);
  if (index == NW_ADT_Vector_AtEnd) {
    return KBrsrFailure; /* TODO: return a more descriptive error code */
  }
  if ((index + 1)
      < NW_ADT_Vector_GetSize(container->children)) {
    return NW_LMgr_ContainerBox_InsertChildAt (container, child, 
                                               ((NW_ADT_Vector_Metric_t)
                                                (index+1)));
  } else {
    return NW_LMgr_ContainerBox_AddChild (container, child);
  }
}

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_ContainerBox_AppendChildrenOf   
 * Description:  Appends the children of "source" to the container pointed
 *               to by "container".
 * Returns:      KBrsrSuccess, KBrsrOutOfMemory.
 */

TBrowserStatusCode
NW_LMgr_ContainerBox_AppendChildrenOf (NW_LMgr_ContainerBox_t* container,
                                       NW_LMgr_ContainerBox_t* source)
{
  NW_TRY(status) {
    while (NW_ADT_Vector_GetSize (source->children) != 0) {
      NW_LMgr_Box_t* child;

      child = *(NW_LMgr_Box_t**) NW_ADT_Vector_ElementAt (source->children, 0);
      NW_ASSERT(child != NULL);

      status = NW_LMgr_ContainerBox_AddChild (container, child);
      _NW_THROW_ON_ERROR(status);
    }
  }
  NW_CATCH(status) {
  }
  NW_FINALLY {
    return status;
  }
  NW_END_TRY
}

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_ContainerBox_ReplaceChild
 * Description:  Replaces "originalChild" with "newChild".
 * Returns:      KBrsrSuccess, KBrsrFailure if the original child
 *               is not inside "container", KBrsrOutOfMemory.
 */

TBrowserStatusCode
NW_LMgr_ContainerBox_ReplaceChild (NW_LMgr_ContainerBox_t* container,
                                   NW_LMgr_Box_t* originalChild,
                                   NW_LMgr_Box_t* newChild)
{
  NW_ADT_Vector_Metric_t index;

  NW_TRY(status) {
    index = NW_ADT_Vector_GetElementIndex(container->children,
                                          &originalChild);
    if (index == NW_ADT_Vector_AtEnd) {
      NW_THROW_STATUS(status, KBrsrFailure);
    }

    status = NW_LMgr_ContainerBox_InsertChildAt (container, newChild, index);
    _NW_THROW_ON_ERROR(status);

    status = NW_LMgr_ContainerBox_RemoveChild (container, originalChild);
    _NW_THROW_ON_ERROR(status);
  }
  NW_CATCH(status) {
  }
  NW_FINALLY {
    return status;
  }
  NW_END_TRY
}

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_ContainerBox_GetChildIndex
 * Description:  Finds the index of "child" in the container.
 * Returns:      The index, or NW_ADT_Vector_AtEnd, if the child in not inside
 *               container.
 */

NW_ADT_Vector_Metric_t
NW_LMgr_ContainerBox_GetChildIndex (NW_LMgr_ContainerBox_t* container,
                                    NW_LMgr_Box_t* child)
{

  return NW_ADT_Vector_GetElementIndex (container->children,
                                        &child);

}

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_ContainerBox_RemoveChild
 * Description:  Removes the child pointed to by "component" from the container.
 * Returns:      KBrsrSuccess, KBrsrFailure if the original child
 *               is not inside "container", KBrsrOutOfMemory.
 */

TBrowserStatusCode
NW_LMgr_ContainerBox_RemoveChild (NW_LMgr_ContainerBox_t* container,
                                  NW_LMgr_Box_t* component)
{
  NW_TRY(status) {
    /* parameter assertion block */
    NW_ASSERT(NW_Object_IsInstanceOf (container, &NW_LMgr_ContainerBox_Class));
    NW_ASSERT(NW_Object_IsInstanceOf (component, &NW_LMgr_Box_Class));

    /* search the 'children' vector for the element */
    status = NW_ADT_DynamicVector_RemoveElement (container->children,
                                                 &component);
    _NW_THROW_ON_ERROR(status);
  
  /* clear the parent field of the component - note that we do this as a
  'friend' of the class */
    NW_LMgr_Box_SetParent (component, NULL);
  }
  NW_CATCH(status) {
  }
  NW_FINALLY {
    return status;
  }
  NW_END_TRY
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_LMgr_ContainerBox_RemoveChildren (NW_LMgr_ContainerBox_t* thisObj)
{
  NW_ADT_VectorIterator_t iterator;

  NW_Object_Invalidate (&iterator);
  NW_TRY (status) {
    (void) NW_ADT_VectorIterator_Initialize (&iterator, thisObj->children,
                                             NW_ADT_Vector_AtFront,
                                             NW_ADT_Vector_AtEnd,
                                             NW_ADT_IteratorDirection_Increment);

    while (NW_ADT_Iterator_HasMoreElements (&iterator)) {
      NW_LMgr_Box_t* child;

      status = NW_ADT_Iterator_GetNextElement (&iterator, &child);
      NW_THROW_ON_ERROR (status);

      status = NW_LMgr_ContainerBox_RemoveChild (thisObj, child);
      NW_THROW_ON_ERROR (status);
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
NW_LMgr_ContainerBox_DeleteChildren (NW_LMgr_ContainerBox_t* thisObj)
{
  NW_LMgr_Box_t** child;

  NW_TRY (status) {
    while ((child = (NW_LMgr_Box_t**) NW_ADT_Vector_ElementAt (thisObj->children, 0)) != NULL) {
      status = NW_LMgr_ContainerBox_RemoveChild (thisObj, *child);
      NW_THROW_ON_ERROR (status);

      NW_Object_Delete(*child);
    }
  }

  NW_CATCH (status) { /* empty */ }

  NW_FINALLY {
    return status;
  } NW_END_TRY
}


/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_ContainerBox_GetChildCount
 * Description:  Counts the number of children of the container.
 * Returns:      The number of children, 0 if the container is empty.
 */

NW_ADT_Vector_Metric_t
NW_LMgr_ContainerBox_GetChildCount (NW_LMgr_ContainerBox_t* container)
{
  return NW_ADT_Vector_GetSize (container->children);
}

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_ContainerBox_GetChild
 * Description:  Gets the child at "index".  
 * Returns:      The child box, or NULL if the index is out of bounds.
 */

NW_LMgr_Box_t*
NW_LMgr_ContainerBox_GetChild (NW_LMgr_ContainerBox_t* container,
                               NW_ADT_Vector_Metric_t index)
{
  NW_LMgr_Box_t** entry;

  entry =
    (NW_LMgr_Box_t**) NW_ADT_Vector_ElementAt (container->children, index);
  if (entry != NULL) {
    return *entry;
  }
  return NULL;
}

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_ContainerBox_SplitAtIndex
 * Description:  SplitAtIndex returns a clone of the container containing the
 *               first "index" children.  The remaining children stay in the 
 *               original container.  If "index" is 0, the split container
 *               will be empty.  If "index" points to the last child, the
 *               original container will remain empty.
 * Returns:      The split container, or NULL if an error occured.
 */

NW_LMgr_Box_t*
NW_LMgr_ContainerBox_SplitAtIndex (NW_LMgr_ContainerBox_t* container,
                                   NW_ADT_Vector_Metric_t index){
  NW_ADT_Vector_Metric_t childCount, childIndex;
  NW_LMgr_ContainerBox_t *splitContainer = NULL;
  NW_LMgr_Box_t *childBox;
  NW_LMgr_Property_t prop;
  NW_LMgr_PropertyValue_t dirValue;
  
  childCount = NW_LMgr_ContainerBox_GetChildCount (container);

  /* Verify input parameters */
  if (childCount <= 1 || index >= childCount) {
    NW_ASSERT(0);
    return NULL;
  }

  NW_TRY(status) {
    /* Get the text direction */
    dirValue.token = NW_CSS_PropValue_ltr;
    status = NW_LMgr_Box_GetPropertyValue(NW_LMgr_BoxOf(container), 
                                          NW_CSS_Prop_textDirection, 
                                          NW_CSS_ValueType_Token,
                                          &dirValue);
    NW_THROW_ON(status, KBrsrOutOfMemory);

    /* Create the new container */
    splitContainer =
      (NW_LMgr_ContainerBox_t*)NW_LMgr_Box_Clone ((NW_LMgr_Box_t*)container);
    NW_THROW_OOM_ON_NULL(splitContainer, status);

  /* Move children to the new container */
    for (childIndex = 0; childIndex < index; childIndex++) {
      childBox = NW_LMgr_ContainerBox_GetChild(container, 0);

      /* AddChild will remove the child from the old parent */
      status = NW_LMgr_ContainerBox_AddChild(splitContainer, childBox);
      _NW_THROW_ON_ERROR(status);
    }

  /* Get and set sibling property on the cloned box's property list */
  prop.value.object = NULL;
  (void)NW_LMgr_Box_GetPropertyFromList(NW_LMgr_BoxOf(container), 
                                        NW_CSS_Prop_sibling, &prop);
  if (prop.value.object == NULL){
    /* Set sibling property on the cloned box's property list */
    prop.type = NW_ADT_ValueType_Object | NW_CSS_ValueType_Copy;
    prop.value.object = container;
    status = NW_LMgr_Box_SetProperty(NW_LMgr_BoxOf(splitContainer), 
                                     NW_CSS_Prop_sibling, &prop);
    NW_ASSERT (status == KBrsrSuccess);
  }

  /* Set sibling property on the master box's property list */
  prop.type = NW_ADT_ValueType_Object | NW_CSS_ValueType_Copy;
  prop.value.object = splitContainer;
  status = NW_LMgr_Box_SetProperty(NW_LMgr_BoxOf(container), 
                                   NW_CSS_Prop_sibling, &prop);
  NW_ASSERT (status == KBrsrSuccess);

#if 0
#pragma message (__FILE__ ": ::MEMO::BE AWARE that this code should be commented out. It does exist in CORE.")
// If this code is added LTR support should be added too
    status = NW_LMgr_Box_SetProperty((NW_LMgr_Box_t*)splitContainer, 
                                     NW_CSS_Prop_rightPadding, 
                                     &prop);
    _NW_THROW_ON_ERROR(status);
    status = NW_LMgr_Box_SetProperty((NW_LMgr_Box_t*)splitContainer, 
                                     NW_CSS_Prop_rightMargin, 
                                     &prop);
    _NW_THROW_ON_ERROR(status);
    status = NW_LMgr_Box_SetProperty((NW_LMgr_Box_t*)container, 
                                     NW_CSS_Prop_leftPadding, 
                                     &prop);
    _NW_THROW_ON_ERROR(status);
    status = NW_LMgr_Box_SetProperty((NW_LMgr_Box_t*)container, 
                                     NW_CSS_Prop_leftMargin, 
                                     &prop);
    _NW_THROW_ON_ERROR(status);
#endif
  }
  NW_CATCH(status) {
    NW_Object_Delete(splitContainer);

    splitContainer = NULL;
  }
  NW_FINALLY {
    return (NW_LMgr_Box_t*)splitContainer;
  }
  NW_END_TRY
}

/* -------------------------------------------------------------------------- *
 * Function:     NW_LMgr_ContainerBox_GetBaseline
 * Description:  Returns the baseline of the container.
 *               According to the CSS spec, the baseline for boxes containing 
 *               multiple lines of text is the baseline of the first line of
 *               text. 
 *               We assume here that this also applies to boxes that contain
 *               mixed text and other content: i.e. that this applies to all
 *               containers by default.
 * Returns:      The baseline of the first child, or 0 if the container is
 *               empty.
 */

/* TODO:  The box may not begin at the top of the container, so its baseline
 * BUG!   might be larger than is currently calculated. 
 */

NW_GDI_Metric_t
_NW_LMgr_ContainerBox_GetBaseline(NW_LMgr_Box_t* box)
{
  NW_LMgr_Box_t *firstChild;

  if (NW_LMgr_ContainerBox_GetChildCount(NW_LMgr_ContainerBoxOf(box)) != 0){
    firstChild = NW_LMgr_ContainerBox_GetChild(NW_LMgr_ContainerBoxOf(box), 0);
    NW_ASSERT(firstChild != NULL);
    return NW_LMgr_Box_GetBaseline(firstChild);
  } else {
    return 0;
  }
}

/* -------------------------------------------------------------------------- *
 * Function:     NW_LMgr_ContainerBox_Render
 * Description:  Renders the container and its children.  
 * Returns:      KBrsrSuccess, KBrsrOutOfMemory
 */

TBrowserStatusCode
_NW_LMgr_ContainerBox_Render (NW_LMgr_Box_t* box,
                              CGDIDeviceContext* deviceContext,
                              NW_GDI_Rectangle_t* clipRect,
                              NW_LMgr_Box_t *currentBox,
                              NW_Uint8 flags,
                              NW_Bool* hasFocus,
                              NW_Bool* skipChildren,
                              NW_LMgr_RootBox_t *rootBox )
{
  NW_GDI_Rectangle_t oldClip = {{0, 0}, {0, 0}};
  NW_GDI_Rectangle_t newClip;
  NW_GDI_Rectangle_t realClip;
  NW_GDI_Rectangle_t visibleArea;
  NW_GDI_Rectangle_t viewBounds;
  NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetDisplayBounds(box);
  NW_LMgr_PropertyValue_t value;
  NW_LMgr_PropertyValue_t floatVal;
  NW_LMgr_PropertyValue_t visibilityVal;
  NW_Bool clipRectChanged = NW_FALSE;
  NW_Bool containerVisible = NW_FALSE;
  void** ptr = NULL;
  NW_Bool cachePresent = NW_FALSE;

  NW_TRY(status) 
  {
    // Should we draw floats? 
    if (!(flags & NW_LMgr_Box_Flags_DrawFloats)) 
      {
      floatVal.token = NW_CSS_PropValue_none;
      NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_float, NW_CSS_ValueType_Token, &floatVal);
      if (floatVal.token != NW_CSS_PropValue_none) 
        {
        NW_THROW_SUCCESS( status );
        }
      }
    // check if the this box has focus
    // if hasFocus is already set to true by any parent
    // then it should not be overuled.
    if( !(*hasFocus) && currentBox == box )
      {
      *hasFocus = NW_TRUE;
      }
    // Get the view bounds 
    viewBounds = *(deviceContext->DisplayBounds());
    viewBounds.point = *(deviceContext->Origin());

    // Determine the part of the box within the clip.  
    if( !NW_GDI_Rectangle_Cross( clipRect, &boxBounds, &visibleArea ) ) 
      {
      // out of the view
      goto drawChildren;
      }
    // Is the box visible on-screen?  If not, stil try to draw
    // the children 
    if( !NW_GDI_Rectangle_Cross( &viewBounds, &visibleArea, NULL ) ) 
      {
      goto drawChildren;
      }
    // If the clip is not visible on-screen, we 
    // draw neither the Container nor its children;
    // in case we are calculating the extents, we will still
    // have to peek into all children 
    if (!NW_GDI_Rectangle_Cross( clipRect, &viewBounds, &realClip ) ) 
      {
      goto drawChildren;
      }
    
    // Save the old clip rect 
    oldClip = deviceContext->ClipRect();
    // Set the new clip rect 
    deviceContext->SetClipRect(  &realClip );
    clipRectChanged = NW_TRUE;

    // Check if visibility val is set to not visible.  In that case
    // we do not draw the Container, but we still try to draw the
    // children (the Visiblity prop may be overriden) 
    visibilityVal.token = NW_CSS_PropValue_visible;
    (void) NW_LMgr_Box_GetPropertyValue (box, NW_CSS_Prop_visibility,
                                         NW_CSS_ValueType_Token, &visibilityVal);
  
    if (visibilityVal.token != NW_CSS_PropValue_visible) 
      {
      goto drawChildren;
      }

    // make the box draw itself 
    status = NW_LMgr_Box_Draw( box, deviceContext, *hasFocus );
    containerVisible = NW_TRUE;
    _NW_THROW_ON_ERROR (status);

	// Add the box and the clip to the cache 
    cachePresent = NW_LMgr_Box_Update_RenderCache (rootBox, box, &realClip, &ptr);
    if (cachePresent)
        {
		NW_THROW_OOM_ON_NULL(ptr, status);
		}

    // Even if the Container itself is not visible, its children
    // might be 
    // note that this is overruled in smallscreen mode as we do not
    // render any children of a non visible container.
drawChildren:
 
    // Modify the clip rectangle, if necessary 
    newClip = *clipRect;
    value.token = NW_CSS_PropValue_visible;
    (void)NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_overflow,
                                       NW_CSS_ValueType_Token, &value);

    if( value.token == NW_CSS_PropValue_hidden ) 
      {
      NW_LMgr_FrameInfo_t padding; 
      NW_LMgr_FrameInfo_t border;
      NW_GDI_Rectangle_t tempClip = boxBounds;

      NW_LMgr_Box_GetPadding( box, &padding, ELMgrFrameAll );
      NW_LMgr_Box_GetBorderWidth( box, &border, ELMgrFrameAll );
      tempClip.point.x = (NW_GDI_Metric_t)(boxBounds.point.x + padding.left
                                           + border.left);
      tempClip.point.y = (NW_GDI_Metric_t)(boxBounds.point.y + padding.top
                                           + border.top);
      tempClip.dimension.width = (NW_GDI_Metric_t)(boxBounds.dimension.width
                                                   - padding.left
                                                   - padding.right
                                                   - border.left - border.right);
      tempClip.dimension.height = (NW_GDI_Metric_t)(boxBounds.dimension.height
                                                    - padding.top
                                                    - padding.bottom
                                                    - border.top
                                                    - border.bottom);

      if (!NW_GDI_Rectangle_Cross(&tempClip, clipRect, &newClip)) 
        {
        // children are out of view
        containerVisible = NW_FALSE;
        NW_THROW_SUCCESS(status);
        }
      else
        {
        // modify cliprect for the children.
  		  *clipRect = newClip;
        }
      }
    }
  NW_CATCH(status) 
    {
    }
  NW_FINALLY 
    {
    // skip children unless the container is visible
    *skipChildren = (NW_Bool)( containerVisible == NW_TRUE ? NW_FALSE : NW_TRUE );
    if( clipRectChanged == NW_TRUE )
      {
      /* Reset the clip rect */
      deviceContext->SetClipRect( &oldClip);
      }
    return status;
    }
  NW_END_TRY
  }

/* ------------------------------------------------------------------------- *
 * Function:    NW_LMgr_ContainerBox_ResizeToChildren 
 * Description: Resize makes the container box just big enough to enclose all of 
 *              its children. Although the box is invisible and has no effect
 *              on the enclosing flow, these bounds are available to the
 *              UI event mechanism to identify which action container should
 *              handle an event. Note a limitation here: the container box
 *              can't actually do this correctly until the children have all
 *              been formatted: something the container can't really do itself
 *              because it lacks a formatting context. Since formatting contexts
 *              are not attached to boxes, there is no way to walk up the
 *              tree looking for one either. 
 * Returns:     KBrsrSuccess
 */

TBrowserStatusCode
NW_LMgr_ContainerBox_ResizeToChildren(NW_LMgr_ContainerBox_t* container)
{
  NW_ADT_Vector_Metric_t numChildren;
  NW_LMgr_Box_t* thisBox = NW_LMgr_BoxOf(container);
  NW_ADT_Vector_Metric_t index;
  const NW_ADT_Vector_t *children;
  NW_LMgr_PropertyValue_t propValue;
  NW_LMgr_FrameInfo_t borders, padding;
  NW_GDI_Rectangle_t containerBounds = NW_LMgr_Box_GetFormatBounds(thisBox);
  NW_LMgr_RootBox_t* rootBox = NW_LMgr_Box_GetRootBox( thisBox );
  TBrowserStatusCode status = KBrsrSuccess;
   NW_Bool FirstVisibleChild = NW_TRUE;

  NW_ASSERT(container != NULL);
  
  children = NW_LMgr_ContainerBox_GetChildren(container);

  numChildren = NW_ADT_Vector_GetSize(children);
   
  // Initialize the values for the case there are no visible children.
  containerBounds.point.x = containerBounds.point.y = 0;
  containerBounds.dimension.width = containerBounds.dimension.height = 0;
  NW_LMgr_Box_SetFormatBounds( thisBox, containerBounds );

  for (index = 0; index < numChildren; index++) {
    NW_LMgr_Box_t* child;
    
    child = *(NW_LMgr_Box_t**) NW_ADT_Vector_ElementAt (children, index);
    containerBounds = NW_LMgr_Box_GetFormatBounds(thisBox);

    // If the child box is not displayable, ignore it.
    propValue.token = NW_CSS_PropValue_display_inline;
    NW_LMgr_Box_GetPropertyValue(child, NW_CSS_Prop_display, NW_CSS_ValueType_Token, &propValue);
    if (propValue.token == NW_CSS_PropValue_none){
      continue;
      }
    
    // This step is skipped in vertical layout mode (see the comment in 
    // CLMgr_BoxFormatHandler::UpdateDisplayBounds) or if the child is an object-box.
    // Object-boxes are always paired with a container-box that has it as the 
    // only child.  Because of this it should never skip the object-box -- otherwise
    // the container-box will have 0,0 dimensions.
    //if( !NW_LMgr_RootBox_GetSmallScreenOn( rootBox ) && !ObjectUtils::IsObjectBox(*child, EFalse))
    //    {
        // If the child box is a float, ignore it.
    //    propValue.token = NW_CSS_PropValue_none;
    //    NW_LMgr_Box_GetPropertyValue(child, NW_CSS_Prop_float, NW_CSS_ValueType_Token, &propValue);
    //    if (propValue.token != NW_CSS_PropValue_none)
    //        {
    //        continue;
    //        }
    //    }
    
    // If the child box is a container that has no visible children, ignore it.
    NW_GDI_Rectangle_t childBoxBounds = NW_LMgr_Box_GetFormatBounds( child );
    if ((childBoxBounds.dimension.height == 0) && (childBoxBounds.dimension.width == 0)){
       if (NW_Object_IsInstanceOf(child, &NW_LMgr_ContainerBox_Class)){
        continue;
        }
      }
    
    if(FirstVisibleChild){
		FirstVisibleChild = NW_FALSE;
      NW_LMgr_Box_SetFormatBounds( NW_LMgr_BoxOf( container ), childBoxBounds );
      }
    else{
      if(childBoxBounds.point.x < containerBounds.point.x){
        containerBounds.point.x = childBoxBounds.point.x;
        NW_LMgr_Box_SetFormatBounds( thisBox, containerBounds );
        }
      
      if(childBoxBounds.point.y < containerBounds.point.y){
        containerBounds.point.y = childBoxBounds.point.y;
        NW_LMgr_Box_SetFormatBounds( thisBox, containerBounds );
        }
      
      if((childBoxBounds.point.x + childBoxBounds.dimension.width) >
        (containerBounds.point.x + containerBounds.dimension.width)){
          containerBounds.dimension.width = (NW_GDI_Metric_t)
          (childBoxBounds.point.x + childBoxBounds.dimension.width - 
             containerBounds.point.x);
          NW_LMgr_Box_SetFormatBounds( thisBox, containerBounds );
        }
      
      if((childBoxBounds.point.y + childBoxBounds.dimension.height) >
        (containerBounds.point.y + containerBounds.dimension.height)){
          containerBounds.dimension.height = (NW_GDI_Metric_t)
          (childBoxBounds.point.y + childBoxBounds.dimension.height - 
             containerBounds.point.y);
          NW_LMgr_Box_SetFormatBounds( thisBox, containerBounds );
        }
      }
    }
  containerBounds = NW_LMgr_Box_GetFormatBounds(thisBox);

  if ((containerBounds.dimension.width != 0) && 
      (containerBounds.dimension.height != 0)) {
    /* Now take into account the borders and padding */
    NW_LMgr_Box_GetBorderWidth(thisBox, &borders, ELMgrFrameAll );
    NW_LMgr_Box_GetPadding(thisBox, &padding, ELMgrFrameAll );
    containerBounds.point.x = (NW_GDI_Metric_t)(containerBounds.point.x - borders.left - padding.left);
    containerBounds.point.y = (NW_GDI_Metric_t)(containerBounds.point.y - borders.top - padding.top);
    containerBounds.dimension.width = (NW_GDI_Metric_t)(containerBounds.dimension.width + 
      borders.left + padding.left + borders.right + padding.right);
    containerBounds.dimension.height = (NW_GDI_Metric_t)(containerBounds.dimension.height + 
      borders.top + padding.top + borders.bottom + padding.bottom);
    }
  NW_LMgr_Box_SetFormatBounds( thisBox, containerBounds );
  return status;
}

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_ContainerBox_Initialize
 * A public method to add any anonymous block container boxes to
 * a block container that contains both block and inline content.
 * This assures that all content in a block box participates
 * in the same format context (either block or inline).  This becomes
 * essential when margins are collapsed.  CSS2 p98.
 */

TBrowserStatusCode
NW_LMgr_ContainerBox_Initialize(NW_LMgr_ContainerBox_t *container)
    {
    NW_TRY (status) 
        {
        NW_ADT_Vector_Metric_t index;
        const NW_ADT_Vector_t *children;
        NW_LMgr_Box_t* child;
        NW_LMgr_ContainerBox_t *newContainer;
        NW_LMgr_PropertyValue_t floatVal;

        children = NW_LMgr_ContainerBox_GetChildren(container);

        index = 0;
        while (index < NW_ADT_Vector_GetSize(children)) {

        child = *(NW_LMgr_Box_t**) NW_ADT_Vector_ElementAt (children, index);

        if (NW_Object_IsInstanceOf(child, &NW_LMgr_SplitTextBox_Class)) 
          {
            NW_Object_Delete(child);
            child = NULL;
            continue;
          }

        else if (NW_Object_IsInstanceOf(child, &NW_LMgr_AbstractTextBox_Class))
            {
            if( NW_LMgr_AbstractTextBox_IsBlank(NW_LMgr_AbstractTextBoxOf(child)) )
                {
                NW_Object_Delete(child);
                child = NULL;
                continue;
                }
            }

        else if (NW_Object_IsInstanceOf(child, &NW_LMgr_FlowBox_Class) ||
          NW_Object_IsClass(child, &NW_LMgr_StaticTableBox_Class) ||
          NW_Object_IsClass(child, &NW_LMgr_StaticTableRowBox_Class)) {

        floatVal.token = NW_CSS_PropValue_none;
        status = NW_LMgr_Box_GetPropertyValue(child, NW_CSS_Prop_float, NW_CSS_ValueType_Token, &floatVal);
        if (status == KBrsrOutOfMemory) {
          return KBrsrOutOfMemory;
        }

        if (floatVal.token == NW_CSS_PropValue_none) {

          newContainer = NW_LMgr_ContainerBox_New(0);
          NW_THROW_OOM_ON_NULL(newContainer, status);

          status = NW_LMgr_ContainerBox_AppendChildrenOf(newContainer, NW_LMgr_ContainerBoxOf(child));
          _NW_THROW_ON_ERROR(status);

          status = NW_LMgr_ContainerBox_ReplaceChild(container, child, NW_LMgr_BoxOf(newContainer));
          _NW_THROW_ON_ERROR(status);

          (void) NW_LMgr_Box_SetPropList(NW_LMgr_BoxOf(newContainer), NW_LMgr_Box_PropListGet(child));
          (void) NW_LMgr_Box_SetPropList(child, NULL);
          NW_LMgr_Box_RemoveProperty(NW_LMgr_BoxOf(newContainer), NW_CSS_Prop_display);
          NW_LMgr_Box_RemoveProperty(NW_LMgr_BoxOf(newContainer), NW_CSS_Prop_float);

          NW_Object_Delete(child);
          child = NULL;
        }
        }

        index ++;
        }

    } NW_CATCH (status) {
    } NW_FINALLY {
    return KBrsrSuccess;
    } NW_END_TRY
    }

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_ContainerBox_InitializeSSL
 * A public method to remove empty text child box in small screen mode
 */

TBrowserStatusCode
NW_LMgr_ContainerBox_InitializeSSL(NW_LMgr_ContainerBox_t *container)
    {
    TBrowserStatusCode status = KBrsrSuccess;
    NW_ADT_Vector_Metric_t index;
    const NW_ADT_Vector_t *children;
    NW_LMgr_Box_t* child;

    children = NW_LMgr_ContainerBox_GetChildren(container);

    index = 0;
    while (index < NW_ADT_Vector_GetSize(children)) 
        {
        child = *(NW_LMgr_Box_t**) NW_ADT_Vector_ElementAt (children, index);

        if (NW_Object_IsInstanceOf(child, &NW_LMgr_AbstractTextBox_Class))
            {
            if( NW_LMgr_AbstractTextBox_IsBlank(NW_LMgr_AbstractTextBoxOf(child)) )
                {
                NW_Object_Delete(child);
                child = NULL;
                continue;
                }
            }
        index ++;
        }

    return status;
    }

NW_Bool NW_LMgr_ContainerBox_IsLastChild(NW_LMgr_Box_t* box)
    {
    return box == NW_LMgr_BoxOf(NW_LMgr_ContainerBox_GetChild(NW_LMgr_Box_GetParent((box)),
                                (NW_ADT_Vector_Metric_t) (NW_LMgr_ContainerBox_GetChildCount(NW_LMgr_Box_GetParent((box))) - 1 )));
    }

/* ------------------------------------------------------------------------- *
  convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_LMgr_ContainerBox_t*
NW_LMgr_ContainerBox_New (NW_ADT_Vector_Metric_t numProperties)
{
  return (NW_LMgr_ContainerBox_t*)
    NW_Object_New (&NW_LMgr_ContainerBox_Class, numProperties);
}





