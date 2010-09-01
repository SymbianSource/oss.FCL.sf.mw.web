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
* Description:  Publish&Subscribe event listener 
*
*/

#ifndef WIDGETUIPSNOTIFIER_H_
#define WIDGETUIPSNOTIFIER_H_

#include <e32base.h>
#include <e32property.h>
#include <flogger.h>
#include <widgetregistryclient.h>
#include <WidgetRegistryData.h>

class CWidgetUiWindowManager;
class CWidgetUiWindow;

const TUid KMyPropertyCat = { 0x10282E5A };
const TUid KPropertyWidgetUI = { 0x10282822 };
enum  TPropertyKeyValues {EWidgetRegAltered = 110, ESapiPrompt = 115, ESapiPromptCleared=116};
enum  TOperations {EIncrement = 1, EDecrement = -1};
class CWidgetUiPSNotifier : public CActive
    {
    CWidgetUiPSNotifier(CWidgetUiWindowManager& aWindowManager, TPropertyKeyValues aKey);

    void ConstructL();
    void RunL();
    void DoCancel();
public:
    static CWidgetUiPSNotifier* NewL(CWidgetUiWindowManager& aWindowManager , TPropertyKeyValues aKey);
    ~CWidgetUiPSNotifier();
    RProperty                           iProperty;    
    void UpdateWindowList();
    void UpdateSapiPromptState();
private:
     CWidgetUiWindowManager& iWindowManager;
     TPropertyKeyValues iKey;
    };
#endif
