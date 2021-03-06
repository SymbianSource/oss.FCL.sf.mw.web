/*
 * Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include "config.h"
#include "ImageSource.h"
#include "bitmap/AnimationDecoder.h"
#include "bitmap/MaskedBitmap.h"
#include "SharedBuffer.h"

#include "IntSize.h"

namespace WebCore {

/*ImageDecoder* createDecoder(const Vector<char>& data)
{
    // We need at least 4 bytes to figure out what kind of image we're dealing with.
    int length = data.size();
    if (length < 4)
        return 0;

    const unsigned char* uContents = (const unsigned char*)data.data();
    const char* contents = data.data();

    // GIFs begin with GIF8(7 or 9).
    if (strncmp(contents, "GIF8", 4) == 0)
        return new GIFImageDecoder();

    // Test for PNG.
    if (uContents[0]==0x89 &&
        uContents[1]==0x50 &&
        uContents[2]==0x4E &&
        uContents[3]==0x47)
        return new PNGImageDecoder();

    // JPEG
    if (uContents[0]==0xFF &&
        uContents[1]==0xD8 &&
        uContents[2]==0xFF)
        return new JPEGImageDecoder();

    // BMP
    if (strncmp(contents, "BM", 2) == 0)
        return new BMPImageDecoder();

    // ICOs always begin with a 2-byte 0 followed by a 2-byte 1.
    // CURs begin with 2-byte 0 followed by 2-byte 2.
    if (!memcmp(contents, "\000\000\001\000", 4) ||
        !memcmp(contents, "\000\000\002\000", 4))
        return new ICOImageDecoder();
   
    // XBMs require 8 bytes of info.
    if (length >= 8 && strncmp(contents, "#define ", 8) == 0)
        return new XBMImageDecoder();

    // Give up. We don't know what the heck this is.
    return 0;
}*/

ImageSource::ImageSource()
  : m_decoder(0)
{}

ImageSource::~ImageSource()
{
    delete m_decoder;
}

bool ImageSource::initialized() const
{
    return m_decoder;
}

void ImageSource::setMimeType(const String& mimeType)
{
    m_mimeType = mimeType;    
}

const String& ImageSource::getMimeType()
{
    return m_mimeType;    
}
void ImageSource::setDataL(SharedBuffer* data, bool allDataReceived)
{
    // Make the decoder by sniffing the bytes.
    // This method will examine the data and instantiate an instance of the appropriate decoder plugin.
    // If insufficient bytes are available to determine the image type, no decoder plugin will be
    // made.
    if( !allDataReceived ) return;
    if( !m_decoder )
        // sync decoding if no observer is passed
        m_decoder = CAnimationDecoder::NewL( NULL );
    if( m_decoder ) {
        TPtrC8 ptr( (const TUint8*)data->data(), data->size() );
        TPtrC16 ptr16(m_mimeType.des());        
        m_decoder->OpenL( ptr, &ptr16, ETrue );
    }
}

bool ImageSource::isSizeAvailable()
{
    if (!m_decoder)
        return false;

    return m_decoder->isSizeAvailable();
}

IntSize ImageSource::size() const
{
    if (!m_decoder)
        return IntSize();

    return m_decoder->Size();
}

int ImageSource::repetitionCount()
{
    if (!m_decoder)
        return cAnimationNone;

    int loop = m_decoder->getLoopCount();
    if( loop == -1) {
        return KMaxTInt;
    }

    return loop;
}

size_t ImageSource::frameCount() const
{
    if( m_decoder ) {
        return m_decoder->AnimationFrameCount();
    }

    return 0;
}

NativeImagePtr ImageSource::createFrameAtIndex(size_t index)
{
    if (!m_decoder)
        return 0;

    return m_decoder->Destination();
}

float ImageSource::frameDurationAtIndex(size_t index)
{
    if (!m_decoder)
        return 0;
    return (float)(m_decoder->Destination()->FrameDelay().Int()) / 1000000.0f;
}

bool ImageSource::frameHasAlphaAtIndex(size_t index)
{
    CMaskedBitmap* bmp = m_decoder->Destination();
    if( bmp->HasMask() ) {
        return bmp->Mask().DisplayMode() == EGray256; 
    }

    return false;
}

void ImageSource::setDecoderListener(ImageObserver* listener)
{
    if( !m_decoder )
        TRAP_IGNORE( m_decoder = CAnimationDecoder::NewL( listener ) );
}

}
