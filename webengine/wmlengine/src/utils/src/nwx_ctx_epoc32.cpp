/*
* Copyright (c) 1999 Nokia Corporation and/or its subsidiary(-ies).
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
**   File: nwx_ctx.cpp
**   Purpose:  Provides the interface to a manager of context pointer for
			   multiple components.
			   On Epoc32 we use thread local storage to maintain the
			   context arrays.
**************************************************************************/


/*
**-------------------------------------------------------------------------
**  Include Files
**-------------------------------------------------------------------------
*/
#include <e32std.h>
#include <eikenv.h>

#include "nwx_ctx_epoc32.h"
#include "nwx_mem.h"
#include "nwx_defs_symbian.h"
#include "urlloader_urlloaderint.h"
#include "nw_object_exceptions.h"
#include "BrsrStatusCodes.h"


/*****************************************************************

  Name: NW_Ctx_GetBrowserIndex()

  Description:  Get the index of the current browser's context.
                A context is init'ed if one doesn't exist.

  Parameters:   The index.

  Return Value: KBrsrSuccess or KBrsrOutOfMemory

******************************************************************/
static TBrowserStatusCode NW_Ctx_GetBrowserIndex(NW_ContextArray_t** aContextArray,
        NW_Uint16* aIndex)
{
    NW_ContextArray_t*  contextArray = NULL;
    void*               appInst = NULL;
    NW_Int16            contextIndex;
    NW_Uint16           i;

    //lint -e{429} Custodial pointer has not been freed or returned

    NW_ASSERT(aContextArray != NULL);
    NW_ASSERT(aIndex != NULL);

    NW_TRY(status) 
        {
        // If need be, initialize the context manager.
        contextArray = (NW_ContextArray_t*) Dll::Tls();
  
        if (contextArray == NULL) 
            {
            contextArray = (NW_ContextArray_t*) malloc(sizeof(NW_ContextArray_t));
            NW_THROW_OOM_ON_NULL(contextArray, status);

            NW_Mem_memset(contextArray, 0x00, sizeof(NW_ContextArray_t));

            // Store the array in TLS.
            Dll::SetTls(contextArray);
            }

        // Get the current instance of browser.
        // TODO: The idea of using the App UI pointer to identify the browser
        //       context isn't valid.  For example, consider when the browser is used by the
        //       the media player to download a image.  In this case the context is created 
        //       when the browser UI is active, but after a image is downloaded and passed to
        //       the media-player the player is the active UI.  While the media-player is still the 
        //       front-most application the browser then "resumes" the document.  This doesn't 
        //       work of course because it fetches the App UI pointer of the media-player 
        //       instead of the browser therefore it can't access its singleton data from the
        //       context and fails.

        // appInst = STATIC_CAST( void*, CEikonEnv::Static()->EikAppUi());
        appInst = (void *) 1L;

        // Search the uniqueNum for the inst.
        contextIndex = -1;
        for (i = 0; i < NW_NUM_INSTANCES; i++)
            {
            if (contextArray->uniqueNum[i] == appInst)
                {
                contextIndex = i;
                break;
                }
            }

        // If not found then create a new context.
        if (contextIndex == -1)
            {
            // Find an empty slot.
            for (i = 0; i < NW_NUM_INSTANCES; i++)
                {
                if (contextArray->uniqueNum[i] == NULL)
                    {
                    contextIndex = i;
                    break;
                    }
                }

            // Throw if it is out of slots.
            if (contextIndex == -1)
{
                NW_THROW_UNEXPECTED(status);
}

            // Init the slot.
            contextArray->uniqueNum[contextIndex] = appInst;

            for (i = 0; i < NW_NUM_CONTEXTS; i++) 
{
                contextArray->contexts[contextIndex][i] = NULL;
                }
            }

        // Return the values.
        *aIndex = contextIndex;
        *aContextArray = contextArray;
        }

    NW_CATCH(status) 
  {
  }

    NW_FINALLY 
  {
        return status;
        } NW_END_TRY
}

/*****************************************************************

  Name: NW_Ctx_DeleteBrowserContext()

  Description:  Delete the current browser's context.

  Parameters:   

  Return Value: KBrsrSuccess or some other error

******************************************************************/
TBrowserStatusCode NW_Ctx_DeleteBrowserContext()
{
    TBrowserStatusCode  status;
    NW_Uint16           index;
  NW_Uint16 i;
    NW_ContextArray_t*  contextArray = NULL;
    NW_Bool             okToDelete;

    // Get the context index
    status = NW_Ctx_GetBrowserIndex(&contextArray, &index);
    if (status != KBrsrSuccess)
        {
        return status;
        }

    // Clear the slot.
    contextArray->uniqueNum[index] = NULL;

    for (i = 0; i < NW_NUM_CONTEXTS; i++) 
        {
        contextArray->contexts[index][i] = NULL;
    }

    // Delete the context array if this was the last instance.
    okToDelete = NW_TRUE;
    for (i = 0; i < NW_NUM_INSTANCES; i++)
    {
        if (contextArray->uniqueNum[i] != NULL)
         {
            okToDelete = NW_FALSE;
             break;
         }
    }   

    if (okToDelete)
        {
        free(contextArray);
        Dll::SetTls(NULL);

        // Release of all STDLIB resources belonging to this thread.
        CloseSTDLIB();
    }

  return KBrsrSuccess;
}
  
/*****************************************************************

  Name: NW_Ctx_Set()

  Description:  Set the context for the specified component

  Parameters:   component - which context
			          instance - which instance (NOT USED YET)
				        *ctx - pointer to the context to store

  Return Value: KBrsrSuccess or KBrsrFailure

******************************************************************/
TBrowserStatusCode NW_Ctx_Set(const NW_CtxComponent_t aComponent, 
        const NW_Uint16 aInstance, void *aContext)
{
    TBrowserStatusCode  status;
  NW_ContextArray_t* contextArray;
    NW_Uint16           index;

    NW_REQUIRED_PARAM(aInstance);

    status = NW_Ctx_GetBrowserIndex(&contextArray, &index);
    if (status == KBrsrSuccess)
      {
        contextArray->contexts[index][aComponent] = aContext;
  }

  return status;
}


/*****************************************************************

  Name: NW_Ctx_Get()

  Description:  Get the context for the specified component

  Parameters:   component - which context
			          instance - which instance (NOT USED YET)

  Return Value: pointer to the component's context or NULL

******************************************************************/
void *NW_Ctx_Get(const NW_CtxComponent_t aComponent,
        const NW_Uint16 aInstance)
{
  TBrowserStatusCode status;
  NW_ContextArray_t* contextArray;
    NW_Uint16           index;

    NW_REQUIRED_PARAM(aInstance);

    status = NW_Ctx_GetBrowserIndex(&contextArray, &index);
    if (status == KBrsrSuccess)
         {
        return contextArray->contexts[index][aComponent];
  }

  return NULL;
}
