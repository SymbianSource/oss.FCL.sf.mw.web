/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides one place for variation in Browser Engine.
*
*/



#ifndef BROWSERENGINEVARIATION_H
#define BROWSERENGINEVARIATION_H

//  INCLUDES

/*
** Includes
*/
#include "nwx_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __SERIES60_27__
NW_Bool NewFeatureEnabled() { return NW_FALSE; }
#else 
#ifdef __SERIES60_28__ 
NW_Bool NewFeatureEnabled() { return NW_FALSE; }
#else
NW_Bool NewFeatureEnabled() { return NW_TRUE; }
#endif // __SERIES60_28__ 
#endif // __SERIES60_27__ 


#ifdef __cplusplus
} /* extern "C" */
#endif


#endif      // BROWSERENGINEVARIATION_H

// End of File
