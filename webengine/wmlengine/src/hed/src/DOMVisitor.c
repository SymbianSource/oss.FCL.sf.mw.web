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


#include "nw_dom_domvisitori.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_cXML_DOM_DOMVisitor_Class_t NW_cXML_DOM_DOMVisitor_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_Object_Dynamic_Class,
    /* queryInterface            */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base            */
    /* interfaceList             */ NULL
  },
  { /* NW_Object_Dynamic         */
    /* instanceSize		 */ sizeof (NW_cXML_DOM_DOMVisitor_t),
    /* construct		 */ _NW_cXML_DOM_DOMVisitor_Construct,
    /* destruct			 */ NULL
  },
  { /* NW_cXML_DOM_DOMVisitor	 */
    /* unused                    */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_cXML_DOM_DOMVisitor_Construct (NW_Object_Dynamic_t* dynamicObject,
                                va_list* argp)
{
  NW_cXML_DOM_DOMVisitor_t* thisObj;

  /* for convenience */
  thisObj = NW_cXML_DOM_DOMVisitorOf (dynamicObject);

  /* initialize the member variables */
  thisObj->rootNode = va_arg (*argp, NW_DOM_Node_t*);

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_cXML_DOM_DOMVisitor_SetRootNode (NW_cXML_DOM_DOMVisitor_t* visitor,
                                 NW_DOM_Node_t* rootNode)
{
  /* set the rootNode member variable - don't forget to clear the currentNode,
     after all this may be a completely new document! */
  visitor->rootNode = rootNode;
  visitor->currentNode = NULL;

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
NW_DOM_Node_t*
NW_cXML_DOM_DOMVisitor_GetRootNode (NW_cXML_DOM_DOMVisitor_t* visitor)
{
  return visitor->rootNode;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_cXML_DOM_DOMVisitor_Reset (NW_cXML_DOM_DOMVisitor_t* visitor)
{
  visitor->currentNode = NULL;
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
NW_DOM_Node_t*
NW_cXML_DOM_DOMVisitor_Next (NW_cXML_DOM_DOMVisitor_t* visitor,
                             NW_Uint8 type)
{
  /* if this is the first time we're called, we simply return the specified
     root */
  if (visitor->currentNode == NULL) {
    visitor->currentNode = visitor->rootNode;
    if (type & NW_DOM_Node_getNodeType (visitor->currentNode)) {
      return visitor->currentNode;
    }
  }

  do {
    NW_DOM_Node_t* node;
    NW_DOM_Node_t* parentNode;

    /* ok, we've been here before ... check whether the current node has any
       children, if so we return the first of its children and make that the
       current node */
    node = NW_DOM_Node_getFirstChild (visitor->currentNode);
    if (node != NULL) {
      visitor->currentNode = node;
      continue;
    }

    /* so, no children - if currentNode equals rootNode were done */
    if (visitor->currentNode == visitor->rootNode) {
      visitor->currentNode = NULL;
      continue;
    }

    /* so, no children (although abstinence is hard) - look for a sibling, if
       found make that our current node and return it */
    node = NW_DOM_Node_getNextSibling (visitor->currentNode);
    if (node != NULL) {
      visitor->currentNode = node;
      continue;
    }

    /* what? no siblings? oh well, we'll have to look for the first sibling of
       the first ancestor we can find (not including our root) */
    for (parentNode = NW_DOM_Node_getParentNode (visitor->currentNode);
         parentNode != visitor->rootNode;
         parentNode = NW_DOM_Node_getParentNode (parentNode)) {
      node = NW_DOM_Node_getNextSibling (parentNode);
      if (node != NULL) {
        visitor->currentNode = node;
        break;
      }
    }
    if (node != NULL) {
      continue;
    }

    /* ok, if we get this far it means that we can find no more nodes to
       iterate over */
    visitor->currentNode = NULL;
  } while (visitor->currentNode != NULL &&
           visitor->currentNode != visitor->rootNode &&
           ! (type & NW_DOM_Node_getNodeType (visitor->currentNode)));

  /* return the result */
  return visitor->currentNode;
}

/* ------------------------------------------------------------------------- */
/* iterates only over the first level children                               */
/* ------------------------------------------------------------------------- */
NW_DOM_Node_t*
NW_cXML_DOM_DOMVisitor_NextChild (NW_cXML_DOM_DOMVisitor_t* visitor,
                                  NW_Uint8 type)
{
  NW_DOM_Node_t* node;

  /* return NULL if the root node has no children */
  if (NW_DOM_Node_hasChildNodes (visitor->rootNode) == NW_FALSE) {
    return NULL;
  }

  /* search for a child of the given type */
  node = visitor->currentNode;
  do {
    if (node == NULL) {
      node = NW_DOM_Node_getFirstChild (visitor->rootNode);
    }
    else  {
      node = NW_DOM_Node_getNextSibling (node);
    }
  } while ((node != NULL) && ((type & NW_DOM_Node_getNodeType (node)) == 0));

  /* return the result */
  visitor->currentNode = node;
  return node;
}

/* ------------------------------------------------------------------------- */
/* Gets the next node using skipChildren functionality                       */
/* ------------------------------------------------------------------------- */
NW_DOM_Node_t*
NW_cXML_DOM_DOMVisitor_NextNode (NW_cXML_DOM_DOMVisitor_t* visitor,
                                 NW_Uint8 type,
                                 NW_Bool skipChildren)
{
  NW_DOM_Node_t* node = NULL;
  NW_DOM_Node_t* nextNode = NULL;

  /* return NULL if the root node has no children */
  if ((visitor->currentNode == NULL) || (NW_DOM_Node_getParentNode(visitor->currentNode) == NULL)){
    return NULL;
  }

  if (skipChildren)
  {
      /* search for a child of the given type */
      node = visitor->currentNode;
      do
      {
        nextNode = NW_DOM_Node_getNextSibling (node);
        if (nextNode && (type & NW_DOM_Node_getNodeType (nextNode)))
        {
           break;
        }
        node = NW_DOM_Node_getParentNode(node);
      } while (node != NULL);
  }
  else
  {
    nextNode = NW_cXML_DOM_DOMVisitor_Next(visitor, type);
  }

  /* return the result */
  return nextNode;
}


/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_cXML_DOM_DOMVisitor_t*
NW_cXML_DOM_DOMVisitor_New (NW_DOM_Node_t* rootNode)
{
  return (NW_cXML_DOM_DOMVisitor_t*)
    NW_Object_New (&NW_cXML_DOM_DOMVisitor_Class, rootNode);
}


/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_cXML_DOM_DOMVisitor_Initialize (NW_cXML_DOM_DOMVisitor_t* visitor, NW_DOM_Node_t* rootNode)
{
  return NW_Object_Initialize (&NW_cXML_DOM_DOMVisitor_Class, visitor, rootNode);
}
