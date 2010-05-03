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
*      Declaration of images map popup and listbox.
*
*
*/


#ifndef __IMAGEMAPPOPUP_H
#define __IMAGEMAPPOPUP_H

#include <e32base.h>
#include <e32def.h>
#include <aknPopup.h>
#include <aknlists.h>
#include <apparc.h>
#include <aknconsts.h>
#include <aknEditStateIndicator.h>
#include <AknsListBoxBackgroundControlContext.h>

class CCoeControl;
class TCoeEvent;
class CEikListBox;
class TListBoxEvent;
class CGulIcon;
class CAknsListBoxBackgroundControlContext;

// use large icon list box with double item text
class CImageMapListBox : public CAknDoubleLargeGraphicPopupMenuStyleListBox
  {
  public:
    
    /**
    * C++ default constructor.
    */
    CImageMapListBox();

    /**
    * Destructor.
    */
    virtual ~CImageMapListBox();

    /**
    * Symbian 2nd phase constructor.
    */
    void ConstructL( CCoeControl& aParent, CArrayPtrFlat<CFbsBitmap>& aBitmapList, 
                     CArrayPtrFlat<HBufC>& aAltTextList );

  private:

    /**
    * Fit
    * @since 2.1
    * @param aSource: source size
    * @param aSource: target size
    * @return TSize target
    */
    TSize Fit( const TSize& aSource, const TSize& aTarget );
    };

// ------------------------------------------------------------------------------------

class CImageMapPopup : public CAknPopupList
  {  
  public:
    
    /**
    * CreateAndRunL
    * @since 2.1
    * @param aBitmapList: bitmap list
    * @param aAltTextList: alt text list
    * @return selected item (-1 if cancel was selected)
    */
    static TInt CreateAndRunL( CArrayPtrFlat<CFbsBitmap>& aBitmapList, CArrayPtrFlat<HBufC>& aAltTextList );

    /**
    * Destructor.
    */
    virtual ~CImageMapPopup();

    /**
    * HandleListBoxEventL
    * @since 2.1
    * @param aListBox: list box
    * @param aEventType: event type
    * @return void
    */
    void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );

    /**
    * HandleControlEventL
    * @since 2.1
    * @param aControl: coe control
    * @param aEventType: event type
    * @return void
    */
    void HandleControlEventL( CCoeControl* aControl, TCoeEvent aEventType );

  private: // New functions

    /**
    * C++ default constructor.
    */
    CImageMapPopup();

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL( CEikListBox *aListBox, TInt aCbaResource, 
      AknPopupLayouts::TAknPopupLayouts aType = AknPopupLayouts::EMenuWindow );
  
  private: // from MObjectProvider
    
    /**
    * MopSupplyObject
    * @since 2.1
    * @param aId: id
    * @return TTypeUid::Ptr
    */
    TTypeUid::Ptr MopSupplyObject( TTypeUid aId );	

  private: // members

  	CAknsListBoxBackgroundControlContext* iSkinContext; // owned
    CArrayPtrFlat<CFbsBitmap>*            iBitmapList;  // owned
    CArrayPtrFlat<HBufC>*                 iAltTextList; // owned
  };

#endif // __IMAGEMAPPOPUP_H

// End of file
