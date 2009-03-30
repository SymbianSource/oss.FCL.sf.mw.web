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


/***************************************************************************
**   File: nwx_time_h
**   Subsystem Name:Time
**   Purpose:   Provides Time manipulation routines in a platform independent
**              manner.
***************************************************************************/
#ifndef NWX_TIME_H
#define NWX_TIME_H

#ifdef __cplusplus
extern "C" {
#endif

/*
** Includes
*/

#include "nwx_defs.h"

/*
** Type Declarations
*/

/* UTC (GMT) system time in seconds since Jan 1st, 1970 */
typedef NW_Uint32 NW_Time_t;

/*UTC Time*/
typedef struct {
  NW_Uint16 Year;          /* eg. 1999, 2000, ... */
  NW_Uint16 Month;         /* Jan = 1, Feb = 2, ... */
  NW_Uint16 Day;           /* 1st = 1, ... 31st = 31 */
  NW_Uint16 DayOfWeek;     /* Sun = 0, Mon = 1, ... */
  NW_Uint16 Hour;          /* 0 to 23 */
  NW_Uint16 Minute;        /* 0 to 59 */
  NW_Uint16 Second;        /* 0 to 59 */
  NW_Uint16 Milliseconds;  /* 0 to 999 */
} NW_Time_Tm_t;


/*
** Global Function Declarations
*/

/* Create a new time structure */
NW_Time_Tm_t *NW_Time_Tm_New();

/* Free a time structure */
void NW_Time_Tm_Delete(NW_Time_Tm_t *time);

/* Convert time from NW_Timer_t to NW_Time_Tm_t format */
NW_Time_Tm_t *NW_Time_CvtToTimeTm(const NW_Time_t time);

/*Convert time from NW_Time_Tm_t to NW_Timer_t format */
NW_Time_t NW_Time_CvtFromTimeTm(const NW_Time_Tm_t *time);

/* Get UTC (GMT) time in Seconds since Jan 1st, 1970 */
NW_Time_t NW_Time_GetTime(void);

/* Get free running time in Milliseconds */
NW_Uint32 NW_Time_GetTimeMSec(void);



#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /* NWX_TIME_H */
