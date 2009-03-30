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
* Description:  
*
*/


#ifndef CREFERRERHELPER_H
#define CREFERRERHELPER_H

//  INCLUDES
#include "nw_hed_documentroot.h"
#include "nw_hed_urlrequest.h"

#ifdef __cplusplus
extern "C" {
#endif

//  FUNCTION
void SetReferrerHeader(NW_HED_UrlRequest_t* urlRequest,
					   NW_Ucs2* referrerUrl);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif      // CREFERRERHELPER_H

// End of File
