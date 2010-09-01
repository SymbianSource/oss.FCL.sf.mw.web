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


#ifndef NW_FBOX_FILESELECTIONBOXSKINI_H
#define NW_FBOX_FILESELECTIONBOXSKINI_H

#include "BrsrStatusCodes.h"
#include "nw_fbox_fileselectionboxskin.h"


/* ------------------------------------------------------------------------- *
   virtual method implementation prototypes
 * ------------------------------------------------------------------------- */

// Constructor for this OOC object.
NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_FileSelectionBoxSkin_Construct(
    NW_Object_Dynamic_t* aDynamicObject,
    va_list* aArgp );

// Destructor.
NW_FBOX_EXPORT
void
_NW_FBox_FileSelectionBoxSkin_Destruct( NW_Object_Dynamic_t* aDynamicObject );

/**
* Draws the skin -- the box and its contents.
* @since 2.6
* @param aSkin Ptr to this OOC object.
* @param aLmgrBox Ptr to the box to which this skin belongs.
* @param aDeviceContext GDI device context.
* @param aHasFocus Flag indicating whether or not box has focus.
* @return Browser status code, indicating status of operation.
*/
NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_FileSelectionBoxSkin_Draw(
    NW_FBox_Skin_t* aSkin,
    NW_LMgr_Box_t* aLmgrBox,
    CGDIDeviceContext* aDeviceContext,
    NW_Uint8 aHasFocus );

/**
* Gets the baseline for the content of this box.
* @since 2.6
* @param aThisObj Ptr to the OOC class structure for this class.
* @param aLmgrBox Ptr to the box to which this skin belongs.
* @return Baseline value.
*/
NW_FBOX_EXPORT
NW_GDI_Metric_t
_NW_FBox_FileSelectionBoxSkin_GetBaseline(
    NW_FBox_FileSelectionBoxSkin_t* aThisObj,
    NW_LMgr_Box_t* aLmgrBox );

/**
* Gets the size (width & height) needed for the content.  The returned width
* is bounded by the display width.
* @since 2.6
* @param aSkin Ptr to the parent OOC skin class for this skin object.
* @param aLmgrBox Ptr to the box to which this skin belongs.
* @param aSize Ptr at which to the size is returned.
* @return Browser status code, indicating status of operation.
*/
NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_FileSelectionBoxSkin_GetSize(
    NW_FBox_Skin_t* aSkin,
    NW_LMgr_Box_t* aLmgrBox,
    NW_GDI_Dimension3D_t* aSize );

/**
* Resets this box; i.e., resets the value of the filename to an empty string.
* @since 2.6
* @param aThisObj Ptr to the OOC class structure for this class.
* @return Browser status code, indicating status of operation.
*/
NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_FileSelectionBoxSkin_Reset( NW_FBox_FileSelectionBoxSkin_t* aThisObj );

/**
* Sets this box to be the active box, which launches the file-selection dialogs.
* @since 2.6
* @param aThisObj Ptr to the OOC class structure for this class.
* @return Browser status code, indicating status of operation.
*/
NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_FileSelectionBoxSkin_SetActive( NW_FBox_FileSelectionBoxSkin_t* aThisObj );

/**
* Checks to see if this box needs to be split and if so, "splits" it; however, 
* this box cannot be split. It is moved to a new line if not already on one.
* That is the best that can be done to split this box.
* @since 2.6
* @param aThisObj Ptr to the OOC class structure for this class.
* @param aSpace Available space -- the width in pixels.
* @param aSplitBox Ptr to returned split box, which is always set to NULL since
*       this box cannot be split..
* @param aFlags Split flags (such as whether or not already at a new line),
*       possible values are defined by the enum NW_LMgr_Box_SplitFlags_e.
* @return Browser status code, indicating status of operation.
*/
TBrowserStatusCode
_NW_FBox_FileSelectionBoxSkin_Split(
    NW_FBox_FileSelectionBoxSkin_t* aThisObj,
    NW_GDI_Metric_t aSpace,
    NW_LMgr_Box_t** aSplitBox,
    NW_Uint8 aFlags );


/* ------------------------------------------------------------------------- *
  protected methods
* ------------------------------------------------------------------------- */

#define NW_FBox_FileSelectionBoxSkin_GetFileSelectionBox(_skin) \
    (NW_FBox_FileSelectionBoxOf(NW_FBox_Skin_GetFormBox(_skin)))


#endif
