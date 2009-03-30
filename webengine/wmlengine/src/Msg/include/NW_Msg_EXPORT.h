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


#ifndef _NW_Msg_EXPORT_h_
#define _NW_Msg_EXPORT_h_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/* ------------------------------------------------------------------------- */
#ifndef NW_MSG_EXPORT
#ifdef NW_IMPORT
#define NW_MSG_EXPORT NW_IMPORT
#else /* NW_IMPORT */
#define NW_MSG_EXPORT extern
#endif /* NW_IMPORT */
#endif /* NW_MSG_EXPORT */

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_Msg_EXPORT_h_ */
