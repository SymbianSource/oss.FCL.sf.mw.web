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


/***************************************************************************
**   File: nwx_ctx.h
**   Purpose:   Provides the interface to a component's global/static context
**              for target platforms which can not have non-const global
**              or static data in dynamically loaded libraries.
**************************************************************************/
#ifndef NWX_CTX_H
#define NWX_CTX_H

/*
** Includes
*/
#include "nwx_defs.h"
#include "BrsrStatusCodes.h"


#ifdef __cplusplus
extern "C" {
#endif

/*
**  Component Definitions
*/

typedef enum {
  NW_CTX_URL_LOADER             = 0,
  NW_CTX_MEM_SEG                = 1,
  NW_CTX_MSG_API                = 2,
  NW_CTX_MSG_DSP                = 3,
  NW_CTX_OSU                    = 4,
  NW_CTX_WML_CORE               = 5,
  NW_CTX_URL                    = 6,
  NW_CTX_SETTINGS               = 7,
  NW_CTX_SCRIPT                 = 8,
  NW_CTX_SCR_SERVER             = 9,
  NW_CTX_SCR_FUNC_TAB           = 10,
  NW_CTX_LOGGER                 = 11,
  NW_CTX_UTILS_MATH             = 12,
  NW_CTX_MEM_PRO_LAST_PTR       = 13,
  NW_CTX_MEM_PRO_SEQ_NO         = 14,
  NW_CTX_BROWSER_APP            = 15,
  NW_CTX_HED_LOAD_QUEUE         = 16,
  NW_CTX_PLUGIN_LIST            = 17,
  NW_CTX_USER_AGENT             = 18,
  NW_CTX_VARIABLE_CSS           = 19,
  NW_CTX_CONT_ACCESS_MGR        = 20,

#ifdef __TEST_TESTHARNESS_ENABLED
  NW_CTX_TESTHARNESS            = NW_CTX_VARIABLE_CSS + 1,
#endif

  NW_CTX_CONTEXT_COUNT

} NW_CtxComponent_t;

// currently 37 (optionally 38)
#define NW_NUM_CONTEXTS NW_CTX_CONTEXT_COUNT 

/* Set a context
   Returns KBrsrSuccess or KBrsrOutOfMemory */
TBrowserStatusCode NW_Ctx_Set(const NW_CtxComponent_t aComponent, 
        const NW_Uint16 aInstance, void *aContext);

/* Get a context */
void *NW_Ctx_Get(const NW_CtxComponent_t aComponent, 
        const NW_Uint16 aInstance);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /* NWX_CTX_H */
