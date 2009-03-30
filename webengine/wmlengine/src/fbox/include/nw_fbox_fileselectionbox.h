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
* Description:  Defines OOC class structures and public method prototypes.
*
*/


#ifndef NW_FBOX_FILESELECTIONBOX_H
#define NW_FBOX_FILESELECTIONBOX_H

//  INCLUDES
#include "nw_fbox_formbox.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
typedef struct NW_FBox_FileSelectionBox_Class_s NW_FBox_FileSelectionBox_Class_t;
typedef struct NW_FBox_FileSelectionBox_s NW_FBox_FileSelectionBox_t;

/* ------------------------------------------------------------------------- *
   virtual method prototypes
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
   OOC class declaration
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_FileSelectionBox_ClassPart_s
    {
    void** unused;
    } NW_FBox_FileSelectionBox_ClassPart_t;

struct NW_FBox_FileSelectionBox_Class_s
    {
    NW_Object_Core_ClassPart_t NW_Object_Core;
    NW_Object_Base_ClassPart_t NW_Object_Base;
    NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
    NW_LMgr_Box_ClassPart_t NW_LMgr_Box;
    NW_LMgr_ActiveBox_ClassPart_t NW_LMgr_ActiveBox;
    NW_FBox_FormBox_ClassPart_t NW_FBox_FormBox;
    NW_FBox_FileSelectionBox_ClassPart_t NW_FBox_FileSelectionBox;
    };

/* ------------------------------------------------------------------------- *
   object declaration
 * ------------------------------------------------------------------------- */
struct NW_FBox_FileSelectionBox_s
    {
    NW_FBox_FormBox_t super;

    // protected: // Data
    NW_Uint16 iDesiredTextAreaWidthInChars;
    };

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_FBox_FileSelectionBox_GetClassPart(_object) \
    (NW_Object_GetClassPart(_object, NW_FBox_FileSelectionBox))

#define NW_FBox_FileSelectionBoxOf(object) \
    (NW_Object_Cast (object, NW_FBox_FileSelectionBox))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT const NW_FBox_FileSelectionBox_Class_t NW_FBox_FileSelectionBox_Class;

/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */

/**
* Gets the desired width of the file-selection text area.
* @since 2.6
* @param aFileSelectionBox Box class from which to get the text-area width.
* @return Text-area width in pixels.
*/
#define NW_FBox_FileSelectionBox_GetDesiredTextAreaWidthInChars(_fileSelectionBox) \
    (NW_FBox_FileSelectionBoxOf(_fileSelectionBox)->iDesiredTextAreaWidthInChars)

/**
* Determines whether or not the box has content.
* @since 2.6
* @param aFileSelectionBox Box class from which to check for content.
* @return Boolean ETrue if box has content; else, EFalse.
*/
TBool
NW_FBox_FileSelectionBox_HasContent( NW_FBox_FileSelectionBox_t* aThisObj );


/* ------------------------------------------------------------------------- *
   convenience methods
 * ------------------------------------------------------------------------- */

/**
* Creates a new file selection box.
* @since 2.6
* @param aNumProperties ?description
* @param aEventHandler Event handler for this box.
* @param aFormCntrlId Control ID for this box.
* @param aFormLiaison Form liaison that owns this box.
* @param aSize Desired size of the input control; specified in # of chars.
* @param aAppServices Not used.
* @return Pointer to new file selection box structure or null if failure. 
*   Caller assumes ownership of memory and must deallocate but not until
*   after calling the OOC destructor.
*/
NW_FBOX_EXPORT
NW_FBox_FileSelectionBox_t*
NW_FBox_FileSelectionBox_New(
    NW_ADT_Vector_Metric_t aNumProperties,
    NW_LMgr_EventHandler_t* aEventHandler,
    void* aFormCntrlId,
    NW_FBox_FormLiaison_t* aFormLiaison,
    NW_Uint16 aSize,
    NW_HED_AppServices_t* aAppServices );

#endif
