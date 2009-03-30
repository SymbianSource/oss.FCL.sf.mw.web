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
* Description:  Contains mapping of string values to internal tokens
*
*/


#ifndef TCSSPROPERTYTABLE_H
#define TCSSPROPERTYTABLE_H

//  INCLUDES
#include <e32std.h>
#include "nw_lmgr_propertylist.h"

// CONSTANTS

// MACROS
struct TCSSPropertyTablePropEntry
{
  const TText16* strName;
  NW_LMgr_PropertyName_t tokenName;
  TUint32 type;
};

struct TCSSPropertyTablePropValEntry
{
  const TText16 *strVal;
  NW_LMgr_PropertyValueToken_t tokenVal;
};

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  This class Contains mapping of string values to internal tokens
*
*  @lib css.lib
*  @since 2.1
*/
class TCSSPropertyTable
  {
   public:

    static const TCSSPropertyTablePropEntry* GetPropertyEntry(const TText16* aStrName);

    static TBool GetPropertyValToken(const TText16* aVal, NW_LMgr_PropertyValueToken_t *aToken);

    static const TCSSPropertyTablePropEntry* GetPropEntryByToken (NW_LMgr_PropertyName_t aTokenName);

    static const TCSSPropertyTablePropValEntry* GetPropValEntryByToken (NW_LMgr_PropertyValueToken_t aTokenVal);

    static const TText16* GetPropValType(TUint8 aValueType);

   private:


};

#endif /* TCSSPROPERTYTABLE_H */
