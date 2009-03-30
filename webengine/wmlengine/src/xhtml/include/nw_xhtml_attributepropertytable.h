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


#ifndef NW_XHTML_ATTRIBUTEPROPERTYTABLE_H
#define NW_XHTML_ATTRIBUTEPROPERTYTABLE_H

#include "nw_object_base.h"
#include "nw_dom_node.h"
#include "nw_lmgr_cssproperties.h"
#include "nw_text_coretextelementhandleri.h"
#include "nw_xhtml_xhtml_1_0_tokens.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum NW_XHTML_AttributePropertyTable_Type_e{
    NW_XHTML_APT_TOKEN,
    NW_XHTML_APT_COLOR,
    NW_XHTML_APT_TRANSCOLOR, /* supports colors plus transparent pen */
    NW_XHTML_APT_PIXEL,
    NW_XHTML_APT_INTEGER,
    NW_XHTML_APT_LENGTH,
    NW_XHTML_APT_STRING
}NW_XHTML_AttributePropertyTable_Type_t;

typedef struct NW_XHTML_AttributePropertyTable_CSSPropRequires_s
{
  NW_LMgr_PropertyName_t CSSPropName;/* corresponding CSS property */
  NW_LMgr_Property_t setting;        /* data type and value setting of CSS property */
} NW_XHTML_AttributePropertyTable_CSSPropRequires_t;

typedef struct NW_XHTML_AttributePropertyTable_ElementEntry_s
{
  NW_Uint16 elementToken;      /* dictionary token for element */
  NW_Uint16 attributeToken;    /* dictionary token for attribute */
  NW_LMgr_PropertyName_t CSSPropName;/* corresponding CSS property */
  NW_XHTML_AttributePropertyTable_Type_t type;     /* data type of property value */
  const NW_LMgr_PropertyValueToken_t* CSSPropVals; /* allowed values for this attribute */
  const NW_XHTML_AttributePropertyTable_CSSPropRequires_t* requires; 
                                     /* additional property */
                                     /* which must be set for prop to take effect */                          
} NW_XHTML_AttributePropertyTable_ElementEntry_t;


typedef struct NW_XHTML_AttributePropertyTable_PropValEntry_s
{
  const NW_Ucs2 *attrValStr;                   /* attribute value as a string */
  NW_Uint16 elementToken;                      /* the element token for the conversion; 0 if any */
  NW_Uint16 attributeToken;                    /* the attribute token for the conversion; 0 if any */
  NW_LMgr_PropertyValueToken_t CSSPropertyVal; /* corresponding CSS property value */
  NW_Bool case_sensitive;                      /* is attribute value case-sensitive? */
} NW_XHTML_AttributePropertyTable_PropValEntry_t;

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */

typedef struct NW_XHTML_AttributePropertyTable_Class_s NW_XHTML_AttributePropertyTable_Class_t;
typedef struct NW_XHTML_AttributePropertyTable_s NW_XHTML_AttributePropertyTable_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

typedef struct NW_XHTML_AttributePropertyTable_ClassPart_s {
  NW_Uint32 numPropertyVals;
  const NW_XHTML_AttributePropertyTable_PropValEntry_t* propertyVals;
  NW_Uint16 numElementEntries;
  const NW_XHTML_AttributePropertyTable_ElementEntry_t* elementEntries;
} NW_XHTML_AttributePropertyTable_ClassPart_t;

struct NW_XHTML_AttributePropertyTable_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_XHTML_AttributePropertyTable_ClassPart_t NW_XHTML_AttributePropertyTable;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_XHTML_AttributePropertyTable_s {
  NW_Object_Core_t super;
};


/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */

#define NW_XHTML_AttributePropertyTable_GetClassPart(_object) \
  (NW_Object_GetClassPart (_object, NW_XHTML_AttributePropertyTable))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */

extern const NW_XHTML_AttributePropertyTable_Class_t NW_XHTML_AttributePropertyTable_Class;
extern const NW_XHTML_AttributePropertyTable_t NW_XHTML_AttributePropertyTable;

/* -------------------------------------------------------------------------- *
    final methods
 * ---------------------------------------------------------------------------*/

extern TBrowserStatusCode
NW_XHTML_AttributePropertyTable_ApplyStyles(NW_XHTML_ContentHandler_t* contentHandler,
                            NW_DOM_ElementNode_t* elementNode,
                            NW_LMgr_Box_t* box);

extern TBrowserStatusCode 
NW_XHTML_AttributePropertyTable_AddActiveBoxDecoration(NW_LMgr_Box_t* box, NW_Bool growBorder);

extern TBrowserStatusCode
NW_XHTML_AttributePropertyTable_ApplyGridModeTableStyle(NW_LMgr_Box_t* aBox);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_XHTML_ATTRIBUTEPROPERTYTABLE_H */
