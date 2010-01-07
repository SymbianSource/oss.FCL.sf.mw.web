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


#ifndef ANIMATIONDECODER_H
#define ANIMATIONDECODER_H

// INCLUDES
#include "AnimationDecoderWrapped.h"

// CLASS DECLARATION
/**
*  CAnimationDecoder
*
*  Proxy for implementation of file based image class.
*  @lib IHL.lib
*  @since 3.0
*  
*  This class is a proxy for CAnimationDecoderWrapped
*  
*  We needed to wrap animation decoding in a proxy because sometimes the cache gets cleared when decoding
*  is in progress; when that happens the animation gets deleted while it's in the middle (which causes all sorts
*  of crashes and memory stomping).  Now, the cache can delete the proxy while the animation is decoding; the proxy
*  will pass on the delete request to the decoder which will cleanup (delete itself) when it's safe to do so.
*/

class CAnimationDecoder
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CAnimationDecoder* NewL(WebCore::ImageObserver* aObs);

        /*
        * Virtual destructor.
        */
        virtual ~CAnimationDecoder();

  public:

        /*
        * From MIHLFileImage, see base class header.
        */
        void OpenL( const TDesC8& aData, TDesC* aMIMEType, TBool aIsComplete );

        /*
        * From MIHLFileImage, see base class header.
        */
        TSize Size() const;

        /*
        * From MIHLFileImage, see base class header.
        */
        TInt AnimationFrameCount() const;
        /*
        * From MIHLFileImage, see base class header.
        */
        TInt LoadFrame( TInt aFrameIndex );

        //funtions related to Loop count of animation image.
        TInt16 getLoopCount();
        CMaskedBitmap* Destination();
        TBool isSizeAvailable() const;
        TBool decodeInProgress() const;

  private: // Private constructors

        CAnimationDecoder(CAnimationDecoderWrapped *aDecoder);
        void ConstructL( );

  private: // Data
      CAnimationDecoderWrapped *iWrapped;
    };

#endif   // CAnimationDecoder_H

// End of File
