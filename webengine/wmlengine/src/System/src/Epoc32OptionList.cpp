/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32def.h> // avoid NULL redefinition warning
#include "nw_system_optionlist.h"
#include "MVCShell.h"
#include "nwx_ctx.h"

//lint -save -esym(528, PtrCFromTText) Symbol not referenced

/* ------------------------------------------------------------------------- */
void
NW_System_OptionList_Initialize (void)
{
	CShell* browserApp = REINTERPRET_CAST(CShell*, (NW_Ctx_Get(NW_CTX_BROWSER_APP, 0)));
	browserApp->OptionListInit();
}

/* ------------------------------------------------------------------------- */
void
NW_System_OptionList_AddItem (NW_Ucs2* string,
                              NW_Uint32 elementID,
                              NW_Int16 aElType)
{
	CShell* browserApp = REINTERPRET_CAST(CShell*, (NW_Ctx_Get(NW_CTX_BROWSER_APP, 0)));
  browserApp->OptionListAddItem( string, elementID, aElType );
}

//lint -restore
