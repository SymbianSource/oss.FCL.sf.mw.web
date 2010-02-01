/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*      View images popup box
*      
*
*/


#include "ImageMapPopup.h"
#include <browser_platform_variant.hrh>
#include <e32math.h>
#include <apmstd.h>
#include <bitdev.h>
#include <gulicon.h>
#include <akniconarray.h>

#include <avkon.mbg>
#include <avkon.rsg>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
const TInt KImageMapBitmapMaxWidth = 43;
const TInt KImageMapBitmapMaxHeight = 32;
const TInt KImageMapPopupWidth = 176;
const TInt KImageMapPopupHeight = 144;
const TInt KImageMapPopupWidth2 = 20;
const TInt KImageMapMaxAreaNum = 6;
_LIT( KImageMapAltTextTab, "\t" );
// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
   
// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CImageMapListBox::CImageMapListBox
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CImageMapListBox::CImageMapListBox() : CAknDoubleLargeGraphicPopupMenuStyleListBox()
  {
  }

// -----------------------------------------------------------------------------
// CImageMapListBox::~CImageMapListBox
// destructor.
// -----------------------------------------------------------------------------
//
CImageMapListBox::~CImageMapListBox()
  {
  }

// -----------------------------------------------------------------------------
// CImageMapListBox::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CImageMapListBox::ConstructL( CCoeControl& aParent, CArrayPtrFlat<CFbsBitmap>& aBitmapList, 
                                   CArrayPtrFlat<HBufC>& aAltTextList )
  {
  // call base class
  CAknDoubleLargeGraphicPopupMenuStyleListBox::ConstructL( &aParent, 0 );
  // create icon array which shows up in the left column
  CArrayPtrFlat<CGulIcon>* iconList = new( ELeave) CAknIconArray( 5 );       
  CleanupStack::PushL( iconList );

  CDesCArray* itemList = STATIC_CAST( CDesCArray*, Model()->ItemTextArray() );     

  // create icons out of bitmaps
  HBufC* tempBuf;
  TSize bitmapSize;
  CGulIcon* icon;
  CFbsBitmap* bitmap;

  TInt bitmapCount( aBitmapList.Count() );
  for( TInt i = 0; i < bitmapCount; i++ )
    {
    // create icons
    bitmap = aBitmapList.At( i );
    bitmapSize = bitmap->SizeInPixels();
    // check if the bitmap needs to be squeezed
    if( bitmapSize.iWidth > KImageMapBitmapMaxWidth || bitmapSize.iHeight > KImageMapBitmapMaxHeight )
      {
      CFbsBitmap* modifiedBitmap = new( ELeave )CFbsBitmap();
      CleanupStack::PushL( modifiedBitmap );

      User::LeaveIfError( modifiedBitmap->Create( TSize( KImageMapBitmapMaxWidth, KImageMapBitmapMaxHeight ), EColor4K ) );
      // create bitmap device
      CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL( modifiedBitmap );
      CleanupStack::PushL( bitmapDevice );

      // create graphics context for bitmap device
      CGraphicsContext* bitmapContext;
      User::LeaveIfError( bitmapDevice->CreateContext( bitmapContext ) );
      CleanupStack::PushL( bitmapContext );

      // squeze it
      TSize targetSize = Fit( bitmapSize, modifiedBitmap->SizeInPixels() );

      // copy bitmap to modified bitmap
      bitmapContext->DrawBitmap( TRect( TPoint( 0,0 ), targetSize ), bitmap, TRect( TPoint( 0,0 ), bitmapSize ) );

      CleanupStack::PopAndDestroy( 2 );   // bitmapContext, bitmapDevice
      
      // replace bitmap in the array
      delete  bitmap;
      aBitmapList.At( i ) = modifiedBitmap;
      // bitmap array takes ownership
      CleanupStack::Pop(); // modifiedBitmap

      bitmap = modifiedBitmap;
      }
    // create icon. CGulIcon takes bitmap ownership
    icon = CGulIcon::NewL( bitmap, bitmap );
    // remove bitmap array ownership
    aBitmapList.At( i ) = NULL;
    CleanupStack::PushL( icon );
    
    // iconlist takes icon ownership
    iconList->AppendL( icon );
    CleanupStack::Pop();    // icon

    // add alt text too    
    // alt text build up as follows
    // 0/talt text1
    // 1/talt text2
    // 2/talt text3
    tempBuf = HBufC::NewLC( 
      // max num is 999999
      KImageMapMaxAreaNum +
      aAltTextList.At( i )->Des().Length() + 
      KImageMapAltTextTab().Length() );

    tempBuf->Des().AppendNum( i );
    tempBuf->Des().Append( KImageMapAltTextTab );
		tempBuf->Des().Append( aAltTextList.At( i )->Des() );
    itemList->AppendL( tempBuf->Des() );
    CleanupStack::PopAndDestroy(); // tempBuf   
    }
  // pass iconList ownership
  ItemDrawer()->ColumnData()->SetIconArray( iconList );
  CleanupStack::Pop(); // iconList 
  }

// -----------------------------------------------------------------------------
// CImageMapListBox::Fit
// Checks if the source bitmap can fit into the target bitmap
// -----------------------------------------------------------------------------
//
TSize CImageMapListBox::Fit( const TSize& aSource, const TSize& aTarget )
  {
  TSize result1( aTarget );
  TSize result2( aTarget );

  TInt dA( aTarget.iWidth - aSource.iWidth );
  TInt dB( aTarget.iHeight - aSource.iHeight );
  //lint -e{790} Suspicious truncation, integral to float)
  TReal tmp = aSource.iHeight + ((TReal)(aSource.iHeight * dA) / (TReal)aSource.iWidth);
  TReal res;
  Math::Round( res, tmp, 0);
  result1.iHeight = (TInt)res;
  //lint -e{790} Suspicious truncation, integral to float)
  tmp = aSource.iWidth + ((TReal)(aSource.iWidth * dB) / (TReal)aSource.iHeight);
  Math::Round( res, tmp, 0);
  result2.iWidth = (TInt)res;

  TSize result;
  if(result1.iWidth <= aTarget.iWidth && result1.iHeight <= aTarget.iHeight)
    {
    result = result1;
    }
  else
    {
    result = result2;
    }
  return result;
  }


// -----------------------------------------------------------------------------
// CImageMapPopup::CImageMapPopup
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CImageMapPopup::CImageMapPopup()
  {
  }

// -----------------------------------------------------------------------------
// CImageMapPopup::~CImageMapPopup
// destructor.
// -----------------------------------------------------------------------------
//
CImageMapPopup::~CImageMapPopup()
  {
  delete iSkinContext;
  // delete lists.
  if(iBitmapList)
    {
      iBitmapList->ResetAndDestroy();
    }
  delete iBitmapList;
  // iAltTextList->ResetAndDestroy();
  // delete iAltTextList;
  }

// -----------------------------------------------------------------------------
// CImageMapPopup::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CImageMapPopup::ConstructL( CEikListBox *aListBox, TInt aCbaResource,
                                 AknPopupLayouts::TAknPopupLayouts aType )
  {
  // call base class construct
  CAknPopupList::ConstructL( aListBox, aCbaResource, aType );
  
  TAknsItemID tileIID = KAknsIIDSkinBmpListPaneNarrowA;
  // these values should come as consts (from LAF).
  AknLayoutUtils::SAknLayoutRect tile = { 0, 0, 0, ELayoutEmpty, ELayoutEmpty, KImageMapPopupWidth2, KImageMapPopupHeight };
	AknLayoutUtils::SAknLayoutRect bg = { 0, 0, 0, ELayoutEmpty, ELayoutEmpty, KImageMapPopupWidth, KImageMapPopupHeight };

  if( AknLayoutUtils::LayoutMirrored( ) )
    {
    tile.iL = ELayoutEmpty;
    tile.iR = 0;
    }
  
  // these values should come as consts (from LAF)
  TRect clientRect( 0, 0, KImageMapPopupWidth, KImageMapPopupHeight );
  TAknLayoutRect tileRect;
  TAknLayoutRect bgRect;
  
  tileRect.LayoutRect( clientRect, tile );	
  bgRect.LayoutRect( clientRect, bg );
  
  iSkinContext = CAknsListBoxBackgroundControlContext::NewL(
    KAknsIIDSkinBmpMainPaneUsual, 
    bgRect.Rect(), 
    EFalse, tileIID,
    tileRect.Rect() ); 
  }

// -----------------------------------------------------------------------------
// CImageMapPopup::CreateAndRunL
// Executes the popup dialog
// -----------------------------------------------------------------------------
//
TInt CImageMapPopup::CreateAndRunL( CArrayPtrFlat<CFbsBitmap>& aBitmapList, CArrayPtrFlat<HBufC>& aAltTextList )
  {
  TInt selectedItem( KErrNotFound );
  // construct dialog
  CImageMapPopup* self = new( ELeave )CImageMapPopup();
  CleanupStack::PushL( self );
  // take lists ownership
  self->iBitmapList = &aBitmapList;
  self->iAltTextList = &aAltTextList;
                       
  // construct image listboxs
  CImageMapListBox* listBox = new( ELeave )CImageMapListBox();
  CleanupStack::PushL( listBox );

  // construct dialog
  self->ConstructL( listBox, R_AVKON_SOFTKEYS_OK_BACK__OK, AknPopupLayouts::EMenuDoubleLargeGraphicWindow );
  
  // construct listbox
  listBox->ConstructL( *self, aBitmapList, aAltTextList );
  listBox->CreateScrollBarFrameL( ETrue );
  listBox->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
  listBox->SetObserver( self );

  HBufC* title = HBufC::NewLC( 200 ); // must come from resource CEikonEnv::Static()->AllocReadResourceL(  );
  title->Des().Copy( _L("Image map areas") );
  self->SetTitleL( *title );
  CleanupStack::PopAndDestroy(); // title
  
  // launch dialog
  if( self->ExecuteLD() )
    {
    selectedItem = listBox->CurrentItemIndex();
    }
  CleanupStack::PopAndDestroy(); // listbox
  CleanupStack::Pop(); // self
  return selectedItem;
  }

// -----------------------------------------------------------------------------
// CImageMapPopup::HandleListBoxEventL
// 
// -----------------------------------------------------------------------------
//
void CImageMapPopup::HandleListBoxEventL( CEikListBox* /*aListBox*/, TListBoxEvent aEventType )
  {
#ifdef BRDO_SINGLE_CLICK_ENABLED_FF  	
  if(aEventType == MEikListBoxObserver::EEventItemDoubleClicked || 
     aEventType == MEikListBoxObserver::EEventEnterKeyPressed || aEventType == EEventItemSingleClicked)
#else
	if(aEventType == MEikListBoxObserver::EEventItemDoubleClicked || 
     aEventType == MEikListBoxObserver::EEventEnterKeyPressed)
#endif     	
    {
    AttemptExitL( ETrue );
    }
  return;
  }

// -----------------------------------------------------------------------------
// CImageMapPopup::HandleControlEventL
// 
// -----------------------------------------------------------------------------
//
void CImageMapPopup::HandleControlEventL( CCoeControl* /*aControl*/, TCoeEvent /*aEventType*/ )
  {
  // overwrite base implementation as it does change CBAs to Mark and Cancel
  }

// -----------------------------------------------------------------------------
// CImageMapPopup::MopSupplyObject
// 
// -----------------------------------------------------------------------------
//
TTypeUid::Ptr CImageMapPopup::MopSupplyObject( TTypeUid aId )
  {
  if( aId.iUid == MAknsControlContext::ETypeId /*&& iUseSkinContext*/ )
    {
    return MAknsControlContext::SupplyMopObject( aId, iSkinContext );
    }
  return SupplyMopObject( aId, (MAknEditingStateIndicator*)NULL );
  }

// End of File
