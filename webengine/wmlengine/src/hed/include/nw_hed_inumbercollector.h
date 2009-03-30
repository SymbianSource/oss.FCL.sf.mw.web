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


#ifndef _NW_HED_INumberCollector_h_
#define _NW_HED_INumberCollector_h_

#include "nw_object_interface.h"
#include "nw_adt_dynamicvector.h"
#include "NW_HED_EXPORT.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** ----------------------------------------------------------------------- **
    @class:       NW_HED_INumberCollector

    @scope:       public

    @description: An interface typically implemented by content handlers.
                  Used by the NW_HED_DocumentRoot to retrieve the list of
                  phone numbers possibly embedded in the document tree.

                  The NW_HED_DocumentRoot will traverse the document tree
                  querying each content handler in the tree for an
                  implementation of the this interface. If found, the document
                  root will invoke this method to retrieve the phone numbers
                  associated with that content handler.

                  How a particular content handler decides to implement this
                  interface is not specified.
 ** ----------------------------------------------------------------------- **/

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_HED_INumberCollector_Class_s NW_HED_INumberCollector_Class_t;
typedef struct NW_HED_INumberCollector_s NW_HED_INumberCollector_t;

typedef struct NW_HED_INumberCollector_ConfigEntry_s NW_HED_INumberCollector_ConfigEntry_t;

/* ------------------------------------------------------------------------- *
   virtual method type definitions
 * ------------------------------------------------------------------------- */
typedef
TBrowserStatusCode
(*NW_HED_INumberCollector_Collect_t) (NW_HED_INumberCollector_t* numberCollector,
                                      NW_ADT_DynamicVector_t* dynamicVector);

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_HED_INumberCollector_ClassPart_s {
  NW_HED_INumberCollector_Collect_t collect;
} NW_HED_INumberCollector_ClassPart_t;

struct NW_HED_INumberCollector_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Interface_ClassPart_t NW_Object_Interface;
  NW_HED_INumberCollector_ClassPart_t NW_HED_INumberCollector;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_HED_INumberCollector_s {
  NW_Object_Interface_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_HED_INumberCollector_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_HED_INumberCollector))

#define NW_HED_INumberCollectorOf(_object) \
  (NW_Object_Cast (_object, NW_HED_INumberCollector))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT const NW_HED_INumberCollector_Class_t NW_HED_INumberCollector_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */

/** ----------------------------------------------------------------------- **
    @method:      NW_HED_INumberCollector_Collect

    @synopsis:    Returns all phone numbers associated with the implementer.

    @scope:       public
    @access:      virtual

    @parameters:
       [in, out] NW_ADT_DynamicVector_t* dynamicVector
                  The dynamic vector into which all the phone numbers will be
                  placed. This is a vector of NW_Text_t*.
      
    @description: Retrieves, by any mechanism necessary, all the phone numbers
                  associated with the interface implementer. The phone numbers
                  are added to the 'dynamicVector' as NW_Text_t* pointers.
                  These NW_Text_t will be freed by the caller when no longer
                  needed.

                  The order in which the numbers are added to the vector is
                  undefined only that they are appended to the vector is
                  mandated. In addition it is not legal for the implementer to
                  remove or otherwise alter any entries that may already be
                  present in the vector at entry.
                  
                  Note that this last point implies that it is not necessary,
                  for the implementer to attempt any intelligent insertion to
                  avoid duplicate items. If the implementer wishes to do so, it
                  must do so by not inserting a new item instead of deleting an
                  existing item so as not to violate the sanctity of existing
                  entries of the 'dynamicVector'.
                  
                  
    @returns:     TBrowserStatusCode
                  On error, it is not the responsibility of the implementer
                  to clean up the out parameters. The only guarantee that the
                  implementer must fullfill is that the parameters are left
                  in a consistent state. This simply means that the entries of
                  the 'dynamicVector' must still be valid NW_Text_t*'s.

       [KBrsrSuccess]
                  Successful completion; 'dynamicVector' will have had all
                  phone numbers appendeded.

       [KBrsrOutOfMemory]
                  Not enough memory available to complete operation.

       [KBrsrUnexpectedError]
                  An unexpected error occurred.
 ** ----------------------------------------------------------------------- **/
#define NW_HED_INumberCollector_Collect(_numberCollector, _dynamicVector) \
  (NW_Object_Invoke (_numberCollector, NW_HED_INumberCollector, collect) ( \
     NW_HED_INumberCollectorOf (_numberCollector), \
     (_dynamicVector)))

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_HED_INumberCollector_h_ */
