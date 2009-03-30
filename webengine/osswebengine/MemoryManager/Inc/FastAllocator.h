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

#ifndef __FASTALLOCATOR_H__
#define __FASTALLOCATOR_H__

#include <e32base.h>
#include <e32cmn.h>

class CFastMemoryPool;

class RFastAllocator : public RAllocator
{
public:
	IMPORT_C RFastAllocator(CFastMemoryPool*);
    IMPORT_C ~RFastAllocator();

	IMPORT_C TAny* Alloc(TInt aSize);
	IMPORT_C void Free(TAny* aPtr);
	IMPORT_C TAny* ReAlloc(TAny* aPtr, TInt aSize, TInt aMode=0);
	IMPORT_C TInt AllocLen(const TAny* aCell) const;
	IMPORT_C TInt Compress();
	IMPORT_C void Reset();
	IMPORT_C TInt AllocSize(TInt& aTotalAllocSize) const;
	IMPORT_C TInt Available(TInt& aBiggestBlock) const;
	IMPORT_C TInt DebugFunction(TInt aFunc, TAny* a1=NULL, TAny* a2=NULL);
	IMPORT_C TInt Extension_(TUint aExtensionId, TAny*& a0, TAny* a1);

private:
    TBool IsLocatedInHeap( TAny* aPtr ) const;

    RHeap& iHeap;
    TUint32 iHeapBase;
    CFastMemoryPool* iPool;
  };

#endif //!__FASTALLOCATOR_H__
