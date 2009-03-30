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
* Description:  Declaration of class CWalletWrapper.
*
*
*/


#ifndef WALLET_WRAPPER_H
#define WALLET_WRAPPER_H

//  INCLUDES
#include <e32base.h>

#ifdef __WALLET
#include <WalletBrowserObserver.h>
#endif


typedef enum
{
    WALLET_XHTML_FILL_ALL,
    WALLET_XHTML_FILL_ONE,
    WALLET_XHTML_FILL_CLOSED,
}Wallet_XHTML_FillOption;

typedef enum
{
    WALLET_XHTML_FAILURE,
    WALLET_XHTML_SUCCESS,
    WALLET_XHTML_OUT_OF_MEMORY,
    WALLET_XHTML_UNEXPECTED_ERROR,
    WALLET_XHTML_BAD_INPUT_PARAM
}WALLET_XHTML_Status_t;


// FORWARD DECLARATION
class CBrCtl;


/**
*  
*  wallet wrapper handling whole wallet realted function
*
*  @lib Browserengine.lib
*  @since Series 60 2.0
*/
class CWalletWrapper :
                            public CBase
#ifdef __WALLET
                              , 
                            public MWalletObserver
#endif
	{
        /*
        * Wallet status 
        */
        enum TWalletStatus
            {
            EWalletCreated = 0,   ///<      Wallet already crated 
            EEWalletNotExist = -1   ///<      wallet already not created.
            };
        /*
        * Wallet tpye 
        */
		enum WalletType
			{
			EEcmlNo,			 ///<      No conatin any ecml filed 
			EEcml				 ///<	   Conatin ecml filed 
			};

	public:     // construction

        /**
        * Two-phased constructor. Leaves on failure.
        * @return The constructed model
        */
        static CWalletWrapper* NewL( CBrCtl* aParent);

        /**
        * Destructor.
        */
        virtual ~CWalletWrapper();

	protected:  // construction

        /**
        * Constructor.
        */
		CWalletWrapper( CBrCtl* aParent);

        /**
        * Second phase constructor. Leaves on failure.
        */
		void ConstructL();

	public:			// used to deriving from MKimonoWalletWrapper

		TBool WalletUI_VisibleCloseWallet();
		WALLET_XHTML_Status_t  WalletFetch_Start( WALLET_XHTML_Status_t (callback)
                                                    (Wallet_XHTML_FillOption, TUint16*, void*), 
                                              TUint16* header, 
                                              TBool ecml_exists, 
                                              void* context);
		void WalletFetch_CloseWallet( TBool end_pressed );
		WALLET_XHTML_Status_t WalletFetch_GetData( TUint16* field_name, 
                                               TUint16** field_value, 
                                               TBool none_filled);
		WALLET_XHTML_Status_t (*iWalletCallback) (Wallet_XHTML_FillOption, TUint16*, void*);

  public:     // from kimone TSK 

        /**
        * wallet already crated or not               
        * @since Series 60 2.0
		* @param -
        * @return TBool wallet status 
        */
		TBool ValletIsCreated();

        /**
        * called when user press use wallet in the browser view              
        * @since Series 60 2.0
		* @param -
        * @return -
        */
        void CmdUseWalletInfoL( );

        /**
        * called when user press close wallet in the browser view
        * @since Series 60 2.0
		* @param -
        * @return TBool  vallet status 
        */
		void CmdCloseWalletL( );

        /**
        * assyncron wallet destroyer 
        * @since Series 60 2.0
		* @param aWrapper pointer to the CWalletWrapper
        * @return TInt  callback int value 
        */
		static TInt DestroyWalletL( TAny* aWrapper );
        
        /**
        * Asyncronous close for wallet on browser exit.
        * @since Series 60 2.0
		* @param aWait pointer to CActiveSchedulerWait
        */
        void CmdCloseWalletOnExitL ( TAny* aWait );


    public: // From MWalletObserver

        /**
        * vallet Handle the wallet messages
        * @since Series 60 2.0
		* @param aOperationId wallet operation id 
		* @param aData	wallet data
        * @return - 
        */
#ifdef __WALLET
        void HandleWalletMessageL( enum TWalletOperationId aOperationId, TAny* aData);
#endif    

private:    // data
#ifdef __WALLET
        MBrowserObserver *iWallet;		///< not owned handled by function 
#endif

        TWalletStatus iWalletStatus;	///< wallet status 
		    WalletType iWalletType;			///< wallet type 				
		    TAny* iContext;					
		    HBufC *iWalletInfoBuffer;    
		    CBrCtl *iBrCtl; // not owned
		    CIdle* iAsyncCallBack;			///< asincron desroyer callback 
		    TBool iFillingFaliure;	
        TAny* iActiveWait;
	};

#endif

// End of file