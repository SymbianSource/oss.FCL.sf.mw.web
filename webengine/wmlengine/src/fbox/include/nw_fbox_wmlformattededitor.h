/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef WMLFORMATTEDEDITOR_H
#define WMLFORMATTEDEDITOR_H

//  INCLUDES
#include <e32std.h>
#include <aknkeys.h>
#include <e32def.h>
#include <coeinput.h>
#include <frmtview.h>
#include <fepbase.h>
#include <eikedwob.h>
#include <eikrted.h>
#include <eikedwin.h>

#include "nw_fbox_wmlformathandler.h"

// DATA TYPES

// JSky editor modes
enum TWmlMode
    {
    EWmlModeNone,
    EWmlModeHiragana,
    EWmlModeKatakana,
    EWmlModeHankakukana,
    EWmlModeAlphabet,
    EWmlModeNumeric
    };

// Moving direction of the cursor
enum TWmlEdNavigation
    {
    EWmlEditorNavigateLeft,
    EWmlEditorNavigateRight,
    EWmlEditorNavigateDelete
    };

// FORWARD DECLARATIONS

class CEikEdwin;
class TEdwinEvent;
class CAknEdwinState;

// CLASS DECLARATION

/**
*  Adds 'formatting' feature to its base class.
*/
class CWmlFormattedEditor : public CEikRichTextEditor
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        CWmlFormattedEditor( TBool aIsSecret );

    public: // Setter functions

        /**
        * Must be called once!
        * Sets format handler. Sets iSecret attribute. When its true editor
        * acts as password editor, so text is echoed in illegible format.
        * @param aHandler Format handler.
        * @param aSecret New value of iSecret.
        */
        void InitializeL( CWmlFormatHandler* aHandler = NULL,
                          TBool aSecret = EFalse,
                          TWmlMode aMode = EWmlModeNone );
        /**
        * Sets default text colour.
        * @param aDefaultColour Default text colour.
        */
        void SetDefaultColour( TRgb aDefaultColour );

        /**
        * Sets the colour of invalid characters.
        * @param aErrorColour Colour of invalid characters.
        */
        void SetErrorColour( TRgb aErrorColour );

        /**
        * Enables or disables T9.
        * @param aEnable Enable T9.
        */
        void EnableT9( TBool aEnable = ETrue, TBool aIsSecret = EFalse );
        
        /**
        * Sets iCursorPos, to keep it updated when in T9 input
        * mode.
        * @param 
        */
        void SetiCursorPos();

        /**
        * Resets iEventKeyEntered, a flag used to indicate when keyboard input arrives via EVENTKEY,
		* as distinguished from multi-tap input mode.
        * @param 
        */
        void ResetFlagEventKeyEntered();

        /**
        * Returns iEventKeyEntered, a flag used to indicate when keyboard input arrives via EVENTKEY,
		* as distinguished from multi-tap input mode.
        * @param 
        */
        TBool IsEventKeyEntered();
		
		/**
        * Sets the current state case format information for the input box
        * @param 
        */
		void SetEditorCurentCase();
		
		/**
		* Sets the editor cursor position
		* @param position
		*/
		void SetCursorPosL( TInt aPos );

    public: // other functions

        /**
        * Returns value of iSecret attribute.
        * @return True whether it's a password editor.
        */
        TBool IsSecret() const;

        /**
        * Sets iPassword with input from user
        * Works with and without formats.  That is, when iHandler, has value or not.
        */
        void SpecialHandlingPasswordL();

        /**
        * Called on HandleEdwinEvent event to update what is stored in iHandler.iDynamic.
        * Works with and without formats.
        */
        void SpecialCharHandlingL();

        /**
        * Called on EEventKeyUp event to update what is stored in iHandler.iDynamic.
        * Works with and without formats.
        */
        void SpecialStaticHandlingL();

        /**
        * Returns text in readable format.
        * @return Zeroterminated text.
        */
        HBufC* ReadableTextL() const;

        /**
        * Returns text in readable format.
        * @return Zeroterminated text.
        */
        HBufC* ReadableTextLC() const;

        /**
        * Marks invalid characters by colouring them.
        */
        void MarkInvalidCharsL();

        /**
        * Frees memory allocated for password.
        * Must be called before destruction!
        */
        void FreePassword();

        /**
        * @return ETrue if T9 is enabled.
        */
        TBool T9Enabled() const;

        /**
        * Handles edwin events. It is called from CEpoc32InputBox
        * @param aEdwin Source of event.
        * @param aEventType Type of event.
        */
        void HandleEdwinEventL( CEikEdwin* aEdwin, 
                                MEikEdwinObserver::TEdwinEvent aEventType );
        
    public: // Functions from base classes

        /**
        * Returns input capabilities.
        * @return TCoeInputCapabilities
        */
        TCoeInputCapabilities InputCapabilities() const;

        /**
        * Handles key events.
        * @param aKeyEvent	Key event.
        * @param aType		Type.
        * @return Response.
        */
        TKeyResponse OfferKeyEventL
            ( const TKeyEvent& aKeyEvent, TEventCode aType );

        /**
        * Handles clear key events.
        * @param TInt	position of cursor in editor.
        * @return TBool if clear key "handled"
        */
        TBool HandleClearKeyL( const TInt cursorPosition );

        /**
        * Handles paste operation.  Called by CEikEdwin.
        * @param aStartPos Beginning of pasted text.
        * @param aLength Length of pasted text.
        */
        void HandleTextPastedL( TInt aStartPos, TInt& aLength );

        /**
        * Checks if paste operation allowed, comparing clipboard info against format mask.
        * @param aStartPos Beginning of pasted text.
        * @param aLength Length of pasted text.
        * @param aPtr Text from clipboard.
        * @return TBool ETrue==paste is allowed
        */
        TBool CheckIfPasteAllowedL( TInt aStartPos, TInt& aLength, TPtr aPtr );


        /**
        * Sets the text of editor, and formathandler. If it gets null string,
        * it fills the editor with static text.
        * When empty text is set character formats will disappear.
        * @param aDes New text.
        */
        void SetTextL( const TDesC* aDes );

        void Draw(const TRect& aRect) const;

        void DrawContents() ;

    public: // From MEikCcpuEditor

        /**
        * Prohibits cutting in password fields and editors
        * containing some static text.
        * @return Cutting is allowed.
        */
        TBool CcpuCanCut() const;

        /**
        * Calls base method and deletes cut text from iHandler.
        */
        void CcpuCutL();

        /**
        * Prohibits copying in secret editors.
        */
        TBool CcpuCanCopy() const;

        /**
        * Determines if paste operation is allowed.
        * @return Pasting is allowed.
        */
        TBool CcpuCanPaste() const;

        /**
        * Calls appropriate routines (CheckPaste..., CheckCursor...) to determine if paste operation is allowed.
        */
        void DoCanPasteL() const;     

        /**
        * Checks clipboard content against format to see if the content is allowed to be pasted.
        * @param aClipboard clipboard
        * @param aRichText boolean describing RichText or PlainText
        * @return flag indicating paste is ALLOWED == 0; or NOT ALLOWED == -1.
        */
        TInt CheckPasteAllowedCharsL(CClipboard& aClipboard, TBool aRichText) const;

        /**
        * Returns next cursor position that is not a STATIC character cursor position.
        * @param aCursoPos check if this cursor position is at a STATIC character in format
        * @return next non-static cursor position (or present one if okay) or KNotFound if impossible to find next position
        */
        TInt CheckCursorPositionForNextNonStatic( TInt aCursorPos ) const;

        /**
        * Calls base method and updates state.
        */
        void CcpuPasteL();

    public: // New functions

        /**
        * Updates fep state.
        * @param aNaviType Direction.
        * @param aUpdateEditingParameters Editing parameters need to be updated.
        */
        void UpdateStateL( TWmlEdNavigation aNaviType,
                           TBool aUpdateEditingParameters = ETrue,
                           TBool aTextChanged = ETrue );

        /**
        * Sets editing parameters according to format of
        * the given document position, such as input mode, case, etc.
        * @param aDocPos Document position.
        */
        TBool UpdateEditingParameters( TInt aDocPos = -1 );
        
        void SetVkbState (CAknEdwinState* aVkbState) { iVkbState = aVkbState; }

	private:

		/**
        * Resets the text color.
        */
		void UpdateColour(TRgb aDefaultColour);

        /**
        *  for VKB state
        */
        void UpdateFlagsState(TUint flags);
        void UpdateInputModeState(TUint inputMode, TUint permittedInputModes);
        void UpdateCaseState(TUint currentCase, TUint permittedCase);
        

    private:    // Data

        // WML format handler object ( used, not owned )
        CWmlFormatHandler* iHandler;
        CRichText* iRichText;           // rich text document
        TRgb iDefaultColour;            // default text colour
        TRgb iErrorColour;              // colour of invalid text
        TBool iT9;                      // ETrue whether T9 is allowed
        TBool iSecret;                  // ETrue for password editors
        HBufC* iPassword;               // Contains password or NULL
        TBool iOverwrite;               // ETrue if editor is in overwrite mode
        TInt iCursorPos;                // previous cursor position
        TBool iStateChanged;            // ignore extra textupdate events
        TInt iFirstEmpty;               // previous first empty position
        TWmlFormatChar iFormat;         // previous format
        TWmlMode iMode;                 // JSky mode
        TWmlMode iPrevMode;             // previous JSky mode
        TCursorSelection iSelection;    // previous selection
        TBool iLineFeedHasBeenInserted; // lastly inserted char is linefeed
        TBool iShift;                   // shift has been pressed
        TBool iPosAtEnd;                // Is set when cursor pos is noted to be at end of string - to 
                                        // differentiate when cursor pos is at other locations
        TBool iEventKeyEntered;         // To help distinguish keyboard input between EVENTKEY events filtering
		                                // to FBOXCEPOC32INPUTBOX.CPP from multi-tap keyboard (ITU-T) entry
        TInt  iNumberOfCharsInStaticInputBox;  // Cannot rely upon iFirstEmpty with statics in input box, so need a counter
        TWmlEdNavigation  iWmlEdNavigation ;
        TBool iDuplicateEvent;          // for ignoring spurious edwin events when handling multi-tap characters.
        TBool iInputFormatProcess;      // Check for resetting the iEventKeyEntered 
        
        CAknEdwinState* iVkbState; // not owned
        TBool           iPenEnabled;
    };

#endif
            
// End of File
