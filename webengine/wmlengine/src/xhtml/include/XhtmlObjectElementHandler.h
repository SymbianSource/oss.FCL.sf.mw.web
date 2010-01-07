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



#ifndef CXHTMLOBJECTELEMENTHANDLER__H
#define CXHTMLOBJECTELEMENTHANDLER__H

// INCLUDES
#include <e32def.h>
#include <e32base.h>

#include "nwx_defs.h"
#include <nw_dom_node.h>

#include "HedLoadObserver.h"
#include "LMgrBoxDestructionListener.h"
#include "ObjectUtils.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct NW_HED_ContentHandler_s NW_HED_ContentHandler_t;
typedef struct NW_XHTML_ContentHandler_s NW_XHTML_ContentHandler_t;
typedef struct NW_XHTML_ElementHandler_s NW_XHTML_ElementHandler_t;
typedef struct NW_LMgr_ContainerBox_s NW_LMgr_ContainerBox_t;
typedef struct NW_LMgr_Box_s NW_LMgr_Box_t;
typedef struct NW_Evt_Event_s NW_Evt_Event_t;
typedef struct NW_Text_Abstract_s NW_Text_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

// CLASS DECLARATION

// TODO: This class should be derived from CElementHandler once the browser is
//       converted to c++.

class CXHTMLObjectElementHandler : public CBase, public MLMgrBoxDestructionListener, 
        public MHEDLoadObserver
    {
    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CXHTMLObjectElementHandler* NewL(const NW_XHTML_ElementHandler_t* aElementHandler,
                NW_XHTML_ContentHandler_t* aContentHandler, NW_DOM_ElementNode_t* aElementNode);

        /**
        * Destructor.
        */
        virtual ~CXHTMLObjectElementHandler();


    public:  // Functions from MLMgrBoxDestructionListener
        /**
        * Recieves notifications when the box has been destroyed. 
        *
        * @since 2.6
        * @return void
        */
        void Destroyed();


    public:  // MHEDLoadObserver
        /**
        * Passes a HEAD response to the observer.
        *
        * @param aTransactionId the transaction id.
        * @param aResponse the HEAD response to be processed.
        * @param aUrlRequest the url request.
        * @param aClientData the client data past with the request.
        * @return void.
        */
        void HeadCompleted(TUint16 aTransactionId, const NW_Url_Resp_t& aResponse, 
                const NW_HED_UrlRequest_t& aUrlRequest, void* aClientData);

        /**
        * Passes a chunk of a response to the observer.
        *
        * @param aTransactionId the transaction id.
        * @param aChunkIndex the chunk count of this call, aChunkIndex > 0
        * @param aResponse the HEAD response to be processed.
        * @param aUrlRequest the url request.
        * @param aClientData the client data past with the request.
        * @return void.
        */
        void ChunkReceived(TUint16 aTransactionId, TUint32 aChunkIndex, 
                const NW_Url_Resp_t& aResponse, const NW_HED_UrlRequest_t& aUrlRequest, 
                void* aClientData);

        /**
        * Called when a response completes successfully or in error.
        *
        * @param aStatusClass the type of status-code found in aStatus -- 
                 BRSR_STAT_CLASS_HTTP or BRSR_STAT_CLASS_GENERAL.
        * @param aStatus the final load status of the request.
        * @param aTransactionId the transaction id.
        * @param aUrlRequest the url request.
        * @param aClientData the client data past with the request.
        * @return void.
        */
        void LoadCompleted(TInt16 aStatusClass, TBrowserStatusCode aStatus, 
                TUint16 aTransactionId, const NW_HED_UrlRequest_t& aUrlRequest, 
                void* aClientData);


    public:  // New Methods
        /**
        * Initializes the ElementHandler.
        *
        * @return status code
        */
        virtual TBrowserStatusCode Initialize(void);

        /**
        * Creates the box-tree associated with the tag.
        *
        * @param aParentBox the parent to attach the new box-tree to.
        * @return status code
        */
        virtual TBrowserStatusCode CreateBoxTree(NW_LMgr_ContainerBox_t& aParentBox);  

        /**
        * Processes events targeted at boxes created by the ElementHandler.
        *
        * @param aBox the target box.
        * @param aEvent the event.
        * @return NW_LMgr_EventAbsorbed or NW_LMgr_EventNotAbsorbed
        */
        virtual NW_Uint8 ProcessEvent(const NW_LMgr_Box_t& aBox, const NW_Evt_Event_t& aEvent);

        /**
        * This is a special purpose method to get the element node 
        * associated with this handler.
        *
        * @return the elementNode associated with this handler.
        */
        const NW_DOM_ElementNode_t* ElementNode(void) const;

        /**
        * This is a special purpose method to get the "selected" element.
        * See the comment on SelectElement.
        *
        * @return the selected elementNode.
        */
        const NW_DOM_ElementNode_t* SelectedElementNode(void) const;

        /**
        * Applies the content type and length properties on the given box.  This is 
        * used in the download dialog box.
        *
        * @param aBox the box to apply the properties on.
        * @return status code.
        */
        TBrowserStatusCode ApplyInfoProperties(NW_LMgr_Box_t& aBox);


    private:  // Private Methods
        /**
        * C++ default constructor.
        */
        CXHTMLObjectElementHandler(const NW_XHTML_ElementHandler_t* aElementHandler,
                NW_XHTML_ContentHandler_t* aContentHandler, NW_DOM_ElementNode_t* aElementNode);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(void);

        /**
        * Loads the resource associated with object tag.
        *
        * @param aType the mime-type of the of the resource or NULL 
        *        if the type isn't known.
        * @param aUrl the url of the of the resource or NULL.
        * @param aHandlerType Type of the load
        * @return status code
        */
        TBrowserStatusCode LoadObjectResource(const TDesC* aType, 
                const TDesC* aUrl, ObjectUtils::THandlerType aHandlerType);

        /**
        * Loads the HEAD information of the resource associated with object tag.
        *
        * @return status code
        */
        TBrowserStatusCode LoadObjectInfo(void);

        /**
        * Returns the associated content handler or NULL if one isn't found.
        *
        * @return the associated content handler 
        */
        NW_HED_ContentHandler_t* AssociatedContentHandler(void);

        /**
        * Set the place-holder text to the given descriptor.
        *
        * @param aObjectBox the Object box. 
        * @param aPlaceHolderText the string.
        *
        * @return status code 
        */
        TBrowserStatusCode SetPlaceHolderText(NW_LMgr_Box_t& aObjectBox, 
                const TDesC& aPlaceHolderText);

        /**
        * Set the place-holder text to the given resource id.
        *
        * @param aObjectBox the Object box. 
        * @param aResourceId the string's resource id.
        *
        * @return status code 
        */
        TBrowserStatusCode SetPlaceHolderText(NW_LMgr_Box_t& aObjectBox, 
                TInt aResourceId);

        /**
        * Removes the place holder -- puts the Object box in the active state.
        *
        * @param aObjectBox the Object box. 
        *
        * @return void 
        */
        void RemovePlaceHolder(NW_LMgr_Box_t& aObjectBox);

        /**
        * CXHTMLObjectElementHandler::SelectElement
        *
        * Embedded resources are specified in an xhtml document using the object tag.  
        * This tag can be very simple to quite complex.  See the cpp file for some examples.
        * This method determines if iElementNode or one of its descendants can be selected
        * and displayed.  Once an element is selected it is stored in iSelectedElementNode.
        * iSelectedElementNode is used in GetAttribute to extract attribute values from 
        * the selected element.
        *
        * @return status code. 
        */
        TBrowserStatusCode SelectElement(void);

        /**
        * Gets the value of the given attribute as a descriptor.
        *
        * @param aToken identifies the attribute to get.
        * @param aValue upon success it equals the value of the attribute.
        *        The caller adopts aValue.
        * @return status code
        */
        TBrowserStatusCode GetAttribute(TInt16 aToken, TDesC** aValue);

        /**
        * Gets the value of the source attribute as a descriptor.
        *
        * @param aValue upon success it equals the value of the src attribute.
        *        The caller adopts aValue.
        * @return status code
        */
        TBrowserStatusCode GetSrcAttribute(TDesC** aValue);

        /**
        * Gets the value of the content-type attribute as a descriptor.
        *
        * @param aValue upon success it equals the value of the src attribute.
        *        The caller adopts aValue.
        * @return status code
        */
        TBrowserStatusCode GetTypeAttribute(TDesC** aValue);

        /**
        * Gets the value of the source attribute by using the classId attribute,
        * the cooresponding param element and the object attribute mappings table.
        *
        * @param aValue upon success it equals the value of the src attribute.
        *        The caller adopts aValue.
        * @return status code
        */
        TBrowserStatusCode GetObjectAttributeMapSrc(TDesC** aValue);

        /**
        * Returns the cooresponding value (either param-name or content-type) given
        * the class-id.
        *
        * @param aClassId the class-id used to map to the requested value.
        * @param aValueSelector used to select the cooresponding value 
        *                       (either KParamName or KContentType)
        * @return the value or NULL on out of memory.
        */
        TDesC* GetObjectAttributeMappedValue(const TDesC& aClassId, TInt aValueSelector);

        /**
        * Returns if the <object> element is enclosed within a hyperlink
        *
        * @return If the <object> element is enclosed within a hyperlink
        */
        TBool InHyperLink();

    private:  // Data
        const NW_XHTML_ElementHandler_t*    iElementHandler;
	      NW_XHTML_ContentHandler_t*          iContentHandler;
        NW_DOM_ElementNode_t*               iElementNode;
        NW_DOM_ElementNode_t*               iSelectedElementNode;

        NW_LMgr_Box_t*                      iObjectBox;
        ObjectUtils::THandlerType           iLoadedHandlerType;
        TBool                               iIsDownloading;

        TBool                               iObjectInfoValidated;
        HBufC*                              iContentType;
        HBufC*                              iContentLength;
        TInt16                              iTransactionId;
    };

#endif  // CXHTMLOBJECTELEMENTHANDLER__H
