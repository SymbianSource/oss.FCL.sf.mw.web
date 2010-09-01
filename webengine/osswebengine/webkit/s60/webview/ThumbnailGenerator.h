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
* Description:   Page thumbnail generator class
*
*/



#ifndef THUMBNAILGENERATOR_H
#define THUMBNAILGENERATOR_H

//  INCLUDES
#include <gdi.h>
#include "PageScaler.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CFbsBitGc;
class CFbsBitmap;
class CFbsBitmapDevice;

class CThumbnailGenerator : public CBase
    {
    public:  // Constructors and destructor
    
        static CThumbnailGenerator* NewL(MPageScalerCallback& aCallback);
        void ConstructL();
        virtual ~CThumbnailGenerator();
        
        void CreatePageThumbnailL();
        CFbsBitmap* PageThumbnail() {return iThumbnailBitmap;}
    
    protected:
        CThumbnailGenerator(MPageScalerCallback& aCallback);
        TBool CreateBitMapL(TSize aSize, CFbsBitmap*& aBm, CFbsBitmapDevice*& aDev, CFbsBitGc*& aGc);
        
    private:
        MPageScalerCallback* iCallback;       
        CFbsBitmap* iThumbnailBitmap; //owned
        CFbsBitmapDevice* iThumbnailBitmapDevice; //owned
        CFbsBitGc* iThumbnailBitmapGc; //owned       
    };

#endif //THUMBNAILGENERATOR_H

// End of File
