/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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


#ifndef __STATICOBJECTCONTAINER_H__
#define __STATICOBJECTCONTAINER_H__

#include <Browser_platform_variant.hrh>
#include <e32base.h>
#include <e32std.h>
#include <wtf/Noncopyable.h>
#include <wtf/HashMap.h>
#include <../bidi.h>
#include <eikenv.h>

class ResourceLoaderDelegate;
class LogStream;
class WebIconDatabase;
class WebSurface;
class WebCursor;
class PluginHandler;
class WebCannedImages;
class CBrCtl;
class MWidgetEngineBridge;
class MDeviceBridge;


namespace WebCore {
class RenderThemeSymbian;
class RenderTheme;
typedef enum
    {
    ELowScreenResolution,
    EMediumScreenResolution,
    EHighScreenResolution
    } TScreenResolution;

class PlatformFontCache;
class FormFillController;
class PictographSymbian;
class OOMHandler;
class OOMStopper;

class StaticObjectsContainer : public Noncopyable 
{
public:
    static StaticObjectsContainer* instance();

    PlatformFontCache* fontCache();
    FormFillController* formFillController();
    RFs& fsSession();
    PictographSymbian* pictograph();
    TScreenResolution screenResolution() const { return m_screenRes; }
    ResourceLoaderDelegate* resourceLoaderDelegate();
    WebIconDatabase* sharedIconDatabase();
    LogStream& logStream();
    WebSurface* webSurface();
    WebCursor* webCursor();
    WebCannedImages* webCannedImages();
    PluginHandler* pluginHandler();
    void setPluginFullscreen(bool val) { m_pluginFullscreen = val; }
    bool isPluginFullscreen() { return m_pluginFullscreen; }
#if defined(BRDO_LIW_FF)
    RLibrary& getDeviceBridgeLibL();
#endif     
    RLibrary& getWidgetEngineBridgeLibL();
    RenderTheme* theme();
    
    virtual ~StaticObjectsContainer();

    void ref(CBrCtl& brctl);
    void deref(CBrCtl& brctl);
    void setCapabilities (unsigned int capabilities)    {m_capabilities = capabilities;}
    unsigned int capabilities() {return m_capabilities;}
    CBrCtl* brctl() const;
    const Vector<CBrCtl*>& activeBrowserControls() const;
    void setFullScreenMode(bool mode);
    bool fullScreenMode(void);
    
private:
    static StaticObjectsContainer* gInstance;
    StaticObjectsContainer();

    PlatformFontCache*      m_fontCache;
    FormFillController*     m_formFillController;
    TScreenResolution       m_screenRes;
    PictographSymbian*      m_pictograph;
    ResourceLoaderDelegate* m_resourceLoaderDelegate;
    LogStream*              m_stream;
    WebIconDatabase*        m_icondatabase;
    WebSurface*             m_rendertarget;
    WebCursor*              m_cursor;
    WebCannedImages*        m_cannedimg;
    PluginHandler*          m_pluginhandler;
    int                     m_refcount;
    unsigned int            m_capabilities;
    Vector<CBrCtl*>         m_activeBrCtls;
    OOMHandler*             m_oomHandler;
    OOMStopper*             m_oomStopper;
    bool                    m_fullScreenMode;
    bool                    m_pluginFullscreen;
    RLibrary                m_widgetLibrary;
#if defined(BRDO_LIW_FF)
    RLibrary                m_deviceLibrary;
#endif
		RenderThemeSymbian*			m_symbianTheme;
};

}

#endif //!__STATICOBJECTCONTAINER_H__
