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
* Description:  Contains functions for Digital Signing (WIM_ENABLED env. only)
*
*/


#include <e32std.h>
#include "WimUi.h"

// ------------------------------------------------------------------------- 
// C++ default constructor can NOT contain any code that might leave
// CWimUi::CWimUi()
// ------------------------------------------------------------------------- 
CWimUi::CWimUi():
CActive(EPriorityStandard)
,iPinPukDialogType(EEnterPinG)
{
}

// ------------------------------------------------------------------------- 
// EPOC default constructor can leave
// void CWimUi::ConstructL()
// ------------------------------------------------------------------------- 
void CWimUi::ConstructL()
{
	iSecDlgs = WimSecurityDialogFactory::CreateL();
	CActiveScheduler::Add(this);
}

// ------------------------------------------------------------------------- 
// Two-phased constructor
// CWimUi* CWimUi::NewL()
// ------------------------------------------------------------------------- 
CWimUi* CWimUi::NewL()
{
	CWimUi* ui = new (ELeave) CWimUi;
	CleanupStack::PushL(ui);
	ui->ConstructL();
	CleanupStack::Pop(); // ui
	return ui;
}

// ------------------------------------------------------------------------- 
// Destructor 
// CWimUi::~CWimUi()
// ------------------------------------------------------------------------- 
CWimUi::~CWimUi()
{
	Cancel();
	delete iSecDlgs;
}

// ------------------------------------------------------------------------- 
// Converter
// LOCAL_C inline TPtrC PtrCFromTText(const TText* aText)
// ------------------------------------------------------------------------- 
LOCAL_C inline TPtrC PtrCFromTText(const TText* aText)
{
    return (aText!=NULL ? TPtrC(aText) : TPtrC());
}

// ------------------------------------------------------------------------- 
// Shows Sign Text Dialog
// void CWimUi::DoShowSignTextDialogL(void* data, void* callbackctx, void* callback)
// ------------------------------------------------------------------------- 
void CWimUi::DoShowSignTextDialogL(void* data, void* callbackctx, void* callback)
{
	iData = data;
	iCallbackCtx = callbackctx;
	iCallback = callback;
	
	iDialogType = KSignTextRequested ;

	iSecDlgs->ShowNoteL(iStatus, EInfoSignTextRequested, 
															 iBooleanValue, KNullDesC);

	SetActive();
}

// ------------------------------------------------------------------------- 
// Shows Select Certificate Dialog
// void CWimUi::DoShowSelectCertificateDlgL(void* data, void* callbackctx, void* callback)
// ------------------------------------------------------------------------- 
void CWimUi::DoShowSelectCertificateDlgL(void* data, void* callbackctx, void* callback)
{
	iData = data;
	iCallbackCtx = callbackctx;
	iCallback = callback;
	iDialogType = KDSignDialogTypeSelectCert;

	CDesCArrayFlat* itemArray = new (ELeave) CDesCArrayFlat(10);
	CleanupStack::PushL(itemArray);

	for (TInt i = 0 ; i < ((NW_Dlg_ListSelect_t*)data)->itemsCount; i++)
	{
		itemArray->AppendL(PtrCFromTText(((NW_Dlg_ListSelect_t*)data)->items[i]));
	}

	//   void ShowSelectCertificateDialogL(TRequestStatus& aStatus, 
	//										TInt& aSelectedCertIndex, 
	//										const MDesCArray& aCertificateLabels);
	iSecDlgs->ShowSelectCertificateDialogL(iStatus,iBooleanValue,
											iSelectedCertIndex,
											*itemArray);
	CleanupStack::PopAndDestroy(); // itemArray
	SetActive();
}

// ------------------------------------------------------------------------- 
// Shows Select Pin Code Query Dialog
// void CWimUi::DoShowPinCodeQueryDialogL(void* data, void* callbackctx, void* callback)
// ------------------------------------------------------------------------- 
void CWimUi::DoShowPinCodeQueryDialogL(void* data, void* callbackctx, void* callback)
{
	iData = data;
	iCallbackCtx = callbackctx;
	iCallback = callback;
	iDialogType = KDSignDialogTypePinCodeQuery;
	TPtrC ptrTitle = ((NW_Dlg_InputPrompt_t*)(data))->title; 
        iParams.iWIMLabel.Format(ptrTitle);
	TPtrC ptrPrompt = ((NW_Dlg_InputPrompt_t*)(data))->prompt; 
	iParams.iPINLabel.Format(ptrPrompt);
	iParams.iMaxLength = KMaxPINInputLength;
	iParams.iMinLength  = KMinPINInputLength;
	
	

	//   void ShowPinCodeQueryDialogL(TRequestStatus& aStatus, 
	//									const TWIMPINParams& aParams, 
	//									const TDialogType aPinPukDialogType, 
	//									TPINValue& aPinValue);
	iSecDlgs->ShowPinCodeQueryDialogL(iStatus, iBooleanValue,
										iParams,
										iPinPukDialogType,
										iPinValue);
	SetActive();
}

// ------------------------------------------------------------------------- 
// Required function for CActive object.
// void CWimUi::RunL()
// ------------------------------------------------------------------------- 
void CWimUi::RunL()
{
	if(iDialogType == KSignTextRequested)
		{
		iDialogType = KDSignDialogTypeSignText;

		iSecDlgs->ShowSignTextDialogL(iStatus, 
										iBooleanValue, 
										PtrCFromTText(((NW_Dlg_Prompt_t*)iData)->prompt));
		SetActive();
		return ;
		}

	if (iDialogType == KDSignDialogTypeSignText)
	{
		if (iBooleanValue)
		{
			((NW_Dlg_Prompt_t*)iData)->selectedButton = (NW_Uint8)((NW_Dlg_Prompt_t*)iData)->button0;
		}
		else
		{
			((NW_Dlg_Prompt_t*)iData)->selectedButton = (NW_Uint8)((NW_Dlg_Prompt_t*)iData)->button1;
		}
	}
	else if (iDialogType == KDSignDialogTypeSelectCert)
	{
		if (iBooleanValue)
		{			
			((NW_Dlg_ListSelect_t*)iData)->selectedButton = (NW_Uint8)((NW_Dlg_ListSelect_t*)iData)->button0;
		// conversion from NW_Uint8 to TInt
		((NW_Dlg_ListSelect_t*)iData)->selectedItem = (NW_Uint8)iSelectedCertIndex;
	}
		else
		{
			((NW_Dlg_ListSelect_t*)iData)->selectedButton = (NW_Uint8)((NW_Dlg_ListSelect_t*)iData)->button1;
		}
		
	}
	else if (iDialogType == KDSignDialogTypePinCodeQuery)
	{
		if (iBooleanValue)
		{			
			
		((NW_Dlg_InputPrompt_t*)iData)->selectedButton = (NW_Uint8)((NW_Dlg_InputPrompt_t*)iData)->button0;		

		HBufC* zeroTerm = HBufC::NewLC(KMaxPINInputLength);
		zeroTerm->Des().Copy(iPinValue);

		TInt length = zeroTerm->Length()+1;
		NW_Mem_memcpy(((NW_Dlg_InputPrompt_t*)iData)->input, zeroTerm->Des().PtrZ(), (length+length));

		CleanupStack::PopAndDestroy(); //zeroTerm
		
		}
		else
		{
			((NW_Dlg_InputPrompt_t*)iData)->selectedButton = (NW_Uint8)((NW_Dlg_InputPrompt_t*)iData)->button1;
			
		}
	}
	else
	{
		User::Leave(KErrNotSupported);
	}

	if (iCallback)
	{
		if (iDialogType == KDSignDialogTypeSignText)
		{
			NW_Dlg_PromptCB_t* callback = (NW_Dlg_PromptCB_t*)iCallback;
			(callback)((NW_Dlg_Prompt_t*)iData, iCallbackCtx);
		}
		else if (iDialogType == KDSignDialogTypeSelectCert)
		{
			NW_Dlg_ListSelectCB_t* callback = (NW_Dlg_ListSelectCB_t*)iCallback;
			(callback)((NW_Dlg_ListSelect_t*)iData, iCallbackCtx);
		}
		else if (iDialogType == KDSignDialogTypePinCodeQuery)
		{
			NW_Dlg_InputPromptCB_t* callback = (NW_Dlg_InputPromptCB_t*)iCallback;
			(callback)((NW_Dlg_InputPrompt_t*)iData, iCallbackCtx);
		}
		else
		{
			User::Leave(KErrNotSupported);
		}
	}
}

// ------------------------------------------------------------------------- 
// Required function for CActive object.
// void CWimUi::DoCancel()
// ------------------------------------------------------------------------- 
void CWimUi::DoCancel()
{
}
