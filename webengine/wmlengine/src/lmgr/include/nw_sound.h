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
* Description:  Defines soundstart and background sound types.
*
*/



#ifndef NW_SOUND_H
#define NW_SOUND_H

// INCLUDES
#include "BrsrTypes.h"
#include "nwx_defs.h"

// CONSTANTS

// MACROS

// DATA TYPES

// Soundstart types. Soundstart is a customer proprietary feature of anchor tags.
// Example:
//  <a href="some_url" soundstart=select|focus src="soundstart_url" volume=high|middle|low loop=N|infinite>
typedef enum
    {
    ESoundstartActionOnSelect,
    ESoundstartActionOnFocus,
    } TSoundstartAction;

typedef enum
    {
    ESoundstartVolumeCurrent = 0, // means use current device volume setting
    ESoundstartVolumeHigh,
    ESoundstartVolumeMiddle,
    ESoundstartVolumeLow,
    } TSoundstartVolume;

typedef struct
    {
    TSoundstartAction action;
    NW_Int16 loop;
    TSoundstartVolume volume;
    NW_Ucs2* url;
    } TSoundstartState;


// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

#endif
