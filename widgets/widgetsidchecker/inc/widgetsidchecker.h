/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef __WIDGETSIDCHECKER_H__
#define __WIDGETSIDCHECKER_H__

#include <apsidchecker.h>
#include <WidgetRegistryClient.h>

class RWidgetRegistryClientSession;

NONSHARABLE_CLASS(CWidgetSidChecker) : public CAppSidChecker
	{
public:
	static CWidgetSidChecker* NewL();
	~CWidgetSidChecker();

public:
	virtual TBool AppRegisteredAt(const TUid& aSid, TDriveUnit aDrive);
	virtual void SetRescanCallBackL(const TCallBack &aCallback);

private:
    CWidgetSidChecker();
    TBool AppRegisteredAtL(TUid aSid );
    RWidgetRegistryClientSession  iClientSession;
	};

#endif

