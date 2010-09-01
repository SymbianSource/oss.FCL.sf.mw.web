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


/*****************************************************************
**    File name:  nwx_time_utils.c
**    Part of: Time
**    Description:   Time utilities. Platform independent
**    Note: None of these is thread safe.
******************************************************************/

/*
**-------------------------------------------------------------------------
**  Include Files
**-------------------------------------------------------------------------
*/

#include "nwx_defs.h"
#include "nwx_time.h"
#include "nwx_math.h"
#include "nwx_string.h"

/*lint -save -esym(578, time), -e650 Declaration of symbol hides symbol, Constant out of range for operator*/

/*
**-------------------------------------------------------------------------
**  Constants
**-------------------------------------------------------------------------
*/
#define SECONDS_IN_MINUTE   60
#define SECONDS_IN_HOUR     (SECONDS_IN_MINUTE * 60)
#define SECONDS_IN_DAY      (SECONDS_IN_HOUR * 24)
#define SECONDS_IN_YEAR     (SECONDS_IN_DAY * 365)
#define SECONDS_IN_4YEARS   (SECONDS_IN_YEAR * 4 + SECONDS_IN_DAY)

//#define SUNDAY      0
//#define MONDAY      1
//#define TUESDAY     2
//#define WEDNESDAY   3
#define THURSDAY    4
//#define FRIDAY      5
//#define SATURDAY    6

#define DAYS_IN_WEEK 7

//#define JANUARY     1
//#define FEBRUARY    2
//#define MARCH       3
//#define APRIL       4
//#define MAY         5
//#define JUNE        6
//#define JULY        7
//#define AUGUST      8
//#define SEPTEMBER   9
//#define OCTOBER     10
//#define NOVEMBER    11
//#define DECEMBER    12

/*#define MONTHS_IN_YEAR 12*/

/*const char * const ABRV_MONTH_STRS[] = { "Jan",
                                  "Feb",
                                  "Mar",
                                  "Apr",
                                  "May",
                                  "Jun",
                                  "Jul",
                                  "Aug",
                                  "Sep",
                                  "Oct",
                                  "Nov",
                                  "Dec" };*/

/*const char * const ABRV_DAYOFWEEK_STRS[] = { "Sun",
                                      "Mon",
                                      "Tue",
                                      "Wed",
                                      "Thu",
                                      "Fri",
                                      "Sat" };*/

/*#define RFC1123TIMESTR_POS_DAYOFWEEK  0
#define RFC1123TIMESTR_POS_DAY        5
#define RFC1123TIMESTR_POS_MONTH      8
#define RFC1123TIMESTR_POS_YEAR       12
#define RFC1123TIMESTR_POS_HOUR       17
#define RFC1123TIMESTR_POS_MINUTE     20
#define RFC1123TIMESTR_POS_SECOND     23
#define RFC1123TIMESTR_LEN            29*/

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
static NW_Uint16 NW_Time_DayOfWeek(const NW_Time_t time)
{
    NW_Uint32 days;

    days = (NW_Uint32)time / SECONDS_IN_DAY;
    days += THURSDAY;       /* Jan 1, 1970 is a Thursday */
    
    return (NW_Uint16)(days % DAYS_IN_WEEK);
}


/*
**-------------------------------------------------------------------------
**  External Public (Exported) Functions
**-------------------------------------------------------------------------
*/

/*****************************************************************
**  Name:  NW_Time_Tm_New
**  Description:  Allocates a new NW_Time_Tm_t structure
**  Parameters:   None
**  Return Value: pointer to the new NW_Time_tm_t or NULL
******************************************************************/
NW_Time_Tm_t *NW_Time_Tm_New(void)
{
  NW_Time_Tm_t *time = NULL;

  time = (NW_Time_Tm_t*) NW_Mem_Malloc(sizeof(NW_Time_Tm_t));

  return time;
}

/*****************************************************************
**  Name:  NW_Time_Tm_Free
**  Description:  Deallocates a NW_Time_Tm_t structure
**  Parameters:   *time - pointer to the NW_Time_Tm_t struct to free
**  Return Value: void
******************************************************************/
void NW_Time_Tm_Delete(NW_Time_Tm_t *time)
{
  NW_Mem_Free(time);
  time = NULL;

  return;
}

/*****************************************************************
**  Name:  NW_Time_CvtToTimeTm
**  Description:  Convert time from NW_Timer_t to NW_Time_Tm_t format
**  Parameters:   time - time to convert
**  Return Value: pointer to the new NW_Time_Tm_t or NULL 
**  Note: caller must call NW_Time_Tm_Free
******************************************************************/
NW_Time_Tm_t *NW_Time_CvtToTimeTm(const NW_Time_t in_time)
{
    NW_Uint32 remainder;
    NW_Uint16 leap_years, year, julday, month, day, hour, minute, second;
    NW_Uint16 daysInMonth[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
    NW_Uint32 baseYear;
    NW_Time_Tm_t *time_tm = NULL;
    NW_Time_t time;

    time = in_time;
    time_tm = NW_Time_Tm_New();
    if (time_tm == NULL) {
      return NULL;
    }

    /* Find the number of Years in the time */
    year = (NW_Uint16)(time / SECONDS_IN_YEAR);
    if (year >= 2)
    {
        /* Past Jan 1, 1972 (which is the first leap year after 1970) */
        baseYear = 1972;
        time -= 2 * SECONDS_IN_YEAR;
    }
    else
    {
        /* This is between Jan 1, 1970 and Jan 1, 1972 (no leaps years in this range) */
        baseYear = 1970;
    }

    /* Found out how many leap years there have been in this time and subtract them out */
    if (baseYear == 1972)
    {
        remainder  = time % SECONDS_IN_4YEARS;
        leap_years = (NW_Uint16)(time / SECONDS_IN_4YEARS);
        /* Compute the number of years since the last leap year */
        /* 4 Cases -------------------------*/
        /* Case 1: The year 2100 is not a leap year */
        if (leap_years >= 32)
        {
            /* Between 2100 and 2104 there is no leap year to consider */
            if (leap_years == 32)
            {
                year = (NW_Uint16)(remainder / SECONDS_IN_YEAR);
                remainder %= SECONDS_IN_YEAR;
            }
            else
            {
                /* Add back in the leap day taken out by above calculations */
                remainder += SECONDS_IN_DAY;
                if (remainder > (SECONDS_IN_YEAR + SECONDS_IN_DAY))
                {
                    /* Take out a day for the previous leap year */
                    remainder -= SECONDS_IN_DAY;
                    year = (NW_Uint16)(remainder / SECONDS_IN_YEAR);
                    remainder %= SECONDS_IN_YEAR;
                }
                else
                {
                    /* This is a leap year */
                    daysInMonth[1] = 29;
                    year = 0;
                }
            }
        }
        /* Case 1: Dec 31st of a leap year */
        else if ((remainder > SECONDS_IN_YEAR) && (remainder < (SECONDS_IN_YEAR + SECONDS_IN_DAY)))
        {

            year = 0;
            daysInMonth[1] = 29;
        }
        /* Case 2: In non-leap year */
        else if (remainder > (SECONDS_IN_YEAR + SECONDS_IN_DAY))
        {
            /* Take out a day for the previous leap year */
            remainder -= SECONDS_IN_DAY;
            year = (NW_Uint16)(remainder / SECONDS_IN_YEAR);
            remainder %= SECONDS_IN_YEAR;
        }
        /* Case 4: In leap year before Dec 31st */
        else
        {
            daysInMonth[1] = 29;
            year = 0;
        }
        /* Compute the number of years since the base year */
        year = NW_UINT16_CAST(leap_years * 4 + year);
    }
    else
    {
        remainder = time % SECONDS_IN_YEAR;
    }

    /* Compute the Julian Day of this time */
    julday     = (NW_Uint16)(remainder / SECONDS_IN_DAY);
    remainder %= SECONDS_IN_DAY;

    /* Compute the Month and day of this Julian Day (base zero) */
    month = 0;
    day = julday;
    while (day >= daysInMonth[month])
    {
        day = NW_UINT16_CAST(day - daysInMonth[month]);
        month++;
    }

    /* Compute the hour, minute, second */
    hour = (NW_Uint16)(remainder / SECONDS_IN_HOUR);
    remainder %= SECONDS_IN_HOUR;
    minute = (NW_Uint16)(remainder / SECONDS_IN_MINUTE);
    second = (NW_Uint16)(remainder % SECONDS_IN_MINUTE);

    /* Fill in the time structure */
    time_tm->Year      = NW_UINT16_CAST(baseYear + year);
    time_tm->Month     = NW_UINT16_CAST(month + 1);
    time_tm->Day       = NW_UINT16_CAST(day + 1);
    time_tm->DayOfWeek = NW_Time_DayOfWeek(in_time);
    time_tm->Hour      = hour;
    time_tm->Minute    = minute;
    time_tm->Second    = second;
    time_tm->Milliseconds = 0;

    return time_tm;
}


/*****************************************************************
**  Name:  NW_Time_CvtFromTimeTm
**  Description:  Convert from a date time struct to an integer time
**                
**  Parameters:   dateTime - a time structure
**  Return Value: sytem time in seconds since Jan 1, 1970
******************************************************************/
NW_Time_t NW_Time_CvtFromTimeTm(const NW_Time_Tm_t *time_tm)
{
    NW_Uint16 daysInMonth[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
    NW_Time_t time = 0;
    NW_Uint16 remainder;
    NW_Uint16 julday, month, leap_years;

    if (time_tm->Year < 1970)
        return 0;

    if (time_tm->Year >= 2104)
    {
        time += (1972 - 1970) * SECONDS_IN_YEAR;
        leap_years = 2100 - 1972;
        leap_years /= 4;
        time += leap_years * SECONDS_IN_4YEARS;
        time += (2104 - 2100) * SECONDS_IN_YEAR;
        time += ((time_tm->Year - 2104) / 4) * SECONDS_IN_4YEARS;
        remainder = NW_UINT16_CAST((time_tm->Year - 2104) % 4);
        time += remainder * SECONDS_IN_YEAR;
        /* If we are in a leap year */
        if (remainder == 0)
        {
            daysInMonth[1] = 29;
        }
        else
        {
            /* otherwise, add in seconds for the previous leap year */
            time += SECONDS_IN_DAY;
        }
    }
    else if (time_tm->Year >= 2100)
    {
        time += (1972 - 1970) * SECONDS_IN_YEAR;
        leap_years = 2100 - 1972;
        leap_years /= 4;
        time += leap_years * SECONDS_IN_4YEARS;
        remainder = NW_UINT16_CAST((time_tm->Year - 2100) % 4);
        time += remainder * SECONDS_IN_YEAR;
    }
    else if (time_tm->Year >= 1972)
    {
        time += (1972 - 1970) * SECONDS_IN_YEAR;
        time += ((time_tm->Year - 1972) / 4) * SECONDS_IN_4YEARS;
        remainder = NW_UINT16_CAST((time_tm->Year - 1972) % 4);
        time += remainder * SECONDS_IN_YEAR;
        /* If we are in a leap year */
        if (remainder == 0)
        {
            daysInMonth[1] = 29;
        }
        else
        {
            /* otherwise, add in seconds for the previous leap year */
            time += SECONDS_IN_DAY;
        }
    }
    else
    {
        time += (time_tm->Year - 1970) * SECONDS_IN_YEAR;
    }

    /* Compute the Julian day */
    julday = NW_UINT16_CAST(time_tm->Day - 1);
    month = 0;
    while (month < (time_tm->Month - 1))
    {
        julday = NW_UINT16_CAST(julday + daysInMonth[month]);
        month++;
    }

    /* Add in the days in year */
    time += julday * SECONDS_IN_DAY;

    /* Add in the time */
    time += time_tm->Hour * SECONDS_IN_HOUR;
    time += time_tm->Minute * SECONDS_IN_MINUTE;
    time += time_tm->Second;

    return time;
}

/*lint -restore*/
