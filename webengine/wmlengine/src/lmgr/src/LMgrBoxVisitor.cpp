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


#include "nw_lmgr_boxvisitori.h"

#include "nw_lmgr_containerbox.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_LMgr_BoxVisitor_Class_t NW_LMgr_BoxVisitor_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_Object_Dynamic_Class,
    /* queryInterface            */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base            */
    /* interfaceList             */ NULL
  },
  { /* NW_Object_Dynamic         */
    /* instanceSize		 */ sizeof (NW_LMgr_BoxVisitor_t),
    /* construct		 */ _NW_LMgr_BoxVisitor_Construct,
    /* destruct			 */ NULL
  },
  { /* NW_LMgr_BoxVisitor	 */
    /* unused                    */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_LMgr_BoxVisitor_Construct (NW_Object_Dynamic_t* dynamicObject,
                                va_list* argp)
{
  NW_LMgr_BoxVisitor_t* thisObj;

  /* for convenience */
  thisObj = NW_LMgr_BoxVisitorOf (dynamicObject);

  /* initialize the member variables */
  thisObj->rootBox = va_arg (*argp, NW_LMgr_Box_t*);

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_LMgr_BoxVisitor_SetRootBox (NW_LMgr_BoxVisitor_t* visitor,
                               NW_LMgr_Box_t* rootBox)
{
  /* set the rootBox member variable - don't forget to clear the currentBox,
     after all this may be a completely new document! */
  visitor->rootBox = rootBox;
  visitor->currentBox = NULL;

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
NW_LMgr_Box_t*
NW_LMgr_BoxVisitor_NextBox (NW_LMgr_BoxVisitor_t* visitor,
                            NW_Bool* skipChildren)
{
  NW_ADT_Vector_Metric_t index;

  /* if this is the first time we're called, we simply return the specified
     root */
  if (visitor->currentBox == NULL) {
    return visitor->currentBox = visitor->rootBox;
  }

  /**/
  index = 0;
  while (visitor->currentBox != NULL) {
    NW_LMgr_ContainerBox_t* containerBox;
    NW_ADT_Vector_t* children;
    NW_ADT_Vector_Metric_t numChildren;

    if ((skipChildren == NULL || !*skipChildren) &&
        NW_Object_IsInstanceOf (visitor->currentBox,
                                &NW_LMgr_ContainerBox_Class)) {
      /* if we haven't finished traversing the children (or even started) we need
         to do that first */
      containerBox = NW_LMgr_ContainerBoxOf (visitor->currentBox);
      children = NW_ADT_VectorOf (NW_LMgr_ContainerBox_GetChildren (containerBox));
      numChildren = NW_ADT_Vector_GetSize (children);
      if (index < numChildren) {
        visitor->currentBox =
          *(NW_LMgr_Box_t**) NW_ADT_Vector_ElementAt (children, index);
        return visitor->currentBox;
      }
    }

    /* reset the skipChildren flag */
    if (skipChildren) {
      *skipChildren = NW_FALSE;
    }

    /* if this was the last child of the root node in which case we have no
       more work to do */
    if (visitor->currentBox == visitor->rootBox) {
      visitor->currentBox = NULL;
      continue;
    }

    /* there are no more children to traverse so look for a sibling and if
       found make that our current box and return it */
    containerBox = NW_LMgr_Box_GetParent (visitor->currentBox);
    children = NW_ADT_VectorOf(NW_LMgr_ContainerBox_GetChildren (containerBox));
    index = (NW_ADT_Vector_Metric_t)
      (NW_ADT_Vector_GetElementIndex (children, &visitor->currentBox) + 1);
    visitor->currentBox = NW_LMgr_BoxOf (containerBox);
  }

  return NULL;
}

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_LMgr_BoxVisitor_Initialize (NW_LMgr_BoxVisitor_t* boxVisitor,
                               NW_LMgr_Box_t* rootBox)
{
  return NW_Object_Initialize (&NW_LMgr_BoxVisitor_Class, boxVisitor, rootBox);
}

/* ------------------------------------------------------------------------- */
NW_LMgr_BoxVisitor_t*
NW_LMgr_BoxVisitor_New (NW_LMgr_Box_t* rootBox)
{
  return (NW_LMgr_BoxVisitor_t*)
    NW_Object_New (&NW_LMgr_BoxVisitor_Class, rootBox);
}
