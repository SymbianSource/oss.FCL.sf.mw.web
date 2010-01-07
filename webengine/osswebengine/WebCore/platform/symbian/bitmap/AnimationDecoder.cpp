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


/*
 *  This class is a proxy for CAnimationDecoderWrapped
 *  
 *  We needed to wrap animation decoding in a proxy because sometimes the cache gets cleared when decoding
 *  is in progress; when that happens the animation gets deleted while it's in the middle (which causes all sorts
 *  of crashes and memory stomping).  Now, the cache can delete the proxy while the animation is decoding; the proxy
 *  will pass on the delete request to the decoder which will cleanup (delete itself) when it's safe to do so.
 *  
 */

// INCLUDE FILES
#include "config.h"
#include "AnimationDecoder.h"

using namespace WebCore;

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
//
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAnimationDecoder::CAnimationDecoder( CAnimationDecoderWrapped* aDecoder )
    : iWrapped(aDecoder)
{
}

// -----------------------------------------------------------------------------
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAnimationDecoder* CAnimationDecoder::NewL( ImageObserver* aObs )
{
    CAnimationDecoderWrapped* aDecoder = CAnimationDecoderWrapped::NewL( aObs );
    CAnimationDecoder* self = new (ELeave) CAnimationDecoder( aDecoder );
    return self;
}

// -----------------------------------------------------------------------------
//
// Symbian constructor can leave.
// -----------------------------------------------------------------------------
void CAnimationDecoder::ConstructL( )
  {
  }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CAnimationDecoder::~CAnimationDecoder()
{
    if (iWrapped) {
        iWrapped->Invalidate();
        iWrapped = NULL;
    }
}

CMaskedBitmap* CAnimationDecoder::Destination() 
{ 
    if (iWrapped)
        return(iWrapped->Destination());
    return(NULL);
}

// -----------------------------------------------------------------------------
// OpenL
// -----------------------------------------------------------------------------
void CAnimationDecoder::OpenL( const TDesC8& aData, TDesC* aMIMEType, TBool aIsComplete )
{
    if (!iWrapped) {
        return;
    }
    TRAPD(err, iWrapped->OpenL(aData, aMIMEType, aIsComplete));
    if (err != KErrNone) {
        delete iWrapped;
        iWrapped = NULL;
    }
    User::LeaveIfError(err);
    
}

TSize CAnimationDecoder::Size() const
{
    if (iWrapped)
        return(iWrapped->Size());
    TSize t;
    return(t);
}

TInt CAnimationDecoder::AnimationFrameCount() const
{
    if (iWrapped)
        return(iWrapped->AnimationFrameCount());
    return(-1);
}

TInt16 CAnimationDecoder::getLoopCount()
{
    if (iWrapped)
        return(iWrapped->getLoopCount());
    return(-1);
}

TBool CAnimationDecoder::isSizeAvailable() const
{
    if (iWrapped)
        return(iWrapped->isSizeAvailable());
    return(EFalse);
}

TInt CAnimationDecoder::LoadFrame(TInt aFrameIndex)
{
    if (iWrapped)
        return(iWrapped->LoadFrame(aFrameIndex));
    return(-1);
}

TBool CAnimationDecoder::decodeInProgress() const
{
    if (iWrapped)
        return(iWrapped->decodeInProgress());
    return(EFalse);
}

//  End of File
