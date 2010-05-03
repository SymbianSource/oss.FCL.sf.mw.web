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


#ifndef CEPOC32INPUTBOX_H
#define CEPOC32INPUTBOX_H

//  INCLUDES
#include <e32def.h>
#include <coemain.h>
#include <eikedwob.h>
#include <coeinput.h>
#include <frmtview.h>
#include <fepbase.h>
#include <eikrted.h>
#include <eikseced.h>
#include "NW_FBox_Validator.h"

#include <fepbase.h>
#include <aknedsts.h> 
#include <fepipext.h>

// CONSTANTS
const TInt KBufLength = 512;
#define KFepUid 0x100056de
#define KLineEnterChar 0x21b2

// FORWARD DECLARATIONS

class CEikSecretEditor;
//class CEikRichTextEditor;
class CMyFepContainer;
class CWmlFormatHandler;
class CWmlFormattedEditor;
class CIdle;
class CParaFormatLayer;
class CCharFormatLayer;
class CSharedDataI;
class CSharedDataInteger;


// CLASS DECLARATION

/**
*  CEpoc32InputBox implements the functionality
*  of the WML input element.
*/
class CEpoc32InputBox : public CEikBorderedControl, public MEikEdwinObserver, public MCoeFepObserver,
public MCoeFocusObserver, 
public MCoeFepAwareTextEditor, public MCoeCaptionRetrieverForFep, 
public TCoeInputCapabilities::MCoeFepSpecificExtensions, 
private MCoeFepAwareTextEditor_Extension1
    {
    protected:      // Constructors and destructor
        /*
         * C++ default constructor.
         * @param aNode The parent node.
         */
        CEpoc32InputBox( CCoeControl* aParent,
                         void* aOocInputSkin,
                         const TInt aInitialWrapWidth,
                         const TInt aMaxLength,
                         const CFont& aFont,
                         const TInt aLayoutMode);

        /*
        * Second-phase constructor. Leaves on failure.
        * @param aParent Parent control.
        * @param aPopup this control is used as popup or not
        */
        void ConstructL( const TRect& aInitialRectangle,
                         const TMargins8& aMargins,
                         const TDesC& aInitialText,
                         const TDesC& aFormat,
                         const NW_FBox_Validator_Mode_t aMode,
                         const TBool aIsSecret,
                         const NW_FBox_Validator_EmptyOk_t aIsEmptyOk,
                         const NW_Uint32 aDocPublicId,
                         const TInt aMaxLength );

    public: // Constructors and destructor

        /*
         * Two-phased constructor.
         * @param aNode The parent node.
         */
        static CEpoc32InputBox* NewL( CCoeControl* aParent,
                                      void* aOocInputSkin,
                                      const TRect& aInitialRectangle,
                                      const TMargins8& aMargins,
                                      const TInt aInitialWrapWidth,
                                      const TDesC& aInitialText,
                                      const TDesC& aFormat,
                                      const NW_FBox_Validator_Mode_t aMode,
                                      const TBool aIsSecret,
                                      const NW_FBox_Validator_EmptyOk_t aIsEmptyOk,
                                      const NW_Uint32 aDocPublicId,
                                      const TInt aMaxLength,
                                      const CFont& aFont,
                                      const TInt aLayoutMode);

        /*
         * Destructor.
         */
        virtual ~CEpoc32InputBox();

    public:
        /**
        *  Refreshes the display content
        */
        static TInt RefreshContents(TAny* aParam);


        /**
        * Creates and initializes RichText object for the FormattedEditor
        */
        void SetInitialTextL( const TDesC& aInitialText );


        /**
        * Return last known cursor position
        */
        TUint32 TextLength();

        /**
        * Return ETRUE if format has static character - special handling
        * required for "cancel" processing - see Epoc32Inputskin.cpp.
        */
        TBool IsStaticCharInvolved();

        /**
        * Return last cursor position
        */
        TUint32 CurPos() const;

        /**
        * Returns current input or NULL in OOM situation
        */
        HBufC* ReadableText();

        /**
        * Returns current input or NULL in OOM situation, may leave on OOM
        */
        HBufC* ReadableTextL();

        /**
        * Verifies whether current input is conformant to the format
        * If there are less characters in the input than minimum amout
        * allowed - displays a note to the user
        */
        TBool OkToExit();     // error is trapped
        TBool OkToExitL();    // will leave on error

        /**
        * Sets new rectangle a a wrap width
        * @param aRect Size of the rectanle for input box
        * @param aWrapWidth WrapWidth
        */
        void SetRectAndWrap(const TRect& aRect, TInt aWrapWidth);

        /**
        * Returns input capabilities.
        * @return TCoeInputCapabilities
        */
        TCoeInputCapabilities InputCapabilities() const;

    private:        // New functions

        enum TWmlCursorHandling
                {
                EWmlMoveCursor,
                EWmlScrollDisplay,
                EWmlNoCursor
                };

        /**
        * Makes the cursor visible by moving it to the last visible position,
        * or by scrolling the display depending on the aMode parameter.
        * @param aMode Mode.
        */
        void ShowCursorL( TWmlCursorHandling aMode = EWmlMoveCursor );

        /**
        * Makes the cursor visible depending upon the setting of iEditor->IsSecret,
        *  that is, if the input box for a password field, then there is no cursor
        *  pursuant to the Editing specification
        */
        void DisplayCursorL();
        
        /**
        * notify fep when cursor changes
        */
        void HandleUpdateCursor();


    public: // From MEikEdwinObserver

        /**
        * Handles edwin events.
        * @param aEdwin Source of event.
        * @param aEventType Type of event.
        */
        void HandleEdwinEventL( CEikEdwin* aEdwin, TEdwinEvent aEventType );


    public: // from CCoeControl
        virtual void HandleCompletionOfTransactionL();
      virtual   void HandleStartOfTransactionL() ;
      virtual   void HandlePointerEventL(const TPointerEvent &aPointerEvent);

        /**
        * Handles key events.
        * @param aKeyEvent      Key event.
        * @param aType          Type.
        * @return Response.
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType );

        TInt CountComponentControls() const;

        CCoeControl* ComponentControl( TInt aIndex )  const;

    public: // from MCoeFepObserver
        virtual void HandleChangeInFocus();
        virtual void HandleDestructionOfFocusedItem();
        virtual void FocusChanged(TDrawNow aDrawNow);


    protected: // from CCoeControl

        void SizeChanged();

    private:

      virtual void MCoeFepObserver_Reserved_1();
      virtual void MCoeFepObserver_Reserved_2();
        /**
        * By default EPOC constructor is private.
        */
        void ConstructL();

        /**
        * Helper function for ConstructL.
        * Analyzes the format and initalizes format handler, if necessary.
        */
        void InitFormatHandlerL( const TDesC& aFormat,
                                 const TBool aIsSecret,
                                 TBool& aT9Allowed,
                                 TInt& aMaxLength,
                                 const NW_FBox_Validator_EmptyOk_t aIsEmptyOk,
                                 const NW_Uint32 aDocPublicId );

        /**
        * Shows general note. Shows plural text if aDynamic > 1,
        * singular otherwise.
        */
        void NoteTooFewCharacterL( TInt aDynamic = 0 );

        /**
        * Opens or closes editor.
        * @param aOn True means enabling.
        */
        void AllowEditingL( TBool aOn = ETrue );

        /**
        * Updates character counter on navipane.
        * @param aVisible EFalse hides counter.
        */
        void UpdateTextCounterL( TBool aVisible = ETrue );

        /**
        * Resizes editor, sets cursor, and relayouts card if necessary.
        */
        void HandleTextChangeL();

        /**
        * Retrieves text entered for password from CEikSecretEditor.
        */
        void GetSecretText(TDes& aText);

        /**
        * Retrieves text entered for password from CEikSecretEditor in
        * HBufC format.
        */
        HBufC* ReadableSecretTextL();

        TBool IsExitRequest(TUint aKeyCode);

        /**
        * Callback that is called when predictive setting is changed
        * in general settings.
        * @param aObj Points to the container object
        */
        static TInt PredictiveTextOnOffGSNotifiaction( TAny* aObj );

public:
    // from MCoeFepAwareTextEditor    
    virtual void StartFepInlineEditL(const TDesC& aInitialInlineText, TInt aPositionOfInsertionPointInInlineText, TBool aCursorVisibility, const MFormCustomDraw* aCustomDraw, MFepInlineTextFormatRetriever& aInlineTextFormatRetriever, MFepPointerEventHandlerDuringInlineEdit& aPointerEventHandlerDuringInlineEdit);
    virtual void UpdateFepInlineTextL(const TDesC& aNewInlineText, TInt aPositionOfInsertionPointInInlineText);
    virtual void SetInlineEditingCursorVisibilityL(TBool aCursorVisibility);
    virtual void CancelFepInlineEdit();
    virtual TInt DocumentLengthForFep() const;
    virtual TInt DocumentMaximumLengthForFep() const;
    virtual void SetCursorSelectionForFepL(const TCursorSelection& aCursorSelection);
    virtual void GetCursorSelectionForFep(TCursorSelection& aCursorSelection) const;
    virtual void GetEditorContentForFep(TDes& aEditorContent, TInt aDocumentPosition, TInt aLengthToRetrieve) const; // must cope with aDocumentPosition being outside the range 0 to DocumentLengthForFep()
    virtual void GetFormatForFep(TCharFormat& aFormat, TInt aDocumentPosition) const;
    virtual void GetScreenCoordinatesForFepL(TPoint& aLeftSideOfBaseLine, TInt& aHeight, TInt& aAscent, TInt aDocumentPosition) const;
    virtual void DoCommitFepInlineEditL();
    virtual MCoeFepAwareTextEditor_Extension1* Extension1(TBool& aSetToTrue);
    virtual void MCoeFepAwareTextEditor_Reserved_2();    

public:
    virtual void GetCaptionForFep(TDes& aCaption) const;
    
public:
     // from MCoeFepAwareTextEditor_Extension1
        void SetStateTransferingOwnershipL(CState* aState, TUid aTypeSafetyUid);
        CState* State(TUid aTypeSafetyUid); // this function does *not* transfer ownership

    public:
        //activate the virtual keyboard
        void ActivateVKB();
        
    private:
    
        // for virtual keyboard
        void UpdateInlineTextL(const TDesC& aText);
        void ClearInlineText();
    
    private: 
        
        virtual TBool IsValidCharacter(TInt aChar);
        virtual void MCoeFepSpecificExtensions_Reserved_1(){};
        virtual void MCoeFepSpecificExtensions_Reserved_2(){};

    private:        // Data

        void*        iOocInputSkin; // NW_FBox_Epoc32InputSkin_t*, NOT OWNED
        const CFont& iFont; // font for the editor, NOT OWNED

        CWmlFormattedEditor* iEditor;
        CWmlFormatHandler* iHandler;
        CEikSecretEditor* iContainer;

        TInt iPrevCurPos;
        TInt iLastCurPos;
        TInt iMaxLength;
        TBool iEmptyOk;
        TBool iConfirmed;
        TBool iOkToExitThreadSafety; // prevent re-entering OkToExitL() while dialog is up

        CParaFormatLayer* iGlobalParaLayer;
        CCharFormatLayer* iGlobalCharLayer;

        CCoeControl* iParent;

        TInt iPredictiveTextOn;

        TInt iWrapWidth;
        TBool iEditCommitFlag ;    // Used by "OkToExit" to determine if call to "CommitFepInlineEdit" is proper.
        TBool iFocusObserverAdded;

        CActiveSchedulerWait iActiveSchedulerWait;// Used to update the contents when the control loses the focus and gains focus
        TInt iLayoutMode;
        TRect iRect;

        CState* iState;
        HBufC*  iDocumentText;
        HBufC*  iInlineEditText;
        TInt    iUncommittedOffset; // for VKB cursor adjustment
        TBool   iPenEnabled;
        
        TBool m_cancelFepInlineEditInProgress;
    };

#endif // CEPOC32INPUTBOX_H

// End of File
