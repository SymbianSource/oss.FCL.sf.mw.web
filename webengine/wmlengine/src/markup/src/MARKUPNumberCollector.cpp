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


#include "nw_markup_numbercollectori.h"
#include "nw_lmgr_containerbox.h"
#include "nw_lmgr_textbox.h"
#include "nw_fbox_inputboxi.h"
#include "nw_fbox_selectbox.h"
#include "nw_lmgr_activecontainerbox.h"
#include "nw_fbox_passwordbox.h"
#include "nw_lmgr_imgcontainerbox.h"
#include "nw_fbox_optionbox.h"
#include "BrsrStatusCodes.h"
#include "MemoryManager.h"

/* ------------------------------------------------------------------------- *
   private methods
 * ------------------------------------------------------------------------- */

static
TBrowserStatusCode
NW_Markup_NumberCollector_GetOptGrpBoxText (NW_FBox_OptGrpBox_t* optgrpBox,
                                   NW_ADT_DynamicVector_t *dynamicVector)
{
    NW_Text_t* text = NULL;
    NW_ADT_Vector_Metric_t numItems;
    NW_ADT_Vector_Metric_t index;


    NW_TRY (status) {

        void** entry;
        numItems = NW_ADT_Vector_GetSize(NW_ADT_VectorOf(optgrpBox->children));
        for(index = 0; index < numItems; index++)
        {
            NW_FBox_FormBox_t* formBox;
            formBox = *(NW_FBox_FormBox_t**)
            NW_ADT_Vector_ElementAt(optgrpBox->children, index);

            if (NW_Object_IsInstanceOf(formBox, &NW_FBox_OptionBox_Class))
            {
                text = NW_Text_Copy (NW_FBox_OptionBoxOf(formBox)->text, NW_TRUE);
                NW_THROW_OOM_ON_NULL (text, status);
                /* Save the text object in the dynamicVector */
                entry = NW_ADT_DynamicVector_InsertAt (dynamicVector, &text,
                                             NW_ADT_Vector_AtEnd);
                NW_THROW_OOM_ON_NULL (entry, status);
            }
        }
    }
    NW_CATCH (status) {
        NW_Object_Delete (text);
    }
    NW_FINALLY {
        return status;
    }NW_END_TRY
}

static
TBrowserStatusCode
NW_Markup_NumberCollector_GetSelectBoxText (NW_LMgr_Box_t* box,
                                   NW_ADT_DynamicVector_t *dynamicVector)
{
    NW_Text_t* text = NULL;
    NW_ADT_Vector_Metric_t numItems;
    NW_ADT_Vector_Metric_t index;


    NW_TRY (status) {

        void** entry;
        NW_FBox_SelectBox_t* selectBox = NW_FBox_SelectBoxOf(box);
        numItems = NW_ADT_Vector_GetSize(NW_ADT_VectorOf(selectBox->children));
        for(index = 0; index < numItems; index++)
        {
            NW_FBox_FormBox_t* formBox;
            formBox = *(NW_FBox_FormBox_t**)
            NW_ADT_Vector_ElementAt(selectBox->children, index);

            if (NW_Object_IsInstanceOf(formBox, &NW_FBox_OptionBox_Class))
            {
                text = NW_Text_Copy (NW_FBox_OptionBoxOf(formBox)->text, NW_TRUE);
                NW_THROW_OOM_ON_NULL (text, status);
                /* Save the text object in the dynamicVector */
                entry = NW_ADT_DynamicVector_InsertAt (dynamicVector, &text,
                                             NW_ADT_Vector_AtEnd);
                NW_THROW_OOM_ON_NULL (entry, status);
            }
            else if (NW_Object_IsInstanceOf(formBox, &NW_FBox_OptGrpBox_Class))
            {
                status = NW_Markup_NumberCollector_GetOptGrpBoxText (NW_FBox_OptGrpBoxOf(formBox), dynamicVector);
                NW_THROW_ON_ERROR(status);
            }
        }
    }
    NW_CATCH (status) {
        NW_Object_Delete (text);
    }
    NW_FINALLY {
        return status;
    }NW_END_TRY
}

/** ----------------------------------------------------------------------- **
    @method:      NW_Markup_NumberCollector_GetText

    @synopsis:    Returns all phone numbers associated with the implementer.

    @scope:       private
    @access:      final

    @parameters:
       [in, out] NW_LMgr_Box_t* box
                  Pointer to a box object from the BoxTree.

       [out] NW_Text_t** text
                  Pointer to store the text object pointer.

       [out] NW_Bool* isCopy
                  Pointer to store a Bool flag indicating that the returned
                  text object has to be freed if not needed.

    @description: The function returns a pointer to a text object from input
                  and text boxes.

                  checks if the current box is a text box or input box. A
                  text object is extructed and its pointer is returned.
                  Exception: Input boxes of the Password type are ignored
                  for security reasons.

    @returns:     NW_Bool
       [NW_TRUE]
                  When a text object is returned.

       [NW_FALSE]
                  When text object is not found. NULL is returned in place
                  of the pointer to a text object
 ** ----------------------------------------------------------------------- **/
static
TBrowserStatusCode
NW_Markup_NumberCollector_GetText (NW_Markup_NumberCollector_t* numberCollector,
                                   NW_LMgr_Box_t* box,
                                   NW_Text_t** text,
                                   NW_Bool* isCopy)
{
  *text = NULL;
  *isCopy = NW_FALSE;
 
  if (NW_Object_IsInstanceOf(box, &NW_LMgr_TextBox_Class)) {
    *text = NW_LMgr_TextBox_GetText(NW_LMgr_TextBoxOf(box));
    return KBrsrSuccess;
  } else if (NW_Object_IsInstanceOf(box, &NW_FBox_InputBox_Class) && 
            !NW_Object_IsInstanceOf(box, &NW_FBox_PasswordBox_Class)) {
    *text = (NW_FBox_InputBoxOf(box))->value;
    return KBrsrSuccess;
  } else if (NW_Object_IsInstanceOf(box, &NW_LMgr_ActiveContainerBox_Class)) {
    *text = NW_Markup_NumberCollector_GetHrefAttr(numberCollector, box);
    *isCopy = NW_TRUE;
    return KBrsrSuccess;
  }else if (NW_Object_IsDerivedFrom(box, &NW_LMgr_ImgContainerBox_Class)) {
    *text = NW_Markup_NumberCollector_GetAltAttr(numberCollector, box);
    *isCopy = NW_TRUE;
    return KBrsrSuccess;
  }
  else if (NW_Object_IsInstanceOf(box, &NW_LMgr_ImgContainerBox_Class)) {
    *text = (NW_LMgr_ImgContainerBoxOf(box))->altText;
    return KBrsrSuccess;
  }

  return KBrsrNotFound;
}

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_Markup_NumberCollector_Class_t NW_Markup_NumberCollector_Class = {
  { /* NW_Object_Core          */
    /* super                   */ &NW_Object_Aggregate_Class,
    /* querySecondary          */ _NW_Object_Core_QuerySecondary
  },
  { /* NW_Object_Secondary     */
    /* offset                  */ 0
  },
  { /* NW_Object_Aggregate     */
    /* secondaryList           */ _NW_Markup_NumberCollector_SecondaryList,
    /* construct               */ NULL,
    /* destruct                */ NULL
  },
  { /* NW_HED_INumberCollector */
    /* getBoxTree              */ NULL,
    /* boxIsValid              */ _NW_Markup_NumberCollector_BoxIsValid,
    /* getHrefAttr             */ NULL,
    /* getAltAttr              */ NULL
  }
};

/* ------------------------------------------------------------------------- */
const
NW_Object_Class_t* const _NW_Markup_NumberCollector_SecondaryList[] = {
  &NW_Markup_NumberCollector_INumberCollector_Class,
  NULL
};

/* ------------------------------------------------------------------------- */
const NW_HED_INumberCollector_Class_t NW_Markup_NumberCollector_INumberCollector_Class = {
  { /* NW_Object_Core          */
    /* super                   */ &NW_HED_INumberCollector_Class,
    /* querySecondary          */ _NW_Object_Core_QuerySecondary
  },
  { /* NW_Object_Secondary     */
    /* offset                  */ offsetof (NW_Markup_NumberCollector_t, NW_HED_INumberCollector)
  },
  { /* NW_HED_INumberCollector */
    /* collect                 */ _NW_Markup_NumberCollector_INumberCollector_Collect
  }
};

/* ------------------------------------------------------------------------- *
   NW_Markup_NumberCollector methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_Bool
_NW_Markup_NumberCollector_BoxIsValid (NW_Markup_NumberCollector_t* numberCollector,
                                       NW_LMgr_Box_t* box)
{
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (numberCollector,
                                     &NW_Markup_NumberCollector_Class));
  NW_ASSERT (NW_Object_IsInstanceOf (box, &NW_LMgr_Box_Class));

  NW_REQUIRED_PARAM(numberCollector);
  NW_REQUIRED_PARAM(box);
  
  /* default implementation is that all boxes are valid */
  return NW_TRUE;
}

/* ------------------------------------------------------------------------- *
   NW_HED_INumberCollector methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Markup_NumberCollector_INumberCollector_Collect (NW_HED_INumberCollector_t* numberCollector,
                                                     NW_ADT_DynamicVector_t* dynamicVector)
{
  NW_Markup_NumberCollector_t* thisObj;
  NW_LMgr_BoxVisitor_t* visitor = NULL;
  NW_Text_t* text = NULL;
  NW_Bool isCopy = NW_FALSE;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (numberCollector,
                                     &NW_Markup_NumberCollector_INumberCollector_Class));
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicVector,
                                     &NW_ADT_DynamicVector_Class));

  /* for convenience */
  thisObj = (NW_Markup_NumberCollector_t*) NW_Object_Interface_GetImplementer (numberCollector);
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_Markup_NumberCollector_Class));

  NW_TRY (status) {
    NW_LMgr_Box_t* boxTree;
    NW_LMgr_Box_t* childBox;
    NW_Bool skipChildren;

    boxTree = NW_Markup_NumberCollector_GetBoxTree (thisObj);
    NW_ASSERT (NW_Object_IsInstanceOf (boxTree, &NW_LMgr_Box_Class));
    visitor = NW_LMgr_ContainerBox_GetBoxVisitor (NW_LMgr_ContainerBoxOf (boxTree));
    NW_THROW_OOM_ON_NULL (visitor, status);

    /* Iterate through all children */
    skipChildren = NW_FALSE;
    while ((childBox = NW_LMgr_BoxVisitor_NextBox (visitor, &skipChildren)) != NULL) {
      void** entry;

      /* ensure that we want to process this particular box */
      if (!NW_Markup_NumberCollector_BoxIsValid (thisObj, childBox)) {
        skipChildren = NW_TRUE;
        continue;
      }

      if (NW_Object_IsInstanceOf(childBox, &NW_FBox_SelectBox_Class) )
      {
          status = NW_Markup_NumberCollector_GetSelectBoxText (childBox, dynamicVector);
          NW_THROW_ON_ERROR (status);
      }
      else
      {

      status = NW_Markup_NumberCollector_GetText (thisObj, childBox, &text, &isCopy);
      if (status == KBrsrNotFound) {
        continue;
      }
      NW_THROW_ON_ERROR (status);

      if (!isCopy && text != NULL) {
        text = NW_Text_Copy (text, NW_TRUE);
        NW_THROW_OOM_ON_NULL (text, status);
      }
      
      /* Save the text object in the dynamicVector */
      entry = NW_ADT_DynamicVector_InsertAt (dynamicVector, &text,
                                             NW_ADT_Vector_AtEnd);
      NW_THROW_OOM_ON_NULL (entry, status);
    }
   }
  }
  
  NW_CATCH (status) {
    NW_Object_Delete (text);
  }
  
  NW_FINALLY {
    NW_Object_Delete (visitor);
    return status;
  } NW_END_TRY
}
