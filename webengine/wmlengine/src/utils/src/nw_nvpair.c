/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
**-------------------------------------------------------------------------
**  Component Generic Include
**-------------------------------------------------------------------------
*/
#include "nwx_mem.h"
#include "nwx_string.h"
#include "nwx_url_utils.h"

/*
**-------------------------------------------------------------------------
**  Module Specific Includes
**-------------------------------------------------------------------------
*/
#include "nw_nvpair.h"
#include "BrsrStatusCodes.h"

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

/* WMLScript Spec 6.1.6 Identifiers */
NW_Bool NW_CheckName(const NW_Ucs2* name)
{
  const NW_Ucs2 *pChar = name;

  NW_ASSERT(name != NULL);
  if (!NW_Str_Isalpha(*name) && (*name != WAE_URL_UNDER_CHAR))
  {
    return NW_FALSE;
  }

  while (*pChar != 0) {
    if (!NW_Str_Isalnum(*pChar) && (*pChar != WAE_URL_UNDER_CHAR))
    {
      return NW_FALSE;
    } else {
      pChar++;
    }
  }

  return NW_TRUE;
}

NW_NVPair_t*  NW_NVPair_New (void)
{
  NW_NVPair_t* thisObj;

  thisObj = (NW_NVPair_t*) NW_Mem_Malloc( sizeof(NW_NVPair_t) );
  if ( thisObj != NULL) {
    NW_Mem_memset( thisObj, 0, sizeof(NW_NVPair_t) );
  }

  return thisObj;
}

void NW_NVPair_Delete (NW_NVPair_t* thisObj)
{
  if ( thisObj == NULL ) {
    return;
  }

  if ( thisObj->strTable != NULL ) {
    NW_Mem_Free( thisObj->strTable );
    thisObj->strTable = NULL;
  }

  NW_Mem_Free( thisObj );
}

NW_Bool NW_NVPair_IsEmpty (NW_NVPair_t* thisObj)
{
  return NW_BOOL_CAST(( thisObj == NULL ) || ( thisObj->size == 0 ));
}


TBrowserStatusCode NW_NVPair_CalSize (NW_NVPair_t* thisObj, 
                               const NW_Ucs2* name,
                               const NW_Ucs2* value)
{
  NW_Uint32 len;

  NW_ASSERT( thisObj != NULL );
  NW_ASSERT( thisObj->iterPtr == NULL );
  NW_ASSERT( thisObj->strTable == NULL );
  NW_ASSERT( name != NULL ); 
  
  if (name[0] == '\0') {

      return KBrsrSuccess;
  }

  thisObj->size = thisObj->size + NW_Str_Strlen( name ) + 1;

  /* Treat NULL pointer as an empty string (""). */
  len = ( value == NULL ) ? 1 : (NW_Str_Strlen(value) + 1);
  thisObj->size = thisObj->size + len;

  return KBrsrSuccess;
}

TBrowserStatusCode NW_NVPair_AllocStorage (NW_NVPair_t* thisObj)
{
  NW_Ucs2* storage;
  NW_Uint32 byteCnt;

  NW_ASSERT( thisObj != NULL );
  NW_ASSERT( thisObj->iterPtr == NULL );
  NW_ASSERT( thisObj->strTable == NULL );
  /* Require an empty structure or at least 1 name-value pair. */
  NW_ASSERT( (thisObj->size == 0) || ( thisObj->size >= 3));

  /* Allow an empty structure. */
  if ( thisObj->size == 0 ) {
    return KBrsrSuccess;
  }

  byteCnt = thisObj->size * sizeof (NW_Ucs2);
  storage = (NW_Ucs2*) NW_Mem_Malloc( byteCnt );
  if (storage == NULL) {
    return KBrsrOutOfMemory;
  }
  
  /* Zero out structure. */
  NW_Mem_memset( storage, 0, byteCnt );

  thisObj->iterPtr = storage;
  thisObj->strTable = storage;

  return KBrsrSuccess;
}

TBrowserStatusCode NW_NVPair_Add (NW_NVPair_t* thisObj, 
                           const NW_Ucs2* name,
                           const NW_Ucs2* value)
{
  NW_Uint32 len;
  NW_Int32  strTableLen;

  NW_ASSERT( thisObj != NULL );
  NW_ASSERT( thisObj->iterPtr != NULL );
  NW_ASSERT( thisObj->strTable != NULL );
  NW_ASSERT( name != NULL ); 

  /* WML Spec says that the name is required 
  If name is NULL, do not add this pair*/
  if (name[0] == '\0') {
      return KBrsrSuccess;
  }

  len = NW_Str_Strlen( name ) + 1;
  strTableLen = thisObj->iterPtr - thisObj->strTable;
  NW_ASSERT( strTableLen >= 0 );
  if ( (NW_UINT32_CAST(strTableLen) + len) > thisObj->size ) {
    return KBrsrBufferTooSmall;
  }
  NW_Str_Strcpy( thisObj->iterPtr, name );
  thisObj->iterPtr += len;

  /* Treat null pointer for value as empty string (""). */
  len = (value == NULL) ? 1 : NW_Str_Strlen( value ) + 1;
  strTableLen = thisObj->iterPtr - thisObj->strTable;
  NW_ASSERT( strTableLen >= 0 );
  if ( (NW_UINT32_CAST(strTableLen) + len) > thisObj->size ) {
    return KBrsrBufferTooSmall;
  }
  
  if ( value == NULL) {
    /* Store NULL pointer as empty string (""). */
    thisObj->iterPtr[0] = 0;
  } else {
    NW_Str_Strcpy( thisObj->iterPtr, value );
  }
  thisObj->iterPtr += len;

  return KBrsrSuccess;
}

TBrowserStatusCode NW_NVPair_ResetIter (NW_NVPair_t* thisObj)
{
  if ( thisObj == NULL ) {
    /* Treat NULL pointer like an empty object. */
    return KBrsrSuccess;
  }
  thisObj->iterPtr = thisObj->strTable;
  
  return KBrsrSuccess;
}

TBrowserStatusCode NW_NVPair_GetNext (NW_NVPair_t* thisObj, NW_Ucs2** name,
                               NW_Ucs2** value)
{
  NW_Int32 strTableLen;

  NW_ASSERT( name != NULL );
  NW_ASSERT( value != NULL );

  *name = NULL;
  *value = NULL;

  if ( thisObj == NULL ) {
    /* Treat NULL pointer like an empty object. */
    return KBrsrNotFound;
  }

  NW_ASSERT( thisObj->iterPtr != NULL );
  NW_ASSERT( thisObj->strTable != NULL );


  strTableLen = thisObj->iterPtr - thisObj->strTable;
  NW_ASSERT( strTableLen >= 0 );
  if ( NW_UINT32_CAST(strTableLen) >= thisObj->size ) {
    return KBrsrNotFound;
  }

  /* Require that name is not an empty string (""). */
  NW_ASSERT( thisObj->iterPtr[0] != 0 ); 

  *name = thisObj->iterPtr;
  thisObj->iterPtr += (NW_Str_Strlen( thisObj->iterPtr ) + 1);

  /* Must be value if found name because always insert in pairs. */
  strTableLen = thisObj->iterPtr - thisObj->strTable;
  NW_ASSERT( ( strTableLen >= 0 ) && 
             ( NW_UINT32_CAST(strTableLen) < thisObj->size ) ); 
  
  *value = thisObj->iterPtr;
  thisObj->iterPtr += (NW_Str_Strlen( thisObj->iterPtr ) + 1);

  return KBrsrSuccess;
}

