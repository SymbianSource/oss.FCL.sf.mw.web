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


#include "nw_fbox_passwordboxi.h"
#include "nw_fbox_inputskin.h"
#include "nw_fbox_inputinteractor.h"
#include "nw_evt_timerevent.h"
#include "nw_fbox_inlineinputskin.h"
#include "nw_lmgr_rootbox.h"
#include "nwx_string.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   forward method declarations
 * ------------------------------------------------------------------------- */

#if 0
/*
static
TBrowserStatusCode
NW_FBox_PasswordBox_TimerCallback (void *timerData);
*/

/* ------------------------------------------------------------------------- *
    private methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- 
static
TBrowserStatusCode
NW_FBox_PasswordBox_CreateTimer (NW_FBox_PasswordBox_t* thisObj) 
{
  NW_TRY (status) {
    if (thisObj->timer == NULL) {
      thisObj->timer = NW_System_Timer_New (NW_FBox_PasswordBox_TimerCallback,
                                            thisObj, 500, NW_TRUE);
      NW_THROW_ON_NULL (thisObj->timer, status, KBrsrUnexpectedError);
    }
  }

  NW_CATCH (status) {  }

  NW_FINALLY {
    return status;
  } NW_END_TRY
}
*/
/* ------------------------------------------------------------------------- 
static
void
NW_FBox_PasswordBox_DestroyTimer (NW_FBox_PasswordBox_t* thisObj)
{
  NW_Object_Delete (thisObj->timer);
  thisObj->timer = NULL;
}*/
#endif /* UNUSED_CODE */

/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_FBox_PasswordBox_SetDisplayTextToObscure (NW_FBox_PasswordBox_t* box)
{
  NW_FBox_InputSkin_t* skin;
  NW_Ucs2* displayStorage;
  NW_Text_Length_t displayLength = 0;
  NW_Text_Length_t index;
  NW_Text_UCS2_t* text;
  NW_FBox_InputBox_t* inputBox;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (box, &NW_FBox_PasswordBox_Class));

  /* get the associated skin */
  skin = NW_FBox_InputSkinOf (NW_FBox_FormBox_GetSkin (box));

  if (skin->displayText){
    NW_Object_Delete (skin->displayText);
    skin->displayText = NULL;
  }

  inputBox = NW_FBox_InputBoxOf(box);

  /* retrieve the storage associated with the current display text */
  if (inputBox->value == NULL) {
    return KBrsrSuccess;
  }
  displayLength = NW_Text_GetCharCount (inputBox->value) ;
  if (displayLength == 0) {
    return KBrsrSuccess;
  }
  displayStorage = NW_Str_New(displayLength);
  NW_Mem_memset(displayStorage, 0, (NW_Uint32)((displayLength+1)*sizeof(NW_Ucs2)));
 
  /* obscure the string */
  for (index = 0; index < displayLength; index++) {
    *(displayStorage + index) = '*';
  }

  /* swap the display text object */
  text = NW_Text_UCS2_New (displayStorage, displayLength,
                           NW_Text_Flags_TakeOwnership);
  if (text == NULL) {
    return KBrsrOutOfMemory;
  }
  if (skin->displayText){
    NW_Object_Delete (skin->displayText);
    skin->displayText = NULL;
  }
  skin->displayText = NW_TextOf (text);

  /* successful completion */
  return KBrsrSuccess;
}

#if 0
/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_FBox_PasswordBox_TimerCallback (void *timerData)
{
  NW_FBox_PasswordBox_t *thisObj;
  NW_LMgr_RootBox_t *rootBox;
  TBrowserStatusCode status = KBrsrSuccess;

  /* cast timerData to a box */
  thisObj = NW_FBox_PasswordBoxOf (timerData);
  NW_ASSERT (thisObj);

  /* make sure it is an PasswordBox */
  NW_ASSERT (NW_Object_IsClass (thisObj, &NW_FBox_PasswordBox_Class));

  /* if the box tree is not visible, we do not draw */
  rootBox = NW_LMgr_Box_GetRootBox (thisObj);
  if (!NW_LMgr_RootBox_HasFocus(rootBox)) {
    return KBrsrSuccess;
  }

  /* Stop the timer */
  NW_FBox_PasswordBox_DestroyTimer (thisObj);

  /* make the DisplayText obscured */
  /* NW_TRUE means the last Char of displayText need to be illegible */
  NW_FBox_PasswordBox_SetDisplayTextToObscure (thisObj, NW_TRUE);

  /* Refresh the box */
  NW_LMgr_Box_Refresh (NW_LMgr_BoxOf (thisObj));

  return status;
  }
#endif /* UNUSED_CODE */

/* ------------------------------------------------------------------------- */
const
NW_FBox_PasswordBox_Class_t  NW_FBox_PasswordBox_Class = {
  { /* NW_Object_Core             */
    /* super                      */ &NW_FBox_InputBox_Class,
    /* queryInterface             */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base             */
    /* interfaceList              */ NULL
  },
  { /* NW_Object_Dynamic          */
    /* instanceSize               */ sizeof (NW_FBox_PasswordBox_t),
    /* construct                  */ _NW_FBox_InputBox_Construct,
    /* destruct                   */ NULL
  },
  { /* NW_LMgr_Box                */
    /* split                      */ _NW_FBox_InputBox_Split,
    /* resize                     */ _NW_FBox_FormBox_Resize,
    /* postResize                */ _NW_LMgr_Box_PostResize,
    /* getMinimumContentSize      */ _NW_FBox_FormBox_GetMinimumContentSize,
    /* hasFixedContentSize        */ _NW_LMgr_Box_HasFixedContentSize,
    /* constrain                  */ _NW_LMgr_Box_Constrain,
    /* draw                       */ _NW_FBox_FormBox_Draw,
    /* render                     */ _NW_LMgr_Box_Render,
    /* getBaseline                */ _NW_FBox_InputBox_GetBaseline,
    /* shift                      */ _NW_LMgr_Box_Shift,
    /* clone                      */ _NW_LMgr_Box_Clone
  },
  { /* NW_LMgr_ActiveBox          */
    /* unused                     */ NW_Object_Unused
  },
  { /* NW_FBox_FormBox            */
    /* initSkin                   */ _NW_FBox_InputBox_InitSkin,
    /* initInteractor             */ _NW_FBox_InputBox_InitInteractor,
    /* reset                      */ _NW_FBox_InputBox_Reset
  },
  { /* NW_FBox_InputBox           */
    /* getHeight                  */ _NW_FBox_InputBox_GetHeight,
    /* setHeight                  */ _NW_FBox_InputBox_SetHeight,
    /* setDisplayText             */ _NW_FBox_InputBox_SetDisplayText,
    /* modifyBuffer               */ _NW_FBox_PasswordBox_ModifyBuffer
  },
  { /* NW_FBox_PasswordBox        */
    /* unused                     */ 0
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

#if 0
void
_NW_FBox_PasswordBox_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_FBox_PasswordBox_t* thisObj = NW_FBox_PasswordBoxOf(dynamicObject);

  /* destroy the timer 
  if (thisObj->timer != NULL) { 
    NW_FBox_PasswordBox_DestroyTimer(thisObj);
  }*/
  
}
#endif

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_FBox_PasswordBox_SetObscureDisplayText (NW_FBox_InputBox_t* thisObj,
                                NW_Bool bufferModificationOn)
{
  NW_REQUIRED_PARAM(bufferModificationOn);
  return NW_FBox_PasswordBox_SetDisplayTextToObscure (NW_FBox_PasswordBoxOf (thisObj));
}

/* --------------------------------------------------------------------------*/
TBrowserStatusCode
_NW_FBox_PasswordBox_ModifyBuffer(NW_FBox_InputBox_t* thisObj, 
                                  NW_Ucs2 c, 
                                  NW_Uint32 repeated)
{
  TBrowserStatusCode status;

  /* call the superclass method */
  status = NW_FBox_InputBox_Class.NW_FBox_InputBox.modifyBuffer(thisObj, c, repeated);

  /* start the timer 
  status = NW_FBox_PasswordBox_CreateTimer ((NW_FBox_PasswordBox_t*)thisObj);
  if (status != KBrsrSuccess) {
    return status;
  }*/

  return status;
}

/* ------------------------------------------------------------------------- *
   convenience methods
 * ------------------------------------------------------------------------- */
NW_FBox_PasswordBox_t*
NW_FBox_PasswordBox_New (NW_ADT_Vector_Metric_t numProperties,
                         NW_LMgr_EventHandler_t* eventHandler,
                         void* formCntrlId,
                         NW_FBox_FormLiaison_t* formLiaison,
                         NW_Uint16 columns,
                         NW_HED_AppServices_t* appServices)
{
  return (NW_FBox_PasswordBox_t*)
    NW_Object_New (&NW_FBox_PasswordBox_Class, numProperties, eventHandler, 
                  formCntrlId, formLiaison, columns, appServices);
}
