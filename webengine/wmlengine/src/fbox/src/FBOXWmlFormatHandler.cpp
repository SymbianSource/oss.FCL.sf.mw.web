/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies).
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
#include <aknutils.h>
#include "nw_fbox_WmlFormatHandler.h"
#include "NW_Unicode_LetterOther.h"
#include "NW_Unicode_LetterChina.h"
#include "NW_Unicode_LetterThai.h"


// CONSTANTS

static const TUint KWmlFormatNone = 0;
static const TUint KWmlFormatA = 'A';
static const TUint KWmlFormata = 'a';
static const TUint KWmlFormatn = 'n';
static const TUint KWmlFormatN = 'N';
static const TUint KWmlFormatX = 'X';
static const TUint KWmlFormatx = 'x';
static const TUint KWmlFormatm = 'm';
static const TUint KWmlFormatM = 'M';
static const TUint KWmlFormatStatic = '\\';

static const TUint KWmlFormatMultiplier = '*';

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
TWmlFormatChar::TWmlFormatChar( TUint aChar, TUint aStatic )
	: iChar( aChar )
	{
	iStatic = ( aChar == KWmlFormatStatic ) ? aStatic : 0;
	}

// ---------------------------------------------------------
// TWmlFormatChar::ForcedCase
// ---------------------------------------------------------
//
TWmlFormatChar::TWmlCase TWmlFormatChar::ForcedCase() const
	{
	TWmlCase result;
	switch( iChar )
		{
		case KWmlFormatA :
		case KWmlFormatX :
			result = EWmlUpperCase;
			break;

		case KWmlFormata :
		case KWmlFormatx :
			result = EWmlLowerCase;
			break;

		case KWmlFormatn :
		case KWmlFormatN :
			result = EWmlNumericCase;
			break;

		default:
			result = EWmlNone;
			break;
		}
	return result;
	}

// ---------------------------------------------------------
// TWmlFormatChar::SuggestedCase
// ---------------------------------------------------------
//
TWmlFormatChar::TWmlCase TWmlFormatChar::SuggestedCase() const
	{
	TWmlCase result;
	switch( iChar )
		{
		case KWmlFormatA :
		case KWmlFormatX :
		case KWmlFormatM :
			result = EWmlUpperCase;
			break;

		case KWmlFormata :
		case KWmlFormatx :
		case KWmlFormatm :
			result = EWmlLowerCase;
			break;

		case KWmlFormatn :
		case KWmlFormatN :
			result = EWmlNumericCase;
			break;

		default:
			result = EWmlNone;
			break;
		}
	return result;
	}

// ---------------------------------------------------------
// TWmlFormatChar::AlphaAllowed
// ---------------------------------------------------------
//
TBool TWmlFormatChar::AlphaAllowed() const
	{
	TBool result;
	switch( iChar )
		{
		case KWmlFormatA :
		case KWmlFormata :
		case KWmlFormatX :
		case KWmlFormatx :
		case KWmlFormatM :
		case KWmlFormatm :
		case KWmlFormatStatic :
			result = ETrue;
			break;

		default:
			result = EFalse;
			break;
		}
	return result;
	}

// ---------------------------------------------------------
// TWmlFormatChar::NumericAllowed
// ---------------------------------------------------------
//
TBool TWmlFormatChar::NumericAllowed() const
	{
	TBool result;
	switch( iChar )
		{
		case KWmlFormatX :
		case KWmlFormatx :
		case KWmlFormatM :
		case KWmlFormatm :
		case KWmlFormatn :
		case KWmlFormatN :
		case KWmlFormatStatic :
			result = ETrue;
			break;

		default:
			result = EFalse;
			break;
		}
	return result;
	}

// ---------------------------------------------------------
// TWmlFormatChar::SpecialAllowed
// ---------------------------------------------------------
//
TBool TWmlFormatChar::SpecialAllowed() const
	{
	TBool result;
	switch( iChar )
		{
		case KWmlFormatA :
		case KWmlFormata :
		case KWmlFormatX :
		case KWmlFormatx :
		case KWmlFormatM :
		case KWmlFormatm :
		case KWmlFormatn :
		case KWmlFormatStatic :
			result = ETrue;
			break;

		default:
			result = EFalse;
			break;
		}
	return result;
	}

// ---------------------------------------------------------
// TWmlFormatChar::Char
// ---------------------------------------------------------
//
TUint TWmlFormatChar::Char() const
	{
	return iChar;
	}

// ---------------------------------------------------------
// TWmlFormatChar::Static()
// ---------------------------------------------------------
//
TUint TWmlFormatChar::Static() const
	{
	return iStatic;
	}

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
TKStaticChar::TKStaticChar( TUint aChar, TInt aPos )
	: iChar( aChar )
	, iPos( aPos )
	{
	}

// ---------------------------------------------------------
// TStaticChar::Char
// ---------------------------------------------------------
//
TUint TKStaticChar::Char() const
	{
	return iChar;	
	}

// ---------------------------------------------------------
// TStaticChar::Pos
// ---------------------------------------------------------
//
TInt TKStaticChar::Pos() const
	{
	return iPos;	
	}

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CWmlFormatHandler::CWmlFormatHandler()
	: iPostFix( KWmlFormatNone )
    {
	}

// EPOC default constructor can leave.
void CWmlFormatHandler::ConstructL
	( const TDesC& aFormatString, TWapDocType aWapDocType )
    {
    if ( aWapDocType != EWmlDocType_Wml_1_0 &&
		 aWapDocType != EWmlDocType_Wml_1_1 &&
		 aWapDocType != EWmlDocType_Wml_1_2 )
		{
		// handle unknown types as WML1.3
		aWapDocType = EWmlDocType_Wml_1_3;
		}

	CArrayFixFlat<TInt>* statics = new( ELeave )CArrayFixFlat<TInt>( 3 );
	CleanupStack::PushL( statics );
	
	// collect the indexes of static characters
	TBool mark = EFalse;
	TInt length = aFormatString.Length();
	for ( TInt i = 0; i < length; i++ )
		{
		if ( mark )	// was the previous character prefix?
			{
			mark = EFalse;
			statics->AppendL( i );
			}
		else
			{		// is it prefix?
			mark = ( aFormatString[ i ] == KWmlFormatStatic );
			}
		}

	TInt numberOfStatics = statics->Count();
	iMinLength = length - numberOfStatics;
	iMaxLength = iMinLength;

	// *f and nf handling	
	if ( length > 1 &&				// longer than one character
		 ( !numberOfStatics ||		// there are no statics, or it isn't static
		   statics->At( numberOfStatics - 1 ) != length - 2 ) )
		{
		if ( aFormatString[ length - 2 ] == KWmlFormatMultiplier )	// *f?
			{
			iPostFix = aFormatString[ ( --length )-- ];
			iMinLength = length - numberOfStatics;
			iMaxLength = KInfiniteLength;
			}
		else
			{
			TChar n( aFormatString[ length - 2 ] );
			if ( n.IsDigit() && n != '0' )	// nf?
				{
                                TInt adjustLength = length;
				iPostFix = aFormatString[ ( --adjustLength )-- ];
				iMaxLength = adjustLength - numberOfStatics + n.GetNumericValue();
				// WML1.0, WML1.1: entry of n characters
				// WML1.2, WML1.3: entry of up to n characters
				iMinLength = aWapDocType > EWmlDocType_Wml_1_1
					? adjustLength - numberOfStatics + 1 : iMaxLength;
				}
			}
		}

	// allocate memory for members
	iMask = HBufC::NewL( length);	
	iStatic = new( ELeave )CArrayFixFlat<TKStaticChar>( 3 );

	// copy aFormatString to iMask except static text
	TInt nextStatic = 0;
	for ( TInt j = 0; j < length; j++ )
		{
		if ( nextStatic < numberOfStatics &&
			 j == statics->At( nextStatic ) )	// is it static?
			{
				nextStatic++;
				iStatic->AppendL
					( TKStaticChar( aFormatString[ j ], j - nextStatic ) );
			}
		else
			{
			iMask->Des().Append( aFormatString[ j ] );
			}
		}

	iDynamic = HBufC::NewL( iMinLength );

	CleanupStack::PopAndDestroy();	// statics
    }

// Two-phased constructor.
CWmlFormatHandler* CWmlFormatHandler::NewLC
	( const TDesC& aFormatString, TWapDocType aWapDocType )
    {
	CWmlFormatHandler* self = new( ELeave )CWmlFormatHandler;
	CleanupStack::PushL( self );
    self->ConstructL( aFormatString, aWapDocType );
	return self;
    }

// Two-phased constructor.
CWmlFormatHandler* CWmlFormatHandler::NewL
	( const TDesC& aFormatString, TWapDocType aWapDocType )
    {
    CWmlFormatHandler* self = NewLC( aFormatString, aWapDocType );
    CleanupStack::Pop();
    return self;
    }
    
// Destructor
CWmlFormatHandler::~CWmlFormatHandler()
    {
	delete iMask;
	delete iStatic;
	delete iDynamic;
    }

// ---------------------------------------------------------
// CWmlFormatHandler::GetTextOnly
// ---------------------------------------------------------
//
HBufC* CWmlFormatHandler::GetTextOnlyL( )
    {
    HBufC* newText = HBufC::NewL( iDynamic->Length() );
    TPtr ptr = iDynamic->Des();

    newText->Des().Copy( ptr );
    return newText;
    }

// ---------------------------------------------------------
// CWmlFormatHandler::SetTextL
// ---------------------------------------------------------
//
TBool CWmlFormatHandler::SetTextL( const TDesC& aText, TBool aNoValidation )
	{
	iDynamic->Des().SetLength( 0 );	// ignore previous text
	
	if(AknLayoutUtils::PenEnabled())
	    {
	    // ignore validation if VKB is in use
	    // validation will be done on exit
	    aNoValidation = ETrue; 
	    }

	// insert chars while they are valid
	TInt j( 0 );
	TInt textLength( aText.Length() );
	for ( ;
		  j < textLength &&
		  ( aNoValidation || ValidateChar( aText[ j ], GetFormat( j ) ) );
		  j++ )
		{

		if ( !GetFormat( j ).Static() )
			{
		    InsertL( j, aText[ j ] );	// insert non-static chars
			}
		}

	return j == textLength;
	}

// ---------------------------------------------------------
// CWmlFormatHandler::TextLC
// ---------------------------------------------------------
//
HBufC* CWmlFormatHandler::TextLC( TBool aReadable ) const
	{
	HBufC* text = StaticTextLC();	// get static text
	TInt staticLength = text->Length();
	TInt dynamicLength = iDynamic->Length();
	TInt dynamicPtr( 0 );
	TInt staticPtr( 0 );
	TInt staticCounter( 0 );
	TInt numberOfStatics( iStatic->Count() );
	// insert dynamic text into the static text as many as possible
	for ( ; dynamicPtr < dynamicLength; ++dynamicPtr, ++staticPtr )
		{
		// find the next non-static position
		while ( staticCounter < numberOfStatics &&
				iStatic->At( staticCounter ).Pos() == staticPtr )
			{
			++staticCounter;
			++staticPtr;
			}

		if ( staticPtr < staticLength )	// is there place for it?
			{
            /*lint -e{111} Assignment to const object */
			text->Des()[ staticPtr ] = aReadable ? ( *iDynamic )[ dynamicPtr ]
												 : KObscuringChar()[ 0 ];
			}
		else
			{
			break;
			}
		}

	if ( dynamicPtr < dynamicLength )	// append the remaining dynamic text
		{
		HBufC* newText = text->ReAllocL
			( staticLength + dynamicLength - dynamicPtr + 1 );

		CleanupStack::Pop();			// text
		CleanupStack::PushL( newText );	// newText
		text = newText;

		if ( aReadable )
			{
			for ( ; dynamicPtr < dynamicLength; dynamicPtr++ )
				{
				text->Des().Append( ( *iDynamic )[ dynamicPtr ] );
				}
			}
		else
			{
			for ( ; dynamicPtr < dynamicLength; dynamicPtr++ )
				{
				text->Des().Append( KObscuringChar );
				}
			}
		}

	text->Des().ZeroTerminate();
	return text;
	}

// ---------------------------------------------------------
// CWmlFormatHandler::InsertL
// ---------------------------------------------------------
//
void CWmlFormatHandler::InsertL( TInt aPos, TChar aChar )
	{
	TBuf<1> buf;
	buf.Append( aChar );
	InsertL( aPos, buf );
	}

// ---------------------------------------------------------
// CWmlFormatHandler::InsertL
// ---------------------------------------------------------
//
void CWmlFormatHandler::InsertL( TInt aPos, const TDesC& aDes )
	{
	TInt newPos = PosExludingStatic( aPos );
	if ( -1 < newPos && newPos <= iDynamic->Length() )
		{
		// make sure there's enough space
		TInt newLength = iDynamic->Length() + aDes.Length();
		if ( iDynamic->Des().MaxLength() < newLength )
			{
			iDynamic = iDynamic->ReAllocL( newLength );
			}

		iDynamic->Des().Insert( newPos, aDes );
		}
	}

// ---------------------------------------------------------
// CWmlFormatHandler::Delete
// ---------------------------------------------------------
//
TBool CWmlFormatHandler::Delete( TInt aPos, TInt aLength )
    {
    TBool status = EFalse;
    TInt newPos = PosExludingStatic( aPos );
    if ( -1 < newPos && newPos <= iDynamic->Length() )
        {
        iDynamic->Des().Delete( newPos, aLength );
        status = ETrue;
        }
    return status;
    }

// ---------------------------------------------------------
// CWmlFormatHandler::StaticTextLC
// ---------------------------------------------------------
//
HBufC* CWmlFormatHandler::StaticTextLC() const
	{
	TInt count = iStatic->Count();
	TInt length = ( count ) ? iStatic->At( count - 1 ).Pos() + 1 : 0;
	HBufC* empty = HBufC::NewLC( length + 1 );
	empty->Des().Fill( ' ', length );
	for ( TInt i = 0; i < count; i++ )
		{
		TKStaticChar staticChar = iStatic->At( i );
        /*lint -e{111} Assignment to const object */
		empty->Des()[ staticChar.Pos() ] =
			STATIC_CAST( TUint16, staticChar.Char() );
		}

	empty->Des().ZeroTerminate();
	return empty;
	}

// ---------------------------------------------------------
// CWmlFormatHandler::NumberOfStatic
// ---------------------------------------------------------
//
TInt CWmlFormatHandler::NumberOfStatic() const
	{
	return iStatic->Count();
	}

// ---------------------------------------------------------
// CWmlFormatHandler::GetFormat
// ---------------------------------------------------------
//
TWmlFormatChar CWmlFormatHandler::GetFormat( TInt aIndex ) const
	{
	TUint code = KWmlFormatNone;
	if ( -1 < aIndex )
		{
		if ( aIndex < iMask->Length() )		// is it in the mask?
			{
			code = ( *iMask )[ aIndex ];
                        TChar n( code );
		        if ( n.IsDigit() || code == KWmlFormatMultiplier )	// nf? *f?
                         {
                          ++aIndex;
                          code = ( *iMask )[ aIndex ];
                         }
			}
		else
			{
			if ( aIndex < iMaxLength ||
				 iMaxLength == KInfiniteLength )		// is it the postfix?
				{
				code = iPostFix;
				}
			}
		}

	TInt staticChar = 0;
	if ( code == KWmlFormatStatic )			// is it static?
		{
		for ( TInt i = iStatic->Count() - 1; i > -1; i-- )
			{
			if ( iStatic->At( i ).Pos() == aIndex )
				{
				staticChar = iStatic->At( i ).Char();
				break;
				}
			}
		}

	return TWmlFormatChar( code, staticChar );
	}

// ---------------------------------------------------------
// CWmlFormatHandler::PostFix
// ---------------------------------------------------------
//
TWmlFormatChar CWmlFormatHandler::PostFix() const
	{
	return iPostFix;
	}

// ---------------------------------------------------------
// CWmlFormatHandler::MinLength
// ---------------------------------------------------------
//
TInt CWmlFormatHandler::MinLength() const
	{
	return iMinLength;
	}

// ---------------------------------------------------------
// CWmlFormatHandler::MaxLength
// ---------------------------------------------------------
//
TInt CWmlFormatHandler::MaxLength() const
	{
	return iMaxLength;
	}

// ---------------------------------------------------------
// CWmlFormatHandler::ValidateChar
// ---------------------------------------------------------
//
TBool CWmlFormatHandler::ValidateChar
	( TChar aChar, TWmlFormatChar aFormatChar ) const
	{
    TBool match( EFalse );

	if ( aFormatChar.Static() )		// is it static?
		{
		match = ( aChar == aFormatChar.Static() );
		}
	else
		{
		if ( aChar.IsDigit() )		// is it number?
			{
			match = aFormatChar.NumericAllowed();
			}
		else
			{
			if ( aChar.IsAlpha() )	// is it alphabetical?
				{
				TWmlFormatChar::TWmlCase forcedCase = aFormatChar.ForcedCase();
				match = ( forcedCase == TWmlFormatChar::EWmlNone )
					? ETrue
					: ( aChar.IsUpper() &&
						forcedCase == TWmlFormatChar::EWmlUpperCase ) ||
					  ( aChar.IsLower() &&
						forcedCase == TWmlFormatChar::EWmlLowerCase );
        // If non-western alpha then assume there is no upper and lower case
        // - so check for LetterOther or LetterChina/LetterThai (depending upon variant).
        // If invalid char for format, value of "match" is retained. 
        if ( !match && !aChar.IsUpper() && !aChar.IsLower() )
            {
            NW_Ucs4 uniCompare;
            EVariantFlag variant = AknLayoutUtils::Variant();
            uniCompare = (NW_Ucs4)aChar;
            if (variant == EApacVariant)
                {
                if ( NW_Unicode_IsLetterChina(uniCompare) )
                    {
                    match = ETrue;
                    }
                if ( NW_Unicode_IsLetterThai(uniCompare) )
                    {
                    match = ETrue;
                    }
                }
            else if ( NW_Unicode_IsLetterOther(uniCompare) )
                {
                match = ETrue;
                }
            }
				}
			else
				{					// it's punctuator or symbol
				match = aFormatChar.SpecialAllowed();
				}
			}
		}

	return match;
	}

// ---------------------------------------------------------
// CWmlFormatHandler::ValidateTextLC
// ---------------------------------------------------------
//
CArrayFixFlat<TInt>* CWmlFormatHandler::ValidateTextLC() const
	{
	CArrayFixFlat<TInt>* errors =
		new( ELeave )CArrayFixFlat<TInt>( 3 );
	CleanupStack::PushL( errors );

	HBufC* text = TextLC();
	TInt length( text->Length() );
	for ( TInt i = 0; i < length; i++ )
		{
		if ( !ValidateChar( ( *text )[ i ], GetFormat( i ) ) )
			{
			errors->AppendL( i );
			}
		}
		
	CleanupStack::PopAndDestroy();	// text
	return errors;
	}

// ---------------------------------------------------------
// CWmlFormatHandler::ValidateTextL
// ---------------------------------------------------------
//
TBool CWmlFormatHandler::ValidateTextL() const
	{
	HBufC* text = TextLC();
	TInt length( text->Length() );
	TInt i;
	for ( i = 0;
		  i < length && ValidateChar( ( *text )[ i ], GetFormat( i ) );
		  i++ )
		{}
		
	CleanupStack::PopAndDestroy();	// text
	return ( i == length );
	}

// ---------------------------------------------------------
// CWmlFormatHandler::PosIncludingStatic
// ---------------------------------------------------------
//
TInt CWmlFormatHandler::PosIncludingStatic( TInt aPosExludingStatic )
	{
	TInt i = 0;
	TInt count = iStatic->Count();
	for ( ; i < count; i++ )
		{
		if ( iStatic->At( i ).Pos() <= aPosExludingStatic )
			{
			aPosExludingStatic++;
			}
		}

	return aPosExludingStatic;
	}

// ---------------------------------------------------------
// CWmlFormatHandler::PosExludingStatic
// ---------------------------------------------------------
//
TInt CWmlFormatHandler::PosExludingStatic( TInt aPosIncludingStatic )
	{
	// find the nearest static character before aPosIncludingStatic
	TInt c = iStatic->Count();
	TInt i (0);
	for ( ;
		  i < c && aPosIncludingStatic >= iStatic->At( i ).Pos();
		  i++ )
		{
		}

	return ( aPosIncludingStatic - i );
	}

// ---------------------------------------------------------
// CWmlFormatHandler::PreviousNonStatic
// ---------------------------------------------------------
//
TInt CWmlFormatHandler::PreviousNonStatic( TInt aPos )
	{
	TInt posExludingStatic = PosExludingStatic( aPos );

	return ( -1 < posExludingStatic )
		? PosIncludingStatic( posExludingStatic )
		: KNotFound;
	}

// ---------------------------------------------------------
// CWmlFormatHandler::NextNonStatic
// ---------------------------------------------------------
//
TInt CWmlFormatHandler::NextNonStatic( TInt aPos )
	{
	TInt posExludingStatic = PosExludingStatic( aPos ) + 1;

	return ( posExludingStatic < iDynamic->Length() )
		? PosIncludingStatic( posExludingStatic )
		: KNotFound;
	}

// ---------------------------------------------------------
// CWmlFormatHandler::FirstEmpty
// ---------------------------------------------------------
//
TInt CWmlFormatHandler::FirstEmpty()
	{
	return PosIncludingStatic( iDynamic->Length() );
	}


// ---------------------------------------------------------
// CWmlFormatHandler::PreviousStatic
// ---------------------------------------------------------
//
TInt CWmlFormatHandler::PreviousStatic( TInt aPos )
	{
  TInt prevStaticPos = 0;
  TInt i = iStatic->Count() - 1;
  for ( ; i >= 0 &&  iStatic->At( i ).Pos() > aPos; i-- )
    {}
  if(i >= 0)
    {
    prevStaticPos  = iStatic->At( i ).Pos() ;
    }
  return prevStaticPos ;
	}



// ---------------------------------------------------------
// CWmlFormatHandler::IsPosStatic
// ---------------------------------------------------------
//
TBool CWmlFormatHandler::IsPosStatic( TInt aPos )
	{
  TInt i = iStatic->Count() - 1;
  for ( ; i >= 0 &&  iStatic->At( i ).Pos() != aPos; i-- )
    {}
  if(i >= 0)
    {
    return ETrue;
    }
  return EFalse;
	}

//  End of File
