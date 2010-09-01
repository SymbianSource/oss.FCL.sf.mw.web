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
* Description:  FileSelectionBox OOC class implementation -- a box for XHTML
*   input file, providing button for launching file-selection dialogs, which in
*   turn allows the user to browse the local file system and select a file for
*   upload.
*
*/


// INCLUDE FILES
#include "BrsrStatusCodes.h"
#include "GDIDevicecontext.h"
#include "GDIFont.h"
#include "nw_evt_accesskeyevent.h"
#include "nw_evt_activateevent.h"
#include "nw_evt_clearfieldevent.h"
#include "nw_evt_ecmaevent.h"
#include "nw_evt_focusevent.h"
#include "nw_fbox_fileselectionboxi.h"
#include "nw_fbox_fileselectionboxskin.h"
#include "nw_fbox_inputinteractor.h"
#include "nw_lmgr_boxi.h"
#include "nw_lmgr_rootbox.h"
#include "nw_image_epoc32cannedimage.h"
#include "nwx_string.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// Default text-area width of file-selection control (from O'Reilly book, "HTML
// & XHTML, The Definitive Guide, 5th edition").
const NW_Uint16 KDefaultTextAreaWidthInChars = 20;

// The size of the text-area width will eventually be converted to pixels using
// an average char width value as stipulated in the UI spec, which differs
// based on the font. For example, Chinese is bigger.  In order to prevent
// arithmetic overflow when the size is converted to pixels, a somewhat
// arbitrary max limit is applied.
const NW_Uint16 KDefaultTextAreaWidthMaxChars = 1000;

// An empty string.
_LIT(KEmptyString, "");


// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

static
const
NW_LMgr_IEventListener_Class_t _NW_FBox_FileSelectionBox_IEventListener_Class =
    {
        { /* NW_Object_Core         */
            /* super                  */ &NW_LMgr_IEventListener_Class,
            /* queryInterface         */ _NW_Object_Interface_QueryInterface
        },
        { /* NW_Object_Interface    */
            /* offset                 */ offsetof ( NW_LMgr_ActiveBox_t,  NW_LMgr_IEventListener )
        },
        { /* NW_LMgr_IEventListener */
            /* processEvent           */ _NW_FBox_FileSelectionBox_IEventListener_ProcessEvent
        }
    };

static
const NW_Object_Class_t* const _NW_FBox_FileSelectionBox_InterfaceList[] =
    {
        &_NW_FBox_FileSelectionBox_IEventListener_Class,
        NULL
    };

const
NW_FBox_FileSelectionBox_Class_t NW_FBox_FileSelectionBox_Class =
    {
        { /* NW_Object_Core        */
            /* super                 */ &NW_FBox_FormBox_Class,
            /* queryInterface        */ _NW_Object_Base_QueryInterface
        },
        { /* NW_Object_Base        */
            /* interfaceList         */ _NW_FBox_FileSelectionBox_InterfaceList
        },
        { /* NW_Object_Dynamic     */
            /* instanceSize          */ sizeof ( NW_FBox_FileSelectionBox_t ),
            /* construct             */ _NW_FBox_FileSelectionBox_Construct,
            /* destruct              */ NULL
        },
        { /* NW_LMgr_Box           */
            /* split                 */ _NW_FBox_FileSelectionBox_Split,
            /* resize                */ _NW_FBox_FormBox_Resize,
            /* postResize            */ _NW_LMgr_Box_PostResize,
            /* getMinimumContentSize */ _NW_FBox_FormBox_GetMinimumContentSize,
            /* hasFixedContentSize   */ _NW_FBox_FormBox_HasFixedContentSize,
            /* constrain             */ _NW_LMgr_Box_Constrain,
            /* draw                  */ _NW_FBox_FormBox_Draw,
            /* render                */ _NW_LMgr_Box_Render,
            /* getBaseline           */ _NW_FBox_FileSelectionBox_GetBaseline,
            /* shift                 */ _NW_LMgr_Box_Shift,
            /* clone                 */ _NW_LMgr_Box_Clone
        },
        { /* NW_LMgr_ActiveBox     */
            /* unused                */ NW_Object_Unused
        },
        { /* NW_FBox_FormBox       */
            /* initSkin              */ _NW_FBox_FileSelectionBox_InitSkin,
            /* initInteractor        */ _NW_FBox_FileSelectionBox_InitInteractor,
            /* reset                 */ _NW_FBox_FileSelectionBox_Reset
        },
        { /* NW_FBox_FileSelectionBox */
            /* unused                */ NW_Object_Unused
        }
    };


// LOCAL FUNCTION PROTOTYPES
TBrowserStatusCode
NW_FBox_FileSelectionBox_SetFormLiaisonValue( NW_FBox_FileSelectionBox_t* aThisObj );


// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

// -----------------------------------------------------------------------------
// NW_FBox_FileSelectionBox::NW_FBox_FileSelectionBox
// OOC constructor.
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
_NW_FBox_FileSelectionBox_Construct(
    NW_Object_Dynamic_t* aDynamicObject,
    va_list* aArgp )
    {
    NW_FBox_FileSelectionBox_t* thisObj;
    TBrowserStatusCode status;

    // For convenience.
    thisObj = NW_FBox_FileSelectionBoxOf( aDynamicObject );

    // Invoke superclass constructor.
    status = _NW_FBox_FormBox_Construct( aDynamicObject, aArgp );
    if ( status != KBrsrSuccess )
        {
        return status;
        }

    // Initialize member variables.

    // The size is specified in the XHTML input as desired # of characters.
    // This will eventually be converted to pixels using an average char width
    // value as stipulated in the UI spec, which differs based on the font.
    // For example, Chinese is bigger.  In order to prevent arithmetic overflow
    // when the size is converted to pixels, a somewhat arbitrary max limit is
    // applied. A default size is used if none specified.  The actual size of
    // the input control will also be limited by the width of the display but
    // that is handled later on.
    NW_Uint16 size = (NW_Uint16)va_arg( *aArgp, NW_Uint32 );
    thisObj->iDesiredTextAreaWidthInChars =
        ((size == 0 ) || (size > KDefaultTextAreaWidthMaxChars))
            ? KDefaultTextAreaWidthInChars : size;

    return KBrsrSuccess;
    }

// -----------------------------------------------------------------------------
// NW_FBox_FileSelectionBox::GetBaseline
// Gets the baseline for the content of this box.
// (Other items are commented in header.)
// -----------------------------------------------------------------------------
//
NW_GDI_Metric_t
_NW_FBox_FileSelectionBox_GetBaseline( NW_LMgr_Box_t* aLmgrBox )
    {
    NW_FBox_FormBox_t* formBox;
    NW_FBox_FileSelectionBoxSkin_t* fileSelectionBoxSkin;

    // Parameter assertion block.
    NW_ASSERT( NW_Object_IsInstanceOf( aLmgrBox, &NW_FBox_FormBox_Class ) );

    // Get the 'skin' class through the form.
    formBox = NW_FBox_FormBoxOf( aLmgrBox );
    NW_ASSERT( formBox );
    fileSelectionBoxSkin = NW_FBox_FileSelectionBoxSkinOf( 
        NW_FBox_FormBox_GetSkin( formBox ) );
    NW_ASSERT( fileSelectionBoxSkin );

    // Delegate the operation to the skin class.
    return NW_FBox_FileSelectionBoxSkin_GetBaseline(
        fileSelectionBoxSkin, aLmgrBox );
    }

// -----------------------------------------------------------------------------
// NW_FBox_FileSelectionBox::InitInteractor
// Creates and initializes the interactor for this box.
// (Other items are commented in header.)
// -----------------------------------------------------------------------------
//
NW_FBox_Interactor_t*
_NW_FBox_FileSelectionBox_InitInteractor( NW_FBox_FormBox_t* aFormBox )
    {
    // Parameter assertion block.
    NW_ASSERT( NW_Object_IsInstanceOf( aFormBox, &NW_FBox_FileSelectionBox_Class ) );

    return ( NW_FBox_Interactor_t * ) NW_FBox_InputInteractor_New( aFormBox );
    }

// -----------------------------------------------------------------------------
// NW_FBox_FileSelectionBox::InitSkin
// Creates and initializes the skin class for this box.
// (Other items are commented in header.)
// -----------------------------------------------------------------------------
//
NW_FBox_Skin_t*
_NW_FBox_FileSelectionBox_InitSkin( NW_FBox_FormBox_t* aFormBox )
    {
    // Parameter assertion block.
    NW_ASSERT( NW_Object_IsInstanceOf( aFormBox, &NW_FBox_FileSelectionBox_Class ) );

    return ( NW_FBox_Skin_t * ) NW_FBox_FileSelectionBoxSkin_New( aFormBox );
    }

// -----------------------------------------------------------------------------
// NW_FBox_FileSelectionBox::Reset
// Resets this box; i.e., resets the value of the filename to an empty string
// but does not redraw the field.
// (Other items are commented in header.)
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
_NW_FBox_FileSelectionBox_Reset( NW_FBox_FormBox_t* aFormBox )
    {
    TBrowserStatusCode status;
    NW_FBox_FileSelectionBoxSkin_t* fileSelectionBoxSkin;

    // Parameter assertion block.
    NW_ASSERT( NW_Object_IsInstanceOf( aFormBox, &NW_FBox_FileSelectionBox_Class ) );

    // Get the 'skin' class through the form.
    fileSelectionBoxSkin = NW_FBox_FileSelectionBoxSkinOf( 
        NW_FBox_FormBox_GetSkin( aFormBox ) );
    NW_ASSERT( fileSelectionBoxSkin );

    // Have the skin reset the field, clearing internal storage but does not
    // redraw the field.
    status = NW_FBox_FileSelectionBoxSkin_Reset( fileSelectionBoxSkin );
    if ( status != KBrsrSuccess )
        {
        return status;
        }

    // Reset the form-liaison's value to the cleared value.
    status = NW_FBox_FileSelectionBox_SetFormLiaisonValue(
        NW_FBox_FileSelectionBoxOf( aFormBox) );

    return status;
    }

// -----------------------------------------------------------------------------
// NW_FBox_FileSelectionBox::Split
// Checks to see if this box needs to be split and if so, "splits" it; however, 
// this box cannot be split. It is moved to a new line if not already on one.
// That is the best that can be done to split this box.
// (Other items are commented in header.)
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
_NW_FBox_FileSelectionBox_Split(
    NW_LMgr_Box_t* aLmgrBox,
    NW_GDI_Metric_t aSpace,
    NW_LMgr_Box_t** aSplitBox,
    NW_Uint8 aFlags )
    {
    NW_FBox_FormBox_t* formBox;
    NW_FBox_FileSelectionBoxSkin_t* fileSelectionBoxSkin;

    // Parameter assertion block.
    NW_ASSERT( NW_Object_IsInstanceOf( aLmgrBox, &NW_FBox_FormBox_Class ) );
    NW_ASSERT( aSplitBox );

    // Get the 'skin' class through the form.
    formBox = NW_FBox_FormBoxOf( aLmgrBox );
    NW_ASSERT( formBox );
    fileSelectionBoxSkin = NW_FBox_FileSelectionBoxSkinOf( 
        NW_FBox_FormBox_GetSkin( formBox ) );
    NW_ASSERT( fileSelectionBoxSkin );

    // Delegate the operation to the skin class.
    return NW_FBox_FileSelectionBoxSkin_Split( fileSelectionBoxSkin, 
        aSpace, aSplitBox, aFlags );
    }


/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */

// -----------------------------------------------------------------------------
// NW_FBox_FileSelectionBox::HasContent
// Determines whether or not the box has content.
// (Other items are commented in header.)
// -----------------------------------------------------------------------------
//
TBool
NW_FBox_FileSelectionBox_HasContent( NW_FBox_FileSelectionBox_t* aThisObj )
    {
    NW_FBox_FileSelectionBoxSkin_t* fileSelectionBoxSkin;

    // Parameter assertion block.
    NW_ASSERT( NW_Object_IsInstanceOf( aThisObj, &NW_FBox_FileSelectionBox_Class ) );

    // Get the 'skin' class through the form.
    fileSelectionBoxSkin = NW_FBox_FileSelectionBoxSkinOf( 
        NW_FBox_FormBox_GetSkin( aThisObj ) );
    NW_ASSERT( fileSelectionBoxSkin );

    // Delegate the operation to the skin class.
    return NW_FBox_FileSelectionBoxSkin_HasContent( fileSelectionBoxSkin );
    }

// -----------------------------------------------------------------------------
// NW_FBox_FileSelectionBox::SetActive
// Activate this item, which launches the file-selection dialogs, allowing the
// user to interactively select a file.
// @since 2.6
// @param aThisObj Ptr to the OOC class structure for this class.
// @return Browser status code: OutOfMemory if allocs fail; Failure on any
// other failure; otherwise, Success.
// -----------------------------------------------------------------------------
//
static
TBrowserStatusCode
NW_FBox_FileSelectionBox_SetActive( NW_FBox_FileSelectionBox_t* aThisObj )
    {
    NW_FBox_FileSelectionBoxSkin_t* fileSelectionBoxSkin;

    // Parameter assertion block.
    NW_ASSERT( NW_Object_IsInstanceOf( aThisObj, &NW_FBox_FileSelectionBox_Class ) );

    // Get the 'skin' class through the form.
    fileSelectionBoxSkin = NW_FBox_FileSelectionBoxSkinOf( 
        NW_FBox_FormBox_GetSkin( aThisObj ) );
    NW_ASSERT( fileSelectionBoxSkin );

    // Delegate the operation to the skin class.
    return NW_FBox_FileSelectionBoxSkin_SetActive( fileSelectionBoxSkin );
    }

// -----------------------------------------------------------------------------
// NW_FBox_FileSelectionBox::SetFormLiaisonValue
// Set the value of this file-selection field to the corresponding variable
// within the form liaison.
// @since 2.6
// @param thisObj Ptr to the OOC class structure for this class.
// @return Browser status code.
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
NW_FBox_FileSelectionBox_SetFormLiaisonValue( NW_FBox_FileSelectionBox_t* aThisObj )
    {
    void* formControlId;
    NW_Text_t* legacyText = NULL;
    NW_FBox_FormBox_t* formBox;
    NW_FBox_FormLiaison_t* formLiaison;
    NW_FBox_FileSelectionBoxSkin_t* fileSelectionBoxSkin;

    // Parameter assertion block.
    NW_ASSERT( NW_Object_IsInstanceOf( aThisObj, &NW_FBox_FileSelectionBox_Class ) );

    NW_TRY( status )
        {
        // Get the skin class for this box.  That's where the value is stored.
        formBox = NW_FBox_FormBoxOf( aThisObj );
        NW_ASSERT( formBox );
        fileSelectionBoxSkin = NW_FBox_FileSelectionBoxSkinOf( 
            NW_FBox_FormBox_GetSkin( formBox ) );
        NW_ASSERT( fileSelectionBoxSkin );

        // Convert from Symbian data structure to legacy data structure. For
        // efficiency, the Symbian data structures are used wherever possible, but
        // now it needs to be converted.  Unfortunately, this allocates yet another
        // buffer for the text object but the storage is just a pointer.
        NW_Ucs2* buffer;
        NW_Text_Length_t length;
        if ( fileSelectionBoxSkin->iFullFileName )
            {
            buffer = (NW_Ucs2*)fileSelectionBoxSkin->iFullFileName->Ptr();
            length = (NW_Text_Length_t)fileSelectionBoxSkin->iFullFileName->Length();
            }
        else
            {
            buffer = (NW_Ucs2*)((&KEmptyString)->Ptr());
            length = 0;
            }
        legacyText = (NW_Text_t*)NW_Text_UCS2_New( buffer, length,
            NW_Text_Flags_NullTerminated );
        NW_THROW_OOM_ON_NULL( legacyText, status );

        // Get the form liaison.
        formLiaison = NW_FBox_FormBox_GetFormLiaison( aThisObj );

        // Every time the value in the formLiaison is set, the most-recent-
        // file-selection-path is updated.  This is done before setting the
        // value -- while this method still has control of the buffer.
        TPtrC ptrMostRecentPath( buffer, length );
        status = NW_FBox_FormLiaison_SetMostRecentFileSelectionPath(
            formLiaison, ptrMostRecentPath );
        _NW_THROW_ON_ERROR( status );

        // Set the value in the formLiaison.  Note, the formLiaison takes ownership
        // of the text object iff SetStringValue succeeds.
        formControlId = NW_FBox_FormBox_GetFormCntrlID( aThisObj );
        status = NW_FBox_FormLiaison_SetStringValue(
            formLiaison, formControlId, legacyText );
        if ( status != KBrsrSuccess )
            {
            NW_Object_Delete( legacyText );
            legacyText = NULL;
            NW_THROW( status );
            }
        }
    // status = KBrsrSuccess; // set by NW_CATCH
    NW_CATCH( status )
        {
        }
    NW_FINALLY
        {
        return status;
        }
    NW_END_TRY
    }


/* ------------------------------------------------------------------------- *
   implemented interface methods
 * ------------------------------------------------------------------------- */

// -----------------------------------------------------------------------------
// NW_FBox_FileSelectionBox::IEventListener_ProcessEvent
// Process events for this box: set-active, clear-field, gain/lose focus, etc.
// (Other items are commented in header.)
// -----------------------------------------------------------------------------
//
NW_Uint8
_NW_FBox_FileSelectionBox_IEventListener_ProcessEvent(
    NW_LMgr_IEventListener_t* aEventListener,
    NW_Evt_Event_t* aEvent )
    {
    NW_FBox_FileSelectionBox_t* thisObj;

    // Parameter assertion block.
    NW_ASSERT( NW_Object_IsInstanceOf( aEventListener, &NW_LMgr_IEventListener_Class ) );
    NW_ASSERT( NW_Object_IsInstanceOf( aEvent, &NW_Evt_Event_Class ) );

    // Obtain the implementer.
    thisObj = ( NW_FBox_FileSelectionBox_t* ) NW_Object_Interface_GetImplementer( aEventListener );
    NW_ASSERT( NW_Object_IsInstanceOf( thisObj, &NW_FBox_FileSelectionBox_Class ) );

    // Dispatch on the event type.
    if ( ( NW_Object_Core_GetClass( aEvent ) == &NW_Evt_ActivateEvent_Class )
         || ( NW_Object_Core_GetClass( aEvent ) == &NW_Evt_AccessKeyEvent_Class ) )
        {

        // According to UI spec, when file Upload field is activated, ecma onFocus 
        // event should be delegated
        NW_FBox_FormLiaison_t* formLiason = NW_FBox_FormBox_GetFormLiaison( thisObj );
        (void)NW_FBox_FormLiaison_DelegateEcmaEvent(
            formLiason, NW_FBox_FormBox_GetFormCntrlID( thisObj ),
            NW_Ecma_Evt_OnFocus );
        (void)NW_FBox_FileSelectionBox_SetActive( thisObj );
        (void)NW_FBox_FileSelectionBox_SetFormLiaisonValue( thisObj );
        return NW_LMgr_EventAbsorbed;
        }
    else if ( NW_Object_Core_GetClass( aEvent ) == &NW_Evt_FocusEvent_Class )
        {
        NW_FBox_FormLiaison_t* formLiason = 
                                NW_FBox_FormBox_GetFormLiaison( thisObj );
        NW_ASSERT( formLiason );
        ( void ) NW_FBox_FormLiaison_DelegateEcmaEvent(
                                     formLiason, 
                                     NW_FBox_FormBox_GetFormCntrlID( thisObj ),
                                     NW_Ecma_Evt_OnFocus );
        return NW_LMgr_EventAbsorbed;
        }
    else if ( NW_Object_Core_GetClass( aEvent ) == &NW_Evt_ClearFieldEvent_Class )
        {
        NW_LMgr_Box_t* lmgrBox = 
            NW_LMgr_BoxOf( thisObj );
        NW_ASSERT( lmgrBox );

        (void)_NW_FBox_FileSelectionBox_Reset( NW_FBox_FormBoxOf( lmgrBox ) );
        (void)NW_LMgr_Box_Refresh( lmgrBox );

        return NW_LMgr_EventAbsorbed;
        }

    return _NW_LMgr_ActiveBox_IEventListener_ProcessEvent( aEventListener, aEvent );
    }

/* ------------------------------------------------------------------------- *
   convenience methods
 * ------------------------------------------------------------------------- */

// -----------------------------------------------------------------------------
// NW_FBox_FileSelectionBox::New
// Creates a new file selection box.
// (Other items are commented in header.)
// -----------------------------------------------------------------------------
//
NW_FBox_FileSelectionBox_t*
NW_FBox_FileSelectionBox_New(
    NW_ADT_Vector_Metric_t aNumProperties,
    NW_LMgr_EventHandler_t* aEventHandler,
    void* aFormCntrlId,
    NW_FBox_FormLiaison_t* aFormLiaison,
    NW_Uint16 aSize,
    NW_HED_AppServices_t* aAppServices )
    {
    // Parameter assertion block.
    NW_ASSERT( aEventHandler );
    NW_ASSERT( aFormLiaison );
    NW_ASSERT( aAppServices );

    return ( NW_FBox_FileSelectionBox_t* )
           NW_Object_New( &NW_FBox_FileSelectionBox_Class, aNumProperties, 
                          aEventHandler, aFormCntrlId, aFormLiaison, (NW_Uint32) aSize, 
                          aAppServices );
    }
