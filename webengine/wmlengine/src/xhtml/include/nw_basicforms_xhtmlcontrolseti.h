/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef NW_XHTML_XHTMLCONTROLSETI_H
#define NW_XHTML_XHTMLCONTROLSETI_H

#include "nw_object_dynamici.h"
#include "nw_basicforms_xhtmlcontrolset.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   virtual method implementation prototypes
 * ------------------------------------------------------------------------- */
extern
TBrowserStatusCode
_NW_XHTML_ControlSet_Construct (NW_Object_Dynamic_t* dynamicObject,
                                va_list* argp);

extern
void
_NW_XHTML_ControlSet_Destruct (NW_Object_Dynamic_t* dynamicObject);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_Tempest_XHTML_XHTMLControlsetI_h */
