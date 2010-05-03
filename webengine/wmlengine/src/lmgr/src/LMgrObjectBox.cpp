/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Box to handle Netscape plugins
*
*/


// INCLUDE FILES
#include <StringLoader.h> 

#include "nwx_settings.h"

#include "LMgrObjectBox.h"
#include "LMgrObjectBoxListener.h"
#include "LMgrObjectBoxOOC.h"

#include "nw_image_cannedimages.h"
#include "nw_lmgr_activecontainerbox.h"
#include "nw_lmgr_containerbox.h"
#include "LMgrBoxTreeListener.h"
#include "nw_lmgr_rootbox.h"
#include "nw_text_ucs2.h"
#include "NW_Text_AbstractI.h"


// ============================ LOCAL DEFINES ==================================
#define KDefaultObjectHeight 100
#define KDefaultObjectWidth 100


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLMgrObjectBox::CLMgrObjectBox
//
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLMgrObjectBox::CLMgrObjectBox(NW_LMgr_Box_t* aBox) :
        
        iBox(aBox),
        iShowingPlaceHolder(ETrue),
        iCalculatePlaceHolderClip(ETrue),
        iDestructionListeners(2)
    {
    NW_ASSERT(aBox != NULL);
    }

// -----------------------------------------------------------------------------
// CLMgrObjectBox::ConstructL
//
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLMgrObjectBox::ConstructL(void)
    {
    }

// -----------------------------------------------------------------------------
// CLMgrObjectBox::~CLMgrObjectBox
//
// Deconstructor.
// -----------------------------------------------------------------------------
//
CLMgrObjectBox::~CLMgrObjectBox()
    {
    // Clean up the place holder
    delete iOrigPlaceHolderText;
    delete iPlaceHolderText;

    // Detach the CPluginInst
    DetachPluginInst();

    // Notify the owners that the box is being destroyed.
    for (TInt i = 0; i < iDestructionListeners.Count(); i++)
        {
        iDestructionListeners[i]->Destroyed();
        }

    iDestructionListeners.Reset();
    }

// -----------------------------------------------------------------------------
// CLMgrObjectBox::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLMgrObjectBox* CLMgrObjectBox::NewL(NW_LMgr_Box_t* aBox)
    {
    CLMgrObjectBox* self = new(ELeave) CLMgrObjectBox(aBox);

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CLMgrObjectBox::NewActiveBox
//
// Creates a Object box-tree, an active container box with a single ObjectBox as a child.
// -----------------------------------------------------------------------------
//
TBrowserStatusCode CLMgrObjectBox::NewBox(NW_LMgr_EventHandler_t* aEventHandler, 
        NW_LMgr_Box_t** aPluginContainerBox, NW_LMgr_Box_t** aPluginBox)
    {
    NW_LMgr_Box_t*  containerBox = NULL;
    NW_LMgr_Box_t*  pluginBox = NULL;

    NW_ASSERT(aPluginContainerBox != NULL);
    NW_ASSERT(aPluginBox != NULL);
    TBool inHyperLink = EFalse;

    NW_TRY (status) 
        {
        if (aEventHandler)
            {
            // Create an ActiveContainer box to hold the boxtree, setting the
            // action type for object box.
            containerBox = (NW_LMgr_Box_t*) NW_LMgr_ActiveContainerBox_New(
                0, aEventHandler, NW_LMgr_ActionType_OpenObject);
            NW_THROW_OOM_ON_NULL(containerBox, status);
            aEventHandler = NULL;
            }
        else
            {
            inHyperLink = ETrue;
            containerBox = (NW_LMgr_Box_t*) NW_LMgr_ContainerBox_New(0);
            NW_THROW_OOM_ON_NULL(containerBox, status);
            }

        // Create the CLMgrObjectBox
        pluginBox = (NW_LMgr_Box_t*) NW_LMgr_ObjectBox_New(0);
        NW_THROW_OOM_ON_NULL(pluginBox, status);
        NW_LMgr_ObjectBox_GetObjectBox((NW_LMgr_ObjectBox_t&)(*pluginBox))->iInHyperLink = inHyperLink;

        // Add the pluginBox to the activeContainerBox
        status = NW_LMgr_ContainerBox_AddChild(NW_LMgr_ContainerBoxOf(containerBox), pluginBox);
        NW_THROW_ON_ERROR(status);
        
        *aPluginBox = pluginBox;
        pluginBox = NULL;
        }

    NW_CATCH (status) 
        {
        if (NW_Object_IsInstanceOf(containerBox, &NW_LMgr_ActiveContainerBox_Class))
            {
            // Orphan aEventHandler before deleting the active container box.
            ((NW_LMgr_ActiveContainerBox_t*) containerBox)->eventHandler = NULL;
            }
        NW_Object_Delete(containerBox);  

        NW_Object_Delete(pluginBox);

        containerBox = NULL;
        *aPluginBox = NULL;
        }
    
    NW_FINALLY 
        {
        *aPluginContainerBox = containerBox;
        return status;
        } NW_END_TRY
    }

// -----------------------------------------------------------------------------
// CLMgrObjectBox::Split
//
// Allows the box to be placed on the on a new line and grows
// the box to the width of "space" if the box doesn't have 
// a width property.
// -----------------------------------------------------------------------------
//
TBrowserStatusCode CLMgrObjectBox::Split(NW_GDI_Metric_t aSpace, 
        NW_LMgr_Box_t** aSplitBox, NW_Uint8 aFlags)
    {
    TBrowserStatusCode status;
    NW_LMgr_PropertyValue_t sizeVal;

    NW_ASSERT(aSplitBox != NULL);

    *aSplitBox = NULL;

    // If box does not fit in the space and the box is not on the new line the 
    // box needs to be pushed on the new line.
    if ((iBox->iFormatBounds.dimension.width > aSpace) && !(aFlags & NW_LMgr_Box_SplitFlags_AtNewLine))
        {
        return KBrsrLmgrNoSplit;
        }

    // If width was specified we do not want to change it.
    status = NW_LMgr_Box_GetPropertyValue(iBox, NW_CSS_Prop_width, NW_CSS_ValueType_Px, &sizeVal);
    if (status == KBrsrSuccess)
        {
        return KBrsrSuccess;
        }

    // Get the box's padding and margin
    NW_LMgr_FrameInfo_t  info;
    TInt                 padding = 0;
    TInt                 border = 0;
    TInt                 margin = 0;

    // NW_LMgr_Box_GetPadding(iBox, &info);
    //padding = info.right;

    // Use the parent's border because it has the border not the object box.
    NW_ASSERT(NW_Object_IsInstanceOf (iBox->parent, &NW_LMgr_ActiveContainerBox_Class) ||
        NW_Object_IsInstanceOf (iBox->parent, &NW_LMgr_ContainerBox_Class));
    NW_LMgr_Box_GetBorderWidth((NW_LMgr_Box_t*) iBox->parent, &info, ELMgrFrameRight );
    border = info.right;

    NW_LMgr_Box_GetMargins(iBox, &info, NULL, ELMgrFrameRight );
    if (info.right != NW_LMgr_FrameInfo_Auto)
        {
        margin = info.right;
        }

    // Set the box's width to the available space minus the border and padding.
    aSpace -= (border + padding + margin);
    iBox->iFormatBounds.dimension.width = aSpace;

    return KBrsrSuccess;
    }

// -----------------------------------------------------------------------------
// CLMgrObjectBox::GetMinimumContentSizeL
//
// Calculates the minimum size needed for the box.
// -----------------------------------------------------------------------------
//
void CLMgrObjectBox::GetMinimumContentSize(NW_GDI_Dimension2D_t& aSize)
    {
    NW_LMgr_PropertyValue_t sizeVal;

    // Set the default size
    aSize.height = KDefaultObjectHeight;
    aSize.width = KDefaultObjectWidth;

    // Adjust the default size if the width or height props are present.
    if (NW_LMgr_Box_GetPropertyValue(iBox, NW_CSS_Prop_width, 
            NW_CSS_ValueType_Px, &sizeVal) == KBrsrSuccess)
        {
        aSize.width = (NW_GDI_Metric_t) sizeVal.integer;
        }

    if (NW_LMgr_Box_GetPropertyValue(iBox, NW_CSS_Prop_height, 
                NW_CSS_ValueType_Px, &sizeVal) == KBrsrSuccess)
        {
        aSize.height = (NW_GDI_Metric_t) sizeVal.integer;
        }

    // If the browser is in small screen mode scale aSize to fit on the screen
    NW_LMgr_RootBox_t* rootBox = NW_LMgr_Box_GetRootBox( iBox );

    if( NW_LMgr_RootBox_GetSmallScreenOn( rootBox ) )
        {
        NW_GDI_Dimension2D_t aOrigSize = aSize;

        // Scale aSize to the screen.
        HandleVerticalLayout(aOrigSize, aSize);
        }
    }

// -----------------------------------------------------------------------------
// CLMgrObjectBox::Draw
//
// Draws the border, background and place holder (while the place
// holder is shown).
// -----------------------------------------------------------------------------
//
TBrowserStatusCode CLMgrObjectBox::Draw(const CGDIDeviceContext& aDeviceContext,
        NW_Bool aHasFocus)
    {
    CGDIDeviceContext*      deviceContext = NULL;
    NW_GDI_Dimension3D_t    imageSize;
    NW_GDI_Rectangle_t      innerRect;
    NW_GDI_Point2D_t        pos;
    NW_GDI_FlowDirection_t  dir = NW_GDI_FlowDirection_LeftRight;
    CGDIFont*               font = NULL;
    NW_GDI_Color_t          oldForgroundColor = 0;
    NW_GDI_Color_t          color;
    NW_LMgr_PropertyValue_t value;

    NW_TRY (status)
        {
        // For convenience.
        deviceContext = CONST_CAST(CGDIDeviceContext*, &aDeviceContext);

        // Get the initial colors -- must be done before any throw statement...
        oldForgroundColor = deviceContext->ForegroundColor();        
        
        // Invoke our superclass
        status = NW_LMgr_MediaBox_Class.NW_LMgr_Box.draw(iBox, 
                deviceContext, aHasFocus);
        NW_THROW_ON_ERROR(status);

        // If the place holder isn't being shown then its done.
        if (!iShowingPlaceHolder)
            {
            if (iBoxListener)
                {
                if (iPluginAttached)
                    {
                    iBoxListener->RefreshPlugin();
                    }
                }
            NW_THROW_SUCCESS(status);
            }

        // If necessary get the image place holder.
        if (!iPlaceHolderImage)
            {
            NW_LMgr_RootBox_t*  rootBox = NULL;

            rootBox = NW_LMgr_Box_GetRootBox(iBox);
            NW_THROW_ON_NULL(rootBox, status, KBrsrUnexpectedError);
            if (!iInHyperLink)
                {
                iPlaceHolderImage = NW_Image_CannedImages_GetImage(
                        rootBox->cannedImages, NW_Image_Object);
                }
            else
                {
                // If the <object> is in a hyperlink, treat it as if it was an image
                iPlaceHolderImage = NW_Image_CannedImages_GetImage(
                        rootBox->cannedImages, NW_Image_Broken);
                }
            NW_THROW_ON_NULL((void *) iPlaceHolderImage, status, KBrsrUnexpectedError);
            }

        // Get the rect it is allowed to draw in
        NW_LMgr_Box_GetInnerRectangle(iBox, &innerRect);

        // Get the image size.
        status = NW_Image_AbstractImage_GetSize(iPlaceHolderImage, &imageSize);
        NW_THROW_ON_ERROR(status);

        if (innerRect.dimension.width < imageSize.width)
            {
            imageSize.width = innerRect.dimension.width;
            }

        if (innerRect.dimension.height < imageSize.height)
            {
            imageSize.height = innerRect.dimension.height;
            }

        // If necessary recalculate the clipped place holder text.
        if (iOrigPlaceHolderText && iCalculatePlaceHolderClip)
            {
            // Discard the old text
            delete iPlaceHolderText;
            iPlaceHolderText = NULL;

            iPlaceHolderText = ClipText(*iOrigPlaceHolderText, *deviceContext,
                    innerRect.dimension.width - imageSize.width, ETrue);

            iCalculatePlaceHolderClip = EFalse;
            }

        // Draw Image place holder.
        pos.x = innerRect.point.x;
        pos.y = innerRect.point.y;
    
        status = NW_Image_AbstractImage_DrawScaled(
                CONST_CAST(NW_Image_AbstractImage_t *, iPlaceHolderImage), 
                deviceContext, &pos, &imageSize);
        NW_THROW_ON_ERROR(status);

        // Draw the text only if the <object> is not in hyperlink
        if (!iInHyperLink)
            {
            font = NW_LMgr_Box_GetFont(iBox);
            NW_THROW_ON_NULL(font, status, KBrsrUnexpectedError);
                
            // Set the foreground color
            status = NW_LMgr_Box_GetPropertyValue(iBox, NW_CSS_Prop_color,
                    NW_CSS_ValueType_Color, &value);

            if (status == KBrsrSuccess)
                {
                color = value.integer;

                deviceContext->SwitchAgainstBG(&color);

                deviceContext->SetForegroundColor(color);
                }


            // Draw the text place holder.
            if (iPlaceHolderText)
                {
                NW_Text_UCS2_t nwText;
                NW_GDI_Rectangle_t oldClip = {{0, 0}, {0, 0}};

                status = NW_Text_UCS2_Initialize(&nwText, (void*) iPlaceHolderText->Ptr(), 
                        iPlaceHolderText->Length(), 0);
                NW_THROW_ON_ERROR (status);

                pos.x = innerRect.point.x + imageSize.width;
                pos.y = innerRect.point.y;
    
                /* Save the old clip rect */
                oldClip = aDeviceContext.ClipRect();

                /* Clip to the inner rect so the text doesn't spill outside of the rect */
                const_cast<CGDIDeviceContext &>(aDeviceContext).SetClipRect(&innerRect);

                status = deviceContext->DrawText(&pos, (NW_Text_t*)(&nwText), font, 0, 
                        (NW_GDI_TextDecoration_e) 0, dir, 
                        innerRect.dimension.width - imageSize.width);

                /* Restore the clip BEFORE checking the status */
                const_cast<CGDIDeviceContext &>(aDeviceContext).SetClipRect(&oldClip);

                NW_THROW_ON_ERROR (status);
                }
            }
        }

    NW_CATCH (status) 
        {
        } 

    NW_FINALLY 
        {
        deviceContext->SetForegroundColor(oldForgroundColor);

        return status;
        } 
    NW_END_TRY
    }

// -----------------------------------------------------------------------------
// CLMgrObjectBox::Render
//
// If necessary it positions the CPluginInst to match the box's 
// bounds and renders the box.
// -----------------------------------------------------------------------------
//
TBrowserStatusCode CLMgrObjectBox::Render(const CGDIDeviceContext& aDeviceContext,
        const NW_LMgr_RootBox_t& aRootBox, const NW_GDI_Rectangle_t& aClipRect, 
        const NW_LMgr_Box_t* aCurrentBox, NW_Uint8 aFlags, NW_Bool& aHasFocus, 
        NW_Bool& aSkipChildren)
    {
    NW_TRY (status)
        {
        // Invoke our superclass
        status = NW_LMgr_MediaBox_Class.NW_LMgr_Box.render(iBox, 
                CONST_CAST(CGDIDeviceContext*, &aDeviceContext), 
                CONST_CAST(NW_GDI_Rectangle_t*, &aClipRect), CONST_CAST(NW_LMgr_Box_t*, 
                aCurrentBox), aFlags, &aHasFocus, &aSkipChildren, 
                CONST_CAST(NW_LMgr_RootBox_t*, &aRootBox));
        NW_THROW_ON_ERROR(status);

        // The PluginInst must be synchronized with the box's new scroll adjusted 
        // location -- the size of the PluginInst may also need to be adjusted.
        // This method is called even if the place holder is shown because it may
        // need to recalculate the clip of the place holder text.
        SyncPluginInstBounds(aDeviceContext);
        }

    NW_CATCH (status) 
        {
        } 

    NW_FINALLY 
        {
        return status;
        } 
    NW_END_TRY
    }

// -----------------------------------------------------------------------------
// CLMgrObjectBox::SetBoxListener
//
// Sets the box's listener
// -----------------------------------------------------------------------------
//
void CLMgrObjectBox::SetBoxListener(MLMgrObjectBoxListener* aListener)
    {
    iBoxListener = aListener;
    }

// -----------------------------------------------------------------------------
// CLMgrObjectBox::AddBoxDestructionListenerL
//
// Adds a box's destruction listener
// -----------------------------------------------------------------------------
//
void CLMgrObjectBox::AddBoxDestructionListenerL(MLMgrBoxDestructionListener* aListener)
    {
    User::LeaveIfError(iDestructionListeners.Append(aListener));
    }

// -----------------------------------------------------------------------------
// CLMgrObjectBox::RemoveBoxDestructionListenerL
//
// Removes a box's destruction listener
// -----------------------------------------------------------------------------
//
void CLMgrObjectBox::RemoveBoxDestructionListener(MLMgrBoxDestructionListener* aListener)
    {
    TInt index;

    index = iDestructionListeners.Find(aListener);
    if (index != KErrNotFound)
        {
        iDestructionListeners.Remove(index);
        }
    }

// -----------------------------------------------------------------------------
// CLMgrObjectBox::SetPlaceHolderTextL
//
// Sets the place holder text.
// -----------------------------------------------------------------------------
//
void CLMgrObjectBox::SetPlaceHolderTextL(const TDesC& aNewText)
    {
    // Set the place holder state to true
    iShowingPlaceHolder = ETrue;

    // Delete the old text
    delete iOrigPlaceHolderText;
    iOrigPlaceHolderText = NULL;

    delete iPlaceHolderText;
    iPlaceHolderText = NULL;

    // Copy the text
    HBufC*  temp;

    temp = HBufC::NewL(aNewText.Length() + 1);
    temp->Des().Append(aNewText);
    temp->Des().ZeroTerminate();

    iOrigPlaceHolderText = temp;

    iCalculatePlaceHolderClip = ETrue;

    // Invalidate the box
    (void) NW_LMgr_Box_Refresh((NW_LMgr_Box_t*) iBox->parent);
    }

// -----------------------------------------------------------------------------
// CXHTMLObjectElementHandler::SetPlaceHolderTextL
//
// Set the place-holder text to the given resource id.
// -----------------------------------------------------------------------------
//
void CLMgrObjectBox::SetPlaceHolderTextL(TInt aResourceId)
    {
    HBufC*  placeHolderText = NULL;

    // Get the string from the StringLoader.
    placeHolderText = StringLoader::LoadLC(aResourceId);

    // Set the place holder text.
    SetPlaceHolderTextL(*placeHolderText);
    CleanupStack::PopAndDestroy(placeHolderText);
    }

// -----------------------------------------------------------------------------
// CLMgrObjectBox::RemovePlaceHolder
//
// Removes the place holder's text and image.
// -----------------------------------------------------------------------------
//
void CLMgrObjectBox::RemovePlaceHolder(void)
    {
    CGDIDeviceContext*   deviceContext = NULL;
    NW_LMgr_RootBox_t*   rootBox = NULL;

    rootBox = NW_LMgr_Box_GetRootBox(iBox);
    NW_ASSERT(rootBox != NULL);

    deviceContext = NW_LMgr_RootBox_GetDeviceContext(rootBox);
    NW_ASSERT(deviceContext != NULL);
    // Set the place holder state to false
    iShowingPlaceHolder = EFalse;

    // Delete the old text
    delete iOrigPlaceHolderText;
    iOrigPlaceHolderText = NULL;

    delete iPlaceHolderText;
    iPlaceHolderText = NULL;

    iForceSyncPluginInst = ETrue;
    if (NW_Object_IsInstanceOf (iBox->parent, &NW_LMgr_ActiveContainerBox_Class))
        {
        NW_LMgr_ActiveContainerBox_SetActionType(iBox->parent, 
            NW_LMgr_ActionType_ActivateObject);
        }

    SyncPluginInstBounds(*deviceContext);
    }

// -----------------------------------------------------------------------------
// CLMgrObjectBox::IsShowingPlaceHolder
//
// Returns ETrue if the place holder is being shown.
// -----------------------------------------------------------------------------
//
TBool CLMgrObjectBox::IsShowingPlaceHolder(void)
    {
    return iShowingPlaceHolder;
    }

// -----------------------------------------------------------------------------
// CLMgrObjectBox::IsActive
//
// Returns if the plugin is active or not.
// -----------------------------------------------------------------------------
//
TBool CLMgrObjectBox::IsActive() const
    {
    return iBoxListener->IsActive();
    }

// -----------------------------------------------------------------------------
// CLMgrObjectBox::FindBoxTree
//
// Each CLMgrObjectBox is contained in a container-box.  In order to identify the 
// plugin box associated with a given elementNode the NW_CSS_Prop_elementId 
// CSS property is applied, where the property value equals the element-node.
// FindBoxTree returns the box that contains a NW_CSS_Prop_elementId with a
// value matching aBoxKey. 
// -----------------------------------------------------------------------------
//
NW_LMgr_Box_t* CLMgrObjectBox::FindBoxTree(const NW_LMgr_Box_t& aBoxTree,
        void* aBoxKey)
    {        
    NW_LMgr_BoxVisitor_t  boxVisitor;
    NW_LMgr_Box_t*        box = NULL;
    NW_LMgr_Box_t*        targetBox = NULL;
    
    NW_ASSERT(aBoxKey != NULL);

    NW_TRY (status)
        {
        // Init a box visitor
        status = NW_LMgr_BoxVisitor_Initialize(&boxVisitor, 
                CONST_CAST(NW_LMgr_Box_t*, &aBoxTree));
        NW_THROW_ON_ERROR(status);
    
        while ((box = NW_LMgr_BoxVisitor_NextBox(&boxVisitor, 0)) != NULL)
            {
            NW_LMgr_PropertyValueType_t type = NW_CSS_ValueType_Object;
            NW_LMgr_PropertyValue_t value;
        
            // Get the ElementId property
            status = NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_elementId, 
                    type, &value);

            if (status == KBrsrNotFound)
                {
                continue;
                }

            NW_THROW_ON_ERROR(status);

            // Does this box match the key?
            if (value.object == aBoxKey)
                {
                targetBox = box;
                break;
                }
            }
        }

    NW_CATCH (status)
        {
        targetBox = NULL;
        }

    NW_FINALLY 
        {
        return targetBox;
        }
    NW_END_TRY
    }

// -----------------------------------------------------------------------------
// CLMgrObjectBox::FindObjectBox
//
// Each CLMgrObjectBox is contained in a container-box.  FindObjectBox finds 
// and returns the first NW_LMgr_ObjectBox found in aBoxTree.
// -----------------------------------------------------------------------------
//
NW_LMgr_Box_t* CLMgrObjectBox::FindObjectBox(const NW_LMgr_Box_t& aBoxTree)
    {        
	TBrowserStatusCode		status;
    NW_LMgr_BoxVisitor_t	boxVisitor;
    NW_LMgr_Box_t*			box = NULL;
    
    // Init a box visitor
    status = NW_LMgr_BoxVisitor_Initialize(&boxVisitor, 
            CONST_CAST(NW_LMgr_Box_t*, &aBoxTree));
	if (status != KBrsrSuccess)
	    {
		return NULL;
	    }

    while ((box = NW_LMgr_BoxVisitor_NextBox(&boxVisitor, 0)) != NULL)
        {
		if (NW_Object_IsInstanceOf(box, &NW_LMgr_ObjectBox_Class))
            {
			return box;
            }
        }

	return NULL;
    }

// -----------------------------------------------------------------------------
// CLMgrObjectBox::GetTextProperty
//
// Returns the given property as a descriptor.
// -----------------------------------------------------------------------------
//
TDesC* CLMgrObjectBox::GetTextProperty(const NW_LMgr_Box_t& aBox, 
        NW_LMgr_PropertyName_t aProperty)
    {
    TBrowserStatusCode  status;
    NW_LMgr_Property_t  prop;
    HBufC*              value = NULL;

    // Get the prop from the box
    prop.type = NW_CSS_ValueType_Text;
    prop.value.object = NULL;
    status = NW_LMgr_Box_GetProperty(CONST_CAST(NW_LMgr_Box_t*, &aBox), aProperty, &prop);
    
    if ((status == KBrsrSuccess) && (prop.value.object))
        {
        NW_Text_t*  temp;

        temp = (NW_Text_t*) prop.value.object;

        // Copy the result into a TDes.
        value = HBufC::New(temp->characterCount + 1);
        if (value)
            {
            value->Des().Append((const TUint16*) temp->storage, temp->characterCount);
            value->Des().ZeroTerminate();
            }
        }

    return value;
    }

// -----------------------------------------------------------------------------
// CLMgrObjectBox::GetParentWindow
//
// Returns the view infrastructure's CCoeControl associated the given root-box.
// -----------------------------------------------------------------------------
//
CCoeControl* CLMgrObjectBox::GetParentWindow(const NW_LMgr_RootBox_t& aRootBox)
    {
    MBoxTreeListener*  listener = NULL;
    CCoeControl*                 parentWindow = NULL;

    listener = NW_LMgr_RootBox_GetBoxTreeListener(CONST_CAST(NW_LMgr_RootBox_t*, &aRootBox));
    NW_ASSERT(listener != NULL);

    parentWindow = (CCoeControl*)listener->GetParentWindow();
    NW_ASSERT(parentWindow != NULL);

    return parentWindow;
    }

// -----------------------------------------------------------------------------
// CLMgrObjectBox::GetParentWindow
//
// Returns the view infrastructure's CCoeControl associated with this instance.
// -----------------------------------------------------------------------------
//
CCoeControl* CLMgrObjectBox::GetParentWindow(void)
    {
    NW_LMgr_RootBox_t* rootBox = NW_LMgr_Box_GetRootBox(iBox);

    if (rootBox)
        {
        return GetParentWindow(*rootBox);
        }
    else
        {
        return NULL;
        }
    }

// -----------------------------------------------------------------------------
// CLMgrObjectBox::AttachPluginInst
//
// Shows, sizes, and locates the CPluginInst or place holder to correspond with 
// the box.
// -----------------------------------------------------------------------------
//
void CLMgrObjectBox::AttachPluginInst(const NW_GDI_Point2D_s& aLocation,
        const NW_GDI_Dimension2D_t& aSize)
    {
    NW_GDI_Rectangle_t bounds;

    // If the place holder is showing determine if the place holder text needs
    // to be re-clipped.
    if (iShowingPlaceHolder)
        {
        // If the width of the box has changed the clipped place holder text may
        // need to be recalculated.
        if (aSize.width != iSize.width)
            {
            iCalculatePlaceHolderClip = ETrue;
            }
        }

    // Otherwise show the plugin in the correct location.
    else
        {
        if (!iPluginAttached)
            {
            iBoxListener->MakeVisible(ETrue);
            iPluginAttached = TRUE;
            }

        // Place it in the given location with the given size.
        bounds.point = aLocation;
        bounds.dimension = aSize;
    
        if ((bounds.dimension.height != 0) || (bounds.dimension.width != 0))
            {
            iBoxListener->SetExtent(bounds);
            }
        }

    // Remember the new location and size.
    iLocation = aLocation;
    iSize = aSize;
    }

// -----------------------------------------------------------------------------
// CLMgrObjectBox::DetachPluginInstL
//
// Detaches the CPluginInst from its parent.
// -----------------------------------------------------------------------------
//
void CLMgrObjectBox::DetachPluginInst(void)
    {
    // Ignore this method if the place holder is showing
    if (iShowingPlaceHolder)
        {
        return;
        }

    // Otherwise hide the CPluginInst
    if (iPluginAttached && iBoxListener)
        {
        iBoxListener->MakeVisible(EFalse);

        // There is a funny Symbian bug that occurs if the BoxListener (a CCoeControl)
        // isn't FULLY contained within its parent control when the current page is
        // replaced.  To prevent this issue the control is moved to zero, zero when
        // the BoxListener is hidden.
        NW_GDI_Rectangle_t bounds;

        bounds.point.x = 0;
        bounds.point.y = 0;
        bounds.dimension = iSize;
        iBoxListener->SetExtent(bounds);
        }

        iPluginAttached = FALSE;
        iForceSyncPluginInst = ETrue;
    }

// -----------------------------------------------------------------------------
// CLMgrObjectBox::SyncPluginInstBoundsL
//
// Adjusts the location and size of the PluginInst or place holder so it matches iBox's
// bounds.
// -----------------------------------------------------------------------------
//
void CLMgrObjectBox::SyncPluginInstBounds(const CGDIDeviceContext& aDeviceContext)
    {
    NW_GDI_Rectangle_t  viewBounds;
    NW_GDI_Point2D_t    origin;
    NW_GDI_Rectangle_t  bounds;

    // Get the box's "inner" rect and the device origin
    (void) NW_LMgr_Box_GetInnerRectangle(iBox, &bounds);
    origin = *(aDeviceContext.Origin());

    // Get the view bounds.
    viewBounds = *(aDeviceContext.DisplayBounds());
    viewBounds.point = origin;

    // Detach the PluginInst if it isn't visible
    if (!NW_GDI_Rectangle_Cross(&bounds, &viewBounds, NULL))
        {
        iPluginAttached = ETrue;
        DetachPluginInst();
        }

    // Otherwise if necessary size and place the PluginInst
    else
        {
        // Determine the new location given the origin and bounds.point
        NW_GDI_Point2D_t  location;

        location.x = bounds.point.x - origin.x;
        location.y = bounds.point.y - origin.y;

        // If necessary adjust the location and size
        if ((iSize.width != bounds.dimension.width) ||
                (iSize.height != bounds.dimension.height) ||            
                (iLocation.x != location.x) || (iLocation.y != location.y) ||
                iForceSyncPluginInst)
            {
            AttachPluginInst(location, bounds.dimension);
            iForceSyncPluginInst = EFalse;
            }
        }
    }

// -----------------------------------------------------------------------------
// CLMgrObjectBox::HandleVerticalLayout
//
// Implements the the Object handling rules for vertical layout
// 1. Scale the box maintaining the aspect ratio if width is greater than display width
// 2. Scale the box maintaining the aspect ratio if height is greater than display height
// 3. If box is too small it doesn't participate in the layout.
// -----------------------------------------------------------------------------
//
void CLMgrObjectBox::HandleVerticalLayout(const NW_GDI_Dimension2D_t& aOrigSize, 
        NW_GDI_Dimension2D_t& aNewSize)
    {
    CGDIDeviceContext*   deviceContext = NULL;
    NW_GDI_Rectangle_t   rectangle;    
    NW_LMgr_FrameInfo_t  padding;
    NW_LMgr_FrameInfo_t  borderWidth;
    NW_GDI_Metric_t      maxWidth = 0;
    NW_GDI_Metric_t      maxHeight = 0;
    NW_LMgr_RootBox_t*   rootBox = NULL;

    // Get the root box.
    rootBox = NW_LMgr_Box_GetRootBox(iBox);
    NW_ASSERT(rootBox != NULL);

    // Determine the maximum width and height for the box.
    deviceContext = NW_LMgr_RootBox_GetDeviceContext(rootBox);
    NW_ASSERT(deviceContext != NULL);

    rectangle = *(deviceContext->DisplayBounds());

    NW_LMgr_Box_GetPadding(iBox, &padding, ELMgrFrameAll );
    NW_LMgr_Box_GetBorderWidth(iBox, &borderWidth, ELMgrFrameAll );
    
    maxWidth = (NW_GDI_Metric_t) (rectangle.dimension.width - padding.left - 
            padding.right - borderWidth.right - borderWidth.left - 6);
    maxHeight = (NW_GDI_Metric_t) (rectangle.dimension.height - padding.top - 
            padding.bottom - borderWidth.top - borderWidth.bottom);
    
    aNewSize = aOrigSize;

    // Scale the box maintaining the aspect ratio if height is greater than display height.
    if (aOrigSize.height > maxHeight)
        {
        aNewSize.width = (NW_GDI_Metric_t) ((maxHeight * aOrigSize.width) / aOrigSize.height);
        aNewSize.height = maxHeight;
        }

    // Scale the box maintaining the aspect ratio if width is greater than display width.
    if (aNewSize.width > maxWidth)
        {
        aNewSize.height = (NW_GDI_Metric_t) ((maxWidth * aNewSize.height) / aNewSize.width);
        aNewSize.width = maxWidth;
        }
    }

// -----------------------------------------------------------------------------
// CLMgrObjectBox::ClipText
//
// Creates a copy of aText such that it width is <= aAvailableSpace -- truncating 
// as necessary.
// -----------------------------------------------------------------------------
//
// TODO Share with IamgeContainerBox
TDesC* CLMgrObjectBox::ClipText(const TDesC& aText, const CGDIDeviceContext& aDeviceContext,
        NW_GDI_Metric_t aAvailableSpace, TBool aAddDots)
    {
    _LIT(kDots, "...");

    CGDIDeviceContext*      deviceContext = NULL;
    CGDIFont*               font = NULL;
    NW_GDI_FlowDirection_t  dir = NW_GDI_FlowDirection_LeftRight;
    NW_Text_UCS2_t          nwText;
    NW_Text_Length_t        clipLength;
    NW_Text_Length_t        newLength;
    HBufC*                  clippedText = NULL;
    TBool                   addDots = aAddDots;

    NW_TRY(status)
        {        
        // For convenience.
        deviceContext = CONST_CAST(CGDIDeviceContext*, &aDeviceContext);

        // Get the box font.
        font = NW_LMgr_Box_GetFont(iBox);
        NW_THROW_ON_NULL(font, status, KBrsrUnexpectedError);
        
        // Wrap aText in an NW_Text_t
        status = NW_Text_UCS2_Initialize(&nwText, (void*) aText.Ptr(), aText.Length(), 0);
        NW_THROW_ON_ERROR (status);

        // Clip the text
        status = deviceContext->SplitText((NW_Text_t*) &nwText, font, aAvailableSpace, 
                0, dir, &clipLength, NW_GDI_SplitMode_Clip);
        NW_THROW_ON_ERROR (status);
        
        // If the text fits in the space or if "..." aren't to be appended then set 
        // the copy-length to clipLength.
        if ((clipLength == NW_Text_GetCharCount(&nwText)) || (!aAddDots))
            {
            newLength = clipLength;
            addDots = EFalse;
            }

        // Otherwise split it again, but this time subtracting the length of "..."
        // from available space.
        else
            {
            NW_GDI_Dimension2D_t dotsExtent;

            // Wrap aText in an NW_Text_t
            status = NW_Text_UCS2_Initialize(&nwText, (void*) kDots().Ptr(), 
                    kDots().Length(), 0);
            NW_THROW_ON_ERROR (status);

            // Get the width of the dots
            dotsExtent.height = 0;
            dotsExtent.width = 0;

            status = deviceContext->GetTextExtent((NW_Text_t*)(&nwText), font,
                0, NW_GDI_FlowDirection_LeftRight, &dotsExtent);
            NW_THROW_ON_ERROR (status);

            // Wrap aText in an NW_Text_t
            status = NW_Text_UCS2_Initialize(&nwText, (void*) aText.Ptr(), aText.Length(), 0);
            NW_THROW_ON_ERROR (status);

            // Clip the text again this time less the dotExtent
            status = deviceContext->SplitText((NW_Text_t*) &nwText, font,
                    aAvailableSpace - dotsExtent.width, 0, dir, &clipLength, NW_GDI_SplitMode_Clip);
            NW_THROW_ON_ERROR (status);

            newLength = clipLength + kDots().Length();

            // CGDIDeviceContext::SplitText returns a clipLength of at least one
            // char.  Because of this a special case is needed.  This case occurs when
            // none of the text fits in aAvailableSpace and aAddDots is true.  In this
            // case the added dots may require more space than aAvailableSpace.
            if ((clipLength == 1) && (dotsExtent.width > aAvailableSpace))
                {
                newLength = 0;
                }
            }

        // Copy the cliped text
        if (newLength > 0)
            {
            clippedText = HBufC::New(newLength + 1);
            NW_THROW_OOM_ON_NULL(clippedText, status);

            clippedText->Des().Append(aText.Ptr(), clipLength);
        
            if (addDots)
                {
                clippedText->Des().Append(kDots().Ptr(), kDots().Length());
                }

            clippedText->Des().ZeroTerminate();
            }
        }

    NW_CATCH (status)
        {
        delete clippedText;
        clippedText = NULL;
        }

    NW_FINALLY
        {
        return clippedText;
        } 
    NW_END_TRY
    }

