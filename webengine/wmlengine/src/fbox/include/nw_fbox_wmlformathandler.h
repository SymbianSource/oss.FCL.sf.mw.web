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


#ifndef CWMLFORMATHANDLER_H
#define CWMLFORMATHANDLER_H

//  INCLUDES

#include <e32base.h>

// CONSTANTS

const TInt KInfiniteLength = -1;
const TInt KNotFound = -1;
_LIT( KObscuringChar, "*" );

// DATA TYPES

enum TWapDocType
	{
	EWmlDocType_Is_Literal_String,
	EWmlDocType_Unknown,
	EWmlDocType_Wml_1_0,
	EWmlDocType_Wta_1_0,
	EWmlDocType_Wml_1_1,
	EWmlDocType_Si_1_0,
	EWmlDocType_Sl_1_0,
	EWmlDocType_Co_1_0,
	EWmlDocType_Channel_1_0,
	EWmlDocType_Wml_1_2,
	EWmlDocType_Wml_1_3
	};


// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
* Tiny class that encapsulates a WML format character.
*/
class TWmlFormatChar
	{
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
		* @param aChar Format character.
		* @param aStatic Static character.
        */
        TWmlFormatChar( TUint aChar = 0, TUint aStatic = 0 );

	public: // New functions

		enum TWmlCase
			{
			EWmlNone,
			EWmlUpperCase,
			EWmlLowerCase,
			EWmlNumericCase
			};

		/**
		* Returns the character case forced by the format character.
		* @return Character case.
		*/
		TWmlCase ForcedCase() const;

		/**
		* Returns the character case suggested by the format character.
		* @return Character case.
		*/
		TWmlCase SuggestedCase() const;

		/**
		* Returns true whether alphabetical characters are allowed.
		* @return Alphabetical characters are allowed.
		*/
		TBool AlphaAllowed() const;

		/**
		* Returns true whether numbers are allowed.
		* @return Numeric characters are allowed.
		*/
		TBool NumericAllowed() const;

		/**
		* Returns true whether special characters are allowed.
		* @return Special characters are allowed.
		*/
		TBool SpecialAllowed() const;

	public:		// Getter functions

		/**
		* Returns format character.
		* @return Format character.
		*/
		TUint Char() const;

		/**
		* Returns static character if the format specifies static text
		* , otherwise 0.
		* @return Static character.
		*/
		TUint Static() const;

	private:	// Data

		TUint iChar;
		TUint iStatic;
	};

class TKStaticChar
	{
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
		* @param aChar Character code.
		* @param aPos Document position.
        */
        TKStaticChar( TUint aChar, TInt aPos );

	public:		// Getter functions

		/**
		* Returns static character.
		* @return Static character.
		*/
		TUint Char() const;

		/**
		* Returns document position.
		* @return Document position.
		*/
		TInt Pos() const;

	private:	// Data

		TUint iChar;
		TInt iPos;
	};

/**
*  Handles WML input format string.
*/
class CWmlFormatHandler : public CBase
    {
    protected:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        CWmlFormatHandler();

    public:  // Constructors and destructor
        
		/**
        * Two-phased constructor.
		* @param aFormatString Validated format string.
		* @param aWapDocType Document type. Format handling is different
		* in earlier WML versions. Other than EWmlDocType_Wml_1_x is handled
		* as EWmlDocType_Wml_1_3.
        */
        static CWmlFormatHandler* NewLC
			( const TDesC& aFormatString, TWapDocType aWapDocType );

        /**
        * Two-phased constructor.
		* @param aFormatString Validated format string.
		* @param aWapDocType Document type. Format handling is different
		* in earlier WML versions. Other than EWmlDocType_Wml_1_x is handled
		* as EWmlDocType_Wml_1_3.
        */
        static CWmlFormatHandler* NewL
			( const TDesC& aFormatString, TWapDocType aWapDocType );
        
        /**
        * Destructor.
        */
        virtual ~CWmlFormatHandler();

    public:	// Text handling

		/**
                 * Returns text only - without static text involved.
                 * This is quite useful for handling the pasting of text into the content of
                 * an input box.
                 * @return HBufC* Pointer to plain text input (no statics inserted).
                 */
                HBufC* GetTextOnlyL();

		/**
		* Stores visible text. If the text is invalid,
		* the static text will be used instead.
		* @param aText Constant descriptor.
		* @param aNoValidation Force no validation.
		* @return ETrue whether the operation succeeds.
		*/
		TBool SetTextL( const TDesC& aText, TBool aNoValidation = EFalse );

		/**
		* Returns zeroterminated copy of the stored content filled with
		* static text in readable or illegible form.
		* @param aReadable If EFalse dynamic text is replaced with
		*				   KObscuringChar.
		* @return Zeroterminated text.
		*/
		HBufC* TextLC( TBool aReadable = ETrue ) const;

		/**
		* Inserts character into the stored text.
		* Nothing happens if aPos is invalid!
		* @param aPos Document position.
		* @param aChar Character to be inserted.
		*/
		void InsertL( TInt aPos, TChar aChar );

		/**
		* Inserts text into the stored text.
		* Nothing happens if aPos is invalid!
		* @param aPos Document position.
		* @param aDes Text to be inserted.
		*/
		void InsertL( TInt aPos, const TDesC& aDes );
		
		/**
		* Deletes characters from the stored text.
		* Nothing happens if aPos or aLength is invalid!
		* @param aPos Document position.
		* @param aLength Specifies the number of characters to be deleted.
		*/
		TBool Delete( TInt aPos, TInt aLength );

	public:	// Static text handling

		/**
		* Returns zeroterminated copy of the static text.
		* @return Zeroterminated copy of the static text.
		*/
		HBufC* StaticTextLC() const;

		/**
		* Returns number of static characters.
		* @return Number of static characters..
		*/
		TInt NumberOfStatic() const;

	public:	// Format handling

        /**
        * Returns the format of the specified document position.
        * @param aIndex Document position.
        * @return Format of the specified document position.
        */
        TWmlFormatChar GetFormat( TInt aIndex ) const;

		/**
		* Returns the format of postfix. It's the value of "f" when
		* format ends with "*f" or "nf", KWmlFormatNone othervise
        * @return Format of postfix.
		*/
		TWmlFormatChar PostFix() const;

	public: // Getter functions
        
		/*
		* Getter function.
		* @return Value of iMinLength member.
		*/
		TInt MinLength() const;

		/*
		* Getter function.
		* @return Value of iMaxLength member.
		*/
		TInt MaxLength() const;

	public:	// Validation functions

		/**
		* Validates the given character against the format character.
		* @param aChar Character to be validated.
		* @param aFormatChar Format character.
		* @return ETrue whether the character matches to the format.
		*/
		TBool ValidateChar( TChar aChar, TWmlFormatChar aFormatChar ) const;

		/**
		* Validates all of the stored text against the stored format.
		* @return List of invalid positions. (always non-NULL, can be empty)
		*/
		CArrayFixFlat<TInt>* ValidateTextLC() const;

		/**
		* Validates all of the stored text against the stored format.
		* @return Text is valid.
		*/
		TBool ValidateTextL() const;

	public:	// position conversions

		/**
		* Returns position in the whole text.
		* @param aPosExludingStatic Position in non-static text.
		* @return Position in the whole text.
		*/
		TInt PosIncludingStatic( TInt aPosExludingStatic );
		
		/**
		* Returns position in the non-static text.
		* @param aPosIncludingStatic Position in the whole text.
		* @return Position in non-static text.
		*/
		TInt PosExludingStatic( TInt aPosIncludingStatic );
	
	public:	// other functions

		/**
		* Returns the position of the previous dynamic character to aPos.
		* Returns KNotFound if there's no previous dynamic character.
		* @param aPos Document position.
		* @return Position of the previous dynamic character.
		*/
		TInt PreviousNonStatic( TInt aPos );

		/**
		* Returns the position of the next dynamic character to aPos.
		* Returns KNotFound if there's no next dynamic character.
		* @param aPos Document position.
		* @return Position of the next dynamic character.
		*/
		TInt NextNonStatic( TInt aPos );

		/**
		* Returns the first position that is empty, but can be filled.
		* Returns the length of the whole text if the text is fully filled.
		* @return First position that is empty, but can be filled.
		*/
		TInt FirstEmpty();

		/**
		* Returns the first static position to the given position from left
		* @return First position of the static character.
		*/

    TInt PreviousStatic( TInt aPos ); 

		/**
		* Returns the ETrue if the given pos is static else false
		* @return is static char 
		*/

    TBool IsPosStatic( TInt aPos );

    private:

        /**
        * By default EPOC constructor is private.
		* @param aFormatString Validated format string.
		* @param aWapDocType Document type. Format handling is different
		* in earlier WML versions. Other than EWmlDocType_Wml_1_x is handled
		* as EWmlDocType_Wml_1_3.
        */
        void ConstructL( const TDesC& aFormatString, TWapDocType aWapDocType );

    private:    // Data
        
		TInt iMinLength;
		TInt iMaxLength;
		HBufC* iMask;
		CArrayFixFlat<TKStaticChar>* iStatic;
		TText iPostFix;
		HBufC* iDynamic;
    };

#endif
            
// End of File
