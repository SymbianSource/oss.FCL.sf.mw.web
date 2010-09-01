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


#include "wml_deck.h"
#include "nw_wml1x_wml_1_3_tokens.h"
#include "nw_wml1x_wml1xentityset.h"
#include "nw_hed_domhelper.h"
#include "wml_decoder.h"

#include "wml_elm_attr.h"
#include "BrsrStatusCodes.h"


/* ------------------------------------------------------------------------- */
struct NW_Wml_Deck_s {
  NW_HED_DomTree_t* domTree;
  NW_HED_DomHelper_t domHelper;
  NW_DOM_Node_t* currentCard;
};

/* ------------------------------------------------------------------------- */
NW_Wml_Deck_t* NW_Wml_Deck_New (NW_Buffer_t* domBuffer,
                                NW_Http_CharSet_t charset,
                                NW_Bool contentWasPlainText)
{
  NW_Wml_Deck_t* deck = NULL;

  NW_ASSERT (domBuffer != NULL);

  /* create the deck */
  NW_THROWIF_NULL (deck = (NW_Wml_Deck_t*) NW_Mem_Malloc (sizeof (NW_Wml_Deck_t)));
  NW_THROWIF_NULL (deck->domTree = NW_HED_DomTree_New (domBuffer, charset, NW_Wml_1_3_PublicId,NULL));
  deck->currentCard = NULL;

  deck->domHelper.entitySet = NW_HED_EntitySetOf (&NW_Wml1x_EntitySet);
  deck->domHelper.context = NULL;
  deck->domHelper.resolvePlainTextVariablesAndEntities = contentWasPlainText;

  /* print the domTree */
#ifdef _DEBUG
  NW_HED_DomHelper_PrintTree (&deck->domHelper, NW_HED_DomTree_GetRootNode (deck->domTree));
#endif

  return deck;

NW_CATCH_ERROR
  NW_Wml_Deck_Delete (deck);

  return NULL;
}

/* ------------------------------------------------------------------------- */
void NW_Wml_Deck_Delete (NW_Wml_Deck_t* thisObj)
{
  if (thisObj != NULL) {
    NW_HED_DomTree_Delete (thisObj->domTree);
    
    NW_Mem_Free (thisObj);
  }
}

/* ------------------------------------------------------------------------- */
NW_HED_DomTree_t* NW_Wml_Deck_GetDomTree (const NW_Wml_Deck_t* thisObj)
{
  NW_ASSERT (thisObj != NULL);
  
  return thisObj->domTree;
}

/* ------------------------------------------------------------------------- */
NW_HED_DomHelper_t* NW_Wml_Deck_GetDomHelper (NW_Wml_Deck_t* thisObj,
                                              const NW_Wml_VarList_t* varList)
{
  NW_HED_Context_t* context = NULL;

  NW_ASSERT (thisObj != NULL);
  
  /* delete the old context because it is stale */
  NW_Object_Delete (((NW_Wml_Deck_t *)thisObj)->domHelper.context);

  /* convert the varList into a context */
  if (varList != NULL) {
    context = NW_DeckDecoder_VarListToContext(varList);
  }

  /* set the domHelper's context to the new context */
  ((NW_Wml_Deck_t *)thisObj)->domHelper.context = context;

  return &(((NW_Wml_Deck_t *)thisObj)->domHelper);
}

/* ------------------------------------------------------------------------- */
NW_DOM_Node_t* NW_Wml_Deck_GetCurrentCard (const NW_Wml_Deck_t* thisObj)
{
  NW_ASSERT (thisObj != NULL);
  
  return thisObj->currentCard;
}

/* ------------------------------------------------------------------------- */
NW_DOM_Node_t* NW_Wml_Deck_GetCardByName (NW_Wml_Deck_t* thisObj, 
                                          const NW_String_t* cardName)
{
  NW_ASSERT (thisObj != NULL);
  NW_ASSERT (cardName != NULL);

  return (NW_HED_DomHelper_FindElement (NW_Wml_Deck_GetDomHelper (thisObj, NULL), 
        NW_HED_DomTree_GetRootNode (thisObj->domTree), 2, CARD_ELEMENT, ID_ATTR, cardName));
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode NW_Wml_Deck_SetCurrentCardByName (NW_Wml_Deck_t* thisObj, 
                                              const NW_String_t* cardName)
{
  NW_DOM_Node_t* cardElem = NULL;
  TBrowserStatusCode status = KBrsrWmlbrowserCardNotInDeck;

  /* if cardName is NULL, use the first card */
  if (cardName->storage == NULL) {
    cardElem = NW_HED_DomHelper_FindElement (NW_Wml_Deck_GetDomHelper (thisObj, NULL), 
        NW_HED_DomTree_GetRootNode (thisObj->domTree), 2, CARD_ELEMENT, 0, NULL);
  }

  /* otherwise find a card with the given name */
  else {
    cardElem = NW_HED_DomHelper_FindElement (NW_Wml_Deck_GetDomHelper (thisObj, NULL), 
        NW_HED_DomTree_GetRootNode (thisObj->domTree), 2, CARD_ELEMENT, ID_ATTR, cardName);
  }

  /* set the card */
  if (cardElem != NULL) {
    NW_Wml_Deck_SetCurrentCardByElement (thisObj, cardElem);
    
    status = KBrsrSuccess;
  }

  return status;
}

/* ------------------------------------------------------------------------- */
void NW_Wml_Deck_SetCurrentCardByElement (NW_Wml_Deck_t* thisObj, 
                                          const NW_DOM_Node_t* element)
{
  NW_ASSERT (thisObj != NULL);

  thisObj->currentCard = (NW_DOM_Node_t*) element;
}

