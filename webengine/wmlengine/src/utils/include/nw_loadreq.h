/*
* Copyright (c) 1999 - 2001 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef NW_LOADREQ_H
#define NW_LOADREQ_H

/*
** Includes
*/
#include "nwx_defs.h"

#include "nw_nvpair.h"
#include "nwx_buffer.h"
#include "nwx_http_header.h"
#include "urlloader_urlresponse.h"
#include "nw_hed_appservices.h"
#include "BrsrStatusCodes.h"
/*
** Type Declarations
*/

/*
** Preprocessor Macro Definitions
*/

#ifdef __cplusplus
extern "C" {
#endif

/*
** Global Function Declarations
*/

/*****************************************************************

  Name: NW_LoadReq_Create

  Description:  
    Create loader data structures for an Http load request.
    Helper routine to create request for the loaders. 

  Parameters:   
    url             - in: url to load.
    postMethodFlag  - in: flag indicating HTTP POST method or not.
    referer         - in: referer to the url.
    acceptCharset   - in: character set gateway will accept.
    postfields      - in: postdata name/value pairs.
                      Note: Not const because have to change to iterate over.
    enctype         - in: urlencoded or multipart/form-data.
    docCharset      - in: charset encoding for document.
    header          - in/out: if empty (NULL) a new header is created.
                              if not empty, a copy is made and added to.
    resultUrl       - out: url created for request
    method          - out: post or get.
    body            - out: body for load request.
    appServices ?
    appCtx ?

  Return Value: KBrsrSuccess
                KBrsrFailure
                KBrsrUnsupportedFormCharset
                KBrsrOutOfMemory

  Notes:
     cacheMode not needed for this helper function.

******************************************************************/
TBrowserStatusCode NW_LoadReq_Create ( const NW_Ucs2 *url,
                                const NW_Bool postMethodFlag, 
                                const NW_Ucs2 *referer, 
                                const NW_Ucs2 *acceptCharset,
                                NW_NVPair_t   *postfields,  /* Can't be const. */
                                const NW_Ucs2 *enctype,
                                const NW_Http_CharSet_t docCharset,
                                NW_Http_Header_t **header,  /* Start output params */
                                NW_Ucs2       **resultUrl,
                                NW_Uint8      *method,
                                NW_Buffer_t   **body,
                                NW_HED_AppServices_t * appServices,
                                void * appCtx);


/************************************************************************
 
  Function: NW_LoadReq_BuildQuery

  Description:  Construct a query from the url and escaped_postdata, 
                and places the result in *query.  It is the caller's 
                responsibility to delete the resulting string.
 
                If the number of postfields is 0, the query is just a 
                new copy the original url. This prevents us from 
                appending a lone '?' at the end of a 'canned' query 
                url like 
 
                "http://waplabdc/foo.wml?name=bar;value=foo"
 
  Parameters:   url        - in: pointer to the URL
                postfields - in: pointer to the postfield data
                resultUrl - out: constructed result URL.
                appServices ?
                appCtx ?
    
  Return Value: KBrsrSuccess
                KBrsrFailure
                KBrsrOutOfMemory
                  
**************************************************************************/
TBrowserStatusCode NW_LoadReq_BuildQuery(const NW_Ucs2 *url,
                                  NW_NVPair_t *postfields, 
                                  NW_Ucs2 **resultUrl,
                                  NW_HED_AppServices_t * appServices,
                                  void * appCtx);

/**
* Checks the encoding type to see if it is for a multipart request.
* @since 2.5
* @param aEncodingType Ptr to encoding type string to be checked.
* @return   NW_TRUE if multipart.
*           NW_FALSE if not multipart.
*/
NW_Bool NW_LoadReq_IsMultipart( const NW_Ucs2* aEncodingType );

/**
* Determines whether or not the current part of the multipart post is for a
* file to be uploaded or not.
* @param aValue Ptr to value of the variable.
* @return   NW_TRUE if the part is for file to be uploaded.
*           NW_FALSE if not a file.
*/
NW_Bool NW_LoadReq_IsUploadFile( const NW_Ucs2 *aValue );


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NW_NVPAIR_H */


