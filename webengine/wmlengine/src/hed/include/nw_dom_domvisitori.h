/*
* Copyright (c) 2000 - 2001 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef NW_CXML_DOM_DOMVISITORI_H
#define NW_CXML_DOM_DOMVISITORI_H

#include "nw_object_dynamici.h"
#include "nw_dom_domvisitor.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   virtual method implementation prototypes
 * ------------------------------------------------------------------------- */
extern
TBrowserStatusCode
_NW_cXML_DOM_DOMVisitor_Construct (NW_Object_Dynamic_t* dynamicObject,
                                va_list* argp);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_CXML_DOM_DOMVISITORI_H */


