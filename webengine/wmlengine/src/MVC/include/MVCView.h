/*
* Copyright (c) 2002-2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The CView class is responsible for displaying the box tree,
*                handling user input events such as scrolling and navigating links.
*
*/


#ifndef __MVCVIEW_H
#define __MVCVIEW_H

//  INCLUDES
#include <eikbctrl.h>
#include <eikscrlb.h>

#include <cenrepnotifyhandler.h>

#include <e32std.h>
#include <e32def.h>
#include <f32file.h>
#include <aknpictographinterface.h>
#include <aknpictographdrawerinterface.h>

#include <brctldefs.h>
#include <brctldialogsprovider.h>
#include "BrsrStatusCodes.h"
#include "ImageUtils.h"
#include "LMgrBoxTreeListener.h"
#include "BrsrTypes.h"
#include "nw_gdi_types.h"
#include "nw_evt_scrollevent.h"
#include "CBrowserSettings.h"

#include "BrCtl.h"
#include "MemoryManager.h"
#include "pagescaler.h"


#include "wmlcontrol.h"
template <class T> class CArrayPtrFlat;

// CONSTANTS
const TInt KPanicKmViewPanicBase = -9000;
const TInt KPanicKmViewTlsNotInitialized = KPanicKmViewPanicBase - 1;
const TInt KPanicKmViewSingletonAlreadyCreated = KPanicKmViewPanicBase - 2;
const TInt KPanicKmViewTlsViewPointerIsNull = KPanicKmViewPanicBase - 3;


// MACROS

// DATA TYPES

typedef enum
{
  EKmEvAddToPhoneBook,
  EKmEvMakeCall,
  EKmEvRemoveFileName,
  EKmEvOpenToExternalApp,
  EKmEvDownloadObject
} TKimonoEventType;

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CEikScrollBarFrame;
class CAknNavigationDecorator;
class CPtrC8Array;
class CViewAsyncInit;
class MViewListener;
class CShell;
//class CBrCtl;
class TBrCtlImageCarrier;
class MViewFocusObserver;

// CLASS DECLARATIONS
class MBrCtlStateChangeObserver;


/**
*  CView
*  The CView class is responsible for displaying the box tree, and
*  handling user input events such as scrolling and navigating links.
*
*  @lib browserengine.dll
*  @since 2.x
*/


NONSHARABLE_CLASS(CView) : public CEikBorderedControl, public MCenRepNotifyHandlerCallback,
              public MAknPictographAnimatorCallBack, public MBoxTreeListener, private MPageScalerCallback
{
public:
        static CView* NewL(CCoeControl* aParent, CBrCtl* aBrCtl, TRect& aRect, CWmlControl* aWmlControl );

        ~CView();

        TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);

        TBrCtlDefs::TBrCtlElementType GetFocusedElementType();

        NW_Bool InputElementEditComplete(TBool aInputConfirmed);

        void HandleActivationEvent();

        void GenerateAndProcessEvent(const TKimonoEventType& aEvent);

    /**
    * SetBrowserSettingL
    * Set the setting's value.
    * @since 2.x
    * @param TUint setting - the number of the setting you wish to set.
    *   This corresponds to a value in the TBrowserSettings enum
    * @param TUint value - the value you wish to set for the setting
    *   If the value is true/false use 1/0 otherwise the value must correspond
    *   to a value in either one of the following enumerated types:
    *   TBrowserFontSizeLevel or TBrowserFullScreenValue
    * @return void
    */
        void SetBrowserSettingL(TUint aSetting, TUint aValue);

    /**
    * GetBrowserSettingL
    * Get the setting's value.
    * @since 2.x
    * @param TUint setting - the setting whose value you wish to retrieve
    * @return TUint - value of the setting (as an integer). For true/false
    *    1/0 will be returned. For other settings, the return value will correspond
    *    with one of the following enumerated types: TBrowserFontSizeLevel or
    *    TBrowserFullScreenValue
    */
        TUint GetBrowserSettingL(TUint aSetting) const;

        CArrayFixFlat<TBrCtlImageCarrier>* GetPageImagesL();

    void SwitchVerticalLayout ();


	void DrawPictographsInText(CBitmapContext& aGc, const CFont& aFont, const TDesC& aText, const TPoint& aPosition);

     /**
     * Check to see if we are in image map view
     *
     * @param None
     * @return TBool ETrue if we are in image map view, EFalse otherwise
     */
     inline TBool IsImageMapView() { return iIsImageMapView; };

     /**
     * This method is called if we are in image map view to create an image list
     * that just contains the one image shown in the image map view.
     *
     * @param None
     * @return TBool ETrue if we are in image map view, EFalse otherwise
     */
        void GetViewImageList(void* aDynamicVector);

    // Return the number of active elements in the page
        TUint GetActiveElements();

	/**
     *
     * @param box
     * @return  NW_Bool NW_TRUE, if vox is just placeholder (0,0;0x0)
	 * NW_FALSE otherwise;
     */
	 NW_Bool
    IsZeroBox (const NW_LMgr_Box_t* box) const;
    // Redraw the positioned box on top every time that something was redrawn
    TBrowserStatusCode DrawPositionedBoxes();

    // Any slow browser initialization can happen here
    void AsyncInit();

    TBrowserStatusCode Draw (const NW_Bool);

public: // from MBrowserSettingsObserver
    void BrowserSettingChanged( enum TBrowserSetting aBrowserSetting );

protected: // From CCoeControl
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);

    void FocusChanged(TDrawNow aDrawNow);

    void SizeChanged();

    void Draw(const TRect& aRect) const;

    TInt CountComponentControls() const;

    CCoeControl* ComponentControl(TInt aIndex) const;

protected:
	  void HandleResourceChange(TInt aType);

private: // from MSharedDataNotifyHandler

    void HandleNotifyString(TUint32 aKeyId, const TDesC16& aValue);

private: // from MAknPictographDrawerCallBack
    void DrawPictographArea();

protected:
    TKeyResponse DoHandleControlKeysL(const TKeyEvent& aKeyEvent);

private:    // from MPageScalerCallback

    void DrawDocumentPart( CFbsBitGc& aGc, CFbsBitmap* /*aBitmap*/, const TRect& aDocumentAreaToDraw );

    TRect DocumentViewport() ;

	void ScaledPageChanged( const TRect& aRect, TBool aFullScreen, TBool aScroll );

	TSize DocumentSize();

    TBool TouchScrolling() { return EFalse; }
    
    CCoeControl& PageControlView() { return *(iBrCtl->CCoeControlParent());}


private:
    CView(CBrCtl* aBrCtl);

    void ConstructL(CCoeControl* aParent, TRect& aRect, CWmlControl* aWmlControl );

    TBrowserStatusCode Cancel ();

    void CreateOffscreenBitmapL( const TRect& aRect );

    // Any slow browser initialization can happen here
    void AsyncInitL();

    // Delete entries in a given directory
    void DeleteDirectoryEntriesL(RFs& aRfs, TPtrC16& aFilePath);

    NW_Bool DispatchEvent (const NW_Evt_Event_t* event);

    TBrowserStatusCode FormatPartial ();

    NW_LMgr_Box_t* GetNextSiblingBox (const NW_LMgr_Box_t* box);

    TBrowserStatusCode InitializeTree (NW_LMgr_RootBox_t *rootBox, TBool isFormatingBoxTree);

	//
    void InitializePageScalerL();

public: // but not exported. For Kimono internal use ("C" side)

    void SetFormattingPriority( CActive::TPriority aPriority );

    TInt AppendFocusObserver(MViewFocusObserver* aViewFocusObserver);

    TInt AddStateChangeObserver(MBrCtlStateChangeObserver* aBrCtlStateChangeObserver);

    void BoxTreeDestroyed ();

    TBool ValidateAddedIndex(NW_LMgr_Box_t* aBox, NW_LMgr_Box_t* aMasterBox);

    TBrowserStatusCode BoxTreeListenerAppendTabItem (NW_LMgr_Box_t* box);

    TBrowserStatusCode BoxTreeListenerRelayout (NW_Bool createTabList);

    TBrowserStatusCode Collapse (NW_LMgr_FlowBox_t* containingFlow, NW_Bool collapseAnonymous);

    TBrowserStatusCode CreateTabIndexList ();

    void DoScrollL(const TInt16 aScrollDirection, const TUint32 aScrollDelta);

    void EnableSuppressDupReformats (NW_Int16 state);

    void EpocDrawNow (NW_Bool drawNow);

    NW_Bool EpocProcessEvent (const NW_Evt_Event_t* event);

    void ExternalLayoutSwitch ();

    TBrowserStatusCode ForceCurrentBox (NW_LMgr_Box_t* newbox, NW_Bool isNewPage, NW_Bool isRefreshNeeded);

    TBrowserStatusCode FormatBoxTree (NW_Bool createTabList);

    void FormattingComplete ();

    NW_ADT_Vector_Metric_t GetActiveBoxId ();

    NW_LMgr_Box_t* GetInFocusBox () const;

    NW_LMgr_Box_t* GetLastVisibleBox () const;

    NW_LMgr_Box_t* GetNextEventListener (NW_LMgr_BoxVisitor_t* boxVisitor);

    const NW_GDI_Point2D_t* GetOrigin ();

    void* GetScroll () const;

    TBrowserStatusCode GetScrollRange (NW_GDI_Point2D_t* scrollRange);

    NW_LMgr_Box_t* GetVisibleBox () const;

    NW_LMgr_Box_t* GetVisibleBoxReverse () const;

    NW_Bool GetXScrollAmt (const NW_LMgr_Box_t* box, NW_Int32* scrollDelta, NW_Evt_ScrollEvent_Direction_t* scrollDir, NW_Bool scrollMaxAmt) const;

    NW_Bool GetYScrollAmt (const NW_LMgr_Box_t* box, NW_Int32* scrollDelta, NW_Evt_ScrollEvent_Direction_t* scrollDir, NW_Bool scrollMaxAmt) const;

    TBrowserStatusCode GotoActiveBox (NW_ADT_Vector_Metric_t activeBoxId);

    TBrowserStatusCode GotoBox (NW_LMgr_Box_t* box);

    void HandleError (TBrowserStatusCode status);

    NW_Bool HandleGNavigation (NW_Uint32 tabIndex);

    static TBrowserStatusCode InputInvalid(void* aCEpoc32View, TInt aMin);

    static TBrowserStatusCode InputInvalidL(void* aCEpoc32View, TInt aMin);

    NW_Bool IsBoxVisible (const NW_LMgr_Box_t* box, NW_GDI_Rectangle_t *bounds) const;

    TBrowserStatusCode JumpToLink (const NW_LMgr_Box_t* box, NW_Bool scrollMaxAmt);

    NW_Bool ProcessAccessKey (const NW_Evt_KeyEvent_t* event);

    NW_Bool ProcessEvent (const NW_Evt_Event_t* event);

    TBrowserStatusCode RedrawDisplay (NW_Bool drawNow);

    TBrowserStatusCode ReformatBox (NW_LMgr_Box_t* box);

    TBrowserStatusCode Refresh (NW_LMgr_Box_t* box);

    TBrowserStatusCode Relayout (NW_Bool createTabList);

    void RemoveFocusObserver(MViewFocusObserver* aViewFocusObserver);

    void RemoveStateChangeObserver(MBrCtlStateChangeObserver* aBrCtlStateChangeObserver);

    NW_Bool ScrollAfterCurrentBox (const NW_LMgr_Box_t* box, NW_Bool scrollMaxAmt);

    void SetAppServices(void *appServices); // appservices are of type NW_HED_AppServices

    TInt SetBoxTree(const NW_LMgr_Box_t* boxTree); // return value is TBrowserStatusCode

    TBrowserStatusCode SetCurrentMasterBox ();

    TBrowserStatusCode SetDeviceContext (CGDIDeviceContext* deviceContext);

    TBrowserStatusCode SetDisplayExtents (NW_GDI_Rectangle_t* displayExtents);

    void SetShell(CShell* shell);

    TBrowserStatusCode SetListener (MViewListener* viewListener);

    TBrowserStatusCode SetOrigin (NW_GDI_Point2D_t* origin);

    void StartCSSApply ();

    void DisableAnimation();

    void EnableAnimation();

    TBrowserStatusCode SwitchFromImageMapView ();

    TBrowserStatusCode SwitchToImageMapView ();

    TBrowserStatusCode SwitchVerticalLayout (NW_Bool aFormatBoxTree, NW_Bool aSwitchBox);

    void UpdateCBAs () const;

    void UpdateScrollBarsL(CEikScrollBar::TOrientation aOrientation,const TInt aThumbPos ,const TInt aScrollSpan);

    TBrowserStatusCode UpdateHScrollBar () const;

    TBrowserStatusCode UpdateVScrollBar () const;

    inline NW_LMgr_Box_t* GetCurrentBox () { return iCurrentBox; };

    inline CGDIDeviceContext* GetDeviceContext () { return iDeviceContext; };

    inline void* GetParentWindow () { return this; };

    inline NW_GDI_Dimension2D_t GetScrollAmt () {return iScrollAmt;};

    inline NW_ADT_DynamicVector_t* GetTabList () {return iTabList;};

    inline NW_LMgr_RootBox_t* GetRootBox () { return iRootBox; };

    inline NW_GDI_Dimension2D_t GetVisibleAmt () {return iVisibleAmt;} ;

    inline NW_Bool IsForcedReformatNeeded () { return iForceReformatNeeded; };

    inline void ForcedReformatNeeded () { iForceReformatNeeded = NW_TRUE; };

    inline void SetCurrentBox (NW_LMgr_Box_t* aCurrentBox) { iCurrentBox = aCurrentBox; };

    inline void SetInitialActiveBoxId (NW_ADT_Vector_Metric_t aInitialActiveBoxId) {iInitialActiveBoxId = aInitialActiveBoxId; };

    inline void SetInitialDocPosition (NW_GDI_Metric_t aX, NW_GDI_Metric_t aY) { iInitialDocPosition.x = aX; iInitialDocPosition.y = aY; };

    inline CPageScaler* PageScaler() { return iPageScaler; }

	TBrowserStatusCode SetAnchorName (NW_Text_t *anchorName);

    void ContentSize(TSize& aContentSize) ;
    NW_Bool FormatPositionedBoxes ();
    void ScrollTo(TPoint aPoint);

public:
    CBrCtl* BrCtl() {return iBrCtl;}
    void DrawHistory( CFbsBitGc& aGc, const TRect& aDocumentAreaToDraw ) const;

private:
    TBrowserStatusCode AppendTabItem (NW_LMgr_Box_t* box);

    void DeleteOptionItems ();

    NW_GDI_Rectangle_t GetBoxDisplayBounds (NW_LMgr_Box_t* aBox);

    TBrowserStatusCode GetBoxTabIndex (NW_LMgr_Box_t* box, NW_ADT_Vector_Metric_t* index);

    TBrowserStatusCode HandleFormattingComplete ();

    NW_Bool ProcessFontSizeChangeEvent ();

    NW_Bool ProcessScrollEvent (const NW_Evt_ScrollEvent_t* scrollEvent);

    NW_Bool ProcessTabEvent (const NW_Evt_TabEvent_t* tabEvent);

    void SetBoxFormatBounds (NW_LMgr_Box_t* aBox, NW_GDI_Rectangle_t aBoxBounds);

    TBrowserStatusCode SetupTableFormat (NW_Bool /*relayoutEvent*/);

    TBrowserStatusCode UpdateScrollBars () const;

	void GetParentBox(NW_LMgr_Box_t* parentBox, NW_LMgr_Box_t** pReturnedParentBox);
private:
    CBitmapContext* iBitmapContext;
    CBitmapDevice* iBitmapDevice;
    TInt    iLastRepeats;		// To signify long keypresses
    RPointerArray<MViewListener> iListeners;
    CFbsBitmap* iOffscreenBitmap;
    CShell* iShell;
    CBrCtl* iBrCtl;
	CAknPictographInterface* iPictographInterface;

    CPageScaler*    iPageScaler;



    CRepository* iRepository;
    CCenRepNotifyHandler* iNotifyHandler;

    RPointerArray <MViewFocusObserver> iViewFocusObserver;
    RPointerArray <MBrCtlStateChangeObserver> iStateChangeObserver;
    CViewAsyncInit* iViewAsyncInit;
    TInt    iVScrollPosition;

  /* The layout manager context */
  CArrayPtrFlat<NW_LMgr_RootBox_t>*	iRootBoxArray;

  void* iBoxFormatHandler;
  NW_Bool iCreateTabList;
  NW_LMgr_Box_t* iCurrentBox;
  CGDIDeviceContext* iDeviceContext;
  NW_Bool iExternalLayoutSwitch;
  NW_Bool iIsImageMapView;
  NW_ADT_DynamicVector_t* iOptionItemList; /* for clean up purpose */
  NW_ADT_DynamicVector_t* iTabList;
  NW_LMgr_RootBox_t* iRootBox;

  /* Scroll data */
  void* iScroll;
  NW_GDI_Dimension2D_t iMaxScrollAmt;
  NW_GDI_Dimension2D_t iScrollAmt;
  NW_GDI_Dimension2D_t iVisibleAmt;
  TPoint iLastPosition;

  NW_Bool iDrawNow;

  TBool           iDocumentFinished;


  /*the initial box id and initial docposition , is used after
    the formatting is complete */
  NW_ADT_Vector_Metric_t iInitialActiveBoxId;
  NW_GDI_Point2D_t iInitialDocPosition;

  /*
  ** Used to suppress duplicate reformats when a single image is repeated
  ** several times within the same page.
  */
  NW_Int16 iSuppressDupReformats;
  NW_LMgr_Box_t* iPendingReformatBox;
  NW_Bool iForceReformatNeeded;
  NW_Text_t *iAnchorName;
  CActive::TPriority iFormatPriority;
  CWmlControl*           iWmlControl;
  TBool iShouldActivate;
  TBool iDrag;
};

#endif  // EPOC32VIEW_H

// End of File


