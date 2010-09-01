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


#ifndef NW_XHTML_TELEPHONYSERVICES_H
#define NW_XHTML_TELEPHONYSERVICES_H

#include "nw_object_base.h"
#include "nw_xhtml_telephonyservices.h"
#include "nw_xhtml_xhtmlcontenthandler.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_XHTML_TelephonyServices_Class_s NW_XHTML_TelephonyServices_Class_t;
typedef struct NW_XHTML_TelephonyServices_s NW_XHTML_TelephonyServices_t;

/* ------------------------------------------------------------------------- *
   method prototypes
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_XHTML_TelephonyServices_ClassPart_s {
  NW_Uint8 unused;
} NW_XHTML_TelephonyServices_ClassPart_t;

struct NW_XHTML_TelephonyServices_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_XHTML_TelephonyServices_ClassPart_t NW_XHTML_TelephonyServices;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_XHTML_TelephonyServices_s {
  NW_Object_Core_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_XHTML_TelephonyServices_GetClassPart(_object) \
  (NW_Object_GetClassPart((_object), NW_XHTML_TelephonyServices))

#define NW_XHTML_TelephonyServicesOf(_object) \
  (NW_Object_Cast (_object, NW_XHTML_TelephonyServices))

 /* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
extern const NW_XHTML_TelephonyServices_Class_t NW_XHTML_TelephonyServices_Class;
extern const NW_XHTML_TelephonyServices_t NW_XHTML_TelephonyServices;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */

extern
TBrowserStatusCode
NW_XHTML_TelephonyServices_MakeCall(NW_XHTML_TelephonyServices_t* telServices,
                                    NW_XHTML_ContentHandler_t* contentHandler,
                                    NW_Text_t* number,
                                    NW_Bool cti);

extern
TBrowserStatusCode
NW_XHTML_TelephonyServices_SaveToPhoneBook(NW_XHTML_TelephonyServices_t* telServices,
                                           NW_XHTML_ContentHandler_t* contentHandler,
                                           NW_Text_t* number,
                                           NW_Text_t* name,
                                           NW_Text_t* email,
                                           NW_Bool cti);

extern
NW_Bool
NW_XHTML_TelephonyServices_ParseNumber(NW_XHTML_TelephonyServices_t* telServices,
                                       NW_Ucs2* number,
                                       NW_Bool cti);

TBrowserStatusCode
NW_XHTML_TelephonyServices_Mailto_SaveToPhoneBook(NW_XHTML_TelephonyServices_t* telServices,
                                                  NW_XHTML_ContentHandler_t* contentHandler,
                                                  NW_Text_t* email,
                                                  NW_Text_t* name,
                                                  NW_Bool isEmail);

TBrowserStatusCode 
NW_UI_Telephony_MakeCall(const NW_Ucs2 *number);

TBrowserStatusCode 
NW_UI_Telephony_AddPBEntry(const NW_Ucs2 *number, const NW_Ucs2* name, const NW_Ucs2* email);



#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_XHTML_MODULE_H */
