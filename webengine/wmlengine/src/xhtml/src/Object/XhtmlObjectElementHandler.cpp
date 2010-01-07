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
* Description:  Handles object tags in XHTML documents.
*
*/

// INCLUDE FILES
#include <e32std.h>
#include <e32def.h>
#include <webkit.rsg>

#include "nw_xhtml_xhtmlcontenthandler.h"
#include "nw_xhtml_elementhandler.h"
//#include "nw_ecma_contenthandler.h"
#include "nw_evt_accesskeyevent.h"
#include "nw_evt_activateevent.h"
#include "nw_evt_openviewerevent.h"
#include "nw_evt_focusevent.h"
#include "nw_hed_documentroot.h"
#include "HEDDocumentListener.h"
#include "nw_hed_hedeventhandler.h"
#include "nw_lmgr_animatedimagebox.h"
#include "nw_lmgr_propertylist.h"
#include "LMgrObjectBoxOOC.h"
#include "nw_lmgr_rootbox.h"
#include "nw_image_cannedimages.h"
#include "nw_image_virtualimage.h"
#include "nwx_settings.h"
#include "PluginContentHandlerOOC.h"
#include "LMgrObjectBox.h"
#include "PluginContentHandler.h"
#include "ObjectUtils.h"
#include "PluginHandler.h"
#include "StringUtils.h"
#include "ObjectDialog.h"
#include "XhtmlObjectElementHandler.h"
#include "nwx_http_defs.h"
#include "urlloader_urlresponse.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXHTMLObjectElementHandler::CXHTMLObjectElementHandler
//
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXHTMLObjectElementHandler::CXHTMLObjectElementHandler(const NW_XHTML_ElementHandler_t* aElementHandler,
        NW_XHTML_ContentHandler_t* aContentHandler, NW_DOM_ElementNode_t* aElementNode)        
    {
    NW_ASSERT(aElementHandler != NULL);
    NW_ASSERT(aContentHandler != NULL);
    NW_ASSERT(aElementNode != NULL);

    iElementHandler = aElementHandler;
    iContentHandler = aContentHandler;
    iElementNode = aElementNode;

    iLoadedHandlerType = ObjectUtils::EHandlerAny;
    }

// -----------------------------------------------------------------------------
// CXHTMLObjectElementHandler::ConstructL
//
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXHTMLObjectElementHandler::ConstructL(void)
    {
    }

// -----------------------------------------------------------------------------
// CXHTMLObjectElementHandler::~CXHTMLObjectElementHandler
//
// Deconstructor.
// -----------------------------------------------------------------------------
//
CXHTMLObjectElementHandler::~CXHTMLObjectElementHandler()
    {
    // Clear the listener so this isn't notified when the box is destroyed,
    // since this instance will no longer be valid.
    if (iObjectBox)
        {
        NW_LMgr_ObjectBox_GetObjectBox(*iObjectBox)->RemoveBoxDestructionListener(this);
        }
    if (iTransactionId)
        {
        NW_XHTML_ContentHandler_SetLoadObserver(iContentHandler, iTransactionId, NULL);
        }

    delete iContentType;
    delete iContentLength;
    }

// -----------------------------------------------------------------------------
// CXHTMLObjectElementHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXHTMLObjectElementHandler* CXHTMLObjectElementHandler::NewL(const NW_XHTML_ElementHandler_t* 
        aElementHandler, NW_XHTML_ContentHandler_t* aContentHandler, 
        NW_DOM_ElementNode_t* aElementNode)
    {
    CXHTMLObjectElementHandler* self = new(ELeave) CXHTMLObjectElementHandler(aElementHandler,
            aContentHandler, aElementNode);

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    
    return self;
    }

// -----------------------------------------------------------------------------
// CPluginContentHandler::Destroyed
//
// Recieves notifications when the box has been destroyed. 
// -----------------------------------------------------------------------------
//
void CXHTMLObjectElementHandler::Destroyed()
    {
    // Forget about the destroyed box.
    iObjectBox = NULL;
    }

// -----------------------------------------------------------------------------
// CXHTMLObjectElementHandler::HeadCompleted
//
// Passes a HEAD response to the observer.
// -----------------------------------------------------------------------------
//
void CXHTMLObjectElementHandler::HeadCompleted(TUint16 aTransactionId, 
        const NW_Url_Resp_t& aResponse, const NW_HED_UrlRequest_t& /*aUrlRequest*/,
        void* /*aClientData*/)
    {
    NW_Ucs2*  temp = NULL;

    NW_TRY (status)
        {
        NW_XHTML_ContentHandler_SetLoadObserver(iContentHandler, aTransactionId, NULL);
        iTransactionId = 0;
        // Ensure iObjectBox isn't NULL.
        NW_THROW_ON_NULL(iObjectBox, status, KBrsrUnexpectedError);

        // Allow the server to override iContentType if it was never set from 
        // the type attribute or it was set but set to a type that isn't supported
        // (this give the server a chance to set the type if a valid type wasn't 
        // set in the tag).
        /*//R->embed
        if (!iContentType || !ObjectUtils::IsSupported(iContentType, 
                NULL, ObjectUtils::EHandlerAny))
            {
            NW_THROW_ON_NULL(aResponse.contentTypeString, status, KBrsrUnexpectedError);

            temp = StringUtils::CopyAsciiToUsc2(aResponse.contentTypeString);
            NW_THROW_OOM_ON_NULL(temp, status);

            delete iContentType;
            iContentType = NULL;

            iContentType = HBufC16::New(User::StringLength(temp));
            NW_THROW_OOM_ON_NULL(iContentType, status);

            iContentType->Des().Append(temp, User::StringLength(temp));

            delete temp;
            temp = NULL;
        	}
		*/
        // Extract and set the new content-length.
        TUint kiloLen = aResponse.contentLength / 1024;

        status = StringUtils::ConvertUintToUcs2(kiloLen > 0 ? kiloLen: 1, &temp);
        NW_THROW_ON_ERROR(status);

        delete iContentLength;
        iContentLength = NULL;

        iContentLength = HBufC16::New(User::StringLength(temp));
        NW_THROW_OOM_ON_NULL(iContentLength, status);

        iContentLength->Des().Append(temp, User::StringLength(temp));

        delete temp;
        temp = NULL;

        // Apply the new values as properties.
        ApplyInfoProperties(*iObjectBox);

        // This flag is set now that it has a valid content type and size.  The
        // flag is used later to skip any future validating head request that may
        // normally be done when the box tree is created.
        iObjectInfoValidated = ETrue;

        // Set the place holder text to "Unsupported" if the verified content-type 
        // isn't supported by a plugin or external-app.
        /*//R->embed
        if (!ObjectUtils::IsSupported(iContentType, NULL, ObjectUtils::EHandlerAny))
            {
            status = SetPlaceHolderText(*iObjectBox, R_XHTML_OBJECT_UNSUPPORTED);
            NW_THROW_ON_ERROR(status);
            }
        */    
        }
		
    NW_CATCH (status) 
        {
        delete temp;
        }
    
    NW_FINALLY 
        {
        } NW_END_TRY
    }

// -----------------------------------------------------------------------------
// CXHTMLObjectElementHandler::ChunkReceived
//
// Passes a chunk of a response to the observer.
// -----------------------------------------------------------------------------
//
void CXHTMLObjectElementHandler::ChunkReceived(TUint16 /*aTransactionId*/, TUint32 /*aChunkIndex*/,
        const NW_Url_Resp_t& /*aResponse*/, const NW_HED_UrlRequest_t& /*aUrlRequest*/, 
        void* /*aClientData*/)
    {
    }

// -----------------------------------------------------------------------------
// CXHTMLObjectElementHandler::LoadCompleted
//
// Called when a response completes successfully or in error.
// -----------------------------------------------------------------------------
//
void CXHTMLObjectElementHandler::LoadCompleted(TInt16 /*aStatusClass*/, TBrowserStatusCode aStatus, 
        TUint16 aTransactionId, const NW_HED_UrlRequest_t& /*aUrlRequest*/, 
        void* /*aClientData*/)
    {
    NW_XHTML_ContentHandler_SetLoadObserver(iContentHandler, aTransactionId, NULL);
    iTransactionId = 0;

    if (iObjectBox)
        {
        // Reset the place holder upon a successful load of a resouce for an external application.
        if ((aStatus == KBrsrSuccess) && (iLoadedHandlerType == ObjectUtils::EHandlerExternalApp))
            {
            CLMgrObjectBox* theBox = NULL;
            theBox = NW_LMgr_ObjectBox_GetObjectBox(*iObjectBox);
            if (!theBox || (theBox->IsShowingPlaceHolder()))
                {
                (void) SetPlaceHolderText(*iObjectBox, R_XHTML_OBJECT_SELECT_TO_DOWNLOAD);
                }
            }
    
        // Reset the place holder upon failure -- unless this was an image load, or plugin was already 
        else if (aStatus != KBrsrSuccess)
            {
            CLMgrObjectBox* theBox = NULL;
            theBox = NW_LMgr_ObjectBox_GetObjectBox(*iObjectBox);
            if (!theBox || (theBox->IsShowingPlaceHolder()))
                {
                (void) SetPlaceHolderText(*iObjectBox, R_XHTML_OBJECT_UNSUPPORTED);
                }
            }
        }

    iIsDownloading = EFalse;
    }

// -----------------------------------------------------------------------------
// CXHTMLObjectElementHandler::Initialize
//
// Initializes the ElementHandler.
// -----------------------------------------------------------------------------
//
TBrowserStatusCode CXHTMLObjectElementHandler::Initialize(void)
    {
    TDesC*  type = NULL;
    TDesC*  data = NULL;

    NW_TRY (status)
        {
        // If this element is the last element in the chunk and this isn't the
        // last chunk then initialize it later.  This only applies to objects not embeds,
        // since embed elements don't have children elements.
        NW_Uint16   element = NW_HED_DomHelper_GetElementToken(iElementNode);

        if ((element == NW_XHTML_ElementToken_object) &&
                (!NW_XHTML_ElementHandler_NextSiblingExists(iContentHandler, iElementNode, NW_TRUE)))
            {
            NW_THROW_SUCCESS (status);
            }

        if (iSelectedElementNode)
            {
            NW_THROW_SUCCESS (status);
            }


        // Determine which element node will be used.  See comment on SelectElement...
        status = SelectElement();
        NW_THROW_ON_ERROR(status);

        // Get the mime-type and url
        (void) GetTypeAttribute(&type);
        (void) GetSrcAttribute(&data);
		/*//R->embed
        if (type || data)
            {
            // Load it now if it is an image unless a plugin can also handle it.
            if (!ObjectUtils::IsSupported(type, data, ObjectUtils::EHandlerPlugin))
                {
                if (ObjectUtils::IsSupported(type, data, ObjectUtils::EHandlerImage))
                    {
                    (void) LoadObjectResource(type, data, ObjectUtils::EHandlerImage);
                    }
                }
            }
		*/
        // If it gets to here it has processed a valid object element.  skipToNode
        // is set so any embedded elements are skipped.
        iContentHandler->skipToNode = NW_DOM_Node_getNextSibling(iElementNode);
        }

    NW_CATCH (status) 
        {
        }
    
    NW_FINALLY 
        {
        delete type;
        delete data;

        return status;
        } NW_END_TRY
    }

// -----------------------------------------------------------------------------
// CXHTMLObjectElementHandler::CreateBoxTree
//
// Creates the box-tree associated with the tag.
// -----------------------------------------------------------------------------
//
TBrowserStatusCode CXHTMLObjectElementHandler::CreateBoxTree(NW_LMgr_ContainerBox_t& /*aParentBox*/)
    {
    NW_HED_DocumentRoot_t*     documentRoot = NULL;
    NW_HED_ContentHandler_t*   contentHandler = NULL;
    NW_LMgr_RootBox_t*         rootBox = NULL;
    NW_LMgr_EventHandler_t*    eventHandler = NULL;
    NW_LMgr_Box_t*             objectBox = NULL;
    NW_LMgr_Box_t*             tempBox = NULL;
    NW_LMgr_Box_t*             innerBox = NULL;
    NW_Image_AbstractImage_t*  image = NULL;
    TDesC*                     contentType = NULL;
    TDesC*                     data = NULL;
    TBool                      safeToDeleteObjectBox = ETrue;
    TBool                      isImage = EFalse;
    TBrowserStatusCode		   status = 0;
	/*
    NW_TRY (status) 
        {
        // If this element is the last element in the chunk and this isn't the
        // last chunk then initialize it later.  This only applies to objects not embeds,
        // since embed elements don't have children elements.
        NW_Uint16   element = NW_HED_DomHelper_GetElementToken(iElementNode);

        if ((element == NW_XHTML_ElementToken_object) &&
                (!NW_XHTML_ElementHandler_NextSiblingExists(iContentHandler, iElementNode, NW_FALSE)))
            {
            NW_THROW_SUCCESS (status);
            }

        // Get the associated content handler if any 
        contentHandler = AssociatedContentHandler();

        // Get the root box
        documentRoot = (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode(iContentHandler);
        rootBox = NW_HED_DocumentRoot_GetRootBox(documentRoot);

        // Get the content-type and url.
        if (iObjectInfoValidated)
            {
            contentType = iContentType->Alloc();
            NW_THROW_OOM_ON_NULL(contentType, status);
            }
        else
            {
            (void) GetTypeAttribute(&contentType);
            }

        (void) GetSrcAttribute(&data);            

        // If the content-type is a supported image type and a plugin can't handle it
        // then create a ImageBox to support the object.
       
       
        if (!ObjectUtils::IsSupported(contentType, data, ObjectUtils::EHandlerPlugin) &&
                ObjectUtils::IsSupported(contentType, data, ObjectUtils::EHandlerImage))
            {
            isImage = ETrue;
            // Get the image box from the ImageCH_Epoc32ContentHandler.
            if (contentHandler)
                {
                status = NW_HED_DocumentNode_GetBoxTree(contentHandler, &objectBox);
                NW_THROW_ON_ERROR(status);

                // Don't allow this box to be deleted because it is owned by 
                // a ImageCH_Epoc32ContentHandler.
                safeToDeleteObjectBox = EFalse;
                }
            
            // Otherwise create a place holder image that will be used by the
            // ImageCH_Epoc32ContentHandler to display the image once it loads.
            else
                {
                const NW_Image_AbstractImage_t* cannedImage;

                cannedImage = (NW_Image_AbstractImage_t*) NW_Image_CannedImages_GetImage(
                        rootBox->cannedImages, NW_Image_Missing);
                NW_THROW_ON_NULL((void*) cannedImage, status, KBrsrUnexpectedError);

                image = (NW_Image_AbstractImage_t*) NW_Image_Virtual_New(
                        const_cast<NW_Image_AbstractImage_t*>(cannedImage));
                NW_THROW_OOM_ON_NULL(image, status);

                objectBox = (NW_LMgr_Box_t*) NW_LMgr_AnimatedImageBox_New(0, 
                        image, NULL, NW_TRUE);
                NW_THROW_OOM_ON_NULL(objectBox, status);
                image = NULL;
                }
            }
			
        // Otherwise create a CLMgrObjectBox to support the object.
        else
            {
            if (!InHyperLink())
                {
                // Create an eventHandler to process events targeted at this box
                eventHandler = (NW_LMgr_EventHandler_t*) NW_HED_EventHandler_New(
                        NW_HED_DocumentNodeOf(iContentHandler), iElementNode);
                NW_THROW_OOM_ON_NULL(eventHandler, status);
                }
            
            status = CLMgrObjectBox::NewBox(eventHandler, &objectBox, &innerBox);
            NW_THROW_ON_ERROR(status);
            
            eventHandler = NULL;
            
            // Save the content-type.  This will be used later in ApplyInfoProperties
            // to set the content-type property.  This step is skipped if the content 
            // type has already been validated.
            if (contentType && !iObjectInfoValidated)
                {
                delete iContentType;
                iContentType = NULL;

                iContentType = HBufC::New(contentType->Length());
                NW_THROW_OOM_ON_NULL(iContentType, status);
                
                iContentType->Des().Append(*contentType);
                }
            
            // Set the object box
            iObjectBox = innerBox;
            
            // Add the element handler as box destruction listener.
            TRAPD(err, NW_LMgr_ObjectBox_GetObjectBox(*iObjectBox)->AddBoxDestructionListenerL(this));
            NW_THROW_ON_TRAP_ERROR(err, status);

            // If a content handler is already present just place the new CLMgrObjectBox 
            // (innerBox) in an active state by clearing the place holder.
           
            if (contentHandler)
                {
                
                if (NW_Object_IsInstanceOf(contentHandler, &NW_Plugin_ContentHandler_Class))
                    {
                    NW_Plugin_ContentHandler_t*  pluginContentHandler = NULL;

                    // Pass the object box to the plugin content handler.
                    pluginContentHandler = (NW_Plugin_ContentHandler_t*) contentHandler;
                    pluginContentHandler->iContentHandler->SetObjectBox(*innerBox);

                    // Removes the place holder which clears the box so the PluginInst
                    // can draw into it.
                    RemovePlaceHolder(*innerBox);
                    }
                }
			
            // Otherwise set the place holder and request the object's info.
            else
                {
                // If data or both content type and data are missing then this object 
                // is unsupported.
                if (!data || (!contentType && !data))
                    {
                    status = SetPlaceHolderText(*innerBox, R_XHTML_OBJECT_UNSUPPORTED);
                    NW_THROW_ON_ERROR(status);
                    }

                // Otherwise set the place holder text and fetch the object's info.
                else
                    {
                    // Set the place holder text to "Select to Download"
                    status = SetPlaceHolderText(*innerBox, R_XHTML_OBJECT_SELECT_TO_DOWNLOAD);
                    NW_THROW_ON_ERROR(status);
            
                    // Fetch the object's information if it hasn't already been validated.
                    // This occurs when a element handler is "resumed" from being previously 
                    // "suspended".
                    
                    if (!iObjectInfoValidated)
                        {
                        // Errors are purposely ignored to allow the box-tree to be created even 
                        // if information about an object can't be fetched.

                        if (ObjectUtils::IsSupported(contentType, data, ObjectUtils::EHandlerPlugin) &&
                            NW_Settings_GetImagesEnabled())
                            {
                            // Set the place holder text to "Select to Download"
                            status = SetPlaceHolderText(*innerBox, R_XHTML_OBJECT_DOWNLOADING);
                            NW_THROW_ON_ERROR(status);
                            iObjectInfoValidated = ETrue;
                            ApplyInfoProperties(*iObjectBox);
                            LoadObjectResource(contentType, data, ObjectUtils::EHandlerPlugin);
                            }
                        else
                            {
                            (void) LoadObjectInfo();
                            }
                        }
                    else
                        {
                        // Set the place holder text to "Unsupported" if the verified content-type 
                        // isn't supported by a plugin or external-app.
                        if (!ObjectUtils::IsSupported(iContentType, NULL, ObjectUtils::EHandlerAny))
                            {
                            status = SetPlaceHolderText(*iObjectBox, R_XHTML_OBJECT_UNSUPPORTED);
                            NW_THROW_ON_ERROR(status);
                            }
                        }
                        
                    }
                }
            
            // TODO: If it is dealing with a object tag with a type but no data attribute then
            //       we need to do some funny things here.
            //
            //       Create a buffer with the "inline" data (the param
            //       elements) and to make a normal load request.  The request's url is of the form
            //       "buffer:url" where the data is provided in the POST body.  The load is  
            //       handled by a new scheme handler, the "buffer" handler.  The handler would
            //       simply create a "response" that contains the buffer and some appropriate 
            //       header values.  The response is then handled as a normal plugin response --
            //       meaning a CPluginContentHandler is created, etc.
            }

        // Attach the active box to the provided parent box.
        status = NW_LMgr_ContainerBox_AddChild(&aParentBox, objectBox);
        _NW_THROW_ON_ERROR(status);
                
        // This is done to prevent a double delete now that parentBox owns innerBox.
        tempBox = (NW_LMgr_Box_t*) objectBox;
        objectBox = NULL;
        
        // Apply common attributes and styles, ignore non-out-of-memory errors.
        // This includes the NW_CSS_Prop_elementId prop which is used by the Plugin content 
        // handler to find this box later on.
        if (isImage)
            {
            status = NW_XHTML_ElementHandler_ApplyStyles(iElementHandler, iContentHandler, 
                    iElementNode, &tempBox, NULL);     
            NW_THROW_ON(status, KBrsrOutOfMemory);
            }
        else
            {
            status = NW_XHTML_ElementHandler_ApplyStyles(iElementHandler, iContentHandler, 
                    iSelectedElementNode, &tempBox, NULL);     
            NW_THROW_ON(status, KBrsrOutOfMemory);
            }
        }

    NW_CATCH (status) 
        {
        if (safeToDeleteObjectBox)
            {
            NW_Object_Delete(objectBox);
            }
        NW_Object_Delete(eventHandler);        
        NW_Object_Delete(image);
        }
    
    NW_FINALLY 
        {
        delete contentType;
        contentType = NULL;

        delete data;
        data = NULL;

        return status;
        } NW_END_TRY
    */    
    return status;    
    }

// -----------------------------------------------------------------------------
// CXHTMLObjectElementHandler::ProcessEvent
//
// Processes events targeted at boxes created by the ElementHandler.
// -----------------------------------------------------------------------------
//
NW_Uint8 CXHTMLObjectElementHandler::ProcessEvent(const NW_LMgr_Box_t& aBox, 
        const NW_Evt_Event_t& aEvent)
    {
    NW_HED_ContentHandler_t*  pluginContentHandler = NULL;
    TDesC*                    standby = NULL;
    TDesC*                    type = NULL;
    TDesC*                    url = NULL;
    NW_Uint8                  absorbed = NW_LMgr_EventNotAbsorbed;
    const NW_Object_Class_t*  eventClass = NULL;
    NW_LMgr_Box_t*             objectBox = NULL;

    NW_TRY (status) 
        {
        // Let the associated content handler process it if one is available.
        pluginContentHandler = AssociatedContentHandler();
        if (pluginContentHandler)
            {
            absorbed = NW_HED_DocumentNode_ProcessEvent(pluginContentHandler,
                    &aBox, &aEvent, NULL);

            if (absorbed == NW_LMgr_EventAbsorbed)
                {
                NW_THROW_SUCCESS(status);
                }
            }

        eventClass = NW_Object_Core_GetClass(&aEvent);

        // Just absorb focus events.
        if (eventClass == &NW_Evt_FocusEvent_Class)
            {
		    absorbed = NW_LMgr_EventAbsorbed;
            }

        // Upon activation load the resource.
        else if ((eventClass == &NW_Evt_OpenViewerEvent_Class) || 
                (eventClass == &NW_Evt_ActivateEvent_Class) ||
                (eventClass == &NW_Evt_AccessKeyEvent_Class))
            {
            ObjectUtils::THandlerType  handlerType = ObjectUtils::EHandlerAny;
            NW_HED_DocumentRoot_t*     docRoot = NULL;

            docRoot = (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode(
                    iContentHandler);
            
            // Let Ecma script try to handle it first
            //R->ecma
            //status = NW_Ecma_HandleEvent(docRoot, iSelectedElementNode, iSelectedElementNode, 
            //        NW_Ecma_Evt_OnClick);
            if(status == KBrsrSuccess)
                {
                absorbed = NW_LMgr_EventAbsorbed;
                NW_THROW_SUCCESS(status);
                }

            // Get the CLMgrObjectBox associated with this event.
            objectBox = CLMgrObjectBox::FindObjectBox(aBox);
            //R->embed NW_ASSERT(objectBox != NULL);
            NW_THROW_ON_NULL(objectBox, status, KBrsrUnexpectedError);

            // If download is in progress, don't start another download.
            if (iIsDownloading)
                {
    		    absorbed = NW_LMgr_EventAbsorbed;
                //R-embed TRAP(status, ObjectDialog::DisplayDownloadingErrorL( docRoot ));
                NW_THROW_SUCCESS(status);
                }

            // Get the resource's content-type from the CSS property.
            // Its ok if GetTextProperty returns NULL.
            type = CLMgrObjectBox::GetTextProperty(*objectBox, NW_CSS_Prop_contentType);

            // Get the resource's url
            // Its ok if GetSrcAttribute fails.
            (void) GetSrcAttribute(&url);

            // Show the Object Download dialog unless this is an OpenViewer event.
            if (eventClass != &NW_Evt_OpenViewerEvent_Class)
                {
                NW_HED_AppServices_t*  appServices = (NW_HED_AppServices_t*) 
                        NW_HED_DocumentRoot_GetAppServices(docRoot);

                if (!appServices->objectDialog.ShowObjectDialog())
                  {
                  // The user canceled the download so just absorb the event.
                  absorbed = NW_LMgr_EventAbsorbed;
                  NW_THROW_SUCCESS(status);
                  }

                // If the content can be handled by both an external app and plugin show a dialog 
                // to allow the user choose which to open it in.
                //R->embed
                /*
                if (ObjectUtils::IsSupported(type, url, ObjectUtils::EHandlerPlugin) &&
                        ObjectUtils::IsSupported(type, url, ObjectUtils::EHandlerExternalApp))
                    {
                    TRAP(status, handlerType = ObjectDialog::SelectViewerL( docRoot ));
                    NW_THROW_ON_ERROR(status);
                    }
				*/                
                }
                

            // Otherwise its a is an OpenViewer event so the dialog isn't needed.
            else
                {
                // Force it to open the resouce in an external application.
                handlerType = ObjectUtils::EHandlerExternalApp;
                }

            // Load the object's resource.
            status = LoadObjectResource(type, url, handlerType);
            NW_THROW_ON_ERROR(status);

            CLMgrObjectBox* theBox = NULL;
            objectBox = CLMgrObjectBox::FindObjectBox(aBox);
            if (objectBox)
                {
                theBox = NW_LMgr_ObjectBox_GetObjectBox(*objectBox);
                }
            if (!theBox || (theBox->IsShowingPlaceHolder()))
                {
                // Upon success set the place holder text to "Downloading Data" or the standby text.
                status = GetAttribute(NW_XHTML_AttributeToken_standby, &standby);

                if (status == KBrsrSuccess)
                    {
                    status = SetPlaceHolderText(*objectBox, *standby);
                    NW_THROW_ON_ERROR(status);

                    delete standby;
                    standby = NULL;
                    }
                else
                    {
                    status = SetPlaceHolderText(*objectBox, R_XHTML_OBJECT_DOWNLOADING);
                    NW_THROW_ON_ERROR(status);
                    }
                }
                absorbed = NW_LMgr_EventAbsorbed;
            }


        // Otherwise ignore the event.
        else
            {
            absorbed = NW_LMgr_EventNotAbsorbed;
            }
        }

    NW_CATCH (status) 
        {
        absorbed = NW_LMgr_EventNotAbsorbed;
        }
    
    NW_FINALLY 
        {
        delete type;
        delete url;

        return absorbed;
        } NW_END_TRY
    }

// -----------------------------------------------------------------------------
// CXHTMLObjectElementHandler::ElementNode
//
// This is a special purpose method to get the element node 
// associated with this handler.
// -----------------------------------------------------------------------------
//
const NW_DOM_ElementNode_t* CXHTMLObjectElementHandler::ElementNode(void) const
    {
    return iElementNode;
    }

// -----------------------------------------------------------------------------
// CXHTMLObjectElementHandler::SelectedElementNode
//
// This is a special purpose method to get the "selected" element.
// See the comment on SelectElement.
// -----------------------------------------------------------------------------
//
const NW_DOM_ElementNode_t* CXHTMLObjectElementHandler::SelectedElementNode(void) const
    {
    return iSelectedElementNode;
    }

// -----------------------------------------------------------------------------
// CXHTMLObjectElementHandler::ApplyInfoProperties
//
// Applies the content type and length properties on the given box.  This is 
// used in the download dialog box.
// -----------------------------------------------------------------------------
//
TBrowserStatusCode CXHTMLObjectElementHandler::ApplyInfoProperties(NW_LMgr_Box_t& aBox)
    {
    NW_Text_t*          text = NULL;
    NW_LMgr_Property_t  prop;

    NW_TRY (status)
        {
        if (iContentType)
            {
            text = NW_Text_New(HTTP_iso_10646_ucs_2, (void*) iContentType->Ptr(), 
                    iContentType->Length(), NW_Text_Flags_Copy);
            NW_THROW_OOM_ON_NULL(text, status);

            prop.type = NW_CSS_ValueType_Text;
            prop.value.object = text;
            status = NW_LMgr_Box_SetProperty(&aBox, NW_CSS_Prop_contentType, &prop);
            NW_THROW_ON_ERROR(status);

            text = NULL;
            }

        if (iContentLength)
            {
            text = NW_Text_New(HTTP_iso_10646_ucs_2, (void*) iContentLength->Ptr(), 
                    iContentLength->Length(), NW_Text_Flags_Copy);
            NW_THROW_OOM_ON_NULL(text, status);

            prop.type = NW_CSS_ValueType_Text;
            prop.value.object = text;
            status = NW_LMgr_Box_SetProperty(&aBox, NW_CSS_Prop_contentLength, &prop);
            NW_THROW_ON_ERROR(status);

            text = NULL;
            }
        }

    NW_CATCH (status) 
        {
        NW_Object_Delete(text);
        }
    
    NW_FINALLY 
        {
        return status;
        } NW_END_TRY
    }

// -----------------------------------------------------------------------------
// CXHTMLObjectElementHandler::LoadObjectResource
//
// Loads the resource associated with object tag.
// -----------------------------------------------------------------------------
//
TBrowserStatusCode CXHTMLObjectElementHandler::LoadObjectResource(const TDesC* aType,
        const TDesC* aUrl, ObjectUtils::THandlerType aHandlerType)
    {
    NW_Text_t*  url = NULL;
    NW_Uint8    loadType = NW_UrlRequest_Type_Any;
    NW_Uint8    reason = NW_HED_UrlRequest_Reason_DocLoad;
    void*       context = NULL;
    TUint16     transactionId;

    NW_TRY (status)
        {
        // Throw an not found error if aUrl is NULL.
        NW_THROW_ON_NULL(aUrl, status, KBrsrNotFound);

        url = NW_Text_New(HTTP_iso_10646_ucs_2, (void*) aUrl->Ptr(), 
                aUrl->Length(), NW_Text_Flags_Copy);
        NW_THROW_OOM_ON_NULL(url, status);

        // If aHandlerType is "any" determine the handler type by querying ObjectUtils.
        // The querying order is significate (plugin, image, external).
        /* //R->embed
        if (aHandlerType == ObjectUtils::EHandlerAny)
            {
            if (ObjectUtils::IsSupported(aType, aUrl, ObjectUtils::EHandlerPlugin))
                {
                aHandlerType = ObjectUtils::EHandlerPlugin;
                }
            else if (ObjectUtils::IsSupported(aType, aUrl, ObjectUtils::EHandlerImage))
                {
                aHandlerType = ObjectUtils::EHandlerImage;
                }
            else if (ObjectUtils::IsSupported(aType, aUrl, ObjectUtils::EHandlerExternalApp))
                {
                aHandlerType = ObjectUtils::EHandlerExternalApp;
                }
            }
		*/
        // The content can be handled by the internal image infrastructure so set up 
        // the load so the response is directed to a NW_LMgr_ImageCH_Epoc32ContentHandler.
        if (aHandlerType == ObjectUtils::EHandlerImage)
            {
            iLoadedHandlerType = ObjectUtils::EHandlerImage;
            loadType = NW_UrlRequest_Type_Image;
            reason = NW_HED_UrlRequest_Reason_DocLoadChild;
            context = STATIC_CAST(void*, iElementNode);

            status = NW_XHTML_ContentHandler_StartLoad_tId(iContentHandler, url,
                    reason, context, loadType, &transactionId);
            NW_THROW_ON_ERROR(status);
            }

        // The content can be handled by a plugin so set up the load so the response 
        // is directed to a CPluginContentHandler.
        else if (aHandlerType == ObjectUtils::EHandlerPlugin)
            {
            iLoadedHandlerType = ObjectUtils::EHandlerPlugin;
            loadType = NW_UrlRequest_Type_Plugin;
            reason = NW_HED_UrlRequest_Reason_DocLoadChild;
            context = STATIC_CAST(void*, iElementNode);
            
            status = NW_XHTML_ContentHandler_StartLoad_tId(iContentHandler, url,
                    reason, context, loadType, &transactionId);
            NW_THROW_ON_ERROR(status);
            }

        // The content can be handled by an external application so set up the load so the 
        // response is directed to the external app infrastructure.
        else if (aHandlerType == ObjectUtils::EHandlerExternalApp)
            {
            iLoadedHandlerType = ObjectUtils::EHandlerExternalApp;
            loadType = NW_UrlRequest_Type_Any;
            reason = NW_HED_UrlRequest_Reason_DocLoad;
            context = NULL;

            // create a new url header
            TUint8* expectedContentType = NULL;
            status = StringUtils::ConvertPtrUsc2ToAscii(TPtrC(*aType), &expectedContentType);
            User::LeaveIfError(status);
            CleanupStack::PushL(expectedContentType);
//R            NW_Http_Header_t* header = UrlLoader_HeadersNew(NULL, NULL, 
//                NULL, NULL, 0, expectedContentType);
			NW_Http_Header_t* header = NULL;
            CleanupStack::PopAndDestroy(1); //expectedContentType
            // create url request
            NW_HED_UrlRequest_t* request;

            request = NW_HED_UrlRequest_New (NW_TextOf (url), NW_URL_METHOD_GET, header,
                                             NULL, reason, NW_HED_UrlRequest_LoadNormal, 
                                             loadType);
 
            NW_THROW_OOM_ON_NULL (request, status);

            if (NW_XHTML_ContentHandler_IsSaveddeck(iContentHandler) &&
                NW_HED_UrlRequest_Reason_DocLoadChild == reason &&
                // Allow to load a plugin from a saved page, because the user explicitly requested it
                loadType != NW_UrlRequest_Type_Plugin)
                {   
                NW_HED_UrlRequest_SetCacheMode(request, NW_CACHE_ONLYCACHE);
                }

            if (NW_XHTML_ContentHandler_IsHistoryLoad(iContentHandler) && 
                NW_HED_UrlRequest_Reason_DocLoadChild == reason)
                {
                NW_HED_UrlRequest_SetCacheMode(request, NW_CACHE_HISTPREV);
                }

            /* pass it to StartRequest to be satisfied */
            status = NW_XHTML_ContentHandler_StartRequest_tId (iContentHandler, request, context, &transactionId);
            NW_THROW_ON_ERROR (status);
            }
        // Otherwise don't load it
        else
            {
            NW_THROW_SUCCESS(status);
            }

        // Set this as the load observer
        status = NW_XHTML_ContentHandler_SetLoadObserver(iContentHandler, transactionId, this);
        iTransactionId = transactionId;
        NW_THROW_ON_ERROR(status);

        iIsDownloading = ETrue;
        }

    NW_CATCH (status) 
        {
        }
    
    NW_FINALLY 
        {
        NW_Object_Delete(url);

        return status;
        } NW_END_TRY
    }

// -----------------------------------------------------------------------------
// CXHTMLObjectElementHandler::LoadObjectInfo
//
// Loads the HEAD information of the resource associated with object tag.
// -----------------------------------------------------------------------------
//
TBrowserStatusCode CXHTMLObjectElementHandler::LoadObjectInfo(void)
    {
    NW_Text_UCS2_t  url;
    TDesC*          urlDes = NULL;

    NW_TRY (status)
        {
        // Prevent network connections from "saved" pages.
        if (NW_XHTML_ContentHandler_IsSaveddeck(iContentHandler))
            {
            if (iContentType)
                {
                iObjectInfoValidated = ETrue;
                ApplyInfoProperties(*iObjectBox);
                }
            NW_THROW_SUCCESS(status);
            }

        // Extract the data attribute.
        status = GetSrcAttribute(&urlDes);
        NW_THROW_ON_ERROR(status);

        status = NW_Text_UCS2_Initialize(&url, (void*) urlDes->Ptr(), 
                urlDes->Length(), 0);
        NW_THROW_ON_ERROR(status);

        // Start a HEAD request for the url.
        TUint16 transactionId;

        status = NW_XHTML_ContentHandler_StartLoad_tId(iContentHandler, (NW_Text_t*) &url,
                NW_HED_UrlRequest_Reason_DocLoadHead, NULL, NW_UrlRequest_Type_Plugin, &transactionId);
        NW_THROW_ON_ERROR(status);

        // Set this as the load observer
        status = NW_XHTML_ContentHandler_SetLoadObserver(iContentHandler, transactionId, this);
        iTransactionId = transactionId;
        NW_THROW_ON_ERROR(status);
        }

    NW_CATCH (status) 
        {
        }
    
    NW_FINALLY 
        {
        delete urlDes;
        return status;
        } NW_END_TRY
    }

// -----------------------------------------------------------------------------
// CXHTMLObjectElementHandler::AssociatedContentHandler
//
// Returns the associated content handler or NULL if one isn't found.
// -----------------------------------------------------------------------------
//
NW_HED_ContentHandler_t* CXHTMLObjectElementHandler::AssociatedContentHandler(void)
    {
    NW_HED_CompositeNode_t*  compositeNode = NULL;

    compositeNode = (NW_HED_CompositeNode_t*) NW_Object_QueryAggregate(
            iContentHandler, &NW_HED_CompositeNode_Class);

    return (NW_HED_ContentHandler_t*) NW_HED_CompositeNode_LookupChild(compositeNode, 
            iElementNode);
    }

// -----------------------------------------------------------------------------
// CXHTMLObjectElementHandler::SetPlaceHolderText
//
// Set the place-holder text to the given descriptor.
// -----------------------------------------------------------------------------
//
TBrowserStatusCode CXHTMLObjectElementHandler::SetPlaceHolderText(
        NW_LMgr_Box_t& aObjectBox, const TDesC& aPlaceHolderText)
    {
    CLMgrObjectBox*  objectBox = NULL;

    // Set the place holder text.
    objectBox = NW_LMgr_ObjectBox_GetObjectBox((NW_LMgr_ObjectBox_t&) aObjectBox);
    NW_ASSERT(objectBox != NULL);

    TRAPD(err, objectBox->SetPlaceHolderTextL(aPlaceHolderText));
    return err;
    }

// -----------------------------------------------------------------------------
// CXHTMLObjectElementHandler::SetPlaceHolderTextL
//
// Set the place-holder text to the given resource id.
// -----------------------------------------------------------------------------
//
TBrowserStatusCode CXHTMLObjectElementHandler::SetPlaceHolderText(
        NW_LMgr_Box_t& aObjectBox, TInt aResourceId)
    {
    CLMgrObjectBox*  objectBox = NULL;

    // Set the place holder text.
    objectBox = NW_LMgr_ObjectBox_GetObjectBox((NW_LMgr_ObjectBox_t&) aObjectBox);
    NW_ASSERT(objectBox != NULL);

    TRAPD(err, objectBox->SetPlaceHolderTextL(aResourceId));
    return(err);
    }

// -----------------------------------------------------------------------------
// CXHTMLObjectElementHandler::RemovePlaceHolder
//
// Removes the place holder -- puts the ObjectBox in the active state.
// -----------------------------------------------------------------------------
//
void CXHTMLObjectElementHandler::RemovePlaceHolder(NW_LMgr_Box_t& aObjectBox)
    {
    CLMgrObjectBox*  objectBox;

    // Remove the place holder
    objectBox = NW_LMgr_ObjectBox_GetObjectBox((NW_LMgr_ObjectBox_t&) aObjectBox);
    NW_ASSERT(objectBox != NULL);

    objectBox->RemovePlaceHolder();
    }

// -----------------------------------------------------------------------------
// CXHTMLObjectElementHandler::SelectElement
//
// Embedded resources are specified in an xhtml document using the object tag.  
// This tag can be very simple to quite complex.  Below are a couple of examples.
// This method determines if iElementNode or one of its descendants can be selected
// and displayed.  Once an element is selected it is stored in iSelectedElementNode.
// iSelectedElementNode is used in GetAttribute to extract attribute values from 
// the selected element.
//
// Selects *** if type-a is supported.
// <object data="some-url" type="type-a" /> ***
//
// Selects *** if type-a isn't supported and type-b is.
// <object data="some-url" type="type-a">
//   <object data="some-other-url" type="type-b" /> ***
// </object>
//
// Selects *** if type-a, type-b, and type-c aren't supported.
// <object data="some-url" type="type-a">
//   <param name="name-a" value="value-a" />
//   <param name="name-b" value="value-b" />
//   <object data="some-other-url" type="type-b">
//     <object data="some-other-url" type="type-c">
//       <param name="name-a" value="value-a" />
//       <param name="name-b" value="value-b" />
//       <img src="some-other-url" /> ***
//     </object>
//   </object>
// </object>
//
// Selects *** if application/x-shockwave-flash is supported.
// <object>
//   <embed src="stocksnapshot.swf" quality="high" salign="lt" bgcolor="#ffffff"
//       width="785" height="550" name="construct" align="" 
//       type="application/x-shockwave-flash" /> ***
// </object>
//
// Selects *** if application/x-shockwave-flash is supported.
// <object data="stocksnapshot.swf" type="application/x-shockwave-flash"> ***
//   <param name="movie" value="/images/flash/landing.swf?conf=/data/landing.xml" />
//   <param name="quality" value="high" />
//   <param name="salign" value="lt" />
//   <param name="bgcolor" value="#ffffff" />
//   <embed src="stocksnapshot.swf" quality="high" salign="lt" bgcolor="#ffffff"
//       width="785" height="550" name="construct" align="" 
//       type="application/x-shockwave-flash" />
// </object>
// -----------------------------------------------------------------------------
//
TBrowserStatusCode CXHTMLObjectElementHandler::SelectElement(void)
    {
    NW_DOM_Node_t*  node = NULL;
    TDesC*          type = NULL;
    TDesC*          data = NULL;

    NW_TRY (status) 
        {
        // Start checking from iElementNode
        node = iElementNode;

        while (node)
            {
            iSelectedElementNode = node;

            // Get the type and data attributes of this element.
            (void) GetTypeAttribute(&type);
            (void) GetSrcAttribute(&data);

            if (data)
                {
                // If it is supported then its done.
                //R->embed
                //if (ObjectUtils::IsSupported(type, data, ObjectUtils::EHandlerAny))
                //    {
                //    NW_THROW_SUCCESS(status);
                //    }
                }

            // Delete the type and data.
            delete type;
            type = NULL;
            delete data;
            data = NULL;

            // Find the first child that is either an object or embed.
            NW_DOM_Node_t*  childNode;

            childNode = NW_DOM_Node_getFirstChild(node);
            node = NULL;

            while (childNode)
                {
                switch (NW_HED_DomHelper_GetElementToken(childNode))
                    {
                    case NW_XHTML_ElementToken_object:
                    case NW_XHTML_ElementToken_embed:
                        node = childNode;
                        childNode = NULL;
                        break;

                    default:
                        childNode = NW_DOM_Node_getNextSibling(childNode);
                        break;
                    }
                }
            }

        // None of the elements are supported.
        //
        // Note: iSelectedElementNode is not cleared because because the UI
        // spec states that if none of the elements are supported then for 
        // the purpose of displaying the box it should use the last element.
        }

    /* Not currently used...
    NW_CATCH (status) 
        {
        // In case of an error, just set iSelectedElementNode to iElementNode.
        // This is probably the best state to be in if some problem occurs.
        iSelectedElementNode = iElementNode;
        }
    */
    
    NW_FINALLY 
        {
        delete type;
        delete data;

        return status;
        } NW_END_TRY
    }

// -----------------------------------------------------------------------------
// CXHTMLObjectElementHandler::GetAttribute
//
// Gets the value of the given attribute as a descriptor.
// -----------------------------------------------------------------------------
//
TBrowserStatusCode CXHTMLObjectElementHandler::GetAttribute(TInt16 aToken, TDesC** aValue)
    {
    NW_Text_t*  temp = NULL;
    HBufC*      value = NULL;

    NW_ASSERT(aValue != NULL);
    NW_ASSERT(iSelectedElementNode != NULL);

    NW_TRY (status)
        {
        *aValue = NULL;

        // Get the attribute
        status = NW_XHTML_GetDOMAttribute(iContentHandler, iSelectedElementNode, aToken, &temp);
        _NW_THROW_ON_ERROR(status);

        // Copy the result into a TDes.
        value = HBufC::New(temp->characterCount + 1);
        NW_THROW_OOM_ON_NULL(value, status);

        value->Des().Append((const TUint16*) temp->storage, temp->characterCount);
        value->Des().ZeroTerminate();
        }

    NW_CATCH (status) 
        {
        delete value;
        value = NULL;
        }
    
    NW_FINALLY 
        {
        NW_Object_Delete(temp);

        *aValue = value;
        return status;
        } NW_END_TRY
    }


// -----------------------------------------------------------------------------
// CXHTMLObjectElementHandler::GetSrcAttribute
//
// Gets the value of the source attribute as a descriptor.
// -----------------------------------------------------------------------------
//
TBrowserStatusCode CXHTMLObjectElementHandler::GetSrcAttribute(TDesC** aValue)
    {
    TDesC*      value = NULL;
    TInt16      token;
    NW_Uint16   element;

    NW_ASSERT(aValue != NULL);
    NW_ASSERT(iSelectedElementNode != NULL);

    NW_TRY (status)
        {
        *aValue = NULL;

        // If this isn't an embed or object tag, throw "no string".
        element = NW_HED_DomHelper_GetElementToken(iSelectedElementNode);
        if ((element != NW_XHTML_ElementToken_embed) && (element != NW_XHTML_ElementToken_object))
            {
            NW_THROW_STATUS(status, KBrsrDomNoStringReturned);
            }

        // Try each source variation, "data", "src" and "href".
        token = 0;
        for (int i = 0; i < 3; i++)
            {
            switch (token)
                {
                case NW_XHTML_AttributeToken_data:
                    token = NW_XHTML_AttributeToken_src;
                    break;

                case NW_XHTML_AttributeToken_src:
                    token = NW_XHTML_AttributeToken_href;
                    break;

                default:
                    token = NW_XHTML_AttributeToken_data;
                    break;
                }

            // Retry the request with a new variant.
            status = GetAttribute(token, &value);
            if (status == KBrsrSuccess)
                {
                // Found a valid variant.
                break;
                }
            }

        // If none of the source variations were found try using the object's
        // param elements.
        if (!value)
            {
            status = GetObjectAttributeMapSrc(&value);
            _NW_THROW_ON_ERROR(status);
            }
        }

    NW_CATCH (status) 
        {
        delete value;
        value = NULL;
        }
    
    NW_FINALLY 
        {
        *aValue = value;
        return status;
        } NW_END_TRY
    }


// -----------------------------------------------------------------------------
// CXHTMLObjectElementHandler::GetTypeAttribute
//
// Gets the value of the content-type attribute as a descriptor.
// -----------------------------------------------------------------------------
//
TBrowserStatusCode CXHTMLObjectElementHandler::GetTypeAttribute(TDesC** aValue)
    {
    TDesC*      value = NULL;
    TDesC*      classId = NULL;

    NW_ASSERT(aValue != NULL);
    NW_ASSERT(iSelectedElementNode != NULL);

    NW_TRY (status)
        {
        // Try to get the type from the attribute.
        status = GetAttribute(NW_XHTML_AttributeToken_type, &value);

        // If not found try to derive it from the ClassId
        /* //R->embed
        if (status != KBrsrSuccess)
            {
            // Get the ClassId attribute
            status = GetAttribute(NW_XHTML_AttributeToken_classid, &classId);
            _NW_THROW_ON_ERROR(status);

            // Look up the cooresponding content-type
            value = ObjectUtils::GetAssociatedContentType(*classId);
            NW_THROW_ON_NULL(value, status, KBrsrDomNoStringReturned)
            }
         */   
        }

    NW_CATCH (status) 
        {
        delete value;
        value = NULL;
        }
    
    NW_FINALLY
        {
        delete classId;
        classId = NULL;

        *aValue = value;
        return status;
        } NW_END_TRY
    }


// -----------------------------------------------------------------------------
// CXHTMLObjectElementHandler::GetObjectAttributeMapSrc
//
// Gets the value of the source attribute by using the classId attribute,
// the cooresponding param element and the object attribute mappings table.
// -----------------------------------------------------------------------------
//
TBrowserStatusCode CXHTMLObjectElementHandler::GetObjectAttributeMapSrc(TDesC** aValue)
    {
    NW_Text_t*    temp = NULL;
    TDesC*        classId = NULL;
    TDesC*        value = NULL;
    TDesC*        paramName = NULL;

    NW_ASSERT(aValue != NULL);
    NW_ASSERT(iSelectedElementNode != NULL);

    NW_TRY (status)
        {
        *aValue = NULL;

        // If this isn't an object tag, throw "no string".
        NW_Uint16   element;

        element = NW_HED_DomHelper_GetElementToken(iSelectedElementNode);
        if (element != NW_XHTML_ElementToken_object)
            {
            //R->embed NW_THROW_STATUS(status, KBrsrDomNoStringReturned);
            }

        // Get the ClassId attribute.
        status = GetAttribute(NW_XHTML_AttributeToken_classid, &classId);
        _NW_THROW_ON_ERROR(status);

        // Look up the cooresponding param name.
//R->embed        paramName = ObjectUtils::GetAssociatedSourceParamName(*classId);
        NW_THROW_OOM_ON_NULL(paramName, status);

        delete classId;
        classId = NULL;

        // Search for an child param element with a "name" of srcParamName 
        // and copy its "value" attribute into value.
        TPtrC16         paramNamePtr(*paramName);
        NW_DOM_Node_t*  paramElement;
        NW_String_t     nameValue;

        nameValue.storage = (NW_Byte *) paramNamePtr.Ptr();
        nameValue.length = paramNamePtr.Size() + sizeof(TInt16);

        paramElement = NW_HED_DomHelper_FindElement (&iContentHandler->domHelper,
                iSelectedElementNode, 1, NW_XHTML_ElementToken_param, 
                NW_XHTML_AttributeToken_name, &nameValue);

        if (paramElement)
            {
            status = NW_XHTML_GetDOMAttribute(iContentHandler, paramElement, 
                    NW_XHTML_AttributeToken_value, &temp);
            _NW_THROW_ON_ERROR(status);

            // Copy the result into a TDes.
            value = HBufC::New(temp->characterCount + 1);
            NW_THROW_OOM_ON_NULL(value, status);

            ((HBufC*) value)->Des().Append((const TUint16*) temp->storage, temp->characterCount);
            ((HBufC*) value)->Des().ZeroTerminate();

            NW_Object_Delete(temp);
            temp = NULL;
            }

        // Otherwise value is left NULL...

        delete paramName;
        paramName = NULL;
        }

    NW_CATCH (status) 
        {
        NW_Object_Delete(temp);
        temp = NULL;

        delete classId;
        classId = NULL;

        delete paramName;
        paramName = NULL;

        delete value;
        value = NULL;
        }
    
    NW_FINALLY 
        {
        *aValue = value;
        return status;
        } NW_END_TRY
    }

// -----------------------------------------------------------------------------
// CXHTMLObjectElementHandler::InHyperLink
//
// Returns if the <object> element is enclosed within a hyperlink
// -----------------------------------------------------------------------------
//
TBool CXHTMLObjectElementHandler::InHyperLink()
    {
    NW_DOM_ElementNode_t* node = iElementNode;
    while (node != NULL && NW_HED_DomHelper_GetElementToken(node) != NW_XHTML_ElementToken_a)
        {
        node = NW_DOM_Node_getParentNode(node);
        }
    return (node  != NULL);
    }

