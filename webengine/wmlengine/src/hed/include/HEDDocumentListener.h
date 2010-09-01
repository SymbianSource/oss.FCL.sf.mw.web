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
* Description:  
*
*/


#ifndef HEDDOCUMENTLISTENER_H
#define HEDDOCUMENTLISTENER_H

//  INCLUDES
#include "BrsrTypes.h"
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES
#ifdef __cplusplus
extern "C" 
  {
#endif 
  typedef
  NW_Bool (ReportErrorCallback) (NW_HED_DocumentRoot_t* documentRoot, NW_Int16 errorClass, NW_WaeError_t error);
#ifdef __cplusplus
  }
#endif 

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Base class for.
*
*  @lib hed.lib
*  @since 2.6
*/
class MHEDDocumentListener
{

  public:
    /**
    * 
    * @since 2.6
    * @param 
    * @return
    */
    virtual void NewDocument (NW_HED_DocumentRoot_t* documentRoot, NW_HED_UrlRequest_t* urlRequest, NW_Bool aDestroyBoxTree) = 0;

    /**
    * 
    * @since 2.6
    * @param
    * @return
    */
    virtual void DestroyPreviousDocument() = 0;

    /**
    * 
    * @since 2.6
    * @param
    * @return
    */
    virtual void RestoreDocument () = 0;

    /**
    * 
    * @since 2.6
    * @param
    * @return NW_Bool
    */
    virtual NW_Bool LoadEnd (NW_HED_DocumentRoot_t* documentRoot, NW_Int16 errorClass, NW_WaeError_t error) = 0;

    /**
    * 
    * @since 2.6
    * @param 
    * @return TBrowserStatusCode
    */
    virtual TBrowserStatusCode DocumentChanged( NW_HED_DocumentRoot_t* documentRoot, 
              CActive::TPriority aMessagePriority ) = 0;

    /**
    * 
    * @since 2.6
    * @param 
    * @return TBrowserStatusCode
    */
    virtual TBrowserStatusCode DocumentChangedPartial( NW_HED_DocumentRoot_t* documentRoot,
              CActive::TPriority aMessagePriority ) = 0;

    /**
    * 
    * @since 2.6
    * @param
    * @return TBrowserStatusCode
    */
    virtual TBrowserStatusCode SwitchLayout (NW_Bool aVerticalLayout, NW_Bool aReformatBoxTree) = 0;

    /**
    * 
    * @since 2.6
    * @param 
    * @return TBrowserStatusCode
    */
    virtual TBrowserStatusCode IsActive (NW_HED_DocumentRoot_t* documentRoot, NW_Bool state) = 0;

    /**
    * 
    * @since 2.6
    * @param 
    * @return TBrowserStatusCode
    */
    virtual TBrowserStatusCode ShowNamedBox (NW_LMgr_Box_t* boxTree, const NW_Text_t* targetName) = 0;

    /**
    * 
    * @since 2.6
    * @param 
    * @return TBrowserStatusCode
    */
    virtual TBrowserStatusCode SetAnchorName (NW_Text_t* anchorName) = 0;

    /**
    * 
    * @since 2.6
    * @param transactionId
    * @return TBrowserStatusCode
    */
    virtual TBrowserStatusCode LoadProgressOn (NW_Uint16 transactionId) = 0;

    /**
    * 
    * @since 2.6
    * @param transactionId
    * @return TBrowserStatusCode
    */
    virtual TBrowserStatusCode LoadProgressOff (NW_Uint16 transactionId) = 0;

    /**
    * 
    * @since 2.6
    * @param 
    * @return NW_Bool
    */
    virtual NW_Bool ReportError (NW_HED_DocumentRoot_t* documentRoot, NW_Int16 errorClass, NW_WaeError_t error,
                                 ReportErrorCallback* callback) = 0;

    /**
    * 
    * @since 2.6
    * @param box
    * @return NW_LMgr_RootBox_t*
    */
    virtual NW_LMgr_RootBox_t* GetRootBox () = 0;

    /**
    * Request the host applicaion to open the URL in a new window
    * @since 3.0
    * @param aUrl The Url of the request to be done in the new window
    * @param aTargetName The name of the new window
    * @param aReason The reason for opening the new window
    * @param aMethod The method to be used for fetching the supplied url
    * @param aContentType If the method is POST, this is the content type of the data to be posted 
    * @param aPostData If the method is POST, this is the data to be posted 
    * @param aBoundary The boundary used if the content is multipart/form-data
    * @param aReserved For future use
    * @return Return Value is for future reference and is currently ignored
    */
    virtual void WindowOpenAndPostL(TUint16* url, TUint16* target, TUint32 reason, 
        TUint16 method, TDesC8* contentType, TDesC8* postData,
        TDesC8* aBoundary, void* reserved) = 0;

    /**
    * 
    * @since 3.0
    * @param url
    * @param target
    * @param reason
    */
    virtual void WindowOpenL(TUint16* url, TUint16* target, TUint32 reason) = 0;


    /**
    * 
    * @since 3.0
    * @return if the Browser is exiting
    */
    virtual TBool Exiting() = 0;


    /**
    * 
    * @since 3.0
    * @return if the file processed by document handler
    */
    virtual void SetFileOpenedByDocHandler( TBool aFileOpenedByDocHandler ) = 0;

};

#endif      // HEDDOCUMENTLISTENER_H

// End of File
