/*
    Copyright (C) 1998 Lars Knoll (knoll@mpi-hd.mpg.de)
    Copyright (C) 2001 Dirk Mueller (mueller@kde.org)
    Copyright (C) 2002 Waldo Bastian (bastian@kde.org)
    Copyright (C) 2006 Samuel Weinig (sam.weinig@gmail.com)
    Copyright (C) 2004, 2005, 2006, 2007 Apple Inc. All rights reserved.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "config.h"
#include "CachedImage.h"
#include "StaticObjectsContainer.h"
#include "ResourceLoaderDelegate.h"
#include "HttpCacheManager.h"

#include "BitmapImage.h"
#include "Cache.h"
#include "CachedResourceClient.h"
#include "CachedResourceClientWalker.h"
#include "DocLoader.h"
#include "Frame.h"
#include "Request.h"
#include "SystemTime.h"
#include <wtf/Vector.h>

#if PLATFORM(CG)
#include "PDFDocumentImage.h"
#endif

#if ENABLE(SVG_EXPERIMENTAL_FEATURES)
#if PLATFORM(MAC) || PLATFORM(QT)
#include "SVGImage.h"
#endif
#endif

using std::max;

namespace WebCore {

CachedImage::CachedImage(const String& url)
    : CachedResource(url, ImageResource)
{
    m_image = 0;
    m_status = Unknown;
}

CachedImage::CachedImage(Image* image)
    : CachedResource(String(), ImageResource)
{
    m_image = image;
    m_status = Cached;
    m_loading = false;
}

CachedImage::~CachedImage()
{
    delete m_image;
}

void CachedImage::load(DocLoader* docLoader) 
{ 
    if (!docLoader || docLoader->autoLoadImages()) 
        CachedResource::load(docLoader, true, false, true); 
    else 
        m_loading = false; 
} 

void CachedImage::ref(CachedResourceClient* c)
{
    CachedResource::ref(c);

    if (!imageRect().isEmpty())
        c->imageChanged(this);

    if (!m_loading)
        c->notifyFinished(this);
}

void CachedImage::allReferencesRemoved()
{
    if (m_image && !m_errorOccurred)
        m_image->resetAnimation();
}

static Image* brokenImage()
{
    static OwnPtr<Image*> brokenImage;
    if (!brokenImage) {
        brokenImage.set(Image::loadPlatformResource("missingImage"));
    }
    Image* ret = brokenImage.get();
    return ret;
}

static Image* nullImage()
{
    static BitmapImage nullImage;
    return &nullImage;
}

Image* CachedImage::image() const
{
    if (m_errorOccurred)
        return brokenImage();

    if (m_image)
        return m_image;

    return nullImage();
}

IntSize CachedImage::imageSize() const
{
    return (m_image ? m_image->size() : IntSize());
}

IntRect CachedImage::imageRect() const
{
    return (m_image ? m_image->rect() : IntRect());
}

void CachedImage::notifyObservers()
{
    CachedResourceClientWalker w(m_clients);
    while (CachedResourceClient *c = w.next())
        c->imageChanged(this);
}

void CachedImage::clear()
{
    destroyDecodedData();
    delete m_image;
    m_image = 0;
    setEncodedSize(0);
}



inline void CachedImage::createImage()
{
    // Create the image if it doesn't yet exist.
    if (m_image)
        return;
#if PLATFORM(CG)
    if (m_response.mimeType() == "application/pdf") {
        m_image = new PDFDocumentImage;
        return;
    }
#endif
#if ENABLE(SVG_EXPERIMENTAL_FEATURES)
#if PLATFORM(MAC) || PLATFORM(QT)
    if (m_response.mimeType() == "image/svg+xml") {
        m_image = new SVGImage(this);
        return;
    }
#endif
#endif
    m_image = new BitmapImage(this);

#if PLATFORM(SYMBIAN)                                
    m_image->setMimeType(m_response.mimeType());
#endif

}

void CachedImage::data(PassRefPtr<SharedBuffer> data, bool allDataReceived)
{
    m_data = data;

    createImage();

    bool sizeAvailable = false;

    // Have the image update its data from its internal buffer.
    // It will not do anything now, but will delay decoding until 
    // queried for info (like size or specific image frames).
    sizeAvailable = m_image->setData(m_data, allDataReceived);

    // Go ahead and tell our observers to try to draw if we have either
    // received all the data or the size is known.  Each chunk from the
    // network causes observers to repaint, which will force that chunk
    // to decode.
    if (allDataReceived) {
        if (!m_image || !sizeAvailable || m_image->isNull()) {
            // This case is hit under OOM and lower layer is unable to set sizeAvailable = true
            // even when allDataReceived is True.
            error();
#if PLATFORM(SYMBIAN) 
            TBool found( EFalse );  
            CHttpCacheManager* cacheManager = WebCore::StaticObjectsContainer::instance()->resourceLoaderDelegate()->httpSessionManager()->cacheManager();
            if ( cacheManager )
            {
                // call cache manager to check for url in cache
                TPtrC ptr = url().des();
                HBufC8* aUrl = HBufC8::New(ptr.Length());
                if ( aUrl )
                {
                    aUrl->Des().Copy(ptr);
                    found = cacheManager->Find( *aUrl );
                    if ( found )
                    {
                        cacheManager->RemoveL(*aUrl);
                    } 
                    delete aUrl;
                }
            }
#endif
            if (inCache())
                cache()->remove(this);
            return;
        }
        
        notifyObservers();

        if (m_image)
            setEncodedSize(m_image->data() ? m_image->data()->size() : 0);
    }
    
    if (allDataReceived) {
        m_loading = false;

#if !PLATFORM(SYMBIAN)
        checkNotify();
#endif
    }
}

void CachedImage::error()
{
    clear();
    m_errorOccurred = true;
    notifyObservers();
    m_loading = false;
    checkNotify();
}

void CachedImage::checkNotify()
{
    if (m_loading)
        return;

    CachedResourceClientWalker w(m_clients);
    while (CachedResourceClient* c = w.next())
        c->notifyFinished(this);
}

void CachedImage::destroyDecodedData()
{
    if (m_image && !m_errorOccurred)
        m_image->destroyDecodedData();
}

void CachedImage::decodedSizeChanged(const Image* image, int delta)
{
    if (image != m_image)
        return;
    
    setDecodedSize(decodedSize() + delta);
}

void CachedImage::didDraw(const Image* image)
{
    if (image != m_image)
        return;
    
    double timeStamp = Frame::currentPaintTimeStamp();
    if (!timeStamp) // If didDraw is called outside of a Frame paint.
        timeStamp = currentTime();
    
    CachedResource::didAccessDecodedData(timeStamp);
}

bool CachedImage::shouldPauseAnimation(const Image* image)
{
    if (image != m_image)
        return false;
    
    CachedResourceClientWalker w(m_clients);
    while (CachedResourceClient* c = w.next()) {
        if (c->willRenderImage(this))
            return false;
    }

    return true;
}

void CachedImage::animationAdvanced(const Image* image)
{
    if (image == m_image)
        notifyObservers();
}

#if PLATFORM(SYMBIAN)
void CachedImage::decoderError( int /*error*/ )
{

}

void CachedImage::partialImage()
{
    notifyObservers();
}

void CachedImage::imageReady(int size_)
{
    setDecodedSize(size_);
    notifyObservers();
    checkNotify();
}

void CachedImage::animationFrameReady(int size_)
{
    setDecodedSize(size_);
    notifyObservers();
	checkNotify();
}

void CachedImage::setMimeType(const String& mime_)
{
    m_response.setMimeType(mime_);
}
#endif

} //namespace WebCore
