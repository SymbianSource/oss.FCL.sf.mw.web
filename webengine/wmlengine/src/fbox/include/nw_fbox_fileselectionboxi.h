/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Defines virtual method implementation prototypes.
*
*/


#ifndef NW_FBOX_FILESELECTIONBOXI_H
#define NW_FBOX_FILESELECTIONBOXI_H

//  INCLUDES
#include "nw_fbox_formboxi.h"
#include "nw_fbox_fileselectionbox.h"
#include "BrsrStatusCodes.h"


/* ------------------------------------------------------------------------- *
   virtual method implementation prototypes
 * ------------------------------------------------------------------------- */

/**
* OOC constructor.
*/
NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_FileSelectionBox_Construct(
    NW_Object_Dynamic_t* aDynamicObject,
    va_list* aArgp );

/**
* Destructor.
*/
NW_FBOX_EXPORT
void
_NW_FBox_FileSelectionBox_Destruct( NW_Object_Dynamic_t* aDynamicObject );

/**
* Gets the baseline for the content of this box.
* @since 2.6
* @param aLmgrBox Ptr to the parent LMgr OOC box class for this box object.
* @return Value of baseline.
*/
NW_FBOX_EXPORT
NW_GDI_Metric_t
_NW_FBox_FileSelectionBox_GetBaseline( NW_LMgr_Box_t* aLmgrBox );

/**
* Creates and initializes the interactor for this box.
* @since 2.6
* @param aFormBox Ptr to the parent Form OOC box class for this box object.
* @return Pointer to the newly created interactor object.
*/
NW_FBOX_EXPORT
NW_FBox_Interactor_t*
_NW_FBox_FileSelectionBox_InitInteractor( NW_FBox_FormBox_t* aFormBox );

/**
* Creates and initializes the skin class for this box.
* @since 2.6
* @param aFormBox Ptr to the parent Form OOC box class for this box object.
* @return Pointer to the newly created skin object.
*/
NW_FBOX_EXPORT
NW_FBox_Skin_t*
_NW_FBox_FileSelectionBox_InitSkin( NW_FBox_FormBox_t* aFormBox );

/**
* Resets this box; i.e., resets the value of the filename to an empty string.
* @since 2.6
* @param aFormBox Ptr to the parent Form OOC box class for this box object.
* @return Browser status code, indicating status of operation.
*/
NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_FileSelectionBox_Reset( NW_FBox_FormBox_t* aFormBox );

/**
* Checks to see if this box needs to be split and if so, "splits" it; however, 
* this box cannot be split. It is moved to a new line if not already on one.
* That is the best that can be done to split this box.
* @since 2.6
* @param aLmgrBox Ptr to the parent LMgr OOC box class for this box object.
* @param aSpace Available space -- the width in pixels.
* @param aSplitBox Ptr to returned split box, which is always set to NULL since
*       this box cannot be split..
* @param aFlags Split flags (such as whether or not already at a new line),
*       possible values are defined by the enum NW_LMgr_Box_SplitFlags_e.
* @return Browser status code, indicating status of operation.
*/
NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_FileSelectionBox_Split(
    NW_LMgr_Box_t* aLmgrBox,
    NW_GDI_Metric_t aSpace,
    NW_LMgr_Box_t** aSplitBox,
    NW_Uint8 aFlags );

/**
* Process events for this box: set-active, clear-field, gain/lose focus, etc.
* @since 2.6
* @param aEventListener Layout manager event listener, which will be called
*       to handle the event as a default if this method doesn't handle the
*       event directly.
* @param aEvent Event to process.
* @return Whether or not the event was handled.  Possible values:
* NW_LMgr_EventAbsorbed or NW_LMgr_EventNotAbsorbed.
*/
NW_FBOX_EXPORT
NW_Uint8
_NW_FBox_FileSelectionBox_IEventListener_ProcessEvent(
    NW_LMgr_IEventListener_t* aEventListener,
    NW_Evt_Event_t* aEvent );

#endif
