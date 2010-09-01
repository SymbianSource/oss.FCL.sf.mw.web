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


#ifndef _NW_Object_AggregateI_h_
#define _NW_Object_AggregateI_h_

#include "nw_object_secondaryi.h"
#include "nw_object_aggregate.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/* ------------------------------------------------------------------------- *
   protected method prototypes
 * ------------------------------------------------------------------------- */
NW_OBJECT_EXPORT
TBrowserStatusCode
_NW_Object_Aggregate_Initialize (NW_Object_Core_t* core,
                                 va_list* argList);

NW_OBJECT_EXPORT
void
_NW_Object_Aggregate_Terminate (NW_Object_Core_t* object);

/* ------------------------------------------------------------------------- *
   protected methods
 * ------------------------------------------------------------------------- */

/** ----------------------------------------------------------------------- **
    @method:      NW_Object_Aggregate_Construct

    @synopsis:    The constructor method for aggregate objects.

    @scope:       protected
    @access:      virtual

    @parameters:
       va_list* argList
                  This method accepts a variable parameter list. It is the
                  responsibility of each constructor method in the
                  aggregate derivation hierarchy to remove all parameters
                  from the 'argList'.

    @description: TBD
                  
    @returns:     TBrowserStatusCode
                  The state of 'object' is undefined in case of all errors.

       [KBrsrSuccess]
                  Successful completion.
 ** ----------------------------------------------------------------------- **/
#define NW_Object_Aggregate_Construct(_aggregate, _argp) \
  (NW_Object_Invoke ((_aggregate), NW_Object_Aggregate, construct) ( \
     NW_Object_AggregateOf (_aggregate), \
     (_argp)))

/** ----------------------------------------------------------------------- **
    @method:      NW_Object_Aggregate_Destruct

    @synopsis:    The destructor method for aggregate objects.

    @scope:       protected
    @access:      virtual

    @description: The destructor method for aggregate objects.
 ** ----------------------------------------------------------------------- **/
#define NW_Object_Aggregate_Destruct(_aggregate) \
  (NW_Object_Invoke ((_aggregate), NW_Object_Aggregate, destruct) ( \
     NW_Object_AggregateOf (_aggregate)))

#define NW_Object_Aggregate_GetAggregator \
  NW_Object_GetOuter

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_Object_AggregateI_h_ */
