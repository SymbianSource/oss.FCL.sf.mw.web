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


#ifndef __WIMUI_H
#define __WIMUI_H

/* #include <wimsecdlgs.h> */
#include <nwx_generic_dlg.h>

typedef TInt TDSignDialogType;

const TInt KDSignDialogTypeSignText = 0;
const TInt KDSignDialogTypeSelectCert = 1;
const TInt KDSignDialogTypePinCodeQuery = 2;
const TInt KSignTextRequested= 3;

const TInt KMaxPINInputLength = 8;
const TInt KMinPINInputLength = 1; // currently in use 


class CWimUi : public CActive
{
	public: 

        /**
        * EPOC two phased constructor
        */
		static CWimUi* NewL();

        /**
        * destructor
        */
		~CWimUi();
		
		/**
		 * Shows Sign Text Dialog
		 *
		 * @param void* data
		 *        void* callbackctx
		 *        void* callback
		 * @return void
		 */
		void DoShowSignTextDialogL(void* data, void* callbackctx, void* callback);

		/**
		 * Shows Select Certificate Dialog
		 *
		 * @param void* data
		 *        void* callbackctx
		 *        void* callback
		 * @return void
		 */
		void DoShowSelectCertificateDlgL(void* data, void* callbackctx, void* callback);

		/**
		 * Shows Select Pin Code Query Dialog
		 *
		 * @param void* data
		 *        void* callbackctx
		 *        void* callback
		 * @return void
		 */
		void DoShowPinCodeQueryDialogL(void* data, void* callbackctx, void* callback);

	private: // Private methods

        /**
        * By default EPOC constructor is private.
        */
		void ConstructL();
		
        /**
        * C++ default constructor.
        */
        CWimUi();

        /**
        * Required function for CActive object.
        */
		void RunL();
		
        /**
        * Required function for CActive object.
        */
		void DoCancel();

	private: // Private member variables

		void* iData;
		void* iCallbackCtx;
		void* iCallback;
		TBool iBooleanValue;
		TInt iSelectedCertIndex;

		TWIMPINParams iParams;
		const TDialogType iPinPukDialogType;
		TPINValue iPinValue;
		
		MWimSecurityDialogs* iSecDlgs;
		TDSignDialogType iDialogType;

};

#endif
