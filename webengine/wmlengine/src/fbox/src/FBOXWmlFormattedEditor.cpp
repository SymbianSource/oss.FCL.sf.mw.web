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
// System includes
#include <calslbs.h>
#include <eikenv.h>
#include <eikedwin.h>
#include <e32std.h>
#include <PtiDefs.h>

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <txtrich.h>
#else
#include <txtrich.h>
#include <txtclipboard.h>
#endif


#include <bautils.h>
#include <webkit.rsg>
#include <PUAcodes.hrh>
#include <featmgr.h>
#include <baclipb.h>
#include <aknedsts.h>

// User includes
#include "nw_fbox_wmlformattededitor.h"
// CONSTANTS

static const TInt KWmlDefaultTextColor = 215; ///< black (plain text)
static const TInt KWmlFormattedInputErrorColor = 35; ///< TEMP : should be clarified!
const TInt KFullFormattingUpperThreshold=2000;

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CWmlFormattedEditor::CWmlFormattedEditor( TBool aIsSecret ) : CEikRichTextEditor()
        , iDefaultColour( AKN_LAF_COLOR( KWmlDefaultTextColor ) )
        , iErrorColour( AKN_LAF_COLOR( KWmlFormattedInputErrorColor ) )
        , iPosAtEnd(EFalse)
        , iEventKeyEntered(EFalse)
        , iDuplicateEvent(EFalse)
        , iVkbState(NULL)
    {
    SetAknEditorPermittedCaseModes( EAknEditorAllCaseModes );

    UpdateInputModeState(EAknEditorTextInputMode, EAknEditorAllInputModes);

    TUint flags( EAknEditorFlagDefault |
                       EAknEditorFlagUseSCTNumericCharmap |
                       EAknEditorFlagNoT9 | EAknEditorFlagMTAutoOverwrite );
    // NOTE: Use of "variant" indicator will be removed in near term - code will be changed
    // allowing simpler conditional as EAknEditorFlagLatinInputModesOnly will be allowed
    // in European and Apac builds.
    EVariantFlag variant = AknLayoutUtils::Variant();
    if (aIsSecret && variant == EApacVariant)
        {
        UpdateFlagsState( flags | EAknEditorFlagLatinInputModesOnly );
        SetAknEditorCase( EAknEditorLowerCase );
        }
    else
        {
        UpdateFlagsState( flags );
        SetAknEditorCase( EAknEditorTextCase );
        }
    SetBorder(TGulBorder::EDeepSunkenWithOutline);
    iWmlEdNavigation = EWmlEditorNavigateRight ;
    iPenEnabled = AknLayoutUtils::PenEnabled();
    }

// ---------------------------------------------------------
// CWmlFormattedEditor::InitializeL
// ---------------------------------------------------------
//
void CWmlFormattedEditor::InitializeL
( CWmlFormatHandler* aHandler, TBool aSecret, TWmlMode aMode )
    {
    iHandler = aHandler;
    iSecret = aSecret;
    if ( iSecret && !iHandler ) // T9 must be disabled in password editor
        {
        iPassword = HBufC::NewL( 1 );
        }

    iMode = aMode;
    iPrevMode = EWmlModeNone;
    /*lint -e{40} Undeclared identifier 'KFeatureIdJapanese'*/
     if (!FeatureManager::FeatureSupported(KFeatureIdJapanese))
        {
        iMode = EWmlModeNone;
        }
    }


// ---------------------------------------------------------
// CWmlFormattedEditor::SetDefaultColour
// ---------------------------------------------------------
//
void CWmlFormattedEditor::SetDefaultColour( TRgb aDefaultColour )
    {
    iDefaultColour = aDefaultColour;
    }

// ---------------------------------------------------------
// CWmlFormattedEditor::SetErrorColour
// ---------------------------------------------------------
//
void CWmlFormattedEditor::SetErrorColour( TRgb aErrorColour )
    {
    iErrorColour = aErrorColour;
    }

// ---------------------------------------------------------
// CWmlFormattedEditor::EnableT9
// ---------------------------------------------------------
//
void CWmlFormattedEditor::EnableT9( TBool aEnable, TBool aIsSecret )
    {
    iT9 = aEnable;

    TUint flags = iT9 ? EAknEditorFlagDefault |
            EAknEditorFlagUseSCTNumericCharmap | EAknEditorFlagMTAutoOverwrite
            : EAknEditorFlagDefault |
            EAknEditorFlagUseSCTNumericCharmap |
            EAknEditorFlagNoT9 | EAknEditorFlagMTAutoOverwrite;
    EVariantFlag variant = AknLayoutUtils::Variant();
    if (aIsSecret && variant == EApacVariant)
        {
        UpdateFlagsState( flags | EAknEditorFlagLatinInputModesOnly );
        }
    else
        {
        UpdateFlagsState( flags );
        }
    }

// ---------------------------------------------------------
// CWmlFormattedEditor::IsSecret
// ---------------------------------------------------------
//
TBool CWmlFormattedEditor::IsSecret() const
    {
    return iSecret;
    }

// ---------------------------------------------------------
// CWmlFormattedEditor::ReadableTextL
// ---------------------------------------------------------
//
HBufC* CWmlFormattedEditor::ReadableTextL() const
    {
    HBufC* result = ReadableTextLC();

    CleanupStack::Pop(); // result

    return result;
    }

// ---------------------------------------------------------
// CWmlFormattedEditor::ReadableTextLC
// ---------------------------------------------------------
//
HBufC* CWmlFormattedEditor::ReadableTextLC() const
    {
    HBufC* result = NULL;
    if ( iHandler ) // formatted?
        {
        result = iHandler->TextLC(); // zeroterminated
        }
    else
        {
        if ( iPassword ) // secret?
            {
            /*lint --e{1058} nitializing a non-const reference */
            // make a zeroterminated copy
            result = HBufC::NewLC( iPassword->Length() + 1);
            result->Des() = *iPassword;
            result->Des().ZeroTerminate();
            }
        else
            {
            // make a zeroterminated copy
            result = HBufC::NewLC( TextLength() + 1 );
            TPtr ptr = result->Des();
            if (TextLength() > 0)
                {
                GetText( ptr );
                }

            result->Des().ZeroTerminate();
            }
        }
    return result;
    }

// ---------------------------------------------------------
// CWmlFormattedEditor::MarkInvalidCharsL
// ---------------------------------------------------------
//
void CWmlFormattedEditor::MarkInvalidCharsL()
    {
    if ( iHandler )
        {
        // make character format
        TCharFormat charFormat;
        charFormat.iFontPresentation.iTextColor = iDefaultColour;
        TCharFormatMask charFormatMask;
        charFormatMask.SetAttrib( EAttColor );

        // unmark all, by colouring to default
        RichText()->ApplyCharFormatL
        ( charFormat, charFormatMask, 0, iText->DocumentLength() );

        charFormat.iFontPresentation.iTextColor = iErrorColour;
        const CArrayFixFlat<TInt>* errors = iHandler->ValidateTextLC();
        TInt endIndex = errors->Count() - 1;
        for ( ; endIndex > -1; ) // loop through the errors
            {
            // find a block of errors
            TInt startIndex = endIndex - 1;
            TInt endPos = errors->At( endIndex );
            for ( ;
                  startIndex > -1 &&
                  endPos - errors->At( startIndex ) ==
                  endIndex - startIndex;
                  startIndex-- )
                { // find the lower border of error block
                }

            TInt blockLength = endIndex - startIndex;
            RichText()->ApplyCharFormatL   // colour that block
            ( charFormat,
              charFormatMask,
              errors->At( startIndex + 1 ),
              blockLength );

            endIndex = startIndex;
            }
        CleanupStack::PopAndDestroy(); // errors
        }
	UpdateColour(iDefaultColour);
    NotifyNewFormatL();  // redraws editor
    }

// ---------------------------------------------------------
// CWmlFormattedEditor::UpdateColour(TRgb aDefaultColour)
// ---------------------------------------------------------
//
void CWmlFormattedEditor::UpdateColour(TRgb aDefaultColour)
    {
        // make character format
        TCharFormat charFormat;
        charFormat.iFontPresentation.iTextColor = aDefaultColour;
        TCharFormatMask charFormatMask;
        charFormatMask.SetAttrib( EAttColor );
    	RichText()->ApplyCharFormatL(charFormat, charFormatMask, CursorPos(), 0);
        }

// ---------------------------------------------------------
// CWmlFormattedEditor::FreePassword
// ---------------------------------------------------------
//
void CWmlFormattedEditor::FreePassword()
    {
    delete iPassword;
    iPassword = NULL;
    }

// ---------------------------------------------------------
// CWmlFormattedEditor<Editor>::T9Enabled
// ---------------------------------------------------------
//
TBool CWmlFormattedEditor::T9Enabled() const
    {
	return iT9;
    }

// ---------------------------------------------------------
// CWmlFormattedEditor<Editor>::SetiCursorPos
// Note: when switching modes to and from T9 this will
// prevent duplication of characters when switch from T9 to
// multi-tap input.
// ---------------------------------------------------------
//
void CWmlFormattedEditor::SetiCursorPos()
    {
    iCursorPos = CursorPos();
    }

// ---------------------------------------------------------
// CWmlFormattedEditor<Editor>::ResetFlagEventKeyEntered
// Resets flag used to indicate input via EVENTKEY event handling.
// ---------------------------------------------------------
//
void CWmlFormattedEditor::ResetFlagEventKeyEntered()
    {
    iEventKeyEntered = EFalse;
    }

// ---------------------------------------------------------
// CWmlFormattedEditor<Editor>::IsEventKeyEntered
// Returns iEventKeyEntered, a flag used to indicate when
// key data received via EVENTKEY event handling.
// ---------------------------------------------------------
//
TBool CWmlFormattedEditor::IsEventKeyEntered()
    {
    return iEventKeyEntered;
    }

// ---------------------------------------------------------
// CWmlFormattedEditor::SpecialHandlingPasswordL
// ---------------------------------------------------------
//
void CWmlFormattedEditor::SpecialHandlingPasswordL()
    {
    TInt cursorPos( CursorPos() );
    TInt length( CursorPos() - iCursorPos +1);
    if (length <= 0)
        {
        length = 1;
        }
    TInt firstEmpty( TextLength() );
    if (iHandler && iHandler->NumberOfStatic() > 0)
        {
        firstEmpty = iHandler->FirstEmpty();
        }
    if ( (iFirstEmpty <= firstEmpty ) )
        {   // insert character into storage place
        if ( IsEventKeyEntered() )
            {
            return; // already added by iEditor->OfferKey
            }
        HBufC* newText = HBufC::NewLC( length );
        TPtr ptr = newText->Des();

        Text()->Extract( ptr, iCursorPos, length );

        if ( iHandler )
            {
            iHandler->InsertL( iCursorPos, *newText );
            iWmlEdNavigation = EWmlEditorNavigateRight;
            }
        else
            {
            if ( iPassword )
                {
                TInt newLength( iPassword->Length() + length );
                if ( newLength > iPassword->Des().MaxLength() )
                    {
                    iPassword = iPassword->ReAllocL( newLength + 1);
                    }
                iPassword->Des().Insert( iCursorPos, *newText );
                iWmlEdNavigation = EWmlEditorNavigateRight;
                }
            }
        CleanupStack::PopAndDestroy();  // newText
        }
    else
        {  // remove character from storage place
        if ( iHandler )
            {
            iHandler->Delete( cursorPos, 1 );  // cursorPos already is actual cursor - 1
            SetCursorPosL( 0 );
            iText->Reset();
            CEikRichTextEditor::SetTextL( iHandler->TextLC() );
            CleanupStack::PopAndDestroy(); // for return value of iHandler->TextLC
            SetCursorPosL( cursorPos );
            }
        else
            {
            if ( iPassword && cursorPos >= 0 )
                {
                iPassword->Des().Delete( cursorPos, 1 );
                }
            }
        }
    }


// ---------------------------------------------------------
// CWmlFormattedEditor::SpecialStaticHandlingL
// ---------------------------------------------------------
//
void CWmlFormattedEditor::SpecialStaticHandlingL()
    {
	TInt  pos = CursorPos();
    TBool adjustPosFlag = EFalse; // if static char in proximity, adjust cursor to get input character.

    // There is no special handling if a DELETE (clear/backspace) has been done.
    // We now distinguish between a "NavigateLeft" and a "NavigateDelete".
    if ( iWmlEdNavigation == EWmlEditorNavigateDelete )
        {
        // reset
        iWmlEdNavigation = EWmlEditorNavigateRight;
        return;
        }

    // This is compensation code for Avkon taking control of cursor movement when looping from last
    // char position to first position and Avkon does not send browser input an indication that this is
    // happening.
    if (iCursorPos > pos)
        {
        iCursorPos = pos;
        if (pos > 0)
            {
            iCursorPos = pos - 1;
            }
        }

    // If an input box with static text, prep the text to use
    // asking if we are doing an insertion
    if ( !IsEventKeyEntered() && iHandler && iHandler->NumberOfStatic())
        {
        HBufC* text = NULL;
        TInt length = pos - iCursorPos;
        if (length > 0)
            {
            text = HBufC::NewLC( length );
            }

        if ( iWmlEdNavigation == EWmlEditorNavigateLeft )
            {
            iCursorPos = pos;     // adjust now
            if (iCursorPos > 0)
                {
                --iCursorPos;
                }
            iWmlEdNavigation = EWmlEditorNavigateRight;
            }

        // When a static char is reached during input, the iCursorPos and CursorPos() values indicate the
        // correct NEXT location, but edwin seems to ignore where it SHOULD be and the cursor remains on the
        // static character location.  This code is to compensate in part for this with respect to inserting
        // the new character into the saved input area.  There is still an issue however
        // with edwin / commitinput() re-setting the cursor position to an improper location - that is - it's
        // not ahead of the newly inserted character, but behind it.
        if ( (pos - 1 >= 0) && iHandler->IsPosStatic( pos - 1 ))
            {
            --pos;
            adjustPosFlag = ETrue;
            }

        if ( text )  // with whatever info was obtained, store it into iHandler
            {
            TPtr ptr = text->Des();
            Text()->Extract( ptr, iCursorPos, length );
            if (adjustPosFlag)
                {
                iCursorPos = pos;
                }
            iHandler->InsertL( iCursorPos, *text );
            ++ iNumberOfCharsInStaticInputBox;
            CleanupStack::PopAndDestroy();		// text
            }

        iCursorPos = pos;
        }
    }

// ---------------------------------------------------------
// CWmlFormattedEditor::SpecialCharHandlingL
// ---------------------------------------------------------
//
void CWmlFormattedEditor::SpecialCharHandlingL()
    {
	if (IsSecret() || ( iHandler && iHandler->NumberOfStatic()) )
        {
        if ( IsSecret() )
            {
            SpecialHandlingPasswordL();
            }
        // if secret (that is, type password) -and- static text in format do additional handling
        // otherwise if static text in format only (input field is -not- a password type)
        // do the additional handling
        if (iHandler && iHandler->NumberOfStatic())
            {
            SpecialStaticHandlingL();
            }
        UpdateStateL( iWmlEdNavigation, ETrue, ETrue );
        }
    else
        {
        if (iHandler)
            {
            HBufC* text;
            text = GetTextInHBufL();
            if ( text )
                {  // add the new char entry to locally held storage.  If there was a shift in input mode
                   // from char to numeric, it may have been possible for a char to slip in before the new
                   // input mode may have taken effect in Avkon - so for right now, remove it.
                TInt pos = CursorPos();
                TWmlFormatChar format = iHandler->GetFormat( pos );
                if ( TWmlFormatChar::EWmlNumericCase == format.ForcedCase() )
                    {
                    // really want to delete the inadvertantly added char and add the number for "scancode"
                    text->Des().Delete(text->Length(), 1);
                    }
                CleanupStack::PushL( text );
                //iHandler->SetTextL( *text, EFalse );  // removes static stuff and stores chars in iHandler.iDynamic
                // For VKB we want all chars to show so we shut off validation.
                //   Validation will still happen when you try to exit the input box.
                iHandler->SetTextL( *text, iPenEnabled );  // removes static stuff and stores chars in iHandler.iDynamic
                CleanupStack::PopAndDestroy();		// text
                }
            else
                {
                iHandler->SetTextL( KNullDesC(), ETrue );
                }
            }
        }
    TInt tempPos = CursorPos();
    UpdateEditingParameters( tempPos );

    if ( IsEventKeyEntered() && !iInputFormatProcess )
        {
        ResetFlagEventKeyEntered();
        }

    }

// ---------------------------------------------------------
// CWmlFormattedEditor::HandleEdwinEventL
// ---------------------------------------------------------
//
void CWmlFormattedEditor::HandleEdwinEventL
( CEikEdwin* /*aEdwin*/, MEikEdwinObserver::TEdwinEvent aEventType )
    {
    switch ( aEventType )
        {
        case MEikEdwinObserver::EEventNavigation:  // navigation
          // Event navigation cannot be done before the text update .
            {
            // keep cursor away from invalid positions
            //SetCursorPosL(iCursorPos);
            //CEikEdwin::DrawContents();
            //UpdateStateL( EWmlEditorNavigateRight );
            break;
            }
        case MEikEdwinObserver::EEventTextUpdate:  // updating
            {
            if (iDuplicateEvent)          // ignore extraneous TextUpdate event - this flag is reset when next "EventKey-X" received in OfferKeyEvent
                {
                return;
                }
            iDuplicateEvent = ETrue;      // set trap to ignore extraneous TextUpdate events
            SpecialCharHandlingL();
            break;
            }
        default:
            break;
        }
    }


// ---------------------------------------------------------
// CWmlFormattedEditor::InputCapabilities
// ---------------------------------------------------------
//
TCoeInputCapabilities CWmlFormattedEditor::InputCapabilities() const
    {
    return CEikRichTextEditor::IsFocused() ?
           CEikRichTextEditor::InputCapabilities() :
           TCoeInputCapabilities( TCoeInputCapabilities::ENone );
    }

// ---------------------------------------------------------
// CWmlFormattedEditor::OfferKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CWmlFormattedEditor::OfferKeyEventL
( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    UpdateColour(iDefaultColour);

	TInt newCursorPos = 0;           // used below in left/right arrow key handling
    TWmlEdNavigation naviType = EWmlEditorNavigateRight;
    TKeyResponse response = EKeyWasNotConsumed;
    iShift =     // filters out the popping up of shift menu
        aKeyEvent.iCode && aKeyEvent.iModifiers & EModifierShift;

    iDuplicateEvent = EFalse;     // reset "trap" for extraneous multi-tap TextUpdate edwin events as we are
                                  // now working on another input (makes no difference if control/numeric/multi-tap).

    if (aType != EEventKey)
        {
        return response;
        }

    TInt pos( CursorPos() );
    switch ( aKeyEvent.iCode )
        {
        // "short key press" of clear key for deleting numbers - in mixed formats - but be aware
        // deletion of chars (and most nums, depending upon format) is now based upon receiving "EventKeyDown" event (see further down).
        // If a default format char/number, deletion of numbers is based upon KeyEvent.  Also, special handling
        // For cursorPos=0, this case is invoked.
        case EKeyBackspace:
            {
             // When pointing to a static character to delete, adjust the cursor position to preserve the
             // static character.
            TBool doBackspace = ETrue;
            if (iHandler && iHandler->NumberOfStatic())
                {
                doBackspace = HandleClearKeyL( pos );
                }
            if (doBackspace)
                {
                response = CEikRichTextEditor::OfferKeyEventL( aKeyEvent, aType );
                SpecialCharHandlingL();
                }
            }
        break;

        case EKeyLeftArrow:
            {
            response = CEikRichTextEditor::OfferKeyEventL( aKeyEvent, aType );
            newCursorPos = CursorPos();
            naviType = EWmlEditorNavigateLeft;
            // "iShift" set to 1 indicates a COPY (ie, CCPU) operation is in progress.  If that is the case,
            // do not adjust cursor position.
            if (!iShift && iHandler && iHandler->NumberOfStatic() > 0 && iHandler->IsPosStatic( newCursorPos) )
                {
                TInt adjustedCursor = iHandler->PreviousNonStatic( newCursorPos );
                if (adjustedCursor >= 0)
                    {
                    SetCursorPosL( adjustedCursor );
                    }
                //else - positioned next to static chars from pos 0 - change cursor pos to end of input
                else
                    {
                    adjustedCursor = iHandler->FirstEmpty();
                    SetCursorPosL( adjustedCursor );
                    }
                iCursorPos = adjustedCursor;
                }
            }
        break;

        case EKeyDownArrow:
        case EKeyUpArrow:
            {
            response = CEikRichTextEditor::OfferKeyEventL( aKeyEvent, aType );
            }
        break;

        case EKeyRightArrow:
            {
            if (iHandler && iHandler->NumberOfStatic() > 0 && iHandler->FirstEmpty() == pos)
                {
                SetCursorPosL( 0 );
                return EKeyWasConsumed;
                }
            response = CEikRichTextEditor::OfferKeyEventL( aKeyEvent, aType );
            newCursorPos = CursorPos();
            // "iShift" set to 1 indicates a COPY (ie, CCPU) operation is in progress.  If that is the case,
            // do not adjust cursor position.
            if (!iShift && iHandler && iHandler->NumberOfStatic() > 0 && iHandler->IsPosStatic( newCursorPos))
                {
                newCursorPos = iHandler->NextNonStatic( newCursorPos );
                SetCursorPosL( newCursorPos );
                }
            }
            iCursorPos = newCursorPos;
            naviType = EWmlEditorNavigateRight;
        break;


        default:
            {
            iEventKeyEntered = ETrue;
            iInputFormatProcess = ETrue;
            TInt maxLength = iHandler ? iHandler->MaxLength() : KInfiniteLength;
            if ( maxLength == KInfiniteLength ||
                  maxLength > MaxLength() )  // is iHandler and iEditor maxlength different?
                {
                maxLength = MaxLength();
                }
            // if the input box is NOT read only and the character in question is NOT a "non character",
            // then add the character to the data structure (iDynamic) maintained by class WmlFormatHandler.
            if ( !(aKeyEvent.iCode & ENonCharacterKeyBase) && !IsReadOnly() )
                {
                if ( iHandler && (iHandler->NumberOfStatic() || iSecret) )
                    {
                    if (iHandler->FirstEmpty() < maxLength )
                        {
                        if ( (TInt)aKeyEvent.iCode == EPtiKey0 && aKeyEvent.iRepeats == 1)
                            {
                            iHandler->Delete( pos, 1);
                            }
                        iHandler->InsertL( pos, aKeyEvent.iCode );
                        if(iHandler->MaxLength() == iHandler->TextLC(ETrue)->Length() && pos < iHandler->MaxLength())
                            {
                            SetCursorPosL(pos+1);// set it to next cursor pos but not > max length
                            }
                        CleanupStack::PopAndDestroy();//iHandler->TextLC
                        naviType = EWmlEditorNavigateRight;
                        }
                    }
                else
                    {
                    if ( iPassword && pos < MaxLength() )
                        {
                        TInt length = iPassword->Length();
                        if ( length == iPassword->Des().MaxLength() )
                            {
                            iPassword =
                                iPassword->ReAllocL( length + 1 );
                            }

                        TBuf<1> currentChar;
                        currentChar.Append( aKeyEvent.iCode );
                        // long press on KeyPad0, results in calling HandleEdwinEventL and OfferKeyEvent
                        // so replace the character
                        if ( (TInt)aKeyEvent.iCode == EPtiKey0 && aKeyEvent.iRepeats == 1)
                            {
                            iPassword->Des().Replace( pos, 1, currentChar );
                            }
                        else
                            {
                            iPassword->Des().Insert( pos, currentChar );
                            }
                        }
                    }
                }
                CEikRichTextEditor::OfferKeyEventL( aKeyEvent, aType );
                SpecialCharHandlingL();
                response = EKeyWasConsumed;

                if (IsEventKeyEntered())
                {
                ResetFlagEventKeyEntered();
                }
                iInputFormatProcess = EFalse;
                break;
            }
        }

    if ( !iShift)
      {
      CEikEdwin::DrawContents();
      }

    iWmlEdNavigation = naviType;
    return response;
    }

// ---------------------------------------------------------
// CWmlFormattedEditor::HandleClearKeyL
// ---------------------------------------------------------
//
TBool CWmlFormattedEditor::HandleClearKeyL( const TInt aCursorPos )
    {
     TInt pos = aCursorPos;
     TBool clearKeyWorked = ETrue;
     if (!iHandler || !iHandler->NumberOfStatic())
          {
          return clearKeyWorked; // if no iHandler, no special handling necessary
          }

     if ( pos <= 0 ) // if there are no user entered chars left, return
          {
          return !clearKeyWorked;
          }

     if ( (pos > 0 || Selection().Length() > 0) && !IsReadOnly() )
     {
          TInt startDeletingFrom;
          TInt charsToDelete;

          // if there's a selection - delete it
          if ( Selection().Length() )
              {
              startDeletingFrom = Selection().LowerPos();
              charsToDelete = Selection().Length();
              }
          else
              {
              startDeletingFrom = pos - 1;
              charsToDelete = 1;
              }
          iHandler->Delete( startDeletingFrom, charsToDelete );
          }
      else
          {
          if ( iPassword && pos > 0 )
              {
              iPassword->Des().Delete( pos - 1, 1 );
              }
          }
      iWmlEdNavigation = EWmlEditorNavigateDelete;
      return clearKeyWorked;

    }

// ---------------------------------------------------------
// CWmlFormattedEditor::HandleTextPastedL
// ---------------------------------------------------------
//
void CWmlFormattedEditor::HandleTextPastedL( TInt aStartPos, TInt& aLength )
    {
    // aLength == length of selected text, not what is in copy buffer
    // aStartPos == starting position in input box (0 based), and includes static text
    if (aLength < 1 || aStartPos < 0)
        {
        return;
        }
    HBufC* copiedText = HBufC::NewLC( aLength );
    TPtr ptr = copiedText->Des();
     // get what is in the clipboard and should be pasted
    Text()->Extract( ptr, aStartPos, aLength );

    // if there is a format, there is a iHandler, so get the stored text
    // from iHandler.iDynamic and manipulate it.  Importantly, with this method, we
    // don't care if there are static characters in the way when we do
    // the pasting.
    if ( iHandler )
        {
        TInt adjustedSelectedLength = 0;
        TInt numberStatics = 0;
        if (iSelection.Length())
            {
            TInt temp = iSelection.LowerPos();
            // if any static chars in selection, account for them in the length to delete
            // from iHandler.iDynamic
            for (;temp < iSelection.HigherPos(); ++temp)
            {
                if (iHandler->IsPosStatic( temp ))
                {
                    ++numberStatics;
                }
            }
            adjustedSelectedLength = iSelection.Length() - numberStatics;
            iHandler->Delete( iSelection.LowerPos(), adjustedSelectedLength );
            // if clipboard is giving us more than selected text will allow, accounting for static chars,
            // adjust length
            if ( aLength > adjustedSelectedLength )
                {
                ptr.SetLength( adjustedSelectedLength );  // set lengh of "copiedText" if cannot take all clipboard
                }
            }
        iHandler->InsertL( aStartPos, *copiedText );
        }
    else
        { // if no iHandler, are we dealing with a password input box & no format
        if ( iPassword )
            {
            TInt newLength( iPassword->Length() + aLength );
            if ( newLength > iPassword->Des().MaxLength() )
                {
                iPassword =
                    iPassword->ReAllocL( newLength + 1 );
                }
            iPassword->Des().Insert( aStartPos, *copiedText );
            }
        }
    iCursorPos = aStartPos;         // save for possible use by UpdateState if eikedwin does not set up cursorpos correctly
    CleanupStack::PopAndDestroy();  // copiedText
    }


// ---------------------------------------------------------
// CWmlFormattedEditor::SetTextL
// ---------------------------------------------------------
//
void CWmlFormattedEditor::SetTextL( const TDesC* aDes )
    {
    TInt pos = 0, length = iText->DocumentLength();
    if ( iHandler )
        {
        /*lint -e{62} Incompatible types (basic) for operator ':' */
        iHandler->SetTextL( aDes ? *aDes : KNullDesC );
        HBufC* text = iHandler->TextLC( !iSecret );
        if ( text->Length() )
            {
            iText->InsertL( 0, text->Des() );
            pos += text->Length();
            }
        CleanupStack::PopAndDestroy();  // text
        }
    else
        {
        if ( aDes && aDes->Length() )
            {
            const TInt len = aDes->Length();
            if ( iPassword )
                {
                HBufC* newPwd = aDes->AllocL();

                // make illegible copy
                HBufC* illegibleText = HBufC::NewLC( len );
                illegibleText->Des().Fill( KObscuringChar()[ 0 ], len );

                iText->InsertL( 0, *illegibleText );
                pos += len;

                CleanupStack::PopAndDestroy();  // illegibleText

                delete iPassword;   // replace password with new one
                iPassword = newPwd;
                }
            else
                {
                iText->InsertL( 0, *aDes );
                pos += len;
                }
            }
        }

    if ( length )
        {
        RichText()->DelSetInsertCharFormatL( pos, length );
        }
    }

// ---------------------------------------------------------
// CWmlFormattedEditor::CcpuCanCut
// ---------------------------------------------------------
//
TBool CWmlFormattedEditor::CcpuCanCut() const
    {
    // not formatted nor password and doesn't contain static text
    return CEikRichTextEditor::CcpuCanCut() &&
           !iSecret &&
           ( !iHandler || !iHandler->NumberOfStatic() );
    }

// ---------------------------------------------------------
// CWmlFormattedEditor::CcpuCutL
// ---------------------------------------------------------
//
void CWmlFormattedEditor::CcpuCutL()
    {
    TCursorSelection selection = Selection();

    CEikRichTextEditor::CcpuCutL();

    if ( iHandler )
        {
        iHandler->Delete( selection.LowerPos(), selection.Length() );
        }
    }

// ---------------------------------------------------------
// CWmlFormattedEditor::CcpuCanCopy
// ---------------------------------------------------------
//
TBool CWmlFormattedEditor::CcpuCanCopy() const
    {
    return CEikRichTextEditor::CcpuCanCopy() && !iSecret;
    }

// ---------------------------------------------------------
// CWmlFormattedEditor::CcpuCanPaste
//
// "TRAPD" use adapted from EIKEDWIN.CPP function CcpuCanPaste
// ---------------------------------------------------------
//
TBool CWmlFormattedEditor::CcpuCanPaste() const
    {
    if (!iHandler)
        {
        return ETrue;   // if no active format - nothing to check
        }
	  TRAPD(err, DoCanPasteL());
	  return err == KErrNone;
    }

// ---------------------------------------------------------
// CWmlFormattedEditor::DoCanPaste
//
// Adapted from EIKEDWIN.CPP function, DoCcpuCanPasteL().
// It is assumed that iHandler EXISTS for the input box
// under consideration.  If it does not exist, then it can be
// assumed that the paste operation may proceed.
// This routine retrieves clipboard info and calls
// CheckPasteAllowedCharsL to see if a paste operation is
// allowed - based upon a comparison with the input box
// format.
// ---------------------------------------------------------
//
void CWmlFormattedEditor::DoCanPasteL() const
	{
	if (!SelectionLength() && (iTextLimit && (TextLength() >= iTextLimit)))
		User::Leave(KErrNotFound);

	CClipboard* cb=CClipboard::NewForReadingL(iCoeEnv->FsSession());
	CleanupStack::PushL(cb);

	TBool richText = ETrue;
	TStreamId streamId=cb->StreamDictionary().At(KClipboardUidTypeRichText);
	if (streamId==KNullStreamId)
		{
		streamId=cb->StreamDictionary().At(KClipboardUidTypePlainText);
		richText = EFalse;
		}
	if (streamId==KNullStreamId)
		User::Leave(KErrNotFound);

	User::LeaveIfError(CheckPasteAllowedCharsL(*cb, richText));

	CleanupStack::PopAndDestroy(1);	// cb
	}

// ---------------------------------------------------------
// CWmlFormattedEditor::CheckPasteAllowedCharsL
//
// Adapted from EIKEDWIN.CPP function, CheckAllowedCharsL,
// this routine validates the characters selected in the
// clipboard against the input box format, if applicable.
// ---------------------------------------------------------
//
TInt CWmlFormattedEditor::CheckPasteAllowedCharsL(CClipboard& aClipboard, TBool aRichText) const
  {
  CPlainText* txtStore;
  CRichText* richText = STATIC_CAST(CRichText*, Text());
  if (aRichText && richText)
    {
    const CParaFormatLayer* paraFormatLayer=richText->GlobalParaFormatLayer();
    const CCharFormatLayer* charFormatLayer=richText->GlobalCharFormatLayer();
    txtStore = CRichText::NewL(paraFormatLayer, charFormatLayer);
    }
  else
    {
    txtStore = CPlainText::NewL();
    }
  CleanupStack::PushL(txtStore);
  txtStore->PasteFromStoreL(aClipboard.Store(), aClipboard.StreamDictionary(), 0);
  TPtrC text = txtStore->Read(0);

  TInt cursorPos = CursorPos();
  TInt length( text.Length() );
  if (length > 0)
    {
    --length;   // in the debugger, clipboard length seems to be 1 greater than it should be
    }
  TInt adjustedPosition = 0;
  TInt err = KErrNone;
  for ( TInt i = 0; i < length; i++ )
    {
    TChar ch = text[i];
    adjustedPosition = CheckCursorPositionForNextNonStatic(i + cursorPos);
    if (adjustedPosition == KNotFound)  // assumed the current cursor position is a static character
      {
      err = KErrNotFound;
      break;
      }
    else if ( ! iHandler->ValidateChar( ch, iHandler->GetFormat( adjustedPosition ) ) )
      {
      err = KErrNotFound;
      break;
      }
    }

  CleanupStack::PopAndDestroy(txtStore);

  return err;
  }

// ---------------------------------------------------------
// CWmlFormattedEditor::CheckCursorPositionForNextNonStatic
//
// Only interested in checking the current cursor position
// to see if it is a static character.  If it is, search for
// the next NON-STATIC cursor position and return that pos.
// If run out of format length, return KNotFound.
// NOTE: The difference between this function and
// CWmlFormatHandler::NextNonStatic is internal input box
// content (iHandler->iDynamic) is not referenced.  At this
// point, only interested in what the format mask directs.
// That is why only CWmlFormatHandler::IsPosStatic is used.
// ---------------------------------------------------------
//
TInt CWmlFormattedEditor::CheckCursorPositionForNextNonStatic( TInt aCursorPos ) const
  {
  if (!iHandler)
      {
      return aCursorPos;
      }
  TInt i = 0;
  TBool found = ETrue;
  TInt length = iHandler->MaxLength(); // quantity of chars entered so far in box considered as maxlength
  if (length == KInfiniteLength)
      {
      length = KFullFormattingUpperThreshold;
      }
  while ((aCursorPos + i) < length)
      {
      if (iHandler->IsPosStatic( aCursorPos + i ))
          {
          ++ i;
          found = EFalse;
          }
      else
          {
          found = ETrue;
          break;
          }
      }
  if (!found)
      {
      return KNotFound;
      }
  return (aCursorPos + i);
}

// ---------------------------------------------------------
// CWmlFormattedEditor::CcpuPasteL
// ---------------------------------------------------------
//
void CWmlFormattedEditor::CcpuPasteL()
    {
      // selection is cancelled before HandleTextPastedL is called back, so get value now.
    iSelection = iTextView->Selection();
    CEikRichTextEditor::CcpuPasteL();

    if ( iHandler )
        {
        UpdateStateL( EWmlEditorNavigateRight );
        }
    else
        {
        if ( iPassword )
            {
            UpdateStateL( EWmlEditorNavigateRight, EFalse );
            }
        }
    }

// ---------------------------------------------------------
// CWmlFormattedEditor::UpdateStateL
// ---------------------------------------------------------
//
void CWmlFormattedEditor::UpdateStateL( TWmlEdNavigation aNaviType,
                                        TBool aUpdateEditingParameters,
                                        TBool aTextChanged )
    {
    TInt pos( CursorPos() );

    if ( iHandler )
        {
        if (pos > iHandler->FirstEmpty())  // if eikedwin does not set cursor pos correctly
            {
            pos = iCursorPos;
            }
        HBufC* tempText = ReadableTextL();
        if (iHandler->NumberOfStatic())
            {
            iFirstEmpty = iHandler->FirstEmpty(); // iHandler->MaxLength();
            }
        else
            {
            iFirstEmpty = tempText->Length();
            }
        delete tempText;

        if ( aNaviType != EWmlEditorNavigateRight )
            {
            for ( ; pos > 0 && iHandler->GetFormat( pos ).Static(); --pos )
                ;
            }

        if ( iHandler->GetFormat( pos ).Static() ||
             aNaviType == EWmlEditorNavigateRight )
            {
            for ( ; pos < iFirstEmpty && iHandler->GetFormat( pos ).Static()
                  ; ++pos )
                ;
            }

        SetCursorPosL( 0 );
        iText->Reset();
        CEikRichTextEditor::SetTextL( iHandler->TextLC());
        CleanupStack::PopAndDestroy(); // return value of iHandler->TextLC
        if ( iSecret && aNaviType == EWmlEditorNavigateRight )
            {
            SetCursorPosL( pos );
            CEikRichTextEditor::SetTextL( iHandler->TextLC( EFalse ) );
            CleanupStack::PopAndDestroy(); // return value of iHandler->TextLC
            }
        }
    else
        {  // no format involved, so simply use the length of text entered so far
        iFirstEmpty = TextLength();
        if ( iSecret )  // if password input, update what Avkon with changes
            {
            HBufC* text = GetTextInHBufL();
            if ( text )
                {
                CleanupStack::PushL( text );
                text->Des().Fill( KObscuringChar()[ 0 ] );
                CEikRichTextEditor::SetTextL( text );
                CleanupStack::PopAndDestroy(); // text
                }
            }
        }

    // set editor cursor to correct position
    if ( pos != CursorPos() )
        {
        TCursorSelection selection = Selection();
        if ( aTextChanged || selection.LowerPos() == selection.HigherPos())
            {
            if (iHandler && iHandler->PosIncludingStatic( iHandler->FirstEmpty() ) < pos)
                {
                pos = iHandler->PosIncludingStatic( iHandler->FirstEmpty() );
                }
            SetSelectionL( pos, pos );
            }
        else
            {

            if (aNaviType == EWmlEditorNavigateRight)
                // Set the selection from right most selection point to the current cursor pos.
                SetSelectionL ( pos , iHandler->IsPosStatic(selection.LowerPos()) ? selection.LowerPos() + 1 : selection.LowerPos() );
            else
                // Set the selection from current cursor pos to left most selection point
                SetSelectionL (pos , iHandler->IsPosStatic(selection.HigherPos()) ? selection.HigherPos() - 1 : selection.HigherPos());
            }
        }

    iSelection = Selection();
    iCursorPos = pos;
    iOverwrite = EFalse;

    if ( aUpdateEditingParameters )
        {
        UpdateEditingParameters( pos );  // Change parameters
        }

    SetCursorPosL(pos);
    CEikEdwin::ReportEdwinEventL(MEikEdwinObserver::EEventTextUpdate);

    CEikEdwin::DrawContents();
    }


// ---------------------------------------------------------
// CWmlFormattedEditor::UpdateEditingParameters
// ---------------------------------------------------------
//
TBool CWmlFormattedEditor::UpdateEditingParameters( TInt aDocPos )
    {
    if (aDocPos < 0)
        {
        aDocPos = 0;
        iFormat = TWmlFormatChar(0,0);
        }
    TUint inputMode( EAknEditorNullInputMode );
    TUint allowedInputModes( EAknEditorAllInputModes );
    TUint fepCase( EAknEditorAllCaseModes );
    TUint flags( EAknEditorFlagDefault );
    TBool needsChange( EFalse );

    TUint alphaInputModes;
    TUint numericInputModes;
    EVariantFlag variant = AknLayoutUtils::Variant();
    if (variant == EApacVariant)
        {
        alphaInputModes = EAknEditorTextInputMode |
          EAknEditorHalfWidthTextInputMode | EAknEditorFullWidthTextInputMode |
          EAknEditorKatakanaInputMode | EAknEditorFullWidthKatakanaInputMode |
          EAknEditorHiraganaKanjiInputMode | EAknEditorHiraganaInputMode;
        numericInputModes = EAknEditorNumericInputMode | EAknEditorFullWidthNumericInputMode;
        }
    else
        {
        alphaInputModes = EAknEditorTextInputMode;
        numericInputModes = EAknEditorNumericInputMode;
        }

    if ( iT9 )
        {
        inputMode = EAknEditorTextInputMode;
        }
    else
        {
        flags |= EAknEditorFlagNoT9;
        if ( ! iHandler )
            {
            inputMode = EAknEditorTextInputMode;
            }
        }

    if ( iHandler )
        {
        TWmlFormatChar format = iHandler->GetFormat( aDocPos );

        if ( iFormat.Char() != format.Char() )
            {
            if ( format.Char() || iT9 )
                {
                needsChange = ETrue;
                }

            if ( format.SuggestedCase() == TWmlFormatChar::EWmlUpperCase )
                {
                fepCase = EAknEditorUpperCase;
                }

            if (format.SpecialAllowed())
                {
                flags |= EAknEditorFlagUseSCTNumericCharmap ;
                }

            if ( format.ForcedCase() != TWmlFormatChar::EWmlNone )
                {
                flags |= EAknEditorFlagFixedCase;
                }

            if ( format.NumericAllowed() )
                {
                inputMode = EAknEditorNumericInputMode;
                }
            else
                {
                allowedInputModes &= ~numericInputModes;
                }

            if ( format.AlphaAllowed() )
                {
                inputMode = EAknEditorTextInputMode;
                }
            else
                {
                allowedInputModes &= ~alphaInputModes;
                }
            iFormat = format;
            }
        }
    else
        {
        flags |= EAknEditorFlagUseSCTNumericCharmap ;

        // WML format takes precedence over JSky mode or istyle,
        // but if no wml format is defined look at JSky attributes.
        if ( iMode != iPrevMode )
            {
            needsChange = ETrue;
            switch ( iMode )
                {
                case EWmlModeAlphabet:
                    {
                    inputMode = EAknEditorTextInputMode;
                    break;
                    }
                case EWmlModeNumeric:
                    {
                    inputMode = EAknEditorNumericInputMode;
                    break;
                    }
                case EWmlModeHiragana:
                    {
                    inputMode = EAknEditorHiraganaKanjiInputMode;
                    break;
                    }
                case EWmlModeKatakana:
                    {
                    inputMode = EAknEditorFullWidthKatakanaInputMode;
                    break;
                    }
                case EWmlModeHankakukana:
                    {
                    inputMode = EAknEditorKatakanaInputMode;
                    break;
                    }
                default:
                    break;

                }

            iPrevMode = iMode;
            }
        }
    if (iSecret)
        {
        EVariantFlag variant = AknLayoutUtils::Variant();
        if (variant == EApacVariant)
            {
            flags |= EAknEditorFlagLatinInputModesOnly;
            }
        UpdateCaseState(fepCase, fepCase);
        }
    else
        {
        allowedInputModes &= ~EAknEditorSecretAlphaInputMode;
        }
    if (!allowedInputModes)
        {
        inputMode = EAknEditorNullInputMode;
        }
    if (iHandler)
        {
        iOverwrite  = iHandler->PreviousStatic(iHandler->MaxLength()) > CursorPos();
        }
    if ( needsChange )
        {
        UpdateInputModeState(inputMode,
                      ( inputMode != EAknEditorNullInputMode ? allowedInputModes : EAknEditorAllInputModes ));

        UpdateFlagsState( iOverwrite
                           ? flags | EAknEditorFlagMTAutoOverwrite
                           : flags &~ EAknEditorFlagMTAutoOverwrite );

        UpdateCaseState(fepCase, fepCase);
        }
    else
        {
        TBool overWrite( AknEdwinFlags() & EAknEditorFlagMTAutoOverwrite );
        if ( !overWrite && iOverwrite ||
             overWrite && !iOverwrite )
            {   // iOverwrite is changed, flags must be updated
            UpdateFlagsState( iOverwrite
                               ? flags | EAknEditorFlagMTAutoOverwrite
                               : flags &~ EAknEditorFlagMTAutoOverwrite );
            needsChange = ETrue;
            }
        }
    return needsChange;
    }

// ---------------------------------------------------------
// CWmlFormattedEditor::SetEditorCurentCase()
// ---------------------------------------------------------
//
void CWmlFormattedEditor::SetEditorCurentCase()
    {
    TUint fepCase = EAknEditorTextCase;
    if (iHandler)
        {
        TWmlFormatChar format = iHandler->GetFormat(CursorPos());
        TWmlFormatChar::TWmlCase caseFormat = format.SuggestedCase();
        switch( caseFormat )
            {
            case TWmlFormatChar::EWmlUpperCase :
            fepCase = EAknEditorUpperCase;
            break;

            case TWmlFormatChar::EWmlLowerCase :
            fepCase = EAknEditorLowerCase;
            break;

            default  :
            fepCase = EAknEditorTextCase;
            break;
            }
        }
    UpdateCaseState( fepCase, EAknEditorAllCaseModes );
	}


// ---------------------------------------------------------
// CWmlFormattedEditor::Draw
// ---------------------------------------------------------
//
void CWmlFormattedEditor::Draw(const TRect& /*aRect*/) const
    {
    CGraphicsContext& gc = SystemGc();
    TRect internalRect = Rect();
    internalRect.Shrink( 1, 1 );
    iBorder.Draw( gc, internalRect );

    const TRect viewRect = iTextView->ViewRect();
    if (IsDimmed())
        gc.SetBrushColor(iEikonEnv->ControlColor(EColorControlDimmedBackground, *this));
    else
        gc.SetBrushColor(iEikonEnv->ControlColor(EColorControlBackground, *this));
    DrawUtils::ClearBetweenRects(gc, iBorder.InnerRect(internalRect), viewRect);
    TrappedDraw(viewRect);
    }

void CWmlFormattedEditor::DrawContents()
    {
    CEikEdwin::DrawContents();
    }

// -----------------------------------------------------------------------------
// UpdateFlagsState
//
//
// -----------------------------------------------------------------------------
void CWmlFormattedEditor::UpdateFlagsState(TUint flags)
{
    SetAknEditorFlags( flags );

    if (iVkbState)
        {
        iVkbState->SetFlags(flags | EAknEditorFlagUseSCTNumericCharmap);
        iVkbState->ReportAknEdStateEventL(MAknEdStateObserver::EAknEdwinStateFlagsUpdate);
        }
}

// -----------------------------------------------------------------------------
// UpdateInputModeState
//
//
// -----------------------------------------------------------------------------
void CWmlFormattedEditor::UpdateInputModeState(TUint inputMode, TUint permittedInputModes)
{
    SetAknEditorNumericKeymap( EAknEditorPlainNumberModeKeymap );
    SetAknEditorAllowedInputModes( permittedInputModes );
    SetAknEditorInputMode( inputMode );

    if (iVkbState)
        {

        if (permittedInputModes != EAknEditorNumericInputMode) {
            EVariantFlag variant = AknLayoutUtils::Variant();
            if (variant == EApacVariant) {
                  permittedInputModes |= EAknEditorTextInputMode |
                  EAknEditorHalfWidthTextInputMode | EAknEditorFullWidthTextInputMode |
                  EAknEditorKatakanaInputMode | EAknEditorFullWidthKatakanaInputMode |
                  EAknEditorHiraganaKanjiInputMode | EAknEditorHiraganaInputMode;

            }
        }

        iVkbState->SetDefaultInputMode(inputMode);
        iVkbState->SetCurrentInputMode(inputMode);
        iVkbState->SetPermittedInputModes(permittedInputModes);
        iVkbState->SetNumericKeymap(static_cast<TAknEditorNumericKeymap>(EAknEditorPlainNumberModeKeymap));

        iVkbState->ReportAknEdStateEventL(MAknEdStateObserver::EAknEdwinStateInputModeUpdate);
        }
}

// -----------------------------------------------------------------------------
// UpdateCaseState
//
//
// -----------------------------------------------------------------------------
void CWmlFormattedEditor::UpdateCaseState(TUint currentCase, TUint permittedCase)
{
    SetAknEditorCurrentCase( currentCase );

    if (iVkbState)
        {
        iVkbState->SetDefaultCase(currentCase);
        iVkbState->SetCurrentCase(currentCase);
        iVkbState->SetPermittedCases(permittedCase);

        iVkbState->ReportAknEdStateEventL(MAknEdStateObserver::EAknEdwinStateCaseModeUpdate);
        }
}

void CWmlFormattedEditor::SetCursorPosL( TInt aPos )
    {
    CEikRichTextEditor::SetCursorPosL( aPos, EFalse );
    TextView()->SetDocPosL(aPos);
    }

//  End of File
