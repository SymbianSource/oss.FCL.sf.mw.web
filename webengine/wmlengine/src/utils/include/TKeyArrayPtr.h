/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Key class for use with pointer-based Symbian array classes.
*
*/


#ifndef TKEYARRAYPTR_H
#define TKEYARRAYPTR_H

//  INCLUDES
#include <e32base.h>
#include "nwx_string.h"

// CLASS DECLARATION

/**
*   Numeric key class for use with pointer-based Symbian array classes.
*   See the following link from Symbian:
*     http://www3.symbian.com/faq.nsf/f6fbf5deaf4cca7080256a64005ac61e/defdb21d5e976aee80256a570051b8d9?OpenDocument&Highlight=2,TKeyArrayFix
*   @lib lmgr.lib
*   @since 2.1
*/
class TNumKeyArrayPtr : public TKeyArrayFix
    {
    public:  // Constructors and destructor

        /**
        * Constructor.
        */
        inline TNumKeyArrayPtr(TInt aOffset, TKeyCmpNumeric aType) : TKeyArrayFix(aOffset, aType) {}


    public: // NEW FUNCTIONS

        /**
        * Gets pointer to array entry at specified index.
        * @since 2.1
        * @param aIndex Index within the array of desired entry.
        * @return Pointer to entry.
        */
        virtual TAny* At(TInt aIndex) const
            {
            if (aIndex == KIndexPtr)
                {
                    return *(TUint8**)iPtr + iKeyOffset;
                }
            return *(TUint8**)iBase->Ptr(aIndex * sizeof(TUint8**)).Ptr() + iKeyOffset;
            }
    };

/**
*   Textual (string) key class for use with pointer-based Symbian array classes.
*   See the following link from Symbian:
*     http://www3.symbian.com/faq.nsf/f6fbf5deaf4cca7080256a64005ac61e/defdb21d5e976aee80256a570051b8d9?OpenDocument&Highlight=2,TKeyArrayFix
*   @lib lmgr.lib
*   @since 2.1
*/
class TTextKeyArrayPtr : public TKeyArrayFix
    {
    public:  // Constructors and destructor

        /**
        * Constructor.
        */
        inline TTextKeyArrayPtr(TInt aOffset, TKeyCmpText aType) : TKeyArrayFix(aOffset, aType) {}


    public: // NEW FUNCTIONS

        /**
        * Gets pointer to array entry at specified index.
        * @since 2.1
        * @param aIndex Index within the array of desired entry.
        * @return Pointer to entry.
        */
        virtual TAny* At(TInt aIndex) const
            {
            if (aIndex == KIndexPtr)
                {
                    return *(TUint8**)iPtr + iKeyOffset;
                }
            return *(TUint8**)iBase->Ptr(aIndex * sizeof(TUint8**)).Ptr() + iKeyOffset;
            }

        virtual TInt Compare(TInt aLeft, TInt aRight) const
            {
            NW_Ucs2* l=(*(NW_Ucs2**)At(aLeft));
            NW_Ucs2* r=(*(NW_Ucs2**)At(aRight));
            return( NW_Str_Strcmp( l, r ) );
            }

    };
#endif // TKEYARRAYPTR_H   
            
// End of File
