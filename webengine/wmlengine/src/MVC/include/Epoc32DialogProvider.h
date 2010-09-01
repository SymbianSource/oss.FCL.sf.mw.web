/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef __EPOC32DIALOGPROVIDER_H
#define __EPOC32DIALOGPROVIDER_H
       
struct TSelectOptionData
    {
    TText* iText;
    TBool iIsSelected;
    TBool iIsOptGroup;
    TBool iHasOnPick; 
    };

typedef enum
{
  ENullButton,
  EBackButton,
  ESelectButton,
  EOkButton,
  ECancelButton,
  ESignButton,
  EEnterButton,
  EOptionsButton
} TGenDlgButtonType;

class MEpoc32DialogProvider
{
public:
      /*
        Callback - we should display an error message
      */
      virtual void NotifyErrorL(const TInt aErrCode)=0;

      /*
         Callback - we should display an HTTP error message for specified URI
      */
      virtual void NotifyHttpErrorL(const TInt aErrCode, const TDesC& aUri)=0;

      /* 
         Callback - display a alert dialog to the user 
      */
      virtual void WmlScriptDlgAlertL(const TDesC& aMessage)=0;

      /* 
          Callback - display a confirm dialog to the user 
      */
      virtual TBool WmlScriptDlgConfirmL(const TDesC& aMessage, const TDesC& aYesMessage, const TDesC& aNoMessage)=0;

      /* 
          Callback - display a prompt dialog to the user 
      */
      virtual void WmlScriptDlgPromptL( const TDesC& aMessage, const TDesC& aDefaultInput, HBufC*& aReturnedInput)=0;

      virtual TBool DialogSelectOptionL( const TDesC& aTitle, 
                                         const TBool aMultiple, 
                                         CArrayFix<TSelectOptionData>& aOptions )=0;

      virtual TInt GetUserAuthenticationDataL( const TDesC& aUrl, 
                                               const TDesC& aRealm, 
                                               const TDesC& aDefaultUserName, 
                                               HBufC*& aReturnedUserName, 
                                               HBufC*& aReturnedPasswd,
                                               TBool& aCancelled,
                                               TBool aBasicAuthentication = EFalse) = 0;

      virtual TBool GetUserRedirectionDataL(const TDesC& aOriginalUrl, const TDesC& aRedirectedUrl) = 0;

      // KM Generic dialog API
      virtual TGenDlgButtonType GenDialogPromptL(const TDesC& aTitle, const TDesC& aPrompt, const TGenDlgButtonType aSoftKey1Button, const TGenDlgButtonType aSoftKey2Button)=0;

      virtual TGenDlgButtonType GenDialogInputPromptL(const TDesC& aTitle, const TDesC& aPrompt, const TBool aHideText, const TBool aNumbersOnly, 
            const TGenDlgButtonType aSoftKey1Button, const TGenDlgButtonType aSoftKey2Button, const TInt aMaxInputLength, 
            TDes* aDefaultInputText) = 0;
        
      virtual TGenDlgButtonType GenDialogListSelectL(const TDesC& aTitle, const TDesC& aPrompt, const TGenDlgButtonType aSoftKey1Button, const TGenDlgButtonType aSoftKey2Button, CArrayFix<TPtrC>& aItemsArray, TInt& aSelectedItem)=0;

      virtual TBool ShowObjectDialogL() = 0;
};

#endif
