/*
* Copyright (c) 2000 - 2001 Nokia Corporation and/or its subsidiary(-ies).
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


#include "nw_lmgr_listbullets.h"
#include "nwx_string.h"
#include "nw_text_ucs2.h"

/* The current implementation of the number-to-string utility is limited to a small set of
 * static strings so that no allocations need to take place. However, this does limit the
 * magnitude of numbers that can be handled. TODO: revisit this limitation
 */

#define NW_LMgr_maxListCount (NW_Uint32)999
#define NW_LMgr_maxBulletLen (NW_Uint8)10

/* Roman numeral support */
const char* const units[] = {
  "i","ii","iii","iv","v","vi","vii","viii","ix" 
};
const char* const tens[] = {
  "x","xx","xxx","xl","l","lx","lxx","lxxx","xc"
};
const char* const hundreds[] = {
  "c","cc","ccc","cd","d","dc","dcc","dccc","cm"
};

static
void
getRomanBullet(NW_Uint32 number, NW_Ucs2* str, NW_Bool toUpper){
  
  number = number % 1000;
  if (number / 100 != 0) { 
    NW_Str_StrcatConst(str, hundreds[number / 100 - 1]);
    number = number % 100;
  }
  if (number / 10 != 0) { 
    NW_Str_StrcatConst(str, tens[number / 10 - 1]);
    number = number % 10;
  }
  if (number != 0) { 
    NW_Str_StrcatConst(str, units[number - 1]);
  }

  if (toUpper) {
    NW_Str_Strupr(str);
  }
}

void NW_LMgr_ListBullets_AStringFromUInt(NW_Uint32 number, NW_Uint32* i, NW_Ucs2* Str, NW_Ucs2 startChar, NW_Uint32 offset)
{
  NW_Uint32 j, new_offset = 1;
  j = *i;
  if (number > 26*27)
  {
    new_offset = 0;
  }
  Str[j] = (NW_Ucs2)(startChar + (number - 1)%26);
  if ((number - offset) / 26)
  {
    *i = *i + 1;
    NW_LMgr_ListBullets_AStringFromUInt((number - offset)/26, i, Str, startChar, new_offset);
  }
}

void NW_LMgr_ListBullets_ReverseStr(NW_Ucs2* Str, NW_Ucs2* temp2, NW_Uint32 i)
{
  NW_Uint32 j;

  if (i <2) 
  {
    NW_Mem_Free (temp2);
    return;
  }
  for (j = 0; j < i; j++)
  {
    Str[i-j-1] = temp2[j];
  }
  NW_Mem_Free (temp2);
}
  
NW_Ucs2*
NW_LMgr_ListBullets_StringFromUInt(NW_Ucs2 format, NW_Uint32 number){
  NW_Ucs2 tempBullet[NW_LMgr_maxBulletLen + 1];
  NW_Uint32 i = 0;

  number = (number % NW_LMgr_maxListCount) + 1;
  NW_Mem_memset(tempBullet, 0, (NW_LMgr_maxBulletLen + 1)*sizeof(NW_Ucs2));
  
  switch (format){
  case 'd':
    (void)NW_Str_Itoa(number, tempBullet);
    break;
  case 'A':
    /* a recursive way to calculate the letter bullet */
    NW_LMgr_ListBullets_AStringFromUInt(number, &i, tempBullet, 'A', 1);
    NW_LMgr_ListBullets_ReverseStr(tempBullet, NW_Str_Newcpy(tempBullet), ++i);
    tempBullet[i] = 0;
    break;
  case 'a':
    NW_LMgr_ListBullets_AStringFromUInt(number, &i, tempBullet, 'a', NW_TRUE);
    NW_LMgr_ListBullets_ReverseStr(tempBullet, NW_Str_Newcpy(tempBullet), ++i);
    tempBullet[i] = 0;
    break;
  case 'R':
    getRomanBullet(number, tempBullet, NW_TRUE);
    break;
  case 'r':
    getRomanBullet(number, tempBullet, NW_FALSE);
    break;
  default:
    NW_ASSERT(0);
    return NULL;
  }

  /* Add the point */
  NW_Str_StrcatConst(tempBullet, ".");

  /* Return the copy of the string */
  return NW_Str_Newcpy(tempBullet);
}

