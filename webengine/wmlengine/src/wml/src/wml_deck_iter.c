/*
* Copyright (c) 1999 - 2001 Nokia Corporation and/or its subsidiary(-ies).
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

#include "nwx_logger.h"

#include <nw_dom_text.h>
#include "wml_deck_iter.h"
#include <nw_dom_document.h>
#include "BrsrStatusCodes.h"

static NW_DOM_Node_t* x_GetNextElement_AllChildren (NW_Wml_DeckIter_t* thisObj)
{
  NW_ASSERT (thisObj != NULL);

  /* if this is the first time we're called, we simply return the specified
     root */
  if (thisObj->currentElement == NULL) {
    thisObj->currentElement = thisObj->rootElement;
  }

  do {
    NW_DOM_Node_t* node;
    NW_DOM_Node_t* parentNode;

    /* ok, we've been here before ... check whether the current node has any
       children, if so we return the first of its children and make that the
       current node */
    node = NW_DOM_Node_getFirstChild (thisObj->currentElement);
    if (node != NULL) {
      thisObj->currentElement = node;
      continue;
    }

    /* so, no children (although abstinence is hard) - look for a sibling, if
       found make that our current node and return it */
    node = NW_DOM_Node_getNextSibling (thisObj->currentElement);
    if (node != NULL) {
      thisObj->currentElement = node;
      continue;
    }

    /* what? no siblings? oh well, we'll have to look for the first sibling of
       the first ancestor we can find (not including our root) */
    for (parentNode = NW_DOM_Node_getParentNode (thisObj->currentElement);
          ( (parentNode != thisObj->rootElement) && (parentNode != 0) );
         parentNode = NW_DOM_Node_getParentNode (parentNode)) {
      node = NW_DOM_Node_getNextSibling (parentNode);
      if (node != NULL) {
        thisObj->currentElement = node;
        break;
      }
    }
    if (node != NULL) {
      continue;
    }

    /* ok, if we get this far it means that we can find no more nodes to
       iterate over */
    thisObj->currentElement = NULL;
  } while ((thisObj->currentElement != NULL) &&
           (thisObj->currentElement != thisObj->rootElement) &&
           ! ((NW_DOM_ELEMENT_NODE | NW_DOM_TEXT_NODE) & NW_DOM_Node_getNodeType (thisObj->currentElement)));

  /* return the result */
  return thisObj->currentElement;
}

static NW_DOM_Node_t* x_GetNextElement_JustSiblings (NW_Wml_DeckIter_t* thisObj)
{
  NW_ASSERT (thisObj != NULL);

  if (thisObj->currentElement == NULL) {
    thisObj->currentElement = NW_DOM_Node_getFirstChild (thisObj->rootElement);
  }

  else {
    thisObj->currentElement = NW_DOM_Node_getNextSibling (thisObj->currentElement);
  }

  /* return the result */
  return thisObj->currentElement;
}

TBrowserStatusCode NW_Wml_DeckIter_Initialize (NW_Wml_DeckIter_t* thisObj,
                                        NW_DOM_Node_t* rootElement,
                                        NW_Bool justOwnChildren)
{
  NW_ASSERT (thisObj != NULL);

  thisObj->rootElement = rootElement;
  thisObj->currentElement = NULL;
  thisObj->justOwnChildren = justOwnChildren;

  return KBrsrSuccess;
}

NW_DOM_Node_t* NW_Wml_DeckIter_GetNextElement (NW_Wml_DeckIter_t* thisObj)
{
  if (thisObj->justOwnChildren == NW_TRUE) {
    return x_GetNextElement_JustSiblings (thisObj);
  }
  else {
    return x_GetNextElement_AllChildren (thisObj);
  }
}
