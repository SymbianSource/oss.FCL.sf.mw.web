/*
* Copyright (c) 2002 - 2002 Nokia Corporation and/or its subsidiary(-ies).
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


#include "nwx_string.h"
#include "nw_hed_iformfiller.h"
#include "nw_markup_formfiller.h"
#include "nw_markup_formfilleri.h"
#include "nw_lmgr_containerbox.h"
#include "nw_lmgr_textbox.h"
#include "nw_lmgr_rootbox.h"
#include "nw_fbox_inputbox.h"
#include "nw_fbox_inputskin.h"
#include "nw_fbox_selectbox.h"
#include "nw_fbox_selectskin.h"
#include "nw_fbox_epoc32selectskin.h"
#include "nw_fbox_passwordbox.h"
#include "nw_fbox_textareabox.h"
#include "nw_fbox_textareaskin.h"
#include "nw_fbox_formliaison.h"
#include "nw_fbox_formboxutils.h"
#include "nwx_generic_dlg.h"
#include "NW_FBox_Validator.h"
#include "BrsrStatusCodes.h"

static const NW_Ucs2 Ecom_prefix[] = {'E','c','o','m','_','\0'};

/* These extern declarations are necessary because the header that declares
   these functions includes C++ code.
*/
extern "C" TBrowserStatusCode
NW_FBox_Epoc32InputSkin_SetValue(NW_FBox_InputSkin_t* inputSkin, 
                                 const NW_Ucs2* string,
                                 NW_Text_Length_t length);
                                 
extern "C" TBrowserStatusCode
NW_FBox_Epoc32InputSkin_Redraw(NW_FBox_InputSkin_t* inputSkin);

/* ------------------------------------------------------------------------- *
private methods
* ------------------------------------------------------------------------- */
/** ----------------------------------------------------------------------- **
    @method:      NW_Markup_FormFiller_IsFormFillerBox

    @synopsis:    Determines if the box is a valid FormFiller box.

    @scope:       private
    @access:      final

    @parameters:
       [in, out]  NW_LMgr_Box_t* box
                  Pointer to a box object from the BoxTree.

    @description: Determines if the given box is a valid FormFiller box,
                  that is one of the following types: a text type input box, 
                  a single select box, a textarea box, or a password box.

    @returns:     NW_Bool
       [NW_TRUE]
                  When box is a valid FormFiller box.

       [NW_FALSE]
                  When box is a not valid FormFiller box.
 ** ----------------------------------------------------------------------- **/

NW_Bool 
NW_Markup_FormFiller_IsFormFillerBox(NW_LMgr_Box_t* box)
{
    if (NW_Object_IsInstanceOf(box, &(NW_FBox_SelectBox_Class))) {
        return NW_TRUE;
    } else if (NW_Object_IsInstanceOf(box, &(NW_FBox_TextAreaBox_Class))) {
      return NW_TRUE;
    } else  if (NW_Object_Core_GetClass(box) == &(NW_FBox_InputBox_Class)) {
      return NW_TRUE;
    } else if (NW_Object_IsInstanceOf(box, &(NW_FBox_PasswordBox_Class))) {
      return NW_TRUE;
    }

    return NW_FALSE;
}  

/** ----------------------------------------------------------------------- **
    @method:      NW_Markup_FormFiller_IsECMLBox

    @synopsis:    Returns the name attribute if the box is an ECML box,
                  NULL otherwise.

    @scope:       private
    @access:      final

    @parameters:
       [in, out]  NW_LMgr_Box_t* box
                  Pointer to a box object from the BoxTree.

       [out]      NW_Bool* isFormFillerBox
                  Pointer to store a Bool flag indicating if the 
                  given box is a valid FormFiller box.

    @description: Determines if the given box is a ECML box.
                  An ECML box is a valid FormFiller box whose name
                  name attribute value begins with the prefix "Ecom_".
                  The routine determines if box is a valid FormFiller box
                  and sets isFormFillerBox accordingly, then returns the 
                  name attribute of the box if it's and ECML box, NULL otherwise.  

    @returns:     NW_Ucs2*
       
                  Returns the name attribute if the box is an ECML box,
                  NULL otherwise.
 ** ----------------------------------------------------------------------- **/

static
NW_Ucs2* 
NW_Markup_FormFiller_IsECMLBox(NW_LMgr_Box_t* box, NW_Bool* isFormFillerBox)
{

    NW_FBox_FormLiaison_t  *formLiaison = NULL;
    void* controlId;
    NW_Ucs2* name;
    TBrowserStatusCode status;

    if (!NW_Markup_FormFiller_IsFormFillerBox(box)) {
      *isFormFillerBox = NW_FALSE;
      return NULL;
    }
    *isFormFillerBox = NW_TRUE;    

    formLiaison = NW_FBox_FormBox_GetFormLiaison (box);
    controlId = NW_FBox_FormBox_GetFormCntrlID(box);
    status = NW_FBox_FormLiaison_GetStringName(formLiaison, controlId, &name);
    if (status != KBrsrSuccess){
      return NULL;
    }
    
    if (!NW_Str_Strncmp(Ecom_prefix, name, (NW_Int32) NW_Str_Strlen(Ecom_prefix))) {
      return name;
    } else {
      /* clean up the temporary text object (if we have one) */
      NW_Str_Delete(name);
      return NULL;
    }
}   

/** ----------------------------------------------------------------------- **
    @method:      NW_Markup_FormFiller_ExistingValue

    @synopsis:    Determines if the box currently has a value.

    @scope:       private
    @access:      final

    @parameters:
       [in, out]  NW_LMgr_Box_t* box
                  Pointer to a box object from the BoxTree.

       [out]      NW_Bool* existingVal
                  Pointer to store a Bool flag indicating if the 
                  given box currently has a value.

    @description: Determines if the given box currently has a value               
                  and sets existingVal accordingly. 

    @returns:     TBrowserStatusCode
       [KBrsrSuccess]
                  When routine was able to determine if the box has a value.

       [KBrsrOutOfMemory]
                  When an allocation failure is encountered.
 ** ----------------------------------------------------------------------- **/

static
TBrowserStatusCode 
NW_Markup_FormFiller_ExistingValue(NW_LMgr_Box_t* box, NW_Bool* existingVal)
{
  NW_Ucs2* buffer = NULL; 
  NW_Text_t* text = NULL;
  
  NW_TRY (status) {
    *existingVal = NW_FALSE;
    
    if (NW_Object_IsInstanceOf(box, &(NW_FBox_SelectBox_Class))) {
      NW_FBox_SelectBox_t* selectBox;
      NW_FBox_SelectSkin_t* selectSkin;
      NW_Uint32 length = 0;

      selectBox = NW_FBox_SelectBoxOf(box);
      selectSkin = NW_FBox_SelectSkinOf(NW_FBox_FormBox_GetSkin(selectBox));   
      status = NW_FBox_Epoc32SelectSkin_GetValue(NW_FBox_Epoc32SelectSkinOf(selectSkin), &buffer);
      NW_THROW_ON_ERROR (status);
      
      if (buffer != NULL) {
        length = NW_Str_Strlen(buffer);
        if (length) {
          *existingVal = NW_TRUE;
        }
      }
    } 
    else {
      NW_FBox_EditableString_t* editableString; 
      
      editableString = NW_FBox_InputBox_GetEditableString (NW_FBox_InputBoxOf(box));
      //text = (NW_Text_t*) NW_FBox_EditableString_GetText (editableString, 0);
#pragma message (__FILE__"   TODO: Replace NW_FBox_EditableString_GetText() " )
	  if( editableString )
      {
        if (NW_Text_GetCharCount (editableString)) {
          *existingVal = NW_TRUE;
        }  
      }
    }
  } NW_CATCH (status) {
    if (status != KBrsrOutOfMemory) {
      status = KBrsrSuccess;
    }
  } NW_FINALLY {
    NW_Str_Delete(buffer);
    NW_Object_Delete(text); 
    return status;
  } NW_END_TRY
}

/** ----------------------------------------------------------------------- **
    @method:      NW_Markup_FormFiller_UpdateBox

    @synopsis:    Updates the FormFiller box with the given value.

    @scope:       private
    @access:      final

    @parameters:
       [in, out]  NW_LMgr_Box_t* box
                  Pointer to a FormFiller box.

       [in, out]  NW_Ucs2* value
                  Value to update the box with.

    @description: Updates the FormFiller box with the given value.

    @returns:     TBrowserStatusCode
       [KBrsrSuccess]
                  When box was updated successfully.

       [KBrsrFailure]
                  When box could not be updated (other than out of memory).

       [KBrsrOutOfMemory]
                  When an allocation failure is encountered.
 ** ----------------------------------------------------------------------- **/

static
TBrowserStatusCode
NW_Markup_FormFiller_UpdateBox(NW_LMgr_Box_t* box, NW_Ucs2* value)
{
  NW_TRY (status) {
    if (NW_Object_IsInstanceOf(box, &(NW_FBox_SelectBox_Class))) {
      NW_FBox_SelectBox_t* selectBox;
      NW_FBox_SelectSkin_t* selectSkin;
      NW_FBox_Epoc32SelectSkin_t* epoc32Skin;
      NW_Bool selectUpdated;
      
      selectBox = NW_FBox_SelectBoxOf(box);
      selectSkin = NW_FBox_SelectSkinOf(NW_FBox_FormBox_GetSkin(selectBox));
      epoc32Skin = NW_FBox_Epoc32SelectSkinOf (selectSkin);
      status = NW_FBox_Epoc32SelectSkin_UpdateSelection(epoc32Skin, value, &selectUpdated);
      (void) NW_FBox_SelectSkin_RefreshText( selectSkin );
      NW_LMgr_Box_Refresh(NW_LMgr_BoxOf(selectBox));
      NW_THROW_ON_ERROR (status);
      if (!selectUpdated) {
        NW_THROW_STATUS(status, KBrsrFailure);
      }
    }
    
    else if (NW_Object_IsInstanceOf(box, &(NW_FBox_InputBox_Class))) {
      NW_FBox_InputBox_t* inputBox;
      NW_FBox_InputSkin_t* inputSkin;
      
      inputBox = NW_FBox_InputBoxOf(box);
      inputSkin = NW_FBox_InputSkinOf(NW_FBox_FormBox_GetSkin(inputBox));
      
      if (NW_Str_Strlen(value) > (NW_Uint16) NW_FBox_InputBox_GetMaxChars (inputBox)) {
        NW_THROW_STATUS(status, KBrsrFailure);
      }
        

	  if( NW_Str_Strlen( value ) > 0 )
		{
		status=NW_FBox_Epoc32InputSkin_SetValue(inputSkin, 
                                              value, 
                                              NW_Str_Strlen( value ) );

		NW_THROW_ON_ERROR(status);

		/* update value to the form liaison */
		status=NW_FBox_InputBox_SetFormLiaisonVal(inputBox);
		NW_THROW_ON_ERROR(status);

		status=NW_FBox_Epoc32InputSkin_Redraw(inputSkin);
		}


        /* Input or password box */
//        status = NW_FBox_Epoc32InputSkin_UpdateInput(NW_FBox_Epoc32InputSkinOf (inputSkin), 
//          inputBox, value, NW_FALSE);
#pragma message (__FILE__"   TODO: Add method NW_FBox_Epoc32InputSkin_UpdateInput()" )

        NW_THROW_ON_ERROR (status);
    }

    else {
      NW_THROW_STATUS(status, KBrsrFailure);
    }
  } NW_CATCH (status) {
    if (status != KBrsrOutOfMemory) {
      status = KBrsrFailure;
    }
  } NW_FINALLY {
    return status;
  } NW_END_TRY
}  

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_Markup_FormFiller_Class_t NW_Markup_FormFiller_Class = {
    { /* NW_Object_Core          */
        /* super                   */ &NW_Object_Aggregate_Class,
        /* querySecondary          */ _NW_Object_Core_QuerySecondary
    },
    { /* NW_Object_Secondary     */
        /* offset                  */ 0
    },
    { /* NW_Object_Aggregate     */
        /* secondaryList           */ _NW_Markup_FormFiller_SecondaryList,
        /* construct               */ NULL,
        /* destruct                */ NULL
    },
    { /* NW_HED_FormFiller */
        /* getBoxTree              */ NULL
    }
};

/* ------------------------------------------------------------------------- */
const
NW_Object_Class_t* const _NW_Markup_FormFiller_SecondaryList[] = {
    &NW_Markup_FormFiller_IFormFiller_Class,
        NULL
};

/* ------------------------------------------------------------------------- */
const NW_HED_IFormFiller_Class_t NW_Markup_FormFiller_IFormFiller_Class = {
    { /* NW_Object_Core          */
        /* super                   */ &NW_HED_IFormFiller_Class,
        /* querySecondary          */ _NW_Object_Core_QuerySecondary
    },
    { /* NW_Object_Secondary     */
        /* offset                  */ offsetof (NW_Markup_FormFiller_t, NW_HED_IFormFiller)
    },
    { /* NW_HED_IFormFiller      */
        /* ECMLExists              */ _NW_Markup_FormFiller_IFormFiller_ECMLExists,
        /* fillAll                 */ _NW_Markup_FormFiller_IFormFiller_FillAll,
        /* fillOne                 */ _NW_Markup_FormFiller_IFormFiller_FillOne
    }

};


/* ------------------------------------------------------------------------- *
NW_HED_IFormFiller methods
* ------------------------------------------------------------------------- */

/** ----------------------------------------------------------------------- **
    @method:      _NW_Markup_FormFiller_IFormFiller_ECMLExists

    @synopsis:    Determines if there is at least one ECML box in the box tree.

    @scope:       public

    @parameters:
       [in, out]  NW_HED_IFormFiller_t* formFiller
                  Pointer to FormFiller interface object.

    @description: Determines if there is at least one ECML box in the box tree.
                  (See NW_Markup_FormFiller_IsECMLBox for a definition of an ECML box.)

    @returns:     NW_Bool
       [NW_TRUE]
                  There is at least one ECML box in the box tree.

       [NW_FALSE]
                  There are no ECML boxes in the box tree.
 ** ----------------------------------------------------------------------- **/
NW_Bool
_NW_Markup_FormFiller_IFormFiller_ECMLExists (NW_HED_IFormFiller_t* formFiller)
{
    NW_Bool ECMLFound = NW_FALSE;
    NW_Markup_FormFiller_t* thisObj;
    NW_LMgr_BoxVisitor_t* visitor = NULL;
    NW_Ucs2* ECMLFieldName = NULL;
    
    /* parameter assertion block */
    NW_ASSERT (NW_Object_IsInstanceOf (formFiller,
        &NW_Markup_FormFiller_IFormFiller_Class));
    
    /* for convenience */
    thisObj = (NW_Markup_FormFiller_t*) NW_Object_Interface_GetImplementer (formFiller);
    NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_Markup_FormFiller_Class));
    
    /*lint -e{550} Symbol not accessed */
    NW_TRY (status) {
        NW_LMgr_Box_t* boxTree;
        NW_LMgr_Box_t* childBox;
        NW_Bool skipChildren;
        NW_Bool isFormFillerBox;
        
        boxTree = NW_Markup_FormFiller_GetBoxTree (thisObj);
        NW_ASSERT (NW_Object_IsInstanceOf (boxTree, &NW_LMgr_Box_Class));
        visitor = NW_LMgr_ContainerBox_GetBoxVisitor (NW_LMgr_ContainerBoxOf (boxTree));
        NW_THROW_OOM_ON_NULL (visitor, status);
        
        /* Iterate through all children */
        skipChildren = NW_FALSE;
        while ((childBox = NW_LMgr_BoxVisitor_NextBox (visitor, &skipChildren)) != NULL) {      
            ECMLFieldName = NW_Markup_FormFiller_IsECMLBox (childBox, &isFormFillerBox);

            if(ECMLFieldName == NULL) {
                continue;
            }

            ECMLFound = NW_TRUE;
            break;
        }
    } NW_CATCH (status) {
    }  NW_FINALLY {
      NW_Str_Delete(ECMLFieldName);
      NW_Object_Delete (visitor);
      return ECMLFound;
    } NW_END_TRY
}

/** ----------------------------------------------------------------------- **
    @method:      _NW_Markup_FormFiller_IFormFiller_FillAll

    @synopsis:    Attempts to fill all valid FormFiller boxes in the box tree
                  with data from the wallet.

    @scope:       public

    @parameters:
       [in, out]  NW_HED_IFormFiller_t* formFiller
                  Pointer to FormFiller interface object.

       [out]      NW_LMgr_Box_t** firstNotFilled
                  Pointer to store an NW_LMgr_Box_t pointer. 
                  This is the pointer to the first empty FormFiller box which could
                  not be filled, or NULL if all FormFiller boxes were successfully filled.

    @description: Attempts to fill all valid FormFiller boxes in the box tree
                  with data from the wallet.(See NW_Markup_FormFiller_IsFormFillerBox 
                  for a definition of a valid FormFiller box.)

    @returns:     TBrowserStatusCode
       [KBrsrSuccess]
                  When all FormFiller boxes were successfully filled.

       [KBrsrFailure]
                  When there is at least one empty FormFiller box which 
                  could not be filled,

       [KBrsrOutOfMemory]
                  When an allocation failure is encountered.

       [KBrsrUnexpectedError]
                  When an unexpected error is encountered.

       [KBrsrBadInputParam]
                  When bad input parameter is encountered.
 ** ----------------------------------------------------------------------- **/
TBrowserStatusCode 
_NW_Markup_FormFiller_IFormFiller_FillAll (NW_HED_IFormFiller_t* formFiller,
                                           NW_LMgr_Box_t** firstNotFilled)
{
  NW_Markup_FormFiller_t* thisObj;
  NW_LMgr_BoxVisitor_t* visitor = NULL;
  NW_Ucs2* value;
  WALLET_XHTML_Status_t walletStatus;

  /* parameter assertion block */
  NW_ASSERT(firstNotFilled != NULL);
  if (firstNotFilled == NULL) {
    return KBrsrBadInputParam;
  }
  NW_ASSERT (NW_Object_IsInstanceOf (formFiller,
    &NW_Markup_FormFiller_IFormFiller_Class));
  
  /* for convenience */
  thisObj = (NW_Markup_FormFiller_t*) NW_Object_Interface_GetImplementer (formFiller);
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_Markup_FormFiller_Class));
  
  NW_TRY (status) {
    NW_LMgr_Box_t* boxTree;
    NW_LMgr_Box_t* childBox;
    NW_Bool skipChildren;
    NW_Bool noneFilled = NW_TRUE;
    NW_Bool isFormFillerBox;
    NW_Bool existingVal;
    NW_Ucs2* ECMLFieldName;
    NW_LMgr_RootBox_t* rootBox = NULL;
   
    *firstNotFilled = NULL;
    boxTree = NW_Markup_FormFiller_GetBoxTree (thisObj);
    NW_ASSERT (NW_Object_IsInstanceOf (boxTree, &NW_LMgr_Box_Class));
    visitor = NW_LMgr_ContainerBox_GetBoxVisitor (NW_LMgr_ContainerBoxOf (boxTree));
    NW_THROW_OOM_ON_NULL (visitor, status);
    rootBox = NW_LMgr_Box_GetRootBox(boxTree);

    /* Iterate through all children */
    skipChildren = NW_FALSE;
    while ((childBox = NW_LMgr_BoxVisitor_NextBox (visitor, &skipChildren)) != NULL) {
      
      ECMLFieldName = NW_Markup_FormFiller_IsECMLBox (childBox, &isFormFillerBox);
      if (!isFormFillerBox) {
        continue;
      }

      /* If the box is a valid input box but not an ECML box, this may be counted
       * as an unfilled field. If there's already a value in the field, it's considered OK.
       * Note: WalletFetch_GetData() will deallocate ECMLFieldName.
       */
      if (ECMLFieldName != NULL) {
        walletStatus = rootBox->appServices->walletAPI.WalletFetch_GetData(ECMLFieldName, &value, noneFilled);
        status = NW_Markup_FormFiller_MapFromWalletStatus(walletStatus);

        /* If the status is KBrsrFailure continue and try to fill the other fields.
         * Exit on more serious errors.
         */
        if (status != KBrsrSuccess && status != KBrsrFailure) {
          NW_THROW (status);
        }


          if (status != KBrsrSuccess) {
            noneFilled = NW_FALSE;
          }
        if (status == KBrsrSuccess && value != NULL) {
        /* Update input box with wallet value.
         * Could still fail to fill in if format is bad,
         * or wallet value is invalid selection field option.
         */
          status = NW_Markup_FormFiller_UpdateBox(childBox, value);
          NW_Str_Delete(value);      

          if (status == KBrsrSuccess) {
//            noneFilled = NW_FALSE;
            continue;
          }
          if (status == KBrsrOutOfMemory) {
            NW_THROW (status);
          }
        }
      }

      /* If field has an existing value, don't count this as unfilled. */
      status = NW_Markup_FormFiller_ExistingValue(childBox, &existingVal);
      if (status == KBrsrOutOfMemory) {
        NW_THROW (status);
      }
      if (!existingVal) {
        if (!(*firstNotFilled)) {
          *firstNotFilled = childBox;
        }
      }
    }

    walletStatus = rootBox->appServices->walletAPI.WalletFetch_GetData((NW_Ucs2*)NULL, &value, noneFilled);
    status = NW_Markup_FormFiller_MapFromWalletStatus(walletStatus);

    /* Last call to WalletFetch_GetData() doesn't do anything.
     * If it fails, somethings wrong!
     */
    if (status != KBrsrSuccess) {
      NW_THROW (status);
    }

  } NW_CATCH (status) {
    if (status != KBrsrOutOfMemory &&
        status != KBrsrUnexpectedError && 
        status != KBrsrBadInputParam) {
      status = KBrsrFailure;
    }
  } 
  NW_FINALLY {
    NW_Object_Delete (visitor);
    if (status == KBrsrSuccess && *firstNotFilled != NULL) {
      status = KBrsrFailure;
    }
    return status;
  } NW_END_TRY
}

/** ----------------------------------------------------------------------- **
    @method:      _NW_Markup_FormFiller_IFormFiller_FillOne

    @synopsis:    Attempts to fill the given box with the requested data.

    @scope:       public

    @parameters:
       [in, out]  NW_HED_IFormFiller_t* formFiller
                  Pointer to FormFiller interface object.

       [in, out]  NW_Ucs2* value
                  Value to be used to update the FormFiller box.

       [in, out]  NW_LMgr_Box_t* node
                  Pointer to the FormFiller box which should be updated with the value.

    @description: Attempts to fill the given box with the requested data.
                  (See NW_Markup_FormFiller_IsFormFillerBox 
                  for a definition of a valid FormFiller box.)

    @returns:     TBrowserStatusCode
       [KBrsrSuccess]
                  When the FormFiller box was successfully filled.

       [KBrsrFailure]
                  When the FormFiller box could not be successfully filled.

       [KBrsrOutOfMemory]
                  When an allocation failure is encountered.

 ** ----------------------------------------------------------------------- **/

TBrowserStatusCode
_NW_Markup_FormFiller_IFormFiller_FillOne (NW_HED_IFormFiller_t* formFiller,
                                           NW_Ucs2* value,
                                           NW_LMgr_Box_t* node)
{
    NW_Markup_FormFiller_t* thisObj;
    NW_LMgr_BoxVisitor_t* visitor = NULL;

    /* parameter assertion block */
    NW_ASSERT (NW_Object_IsInstanceOf (formFiller,
        &NW_Markup_FormFiller_IFormFiller_Class));
    
    /* for convenience */
    thisObj = (NW_Markup_FormFiller_t*) NW_Object_Interface_GetImplementer (formFiller);
    NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_Markup_FormFiller_Class));
    
    NW_TRY (status) {
        NW_LMgr_Box_t* boxTree;
        NW_LMgr_Box_t* childBox;
        NW_Bool skipChildren;
        
        boxTree = NW_Markup_FormFiller_GetBoxTree (thisObj);
        NW_ASSERT (NW_Object_IsInstanceOf (boxTree, &NW_LMgr_Box_Class));
        visitor = NW_LMgr_ContainerBox_GetBoxVisitor (NW_LMgr_ContainerBoxOf (boxTree));
        NW_THROW_OOM_ON_NULL (visitor, status);
        
        /* Iterate through all children */
        skipChildren = NW_FALSE;
        while ((childBox = NW_LMgr_BoxVisitor_NextBox (visitor, &skipChildren)) != NULL) {
          
          /* Search for the in-focus box */
          if (childBox != node) {
            continue;
          }
          
          /* Verify that it's a valid formfiller box */
          if(!NW_Markup_FormFiller_IsFormFillerBox (childBox)) {
            NW_THROW_STATUS(status, KBrsrFailure);
          }
          
          /* Update input box with wallet value */      
          status = NW_Markup_FormFiller_UpdateBox(childBox, value);
          if (status == KBrsrSuccess) {
            break;
          }
          NW_THROW(status);
        }
    } NW_CATCH (status) { /* empty */
    }  NW_FINALLY {
      NW_Object_Delete (visitor);
      NW_Str_Delete(value);
      return status;
    } NW_END_TRY
}

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */

/** ----------------------------------------------------------------------- **
    @method:      NW_Markup_FormFiller_MapFromWalletStatus

    @synopsis:    Maps from wallet status to browser status.

    @scope:       public

    @parameters:
       [in, out]  WALLET_XHTML_Status_t walletStatus
                  Wallet status to be mapped.

    @description: Maps from wallet status to browser status.

    @returns:     TBrowserStatusCode
                  Returns the corresponding browser status.

 ** ----------------------------------------------------------------------- **/
TBrowserStatusCode 
NW_Markup_FormFiller_MapFromWalletStatus(WALLET_XHTML_Status_t walletStatus)
{
  switch (walletStatus) { 
  case WALLET_XHTML_SUCCESS: return KBrsrSuccess;
  case WALLET_XHTML_OUT_OF_MEMORY: return KBrsrOutOfMemory;
  case WALLET_XHTML_UNEXPECTED_ERROR: return KBrsrUnexpectedError;
  case WALLET_XHTML_BAD_INPUT_PARAM: return KBrsrBadInputParam;
  default: return KBrsrFailure;
  }
}

/** ----------------------------------------------------------------------- **
    @method:      NW_Markup_FormFiller_MapToWalletStatus

    @synopsis:    Maps from browser status to wallet status.

    @scope:       public

    @parameters:
       [in, out]  TBrowserStatusCode status
                  Browser status to be mapped.

    @description: Maps from browser status to wallet status.

    @returns:     WALLET_XHTML_Status_t
                  Returns the corresponding wallet status.

 ** ----------------------------------------------------------------------- **/
WALLET_XHTML_Status_t
NW_Markup_FormFiller_MapToWalletStatus(TBrowserStatusCode status)
{
  switch (status) { 
  case KBrsrSuccess: return WALLET_XHTML_SUCCESS;
  case KBrsrOutOfMemory: return WALLET_XHTML_OUT_OF_MEMORY;
  case KBrsrUnexpectedError: return WALLET_XHTML_UNEXPECTED_ERROR;
  case KBrsrBadInputParam: return WALLET_XHTML_BAD_INPUT_PARAM;
  default: return WALLET_XHTML_FAILURE;
  }
}

/** ----------------------------------------------------------------------- **
    @method:      NW_Markup_FormFiller_GetInFocusBox

    @synopsis:    Locates the in-focus FormFiller box if there is one,
                  and passes back it's title attribute.

    @scope:       public

    @parameters:
       [in, out]  NW_HED_IFormFiller_t* formFiller
                  Pointer to a FormFiller object.

       [in]       const NW_MVC_View_t* view
                  Pointer to the current view.

       [out]      NW_Ucs2** titleOut
                  Pointer to store the title string pointer.
          
    @description: If there is currently a box in focus, and it is a valid
                  FormFiller box, the routine returns a pointer to that box.
                  Otherwise, it returns the NULL pointer.

                  Also passes the title attribute of the in-focus FormFiller box
                  back in the titleOut parameter. If there's no in-focus FormFiller
                  box, or there is such a box but it has no title, passes back NULL.

    @returns:     NW_LMgr_Box_t*
                  A pointer to the in-focus FormFiller box if there is one,
                  NULL otherwise.

 ** ----------------------------------------------------------------------- **/

NW_LMgr_Box_t*
NW_Markup_FormFiller_GetInFocusBox(NW_HED_IFormFiller_t* formFiller,
                                   const MBoxTreeListener* boxTreeListener, 
                                   NW_Ucs2** titleOut)
{
  NW_LMgr_Box_t* box;
  NW_FBox_FormLiaison_t  *formLiaison = NULL;
  void* controlId;
  TBrowserStatusCode status;
  NW_Ucs2* title;

  NW_REQUIRED_PARAM(formFiller);

  NW_ASSERT(titleOut != NULL);

  *titleOut = NULL;
  box = boxTreeListener->GetInFocusBox ();
  if (box == NULL) {
    return box;
  }

  if (!NW_Markup_FormFiller_IsFormFillerBox(box)) {
    return (NW_LMgr_Box_t*) NULL;
  }

  formLiaison = NW_FBox_FormBox_GetFormLiaison(box);
  controlId = NW_FBox_FormBox_GetFormCntrlID(box);
  status = NW_FBox_FormLiaison_GetStringTitle(formLiaison, controlId, &title);
  if (status != KBrsrSuccess) {
    return box;
  }

  *titleOut = title;
  return box;
}
