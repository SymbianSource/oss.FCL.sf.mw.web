/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Page thumbnail generator class definition
*
*/


// INCLUDE FILES
#include "ThumbnailGenerator.h"

#include <fbs.h>
#include <bitstd.h>
#include <w32std.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================= CLASSES METHODS===============================

// ============================ MEMBER FUNCTIONS ===============================


CThumbnailGenerator* CThumbnailGenerator::NewL(MPageScalerCallback& aCallback)
    {
        CThumbnailGenerator* self = new( ELeave ) CThumbnailGenerator(aCallback);
        
        CleanupStack::PushL( self );
        self->ConstructL();
        CleanupStack::Pop();
        
        return self;
    
    }

CThumbnailGenerator::CThumbnailGenerator(MPageScalerCallback& aCallback)
    : iCallback(&aCallback),
    iThumbnailBitmap(NULL),
    iThumbnailBitmapDevice(NULL),
    iThumbnailBitmapGc(NULL)
    {
    }

void CThumbnailGenerator::ConstructL()
    {
    }

CThumbnailGenerator::~CThumbnailGenerator()
    {
        delete iThumbnailBitmap;
        delete iThumbnailBitmapDevice;
        delete iThumbnailBitmapGc;
    }

TBool CThumbnailGenerator::CreateBitMapL(TSize aSize, CFbsBitmap*& aBm, CFbsBitmapDevice*& aDev, CFbsBitGc*& aGc)
    {
    if ( aSize.iWidth==0 || aSize.iHeight==0 )
        {
        // delete bitmap if there was one
        delete aGc;
        delete aDev;
        delete aBm;
        aGc = 0;
        aDev = 0;
        aBm = 0;
        return EFalse;
        }
    else
        {
        if ( aBm && aSize != aBm->SizeInPixels() )
            {
            // resize if different size
            User::LeaveIfError(aDev->Resize(aSize));
            aGc->Resized();
            }
        else if ( !aBm )
            {
            // create new
            CFbsBitmap* bm = new (ELeave) CFbsBitmap;
            CleanupStack::PushL(bm);
            User::LeaveIfError(bm->Create(aSize,EColor64K));
            CFbsBitmapDevice* dev = CFbsBitmapDevice::NewL(bm);
            CleanupStack::PushL(dev);
            User::LeaveIfError(dev->CreateContext(aGc));
            aDev = dev;
            aBm = bm;
            CleanupStack::Pop(2);
            }
        }
    return ETrue;
    }

void CThumbnailGenerator::CreatePageThumbnailL()
    {
    //Get the thumnail size
    //if in portrait mode, take Thumbnail(60*height, height)
    //In lanadscape mode, take Thumbnail(60*width,width)
    
    TInt thumbnailWidth;
    TInt thumbnailHeight;
    TRect clientRect = iCallback->DocumentViewport();
    if (clientRect.Width() > clientRect.Height())
        {
        thumbnailWidth = clientRect.Width() * 60 / 100;
        thumbnailHeight = clientRect.Width();
        }
    else
        {
        thumbnailWidth = clientRect.Height() * 60 / 100;
        thumbnailHeight = clientRect.Height();
        }
        
    TRect viewRect = TRect(0, 0, thumbnailWidth, thumbnailHeight);
    if (!CreateBitMapL(viewRect.Size(), iThumbnailBitmap, iThumbnailBitmapDevice, iThumbnailBitmapGc))
        {
        return;
        }
    
    iCallback->DrawDocumentPart(*iThumbnailBitmapGc,iThumbnailBitmap,viewRect);
    TSize bitmapSize = iThumbnailBitmap->SizeInPixels();
    iCallback->ScaledPageChanged(viewRect, true, false);
    }


