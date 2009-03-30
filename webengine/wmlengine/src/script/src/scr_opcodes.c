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
   $Workfile: scr_opcodes.c $

    Purpose:

      Inline parameters have been used to optimally pack information into as
      few bytes as possible.  Therefore the opcodes types are:
      1XXPPPPP  ==> type0
      010XPPPP  ==> type1
      011XXPPP  ==> type2
      00XXXXXX  ==> type3
      This file parses the opcode and return its type and any parameters/instrunctions
      that has been packed with it.
 */
        
#include "scr_defs.h"

void parse_opcode(NW_Byte *opcode, NW_Byte *param)
{
  /* 1XXPPPP */
  if ((*opcode & 0x80) == 0x80) {
    *param  = (NW_Byte)(*opcode & 0x1F);
    *opcode = (NW_Byte)(*opcode & 0xE0);
  }

  /* 010XPPPP */
  else if ((*opcode & 0xE0) == 0x40) {
    *param  = (NW_Byte)(*opcode & 0x0F);
    *opcode = (NW_Byte)(*opcode & 0xF0);
  }

  /* 011XXPPP */
  else if ((*opcode & 0xE0) == 0x60 ) {
    *param  = (NW_Byte)(*opcode & 0x07);
    *opcode = (NW_Byte)(*opcode & 0x78);
  }
}
