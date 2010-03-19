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

/** ----------------------------------------------------------------------- **
    @package:     NW_HED

    @synopsis:    Convenience functions for accessing the DOM.

    @description: default

 ** ----------------------------------------------------------------------- **/

#ifndef NW_HED_DOM_HELPER_H
#define NW_HED_DOM_HELPER_H

#include "nw_dom_node.h"
#include "nw_string_string.h"
#include "nw_hed_context.h"
#include "nw_hed_entityset.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   public data types
 * ------------------------------------------------------------------------- */

/** ----------------------------------------------------------------------- **
    @struct:       NW_HED_DomHelper_t

    @synopsis:    default

    @scope:       public
    @variables:
       const NW_HED_EntitySet_t*
                  default

       NW_HED_Context_t* context
                  default

       NW_Bool resolvePlainTextVariablesAndEntities
                  default

    @description: default
 ** ----------------------------------------------------------------------- **/
typedef struct NW_HED_DomHelper_s{
  const NW_HED_EntitySet_t* entitySet;
  NW_HED_Context_t* context;

  NW_Bool resolvePlainTextVariablesAndEntities;
} NW_HED_DomHelper_t;



/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */

/** ----------------------------------------------------------------------- **
    @function:    NW_HED_DomHelper_ResolveEntities

    @synopsis:    default

    @scope:       public

    @parameters:
       [in] const NW_HED_EntitySet_t* entitySet
                  default

       [in] const NW_Ucs2* origText
                  default

       [out] NW_String_t* destString
                  default

    @description: default

    @returns:     TBrowserStatusCode
                  default

       [default]
                  default

 ** ----------------------------------------------------------------------- **/
TBrowserStatusCode NW_HED_DomHelper_ResolveEntities (const NW_HED_EntitySet_t* entitySet,
                                                     const NW_Ucs2* origText, 
                                                     NW_String_t* destString);


/** ----------------------------------------------------------------------- **
    @function:    NW_HED_DomHelper_GetAttributeValue

    @synopsis:    default

    @scope:       public

    @parameters:
       [in] const NW_HED_DomHelper_t* domHelper
                  default

       [in] const NW_DOM_Node_t* element
                  default

       [in] NW_Uint16 attribute
                  default

       [out] NW_String_t* value
                  default

    @description: default

    @returns:     TBrowserStatusCode
                  default

       [default]
                  default

 ** ----------------------------------------------------------------------- **/
TBrowserStatusCode NW_HED_DomHelper_GetAttributeValue (const NW_HED_DomHelper_t* domHelper,
                                                const NW_DOM_Node_t* element, 
                                                NW_Uint16 attribute, 
                                                NW_String_t* value);

TBrowserStatusCode NW_HED_DomHelper_GetHTMLAttributeValue (const NW_HED_DomHelper_t* domHelper,
                                                const NW_DOM_Node_t* element, 
                                                NW_Uint16 attribute, 
                                                NW_String_t* value);

/** ----------------------------------------------------------------------- **
    @function:    NW_HED_DomHelper_GetText

    @synopsis:    default

    @scope:       public

    @parameters:
       [in] const NW_HED_DomHelper_t* domHelper
                  default

       [in] const NW_DOM_Node_t* element
                  default

       [in] NW_Bool forceUcs2Copy
                  default

       [out] NW_String_t* value
                  default

       [out] NW_Uint32* valueEncoding
                  default

    @description: default

    @returns:     TBrowserStatusCode
                  default

       [default]
                  default

 ** ----------------------------------------------------------------------- **/
TBrowserStatusCode NW_HED_DomHelper_GetText (const NW_HED_DomHelper_t* domHelper,
                                      const NW_DOM_Node_t* element,
                                      NW_Bool forceUcs2Copy,
                                      NW_String_t* value,
                                      NW_Uint32* valueEncoding);


/** ----------------------------------------------------------------------- **
    @function:    NW_HED_DomHelper_GetChildText

    @synopsis:    default

    @scope:       public

    @parameters:
       [in] const NW_HED_DomHelper_t* domHelper
                  default

       [in] const NW_DOM_Node_t* element
                  default

       [out] NW_String_t* value
                  default

    @description: default

    @returns:     TBrowserStatusCode
                  default

       [default]
                  default

 ** ----------------------------------------------------------------------- **/
TBrowserStatusCode NW_HED_DomHelper_GetChildText (const NW_HED_DomHelper_t* domHelper,
                                           const NW_DOM_Node_t* element,
                                           NW_String_t* value);

/** ----------------------------------------------------------------------- **
    @function:    NW_HED_DomHelper_AttributeEquals

    @synopsis:    default

    @scope:       public

    @parameters:
       [in] const NW_HED_DomHelper_t* domHelper
                  default

       [in] const NW_DOM_Node_t* element
                  default

       [in] NW_Uint16 attribute
                  default

       [in] const NW_String_t* value
                  default

    @description: default

    @returns:     NW_Bool
                  default

 ** ----------------------------------------------------------------------- **/
NW_Bool NW_HED_DomHelper_AttributeEquals (const NW_HED_DomHelper_t* domHelper,
                                          const NW_DOM_Node_t* element, 
                                          NW_Uint16 attribute,
                                          const NW_String_t* value);


/** ----------------------------------------------------------------------- **
    @function:    NW_HED_DomHelper_IsMatch

    @synopsis:    default

    @scope:       public

    @parameters:
       [in] const NW_HED_DomHelper_t* domHelper
                  default

       [in] const NW_DOM_Node_t* element
                  default

       [in] NW_Uint16 elementType
                  default

       [in] NW_Uint16 attribute
                  default

       [in] const attributeMatchString
                  default

    @description: default

    @returns:     NW_Bool
                  default

       [default]
                  default

 ** ----------------------------------------------------------------------- **/
NW_Bool NW_HED_DomHelper_IsMatch (const NW_HED_DomHelper_t* domHelper,
                                  NW_DOM_Node_t* element,
                                  NW_Uint16 elementType,
                                  NW_Uint16 attribute,
                                  const NW_String_t* attributeMatchString);


/** ----------------------------------------------------------------------- **
    @function:    NW_HED_DomHelper_FindElement

    @synopsis:    default

    @scope:       public

    @parameters:
       [in] const NW_HED_DomHelper_t* domHelper
                  default

       [in] NW_DOM_Node_t* rootNode
                  default

       [in] NW_Int32 maxSearchDepth
                  default

       [in] NW_Uint16 elementType
                  default

       [in] NW_Uint16 attribute
                  default

       [in] const NW_String_t* attributeMatchString
                  default

    @description: default

    @returns:     NW_DOM_Node_t*
                  default

 ** ----------------------------------------------------------------------- **/
NW_DOM_Node_t* NW_HED_DomHelper_FindElement (const NW_HED_DomHelper_t* domHelper,
                                             NW_DOM_Node_t* rootNode,
                                             NW_Int32 maxSearchDepth,
                                             NW_Uint16 elementType,
                                             NW_Uint16 attribute,
                                             const NW_String_t* attributeMatchString);


/** ----------------------------------------------------------------------- **
    @function:    NW_HED_DomHelper_GetEncoding

    @synopsis:    default

    @scope:       public

    @parameters:
       [in] const NW_DOM_Node_t* element
                  default

    @description: default

    @returns:     NW_Uint32
                  default

 ** ----------------------------------------------------------------------- **/
NW_Uint32 NW_HED_DomHelper_GetEncoding (const NW_DOM_Node_t* element);


/** ----------------------------------------------------------------------- **
    @function:    NW_HED_DomHelper_GetDictionary

    @synopsis:    default

    @scope:       public

    @parameters:
       [in] const NW_DOM_Node_t* element
                  default

    @description: default

    @returns:     NW_WBXML_Dictionary_t*
                  default

 ** ----------------------------------------------------------------------- **/
NW_WBXML_Dictionary_t* NW_HED_DomHelper_GetDictionary (const NW_DOM_Node_t* element);


/** ----------------------------------------------------------------------- **
    @function:    NW_HED_DomHelper_GetAttributeName

    @synopsis:    default

    @scope:       public

    @parameters:
       [in] const NW_DOM_Node_t* element
                  default

       [in] NW_Uint16 attribute
                  default

       [out] NW_String_t* name
                  default

    @description: default

    @returns:     TBrowserStatusCode
                  default

       [default]
                  default

 ** ----------------------------------------------------------------------- **/
TBrowserStatusCode NW_HED_DomHelper_GetAttributeName (const NW_DOM_Node_t* element, 
                                               NW_Uint16 attribute, 
                                               NW_String_t* name);

/** ----------------------------------------------------------------------- **
    @function:    NW_HED_DomHelper_GetElementToken

    @synopsis:    default

    @scope:       public

    @parameters:
       [in] const NW_DOM_Node_t* element
                  default

    @description: default

    @returns:     NW_Uint16
                  default

 ** ----------------------------------------------------------------------- **/
NW_Uint16 NW_HED_DomHelper_GetElementToken (const NW_DOM_Node_t* element);


/** ----------------------------------------------------------------------- **
    @function:    NW_HED_DomHelper_GetElementName

    @synopsis:    default

    @scope:       public

    @parameters:
       [in] const NW_DOM_Node_t* element
                  default

       [out] NW_String_t* name
                  default

    @description: default

    @returns:     TBrowserStatusCode
                  default

       [default]
                  default

 ** ----------------------------------------------------------------------- **/
TBrowserStatusCode NW_HED_DomHelper_GetElementName (const NW_DOM_Node_t* element,
                                             NW_String_t* name);

/** ----------------------------------------------------------------------- **
    @function:    NW_HED_DomHelper_PrintTree

    @synopsis:    default

    @scope:       public

    @parameters:
       [in] const NW_HED_DomHelper_t* domHelper
                  default

       [in] const rootNode
                  const NW_DOM_Node_t* rootNode

    @description: default

    @returns:     TBrowserStatusCode
                  default

       [default]
                  default

 ** ----------------------------------------------------------------------- **/
TBrowserStatusCode NW_HED_DomHelper_PrintTree (const NW_HED_DomHelper_t* domHelper,
                                        const NW_DOM_Node_t* rootNode);


TBrowserStatusCode NW_HED_DomHelper_PrintNode (const NW_HED_DomHelper_t* domHelper,
                                                const NW_DOM_Node_t* domNode);
/** ----------------------------------------------------------------------- **
    @function:    NW_HED_DomHelper_CatString

    @synopsis:    default

    @scope:       public

    @parameters:
       [in-out] NW_String_t* destString
                  default

       [in] const NW_String_t* srcString
                  default

       [in] NW_Uint32 srcEncoding
                  default

    @description: default

    @returns:     TBrowserStatusCode
                  default

       [default]
                  default

 ** ----------------------------------------------------------------------- **/
TBrowserStatusCode NW_HED_DomHelper_CatString (NW_String_t* destString, 
                                        const NW_String_t* srcString, 
                                        NW_Uint32 srcEncoding);

/** ----------------------------------------------------------------------- **
    @function:    NW_HED_DomHelper_CatUcs2String

    @synopsis:    default

    @scope:       public

    @parameters:
       [in-out] NW_String_t* destString
                  default

       [in] const NW_Ucs2* srcString
                  default

    @description: default

    @returns:     TBrowserStatusCode
                  default

       [default]
                  default

 ** ----------------------------------------------------------------------- **/
TBrowserStatusCode NW_HED_DomHelper_CatUcs2String (NW_String_t* destString, 
                                            const NW_Ucs2* srcString);

/** ----------------------------------------------------------------------- **
    @function:    NW_HED_DomHelper_HasAttribute

    @synopsis:    default

    @scope:       public

    @parameters:

       [in] const NW_DOM_Node_t* element
                  default

       [in] NW_Uint16 attribute
                  default

    @description: default

    @returns:     TBrowserStatusCode
                  KBrsrSuccess- Attribute is found.
                  KBrsrNotFound - Attribute is not found.
                  KBrsrDomNodeTypeErr - not a DOM node

       [default]
                  default
 ** ----------------------------------------------------------------------- **/

TBrowserStatusCode  NW_HED_DomHelper_HasAttribute(const NW_DOM_Node_t* element, 
                                                NW_Uint16 attributeToken);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_HED_DOM_HELPER_H */
