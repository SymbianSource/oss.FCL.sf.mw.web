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


#ifndef NW_FBOX_FILESELECTIONBOXSKIN_H
#define NW_FBOX_FILESELECTIONBOXSKIN_H

//  INCLUDES
#include <e32std.h>

#include "BrsrStatusCodes.h"
#include "nw_fbox_skin.h"
#include "nw_fbox_formbox.h"


// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
typedef struct NW_FBox_FileSelectionBoxSkin_Class_s NW_FBox_FileSelectionBoxSkin_Class_t;
typedef struct NW_FBox_FileSelectionBoxSkin_s NW_FBox_FileSelectionBoxSkin_t;

/* ------------------------------------------------------------------------- *
   virtual method prototypes
 * ------------------------------------------------------------------------- */

typedef
NW_GDI_Metric_t
( *NW_FBox_FileSelectionBoxSkin_GetBaseline_t ) (
    NW_FBox_FileSelectionBoxSkin_t* aThisObj,
    NW_LMgr_Box_t* aLmgrBox );

typedef
TBrowserStatusCode
( *NW_FBox_FileSelectionBoxSkin_Reset_t ) ( NW_FBox_FileSelectionBoxSkin_t* aThisObj );

typedef
TBrowserStatusCode
( *NW_FBox_FileSelectionBoxSkin_SetActive_t ) ( NW_FBox_FileSelectionBoxSkin_t* aThisObj );

typedef
TBrowserStatusCode
( *NW_FBox_FileSelectionBoxSkin_Split_t ) (
    NW_FBox_FileSelectionBoxSkin_t* aThisObj,
    NW_GDI_Metric_t aSpace,
    NW_LMgr_Box_t** aSplitBox,
    NW_Uint8 aFlags );

/* ------------------------------------------------------------------------- *
   OOC class declaration
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_FileSelectionBoxSkin_ClassPart_s
    {
    NW_FBox_FileSelectionBoxSkin_GetBaseline_t getBaseline;
    NW_FBox_FileSelectionBoxSkin_Reset_t reset;
    NW_FBox_FileSelectionBoxSkin_SetActive_t setActive;
    NW_FBox_FileSelectionBoxSkin_Split_t split;
    } NW_FBox_FileSelectionBoxSkin_ClassPart_t;

struct NW_FBox_FileSelectionBoxSkin_Class_s
    {
    NW_Object_Core_ClassPart_t NW_Object_Core;
    NW_Object_Base_ClassPart_t NW_Object_Base;
    NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
    NW_FBox_Skin_ClassPart_t NW_FBox_Skin;
    NW_FBox_FileSelectionBoxSkin_ClassPart_t NW_FBox_FileSelectionBoxSkin;
    };

/* ------------------------------------------------------------------------- *
   object declaration
 * ------------------------------------------------------------------------- */
struct NW_FBox_FileSelectionBoxSkin_s
    {
    NW_FBox_Skin_t super;

    // Member variables.
    HBufC* iFullFileName;   // fully-qualified file name
    TPtrC iFileNameAndExt;  // ptr into above at name.ext only
    };

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_FBox_FileSelectionBoxSkin_GetClassPart(_object) \
    (NW_Object_GetClassPart(_object, NW_FBox_FileSelectionBoxSkin))

#define NW_FBox_FileSelectionBoxSkinOf(object) \
    (NW_Object_Cast (object, NW_FBox_FileSelectionBoxSkin))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT const NW_FBox_FileSelectionBoxSkin_Class_t NW_FBox_FileSelectionBoxSkin_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */

// Documented in nw_fbox_fileselectionboxskini.h.
#define NW_FBox_FileSelectionBoxSkin_GetBaseline(_object, _box) \
    (NW_Object_Invoke (_object, NW_FBox_FileSelectionBoxSkin, getBaseline) \
    ((_object), (_box)))

// Documented in nw_fbox_fileselectionboxskini.h.
#define NW_FBox_FileSelectionBoxSkin_Reset(_object) \
    (NW_Object_Invoke (_object, NW_FBox_FileSelectionBoxSkin, reset) \
    ((_object)))

// Documented in nw_fbox_fileselectionboxskini.h.
#define NW_FBox_FileSelectionBoxSkin_SetActive(_object) \
    (NW_Object_Invoke (_object, NW_FBox_FileSelectionBoxSkin, setActive) \
    ((_object)))

// Documented in nw_fbox_fileselectionboxskini.h.
#define NW_FBox_FileSelectionBoxSkin_Split(_object, _space, _splitBox, _flags) \
    (NW_Object_Invoke (_object, NW_FBox_FileSelectionBoxSkin, split) \
    ((_object), (_space), (_splitBox), (_flags)))

/**
* Gets the desired width of the file-selection text area.
* @since 2.6
* @param _object Ptr to the OOC class structure for this class.
* @return Text-area width in pixels.
*/
#define NW_FBox_FileSelectionBoxSkin_HasContent(_object) \
(((_object->iFullFileName) && (_object->iFileNameAndExt.Length())) ? ETrue : EFalse )



/* ------------------------------------------------------------------------- *
   convenience methods
 * ------------------------------------------------------------------------- */
/**
* Creates a new instance of this box skin class.
* @since 2.6
* @param aFormBox Form box to which the new skin class will belong.
* @return Pointer to new instance of this box.
*/
NW_FBOX_EXPORT
NW_FBox_FileSelectionBoxSkin_t*
NW_FBox_FileSelectionBoxSkin_New( NW_FBox_FormBox_t* aFormBox );

#endif
