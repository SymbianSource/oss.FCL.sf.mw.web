/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of OOM memory plugin. Registers Browser UID with
*				 the framework to receive OOM notifications.
*
*/


#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include <oommonitorplugin.h>

TAny* CreateBrowserOOMPlugin()
	{
	const TUid KBrowserAppUid = { 0x10008d39 };
	return CAppOomMonitorPlugin::NewL(KBrowserAppUid);
	}

const TImplementationProxy ImplementationTable[] =
	{
	{{0x10281f93}, ::CreateBrowserOOMPlugin}
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy) ;
	return ImplementationTable;
	}
