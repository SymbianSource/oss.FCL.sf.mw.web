/*
* Copyright (c) 2000 - 2004 Nokia Corporation and/or its subsidiary(-ies).
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


// This file was given to Nokia Boston by HAMETVAARA. It forwards content
// which is not handled by the browser to the OS document handler.

// ContentLoader.h: interface for the CContentLoader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(CONTENTLOADER_H)
#define CONTENTLOADER_H

#include <e32base.h>            // Basic Epoc stuff
#include "nwx_defs.h"
#include "urlloader_urlresponse.h"
#include "nw_hed_documentroot.h"
#include "BrsrStatusCodes.h"

// FORWARD DECLARATIONS
class CEikProcess;
class CErrorUI;

class CContentLoader : public CBase  
{
public:
    /*
    * Constructor
    * @param aProcess An instance of CEikProcess. Needed for embedding.
    **/ 
	CContentLoader( CEikProcess* aProcess );

    /**
    * Destructor
    **/
	virtual ~CContentLoader();

    /**
    * Handler function. 
    * <ul>
    *   <li>Constructs DocumentHandler and passes content to it.</li>
    *   <li>Tries to extract the file name from the request url in order to
    *       get a meaninful name for content. The file name extension also
    *       helps Epoc recognizers. <b>What to do if the file name is 
    *       data.php and contains a wav sound clip?</b></li>
    *   <li>Leaves through any system wide error. Catched by the app framework.</li>
    * </ul>
    */
    void HandleL( HBufC* aUrl, HBufC8* aCharset, HBufC8* aContentType,
                  NW_HED_DocumentRoot_t* aDocRoot,
                  const TDesC& aFileName, TBool* aNeedDelete );
    RFs               iRfs;
    HBufC*            iFileName;

private:

    void SetMimeTypeL( TUint8* aContentType, TDes8& mimetypestr );

    CEikProcess*      iProcess;

};

TBrowserStatusCode HandleContent(HBufC* aUrl, HBufC8* aCharset, HBufC8* aContentType,
                                 NW_HED_DocumentRoot_t* aDocRoot,
                                 const TDesC& aFileName, TBool* aNeedDelete);

void DeletePrevContentL(struct NW_HED_DocumentRoot_s* aDocRoot);

#endif // !defined(CONTENTLOADER_H)
