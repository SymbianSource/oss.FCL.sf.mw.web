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


#ifndef NW_ADT_TYPE_H
#define NW_ADT_TYPE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
 * ------------------------------------------------------------------------- */
typedef NW_Uint32 NW_ADT_Token_t;

typedef NW_Uint8 NW_ADT_ValueType_t;
enum {
  NW_ADT_ValueType_Integer    = 0x01,
  NW_ADT_ValueType_Decimal    = 0x02,
  NW_ADT_ValueType_Pointer    = 0x03,
  NW_ADT_ValueType_Object     = 0x80
};

typedef union NW_ADT_Value_u {
  NW_Int32 integer;
  NW_Float32 decimal;
  void* pointer;
  NW_Object_t* object;
} NW_ADT_Value_t;

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_ADT_TYPE_H */
