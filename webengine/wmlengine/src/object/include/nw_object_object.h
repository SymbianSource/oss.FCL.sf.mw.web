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


#ifndef NW_OBJECT_OBJECT_H
#define NW_OBJECT_OBJECT_H

#include "NW_Object_EXPORT.h"
#include "nwx_defs.h"
#include "nwx_assert.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   type declarations
 * ------------------------------------------------------------------------- */
typedef void NW_Object_Class_t;
typedef void NW_Object_t;

/* ------------------------------------------------------------------------- *
   public constants
 * ------------------------------------------------------------------------- */
#define NW_Object_Unused ((void**)0)

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#ifdef _DEBUG
#define NW_Object_Cast(_object, _type) \
  (NW_ASSERT((_object) == NULL || \
             NW_Object_IsInstanceOf (_object, &_type##_Class)), \
   ((_type##_t*) (_object)))
#else /* DEBUG */
#define NW_Object_Cast(_object, _type) \
  ((_type##_t*) (_object))
#endif /* DEBUG */

#define NW_Object_GetClassPart(_object, _type) \
  ((( _type##_Class_t*) NW_Object_Core_GetClass (_object))->_type)

#define NW_Object_Invalidate(_object) \
  (*(NW_Object_Class_t**) (_object) = NULL, (void) 0)

#define NW_Object_Invoke(_object, _type, _method) \
  (NW_Object_GetClassPart(_object, _type)._method)

#if defined(LIMIT_TI_COMPILER_WARNINGS)
#define NW_OBJECT_CONSTCAST(_type) (_type)
#else
#define NW_OBJECT_CONSTCAST(_type) (_type)
#endif

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_Object_GetClass(_object) \
  ((const NW_Object_Class_t*) NW_Object_Core_GetClass (_object))

#define NW_Object_GetSuperclass(_object) \
  (NW_Object_Core_GetClassPart (_object).super)

#define NW_Object_IsClass(_object, _class) \
  (NW_Object_GetClass (_object) == \
   ((const NW_Object_Class_t*) (_class)))

NW_OBJECT_EXPORT
NW_Bool
NW_Object_IsDerivedFrom (const NW_Object_Class_t* objClass,
		         const NW_Object_Class_t* superClass);

NW_OBJECT_EXPORT
NW_Bool
NW_Object_IsInstanceOf (const NW_Object_t* object,
		        const NW_Object_Class_t* superClass);

NW_OBJECT_EXPORT
NW_Object_t*
NW_Object_QuerySecondary (const NW_Object_t* object,
                          const NW_Object_Class_t* secondaryClass);

#define NW_Object_QueryInterface(_object, _class) \
  (NW_Object_QuerySecondary((_object), (_class)))

#define NW_Object_QueryAggregate(_object, _class) \
  (NW_Object_QuerySecondary((_object), (_class)))

/** ----------------------------------------------------------------------- **
    @method:      NW_Object_GetOuter

    @synopsis:    Returns the enclosing object of the secondary.

    @scope:       public
    @access:      final

    @parameters:
      const NW_Object_t* secondary
                  The secondary instance for which to retrieve the
                  outer object.

    @description: All secondary objects have an outer primary that
                  encapsulates the secondary object. This method will
                  return an reference to that outer object that aggregates
                  the specified secondary.
                  
    @returns:     NW_Object_t*
                  If successful, the method will return the outer object of
                  the specified secondary.

       [NULL]
                  Unexpected error.
 ** ----------------------------------------------------------------------- **/
NW_OBJECT_EXPORT
NW_Object_t*
NW_Object_GetOuter (const NW_Object_t* object);

/** ----------------------------------------------------------------------- **
    @method:      NW_Object_GetPrimaryOuter

    @synopsis:    Returns the primary outer object of the secondary.

    @scope:       public
    @access:      final

    @parameters:
      const NW_Object_t* instance
                  The secondary instance for which to retrieve the
                  primary outer object.

    @description: All secondary objects have a primary outer object that is
                  the root of the secondary object hierarchy. This method will
                  return an reference to that primary outer object of the
                  specified secondary object.
                  
    @returns:     NW_Object_t*
                  If successful, the method will return the primary outer
                  object of the specified secondary.

       [NULL]
                  Unexpected error.
 ** ----------------------------------------------------------------------- **/
NW_OBJECT_EXPORT
NW_Object_t*
NW_Object_GetPrimaryOuter (const NW_Object_t* object);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_OBJECT_OBJECT_H */
