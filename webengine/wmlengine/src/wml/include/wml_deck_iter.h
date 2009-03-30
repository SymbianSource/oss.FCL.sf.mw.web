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


#ifndef WML_DECK_ITER_H
#define WML_DECK_ITER_H

#include "wml_deck.h"

#include "nw_hed_domhelper.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   public data types
 * ------------------------------------------------------------------------- */

typedef struct NW_Wml_DeckIter_s {
  NW_DOM_Node_t* rootElement;
  NW_DOM_Node_t* currentElement;
  NW_Bool justOwnChildren;
} NW_Wml_DeckIter_t;


/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */

TBrowserStatusCode NW_Wml_DeckIter_Initialize (NW_Wml_DeckIter_t* thisObj,
                                        NW_DOM_Node_t* rootElement,
                                        NW_Bool justOwnChildren);


NW_DOM_Node_t* NW_Wml_DeckIter_GetNextElement (NW_Wml_DeckIter_t* thisObj); 

#endif /* WML_DECK_ITER_H */
