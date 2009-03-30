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


#ifndef __TENCODINGMAPPING_H
#define __TENCODINGMAPPING_H

#include "BrsrTypes.h"

typedef struct
    {
  NW_Uint32 uid;
  NW_Uint16 ianaEncoding;
  NW_Uint16 internalEncoding;
  NW_Uint8* charset;
  NW_Bool needsConvert;
}SupportedCharset;

extern const SupportedCharset supportedCharset[];

#endif // __TENCODINGMAPPING_H