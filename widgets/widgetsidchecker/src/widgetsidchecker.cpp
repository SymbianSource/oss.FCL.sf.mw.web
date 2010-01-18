/*
* Copyright (c) 2007, 2008 Nokia Corporation and/or its subsidiary(-ies).
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
#include "widgetsidchecker.h"

#include <e32std.h>
#include <e32const.h>
#include <implementationproxy.h>
#include <f32file.h>
#include <w32std.h>
#include <apgtask.h>
//#include <eikenv.h>
#include <s32file.h>

//WidgetRegistry.EXE[10282f06]0001
//const TUid KWidgetRegistry = { 0x10282F06 };
//0x10282F06
//hb
//#include <WidgetRegistryClient.h>
//////////////////////////////
// ECOM Implementation Table
//////////////////////////////


TBool E32Dll()
	{
	return (ETrue);
	}

const TImplementationProxy ImplementationTable[] =
	{
		IMPLEMENTATION_PROXY_ENTRY(0x10281FC0, CWidgetSidChecker::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}

//////////////////////////////
// CWidgetSidChecker
//////////////////////////////

CWidgetSidChecker* CWidgetSidChecker::NewL()
	{
	CWidgetSidChecker* self = new(ELeave) CWidgetSidChecker();
	return self;
	}

    CWidgetSidChecker::CWidgetSidChecker()
    {
	}
	

CWidgetSidChecker::~CWidgetSidChecker()
	{
	iClientSession.Close();
	}

TBool CWidgetSidChecker::AppRegisteredAt(const TUid& aSid, TDriveUnit /*aDrive*/)
    {
      TFindProcess findProcess (_L("widgetregistry*"));
      TFullName result;       
      if(findProcess.Next(result) == KErrNone )
        {
        TBool res = AppRegisteredAtL(aSid);
        return res;
        }
      return EFalse;      
    }
	

TBool CWidgetSidChecker::AppRegisteredAtL( TUid aSid )
    {
    TBuf<KMaxFileName> aWidgetBundleId;
    TBool res = EFalse;
    
    User::LeaveIfError( iClientSession.Connect() );    
    iClientSession.GetWidgetBundleId(aSid, aWidgetBundleId);
    //check if the widget exists by querying to WidgetRegisrty return ETrue if exists else return EFalse
    if(iClientSession.WidgetExistsL( aWidgetBundleId ) )    
      res = ETrue;
    else
      res = EFalse;
    
    iClientSession.Disconnect();
    return res;
    }
	
void CWidgetSidChecker::SetRescanCallBackL(const TCallBack &/*aCallBack*/)
	{
	return;
	}

// End of file
