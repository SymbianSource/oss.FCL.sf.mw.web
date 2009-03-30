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


#ifndef NW_HED_CONTEXTVISITORI_H
#define NW_HED_CONTEXTVISITORI_H

#include "nw_object_dynamici.h"
#include "nw_hed_contextvisitor.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   virtual method implementation prototypes
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_ContextVisitor_Construct (NW_Object_Dynamic_t* dynamicObject,
                               va_list* argp);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_HED_CONTEXTVISITORI_H */


