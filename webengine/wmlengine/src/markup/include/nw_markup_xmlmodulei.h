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


#ifndef NW_MARKUP_XMLMODULEI_H
#define NW_MARKUP_XMLMODULEI_H

#include "nw_markup_modulei.h"
#include "nw_markup_xmlmodule.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   protected types
 * ------------------------------------------------------------------------- */
struct NW_Markup_XMLModule_ElementHandlerEntry_s
{
  const NW_Ucs2* tagName;
  NW_Markup_ElementHandler_t* elementHandler;
};

/* ------------------------------------------------------------------------- *
   method implementation prototypes
 * ------------------------------------------------------------------------- */
NW_MARKUP_EXPORT
NW_Markup_ElementHandler_t*
_NW_Markup_XMLModule_GetElementHandler (const NW_Markup_Module_t* module,
                                       const NW_DOM_ElementNode_t* domElement);

/* ------------------------------------------------------------------------- *
   protected convenience macros
 * ------------------------------------------------------------------------- */
#define NW_MARKUP_NUM_ELEMENT_HANDLERS(_module) \
  (sizeof (_NW_##_module##Module_ElementHandlers) \
   / sizeof (NW_Markup_XMLModule_ElementHandlerEntry_t))

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_MARKUP_XMLMODULEI_H */
