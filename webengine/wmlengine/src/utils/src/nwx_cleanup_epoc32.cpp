/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides the interface to a manager of context pointer for
			           multiple components.
*
*/


/*
**-------------------------------------------------------------------------
**  Include Files
**-------------------------------------------------------------------------
*/
#include <e32std.h>
#include <e32svr.h>

#include "nwx_defs.h"
#include "nwx_cleanup_epoc32.h"
#include "nwx_ctx.h"
#include "nw_wae.h"
#include "nwx_psq.h"
#include "nwx_msg_dsp_api_epoc32.h"
#include "nwx_math.h"
#include "CBrowserSettings.h"
#include "nwx_ctx_epoc32.h"
#include "nwx_osu_epoc32.h"
#include "nwx_logger.h"
#include "BrsrStatusCodes.h"

/*
**-------------------------------------------------------------------------
**  External Public (Exported) Functions
**-------------------------------------------------------------------------
*/



/*****************************************************************
  Name: NW_Cleanup()
  Description:  Deletes the rainbow data elements.
  Parameters:   
  Return Value: KBrsrSuccess
******************************************************************/
TBrowserStatusCode NW_Cleanup()
{
  void*  ctx;

  /* Epoc Shutdown */
  NW_Msg_Shutdown();
  NW_Msg_DispatcherShutdown();

  Math_ContextFree();

  ctx = NW_Ctx_Get(NW_CTX_SCRIPT, 0);
  NW_Mem_Free(ctx);
  NW_Ctx_Set(NW_CTX_SCRIPT, 0, NULL);

  ctx = NW_Ctx_Get(NW_CTX_SCR_FUNC_TAB, 0);
  NW_Mem_Free(ctx);
  NW_Ctx_Set(NW_CTX_SCR_FUNC_TAB, 0, NULL);

  ctx = NW_Ctx_Get(NW_CTX_SCR_SERVER, 0);
  NW_Mem_Free(ctx);
  NW_Ctx_Set(NW_CTX_SCR_SERVER, 0, NULL);

  ctx = NW_Ctx_Get(NW_CTX_MEM_SEG, 0);
  NW_Mem_Free(ctx);
  NW_Ctx_Set(NW_CTX_MEM_SEG, 0, NULL);

  CBrowserSettings* settings = (CBrowserSettings*) NW_Ctx_Get(NW_CTX_SETTINGS, 0);
  delete settings;
  NW_Ctx_Set(NW_CTX_SETTINGS, 0, NULL);
  
  NW_Osu_DeleteContext();

#ifdef FEATURE_MEMORY_PROFILE
  SendStringToLogger1((char *) NULL);
#endif
  NW_LOG_CLOSELOGGER();

  NW_Ctx_DeleteBrowserContext();

  return KBrsrSuccess;
}

