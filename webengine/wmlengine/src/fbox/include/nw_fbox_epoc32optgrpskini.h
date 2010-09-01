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


#ifndef NW_FBOX_EPOC32OPTGRPSKINI_H
#define NW_FBOX_EPOC32OPTGRPSKINI_H

#include "nw_fbox_optgrpskini.h"
#include "nw_fbox_epoc32optgrpskin.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* ------------------------------------------------------------------------- *
   virtual method implementation prototypes
 * ------------------------------------------------------------------------- */

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_Epoc32OptGrpSkin_AddOption(NW_FBox_OptGrpSkin_t* optGrpSkin,
                                    NW_FBox_OptionBox_t* optionBox);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_Epoc32OptGrpSkin_AddOptGrp(NW_FBox_OptGrpSkin_t* optGrpSkin,
                                    NW_FBox_OptGrpBox_t* optGrpBox);


#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_FBOX_EPOC32OPTGRPSKINI_H */
