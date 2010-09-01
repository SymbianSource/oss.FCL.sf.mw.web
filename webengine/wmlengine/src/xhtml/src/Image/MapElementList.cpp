/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Hold different map element objects
*
*/


// INCLUDE FILES
#include <e32def.h>  // First to avoid NULL redefine warning (no #ifndef NULL).
#include "MapElementList.h"
#include "TKeyArrayPtr.h"
#include "nw_text_ucs2.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CMapElementEntry::CMapElementEntry
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CMapElementEntry::CMapElementEntry( ) 
  {
  }

// -----------------------------------------------------------------------------
// CMapElementEntry::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMapElementEntry::ConstructL( TUint16* aMapName )
  {
  // make a copy of the aMapName
  iMapName = NW_Str_Newcpy( aMapName );
  User::LeaveIfNull( iMapName );
  }

// -----------------------------------------------------------------------------
// CMapElementEntry::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMapElementEntry* CMapElementEntry::NewL( TUint16* aMapName )
  {
  CMapElementEntry* self = new( ELeave )CMapElementEntry();

  CleanupStack::PushL( self );
  self->ConstructL( aMapName );
  CleanupStack::Pop();

  return self;
  }

// -----------------------------------------------------------------------------
// CMapElementEntry::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMapElementEntry* CMapElementEntry::NewLC( TUint16* aMapName )
  {
  CMapElementEntry* self = CMapElementEntry::NewL( aMapName );
  CleanupStack::PushL( self );

  return self;
  }

// -----------------------------------------------------------------------------
// CMapElementEntry::~CMapElementEntry
// Destructor.
// -----------------------------------------------------------------------------
CMapElementEntry::~CMapElementEntry()
  {
  NW_Mem_Free ((void*)iMapName);
  
  if( iImageMapBoxList )
    {
    // do not call ResetAndDestroy as
    // the ImageMapboxes are not owned by
    // this list
    iImageMapBoxList->Reset();
    }
  delete iImageMapBoxList;
	}

// -----------------------------------------------------------------------------
// CMapElementEntry::SetDomNode
// Sets the dom node for a given map name
// -----------------------------------------------------------------------------
void CMapElementEntry::SetDomNode( NW_DOM_ElementNode_t* aDomNode )
  {
  iDomNode = aDomNode;
  }

// -----------------------------------------------------------------------------
// CMapElementEntry::AddImageMapBoxL
// Adds an image box for a given map name
// -----------------------------------------------------------------------------
TBrowserStatusCode CMapElementEntry::AddImageMapBox( NW_LMgr_ImageMapBox_t* aImageMapBox )
  {
  if( aImageMapBox )
    {
    // do not create image map box list at construct time as 
    // we create a lots of lookup entry which do need this list at all.
    // so create it only if it's necessary
    if( !iImageMapBoxList )
      {
      // create a list
      TRAPD( ret, iImageMapBoxList = new(ELeave) CArrayPtrFlat<NW_LMgr_ImageMapBox_t>(2) );
      if( ret == KErrNoMemory )
        {
        return KBrsrOutOfMemory;
        }
      }
    TRAPD( ret, iImageMapBoxList->AppendL( aImageMapBox ) );    
    if( ret == KErrNoMemory )
      {
      return KBrsrOutOfMemory;
      }
    }  
  return KBrsrSuccess;
  }


// -----------------------------------------------------------------------------
// CMapElementList::CMapElementList
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CMapElementList::ConstructL()
{
  // initialize the imageList 
  iMapElementList = new (ELeave) CMapElementListArray(2);  
}

// -----------------------------------------------------------------------------
// CMapElementList::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
CMapElementList* CMapElementList::NewL()
  {
  CMapElementList* self = new( ELeave ) CMapElementList();
  
  CleanupStack::PushL( self );
  self->ConstructL();
  CleanupStack::Pop();

  return self;
  }

// -----------------------------------------------------------------------------
// CMapElementList::~CMapElementList
// Destructor.
// -----------------------------------------------------------------------------
CMapElementList::~CMapElementList()
  {
  if(iMapElementList)
    {
    iMapElementList->ResetAndDestroy();
    }
  delete iMapElementList;
  }

// -----------------------------------------------------------------------------
// CMapElementList::SetDomNode
// Sets the dom node for a given map name
// -----------------------------------------------------------------------------
TBrowserStatusCode 
CMapElementList::SetDomNode( TUint16* aMapName, NW_DOM_ElementNode_t* aDomNode )
  {
  return Set( aMapName, aDomNode, NULL );
  }

// -----------------------------------------------------------------------------
// CMapElementList::SetImageMapBox
// Sets an image map box for a given map name
// -----------------------------------------------------------------------------
TBrowserStatusCode 
CMapElementList::SetImageMapBox( TUint16* aMapName, NW_LMgr_ImageMapBox_t* aImageMapBox )
  {
  return Set( aMapName, NULL, aImageMapBox );
  }

// -----------------------------------------------------------------------------
// CMapElementList::DomNode
// Gets the dom node for a given map name
// -----------------------------------------------------------------------------
NW_DOM_ElementNode_t* CMapElementList::DomNode( TUint16* aMapName )
  {
  NW_DOM_ElementNode_t* domNode = NULL;
  CMapElementEntry* entry = Get( aMapName );  

  if( entry )
    {
    domNode = entry->DomNode();
    }
  return domNode;
  }

// -----------------------------------------------------------------------------
// CMapElementList::ImageMapBoxList
// Gets the image map list for a given map name
// -----------------------------------------------------------------------------
CImageMapBoxList* CMapElementList::ImageMapBoxList( TUint16* aMapName )
  {
  CImageMapBoxList* imageBoxList = NULL;
  CMapElementEntry* entry = Get( aMapName );  

  if( entry )
    {
    imageBoxList = entry->ImageMapBoxList();
    }
  return imageBoxList;
  }

// -----------------------------------------------------------------------------
// CMapElementList::Set
// Sets either a dom node or an image map box for a given map name
// -----------------------------------------------------------------------------
TBrowserStatusCode 
CMapElementList::Set(TUint16* aMapName, NW_DOM_ElementNode_t* aDomNode, NW_LMgr_ImageMapBox_t* aImageMapBox )
  {
  TInt index;
  TBrowserStatusCode status = KBrsrSuccess;
  CMapElementEntry* lookupEntry = NULL;
  
  // create a new map element entry
  TRAPD( ret, lookupEntry = CMapElementEntry::NewL( aMapName ) );
  
  if( ret != KErrNoMemory )
    {
    CleanupStack::PushL( lookupEntry );
    // if the entry is already in the list then update it otherwise
    // create a new one as insert it.  
    TTextKeyArrayPtr matchkey( _FOFF( CMapElementEntry, iMapName ), ECmpNormal16 );
    if( iMapElementList->FindIsq( lookupEntry, matchkey, index ) == 0 )
      {
      // update entry
      CMapElementEntry* entry = iMapElementList->At( index );
      // update either dom node or add a new image map box
      if( aDomNode )
        {
        entry->SetDomNode( aDomNode );
        }
      else if( aImageMapBox )
        {
        status = entry->AddImageMapBox( aImageMapBox );
        // status can be either KBrsrSuccess or KBrsrOutOfMemory
        }      
      CleanupStack::PopAndDestroy(); // lookupEntry 
      } 
    else
      {
      // set the entry in the map
      lookupEntry->SetDomNode( aDomNode );
      lookupEntry->AddImageMapBox( aImageMapBox );
      TRAP( ret, iMapElementList->InsertIsqL( lookupEntry, matchkey ) );
      if( ret == KErrNoMemory )
        {
        CleanupStack::PopAndDestroy(); // lookupEntry
        status = KBrsrOutOfMemory;
        }
      else
        {
        // do not destroy lookupEntry
        CleanupStack::Pop(); // lookupEntry
        }
      }
    }
  else
    {
    status = KBrsrOutOfMemory;
    }  
  return status;  
  }


// -----------------------------------------------------------------------------
// CMapElementList::Get
// Gets the entry for a given map name
// -----------------------------------------------------------------------------
CMapElementEntry* CMapElementList::Get( TUint16* aMapName )
  {
  TInt index;
  CMapElementEntry* entry = NULL;
  CMapElementEntry* lookupEntry = NULL;
  
  TRAPD( status, lookupEntry = CMapElementEntry::NewL( aMapName ) );
  CleanupStack::PushL( lookupEntry );

  if( status == KErrNone )
    {
    TTextKeyArrayPtr matchkey( _FOFF( CMapElementEntry, iMapName ), ECmpNormal16 );
    if( iMapElementList->FindIsq( lookupEntry, matchkey, index ) == 0 )
      {
      entry = iMapElementList->At( index );
      } 
    }
  CleanupStack::PopAndDestroy(); // lookupEntry 
  return entry;
  }
