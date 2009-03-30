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

/*
    $Workfile: wml_elements.h $

    Purpose:

        Declares the main WML structures: types deck_t and card_t. Also declares 
        functions for parsing global WML NW_Byte masks.
*/

#ifndef WML_ELEMENTS_H
#define WML_ELEMENTS_H

#include "nw_wml_decoder.h"
#include "nw_wml_elements.h"
#include "nw_wml_defs.h"

#include "nw_hed_domhelper.h"

/* typedef struct NW_Wml_Element_s NW_Wml_Element_t; */

struct NW_Wml_Element_s {
  NW_Int16 id;
  NW_DOM_Node_t* domElement;
};



#endif
