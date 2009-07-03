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
* Description:  
*
*/


#include "config.h"
#include <../bidi.h>            // work around for multiple bidi.h files
#include "StaticObjectsContainer.h"
#include "Brctl.h"
#include "AtomicString.h"
#include "PlatformFontCache.h"
#include "PictographSymbian.h"
#include "FontCache.h"
#include "FormFillController.h"
#include "ResourceLoaderDelegate.h"
#include "webkitLogger.h"
#include "brctldefs.h"
#include "WebIconDatabase.h"
#include "WebSurface.h"
#include "WebCursor.h"
#include "PluginHandler.h"
#include "WebCannedImages.h"
#include "OOMHandler.h"
#include "SharedTimer.h"
#include "TextEncodingRegistry.h"
#include "CSSStyleSelector.h"
#include "RenderStyle.h"
#include "Page.h"
#include "Cache.h"
#include "StreamingTextCodecSymbian.h"
#include "HTMLNames.h"
#include "XMLNames.h"
#include "FontCache.h"
#include "RenderThemeSymbian.h"
#include "qualifiedname.h"
#include "XMLTokenizer.h"
#include <eikenv.h>

#include "WidgetEngineBridge.h"
#if defined(BRDO_LIW_FF)
#include "DeviceBridge.h"
#endif

const TInt KLowResolutionDpi = 130;
const TInt KMediumResolutionDpi = 200;

namespace WebCore {

StaticObjectsContainer* StaticObjectsContainer::gInstance = 0;

StaticObjectsContainer::StaticObjectsContainer() : 
     m_fontCache(0)
    ,m_formFillController(0)
    ,m_pictograph(NULL)
    ,m_resourceLoaderDelegate(NULL)
    ,m_stream(NULL)
    ,m_icondatabase(NULL)
    ,m_rendertarget(NULL)
    ,m_cursor(NULL)
    ,m_cannedimg(NULL)
    ,m_pluginhandler(NULL)
    ,m_refcount(0)
    ,m_capabilities(0)
    ,m_oomHandler(0)
    ,m_fullScreenMode(false)
    ,m_pluginFullscreen(false)
    ,m_symbianTheme(NULL)
{
    // Check the device resolution
    CEikonEnv* eikEnv = CEikonEnv::Static();
    if( eikEnv ) {
        CWsScreenDevice& screenDev = *eikEnv->ScreenDevice();

        TUint dpi = screenDev.VerticalTwipsToPixels(KTwipsPerInch);

        if( dpi <= KLowResolutionDpi ) {
            m_screenRes = ELowScreenResolution;
        }
        else if( dpi > KLowResolutionDpi && dpi <= KMediumResolutionDpi ) {
            m_screenRes = EMediumScreenResolution;
        }
        else {
            m_screenRes = EHighScreenResolution;
        }
    }
    m_oomHandler = new OOMHandler();
}

StaticObjectsContainer::~StaticObjectsContainer()
{
    // Run KJS collector to cleanup any remaining references
    // This must be run before Cache::deleteStaticCache to properly free resources
    KJS::Collector::collect();
    delete m_oomHandler;
    FontCache::deleteFontDataCache();
    delete m_fontCache;
    delete m_formFillController;
    delete m_pictograph;
    delete m_resourceLoaderDelegate;
    delete m_stream;
    delete m_icondatabase;
    delete m_rendertarget;
    delete m_cannedimg;
    delete m_cursor;
    delete m_pluginhandler;
    delete m_symbianTheme;
    gInstance = NULL;
    deletePageStaticData();
    CSSStyleSelector::deleteDefaultStyle();
    deleteEncodingMaps();
    RenderStyle::deleteDefaultRenderStyle();
    Cache::deleteStaticCache();
    TextCodecSymbian::deleteStatAvailCharsets();
    QualifiedName::cleanup();
    XMLNames::remove();
    // HTMLNames::remove() will destroy the AtomicString table
    // All other atomic string destruction must be done before this call
    //
    HTMLNames::remove();
    XMLTokenizer::cleanupXMLStringParser();
    shutdownSharedTimer();
    m_widgetLibrary.Close();
#if defined(BRDO_LIW_FF)
    m_deviceLibrary.Close();
#endif
    CloseSTDLIB();
}

StaticObjectsContainer* StaticObjectsContainer::instance()
{
    if( !gInstance )
        gInstance = new (ELeave) StaticObjectsContainer;
    return gInstance;
}

PictographSymbian* StaticObjectsContainer::pictograph()
{
    if (!m_pictograph)
        {        
        m_pictograph = new PictographSymbian();
        }
    return m_pictograph;
}

PlatformFontCache* StaticObjectsContainer::fontCache()
{
    if( !m_fontCache )
        {
        m_fontCache = new PlatformFontCache();
        // tot:fixme. find out why platforminit is not called by the core
        FontCache::platformInit();
        }

    return m_fontCache;
}

FormFillController* StaticObjectsContainer::formFillController()
{
    if(!m_formFillController)
        m_formFillController = new FormFillController();

    return m_formFillController;
}

RFs& StaticObjectsContainer::fsSession()
{
    return CEikonEnv::Static()->FsSession();
}

ResourceLoaderDelegate* StaticObjectsContainer::resourceLoaderDelegate()
{
    if (!m_resourceLoaderDelegate) {
        m_resourceLoaderDelegate = new ResourceLoaderDelegate;
    }
    return m_resourceLoaderDelegate; 
}

LogStream& StaticObjectsContainer::logStream()
{
    if(!m_stream)
        m_stream = new LogStream();
    return *m_stream;
}


WebIconDatabase* StaticObjectsContainer::sharedIconDatabase()
{
    if (!m_icondatabase) {
        m_icondatabase = new WebIconDatabase();
        m_icondatabase->openSharedDatabase();
    }
    return m_icondatabase;
}

WebSurface* StaticObjectsContainer::webSurface()
{
    if (!m_rendertarget) {
        m_rendertarget = WebSurface::NewL();
    }
    return m_rendertarget;
}

WebCursor* StaticObjectsContainer::webCursor()
{
    if (!m_cursor) {
        m_cursor = WebCursor::NewL();
    }
    return m_cursor;
}

WebCannedImages* StaticObjectsContainer::webCannedImages()
{
    if (!m_cannedimg) {
        m_cannedimg = WebCannedImages::NewL();
    }
    return m_cannedimg;
}

PluginHandler* StaticObjectsContainer::pluginHandler()
{
    if (!m_pluginhandler) {
        //tot:fixme 
        TBool enablePlugin = ETrue;
        m_pluginhandler = PluginHandler::NewL(enablePlugin);
    }
    return m_pluginhandler;
}

#if defined(BRDO_LIW_FF)
MDeviceBridge* StaticObjectsContainer::getDeviceBridgeL()
{
    MDeviceBridge* device(NULL);
    
    if( !m_deviceLibrary.Handle() ) {
        _LIT( KDeviceDLLName, "jsdevice.dll" );
        User::LeaveIfError( m_deviceLibrary.Load(KDeviceDLLName) );
    }
    
    TLibraryFunction device_entry = m_deviceLibrary.Lookup(1);
    if (device_entry) {
        device = (MDeviceBridge*) device_entry();
    }
    return device;
}
#endif 

MWidgetEngineBridge* StaticObjectsContainer::getWidgetEngineBridgeL()
{
    MWidgetEngineBridge* widget(NULL);
    
    if( !m_widgetLibrary.Handle() ) {
        _LIT( KBrowserWidgetEngineName, "widgetengine.dll" );
        User::LeaveIfError( m_widgetLibrary.Load(KBrowserWidgetEngineName) );
    }
    
    TLibraryFunction entry = m_widgetLibrary.Lookup(1);
    if (entry) {
        widget = (MWidgetEngineBridge*) entry(); 
    }
    return widget;    
}

CBrCtl* StaticObjectsContainer::brctl() const
{
    if (m_activeBrCtls.size() > 0)
        return m_activeBrCtls[0];
    return NULL;        
}

const Vector<CBrCtl*>& StaticObjectsContainer::activeBrowserControls() const
{
    return m_activeBrCtls;
}

void StaticObjectsContainer::ref(CBrCtl& brctl)              
{ 
    m_refcount++; 
    m_activeBrCtls.append(&brctl);
}

void StaticObjectsContainer::deref(CBrCtl& brctl)            
{ 
    for (int i = 0; i < m_activeBrCtls.size(); i++) {
        if (m_activeBrCtls[i] == &brctl) {
            m_activeBrCtls.remove(i);
            break;
        }
    }
    if( --m_refcount==0 ) 
        delete this; 
}

void StaticObjectsContainer::setFullScreenMode(bool mode)
{
    m_fullScreenMode = mode;
}

bool StaticObjectsContainer::fullScreenMode(void){
    return m_fullScreenMode;
}

RenderTheme* StaticObjectsContainer::theme()
{
    if(!m_symbianTheme)
        m_symbianTheme = new RenderThemeSymbian();
    return m_symbianTheme;
}
}

// END OF FILE
