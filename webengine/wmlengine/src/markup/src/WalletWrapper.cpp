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
*     
*
*/



// INCLUDE FILES

#include <e32std.h>
#include <eikdef.h>
#include <akninfrm.h> 
#include <features.hrh>
#include <WalletAVBrowserInterface.h>

#include "BrCtl.h"
#include "MVCShell.h"
#include "WalletWrapper.h"
#include "nwx_string.h"


const TInt KMaxWalletSringLenght	= 512;

// -----------------------------------------------------------------------------
// CWalletWrapper::NewL
// -----------------------------------------------------------------------------
//
CWalletWrapper* CWalletWrapper::NewL ( CBrCtl* aParent)
    { 
    CWalletWrapper* self = new(ELeave) CWalletWrapper(aParent);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();    // self
    return self;
    }


// -----------------------------------------------------------------------------
// CWalletWrapper::~CWalletWrapper
// -----------------------------------------------------------------------------
//
CWalletWrapper::~CWalletWrapper()
    {
	//Check if there is a wallet to close & destroy
	delete iAsyncCallBack;
    }

// -----------------------------------------------------------------------------
// CWalletWrapper::CWalletWrapper
// -----------------------------------------------------------------------------
//
CWalletWrapper::CWalletWrapper( CBrCtl* aParent )
: iBrCtl( aParent )
    {
	iWalletStatus = EEWalletNotExist;	
    }

// -----------------------------------------------------------------------------
// CWalletWrapper::ConstructL
// -----------------------------------------------------------------------------
//
void CWalletWrapper::ConstructL()
  {	    
  iAsyncCallBack = CIdle::NewL( CActive::EPriorityIdle );
  iWalletInfoBuffer = NULL;
  }


// -----------------------------------------------------------------------------
// CWalletWrapper::CmdUseWalletInfo
// -----------------------------------------------------------------------------
//
void CWalletWrapper::CmdUseWalletInfoL(  )
  {	    
	TBool cmdCard = EFalse;

	iBrCtl->Shell()->UseWalletInfo();
	cmdCard = iBrCtl->View()->HasECMLOnPage();

  TRAPD( err,
    iWalletType = ( WalletType ) cmdCard;
	  if( iWalletStatus == EEWalletNotExist )
      {		
		  CWalletAVBrowserInterface::CreateAndRunLD( this, cmdCard );		
      }
	  else
      {
		  if( iWallet )
        {
			  iWallet->HandleBrowserMessageL( MBrowserObserver::EActivateWallet, (TAny*)&cmdCard );
			}
		}
	);

  }


// -----------------------------------------------------------------------------
// CWalletWrapper::DestroyWalletL
// -----------------------------------------------------------------------------
//
TInt CWalletWrapper::DestroyWalletL( TAny* aWrapper )
    {            

	CWalletWrapper* wrap =
        REINTERPRET_CAST( CWalletWrapper*, aWrapper );	
	CWalletAVBrowserInterface::DestroyL(wrap->iWallet);
	wrap->iWalletStatus = EEWalletNotExist;
	wrap->iWallet = NULL;
    CActiveSchedulerWait* wait=
    REINTERPRET_CAST( CActiveSchedulerWait*, wrap->iActiveWait );	
    if ( wait )
        {
        if( wait->IsStarted() )
            {
            wait->AsyncStop();
            }
        }

    return EFalse;
    }

 
// -----------------------------------------------------------------------------
// CWalletWrapper::ValletIsCreated
// -----------------------------------------------------------------------------
//
TBool CWalletWrapper::ValletIsCreated()
	{
	TBool ret = EFalse;
	if( iWalletStatus != EEWalletNotExist )
		{
		ret= ETrue;
		}
    return ret;
    }


// -----------------------------------------------------------------------------
// CWalletWrapper::CmdCloseWalletOnExitL
// -----------------------------------------------------------------------------
//
void CWalletWrapper::CmdCloseWalletOnExitL( TAny* aWait )
	{
    iActiveWait = aWait;
    iWallet->HandleBrowserMessageL( iWallet->ECloseWallet,(TAny*)NULL );
    }


// -----------------------------------------------------------------------------
// CWalletWrapper::CmdCloseWalletL
// -----------------------------------------------------------------------------
//
void CWalletWrapper::CmdCloseWalletL()
	{
	iWallet->HandleBrowserMessageL( iWallet->ECloseWallet,(TAny*)NULL );
    }


// -----------------------------------------------------------------------------
// CBrowserContentView::HandleWalletMessageL
// -----------------------------------------------------------------------------
//
void CWalletWrapper::HandleWalletMessageL( enum TWalletOperationId aOperationId, TAny* aData)
  {
  switch( aOperationId )
    {
    case EWalletObserver:
			{	
      iWallet = (MBrowserObserver*)aData;
  		if( iWallet )
				{
				iWalletStatus = EWalletCreated;
				}
			}
      break;

    case EControlToBrowser:
      break;

    case EWalletClosed:
      {				
			if ( !iAsyncCallBack->IsActive() )	// asyincon destroy
				{
				iAsyncCallBack->Start( TCallBack( DestroyWalletL, this ) );
				}
      }
      break;

    case EFillCurrentField:
			{
			if ( aData )
				{
				WALLET_XHTML_Status_t status;
				iWalletInfoBuffer = (HBufC*)aData;

				iWalletInfoBuffer = iWalletInfoBuffer->ReAlloc( iWalletInfoBuffer->Des().Length() + 2 );
				iWalletInfoBuffer->Des().ZeroTerminate();
				TText *buf = ::NW_Str_Newcpy( (TText*)iWalletInfoBuffer->Des().Ptr() );
				status = (iWalletCallback)( WALLET_XHTML_FILL_ONE,buf,iContext);

        // TODO: line is neeeded to avoid a memory leak ! Not need now !
				// delete iWalletInfoBuffer;  
				}
			}
			break;

		case EReadyToFill:
			{				
			WALLET_XHTML_Status_t status;
			//iWalletInfoBuffer = (HBufC*)aData;  												
			status = (iWalletCallback)( WALLET_XHTML_FILL_ALL, NULL, iContext );
			TBool walletStatus = iFillingFaliure;

			iWallet->HandleBrowserMessageL(iWallet->EBrowserReady, (TAny*) &walletStatus);
			}
      break;

    default:
      break;
    } 
  } 

// -----------------------------------------------------------------------------
// CWalletWrapper::WalletUI_VisibleCloseWallet
// this callback called by KIMONO callback 
// -----------------------------------------------------------------------------
// 
TBool CWalletWrapper::WalletUI_VisibleCloseWallet()
	{
	TBool ret = ValletIsCreated();
	return ret;
	}


// -----------------------------------------------------------------------------
// CWalletWrapper::WalletFetch_Start
// this callback called by KIMONO callback 
// -----------------------------------------------------------------------------
// 
WALLET_XHTML_Status_t  
CWalletWrapper::WalletFetch_Start( WALLET_XHTML_Status_t (callback)(Wallet_XHTML_FillOption,
                                                                    TUint16*, void*),
                                   TUint16* /*header*/, 
                                   TBool ecml_exists, 
                                   void* context)
	{
	iWalletCallback = callback;
	iWalletType = (WalletType)ecml_exists;
	iContext = context;
	return WALLET_XHTML_SUCCESS;
	}


void CWalletWrapper::WalletFetch_CloseWallet( TBool end_pressed )
	{
	TBool press = EFalse;
	press = end_pressed;
	TRAPD( err,iWallet->HandleBrowserMessageL( iWallet->ECloseWallet,(TAny*)&press ) );
	}


// -----------------------------------------------------------------------------
// CWalletWrapper::WalletFetch_GetData
// this callback called by KIMONO callback 
// -----------------------------------------------------------------------------
// 
WALLET_XHTML_Status_t 
CWalletWrapper::WalletFetch_GetData( TUint16* field_name,
											               TUint16** field_value,
											               TBool none_filled )
	{
	TBuf<KMaxWalletSringLenght+2> field;
	WALLET_XHTML_Status_t  ret = WALLET_XHTML_SUCCESS; 

	if( field_name )
		{
		TPtrC tempField( field_name );
		if( tempField.Length() <= KMaxWalletSringLenght )
			{
			field.Copy( field_name );
			}
		else
			{
			field.Copy( field_name,KMaxWalletSringLenght );
			}
		field.ZeroTerminate();

		iWalletInfoBuffer = HBufC::NewL( KMaxWalletSringLenght );

		iWalletInfoBuffer->Des().Copy( field );
		TRAPD( errhandle, iWallet->HandleBrowserMessageL( iWallet->EFillAllFields, 
			                                                (TAny * )iWalletInfoBuffer ) );
		if( iWalletInfoBuffer->Compare( KNullDesC ) ) 
			{
			 TText *waletOut = CONST_CAST( TText*, iWalletInfoBuffer->Des().Ptr() );
			*field_value = ::NW_Str_Newcpy( waletOut );
			}
		else
			{
			field_value = (TText**)NULL;
			ret = WALLET_XHTML_FAILURE ;//WALLET_XHTML_FAILURE; //WALLET_XHTML_BAD_INPUT_PARAM 
			}

// TODO: line is neeeded to avoid a memory leak ! Not need now !
			//delete iWalletInfoBuffer;
		}
	else
		{
		iFillingFaliure = none_filled;
		}
	return ret;
	}