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


#ifndef NW_LMGR_ACTIVECONTAINERBOX_H
#define NW_LMGR_ACTIVECONTAINERBOX_H


#include <nw_dom_node.h>
#include "nw_lmgr_containerbox.h"
#include "NW_LMgr_IEventListener.h"
#include "nw_lmgr_eventhandler.h"
#include "nw_sound.h"
#include "nw_sound_contenthandler.h"

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   public types
 * ------------------------------------------------------------------------- */
typedef enum NW_LMgr_ActiveContainerBox_ActionType_e
{
  NW_LMgr_ActionType_OpenLink = 0,
  NW_LMgr_ActionType_DialNumber,
  NW_LMgr_ActionType_SendMail,
  NW_LMgr_ActionType_OpenObject,
  NW_LMgr_ActionType_ActivateObject
}NW_LMgr_ActiveContainerBox_ActionType_t;


/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_LMgr_ActiveContainerBox_Class_s NW_LMgr_ActiveContainerBox_Class_t;
typedef struct NW_LMgr_ActiveContainerBox_s NW_LMgr_ActiveContainerBox_t;

/* ------------------------------------------------------------------------- *
   class declaration
 * ------------------------------------------------------------------------- */
typedef struct NW_LMgr_ActiveContainerBox_ClassPart_s {
  void** unused;
} NW_LMgr_ActiveContainerBox_ClassPart_t;
  
struct NW_LMgr_ActiveContainerBox_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_Box_ClassPart_t NW_LMgr_Box;
  NW_LMgr_ContainerBox_ClassPart_t NW_LMgr_ContainerBox;
  NW_LMgr_ActiveContainerBox_ClassPart_t NW_LMgr_ActiveContainerBox;
};

/* ------------------------------------------------------------------------- *
   object declaration
 * ------------------------------------------------------------------------- */
struct NW_LMgr_ActiveContainerBox_s {
  NW_LMgr_ContainerBox_t super;

  /* implemented interfaces */
  NW_LMgr_IEventListener_t NW_LMgr_IEventListener;

  /* member variables */
  NW_LMgr_EventHandler_t* eventHandler;
  NW_Text_t* iName;
  NW_Text_t* hrefAttr;
  NW_LMgr_ActiveContainerBox_ActionType_t actionType;
};
    
/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_LMgr_ActiveContainerBox_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_LMgr_ActiveContainerBox))

#define NW_LMgr_ActiveContainerBoxOf(object) \
  (NW_Object_Cast (object, NW_LMgr_ActiveContainerBox))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT const NW_LMgr_ActiveContainerBox_Class_t NW_LMgr_ActiveContainerBox_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_LMgr_ActiveContainerBox_GetActionType(_object) \
  (NW_OBJECT_CONSTCAST(NW_LMgr_ActiveContainerBox_ActionType_t) \
     NW_LMgr_ActiveContainerBoxOf(_object)->actionType)

#define NW_LMgr_ActiveContainerBox_SetActionType(_object, _actionType) \
  ((void) (NW_LMgr_ActiveContainerBoxOf(_object)->actionType = _actionType))


/* ------------------------------------------------------------------------- *
   convenience methods
 * ------------------------------------------------------------------------- */

/**
* Creates a new file selection box.
* @param aNumProperties ?description (not used)
* @param aEventHandler Event handler for this box.
* @param aActionType type of action to be taken when link is activated.
* @return Pointer to new active container box structure or null if failure. 
*   Caller assumes ownership of memory and must deallocate but not until
*   after calling the OOC destructor.
*/
NW_LMGR_EXPORT
NW_LMgr_ActiveContainerBox_t*
NW_LMgr_ActiveContainerBox_New (NW_ADT_Vector_Metric_t numProperties,
                                NW_LMgr_EventHandler_t* eventHandler,
                                NW_LMgr_ActiveContainerBox_ActionType_t aActionType);

/**
* Sets the name of this active container box. The name is used by ECMA script.
* @since 2.8
* @param aThisObj Ptr to the OOC class structure for this class.
* @param aName value of name attribute for this element or NULL if none.
* @param aTakeOwnershipOfNameObject flag indicating whether ownership of the
*   name object passed is transferred to this object.
* @return none
*/
void
NW_LMgr_ActiveContainerBox_SetName( NW_LMgr_ActiveContainerBox_t* aThisObj,
    NW_Text_t* aName, NW_Bool aTakeOwnershipOfNameObject );

/**
* Sets the soundstart state of this active container box. Soundstart is a
* proprietary customer feature of anchor tags. The soundstart state is stored
* as CSS properties of this box. Note, the soundstart src URL is NOT set.
* @since 2.8
* @param aThisObj Ptr to the OOC class structure for this class.
* @param aElementNode address of the DOM node for this element.
* @param aSoundstartState contains soundstart state values to set as CSS
*   properties.
* @return Browser status code:
*   KBrsrSuccess for success,
*   KBrsrOutOfMemory if out of memory,
*   otherwise, one of the other KBrsr... error codes.
*/
TBrowserStatusCode
NW_LMgr_ActiveContainerBox_SetSoundstartState( NW_LMgr_ActiveContainerBox_t* aThisObj,
    NW_DOM_ElementNode_t* aElementNode, TSoundstartState* aSoundstartState );

/**
* Gets the soundstart state from this active container box, if present.
* Soundstart is a proprietary customer feature of anchor tags. The soundstart
* state is stored as CSS properties of this box. Note, the soundstart src URL
* is NOT stored and is returned as NULL in the soundstartState structure.
* @since 2.8
* @param aThisObj Ptr to the OOC class structure for this class.
* @param aSoundCH Ptr to destination into which the ptr to the sound content
*   handler is returned.
* @param aSoundstartState returned soundstart state values obtained from CSS
*   properties of this box.
* @return Browser status code:
*   KBrsrSuccess for success,
*   KBrsrOutOfMemory if out of memory,
*   KBrsrNotFound if no soundstart for this box,
*   otherwise, one of the other KBrsr... error codes.
*/
TBrowserStatusCode
NW_LMgr_ActiveContainerBox_GetSoundstartState( NW_LMgr_ActiveContainerBox_t* aThisObj,
    NW_Sound_ContentHandler_t** aSoundCH, TSoundstartState* aSoundstartState );

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_LMGR_ACTIVECONTAINERBOX_H */
