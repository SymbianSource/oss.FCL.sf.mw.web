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
*
*/
#include "FastAllocator.h"
#include "MemoryManager.h"
#include "MemoryPool.h"

EXPORT_C RFastAllocator::RFastAllocator(CFastMemoryPool* aPool) : iHeap( User::Heap() ), iPool( aPool )
{
    iHeapBase = (TUint32)iHeap.Base() & 0xffff0000;
}

EXPORT_C RFastAllocator::~RFastAllocator()
{
}


EXPORT_C TAny* RFastAllocator::Alloc(TInt aSize)
{
    return iPool->Allocate( aSize );
}

EXPORT_C void RFastAllocator::Free(TAny* aPtr)
{
    // make sure we go to the right memory pool
    if( IsLocatedInHeap( aPtr ) )
        iHeap.Free( aPtr );
    else
        iPool->Free( aPtr );
}

EXPORT_C TAny* RFastAllocator::ReAlloc(TAny* aPtr, TInt aSize, TInt aMode)
{
    TAny* p(NULL);
    TInt originalSize(0);
    
    // check the right memory pool
    if( IsLocatedInHeap( aPtr ) )
        {
        // allocate in memory manager's pool
        p = iPool->Allocate( aSize );
        TInt cpSize = iHeap.AllocLen( aPtr );
        originalSize = cpSize;
        cpSize = cpSize < aSize ? cpSize : aSize;
        memcpy( p, aPtr, cpSize );
        iHeap.Free( aPtr );
        }
    else
        {
        originalSize = iPool->MemorySize(aPtr);
        p = iPool->ReAllocate( aPtr, aSize );
        }

    // Handle aMode.
    // ENeverMove: Don't move memory on realloc, return NULL if it was moved.
    // EAllowMoveOnShrink: Allow moving memory on realloc when size requested
    //    is smaller, return NULL if moved and request size is greater (equal).
    if ( (aMode == ENeverMove && p != aPtr) ||
         (aMode == EAllowMoveOnShrink && originalSize >= aSize && p != aPtr) )
        {
        Free(p);
        return NULL;
        }

    return p;
}

EXPORT_C TInt RFastAllocator::AllocLen(const TAny* aCell) const
{
    return iPool->MemorySize( (void*)(aCell) );
}

EXPORT_C TInt RFastAllocator::Compress()
{
    // DO NOTHING
    return 0;
}

EXPORT_C void RFastAllocator::Reset()
{
    // DO NOTHING
}

EXPORT_C TInt RFastAllocator::AllocSize(TInt& aTotalAllocSize) const
{
    // allocated cell size
    aTotalAllocSize = 0;
    return 0;
}

EXPORT_C TInt RFastAllocator::Available(TInt& aBiggestBlock) const
{
    // make sure the caller is not scared of futher allocation
    aBiggestBlock = KMaxTInt;
    return KMaxTUint;
}

EXPORT_C TInt RFastAllocator::DebugFunction(TInt, TAny*, TAny*)
{
    return 0;
}

EXPORT_C TInt RFastAllocator::Extension_(TUint, TAny*&, TAny*)
{
    return 0;
}

TBool RFastAllocator::IsLocatedInHeap( TAny* aPtr ) const
{
    // Quick check first, the initial heap size is usually defined as
    // 0x5000. For memory cell allocated in this heap, it's address 
    // share the same upper 16 bits with heap base address.
    if( ((TUint32)aPtr & 0xffff0000) != iHeapBase ) return EFalse;

    // very rare case, check it anyway
    TUint32 top = iHeapBase + iHeap.Size();
    return ( (TUint32)aPtr >= iHeapBase && (TUint32)aPtr <= top );
}

// END OF FILE
