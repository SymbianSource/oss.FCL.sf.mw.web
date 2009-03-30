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


// INCLUDE FILES
#include <E32KEYS.H>
#include <E32STD.H>
#include <ptidefs.h>
#include "nw_fbox_CEpoc32InputBox.h"
#include "nw_fbox_WmlFormatHandler.h"
#include "nw_fbox_formboxutils.h"
#include "nw_fbox_WmlFormattedEditor.h"
#include "nw_wml1x_wml_1_3_tokens.h"

#include <aknenv.h>
#include <akneditstateindicator.h>
#include <aknindicatorcontainer.h>
#include <aknutils.h>
#include <barsread.h>       // for resource reader
#include <webkit.rsg>

#include <centralrepository.h>
#include <AknFepInternalCRKeys.h>

#include <eikseced.h>       // for secret editor
#include <eikrted.h>
#include <txtrich.h>
#include <aknnotedialog.h>
#include <gulbordr.h>
#include "NW_Fbox_InputBox.h"
#include "NW_Fbox_inputskini.h"
#include "NW_Fbox_InputSkin.h"
#include "NW_Fbox_Epoc32InputSkin.h"
#include "NW_Fbox_textareabox.h"
#include "BrsrStatusCodes.h"

#include "eikon.hrh"


//#include <aknedsts.h>
//#include <FEPBASE.H>


// CONSTANTS

static const TInt KNumberOfLines = 1000;
_LIT(KBlankDesC," ");

// ================= STATIC FUNCTIONS =======================
TInt CEpoc32InputBox::RefreshContents( TAny* aParam )
{
  CEpoc32InputBox* epoc32InputBox= (CEpoc32InputBox* )aParam ;
  epoc32InputBox->iEditor->DrawContents();
  epoc32InputBox->iActiveSchedulerWait.AsyncStop();
  return 0;
}

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CEpoc32InputBox::CEpoc32InputBox( CCoeControl* aParent,
                                  void* aOocInputSkin,
                                  const TInt aInitialWrapWidth,
                                  const TInt aMaxLength,
                                  const CFont& aFont,
                                  const TInt aLayoutMode) :
    iOocInputSkin ( aOocInputSkin ),
    iFont( aFont ),
    iMaxLength( aMaxLength ),
    iParent( aParent ),
    iWrapWidth( aInitialWrapWidth ),
    iLayoutMode(aLayoutMode)
    {
    // ignore negative values, and max possible value (it means that maxlength is not specified)
    if ( iMaxLength < 0 )
        {
        iMaxLength = 0;
        }
    }

// Initialize format mask handler
//
void CEpoc32InputBox::InitFormatHandlerL( const TDesC& aFormat,
                                          const TBool aIsSecret,
                                          TBool& aT9Allowed,
                                          TInt& aMaxLength,
                                          const NW_FBox_Validator_EmptyOk_t aIsEmptyOk,
                                          const NW_Uint32 aDocPublicId )
    {
    // T9 is not allowed for password boxes (or touch)
    aT9Allowed = (!aIsSecret && !iPenEnabled);


    // get TWapDocType from aDocPublicId
    TWapDocType wapDocType;
    switch ( aDocPublicId )
        {
        case NW_Wml_1_0_PublicId:
            wapDocType = EWmlDocType_Wml_1_0;
            break;

        case NW_Wml_1_1_PublicId:
            wapDocType = EWmlDocType_Wml_1_1;
            break;

        case NW_Wml_1_2_PublicId:
            wapDocType = EWmlDocType_Wml_1_2;
            break;

        case NW_Wml_1_3_PublicId:
            wapDocType = EWmlDocType_Wml_1_3;
            break;

        default:
            wapDocType = EWmlDocType_Unknown;
            break;
        }

    if ( aFormat.Length() )  // is FORMAT specified?
        {
        // create format handler
        iHandler = CWmlFormatHandler::NewL( aFormat, wapDocType );

        // Is it other than "*f" or "nf" where f is not "n" or "N" ?
        if ( aFormat.Length() != 2 || !iHandler->PostFix().AlphaAllowed() )
            {
            aT9Allowed = EFalse;
            }
        }

    TInt formatMax( iHandler ? iHandler->MaxLength() : KInfiniteLength );

    if ( iMaxLength && formatMax != KInfiniteLength ) // both are specified
        {
        aMaxLength = Min( formatMax, iMaxLength );
        }
    else
        {
        aMaxLength = iMaxLength;
        }

  if ( aIsEmptyOk == NW_FBox_Validator_EmptyOk_True )
    {
    iEmptyOk = ETrue;
    }
  else if ( aIsEmptyOk == NW_FBox_Validator_EmptyOk_False )
        {
        iEmptyOk = EFalse;
        }
    else
        {
        // emptyok is not specified.
        // if there is no iHandler value, there is no format specified, too
        // so use a default format - *m, which implies emptyok=true
        if ( !iHandler || wapDocType == EWmlDocType_Unknown )
            {
            iEmptyOk = ETrue;
            }
        if ( iHandler )     // Is format attribute specified?
            {
            iEmptyOk = !iHandler->MinLength() &&   // Does it allow empty input?
                         (wapDocType == EWmlDocType_Wml_1_3 || wapDocType == EWmlDocType_Unknown);    // Is it WML 1.3?
            }        // for unknown documents (like XHTML) iEmptyOk will be true
    }
    }


// EPOC default constructor can leave.
//
void CEpoc32InputBox::ConstructL( const TRect& aInitialRectangle,
                                  const TMargins8& aMargins,
                                  const TDesC& aInitialText,
                                  const TDesC& aFormat,
                                  const NW_FBox_Validator_Mode_t aMode,
                                  const TBool aIsSecret,
                                  const NW_FBox_Validator_EmptyOk_t aIsEmptyOk,
                                  const NW_Uint32 aDocPublicId,
                                  const TInt aMaxLength )
    {
    TBool t9Allowed;
    TInt maxLength = aMaxLength;
    TWmlMode mode;
    iContainer = NULL;

    iPenEnabled = AknLayoutUtils::PenEnabled();

    // if password input box, create the secret editor for it
    if ( aIsSecret )
        {
        TInt pwLength = aMaxLength;

        iContainer = new (ELeave) CEikSecretEditor;

        iContainer->SetMopParent(this);

        iContainer->AknSetFont( iFont );
        iContainer->SetBorder( TGulBorder::ESingleBlack );

        iEikonEnv->AddFepObserverL( *this );

        // add ourselves to the controls stack, so that we'll recieve key events
        iEikonEnv->EikAppUi()->AddToStackL( this );
        iEikonEnv->AddFocusObserverL(*this);

        CreateWindowL( iParent );

        TResourceReader reader;

        iCoeEnv->CreateResourceReaderLC(reader, R_WMLBROWSER_SECRET_EDITOR);

        iContainer->SetContainerWindowL(*this);

        iContainer->ConstructFromResourceL(reader);

        CleanupStack::PopAndDestroy();  // Resource reader

        ActivateL();

        if (pwLength == KInfiniteLength || pwLength < 0 || pwLength > CEikSecretEditor::EMaxSecEdSecArrayLength )
            {
            pwLength = CEikSecretEditor::EMaxSecEdSecArrayLength;
            }

        iContainer->SetMaxLength( pwLength );

        InitFormatHandlerL( aFormat,
                            aIsSecret,
                            t9Allowed,
                            pwLength,
                            aIsEmptyOk,
                            aDocPublicId );

        TRect adjustedRect;
        adjustedRect.iTl.iX = aInitialRectangle.iTl.iX;
        adjustedRect.iTl.iY = aInitialRectangle.iTl.iY;
        adjustedRect.iBr.iX = aInitialRectangle.iBr.iX;
        adjustedRect.iBr.iY = aInitialRectangle.iBr.iY;
        adjustedRect.Shrink(1, 1);
        iContainer->SetRect(adjustedRect);
        iRect = adjustedRect;
        iFocusObserverAdded= ETrue;

        if (aFormat.Length() > 0 && (aFormat[0] == 'n' || aFormat[0] == 'N'))
            {
            iContainer->SetDefaultInputMode(EAknEditorNumericInputMode);
            }
        else
            {
            iContainer->SetDefaultInputMode(EAknEditorSecretAlphaInputMode);
            }

        iContainer->SetText( aInitialText.Left( pwLength ) );

        iEditor = NULL;

        AllowEditingL( ETrue );

        ActivateVKB();

        return;
        }
    else
        {
        CreateWindowL( iParent );
        }

    // Create formatted editor
    iEditor = new( ELeave )CWmlFormattedEditor(aIsSecret);

    // Create/Get the Avkon FEP repository
    CRepository* aknFepRepository = CRepository::NewL( KCRUidAknFep );
    User::LeaveIfNull( aknFepRepository );

    // Read the predictive text setting
#ifdef RD_INTELLIGENT_TEXT_INPUT
    TPtiKeyboardType keyboardType( EPtiKeyboardNone );
    TInt predTxtMask( 0 );

    aknFepRepository->Get( KAknFepPhysicalKeyboards, (TInt&)keyboardType );
    aknFepRepository->Get( KAknFepPredTxtFlagExtension, predTxtMask );

    switch( keyboardType )
        {
        case EPtiKeyboardNone:
            // No hardware keyboard
            break;

        case EPtiKeyboard12Key:
            // ITU-T keyboard predictive text bit mask enabled
            predTxtMask &= 0x01; // EKeyboardStyle12Key define missing in AknFepInternalCRKeys.h
            break;

        case EPtiKeyboardQwerty4x12:
        case EPtiKeyboardQwerty4x10:
        case EPtiKeyboardQwerty3x11:
        case EPtiKeyboardHalfQwerty:
        case EPtiKeyboardCustomQwerty:
            // qwerty keyboard predictive text bit mask enabled
            predTxtMask &= 0x02; // EKeyboardStyleQwerty define missing in AknFepInternalCRKeys.h
            break;

        default:
            break;
        }

    iPredictiveTextOn = ( predTxtMask ? 1 : 0);
#else
        // Check repository for predictive text enable flag
        aknFepRepository->Get( KAknFepPredTxtFlag, iPredictiveTextOn );
#endif

    // Delete the repository
    delete aknFepRepository;
    aknFepRepository = NULL;

    // create format handler and get t9allowed and maxLength
    InitFormatHandlerL( aFormat,
                        aIsSecret,
                        t9Allowed,
                        maxLength,
                        aIsEmptyOk,
                        aDocPublicId );

    // initialize formatted editor
    iEditor->ConstructL( iParent,
                         KNumberOfLines,
                         maxLength,
                         CEikEdwin::ENoAutoSelection );

    iEditor->AddFlagToUserFlags ( CEikEdwin::EJustAutoCurEnd );
    iEditor->EnableT9( t9Allowed, aIsSecret );
    switch ( aMode )
    {
        case NW_FBox_Validator_Mode_Alphabet:
            mode = EWmlModeAlphabet;
            break;

        case NW_FBox_Validator_Mode_Numeric:
            mode = EWmlModeNumeric;
            break;

        case NW_FBox_Validator_Mode_Hiragana:
            mode = EWmlModeHiragana;
            break;

        case NW_FBox_Validator_Mode_Katakana:
            mode = EWmlModeKatakana;
            break;

        case NW_FBox_Validator_Mode_Hankakukana:
            mode = EWmlModeHankakukana;
            break;

        default:
            mode = EWmlModeNone;
            break;
        }
    iEditor->InitializeL( iHandler, aIsSecret, mode );
    iEditor->SetBorderViewMargins( aMargins );
    iEditor->SetBorder( TGulBorder::ESingleBlack );

      // set ourselves to be observer of the formatted editor
    iEditor->SetEdwinObserver( this );
    iEikonEnv->AddFepObserverL(*this);
    // add ourselves to the controls stack, so that we'll recieve key events
    iEikonEnv->EikAppUi()->AddToStackL( this );

    iEditCommitFlag  = EFalse;
    const NW_Object_t* object = NW_FBox_InputSkin_GetInputBox ((NW_FBox_Epoc32InputSkin_t *)iOocInputSkin);
    if (!NW_Object_IsClass(object, &NW_FBox_TextAreaBox_Class))
        {   // if NOT textarea box...
        SetRect( aInitialRectangle );
    iEditor->AddFlagToUserFlags (CEikEdwin::ENoLineOrParaBreaks|EEikEdwinNoWrap);
    iEditor->SetFocus(ETrue);
    iEditor->SetRect(aInitialRectangle);
        iEikonEnv->AddFocusObserverL(*this);
        iFocusObserverAdded= ETrue;
    }
    else
        {  // if textarea box...
        iEditor->AddFlagToUserFlags (CEikEdwin::ENoHorizScrolling);
        iEditor->ActivateL();
        SetRectAndWrap( aInitialRectangle, iWrapWidth );
        iFocusObserverAdded= EFalse;
        }
    // replace all NW_TEXT_UCS2_LF with NW_TEXT_UCS2_PARASEP
    TInt pos;
    HBufC* buf = aInitialText.AllocLC();
    TPtr ptr = buf->Des();
    while ( (pos=ptr.Locate(TChar(NW_TEXT_UCS2_LF))) != KErrNotFound )
       {
       ptr[pos] = NW_TEXT_UCS2_PARASEP;
       if ( pos > 0 && ptr[pos-1] == NW_TEXT_UCS2_CR )
           {
           ptr.Delete(pos-1,1);
           }
       }

    // replace all NW_TEXT_UCS2_TAB with ' '
    ptr = buf->Des();
    while ( (pos=ptr.Locate(TChar(NW_TEXT_UCS2_TAB))) != KErrNotFound )
       {
       ptr[pos] = ' ';
       }

    // set initial text
    SetInitialTextL( buf->Des() );

    CleanupStack::PopAndDestroy(); // buf

    iEditor->SetAllowUndo( EFalse );

    // start editing
    AllowEditingL( ETrue );

    UpdateTextCounterL();

    ActivateVKB();

    }


// Two-phased constructor.
CEpoc32InputBox* CEpoc32InputBox::NewL( CCoeControl* aParent,
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
                                        const TInt aLayoutMode)
    {

    CEpoc32InputBox* thisObj = new( ELeave )CEpoc32InputBox( aParent,
                                                             aOocInputSkin,
                                                             aInitialWrapWidth,
                                                             aMaxLength,
                                                             aFont,
                                                             aLayoutMode);

    CleanupStack::PushL( thisObj );

    thisObj->ConstructL( aInitialRectangle,
                         aMargins,
                         aInitialText,
                         aFormat,
                         aMode,
                         aIsSecret,
                         aIsEmptyOk,
                         aDocPublicId,
                         aMaxLength );

    CleanupStack::Pop(); // thisObj

    return thisObj;
    }

// Destructor
CEpoc32InputBox::~CEpoc32InputBox()
    {
    // remove ourselves from controls stack
    iEikonEnv->EikAppUi()->RemoveFromStack(this);

    if (iFocusObserverAdded)
      {
      iEikonEnv->RemoveFocusObserver(*this);
      }
    iEikonEnv->RemoveFepObserver(*this);

    if (iEditor)
      {
      iEditor->CancelFepTransaction();        // cancel inline editing
      }

    TRAP_IGNORE( AllowEditingL( EFalse ) );   // close editor

    if ( iEditor )
      {
      iEditor->SetVkbState(NULL);
      iEditor->FreePassword();  // must be called before destruction!!!!
      delete iEditor;
      }

    delete iInlineEditText;
    delete iDocumentText;
    delete iState;
    delete iHandler;
    delete iContainer;

    // release editor's rich text content
    delete iGlobalParaLayer;
    delete iGlobalCharLayer;
    }

// ---------------------------------------------------------
// CEpoc32InputBox::ShowCursorL
// ---------------------------------------------------------
void CEpoc32InputBox::ShowCursorL( TWmlCursorHandling aMode )
    {
    // get cursor co-ordinates
    TPoint cursorPos;
        iEditor->TextView()->DocPosToXyPosL( iEditor->CursorPos(), cursorPos );

    TInt width = iFont.MaxCharWidthInPixels();
    TInt height, ascent;
        iEditor->TextLayout()->GetFontHeightAndAscentL
            ( iFont.FontSpecInTwips(), height, ascent );

    // visible part of the textview
    TRect viewRect( iEditor->TextView()->ViewRect() );
    TRect browserViewRect;
    NW_FBox_Epoc32InputSkin_GetViewRect(
                                (NW_FBox_Epoc32InputSkin_t *)iOocInputSkin,
                                browserViewRect );


    TBool above = cursorPos.iY - height < browserViewRect.iTl.iY;
    TBool below = Min (cursorPos.iY , ((viewRect).iBr).iY) > browserViewRect.iBr.iY;
    viewRect.Intersection( browserViewRect );

    // Is cursor invisible?
    if ( above || below )
    {
    switch ( aMode )
        {
        case EWmlMoveCursor:
          {
            // get the position of the character in the bottom right corner
            TPoint xYPos = viewRect.iBr - TPoint( width, height );
            TInt docPos = iEditor->TextView()->XyPosToDocPosL( xYPos );
            if ( docPos > 0 )
            {
              // if input is not empty - place cursor before the
              // character, not after (because it'll jump onto the next line)
              docPos--;
            }
            iEditor->SetCursorPosL( docPos );
          }
          break;

        case EWmlNoCursor:
            {
            iEditor->TextView()->SetCursorVisibilityL(      // hide cursor
                TCursor::EFCursorInvisible,
                TCursor::EFCursorInvisible );
            break;
            }

        case EWmlScrollDisplay:
            {
                iEditor->TextView()->SetCursorVisibilityL(      // hide cursor
                    TCursor::EFCursorInvisible,
                    TCursor::EFCursorInvisible );

            // calculate amount of pixels to scroll
            TInt deltaY = ( below )
                ? viewRect.iBr.iY - cursorPos.iY + ascent - height
                : viewRect.iTl.iY - cursorPos.iY + ascent;

            NW_FBox_Epoc32InputSkin_Scroll(
                (NW_FBox_Epoc32InputSkin_t *)iOocInputSkin, deltaY );

            /* update Edwin editor position */
            /* cursor position business is here because */
            /* SetRectAndWrap calls TextView's reformat and ruins cursor pos */
            TRect rc = Rect();
            rc.Move( TPoint( 0, deltaY ) );

            const NW_Object_t* object = NW_FBox_InputSkin_GetInputBox ((NW_FBox_Epoc32InputSkin_t *)iOocInputSkin);
            if (!NW_Object_IsClass(object, &NW_FBox_TextAreaBox_Class))
                {
            SetRect(rc);
                if ( iEditor )
                    {
                    iEditor->TextView()->FormatTextL();
                    }
                }
            else
                {
                SetRectAndWrap( rc, iWrapWidth );
                }

            /* update offscreen bitmap and redraw the screen */
            NW_FBox_Epoc32InputSkin_t* epoc32InputSkin =
                        (NW_FBox_Epoc32InputSkin_t*)iOocInputSkin;
            NW_FBox_Epoc32InputSkin_RedrawScreen( epoc32InputSkin );

                iEditor->TextView()->SetCursorVisibilityL(      // show cursor
                    TCursor::EFCursorInvisible,
                    TCursor::EFCursorFlashing );
            DrawDeferred();
          }
          break;

        default:
            {
            break;
            }
         }
      }
    }

// ---------------------------------------------------------
// CEpoc32InputBox::HandleEdwinEventL
// Handles multi-tap input from Avkon
// ---------------------------------------------------------
void CEpoc32InputBox::HandleEdwinEventL
( CEikEdwin* aEdwin, TEdwinEvent aEventType )
    {
    if (iMaxLength == 0 && aEventType == EEventTextUpdate)
        {
        return;
        }

    if ( aEventType == EEventNavigation )
        {
        iEditor->HandleEdwinEventL( aEdwin, aEventType );
        DrawNow();
        }
    else
        {
        // editor isn't in T9 mode and it isn't cancelled
        if ( !( iEditor->T9Enabled() && iPredictiveTextOn ) &&

             ( iConfirmed || iEditor->IsFocused() ) )
            {
            DisplayCursorL();
            iEditor->HandleEdwinEventL( aEdwin, aEventType );
            UpdateTextCounterL();
            HandleTextChangeL();
            }
        else
            {  // The following is done when switching between T9 and other input modes to
               // reestablish some key variables for use in wmlformattededitor.
            iEditor->SetiCursorPos();
            }

        }
  }

// ---------------------------------------------------------
// CEpoc32InputBox::OfferKeyEventL
// ---------------------------------------------------------
TKeyResponse CEpoc32InputBox::OfferKeyEventL
( const TKeyEvent& aKeyEvent, TEventCode aType )
{
    if (iMaxLength == 0)
        {
        NW_FBox_Epoc32InputSkin_Redraw(NW_FBox_Epoc32InputSkinOf(iOocInputSkin));
        NW_FBox_Epoc32InputSkin_TryExitEditMode(NW_FBox_Epoc32InputSkinOf(iOocInputSkin), NW_FALSE);
        return EKeyWasConsumed;
        }

    if (iContainer)
       {
       iContainer->OfferKeyEventL( aKeyEvent, aType );
       }
    else
       {
       if (aType == EEventKeyDown)
           {
           iPrevCurPos = iEditor->CursorPos();
           }
       // call OfferKeyEvent of CWmlFormattedEditor
       iEditor->OfferKeyEventL( aKeyEvent, aType );
       }

    switch (aKeyEvent.iScanCode)
           {
            case EKeyEnter:
            case EStdKeyLeftFunc:
            case EStdKeyDevice0:
              break;

            default :
                iEditCommitFlag = ETrue;
              break;
            }


        if ( IsExitRequest(aKeyEvent.iCode) )
            {
            NW_FBox_Epoc32InputSkin_TryExitEditMode(NW_FBox_Epoc32InputSkinOf(iOocInputSkin), NW_TRUE);
            }
        else
            {
            if (iEditor)
                {
                    UpdateTextCounterL();
                    // Paste or Cut Text
                    HandleTextChangeL();
                iLastCurPos = iEditor->CursorPos();
                if (iHandler && !iPrevCurPos && aType == EEventKeyUp && aKeyEvent.iScanCode == EStdKeyLeftArrow)
                    {
                    iEditor->SetCursorPosL( iHandler->FirstEmpty() );
                    }
                }
            }

    return EKeyWasConsumed;
    }

// ---------------------------------------------------------
// CEpoc32InputBox::SetInitialTextL
// Creates and initializes RichText object for the FormattedEditor
// ---------------------------------------------------------
//
void CEpoc32InputBox::SetInitialTextL( const TDesC& aInitialText )
    {
    // if this routine is called by ECMA implementation for a password box,
    // simply set the text into the SecretEditor and leave.
    if (iContainer && !iEditor)
        {
        iContainer->SetText( aInitialText );
        return;
        }
    // Create paragraph format layer
    CParaFormat* paraFormat = CParaFormat::NewLC();
    paraFormat->iLineSpacingInTwips = NW_FBOX_INPUTBOX_LINE_SPACING_IN_TWIPS;
    TParaFormatMask paraMask;
    paraMask.SetAttrib( EAttLineSpacing );
    iGlobalParaLayer = CParaFormatLayer::NewL( paraFormat, paraMask );
    CleanupStack::PopAndDestroy();  // paraFormat

    // Create character format layer
    TCharFormat charFormat;
    charFormat.iFontSpec = iFont.FontSpecInTwips();
    TCharFormatMask charMask;
    charMask.SetAttrib( EAttFontTypeface );
    charMask.SetAttrib( EAttFontHeight );
    iGlobalCharLayer = CCharFormatLayer::NewL( charFormat, charMask );

    // Create richtext object with the appropriate layers
    CRichText* richText = CRichText::NewL( iGlobalParaLayer, iGlobalCharLayer );
    CleanupStack::PushL( richText );
    // make it used by the editor
    iEditor->SetDocumentContentL( *richText, CEikEdwin::ECopyText );
    CleanupStack::PopAndDestroy();          // richText

    NW_FBox_Epoc32InputSkin_t* thisObj = NW_FBox_Epoc32InputSkinOf( iOocInputSkin );
    const NW_Object_t* object = NW_FBox_InputSkinOf(thisObj);
    NW_FBox_InputBox_t* inputBox = NW_FBox_InputSkin_GetInputBox (object);


    // position the cursor appropriately
    // format exists and either initial value specified in content or value entered by user are present;
    if ( aInitialText.Length() == 0 ||
         ( thisObj->reallyEdited == NW_FALSE &&
           (iHandler && iHandler->NumberOfStatic() > 0) &&
           (inputBox && inputBox->initialValueUsed == NW_FALSE) ) )
    {                                   // this must be first time seen(cursorval==0)
      iEditor->SetTextL( NULL);
      iPrevCurPos = 0;
    }
    else                                // initial text probably present
    {
      iEditor->SetTextL( &aInitialText );           // put into the editor
      UpdateInlineTextL(aInitialText);
      DoCommitFepInlineEditL();
      iPrevCurPos = ( thisObj->reallyEdited ? thisObj->posCursorValue : aInitialText.Length());
    }
    iLastCurPos = iEditor->CursorPos();
    thisObj->prevCursorValue = iPrevCurPos;             // save cursor pos for cancel event handling
    iEditor->SetAlignment(iLayoutMode);

    }

// ---------------------------------------------------------
// CEpoc32InputBox::AllowEditingL
// Called by constructor and destructor,
// for edwin editor:
//     it sets and unsets focus
//     sets cursor
//     updates state
//     display cursor
// for secret-editor:
//     only focus change is used
// ---------------------------------------------------------
//
void CEpoc32InputBox::AllowEditingL( TBool aOn )
    {

    if (iContainer)  // if CEikSecretEditor password input box, there is no iEditor
        {
        iContainer->SetFocus( aOn );
        CCoeEnv::Static()->SyncNotifyFocusObserversOfChangeInFocus();
        }
    else if (iEditor)
        {
        iEditor->SetFocus( aOn );
        UpdateTextCounterL( aOn );

        if (aOn)
            {
            /* if pointing to a non-existant cursor location, set the cursor to the text length instead */
            TInt temp = (iPrevCurPos > iEditor->TextLength()) ? iEditor->TextLength() : iPrevCurPos;

            iEditor->SetCursorPosL(temp);
            iEditor->UpdateStateL( EWmlEditorNavigateRight);
            DisplayCursorL();
            }

        CCoeEnv::Static()->SyncNotifyFocusObserversOfChangeInFocus();

        iEditor->SetEditorCurentCase();
        }
    }

// ---------------------------------------------------------
// CEpoc32InputBox::DisplayCursorL
// ---------------------------------------------------------
//
void CEpoc32InputBox::DisplayCursorL()
    {
    if (iContainer)
        {
        return;
        }
    TWmlCursorHandling displayCursor = EWmlScrollDisplay;
    if (iEditor->IsSecret())
        {
        displayCursor = EWmlNoCursor;
        }
    ShowCursorL( displayCursor );
    }

// ---------------------------------------------------------
// CEpoc32InputBox::UpdateTextCounterL
// ---------------------------------------------------------
//
void CEpoc32InputBox::UpdateTextCounterL( TBool aVisible )
    {
    MAknEditingStateIndicator* editingStateIndicator =
        CAknEnv::Static()->EditingStateIndicator();

    if ( editingStateIndicator )
        {
        CAknIndicatorContainer* indi =
            editingStateIndicator->IndicatorContainer();

        if ( indi )
            {
            TUid id = TUid::Uid( EAknNaviPaneEditorIndicatorMessageLength );

            if ( aVisible && iMaxLength >= 0 && !iEditor->IsSecret() )
                {
                TBuf<32> buf;           // 32 digit must be enough!
                buf.Num( iMaxLength - iEditor->TextLength() );
                indi->SetIndicatorValueL( id, buf );
                indi->SetIndicatorState( id, EAknIndicatorStateOn );
                }
            else
                {
                indi->SetIndicatorState( id, EAknIndicatorStateOff );
                }
            }
        }
    }

// ---------------------------------------------------------
// CEpoc32InputBox::ComponentControl
// ---------------------------------------------------------
//
CCoeControl* CEpoc32InputBox::ComponentControl( TInt aIndex ) const
    {
    CCoeControl* retVal=NULL;

    switch ( aIndex )
        {
        case 0:
            {
            if (iEditor)
                {
                retVal = iEditor;
                }
            else
                {
                retVal = iContainer;
                }
            break;
            }

        default:
            {
            retVal = NULL;
            break;
            }
        }

    return retVal;
    }

// ---------------------------------------------------------
// CEpoc32InputBox::CountComponentControls
// ---------------------------------------------------------
//
TInt CEpoc32InputBox::CountComponentControls() const
    {
    const TInt KNumberOfControls    = 1;
    TInt retVal = 0;
    if (iEditor)
        {
        retVal = KNumberOfControls;
        }
    if (iContainer)
        {
        retVal = KNumberOfControls;
        }
    return retVal;
    }

// ---------------------------------------------------------
// CEpoc32InputBox::HandleTextChangeL
// ---------------------------------------------------------
//
void CEpoc32InputBox::HandleTextChangeL()
    {
    TUint pushCnt = 0;
    HBufC* buf = NULL;
    // if text has been changed - update it to the input skin
    if (iContainer)
        {
        buf = ReadableSecretTextL();
        CleanupStack::PushL( buf );
        ++pushCnt;
        }
    else
        {
        buf = iEditor->ReadableTextLC();
        ++pushCnt;
        }

    // replace all NW_TEXT_UCS2_PARASEP with NW_TEXT_UCS2_LF
    TInt pos;
    TPtr ptr = buf->Des();
    while ( (pos=ptr.Locate(TChar(NW_TEXT_UCS2_PARASEP))) != KErrNotFound )
        {
        ptr[pos] = NW_TEXT_UCS2_LF;
        }

    TBool textChanged = (TBool)NW_FBox_Epoc32InputSkin_TextChanged(
                                  NW_FBox_Epoc32InputSkinOf( iOocInputSkin ),
                                  buf->Des().Ptr() );

    if ( textChanged )
        {
        // update value to the skin and draw new text with our GDI to the
        // offscreen bitmap, so that if redraw event will come, we'll draw
        // correct content
        if ( NW_FBox_Epoc32InputSkin_SetValue(
                                  (NW_FBox_Epoc32InputSkin_t*)iOocInputSkin,
                                  buf->Des().Ptr(),
                                  (NW_Text_Length_t)buf->Length() ) != KBrsrSuccess )
            {
            User::Leave(KErrNoMemory);
            }
        if (!iUncommittedOffset)
            {
            ClearInlineText();
            delete iDocumentText;
            iDocumentText = NULL;
            UpdateInlineTextL(*buf);
            iDocumentText = iInlineEditText->AllocL();
            }
        }

     CleanupStack::PopAndDestroy(pushCnt); // buf
     if (iEditor)
        {
        DisplayCursorL();
        HandleUpdateCursor();
        }
    }

// ---------------------------------------------------------
// CEpoc32InputBox::TextLength
// ---------------------------------------------------------
//
TUint32 CEpoc32InputBox::TextLength()
{
  // if iHandler is NULL the return value does not matter
  return iHandler?iHandler->FirstEmpty():0;
}


// ---------------------------------------------------------
// CEpoc32InputBox::IsStaticCharsInvolved
// ---------------------------------------------------------
//
TBool CEpoc32InputBox::IsStaticCharInvolved()
{
  if (!iHandler || !iHandler->NumberOfStatic() )
      {
      return EFalse;
      }
  return ETrue;
}


// ---------------------------------------------------------
// CEpoc32InputBox::CurPos
// ---------------------------------------------------------
//
TUint32 CEpoc32InputBox::CurPos() const
{
  if (iContainer)
      {
      return 0;
      }
  return ( iEditor->CursorPos() );
}

// ---------------------------------------------------------
// CEpoc32InputBox::OkToExit
// ---------------------------------------------------------
//
TBool CEpoc32InputBox::OkToExit()
    {
    TBool result = EFalse;

    if (!iOkToExitThreadSafety)
        {
        iOkToExitThreadSafety = ETrue;
        TRAPD( err, result = OkToExitL() );
        iOkToExitThreadSafety = EFalse;
        if (err)
            {
            result = EFalse;
            }
        }
    return result;
    }

// ---------------------------------------------------------
// CEpoc32InputBox::OkToExitL
// ---------------------------------------------------------
//
TBool CEpoc32InputBox::OkToExitL()
    {
    if (iUncommittedOffset)
        {
        DoCommitFepInlineEditL();
        }
    if (iContainer)  // if under CEikSecretEditor control
      {
      TBool response = ETrue;
      HandleTextChangeL();  // update the skin with the present input value of control
      HBufC* buf = ReadableSecretTextL();
      if (! iEmptyOk && ! buf->Length())
          {
          NW_FBox_InputBox_t* inputBox = NW_FBox_InputSkin_GetInputBox ((NW_FBox_Epoc32InputSkin_t *)iOocInputSkin);
          NW_HED_DocumentRoot_t* docRoot = NW_FBox_FormBoxUtils_GetDocRoot(NW_LMgr_BoxOf(inputBox));
          (void) docRoot->appServices->formBoxCallbacks.InputInvalid(iParent, 0);
          response = EFalse;
          }
      delete buf;
      buf = NULL;
      return response;
      }

    TBool okToExit = EFalse;

    TInt cursorValue( iEditor->CursorPos() );

    // Mobile Browser keeps track of user input if there are any static characters within the format.
    // So, retrieve input values that Avkon knows about if there are NO static characters in the format
    // string.
    if ( iHandler && !iEditor->IsSecret() && !iHandler->NumberOfStatic() )
      {
      HBufC* text = iEditor->GetTextInHBufL();
      if ( text )     // copy finished text to iDynamic in iHandler structure
        {
        iHandler->SetTextL( *text, ETrue );
        delete text;
        }
      }
    if (iHandler && iHandler->NumberOfStatic() > 0)     // accounts for static characters
      {
      cursorValue = iHandler->PreviousNonStatic( iEditor->CursorPos() );
      if (cursorValue == KNotFound)  // possible return value from PreviousNonStatic()
        {
        cursorValue = 0;
        }
      }
    else
      {
      if (cursorValue > 0)
         {
         --cursorValue;
         }
      }

    if (iEditCommitFlag)
      {
      if (iHandler && iHandler->NumberOfStatic() > 0)
        {
        iEditCommitFlag  = EFalse;
        }
      else
        {
        HBufC* buf = iEditor->ReadableTextLC();
        // if the buf des length is equal to the document length , then no more chars are expected to commit
        if (buf->Des().Length()  >= iEditor->Text()->DocumentLength() )
          {
          iEditCommitFlag  = EFalse;
          }
       else        if (buf->Des().Length() >= iEditor->MaxLength() ) // if chars are entered beyond the maxlen no more chars are expected to commit
          {
          iEditCommitFlag = EFalse;
          }
        else  // check if the last entered data is numeric
          {
          TPtrC   lastInputCharPtr = (iEditor->Text()->Read(cursorValue, 1));
          const TUint16* lastInputChar = lastInputCharPtr.Ptr();
          if (NW_Str_Isdigit(*lastInputChar))
            {
            iEditCommitFlag  = EFalse;
            }
          }
        CleanupStack::PopAndDestroy(); // buf
        }
      }

    // commit any outstanding input characters to buffer, before checking if "okay" to exit.
    if (iEditCommitFlag )
        {
        InputCapabilities().FepAwareTextEditor()->CommitFepInlineEditL(*iEikonEnv);
        }

    TInt textLength( iEditor->ReadableTextLC()->Des().Length());
    CleanupStack::PopAndDestroy();
    //if (iHandler && iHandler->NumberOfStatic() > 0)   // if static characters found?
    //    {
    //    textLength = iHandler->MinLength() - iHandler->NumberOfStatic();
    //    }

    TInt minLength = 0;

    minLength = ( iHandler ) ? (iHandler->MinLength()) : 0;
    if ( !iEmptyOk )
        {
        // When EMPTYOK is false, input is required even if the format
        // mask would otherwise not require it.
        // [WML Spec section 11.6.3]
        if ( !minLength )
            {
            ++minLength;    // can't be empty
            }
        }

    if   ( ( iHandler && // if no format specified, pass through, and allow okToExit=ETrue.
           (  (    (iHandler->NumberOfStatic() > 0) ?   // otherwise, if one secified, check the string lengths for validity.
                    iHandler->FirstEmpty() : textLength
              )
           ) >= minLength
         )
         || textLength >= minLength
       )  // is it long enough?
      {
      if (iHandler)
        {
        iEditor->MarkInvalidCharsL();
        }
      if ( !iHandler || iHandler->ValidateTextL() )
        {
          iConfirmed = ETrue;
          okToExit = ETrue;
        }
      }
    else
      {
        NW_FBox_InputBox_t* inputBox = NW_FBox_InputSkin_GetInputBox ((NW_FBox_Epoc32InputSkin_t *)iOocInputSkin);
        NW_HED_DocumentRoot_t* docRoot = NW_FBox_FormBoxUtils_GetDocRoot(NW_LMgr_BoxOf(inputBox));
        TInt reportable = 1;
        if (textLength > 0)
           {
            reportable = minLength;
           }
        else
           {
           if (iEmptyOk)
             {
             reportable = textLength;
             }
             else
             {
             reportable = 1;
             }
           }
        if (iHandler && iHandler->NumberOfStatic() > 0)
             {
             reportable -= iHandler->NumberOfStatic();
             }
        if (reportable == 0 && iEmptyOk)
            {
            okToExit = ETrue;
            }
        else if (reportable > 0)
              {
              (void) docRoot->appServices->formBoxCallbacks.InputInvalid(iParent, reportable);
              }
      }

    return okToExit;
    }

// ---------------------------------------------------------
// CEpoc32InputBox::ReadableText
// ---------------------------------------------------------
//
HBufC* CEpoc32InputBox::ReadableText()
    {
    HBufC* buf = NULL;
  TInt err;
    TRAP( err, buf = ReadableTextL() );
    return buf;
    }

// ---------------------------------------------------------
// CEpoc32InputBox::ReadableTextL
// ---------------------------------------------------------
//
HBufC* CEpoc32InputBox::ReadableTextL()
    {
    HBufC* buf = NULL;
    if (iEditor)
        {
        buf = iEditor->ReadableTextL();
        }
    else if (iContainer)
        {
        buf = ReadableSecretTextL();
        }
    return buf;
    }

// ---------------------------------------------------------
// CEpoc32InputBox::SizeChanged
// ---------------------------------------------------------
//
void CEpoc32InputBox::SizeChanged()
    {
    TRect rect( Rect() );
    CCoeControl* focusedControl = iEditor;
    if (iContainer)
        {
        focusedControl = iContainer;
        }
    // update editor's size
        focusedControl->SetRect( rect );
        }

// ---------------------------------------------------------
// CEpoc32InputBox::PredictiveTextOnOffGSNotifiaction
// ---------------------------------------------------------
//
TInt CEpoc32InputBox::PredictiveTextOnOffGSNotifiaction( TAny* /*aObj*/ )
    {
    return KErrNone;
    }

// ---------------------------------------------------------
// CEpoc32InputBox::SetRectAndWrap
// ---------------------------------------------------------
//
void CEpoc32InputBox::SetRectAndWrap(const TRect& aRect, TInt aWrapWidth)
    {
    SetRect( aRect );
    if ( iEditor )
        {
        // set new wrap width
        iWrapWidth = aWrapWidth;
        iEditor->TextLayout()->SetWrapWidth( aWrapWidth );
        // disable scrolling
        iEditor->AddFlagToUserFlags (CEikEdwin::ENoHorizScrolling);
        // reformat input
        iEditor->TextView()->FormatTextL();
        }
    }

// ---------------------------------------------------------
// CEpoc32InputBox::InputCapabilities
// ---------------------------------------------------------
//
TCoeInputCapabilities CEpoc32InputBox::InputCapabilities() const
    {
    if (iContainer)
        {
        return iContainer->InputCapabilities();
        }
    else
        {
        if (iPenEnabled)
            {
            return TCoeInputCapabilities(iEditor->InputCapabilities().Capabilities(),
                    (MCoeFepAwareTextEditor*) this,
                    (MCoeCaptionRetrieverForFep*) this);
            }
        else
            {
            return iEditor->InputCapabilities();
            }
        }
    }

// ---------------------------------------------------------
// CEpoc32InputBox::HandleCompletionOfTransactionL()
// ---------------------------------------------------------
//
void CEpoc32InputBox::HandleCompletionOfTransactionL()
{
  iEditCommitFlag = EFalse;
}

// ---------------------------------------------------------
// CEpoc32InputBox::HandleStartOfTransactionL()
// ---------------------------------------------------------
//
void CEpoc32InputBox::HandleStartOfTransactionL()
{
  iEditCommitFlag = ETrue;
}

// ---------------------------------------------------------
// CEpoc32InputBox::MCoeFepObserver_Reserved_1()
// ---------------------------------------------------------
//
void CEpoc32InputBox::MCoeFepObserver_Reserved_1()
{}

// ---------------------------------------------------------
// CEpoc32InputBox::MCoeFepObserver_Reserved_2()
// ---------------------------------------------------------
//
void CEpoc32InputBox::MCoeFepObserver_Reserved_2()
{}

// ---------------------------------------------------------
// CEpoc32InputBox::FocusChanged
// ---------------------------------------------------------
//
void CEpoc32InputBox::FocusChanged(TDrawNow aDrawNow)
    {
    if (iContainer)
        {
        iContainer->SetFocus(IsFocused(), aDrawNow);
        }
    else
        {
        iEditor->SetFocus(IsFocused(), aDrawNow);
        }
    }

// ---------------------------------------------------------
// CEpoc32InputBox::HandleChangeInFocus
// ---------------------------------------------------------
//
void CEpoc32InputBox::HandleChangeInFocus()
  {
    if (iContainer && IsFocused())
      {
      // Rect() would not give us coordinates for password box - better to save
      // those coordinates in iRect during the constructor phase and reference them here.
      SetRect(iRect);   // forces a redraw of password box and of window
      }
  }

// ---------------------------------------------------------
// CEpoc32InputBox::HandleDestructionOfFocusedItem
// ---------------------------------------------------------
//
void CEpoc32InputBox::HandleDestructionOfFocusedItem()
  {
  }


// Decides whether to exit the control.
TBool CEpoc32InputBox::IsExitRequest(TUint aKeyCode)
    {
    TBool return_value = EFalse;

    switch (aKeyCode)
        {
        case EKeyDevice3:
            {
            return_value = ETrue;
            break;
            }

        case EKeyUpArrow:
        case EKeyDownArrow:
            {
            const NW_Object_t* object = NW_FBox_InputSkin_GetInputBox( (NW_FBox_Epoc32InputSkin_t *)iOocInputSkin );
            if (NW_Object_IsClass(object, &NW_FBox_TextAreaBox_Class))
                {
                // for TextArea, only exit if cursor is on top row moving up, or bottom row moving down.
                return_value = EFalse;
                TPoint cursorPos, startCursorPos, endCursorPos;

                iEditor->TextView()->DocPosToXyPosL( iLastCurPos, cursorPos );
                iEditor->TextView()->DocPosToXyPosL( 0, startCursorPos);
                iEditor->TextView()->DocPosToXyPosL( iEditor->Text()->DocumentLength(), endCursorPos);

                if (    cursorPos.iY == startCursorPos.iY        // At beginning of the range? And key is...
                     && aKeyCode == EKeyUpArrow )                // North
                    return_value = ETrue;                        // If so, allow the exit

                if (    cursorPos.iY == endCursorPos.iY          // At end of the range? And key is...
                     && aKeyCode == EKeyDownArrow )              // South
                    return_value = ETrue;                        // If so, allow the exit

                }
            else
                {
                // for InputBox and Password
                return_value = ETrue;
                }

            break;
            }

// All of the diagonal KeyEvents are allowed to flow through the "default" case...
//
//      case EKeyRightUpArrow:        // Northeast
//      case EStdKeyDevice11:         //   : Additional KeyEvent to support the .SIS wedge
//      case EKeyRightDownArrow:      // Southeast
//      case EStdKeyDevice12:         //   : Additional KeyEvent to support the .SIS wedge
//      case EKeyLeftDownArrow:       // Southwest
//      case EStdKeyDevice13:         //   : Additional KeyEvent to support the .SIS wedge
//      case EKeyLeftUpArrow:         // Northwest
//      case EStdKeyDevice10:         //   : Additional KeyEvent to support the .SIS wedge
        default:
            break;

        }
    return return_value;
}


// ---------------------------------------------------------
// CEpoc32InputBox::GetText
// ---------------------------------------------------------
//
void CEpoc32InputBox::GetSecretText(TDes& aText)
    {
    iContainer->GetText( aText );
    }


// ---------------------------------------------------------
// CEpoc32InputBox::ReadableTextL
// ---------------------------------------------------------
//
HBufC* CEpoc32InputBox::ReadableSecretTextL()
    {
    TBuf<KBufLength> text;
    GetSecretText( text );
    HBufC* buf = NULL;
    if (text.Length())
        {
        buf = text.AllocL();
        }
    else
        {
        buf = HBufC::NewL(1);
        buf->Des().FillZ(1);
        buf->Des().SetLength(0);
        }
    return buf;
    }

// from MCoeFepAwareTextEditor_Extension1
// -----------------------------------------------------------------------------
// CEpoc32InputBox::SetStateTransferingOwnershipL
// -----------------------------------------------------------------------------
void CEpoc32InputBox::SetStateTransferingOwnershipL(CState* aState, TUid /*aTypeSafetyUid*/)
{
    delete iState;
    iState = aState;
    if (iEditor)
        {
        iEditor->SetVkbState(STATIC_CAST(CAknEdwinState*, iState));
        }
}

// -----------------------------------------------------------------------------
// CEpoc32InputBox::State
// -----------------------------------------------------------------------------
MCoeFepAwareTextEditor_Extension1::CState* CEpoc32InputBox::State(TUid /*aTypeSafetyUid*/)
{
    if (!iState)
      {
      CAknEdwinState* state = new CAknEdwinState();
      state->SetObjectProvider(this);
      iState = state;
      if (iEditor)
        {
        iEditor->SetVkbState(STATIC_CAST(CAknEdwinState*, iState));
        }
      }
    return iState;
}

// -----------------------------------------------------------------------------
//CEpoc32InputBox::Activate VKB
// -----------------------------------------------------------------------------
void CEpoc32InputBox::ActivateVKB()
{
  if (iPenEnabled)
    {
    CAknEdwinState* state = NULL;
    MCoeFepAwareTextEditor_Extension1* fepAwareTextEditor = InputCapabilities().FepAwareTextEditor()->Extension1();

    if ( fepAwareTextEditor )
      {
      state = STATIC_CAST(CAknEdwinState*, fepAwareTextEditor->State(KNullUid));
      }

    if ( iEditor )
        {
            iEditor->SetEditorCurentCase();
            iEditor->UpdateEditingParameters();
        }

    if ( state )
      {
      TRAP_IGNORE( state->ReportAknEdStateEventL(MAknEdStateObserver::EAknActivatePenInputRequest ) );
      }
    }
}

////////////////////////////////////////////////////////////////////////////////
// from MCoeFepAwareTextEditor
//
// -----------------------------------------------------------------------------
// StartFepInlineEditL
//
//
// -----------------------------------------------------------------------------
void CEpoc32InputBox::StartFepInlineEditL(
      const TDesC& aInitialInlineText,
      TInt /*aPositionOfInsertionPointInInlineText*/,
      TBool /*aCursorVisibility*/,
      const MFormCustomDraw*,
      MFepInlineTextFormatRetriever&,
      MFepPointerEventHandlerDuringInlineEdit& /*aPointerEventHandlerDuringInlineEdit*/)
{
    CCoeEnv::Static()->ForEachFepObserverCall(FepObserverHandleStartOfTransactionL);
    ClearInlineText();
    iUncommittedOffset = aInitialInlineText.Length();
    UpdateInlineTextL(aInitialInlineText);
}
// -----------------------------------------------------------------------------
// UpdateFepInlineTextL
//
//
// -----------------------------------------------------------------------------
void CEpoc32InputBox::UpdateFepInlineTextL(const TDesC& aNewInlineText,TInt /*aPositionOfInsertionPointInInlineText*/)
{
    iUncommittedOffset = aNewInlineText.Length();
    UpdateInlineTextL(aNewInlineText);
}

// -----------------------------------------------------------------------------
// SetInlineEditingCursorVisibilityL
//
//
// -----------------------------------------------------------------------------
void CEpoc32InputBox::SetInlineEditingCursorVisibilityL(TBool /*aCursorVisibility*/)
{
}

// -----------------------------------------------------------------------------
// CancelFepInlineEdit
//
//
// -----------------------------------------------------------------------------
void CEpoc32InputBox::CancelFepInlineEdit()
{
    if (!m_cancelFepInlineEditInProgress) //prevent recursive calls on inputting enter key
    {
        m_cancelFepInlineEditInProgress  = ETrue;
        CAknEdwinState* state = static_cast<CAknEdwinState*>(State(KNullUid));
        if ( state ) {
            TRAP_IGNORE( state->ReportAknEdStateEventL(MAknEdStateObserver::EAknEdwinStateEventStateUpdate ) );
        }
        m_cancelFepInlineEditInProgress  = EFalse;
    }
}

// -----------------------------------------------------------------------------
// DocumentLengthForFep
//
//
// -----------------------------------------------------------------------------
TInt CEpoc32InputBox::DocumentLengthForFep() const
{
    return (iInlineEditText ? iInlineEditText->Length() : 0);
}


// -----------------------------------------------------------------------------
// DocumentMaximumLengthForFep
//
//
// -----------------------------------------------------------------------------
TInt CEpoc32InputBox::DocumentMaximumLengthForFep() const
{
    return KMaxTInt;
}

// -----------------------------------------------------------------------------
// SetCursorSelectionForFepL
//
//
// -----------------------------------------------------------------------------
void CEpoc32InputBox::SetCursorSelectionForFepL(const TCursorSelection& aCursorSelection)
{
    if ( iEditor )
        {
            iEditor->SetCursorPosL( aCursorSelection.iCursorPos );
            iEditor->UpdateEditingParameters( aCursorSelection.iCursorPos );
        }
}

// -----------------------------------------------------------------------------
// GetCursorSelectionForFep
//
//
// -----------------------------------------------------------------------------
void CEpoc32InputBox::GetCursorSelectionForFep(TCursorSelection& aCursorSelection) const
{
    if ( iEditor )
        {
            TInt cursorPos (iEditor->CursorPos() + iUncommittedOffset);
            aCursorSelection.SetSelection(cursorPos,cursorPos);
        }
}

// -----------------------------------------------------------------------------
// GetEditorContentForFep
//
//
// -----------------------------------------------------------------------------
void CEpoc32InputBox::GetEditorContentForFep(TDes& aEditorContent,TInt aDocumentPosition,TInt aLengthToRetrieve) const
{
    aEditorContent = KBlankDesC;
    if ( iInlineEditText && aDocumentPosition >= 0 && (aDocumentPosition + aLengthToRetrieve) <= iInlineEditText->Length() )
        {
        aEditorContent = iInlineEditText->Des().Mid(aDocumentPosition,aLengthToRetrieve);
        }
}

// -----------------------------------------------------------------------------
// GetFormatForFep
//
//
// -----------------------------------------------------------------------------
void CEpoc32InputBox::GetFormatForFep(TCharFormat& /*aFormat*/,TInt /*aDocumentPosition*/) const
{
}

// -----------------------------------------------------------------------------
// GetScreenCoordinatesForFepL
//
//
// -----------------------------------------------------------------------------
void CEpoc32InputBox::GetScreenCoordinatesForFepL(TPoint& aLeftSideOfBaseLine,TInt& aHeight,TInt& aAscent,TInt /*aDocumentPosition*/) const
{
    aLeftSideOfBaseLine = TPoint(10,10);
    aHeight = 0;
    aAscent = 0;
}

// -----------------------------------------------------------------------------
// UpdateInlineTextL
//
//
// -----------------------------------------------------------------------------
void CEpoc32InputBox::UpdateInlineTextL(const TDesC& aText)
{
    if (DocumentLengthForFep() >= DocumentMaximumLengthForFep())
        return;

    delete iInlineEditText;
    iInlineEditText = NULL;

    if (!iDocumentText)
        {
        iInlineEditText = aText.AllocL();
        }
    else
        {
        TInt curPos(iEditor->CursorPos());
        HBufC* newText = HBufC::NewL(iDocumentText->Length() + aText.Length());
        newText->Des().Append(iDocumentText->Left(curPos));
        newText->Des().Append(aText);
        newText->Des().Append(iDocumentText->Right(iDocumentText->Length() - curPos));
        iInlineEditText = newText;
        }
}

// -----------------------------------------------------------------------------
// DoCommitFepInlineEditL
//
//
// -----------------------------------------------------------------------------
void CEpoc32InputBox::DoCommitFepInlineEditL()
{
    if (iInlineEditText)
        {
        HBufC* newText = iInlineEditText->AllocL();
        delete iDocumentText;
        iDocumentText = newText;
        }

    if ( iEditor )
        {
        iEditor->SetTextL( iDocumentText );
        }
    TCursorSelection sel;
    GetCursorSelectionForFep(sel);
    SetCursorSelectionForFepL(sel);
    iUncommittedOffset = 0;
}

// -----------------------------------------------------------------------------
// Extension1
//
//
// -----------------------------------------------------------------------------
MCoeFepAwareTextEditor_Extension1* CEpoc32InputBox::Extension1(TBool& aSetToTrue)
{
    aSetToTrue=ETrue;
    return STATIC_CAST(MCoeFepAwareTextEditor_Extension1*, this);
}

// -----------------------------------------------------------------------------
// MCoeFepAwareTextEditor_Reserved_2
//
//
// -----------------------------------------------------------------------------
void CEpoc32InputBox::MCoeFepAwareTextEditor_Reserved_2()
{
}

// -----------------------------------------------------------------------------
// MCoeFepAwareTextEditor_Reserved_2
//
//
// -----------------------------------------------------------------------------
void CEpoc32InputBox::GetCaptionForFep(TDes& /*aCaption*/) const
{
}

// -----------------------------------------------------------------------------
// ClearInlineText
//
//
// -----------------------------------------------------------------------------
void CEpoc32InputBox::ClearInlineText()
{
    delete iInlineEditText;
    iInlineEditText = NULL;
}

void CEpoc32InputBox::HandlePointerEventL(const TPointerEvent &aPointerEvent)
    {
        CCoeControl::HandlePointerEventL(aPointerEvent);

        if ( iContainer && aPointerEvent.iType == TPointerEvent::EButton1Up)
            {
                ActivateVKB();
            }

    }

// -----------------------------------------------------------------------------
// HandleUpdateCursor
//
//
// -----------------------------------------------------------------------------
void CEpoc32InputBox::HandleUpdateCursor()
{
    // ReportAknEdStateEventL, for events see aknedstsobs.h
    // MAknEdStateObserver::EAknCursorPositionChanged
    if ( iEditor->CursorPos() == iPrevCurPos ) {
         return;
     }

    CAknEdwinState* state = static_cast<CAknEdwinState*>(State(KNullUid));
    if ( state ) {
        TRAP_IGNORE( state->ReportAknEdStateEventL( MAknEdStateObserver::EAknCursorPositionChanged ) );
    }
}


//  End of File

