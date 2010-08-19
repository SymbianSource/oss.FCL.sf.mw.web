/*
* Copyright (c) 2004-2008 Nokia Corporation and/or its subsidiary(-ies).
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
#include <e32std.h> 
#include "SyncDecodeThread.h"
#include "MaskedBitmap.h"
#include <ImageConversion.h>

namespace TBidirectionalState {
    class TRunInfo;
};

#include <EIKENV.H> 

#define KMaxHeapSize 0x1000000

class BmElem 
{
    public:
        TPtrC8 iData;
        TThreadId iParentThreadId;
        TRequestStatus* iRequestStatus;
        TInt iBitmapHandle;
        TInt iMaskHandle;
};

enum DecoderState {ENewDecodeRequest, EDecodeInProgress, EDecoderIdle, EDecoderTerminate};
class CSynDecoder : public CActive
    {
    public:  // Constructors and destructor
        static CSynDecoder* NewL();
        virtual ~CSynDecoder();

    public:
        void Open(const TDesC8& aData, TRequestStatus *status);
        void Lock() { iDecoderLock.Wait(); }
        void Release() { iDecoderLock.Signal(); }
        void Terminate() { iDecodeState = EDecoderTerminate; }

    private: // From base class CActive
        void DoCancel();
        void RunL();
        TInt RunError( TInt aError );
        void SignalParent( TInt aError );
        void StartDecodeL();        
        void SetIdle();
        
    private: // Private constructors
        CSynDecoder();
        void ConstructL();

    private: // Data
        BmElem iElem;
        CImageDecoder* iDecoder;
        CMaskedBitmap* iBitmap;
        RFastLock iDecoderLock;
        DecoderState iDecodeState;

friend class CSynDecodeThread;        
    };

// FORWARD DECLARATIONS
CSynDecoder *CSynDecodeThread::iSyncDecoder = NULL;

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
//
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CSynDecoder::CSynDecoder() : CActive(CActive::EPriorityHigh)
    {
    CActiveScheduler::Add( this );
    }

void CSynDecoder::ConstructL()
{
    User::LeaveIfError(iDecoderLock.CreateLocal());
    SetIdle();
}

CSynDecoder* CSynDecoder::NewL()
    {
    CSynDecoder* self = new (ELeave) CSynDecoder();
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop(); // self
    return self;
    }

CSynDecoder::~CSynDecoder()
    {
    Cancel();
    delete iDecoder;
    delete iBitmap;
    iDecoderLock.Close();
    }

// -----------------------------------------------------------------------------
// Decode - Decode request submitted from client thread
// -----------------------------------------------------------------------------
void CSynDecoder::Open(const TDesC8& aData, TRequestStatus *status)
{
    // FbsSession is needed for parent thread if it doesn't have already
    if(!RFbsSession::GetSession())
        RFbsSession::Connect();

    iElem.iRequestStatus = status;
    iElem.iData.Set(aData); 
    iElem.iParentThreadId = RThread().Id();
    iElem.iBitmapHandle = 0;
    iElem.iMaskHandle = 0;    
    iDecodeState = ENewDecodeRequest;
}

void CSynDecoder::SetIdle()
{
    iDecodeState = EDecoderIdle;
    iElem.iParentThreadId = 0;
    if(!IsActive()) {
        iStatus = KRequestPending;
        SetActive();
    }
}

void CSynDecoder::StartDecodeL()
{
    if(iDecoder) {
        delete iDecoder;
        iDecoder = NULL;
    }
    if(iBitmap) {
        delete iBitmap;
        iBitmap = NULL;
    }
    
    iDecoder = CImageDecoder::DataNewL(CEikonEnv::Static()->FsSession(), iElem.iData);
    iBitmap = CMaskedBitmap::NewL();
    
    TFrameInfo frameInfo = iDecoder->FrameInfo( 0 );
    TInt err = KErrNone;
    if( frameInfo.iFlags & TFrameInfo::ETransparencyPossible ) {
        TDisplayMode maskmode = (frameInfo.iFlags & TFrameInfo::EAlphaChannel) ? EGray256 : EGray2;
        err = iBitmap->Create( frameInfo.iOverallSizeInPixels, EColor64K, maskmode );
    }
    else
        err = iBitmap->Create( frameInfo.iOverallSizeInPixels, EColor64K );
    User::LeaveIfError(err);
    
    // start decoding
    CFbsBitmap& dstBitmap = iBitmap->BitmapModifyable();
    CFbsBitmap& dstMask = iBitmap->MaskModifyable();    
    if( ( frameInfo.iFlags & TFrameInfo::ETransparencyPossible ) && dstMask.Handle() )
        iDecoder->Convert( &iStatus, dstBitmap, dstMask, 0 );
    else {
        dstMask.Reset();
        iDecoder->Convert( &iStatus, dstBitmap, 0 );
    }
    
    iDecodeState = EDecodeInProgress;
    SetActive();
}

// -----------------------------------------------------------------------------
// CSynDecoder::DoCancel
// -----------------------------------------------------------------------------
void CSynDecoder::DoCancel()
{
    iDecoder->Cancel();
    SignalParent( KErrCancel );
    SetIdle();
}

// -----------------------------------------------------------------------------
// CSynDecoder::RunL
// -----------------------------------------------------------------------------
void CSynDecoder::RunL()
{    
    switch(iDecodeState)
    {
    case ENewDecodeRequest:
        StartDecodeL(); // start async decode
        break;
    case EDecodeInProgress:
        if( iStatus.Int() == KErrNone ) {
            iElem.iBitmapHandle = iBitmap->Bitmap().Handle();
            iElem.iMaskHandle = iBitmap->Mask().Handle();        
        }
        
        SignalParent(iStatus.Int());
        SetIdle();
        break;
    case EDecoderTerminate:
        // if any thread is waiting for decode, signal it
        if( iElem.iParentThreadId )
            SignalParent(KErrCompletion);
        CActiveScheduler::Stop();
        break;
    default:
        SetIdle();
    }        
}

// -----------------------------------------------------------------------------
// CSynDecoder::RunError
// -----------------------------------------------------------------------------
TInt CSynDecoder::RunError(TInt aError)
{
    SignalParent(aError);
    SetIdle();
    return KErrNone;
}

// -----------------------------------------------------------------------------
// CSynDecoder::RunError
// -----------------------------------------------------------------------------
void CSynDecoder::SignalParent(TInt aError)
{
    RThread parent;
    parent.Open(iElem.iParentThreadId);
    parent.RequestComplete(iElem.iRequestStatus, aError );
    parent.Close();
}


CSynDecodeThread* CSynDecodeThread::NewL()
{
    CSynDecodeThread* self = new (ELeave) CSynDecodeThread();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

CSynDecodeThread::CSynDecodeThread()
{
}

CSynDecodeThread::~CSynDecodeThread()
{
    // signal the thread to do cleanup and stop
    iSyncDecoder->Terminate();
    TRequestStatus *ps = &(iSyncDecoder->iStatus);
    iDecoderThread.RequestComplete(ps, KErrNone);
    
	iDecoderThread.Close();    
}

void CSynDecodeThread::ConstructL()
{   
    _LIT(KThreadName, "iconDecoder");
    RAllocator &allocator = User::Allocator();
    TInt err = iDecoderThread.Create(KThreadName, CSynDecodeThread::ScaleInThread, KDefaultStackSize, &allocator, NULL);
    if(err == KErrAlreadyExists) {
        // It may happen only in rare cases, if container browserengine..dll was unloaded and loaded again before
        // syncDecodeThread could close itself (scheduled). In that case, we kill that thread since we can't reuse that
        // and create a new one with same name.
        RThread th;
        th.Open(KThreadName);
        th.Kill(KErrNone);
        th.Close();

        // Leave this time if problem still exists
        User::LeaveIfError(iDecoderThread.Create(KThreadName, CSynDecodeThread::ScaleInThread, KDefaultStackSize, &allocator, NULL));
    }

    iDecoderThread.SetPriority(EPriorityMore);
    TRequestStatus status = KRequestPending;
    iDecoderThread.Rendezvous(status);
    iDecoderThread.Resume();
    User::WaitForRequest(status);
    User::LeaveIfError(status.Int());
}


TInt CSynDecodeThread::Decode(const TDesC8& aData)
{
    iSyncDecoder->Lock();
    
    //notify decoder thread about new request
    TRequestStatus status = KRequestPending;
    iSyncDecoder->Open(aData, &status);
    TRequestStatus *ps = &(iSyncDecoder->iStatus);
    iDecoderThread.RequestComplete(ps, KErrNone);

    //wait for decode complete
    User::WaitForRequest(status);    
    
    iSyncDecoder->Release();
    return status.Int();    
}

void CSynDecodeThread::Handle(TInt& aBitmapHandle, TInt& aMaskHandle)
{
    aBitmapHandle = iSyncDecoder->iElem.iBitmapHandle;
    aMaskHandle = iSyncDecoder->iElem.iMaskHandle;    
}

TInt CSynDecodeThread::ScaleInThread(TAny *aPtr)
{
    CTrapCleanup* cleanup = CTrapCleanup::New();
    CActiveScheduler* as = new CActiveScheduler;
    CActiveScheduler::Install(as);  
    RFbsSession fbs;
    fbs.Connect();

    // start event loop
    TRAPD(err, iSyncDecoder = CSynDecoder::NewL());
    if (err == KErrNone) {
        // wait for decode request from client threads
        RThread().Rendezvous(KErrNone);
        CActiveScheduler::Start();
    }
    else {
        RThread().Rendezvous(err);
    }
    
    // thread shutdown 
    delete as;
    delete iSyncDecoder; 
    delete cleanup; 
    fbs.Disconnect();       
    return err;
}
