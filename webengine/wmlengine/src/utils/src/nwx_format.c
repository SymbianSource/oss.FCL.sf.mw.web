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
**-----------------------------------------------------------------------------
** REFERENCES:
** WAP-188-WAPGenFormats, Version 15-Aug-2000
** Design Specification - Spectrum Coding Standards
**-----------------------------------------------------------------------------
*/

/*
**-----------------------------------------------------------------------------
** Component Generic Includes
**-----------------------------------------------------------------------------
*/


/*
**-----------------------------------------------------------------------------
** Module Specific Includes
**-----------------------------------------------------------------------------
*/
#include "nwx_string.h"

/*
**-----------------------------------------------------------------------------
** MACROS
**-----------------------------------------------------------------------------
*/
#define   GLOBALEXTENSION  0x2b /* + */
//#define   PAUSECHAR        0x2c /* , */
//#define   UPPER_D          0x44
#define   ASTERISK         0x2a
#define   HASHMARK         0x23

/*
**-----------------------------------------------------------------------------
** External Public (Exported) Functions
**-----------------------------------------------------------------------------
*/
/******************************************************************************
  Name: 
    NW_Format_IsValidPhoneNumber( const NW_Ucs2 *phoneNumber )
  
  Description:
    Validate "input" as complying with WAPGenFormat-188 item, "phone_number".    
    Assume that phone_number consists of: [+] DIGIT+
    DIGIT ::= "0" | "1" | "2" | "3" | "4" |
              "5" | "6" | "7" | "8" | "9"
  Parameters:
    NW_Ucs2 *phoneNumber

  Return Value:
    NW_TRUE - "phone number" is valid
    NW_FALSE- not valid
******************************************************************************/
NW_Bool NW_Format_IsValidPhoneNumber( const NW_Ucs2 *phoneNumber ) {
  NW_Bool digitFound = NW_FALSE;

  if (*phoneNumber == GLOBALEXTENSION) {
    ++phoneNumber;
  }
  while (*phoneNumber) {
    switch (*phoneNumber) {
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
      digitFound = NW_TRUE;
      break;
    default:
      return NW_FALSE;
    }
    ++phoneNumber;
  }
  return digitFound;
}


/******************************************************************************
  Name: 
    NW_Format_IsValidToneSequence( const NW_Ucs2 *toneSequence )
  
  Description:
    Validate "input" as complying with WAPGenFormat-188 item, "tone_sequence".  
    tone_sequence ::= (pause | dtmf_sequence)*
    pause ::= ","
    dtmf_sequence ::= dtmf_digit+
    dtmf_digit ::= DIGIT | "*" | "#" | "A" | "B" | "C" | "D"
    DIGIT ::= "0" | "1" | "2" | "3" | "4" |
              "5" | "6" | "7" | "8" | "9"
    
  Parameters:
    NW_Ucs2 *toneSequence

  Return Value:
    NW_TRUE - "toneString" is a valid tone sequence
    NW_FALSE- not valid
******************************************************************************/
NW_Bool NW_Format_IsValidToneSequence( const NW_Ucs2 *toneSequence ) {
  NW_Bool digitFound = NW_TRUE; /* Changed from FALSE to TRUE inorder to allow empty DTMF sequence. moved from Wave */

  while (*toneSequence) {
    switch (*toneSequence) {
    case ASTERISK:
    case HASHMARK:
    case ',':
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
    case 'A': case 'B': case 'C': case 'D':
      digitFound = NW_TRUE;
      break;
    default:
      return NW_FALSE;
    }
    ++toneSequence;
  }
  return digitFound;
}
