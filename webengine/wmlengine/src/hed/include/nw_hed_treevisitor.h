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


#ifndef _NW_HED_TreeVisitor_h_
#define _NW_HED_TreeVisitor_h_

#include "nw_object_dynamic.h"
#include "nw_hed_documentnode.h"
#include "nw_adt_iterator.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** ----------------------------------------------------------------------- **
    @class:       NW_HED_TreeVisitor

    @scope:       public

    @description: 
 ** ----------------------------------------------------------------------- **/

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_HED_TreeVisitor_Class_s NW_HED_TreeVisitor_Class_t;
typedef struct NW_HED_TreeVisitor_s NW_HED_TreeVisitor_t;

typedef struct NW_HED_TreeVisitor_ConfigEntry_s NW_HED_TreeVisitor_ConfigEntry_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_HED_TreeVisitor_ClassPart_s {
  void** unused;
} NW_HED_TreeVisitor_ClassPart_t;

struct NW_HED_TreeVisitor_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_HED_TreeVisitor_ClassPart_t NW_HED_TreeVisitor;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_HED_TreeVisitor_s {
  NW_Object_Dynamic_t super;

  /* member variables */
  NW_HED_DocumentNode_t* rootNode;
  NW_HED_DocumentNode_t* currentNode;
  NW_ADT_Iterator_t* childIterator;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_HED_TreeVisitor_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_HED_TreeVisitor))

#define NW_HED_TreeVisitorOf(_object) \
  (NW_Object_Cast (_object, NW_HED_TreeVisitor))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT const NW_HED_TreeVisitor_Class_t NW_HED_TreeVisitor_Class;

/* ------------------------------------------------------------------------- *
   public method prototypes
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT
NW_Bool
_NW_HED_TreeVisitor_HasNext (NW_HED_TreeVisitor_t* treeVisitor);

NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_TreeVisitor_GetNext (NW_HED_TreeVisitor_t* treeVisitor,
                             NW_HED_DocumentNode_t** documentNode);

NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_TreeVisitor_Initialize (NW_HED_TreeVisitor_t* treeVisitor,
                               NW_HED_DocumentNode_t* rootNode);

NW_HED_EXPORT
NW_HED_TreeVisitor_t*
_NW_HED_TreeVisitor_New (NW_HED_DocumentNode_t* rootNode);

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */

/** ----------------------------------------------------------------------- **
    @method:      NW_HED_TreeVisitor_HasNext

    @synopsis:    Specifies whether there are any more document nodes to
                  iterate.

    @scope:       public
    @access:      final

    @description: This method will return a boolean specifying whether there
                  are more document nodes to iterate; in other words, whether
                  a successive call to 'NW_HED_TreeVisitor_GetNext' would
                  succeed or fail.
                  
    @returns:     NW_Bool
                  Returns a boolean specifying if there are any more document
                  nodes to iterate.

       [TRUE]
                  There are more node's to iterate. Another call to
                  'NW_HED_TreeVisitor_GetNext' will succeed.

       [FALSE]
                  There are no more document nodes to iterate. Another call to
                  'NW_HED_TreeVisitor_GetNext' will fail.
 ** ----------------------------------------------------------------------- **/
#define NW_HED_TreeVisitor_HasNext(_treeVisitor) \
  (_NW_HED_TreeVisitor_HasNext ( \
     NW_HED_TreeVisitorOf (_treeVisitor)))

/** ----------------------------------------------------------------------- **
    @method:      

    @synopsis:    

    @scope:       public
    @access:      final

    @parameters:
       [out] NW_HED_DocumentNode_t** documentNode

    @description:                   
                  
    @returns:     

       [KBrsrSuccess]
                  Successful completion; 'documentNode' is a pointer to the
                  next document node.

       [KBrsrUnexpectedError]
                  An unexpected error occurred.
 ** ----------------------------------------------------------------------- **/
#define NW_HED_TreeVisitor_GetNext(_treeVisitor, _documentNode) \
  (_NW_HED_TreeVisitor_GetNext ( \
     NW_HED_TreeVisitorOf (_treeVisitor), \
     (_documentNode)))

/** ----------------------------------------------------------------------- **
    @method:      NW_HED_TreeVisitor_Initialize

    @synopsis:    

    @scope:       public
    @access:      static

    @parameters:
       [in] NW_HED_TreeVisitor_t* treeVisitor

       [in] NW_HED_DocumentNode_t* rootNode

    @description:                   
                  
    @returns:     TBrowserStatusCode

       [KBrsrSuccess]
                  Successful completion; 

       [KBrsrUnexpectedError]
                  An unexpected error occurred.
 ** ----------------------------------------------------------------------- **/
#define NW_HED_TreeVisitor_Initialize(_treeVisitor, _rootNode) \
  (_NW_HED_TreeVisitor_Initialize ( \
     /*MT* NW_HED_TreeVisitorOf */(_treeVisitor), \
     NW_HED_DocumentNodeOf (_rootNode)))

/** ----------------------------------------------------------------------- **
    @method:      NW_HED_TreeVisitor_New

    @synopsis:    

    @scope:       public
    @access:      static

    @parameters:
       [in] NW_HED_DocumentNode_t* rootNode

    @description:                   
                  
    @returns:     NW_TreeVisitor_t*
 ** ----------------------------------------------------------------------- **/
#define NW_HED_TreeVisitor_New(_rootNode) \
  (_NW_HED_TreeVisitor_Initialize ( \
     NW_HED_DocumentNodeOf (_rootNode)))

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_HED_TreeVisitor_h_ */
