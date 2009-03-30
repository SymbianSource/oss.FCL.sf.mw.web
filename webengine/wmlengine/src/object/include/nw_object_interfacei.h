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


#ifndef _NW_Object_InterfaceI_h_
#define _NW_Object_InterfaceI_h_

#include "nw_object_interface.h"
#include "nw_object_secondaryi.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   protected symbols
 * ------------------------------------------------------------------------- */
#define _NW_Object_Interface_QueryInterface \
  _NW_Object_Core_QuerySecondary

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_Object_InterfaceI_h_ */
