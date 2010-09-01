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

#ifndef NW_NVPAIR_H
#define NW_NVPAIR_H

/*
** Includes
*/
#include "nwx_defs.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Identifier validation according WMLScript 6.1.6 */
NW_Bool NW_CheckName(const NW_Ucs2* name);

/* 
  Description: 
    The NW_NVPair_t abstract data type is for storing name value string
    pairs.  It is implemented to be created and accessed 
    sequencially, and to use as little memory as possible.  A typical
    calling sequence is:


    Typical calling sequence to create and populate an object:
      postFields = NW_NVPair_New();
      while () {
        status = NW_NVPair_CalSize(postFields, name, value);
      }
      status = NW_NVPair_AllocStorage( postFields );

      while () {
        status = NW_NVPair_Add(postFields, name, value);
      } 

      (void) NW_NVPair_ResetIter( postFields );

    Typical calling sequence to interate through a created object:
      (void) NW_NVPair_ResetIter( postfields );

      status = NW_NVPair_GetNext( postfields, &name, &value );
      while ( status == KBrsrSuccess ) {
        ** code that uses name-value pairs **
        status = NW_NVPair_GetNext( postfields, &name, &value );
      }
      if ( status != KBrsrNotFound ) {
        return status;
      }
*/

/*
** Type Declarations
*/
/* Holds name-value pairs as ucs2 strings. */ 
typedef struct {
  NW_Ucs2*  iterPtr;      /* Used to keep place with iterating over pairs. */
  NW_Ucs2*  strTable;     /* Array of null-terminated ucs2 strings. */
  NW_Uint32 size;         /* Total # of ucs2 chars of string space. */
} NW_NVPair_t;

/* a function type for passing a name value string pair */
typedef TBrowserStatusCode (NW_NVPair_NVFunction_t)(NW_NVPair_t* thisObj,
                                             const NW_Ucs2* name,
                                             const NW_Ucs2* value);
/*
** Preprocessor Macro Definitions
*/


/*
** Global Function Declarations
*/

/*****************************************************************

  Name: NW_NVPair_New

  Description:  Allocates a NW_Pair_t object.

  Parameters:  none

  Return Value: pointer to newly create object.  If fails, NULL.

******************************************************************/
NW_NVPair_t*  NW_NVPair_New (void);

/*****************************************************************

  Name: NW_NVPair_Delete

  Description:  
    Dellocates a NW_Pair_t object.  Allows thisObj == NULL.


  Parameters:  
    thisObj - in: Object to deallocate.

  Return Value: none

******************************************************************/
void NW_NVPair_Delete (NW_NVPair_t* thisObj);

/*****************************************************************

  Name: NW_NVPair_IsEmpty

  Description:  
    Returns NW_TRUE if the object is NULL or it does not contain
    any value-pairs.

  Parameters:  
    thisObj - in: Object to deallocate.

  Return Value:
     NW_TRUE - if object contains 1 or more name-value pairs.
     NW_FALSE - if object is null or it is empty (no name-value pairs). 

******************************************************************/
NW_Bool NW_NVPair_IsEmpty (NW_NVPair_t* thisObj);


/*****************************************************************

  Name: NW_NVPair_CalSize

  Description: 
    Records the storage requirements for the name-value
    pair. Counts string plus null-terminater.  Used when AddFirst is
    called to allocate all the necessary storage at once.

  Parameters:  
    thisObj - in/out: NW_NV_Pair_t object.
    name - in: name string
    value - in: value string

  Requires:
    (name != NULL) && (name[0] != '\0')
    thisObj != NULL

  Return Value:
     KBrsrSuccess

******************************************************************/
NW_NVPair_NVFunction_t NW_NVPair_CalSize;


/*****************************************************************

  Name: NW_NVPair_AllocStorage

  Description:  
    This routine allocates the storage to all the name-value pairs
    this object will hold.

  Parameters:  
    thisObj - in/out: NW_NV_Pair_t object.

  Requires:
    thisObj != NULL
    thisObj->size >= 3   (minimum to hold 1 name-value pair)

  Return Value:
     KBrsrSuccess
     KBrsrOutOfMemory - unable to allocate storage for name-value
                             pairs.

******************************************************************/
TBrowserStatusCode NW_NVPair_AllocStorage (NW_NVPair_t* thisObj);

/*****************************************************************

  Name: NW_NVPair_Add

  Description:
    Adds a name-value pair to the object.  Must be called after
    NW_NVPair_AllocStorage is called to allocate memory.

  Parameters:  
    thisObj - in/out: NW_NV_Pair_t object.
    name - in: name string.
    value - in: value string.

  Requires:
    thisObj != NULL
    (name != NULL) && (name[0] != '\0')

  Return Value:
     KBrsrSuccess
     KBrsrBufferTooSmall - name-value will not fit in allocated 
                                storage.

******************************************************************/
NW_NVPair_NVFunction_t NW_NVPair_Add;


/*****************************************************************

  Name: NW_NVPair_ResetIter

  Description:  
    Sets the iterator for NW_NVPair_GetNext to start at the first
    name-value pair.  Allows thisObj == NULL;

  Parameters:  
    thisObj - in/out: NW_NV_Pair_t object.

  Return Value:
     KBrsrSuccess

******************************************************************/
TBrowserStatusCode NW_NVPair_ResetIter (NW_NVPair_t* thisObj);

/*****************************************************************

  Name: NW_NVPair_GetNext

  Description:  
    Returns subsequent name-value pair in the order they have been
    inserted. Returns KBrsrNotFound when no more name-value
    pairs to return.    Allows thisObj == NULL;

  Parameters:  
    thisObj - in/out: NW_NV_Pair_t object.
    name - out: name string.
    value - out: value string.  Returns NULL pointer for empty string ("").

  Return Value:
     KBrsrSuccess
     KBrsrNotFound - returned if there is not a name-value
                         pair to return.

******************************************************************/
TBrowserStatusCode NW_NVPair_GetNext (NW_NVPair_t* thisObj, NW_Ucs2** name,
                               NW_Ucs2** value);


/*
** Global Variable Declarations
*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NW_NVPAIR_H */


