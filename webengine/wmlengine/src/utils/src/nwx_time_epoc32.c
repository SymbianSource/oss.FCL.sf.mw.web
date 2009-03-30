/*
* Copyright (c) 1999 Nokia Corporation and/or its subsidiary(-ies).
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

/*****************************************************************
**    File name:  nwx_time_epoc32.c
**    Part of: Time
**    Description:   Provides interfaces to time routines.
**       Note: None of these is thread safe.
******************************************************************/

/*
**-------------------------------------------------------------------------
**  Include Files
**-------------------------------------------------------------------------
*/
#include <time.h>
#include "nwx_time.h"
#include "nwx_settings.h"

/*
**-------------------------------------------------------------------------
**  Constants
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  Internal Types
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  Macros
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  Internal Prototypes
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  File Scoped Static Variables
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  Global Variable Definitions
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  Internal Functions
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  External Public (Exported) Functions
**-------------------------------------------------------------------------
*/

/*****************************************************************
**  Name:  NW_Time_GetTime
**  Description:  Get current system time
**  Parameters:   none
**  Return Value: sytem time in seconds since Jan 1st, 1970
******************************************************************/
NW_Time_t NW_Time_GetTime()
{
  time_t ltime = time(NULL);
  time_t utctime = mktime(gmtime(&ltime));  // on EPOC this is a signed int

  // NW_Time_t is an NW_Int32, so the typecast is fine.
  return (NW_Time_t)(utctime);
}

/*****************************************************************
**  Name:  NW_Time_GetTimeMSec
**  Description:  Get current system time
**  Parameters:   none
**  Return Value: sytem time in mSecs since Jan 1st, 1970
**  Note: this timer will roll-over every 49.7 days.
******************************************************************/
NW_Uint32 NW_Time_GetTimeMSec()
{
  // EPOC32 does seem to have any way of getting the current msec
  // value.
  NW_Uint32 sec = (NW_Uint32)time(NULL);
  NW_Uint32 msec = sec * 1000;

  return msec;
}
