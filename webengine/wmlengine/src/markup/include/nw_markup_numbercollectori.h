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


#ifndef _NW_Markup_NumberCollectorI_h_
#define _NW_Markup_NumberCollectorI_h_

#include "nw_object_aggregatei.h"
#include "nw_hed_inumbercollectori.h"
#include "nw_markup_numbercollector.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/* ------------------------------------------------------------------------- *
   protected global data
 * ------------------------------------------------------------------------- */
NW_MARKUP_EXPORT const NW_Object_Class_t* const _NW_Markup_NumberCollector_SecondaryList[];
NW_MARKUP_EXPORT const NW_HED_INumberCollector_Class_t NW_Markup_NumberCollector_INumberCollector_Class;

/* ------------------------------------------------------------------------- *
   virtual method implementation prototypes
 * ------------------------------------------------------------------------- */

/** ----------------------------------------------------------------------- **

    @method:      NW_Murkup_NumberCollector_Collect

    @synopsis:    Returns all text strings the may contain phone numbers.

    @scope:       public
    @access:      implementation

    @parameters:
       [in, out] NW_ADT_DynamicVector_t* dynamicVector
                  The dynamic vector into which all the phone numbers will be
                  placed. This is a vector of NW_Text_t*.

    @description: The function traverses the box tree, looks inside text and 
                  input boxes and makes text objects. The function attaches 
                  all created text objects to the dynamic vector. It is the caller's
                  responsibility to free the text objects.

                  Text from Password type input boxes is not passed to the caller.

                  Note that although the name of the function is NumberCollector
                  it in fact collects all text strings. The function can be
                  improved by introducing an algorithm of extructing numbers or
                  only strings that have numbers.

    @returns:     TBrowserStatusCode
       [KBrsrSuccess]
                  Successful completion.
     
       [KBrsrOutOfMemory]
                  An unrecoverable out of memory condition was reached.

       [KBrsrUnexpectedError]
                  An unexpected, unrecoverable and/or unhandled error was
                  encountered. Although the method will attempt to return
                  without disrupting the state of the system, this can not be
                  guaranteed.
 ** ----------------------------------------------------------------------- **/
NW_MARKUP_EXPORT
TBrowserStatusCode
_NW_Markup_NumberCollector_INumberCollector_Collect (NW_HED_INumberCollector_t* numberCollector,
                                                     NW_ADT_DynamicVector_t* dynamicVector);

NW_MARKUP_EXPORT
NW_Bool
_NW_Markup_NumberCollector_BoxIsValid (NW_Markup_NumberCollector_t* numberCollector,
                                       NW_LMgr_Box_t* box);

/* ------------------------------------------------------------------------- *
   protected methods
 * ------------------------------------------------------------------------- */
#define NW_Markup_NumberCollector_GetBoxTree(_numberCollector) \
  (NW_Object_Invoke (_numberCollector, NW_Markup_NumberCollector, getBoxTree) ( \
     NW_Markup_NumberCollectorOf (_numberCollector)))

#define NW_Markup_NumberCollector_GetHrefAttr(_numberCollector, _box) \
  (NW_Object_Invoke (_numberCollector, NW_Markup_NumberCollector, getHrefAttr) ( \
     NW_Markup_NumberCollectorOf (_numberCollector), NW_LMgr_BoxOf (_box)))

#define NW_Markup_NumberCollector_GetAltAttr(_numberCollector, _box) \
  (NW_Object_Invoke (_numberCollector, NW_Markup_NumberCollector, getAltAttr) ( \
     NW_Markup_NumberCollectorOf (_numberCollector), NW_LMgr_BoxOf (_box)))

#define NW_Markup_NumberCollector_BoxIsValid(_numberCollector, _box) \
  (NW_Object_Invoke (_numberCollector, NW_Markup_NumberCollector, boxIsValid) ( \
     NW_Markup_NumberCollectorOf (_numberCollector), NW_LMgr_BoxOf (_box)))

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_Markup_NumberCollectorI_h_ */
