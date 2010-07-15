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

// INCLUDE FILES

#include <MemoryManager.h>
#include "MemoryPool.h"
#include "FastAllocator.h"
#include "MemoryLogger.h"

// CONSTANTS

_LIT( KMemManPanicDes, "MemMan:0"  );

// CLASS DECLARATION

//  initializing a global memory pool.
static CMemoryPool *s_pool = 0;


//-----------------------------------------------------------------------------
// Pool() - a utility function for accessing the right memory pool
//-----------------------------------------------------------------------------
inline CMemoryPool* Pool() 
    {
    if( !s_pool ) {
        // applications using fast memory pool already created the pool at 
        // startup.
        s_pool = new CDefaultMemoryPool();
        s_pool->Create();
    }

    return s_pool;
    }

//-----------------------------------------------------------------------------
// MemoryManager::CreateAllocator
//-----------------------------------------------------------------------------
EXPORT_C void MemoryManager::CreateFastAllocator()
    {
    // create the right memory pool
    MEM_LOG_CREATE();    
#ifdef __NEW_ALLOCATOR__
    CMemoryPool *pool = new CNewSymbianHeapPool();
    pool->Create();
    RSymbianDlAllocatorWrapper* allocator = new RSymbianDlAllocatorWrapper((CNewSymbianHeapPool*)pool);
    User::SwitchAllocator(allocator);
    MEM_LOGF(_L8("MemoryManager::CreateFastAllocator - new pool=%x, allocator=%x"), pool, allocator);
#endif
    }

//-----------------------------------------------------------------------------
// MemoryManager::InitAllocator
//-----------------------------------------------------------------------------
EXPORT_C void MemoryManager::InitFastAllocator()
    {
    // Initialize s_pool variable from current allocator, assumption is that main program has already called CreateAllocator()
    // It is special case when this allocator is created in SetupThreadHeap() where can not initialize static data. It also
    // solves problems due to static data destruction in Symbian 9.5.
#ifdef __NEW_ALLOCATOR__    
    RAllocator &aAllocator = User::Allocator();
    RSymbianDlAllocatorWrapper* allocator = (RSymbianDlAllocatorWrapper*) &aAllocator;
    s_pool = allocator->iPool;
    MEM_LOGF(_L8("MemoryManager::InitFastAllocator - s_pool=%x, allocator=%x"), s_pool, allocator);
#endif    
    }

//-----------------------------------------------------------------------------
// MemoryManager::SwitchToFastAllocator
//-----------------------------------------------------------------------------
EXPORT_C RAllocator* MemoryManager::SwitchToFastAllocator()
    {
    // create the right memory pool
    __ASSERT_DEBUG( s_pool == 0, User::Panic( KMemManPanicDes, 0 ) );
    MEM_LOGF(_L8("MemoryManager::SwitchToFastAllocator - s_pool=%x"), s_pool);
#ifdef __NEW_ALLOCATOR__
    s_pool = new CNewSymbianHeapPool();
    s_pool->Create();
    RSymbianDlAllocatorWrapper* allocator = new RSymbianDlAllocatorWrapper((CNewSymbianHeapPool*)s_pool);
    MEM_LOGF(_L8("MemoryManager::SwitchToFastAllocator - new s_pool=%x"), s_pool);
    return User::SwitchAllocator( allocator );
#else
    s_pool = new CFastMemoryPool();
    s_pool->Create();
    RFastAllocator* allocator = new RFastAllocator((CFastMemoryPool*)s_pool);
    return User::SwitchAllocator( allocator );
#endif
    }

//-----------------------------------------------------------------------------
// MemoryManager::InitOOMHandler
//-----------------------------------------------------------------------------
EXPORT_C void MemoryManager::InitOOMDialog()
    {
#ifdef __NEW_ALLOCATOR__
    if (s_pool)
        s_pool->InitOOMDialog();
#endif
    }

//-----------------------------------------------------------------------------
// MemoryManager::ResetOOMDialogDisplayed
//-----------------------------------------------------------------------------
EXPORT_C void MemoryManager::ResetOOMDialogDisplayed()
    {
#ifdef __NEW_ALLOCATOR__
    if (s_pool)
        s_pool->ResetOOMDialog();
#endif
    }

//-----------------------------------------------------------------------------
// MemoryManager::CloseFastAllocator
//-----------------------------------------------------------------------------
EXPORT_C void MemoryManager::CloseFastAllocator(RAllocator* aDefaultAllocator)
    {
#ifdef __NEW_ALLOCATOR__
    RAllocator* allocator = User::SwitchAllocator( aDefaultAllocator );
    delete (RSymbianDlAllocatorWrapper*)(allocator);
#else
    RAllocator* allocator = User::SwitchAllocator( aDefaultAllocator );
    delete (RFastAllocator*)(allocator);
#endif
    }

//-----------------------------------------------------------------------------
// MemoryManager::AddCollector
//-----------------------------------------------------------------------------
EXPORT_C void MemoryManager::AddCollector( MMemoryCollector* aCollector )
    {
    Pool()->AddCollector( aCollector );
    }

//-----------------------------------------------------------------------------
// MemoryManager::RemoveCollector
//-----------------------------------------------------------------------------
EXPORT_C void MemoryManager::RemoveCollector( MMemoryCollector* aCollector )
    {
    Pool()->RemoveCollector( aCollector );
    }

//-----------------------------------------------------------------------------
// MemoryManager::AddStopper
//-----------------------------------------------------------------------------
EXPORT_C void MemoryManager::AddStopper( MOOMStopper* aStopper )
    {
    Pool()->AddStopper( aStopper );
    }

//-----------------------------------------------------------------------------
// MemoryManager::RemoveStopper
//-----------------------------------------------------------------------------
EXPORT_C void MemoryManager::RemoveStopper( MOOMStopper* aStopper )
    {
    Pool()->RemoveStopper( aStopper );
    }

//-----------------------------------------------------------------------------
// MemoryManager::SetNotifier
//-----------------------------------------------------------------------------
EXPORT_C void MemoryManager::SetNotifier( MOOMNotifier* aNotifier )
    {
    Pool()->SetNotifier( aNotifier );
    }

//-----------------------------------------------------------------------------
// MemoryManager::PreCheck
//-----------------------------------------------------------------------------
EXPORT_C TBool MemoryManager::PreCheck( TUint aTotalSize, TUint aMaxBufSize, const TDesC8& aChecker )
    {
    return Pool()->PreCheck( aTotalSize, aMaxBufSize, aChecker );
    }

//-----------------------------------------------------------------------------
// MemoryManager::PostCheck
//-----------------------------------------------------------------------------
EXPORT_C TUint MemoryManager::PostCheck()
    {
    return Pool()->PostCheck();
    }

//-----------------------------------------------------------------------------
// MemoryManager::SetStatus
//-----------------------------------------------------------------------------
EXPORT_C void MemoryManager::SetStatus( TOOMCheckResult aType )
    {
    Pool()->SetStatus( aType );
    }

//-----------------------------------------------------------------------------
// MemoryManager::Status
//-----------------------------------------------------------------------------
EXPORT_C TUint MemoryManager::Status()
    {
    return Pool()->Status();
    }

//-----------------------------------------------------------------------------
// MemoryManager::FreeRam
//-----------------------------------------------------------------------------
EXPORT_C void MemoryManager::FreeRam()
    {
    Pool()->CollectMemory();
    }

//-----------------------------------------------------------------------------
// MemoryManager::RestoreCollectors
//-----------------------------------------------------------------------------
EXPORT_C void MemoryManager::RestoreCollectors()
    {
    Pool()->RestoreCollectors(EOOM_PriorityLow);
    }

//-----------------------------------------------------------------------------
// MemoryManager::SetRescueBufferSize
//-----------------------------------------------------------------------------
EXPORT_C void MemoryManager::SetRescueBufferSize( TInt aSize )
    {
    Pool()->SetRescueBufferSize( aSize );
    }

//-----------------------------------------------------------------------------
// MemoryManager::MemorySize
//-----------------------------------------------------------------------------
EXPORT_C TUint MemoryManager::MemorySize( TAny* aPtr )
    {
    return Pool()->MemorySize( aPtr );
    }


EXPORT_C void MemoryManager::DumpMemoryLogs()
    {
#if defined(OOM_LOGGING) && defined (__NEW_ALLOCATOR__)
    CNewSymbianHeapPool *hPool = (CNewSymbianHeapPool *)Pool();
    hPool->DumpHeapLogs(0);
#endif    
    }
