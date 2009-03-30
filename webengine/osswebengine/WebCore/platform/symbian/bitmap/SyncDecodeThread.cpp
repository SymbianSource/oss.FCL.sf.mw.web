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

class CSynDecoder : public CActive
    {
    public:  // Constructors and destructor
        static CSynDecoder* NewL();
        virtual ~CSynDecoder();

    public:
        TInt Open(BmElem* aElem);

    private: // From base class CActive
        void DoCancel();
        void RunL();
        TInt RunError( TInt aError );
        void SignalParent( TInt aError );
        
    private: // Private constructors
        CSynDecoder();
        void ConstructL() {}

    private: // Data
        BmElem* iElem; // not owned
        CImageDecoder* iDecoder; // owned
        CMaskedBitmap* iBitmap; // owned
    };

// FORWARD DECLARATIONS

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
    }

// -----------------------------------------------------------------------------
// OpenL
// -----------------------------------------------------------------------------
TInt CSynDecoder::Open(BmElem* aElem)
{
    iElem = aElem;
    // reset decoder
    TRAPD( err, 
        iDecoder = CImageDecoder::DataNewL(CEikonEnv::Static()->FsSession(), iElem->iData);
        iBitmap = CMaskedBitmap::NewL();
        );
    if( err != KErrNone )
        return err;

    TFrameInfo frameInfo = iDecoder->FrameInfo( 0 );

    if( frameInfo.iFlags & TFrameInfo::ETransparencyPossible ) {
        TDisplayMode maskmode = (frameInfo.iFlags & TFrameInfo::EAlphaChannel) ? EGray256 : EGray2;

        err = iBitmap->Create( frameInfo.iOverallSizeInPixels, EColor64K, maskmode );
    }
    else
        err = iBitmap->Create( frameInfo.iOverallSizeInPixels, EColor64K );
    //
    if( err != KErrNone )
        return err;
   // start decoding
    CFbsBitmap& dstBitmap = iBitmap->BitmapModifyable();
    CFbsBitmap& dstMask = iBitmap->MaskModifyable();

    if( ( frameInfo.iFlags & TFrameInfo::ETransparencyPossible ) && dstMask.Handle() )
        iDecoder->Convert( &iStatus, dstBitmap, dstMask, 0 );
    else {
        dstMask.Reset();
        iDecoder->Convert( &iStatus, dstBitmap, 0 );
    }
    SetActive();
    return KErrNone;
}

// -----------------------------------------------------------------------------
// CSynDecoder::DoCancel
// -----------------------------------------------------------------------------
void CSynDecoder::DoCancel()
{
    iDecoder->Cancel();
    SignalParent( KErrCancel );
}

// -----------------------------------------------------------------------------
// CSynDecoder::RunL
// -----------------------------------------------------------------------------
void CSynDecoder::RunL()
{
    SignalParent( iStatus.Int() );

    if( iStatus.Int() == KErrNone ) {
        iElem->iBitmapHandle = iBitmap->Bitmap().Handle();
        iElem->iMaskHandle = iBitmap->Mask().Handle();

        RThread self;
        self.Suspend(); 
        self.Close();
        // destroy
        CActiveScheduler::Stop();
    }
}

// -----------------------------------------------------------------------------
// CSynDecoder::RunError
// -----------------------------------------------------------------------------
TInt CSynDecoder::RunError(TInt aError)
{
    SignalParent( aError );
    return KErrNone;
}

// -----------------------------------------------------------------------------
// CSynDecoder::RunError
// -----------------------------------------------------------------------------
void CSynDecoder::SignalParent(TInt aError)
{
    RThread parent;
    parent.Open(iElem->iParentThreadId);
    parent.RequestComplete(iElem->iRequestStatus, aError );
    parent.Close();            

    if (aError != KErrNone)
        CActiveScheduler::Stop();
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
	if(iUp) {
		iDecoderThread.Resume();
		iDecoderThread.Kill(KErrNone);  
		iDecoderThread.Close();  
	}
	delete iElem;
}


void CSynDecodeThread::ConstructL()
{
}

TInt CSynDecodeThread::Decode(const TDesC8& aData, TRequestStatus* aRequestStatus)
{
	iElem = new (ELeave) BmElem;
	iElem->iData.Set( aData );
	iElem->iParentThreadId = RThread().Id();
	iElem->iRequestStatus = aRequestStatus;

    TBuf<20> randName;
	TTime t;
	t.HomeTime();
	randName.Num( I64INT(t.Int64()) );
    
	TInt err = iDecoderThread.Create(randName, CSynDecodeThread::ScaleInThread, KDefaultStackSize, KMinHeapSize, KMaxHeapSize, iElem);
    if (err==KErrNone) {
		iUp = ETrue;
        iDecoderThread.SetPriority(EPriorityMore);
        *aRequestStatus = KRequestPending;
        iDecoderThread.Resume(); 
    }
    return err;
}

void CSynDecodeThread::Handle(TInt& aBitmapHandle, TInt& aMaskHandle)
{
    aBitmapHandle = iElem->iBitmapHandle;
    aMaskHandle = iElem->iMaskHandle;    
}

TInt CSynDecodeThread::ScaleInThread(TAny *aPtr)
{
    CTrapCleanup* cleanup = CTrapCleanup::New();
    CActiveScheduler* as = new CActiveScheduler;
    CActiveScheduler::Install(as);  

    CSynDecoder* decoder = NULL;         

    RFbsSession fbs;
    fbs.Connect();

    BmElem* elem = (BmElem*)aPtr;
    TRAPD(err, decoder = CSynDecoder::NewL());

    if (err == KErrNone && (err = decoder->Open(elem))== KErrNone )
        CActiveScheduler::Start();              
    else {    
        RThread parent;
        parent.Open(elem->iParentThreadId);
        parent.RequestComplete(elem->iRequestStatus, err);
        parent.Close();            
    }

    delete as;
    delete decoder; 
    delete cleanup; 
    fbs.Disconnect();       
    return err;
}
