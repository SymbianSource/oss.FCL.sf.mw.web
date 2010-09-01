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


#ifndef NWX_FORMAT_H
#define NWX_FORMAT_H

#ifdef __cplusplus
extern "C" {
#endif

/*
** Global Function Declarations
*/

/******************************************************************************
  Name: 
    NW_Format_IsValidPhoneNumber( const NW_Ucs2 *phoneNumber )
  
  Description:
    Validate "input" as complying with WAPGenFormat-188 item, "phone_number".    

  Parameters:
    NW_Ucs2 *phoneNumber

  Return Value:
    NW_TRUE - "input" is a valid phone_number
    NW_FALSE- not valid
******************************************************************************/
NW_Bool NW_Format_IsValidPhoneNumber( const NW_Ucs2 *phoneNumber );


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
    NW_TRUE - "input" is a valid tone sequence
    NW_FALSE- not valid
******************************************************************************/
NW_Bool NW_Format_IsValidToneSequence( const NW_Ucs2 *toneSequence );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif    /* NWX_FORMAT_H */
