/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This class represents the Widget Extension object
*
*/


#ifndef __WIDGETEXTENSION_H
#define __WIDGETEXTENSION_H

//  INCLUDES
#include <e32base.h>
#include <e32std.h>
#include <Browser_Platform_Variant.hrh>
#include <brctldefs.h>
#include <brctlinterface.h>
#include "WidgetEngineCallbacks.h"

// FUNCTION PROTOTYPES

// FORWARD DECLARATION
class WebView;
class MWidgetEngineBridge;
#if defined(BRDO_LIW_FF)
class MDeviceBridge;
#endif

// CLASS DECLARATION
/**
*  CWidgetExtension
*
*  @lib browserengine.dll
*  @since 3.2
*/
class CWidgetExtension: public CBase, public MWidgetExtension, public MWidgetEngineCallbacks
{

public:
    static CWidgetExtension* NewL( WebView& aWebKitView, MWidgetCallback& aWidgetCallback );
    ~CWidgetExtension();

public:
    void MenuItemSelected(TInt aCommand);
    void AddOptionMenuItemsL( CEikMenuPane& aMenuPane,TInt aResourceId );
    void DrawTransition(CWindowGc& gc, CFbsBitmap* fbsBm);
    void SetParamL(TBrCtlDefs::TBrCtlWidgetParams aParam, const TDesC& aValue);
    void SetParamL(TBrCtlDefs::TBrCtlWidgetParams aParam, TUint aValue);
    TBool HandleCommandL( TInt aCommandId );
    TInt GetWidgetId(){ return iWidgetId;}

public:
    void drawWidgetTransition();
    CFbsBitmap* offscreenBitmap();
    void setRightSoftKeyLabel(const TDesC& aText);
    void setLeftSoftKeyLabel(const TDesC& aText);
    void setTabbednavigation(bool aOn);
    bool IsWidgetPublising(){ return m_isWidgetPublishing;}

private:
    void AddJSExtension(const TDesC& id, void* obj);
    CWidgetExtension(WebView& aWebKitView);
    void ConstructL(MWidgetCallback& aWidgetCallback);

    RLibrary                 m_widgetLibrary;
    WebView*                 m_webview;

    MWidgetEngineBridge*     m_widgetengine;
    MWidgetCallback*         m_widgetcallback;
    TInt 		             iWidgetId;	
    bool                     m_isWidgetPublishing;

#if defined(BRDO_LIW_FF)
    RLibrary                 m_deviceLibrary;
    MDeviceBridge*           m_deviceBridge;
#endif
};


#endif
