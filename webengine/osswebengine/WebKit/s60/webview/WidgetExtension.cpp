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


// INCLUDE FILES
#include "WidgetExtension.h"
#include "WidgetEngineBridge.h"
#if defined(BRDO_LIW_FF)
#include "DeviceBridge.h"
#endif
#include "WebView.h"
#include "WebFrame.h"
#include "WebSurface.h"
#include "StaticObjectsContainer.h"
#include "Frame.h"
#include "Page.h"
#include "Settings.h"
#include "WebCoreFrameBridge.h"
#include "BrCtl.h"
#include "WebCursor.h"
#include "SettingsContainer.h"

// LOCAL FUNCTION PROTOTYPES

// ----------------------------------------------------------------------------
// CWidgetExtension::NewL
//
//
//
// ----------------------------------------------------------------------------
//
CWidgetExtension* CWidgetExtension::NewL(WebView& aWebKitView, MWidgetCallback& aWidgetCallback)
{
    CWidgetExtension* self = new (ELeave) CWidgetExtension(aWebKitView);
    CleanupStack::PushL(self);
    self->ConstructL(aWidgetCallback);
    CleanupStack::Pop();
    return self;
}

// ----------------------------------------------------------------------------
// CWidgetExtension::CWidgetExtension
//
//
//
// ----------------------------------------------------------------------------
//
CWidgetExtension::CWidgetExtension(WebView& aWebKitView) : 
                            m_webview(&aWebKitView), 
                            m_isWidgetPublishing ( false)
{
}


CWidgetExtension::~CWidgetExtension()
{
    delete m_widgetengine;
    m_widgetLibrary.Close();
#if defined(BRDO_LIW_FF)
    delete m_deviceBridge;
    m_deviceLibrary.Close();
#endif
}

void CWidgetExtension::ConstructL(MWidgetCallback& aWidgetCallback)
{
    if (!m_widgetengine) {
        _LIT( KBrowserWidgetEngineName, "widgetengine.dll" );
        User::LeaveIfError( m_widgetLibrary.Load(KBrowserWidgetEngineName) );
        TLibraryFunction entry = m_widgetLibrary.Lookup(1);
        if (!entry) {
            User::Leave(KErrNotFound);
        }
        m_widgetengine = (MWidgetEngineBridge*) entry();
        m_widgetcallback = &aWidgetCallback;

        if (m_widgetengine) {
            AddJSExtension(_L("widget"),m_widgetengine->Widget(*m_widgetcallback, *this));
            AddJSExtension(_L("menu"),m_widgetengine->Menu(*m_widgetcallback, *this));
            AddJSExtension(_L("MenuItem"),m_widgetengine->MenuItem(*m_widgetcallback, *this));
        }

#if defined(BRDO_LIW_FF)
        // device for SAPI
        _LIT( KDeviceDLLName, "jsdevice.dll" );
        User::LeaveIfError( m_deviceLibrary.Load(KDeviceDLLName) );
        TLibraryFunction device_entry = m_deviceLibrary.Lookup(1);
        if (!device_entry) {
            User::Leave(KErrNotFound);
        }
        m_deviceBridge = (MDeviceBridge*) device_entry();

        if (m_deviceBridge) {
            AddJSExtension(_L("device"), m_deviceBridge->Device(0));
        }
#endif

        if (m_webview && m_webview->page()) {
            WebCore::Settings* settings = m_webview->page()->settings();
            if (settings) {
                settings->setUsesDashboardBackwardCompatibilityMode(true);
            }
        }

    }


}

void CWidgetExtension::AddJSExtension(const TDesC& id, void* obj)
{
    WebCore::Frame* frm = core(m_webview->mainFrame());
    if (frm) {
       WebCoreFrameBridge* brdg =  frm->bridge();
       if (brdg) {
            brdg->addJSExtension(id, static_cast<KJS::JSObject*>(obj));
       }
    }

}

void CWidgetExtension::MenuItemSelected(TInt aCommand)
{
    if (m_widgetengine) {
        m_widgetengine->MenuItemSelected(aCommand);
    }
}

void CWidgetExtension::AddOptionMenuItemsL( CEikMenuPane& aMenuPane,TInt aResourceId )
{
    if (m_widgetengine) {
        m_widgetengine->AddOptionMenuItemsL(aMenuPane, aResourceId);
    }
}

void CWidgetExtension::DrawTransition(CWindowGc& gc, CFbsBitmap* fbsBm)
{

    if (m_widgetengine) {
        m_widgetengine->DrawTransition(gc,fbsBm);
    }

}

//From MWidgetExtension
void CWidgetExtension::SetParamL(TBrCtlDefs::TBrCtlWidgetParams aParam, const TDesC& aValue)
{
    if (m_widgetengine) {
        m_widgetengine->SetParamL(aParam,aValue);
    }
}

void CWidgetExtension::SetParamL(TBrCtlDefs::TBrCtlWidgetParams aParam, TUint aValue)
{
    if ( aParam == TBrCtlDefs::EWidgetPublishState)  {
        m_isWidgetPublishing = aValue;
        return ; 
    }
    if (m_widgetengine) {
             m_widgetengine->SetParamL(aParam,aValue);
             switch( aParam ) {
                 case TBrCtlDefs::EWidgetIdentifier: {
                     iWidgetId = aValue;
#if defined(BRDO_LIW_FF)
                     m_deviceBridge->SetUid( iWidgetId);
#endif
                 break;
                }
            }
        }
}

TBool CWidgetExtension::HandleCommandL( TInt aCommandId )
{
    if (m_widgetengine) {
        return m_widgetengine->HandleCommandL(aCommandId);
    }
    return EFalse;
}

//From WidgetEngineCallbacks
void CWidgetExtension::drawWidgetTransition()
{
    WebSurface* surface = WebCore::StaticObjectsContainer::instance()->webSurface();
    if (surface) {
        surface->topView()->DrawDeferred();
    }
}

CFbsBitmap* CWidgetExtension::offscreenBitmap()
{
    return WebCore::StaticObjectsContainer::instance()->webSurface()->offscreenBitmap();
}

void CWidgetExtension::setRightSoftKeyLabel(const TDesC& aText)
{
    if (m_webview && m_webview->brCtl() && m_webview->brCtl()->brCtlSoftkeysObserver()) {
        m_webview->brCtl()->brCtlSoftkeysObserver()->UpdateSoftkeyL(EKeyRight, aText, 0, EChangeReasonLoad );
    }

}

void CWidgetExtension::setLeftSoftKeyLabel(const TDesC& aText)
{
    if (m_webview && m_webview->brCtl() && m_webview->brCtl()->brCtlSoftkeysObserver()) {
        m_webview->brCtl()->brCtlSoftkeysObserver()->UpdateSoftkeyL(EKeyLeft, aText, 0, EChangeReasonLoad );
    }

}

void CWidgetExtension::setTabbednavigation(bool aOn)
{
    m_webview->brCtl()->settings()->setTabbedNavigation(aOn);
    WebCore::StaticObjectsContainer::instance()->webCursor()->cursorUpdate(true);
}


//END OF FILE



