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


// INCLUDE FILES
#include "config.h"
#include "AnimationDecoder.h"
#include "MaskedBitmap.h"
#include "ImageObserver.h"
#include "SyncDecodeThread.h"
#include "Oma2Agent.h"
using namespace ContentAccess;

namespace TBidirectionalState {
    class TRunInfo;
};

#include <eikenv.h>

// constants

// Private namespace for constants and functions
namespace
  {
  // Panic function
  void Panic( TInt aPanicCode ) { User::Panic( _L("AnimationDecoder"), aPanicCode ); }
  }

using namespace WebCore;
CSynDecodeThread *CAnimationDecoder::iSyncDecodeThread  = NULL;

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
//
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAnimationDecoder::CAnimationDecoder( ImageObserver* aObs )
    :CActive( CActive::EPriorityIdle )
    , iObserver(aObs)
    , iLoopCount( -1 )
    , iCurLoopCount( -1 )
    , iSyncBitmapHandle(-1)
    , iSyncMaskHandle(-1)
    , iDecodeInProgress(ETrue)
{
    if (CActiveScheduler::Current())
        CActiveScheduler::Add( this );    
}

// -----------------------------------------------------------------------------
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAnimationDecoder* CAnimationDecoder::NewL( ImageObserver* aObs )
{
    CAnimationDecoder* self = new (ELeave) CAnimationDecoder( aObs );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop(); // self
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
    Cancel();

    if( iDecoder ) {
        // animated images still being decoded.
        iDecoder->Cancel();
        delete iDecoder, iDecoder = NULL;
    }

    delete iAnimationBitmap, iAnimationBitmap = NULL;
    delete iDestination, iDestination = NULL;
    if(iDrmContent) 
    {
        delete iDrmContent;
        iDrmContent = NULL;
    }
}

// -----------------------------------------------------------------------------
// OpenL
// -----------------------------------------------------------------------------
void CAnimationDecoder::OpenAndDecodeSyncL( const TDesC8& aData )
{    
    iSizeAvailable = EFalse;
    iRawDataComplete = ETrue;
    delete iDestination;
    iDestination = NULL;
    
    if(!iSyncDecodeThread) { // first time, create decoder thread
        iSyncDecodeThread = CSynDecodeThread::NewL();
    }
    
    if (iSyncDecodeThread->Decode(aData) == KErrNone) {
        iSyncDecodeThread->Handle(iSyncBitmapHandle, iSyncMaskHandle); 
        Destination(); // duplicate bitmap handles
        iSizeAvailable = ETrue;
    }        
}

CMaskedBitmap* CAnimationDecoder::Destination() 
{ 
    if (iDestination)
        return iDestination; 
        
    if (iSyncBitmapHandle != -1 && iSyncMaskHandle != -1) {
        CFbsBitmap* bitmap = new CFbsBitmap();
        bitmap->Duplicate(iSyncBitmapHandle);
        CFbsBitmap* mask = new CFbsBitmap();
        mask->Duplicate(iSyncMaskHandle);

        iDestination = new CMaskedBitmap(bitmap, mask);
        iDestination->SetFrameIndex(0);
        iDestination->SetFrameDelay(0);
        iSyncBitmapHandle = -1;
        iSyncMaskHandle = -1;
    }
    
    return iDestination;
}
//=============================================================================
// DecodeDRMImageContentL : Function for handling the DRM image content
//=============================================================================
HBufC8* CAnimationDecoder::DecodeDRMImageContentL(const TDesC8& aData)
{
    // input buffers for image conversion
    HBufC8* bufInput = HBufC8::NewLC( aData.Length() + 1 );
    TPtr8 ptrInput = bufInput->Des();
    //Reader intends to view content
    ptrInput.Append( EView );
    ptrInput.Append( aData );

    // output buffer for image conversion
    HBufC8* animatedDRMdata = HBufC8::NewLC( aData.Length() + 256 );
    TPtr8 ptrOutput = animatedDRMdata->Des();

  //Find DRM agent
    TAgent agentDRM;
    
    RArray<ContentAccess::TAgent> agents;
    ContentAccess::CManager* manager = CManager::NewLC();
    manager->ListAgentsL( agents );
    for ( TInt i = 0; i < agents.Count(); i++ )
      {
    if ( agents[i].Name().Compare( KOmaDrm2AgentName ) == 0)
      {
       agentDRM = agents[i];
       //convert the DRM image
       manager->AgentSpecificCommand( agentDRM, EDecryptOma1DcfBuffer, ptrInput,ptrOutput);
       break;
      }
    }

    CleanupStack::PopAndDestroy(manager);
    //keep animatedDRMdata to return
    CleanupStack::Pop(animatedDRMdata);
    CleanupStack::PopAndDestroy(bufInput);

    return animatedDRMdata;
    }
// -----------------------------------------------------------------------------
// OpenL
// -----------------------------------------------------------------------------
void CAnimationDecoder::OpenL( const TDesC8& aData, TDesC* aMIMEType, TBool aIsComplete )
{
    if(!iObserver) {
        OpenAndDecodeSyncL(aData);
        return;
    }
        
    
    delete iDestination;
    iDestination = NULL;
    iDestination = CMaskedBitmap::NewL();

    HBufC8* mime = 0;
    TPtrC8 buffer(aData.Ptr(),aData.Length());
    if (aMIMEType) {
        // it is safer to ignore the server supplied mime type and just recognize
        // the image type from the data headers. this does not work for all formats though
        if ( *aMIMEType==KMimeWBMP || *aMIMEType==KMimeOTA || *aMIMEType==KMimeWMF){
            // convert to 8 bit
            mime = HBufC8::NewLC(aMIMEType->Length());
            mime->Des().Copy(*aMIMEType);
        }
        if( *aMIMEType==KMimeDRM )
        {
        iDrmContent = DecodeDRMImageContentL(aData);
        
        TInt drmContentLength = iDrmContent->Des().Length();
        buffer.Set( (const TUint8*)iDrmContent->Des().Ptr(), drmContentLength);
        }
    }
         
    if( !iDecoder )
        iDecoder = CBufferedImageDecoder::NewL(CEikonEnv::Static()->FsSession());

    if (mime){
        iDecoder->OpenL(buffer,*mime,CImageDecoder::EOptionNone);
        CleanupStack::PopAndDestroy(); // mime
    }
    else {
        iDecoder->OpenL(buffer,CImageDecoder::EOptionNone);
    }

    iRawDataComplete = aIsComplete;

    if(iDecoder && iDecoder->ValidDecoder()  && iDecoder->IsImageHeaderProcessingComplete())
        StartDecodingL();
    else
        // remove me when incremental image rendering gets supported
        User::Leave( KErrCorrupt );

    //If it is an animated image, let's figure out loop count
    if(IsAnimation()) {
        // first see if have a netscape 2.0 extension header 
        const TUint8 extString[] = { 'N', 'E', 'T', 'S', 'C', 'A', 'P','E','2','.','0','\3','\1' };
        const TInt sizeofextString = sizeof(extString);
        TPtrC8 rawDataPtr((TUint8*)aData.Ptr(), aData.Length());
        TInt offset = rawDataPtr.Find(extString, sizeofextString);
        if(offset != KErrNotFound) {
            // found a header, get the loop count -
            // (the loop count is in the 2 bytes following the header string listed above,
            // stored low byte then high byte)
            iLoopCount = (TInt16)((rawDataPtr[offset+sizeofextString+1] * 256) + rawDataPtr[offset+sizeofextString]);
            if(iLoopCount != 0) {
                ++iLoopCount;  // +1 to make it 1 based rather than 0 based 
            }
            else{
            // 0 indicates infinite - map to internal loop count infinite value 
                iLoopCount = -1;
            }
        }
        else {
            // no header found, assume 1x thru loop 
            iLoopCount = 1;
        }
        iCurLoopCount = iLoopCount;
    }
}

// -----------------------------------------------------------------------------
// CAnimationDecoder::AddDataL
// New chunk of raw data
//
// -----------------------------------------------------------------------------
//
void CAnimationDecoder::AddDataL(
    const TDesC8& aNextChunk,
    TBool aIsComplete )
{
    iRawDataComplete = aIsComplete;

    if( iDecoder ) {
        iDecoder->AppendDataL(aNextChunk);
        if( iDecoder->ValidDecoder() ) {
            //  if the image conversion is busy , then just appending the
            // data should be sufficient
            if(iStatus == KRequestPending) {
                // more image data
                iDecoder->ContinueConvert( &iStatus );
                SetActive();
            }
        }
        else {
            iDecoder->ContinueOpenL() ;
            if(iDecoder->ValidDecoder()  && iDecoder->IsImageHeaderProcessingComplete()){
                StartDecodingL();
            }
        }
    }
}

// -----------------------------------------------------------------------------
// CImageLoader::StartDecodingL
//
// -----------------------------------------------------------------------------
//
void CAnimationDecoder::StartDecodingL()
{
    // Check frame count
    iAnimationFrameCount = iDecoder->FrameCount();
    iAnimation = iAnimationFrameCount > 1;
    iFrameInfo = iDecoder->FrameInfo( 0 );
    iSizeAvailable = ETrue;

    if (iFrameInfo.iFlags & TFrameInfo::ETransparencyPossible){
        // we only support gray2 and gray256 tiling
        TDisplayMode maskmode = ( (iFrameInfo.iFlags & TFrameInfo::EAlphaChannel) && (iFrameInfo.iFlags & TFrameInfo::ECanDither)) ? EGray256 : EGray2;
        TInt error = iDestination->Create( iFrameInfo.iOverallSizeInPixels, DisplayMode(), maskmode );

        if (!error)
            LoadFrame(0);
        else
            RunError(KErrNoMemory);
    }
    else {
        TInt error = iDestination->Create( iFrameInfo.iOverallSizeInPixels, DisplayMode() );
        if (!error)
            LoadFrame(0);
        else
            RunError(KErrNoMemory);
    }
}

// -----------------------------------------------------------------------------
// CAnimationDecoder::MaskDisplayMode
// -----------------------------------------------------------------------------
TDisplayMode CAnimationDecoder::MaskDisplayMode() const
{
    if( iFrameInfo.iFlags & TFrameInfo::ETransparencyPossible ){
        if( iFrameInfo.iFlags & TFrameInfo::EAlphaChannel && (iFrameInfo.iFlags & TFrameInfo::ECanDither))
            return EGray256;
    return EGray2;
    }
    return ENone;
}

// -----------------------------------------------------------------------------
// CAnimationDecoder::AnimationFrameDelay
// -----------------------------------------------------------------------------
TTimeIntervalMicroSeconds32 CAnimationDecoder::AnimationFrameDelay( TInt aAnimationFrameIndex ) const
{
    __ASSERT_ALWAYS( aAnimationFrameIndex >= 0 &&
    aAnimationFrameIndex < iAnimationFrameCount, Panic( KErrArgument ) );

    return I64INT( iDecoder->FrameInfo( aAnimationFrameIndex ).iDelay.Int64() );
}


// -----------------------------------------------------------------------------
// CAnimationDecoder::DoCancel
// -----------------------------------------------------------------------------
void CAnimationDecoder::DoCancel()
{
    iDecoder->Cancel();
    // Delete all processed bitmaps
    ErrorCleanup();
    // Complete with cancel
    iImageState = EInactive;
}

// -----------------------------------------------------------------------------
// CAnimationDecoder::RunL
// -----------------------------------------------------------------------------
void CAnimationDecoder::RunL()
{
    __ASSERT_DEBUG( iDestination, Panic( KErrGeneral ) );
    // don't kick off the image decoding until the preview mode is over
    if (iStatus==KErrUnderflow) {
        if (!IsAnimation())
            iObserver->partialImage();
        return;
    }
    else if( iStatus == KErrCorrupt ) {
        RunError( iStatus.Int() );
        return;
    }
    User::LeaveIfError( iStatus.Int() );
    switch( iImageState ) {
        case EStartLoad:
            {
            // start loading the bitmaps
            StartLoadL();
            break;
            }
        case ECompleteLoad:
            {
            // complete loading the bitmaps
            CompleteLoadL();
            break;
            }
        default:
            {
            Panic( KErrTotalLossOfPrecision );
            }
    }
}

// -----------------------------------------------------------------------------
// CAnimationDecoder::RunError
// -----------------------------------------------------------------------------
TInt CAnimationDecoder::RunError( TInt aError )
{
    // Delete all processed bitmaps
    ErrorCleanup();
    // Complete with error
    iImageState = EInactive;
    iObserver->decoderError(aError);
    return KErrNone;
}

// -----------------------------------------------------------------------------
// CAnimationDecoder::LoadFrame
// -----------------------------------------------------------------------------
TInt CAnimationDecoder::LoadFrame( TInt aFrameIndex )
{
    if( IsBusy() )
        return KErrNotReady;

    if( aFrameIndex < 0 || aFrameIndex >= iDecoder->FrameCount() )
        return KErrArgument;

    iFrameIndex = aFrameIndex;
    // Start the active object
    iImageState = EStartLoad;
    SelfComplete();
    return KErrNone;
}

// -----------------------------------------------------------------------------
// CAnimationDecoder::StartLoadL
// -----------------------------------------------------------------------------
void CAnimationDecoder::StartLoadL()
{
    __ASSERT_DEBUG( !iAnimationBitmap, Panic( KErrGeneral ) );

    if( iAnimation ) {
        // Start animation from first frame by default
        iAnimationFrameIndex = 0;

        // Check is animation can be continued on top of destination bitmap
        if( iDestination->FrameIndex() < iFrameIndex )
          iAnimationFrameIndex = iDestination->FrameIndex() + 1;

        StartLoadAnimationBitmapL( iAnimationFrameIndex );
    }
    else // normal image
        StartLoadNormalBitmap( iFrameIndex );
    
    iDecodeInProgress = EFalse;    
    iImageState = ECompleteLoad;
    SetActive();
}

// -----------------------------------------------------------------------------
// CAnimationDecoder::StartLoadNormalBitmap
// -----------------------------------------------------------------------------
void CAnimationDecoder::StartLoadNormalBitmap( TInt aFrameIndex )
{
    CFbsBitmap& dstBitmap = iDestination->BitmapModifyable();
    CFbsBitmap& dstMask = iDestination->MaskModifyable();

    if( MaskDisplayMode() != ENone && dstMask.Handle() )
        iDecoder->Convert( &iStatus, dstBitmap, dstMask, aFrameIndex );
    else {
        dstMask.Reset();
        iDecoder->Convert( &iStatus, dstBitmap, aFrameIndex );
    }
}

// -----------------------------------------------------------------------------
// CAnimationDecoder::StartLoadAnimationBitmapL
// -----------------------------------------------------------------------------
void CAnimationDecoder::StartLoadAnimationBitmapL( TInt aFrameIndex )
{
    __ASSERT_DEBUG( !iAnimationBitmap, Panic( KErrGeneral ) );

    // Create animation bitmap
    iAnimationBitmap = CMaskedBitmap::NewL();
    CFbsBitmap& animBitmap = iAnimationBitmap->BitmapModifyable();
    CFbsBitmap& animMask = iAnimationBitmap->MaskModifyable();

    TFrameInfo frameInfo( iDecoder->FrameInfo( aFrameIndex ) );
    User::LeaveIfError( animBitmap.Create(
    frameInfo.iOverallSizeInPixels, EColor16M ) );

    TDisplayMode maskDisplayMode( ENone );

    if( frameInfo.iFlags & TFrameInfo::ETransparencyPossible ) {
        if( frameInfo.iFlags & TFrameInfo::EAlphaChannel && (frameInfo.iFlags & TFrameInfo::ECanDither)) 
            maskDisplayMode = EGray256;
        maskDisplayMode = EGray2;

        User::LeaveIfError( animMask.Create( frameInfo.iOverallSizeInPixels, maskDisplayMode ) );
        iDecoder->Convert( &iStatus, animBitmap, animMask, aFrameIndex );
    }
    else
        iDecoder->Convert( &iStatus, animBitmap, aFrameIndex );
}

// -----------------------------------------------------------------------------
// CAnimationDecoder::CompleteLoadL
// -----------------------------------------------------------------------------
void CAnimationDecoder::CompleteLoadL()
{
    TSize frameSize = iFrameInfo.iOverallSizeInPixels;
    int sizeinBytes = frameSize.iWidth * frameSize.iHeight * 2;
    if( iAnimationBitmap ){
        // Copy animation bitmap to destination
        BuildAnimationFrameL();
        delete iAnimationBitmap;
        iAnimationBitmap = NULL;

        iDestination->SetFrameIndex( iAnimationFrameIndex );
        iDestination->SetFrameDelay( AnimationFrameDelay( iAnimationFrameIndex ) );

        if( iAnimationFrameIndex < iFrameIndex ) {
            // re-start the active object and load next frame
            iAnimationFrameIndex++;
            iImageState = EStartLoad;
            SelfComplete();
        }
        else {
            // Animation ready
            iImageState = EInactive;
            iObserver->animationFrameReady(sizeinBytes);
        }
    }
    else {
        // Save source info destination
        iDestination->SetFrameIndex( iFrameIndex );
        iDestination->SetFrameDelay( 0 );

        // Normal image ready
        //iDestination = NULL;
        iImageState = EInactive;
        iObserver->imageReady(sizeinBytes);
        delete iDecoder, iDecoder = NULL;
    }
}

// -----------------------------------------------------------------------------
// CAnimationDecoder::BuildAnimationFrameL
// -----------------------------------------------------------------------------
void CAnimationDecoder::BuildAnimationFrameL()
    {
    __ASSERT_DEBUG( iAnimationBitmap, Panic( KErrGeneral ) );
    const CFbsBitmap& animBitmap = iAnimationBitmap->Bitmap();
    const CFbsBitmap& animMask = iAnimationBitmap->Mask();
    __ASSERT_DEBUG( animBitmap.Handle(), Panic( KErrGeneral ) );


		//If the first frame starts from position(0,0), copy directly to the destination bitmap 
		//otherwise frame has to be appropriately positioned in the destination bitmap  
    TPoint aStartPoint(0,0);
    if( (iAnimationFrameIndex==0) && (iFrameInfo.iFrameCoordsInPixels.iTl==aStartPoint) )
    {
        // First frame can be directly put into destination
        User::LeaveIfError( iDestination->Copy( animBitmap, animMask, ETrue ) );
    }
    else {
        CFbsBitmap& prevBitmap = iDestination->BitmapModifyable();
        CFbsBitmap& prevMask = iDestination->MaskModifyable();

        // Other frames must be build on top of previous frames
        __ASSERT_DEBUG( prevBitmap.Handle(), Panic( KErrGeneral ) );

        // Create bitmap device to destination bitmap
        CFbsBitGc* bitGc;
        CFbsBitmapDevice* bitDevice = CFbsBitmapDevice::NewL( &prevBitmap );
        CleanupStack::PushL( bitDevice );
        User::LeaveIfError( bitDevice->CreateContext( bitGc ) );
        CleanupStack::PushL( bitGc );

        // Restore area in destination bitmap if needed
        TRect restoreRect;
        TBool restoreToBackground( EFalse );
        
        TInt aFrameNo = (iAnimationFrameIndex >= 1)?(iAnimationFrameIndex):1;
        TFrameInfo prevFrameInfo(iDecoder->FrameInfo(aFrameNo - 1));
        
        //TFrameInfo prevFrameInfo( iDecoder->FrameInfo( iAnimationFrameIndex - 1 ) );
        
        if( (prevFrameInfo.iFlags & TFrameInfo::ERestoreToBackground )|| (iAnimationFrameIndex ==0)) 
        {
            restoreToBackground = ETrue;
            restoreRect = prevFrameInfo.iFrameCoordsInPixels;
            bitGc->SetPenColor( prevFrameInfo.iBackgroundColor );
            bitGc->SetBrushColor( prevFrameInfo.iBackgroundColor );
            bitGc->SetBrushStyle( CGraphicsContext::ESolidBrush );
            if(iAnimationFrameIndex ==0){
              bitGc->Clear(); 		
            }
            else{
               bitGc->DrawRect( restoreRect ); 	
            }
            bitGc->SetBrushStyle( CGraphicsContext::ENullBrush );
         }
        // Copy animation frame to destination bitmap
        TFrameInfo frameInfo( iDecoder->FrameInfo( iAnimationFrameIndex) );
        if( animMask.Handle() ) {
            bitGc->BitBltMasked( frameInfo.iFrameCoordsInPixels.iTl, &animBitmap,
            animBitmap.SizeInPixels(), &animMask, EFalse );
        }
        else {
            bitGc->BitBlt( frameInfo.iFrameCoordsInPixels.iTl, &animBitmap,
            animBitmap.SizeInPixels() );
        }
        CleanupStack::PopAndDestroy( 2 ); // bitmapCtx, bitmapDev

        // Combine masks if any
        if( prevMask.Handle() && animMask.Handle() ) {
            bitDevice = CFbsBitmapDevice::NewL( &prevMask );
            CleanupStack::PushL( bitDevice );
            User::LeaveIfError( bitDevice->CreateContext( bitGc ) );
            CleanupStack::PushL( bitGc );

            if( restoreToBackground ) {
                bitGc->SetBrushColor( KRgbBlack );
                bitGc->SetBrushStyle( CGraphicsContext::ESolidBrush );
                if(iAnimationFrameIndex ==0){
                   bitGc->Clear(); 		
                 }
                 else{
                   bitGc->DrawRect( restoreRect ); 	
                 }
                bitGc->SetBrushStyle( CGraphicsContext::ENullBrush );
            }
            CFbsBitmap* tmpMask = new(ELeave) CFbsBitmap;
            CleanupStack::PushL( tmpMask );
            User::LeaveIfError( tmpMask->Create( prevMask.SizeInPixels(), prevMask.DisplayMode() ) );
            CFbsBitmapDevice* tmpMaskDev = CFbsBitmapDevice::NewL( tmpMask );
            CleanupStack::PushL( tmpMaskDev );
            CFbsBitGc* tmpMaskGc;
            User::LeaveIfError( tmpMaskDev->CreateContext( tmpMaskGc ) );
            CleanupStack::PushL( tmpMaskGc );

            tmpMaskGc->BitBlt( TPoint( 0, 0 ), &prevMask, frameInfo.iFrameCoordsInPixels );

            bitGc->BitBltMasked( frameInfo.iFrameCoordsInPixels.iTl, &animMask,
            animMask.SizeInPixels(), tmpMask, ETrue );

            CleanupStack::PopAndDestroy( 5 ); //tmpMask, tmpMaskDev, tmpMaskGc, bitGc, bitDevice
        }
    else
        prevMask.Reset(); // Mask not valid anymore -> reset
    }
}

// -----------------------------------------------------------------------------
// CAnimationDecoder::ErrorCleanup
// -----------------------------------------------------------------------------
void CAnimationDecoder::ErrorCleanup()
{
    if( iAnimationBitmap ) {
        delete iAnimationBitmap;
        iAnimationBitmap = NULL;
    }

    if( iDestination ) {
        delete iDestination;
        iDestination = NULL;
    }
}

// -----------------------------------------------------------------------------
// CAnimationDecoder::SelfComplete
// -----------------------------------------------------------------------------
void CAnimationDecoder::SelfComplete( TInt aError )
{
    SetActive();
    iStatus = KRequestPending;
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, aError );
}



//  End of File
