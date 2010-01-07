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


#ifndef NW_CXML_DOM_DOMVISITOR_H
#define NW_CXML_DOM_DOMVISITOR_H

#include "nw_object_dynamic.h"
#include <nw_dom_node.h>
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/** ----------------------------------------------------------------------- **
    @struct:       NW_cXML_DOM_DOMVisitor_ClassPart

    @synopsis:    default

    @scope:       public
    @variables:
       void** unused
                  default

    @description: default
 ** ----------------------------------------------------------------------- **/
typedef struct NW_cXML_DOM_DOMVisitor_ClassPart_s {
  void** unused;
} NW_cXML_DOM_DOMVisitor_ClassPart_t;


/** ----------------------------------------------------------------------- **
    @struct:      NW_cXML_DOM_DOMVisitor_Class

    @synopsis:    default

    @scope:       public
    @variables:
       NW_Object_Core_ClassPart_t NW_Object_Core
                  default

       NW_Object_Base_ClassPart_t NW_Object_Base
                  default

       NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic
                  default

       NW_cXML_DOM_DOMVisitor_ClassPart_t NW_cXML_DOM_DOMVisitor
                  default

    @description: default
 ** ----------------------------------------------------------------------- **/
typedef struct NW_cXML_DOM_DOMVisitor_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_cXML_DOM_DOMVisitor_ClassPart_t NW_cXML_DOM_DOMVisitor;
}NW_cXML_DOM_DOMVisitor_Class_t;


/** ----------------------------------------------------------------------- **
    @struct:      NW_cXML_DOM_DOMVisitor

    @synopsis:    default

    @scope:       public
    @variables:
       NW_Object_Dynamic_t super
                  default

       NW_DOM_Node_t* rootNode
                  default

       NW_DOM_Node_t* currentNode
                  default

    @description: default
 ** ----------------------------------------------------------------------- **/
typedef struct NW_cXML_DOM_DOMVisitor_s {
  NW_Object_Dynamic_t super;
  /* member variables */
  NW_DOM_Node_t* rootNode;
  NW_DOM_Node_t* currentNode;
}NW_cXML_DOM_DOMVisitor_t;


/* ------------------------------------------------------------------------- */
#define NW_cXML_DOM_DOMVisitor_GetClassPart(_object) \
  (NW_Object_GetClassPart (_object, NW_cXML_DOM_DOMVisitor))

#define NW_cXML_DOM_DOMVisitorOf(_object) \
  (NW_Object_Cast (_object, NW_cXML_DOM_DOMVisitor))

/* ------------------------------------------------------------------------- */
extern const NW_cXML_DOM_DOMVisitor_Class_t NW_cXML_DOM_DOMVisitor_Class;

/* ------------------------------------------------------------------------- *
   final public methods
 * ------------------------------------------------------------------------- */


/** ----------------------------------------------------------------------- **
    @function:    NW_cXML_DOM_DOMVisitor_SetRootNode

    @synopsis:    Sets root node.

    @scope:       public

    @parameters:
       [in-out] NW_cXML_DOM_DOMVisitor_t* visitor
                  default

       [in-out] NW_DOM_Node_t* rootNode
                  default

    @description: Sets root node.

    @returns:     TBrowserStatusCode
                  default

       [default]
                  default

 ** ----------------------------------------------------------------------- **/
extern  TBrowserStatusCode
NW_cXML_DOM_DOMVisitor_SetRootNode (NW_cXML_DOM_DOMVisitor_t* visitor,
                                    NW_DOM_Node_t* rootNode);

/** ----------------------------------------------------------------------- **
    @function:    NW_cXML_DOM_DOMVisitor_GetRootNode

    @synopsis:    Gets root node.

    @scope:       public

    @parameters:
       [in-out] NW_cXML_DOM_DOMVisitor_t* visitor
                  default

    @description: Returns the root node.

    @returns:     NW_DOM_Node_t*
                  default

 ** ----------------------------------------------------------------------- **/
extern
NW_DOM_Node_t*
NW_cXML_DOM_DOMVisitor_GetRootNode (NW_cXML_DOM_DOMVisitor_t* visitor);


/** ----------------------------------------------------------------------- **
    @function:    NW_cXML_DOM_DOMVisitor_Next

    @synopsis:    Gets next node.

    @scope:       public

    @parameters:
       [in] NW_cXML_DOM_DOMVisitor_t* visitor
                  default

       [in] NW_Uint8 type
                  default

     @description: Returns the next node of the given type.  Starting with 
                  the root node it returns nodes in a depth-first fashion.
                  NULL is returned when all of the nodes have been iterated
                  over.

    @returns:     NW_DOM_Node_t*
                  default

       [default]
                  default

 ** ----------------------------------------------------------------------- **/
extern
NW_DOM_Node_t*
NW_cXML_DOM_DOMVisitor_Next (NW_cXML_DOM_DOMVisitor_t* visitor,
                             NW_Uint8 type);


/** ----------------------------------------------------------------------- **
    @function:    NW_cXML_DOM_DOMVisitor_NextChild

    @synopsis:    Like Next except that it only iterates over the root's 
                  direct children.  NULL is returned when all of the nodes 
                  have been iterated over.

    @scope:       public

    @parameters:
       [in] NW_cXML_DOM_DOMVisitor_t* visitor
                  default

       [in] NW_Uint8 type
                  default

    @description: Like Next except that it only iterates over the root's direct children..

    @returns:     NW_DOM_Node_t*
                  default

 ** ----------------------------------------------------------------------- **/
extern
NW_DOM_Node_t*
NW_cXML_DOM_DOMVisitor_NextChild (NW_cXML_DOM_DOMVisitor_t* visitor,
                                  NW_Uint8 type);


/** ----------------------------------------------------------------------- **
    @function:    NW_cXML_DOM_DOMVisitor_Reset

    @synopsis:    resets the visitor to its initial state.

    @scope:       public

    @parameters:
       [in] NW_cXML_DOM_DOMVisitor_t* visitor
                  default

    @description: resets the visitor to its initial state.

    @returns:     TBrowserStatusCode
                  default

 ** ----------------------------------------------------------------------- **/
extern
TBrowserStatusCode
NW_cXML_DOM_DOMVisitor_Reset (NW_cXML_DOM_DOMVisitor_t* visitor);

/* ------------------------------------------------------------------------- */
/* Gets the next node using skipChildren functionality                       */
/* ------------------------------------------------------------------------- */
NW_DOM_Node_t*
NW_cXML_DOM_DOMVisitor_NextNode (NW_cXML_DOM_DOMVisitor_t* visitor,
                                 NW_Uint8 type,
                                 NW_Bool skipChildren);


/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */


/** ----------------------------------------------------------------------- **
    @function:    NW_cXML_DOM_DOMVisitor_New

    @synopsis:    Creates new node.

    @scope:       public

    @parameters:
       [in] NW_DOM_Node_t* rootNode
                  default

    @description: Creates new node.

    @returns:     NW_cXML_DOM_DOMVisitor_t*
                  default

       [default]
                  default

 ** ----------------------------------------------------------------------- **/
extern
NW_cXML_DOM_DOMVisitor_t*
NW_cXML_DOM_DOMVisitor_New (NW_DOM_Node_t* rootNode);


extern
TBrowserStatusCode
NW_cXML_DOM_DOMVisitor_Initialize (NW_cXML_DOM_DOMVisitor_t* visitor, NW_DOM_Node_t* rootNode);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_CXML_DOM_DOMVISITOR_H */
