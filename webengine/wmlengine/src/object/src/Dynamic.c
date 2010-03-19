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


#include "nw_object_dynamici.h"
#include "nw_object_aggregatei.h"
#include <nwx_mem.h>
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- */
NW_Bool
NW_Object_Base_IsEncapsulated (const NW_Object_Base_Class_t* objClass,
                               const NW_Object_Secondary_Class_t* secondaryClass)
{
  /* iterate through the class hierarchy */
  while (((NW_Object_Class_t*) objClass) != &NW_Object_Base_Class) {
    const void* const* secondaryList;

    /* get the interface list of the class */
    secondaryList = objClass->NW_Object_Base.secondaryList;
    if (secondaryList != NULL) {
      NW_Uint32 index;

      for (index = 0; secondaryList[index] != NULL; index++) {
        if (secondaryList[index] == secondaryClass) {
          return NW_TRUE;
        }
      }
    }

    /* the interface implementation was not found at this level of the class
       hierarchy, let's try the superclass */
    objClass = (const NW_Object_Base_Class_t*) objClass->NW_Object_Core.super;
  }

  return NW_FALSE;
}

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_Object_Dynamic_Class_t NW_Object_Dynamic_Class = {
  { /* NW_Object_Core    */
    /* super             */ &NW_Object_Base_Class,
    /* querySecondary    */ _NW_Object_Core_QuerySecondary
  },
  { /* NW_Object_Base    */
    /* interfaceList     */ NULL
  },
  { /* NW_Object_Dynamic */
    /* instanceSize      */ sizeof (NW_Object_Dynamic_t),
    /* construct         */ NULL,
    /* destruct          */ NULL
  }
};

/* ------------------------------------------------------------------------- *
   public/protected methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Object_Dynamic_Initialize (NW_Object_Core_t* core,
                               va_list* argList)
{
  NW_TRY (status) {
    const NW_Object_Dynamic_Class_t* objClass;

    /* zero the object */
    objClass = (const NW_Object_Dynamic_Class_t*) core->objClass;
    (void) NW_Mem_memset (core, 0, objClass->NW_Object_Dynamic.instanceSize);
    core->objClass = objClass;

    /* call our superclass method */
    status = _NW_Object_Core_Initialize (core, argList);
    if (status != KBrsrSuccess) {
      NW_THROW (status);
    }

    /* call the class constructor on the newly allocated object */
    if (NW_Object_Dynamic_GetClassPart (core).construct != NULL) {
      status = NW_Object_Dynamic_Construct (core, argList);
      NW_THROW_ON_ERROR (status);
    }
  } NW_CATCH (status) {
  } NW_FINALLY {
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
void
_NW_Object_Dynamic_Terminate (NW_Object_Core_t* core)
{
  const NW_Object_Dynamic_Class_t* objClass;

  for (objClass = (const NW_Object_Dynamic_Class_t*) NW_Object_GetClass (core);
       objClass != &NW_Object_Dynamic_Class;
       objClass = (const NW_Object_Dynamic_Class_t*) objClass->NW_Object_Core.super) {
    /* invoke the Dynamic destruct method */
    if (objClass->NW_Object_Dynamic.destruct != NULL) {
      objClass->NW_Object_Dynamic.destruct ((NW_Object_Dynamic_t*) core);
    }

    /* does the class have secondaries? */
    if (objClass->NW_Object_Base.secondaryList != NULL) {
      size_t superOffset;
      NW_Uint32 index;

      superOffset =
        ((NW_Object_Dynamic_Class_t*) objClass->NW_Object_Core.super)->
          NW_Object_Dynamic.instanceSize;

      /* iterate through the secondary list in order to terminate immediate
         aggregates */
      for (index = 0;
           objClass->NW_Object_Base.secondaryList[index] != NULL;
           index++) {
        const NW_Object_Secondary_Class_t* secondaryClass;
        size_t secondaryOffset;
        NW_Object_Secondary_t* secondary;

        secondaryClass = (NW_Object_Secondary_Class_t*)
          objClass->NW_Object_Base.secondaryList[index];
        secondaryOffset = secondaryClass->NW_Object_Secondary.offset;
        secondary =
          (NW_Object_Secondary_t*) ((NW_Byte*) core + secondaryOffset);

        /* ignore tagged secondaries */
        if (*(void**) secondary == NULL) {
          continue;
        }

        /* ignore secondaries that are not aggregates, i.e. interfaces */
        if (!NW_Object_IsDerivedFrom (secondaryClass,
                                      &NW_Object_Aggregate_Class)) {
          /* tag the secondary such that we don't process it again */
          *(void**) secondary = NULL;
          continue;
        }

        /* traverse up the aggregate class hierarchy destroying all that are
           not referenced by our superclass secondary list */
        while (secondaryClass != (void*) &NW_Object_Aggregate_Class) {
          const NW_Object_Aggregate_Class_t* aggregateClass;

          if (secondaryOffset < superOffset && 
              NW_Object_Base_IsEncapsulated ((const NW_Object_Base_Class_t*) 
                    objClass->NW_Object_Core.super, secondaryClass)) {
            break;
          }

          aggregateClass = (NW_Object_Aggregate_Class_t*) secondaryClass;
          if (aggregateClass->NW_Object_Aggregate.destruct != NULL) {
            aggregateClass->NW_Object_Aggregate.
              destruct ((NW_Object_Aggregate_t*) secondary);
          }
          secondaryClass = (const NW_Object_Secondary_Class_t*) secondaryClass->NW_Object_Core.super;
        }

        /* if we reached the aggregate class, tag it as done */
        if (secondaryClass == (void*) &NW_Object_Aggregate_Class) {
          *(void**) secondary = NULL;
        }
      }
    }
  }
}
