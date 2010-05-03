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
* Description:  Provides a means for PluginInst's to communicate with thier owner
*
*/



#ifndef CPLUGINBOX_H
#define CPLUGINBOX_H

// INCLUDES
#include <e32def.h>
#include <e32base.h>

#include "nw_gdi_types.h"
#include "nw_lmgr_propertylist.h"
#include "GDIDevicecontext.h"

// FORWARD DECLARATIONS
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct NW_LMgr_RootBox_s NW_LMgr_RootBox_t;
typedef struct NW_LMgr_Box_s NW_LMgr_Box_t;
typedef struct NW_LMgr_FormatContext_s NW_LMgr_FormatContext_t;
typedef struct NW_LMgr_EventHandler_s NW_LMgr_EventHandler_t;
typedef struct NW_Text_Abstract_s NW_Text_t;
typedef struct NW_Image_AbstractImage_s NW_Image_AbstractImage_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

class CCoeControl; 
class MLMgrObjectBoxListener;
class MLMgrBoxDestructionListener;

// CLASS DECLARATION

// TODO: This class should be derived from CMediaBox once the browser is
//       converted to c++.

class CLMgrObjectBox : public CBase
    {
    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        *
        * @param aBox the NW_LMgr_NpiBox it is managing.
        */
        static CLMgrObjectBox* NewL(NW_LMgr_Box_t* aBox);

        /**
        * Creates a NPI box-tree, an active container box with a single NPI box as a child.
        *
        * @param aEventHandler the active container box's event handler.
        * @param aPluginContainerBox upon success, an active container box.
        * @param aPluginBox upon success, the new NPI Box
        * @return TBrowserStatusCode
        */
        static TBrowserStatusCode NewBox(NW_LMgr_EventHandler_t* aEventHandler, 
                NW_LMgr_Box_t** aPluginContainerBox, NW_LMgr_Box_t** aPluginBox);

        /**
        * Destructor.
        */
        virtual ~CLMgrObjectBox();


    public:  // New Methods
        /**
        * Allows the box to be placed on the on a new line and grows
        * the box to the width of "space" if the box doesn't have 
        * a width property.
        *
        * @param aSpace the amount of space available on the line.
        * @param aSplitBox not used.
        * @param aFlags LMgr split flags.
        * @return KBrsrLmgrNoSplit, KBrsrSuccess
        */
        virtual TBrowserStatusCode Split(NW_GDI_Metric_t aSpace, 
                NW_LMgr_Box_t** aSplitBox, NW_Uint8 aFlags);

        /**
        * Calculates the minimum size needed for the box.
        *
        * @param aSize the size of the box.
        * @return void
        */
        virtual void GetMinimumContentSize(NW_GDI_Dimension2D_t& aSize);

        /**
        * Draws the border, background and place holder (while the place
        * holder is shown).
        *
        * @param  aDeviceContext the device context used for drawing.
        * @param  aHasFocus whether the box has focus.
        * @return TBrowserStatusCode
        */
        virtual TBrowserStatusCode Draw(const CGDIDeviceContext& aDeviceContext,
                NW_Bool aHasFocus);

        /**
        * If necessary it positions the CPluginInst to match the box's 
        * bounds and renders the box.
        *
        * @param  aDeviceContext the device context used for drawing.
        * @param  aClipRect the current clip rect.
        * @param  aCurrentBox the current box.
        * @param  aCurrentBox flags used during rendering.
        * @param  aHasFocus whether the box has focus.
        * @param  aSkipChildren whether the children should be skipped.
        * @param  aRootBox the root box.
        * @return TBrowserStatusCode
        */
        virtual TBrowserStatusCode Render(const CGDIDeviceContext& aDeviceContext,
                const NW_LMgr_RootBox_t& aRootBox, const NW_GDI_Rectangle_t& aClipRect, 
                const NW_LMgr_Box_t* aCurrentBox, NW_Uint8 aFlags, NW_Bool& aHasFocus, 
                NW_Bool& aSkipChildren);

        /**
        * Set the box's listener
        *
        * @param aListener the listener
        * @return void
        */
        void SetBoxListener(MLMgrObjectBoxListener* aListener);

        /**
        * Adds a box destruction listener.
        *
        * @param aListener the listener
        * @return void
        */
        void AddBoxDestructionListenerL(MLMgrBoxDestructionListener* aListener);

        /**
        * Removes a box destruction listener.
        *
        * @param aListener the listener
        * @return void
        */
        void RemoveBoxDestructionListener(MLMgrBoxDestructionListener* aListener);

        /**
        * Sets the place holder text.
        *
        * @param aText
        * @return void
        */
        void SetPlaceHolderTextL(const TDesC& aNewText);

        /**
        * Set the place-holder text to the given resource id.
        *
        * @param aResourceId the string's resource id.
        *
        * @return void
        */
        void SetPlaceHolderTextL(TInt aResourceId);

        /**
        * Removes the place holder's text and image.
        *
        * @return void
        */
        void RemovePlaceHolder(void);

        /**
        * Returns ETrue if the place holder is being shown.
        *
        * @return ETure or EFalse
        */
        TBool IsShowingPlaceHolder(void);

        /**
        * Returns if the plugin is active or not.
        *
        * @param .
        * @return if the plugin is active or not.
        */
        TBool IsActive() const;

        /**
        * Each CLMgrObjectBox is contained in a container-box.  In order to identify the 
        * plugin box associated with a given elementNode the NW_CSS_Prop_elementId 
        * CSS property is applied, where the property value equals the element-node.
        * FindBoxTree returns the box that contains a NW_CSS_Prop_elementId with a
        * value matching aBoxKey. 
        *
        * @param aBoxTree the box-tree of the enclosing content handler.
        * @param aBoxKey a pointer to the associated element-node.
        * @return the CLMgrObjectBox's container-box.
        */
        static NW_LMgr_Box_t* FindBoxTree(const NW_LMgr_Box_t& aBoxTree,
                void* aBoxKey);

        /**
        * Each CLMgrObjectBox is contained in a container-box.  FindObjectBox finds 
        * and returns the first NW_LMgr_NpiBox found in aBoxTree.
        *
        * @param aBoxTree the CLMgrObjectBox's container-box.
        * @return the NW_LMgr_NpiBox.
        */
	    static NW_LMgr_Box_t* FindObjectBox(const NW_LMgr_Box_t& aBoxTree);

        /**
        * Returns the given property as a descriptor.
        *
        * @param aBoxTree the CLMgrObjectBox's container-box.
        * @return the NW_LMgr_NpiBox.
        */
        static TDesC* GetTextProperty(const NW_LMgr_Box_t& aBox, 
                NW_LMgr_PropertyName_t aProperty);

        /**
        * Returns the view infrastructure's CCoeControl associated the given root-box.
        *
        * @param aRootBox a root-box.
        * @return the CCoeControl associated the given root-box.
        */
        static CCoeControl* GetParentWindow(const NW_LMgr_RootBox_t& aRootBox);

        /**
        * Returns the OOC ObjectBox.
        *
        * @param .
        * @return the OOC ObjectBox.
        */
        NW_LMgr_Box_t* Box(){return iBox;}


    private:  // Private Methods
        /**
        * C++ default constructor.
        */
        CLMgrObjectBox(NW_LMgr_Box_t* aBox);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(void);

        /**
        * Returns the view infrastructure's CCoeControl associated with this instance.
        *
        * @return the CCoeControl associated with this instance.
        */
        CCoeControl* GetParentWindow(void);

        /**
        * Shows, sizes, and locates the CPluginInst or place holder to correspond with 
        * the box.
        *
        * @param aLocation where the PluginInst should be placed
        * @param aSize the size the PluginInst or place holder should be.
        * @return void
        */
        void AttachPluginInst(const NW_GDI_Point2D_s& aLocation,
                const NW_GDI_Dimension2D_t& aSize);

        /**
        * Detaches the CPluginInst from its parent.
        *
        * @param void
        * @return void
        */
        void DetachPluginInst(void);

        /**
        * Adjusts the location and size of the PluginInst or place holder so it matches iBox's
        * bounds.
        *
        * @param  aDeviceContext the device context used for drawing.
        * @return void
        */
        void SyncPluginInstBounds(const CGDIDeviceContext& aDeviceContext);

        /**
        * Implements the the NPI handling rules for vertical layout
        * 1. Scale the box maintaining the aspect ratio if width is greater than display width
        * 2. Scale the box maintaining the aspect ratio if height is greater than display height
        * 3. If box is too small it doesn't participate in the layout.
        *
        * @param aOrigSize the original size of the box
        * @param aNewSize the new size of the box
        * @return void
        */
        void HandleVerticalLayout(const NW_GDI_Dimension2D_t& aOrigSize, 
               NW_GDI_Dimension2D_t& aNewSize);

        /**
        * Creates a copy of aText such that it width is <= aAvailableSpace -- truncating as necessary.
        *
        * @param  aText the string to be copied.
        * @param  aDeviceContext the device context used for drawing.
        * @param  aAvailableSpace the amount of space available -- the size aText must fit in.
        * @param  aAddDots if true "..." will be appended if aText needs to be clipped.
        * @return a copy of aText whos width is <= aAvailableSpace.
        */
        TDesC* ClipText(const TDesC& aText, const CGDIDeviceContext& aDeviceContext,
                NW_GDI_Metric_t aAvailableSpace, TBool aAddDots);


    private:  // Data
	    NW_LMgr_Box_t*                              iBox;
        CCoeControl*                                iPluginInst;
        TBool                                       iPluginAttached;
        TBool                                       iInHyperLink;

        TBool                                       iShowingPlaceHolder;
        const NW_Image_AbstractImage_t*             iPlaceHolderImage;
        TDesC*                                      iOrigPlaceHolderText;
        TDesC*                                      iPlaceHolderText;
        TBool                                       iCalculatePlaceHolderClip;

        TBool                                       iForceSyncPluginInst;
        NW_GDI_Point2D_t                            iLocation;
        NW_GDI_Dimension2D_t                        iSize;

        MLMgrObjectBoxListener*                     iBoxListener;
        RPointerArray<MLMgrBoxDestructionListener>  iDestructionListeners;
    };

#endif      // CPLUGINBOX_H
