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
* Description:  Asynchronous initialization of the browser engine
*
*/



#ifndef VIEWASYNCINIT_H
#define VIEWASYNCINIT_H

//  INCLUDES
#include <e32std.h>
#include <e32def.h>
#include <e32base.h>
#include "MVCView.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CEpoc32View;

// CLASS DECLARATION

/**
*  Asynchronous initialization of the browser engine
*
*  @lib browserengine.dll
*  @since 2.6
*/
NONSHARABLE_CLASS(CViewAsyncInit) : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CViewAsyncInit* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CViewAsyncInit();

    public: // New functions

    public: // Functions from base classes
        /**
        * From CActive 
        * @since 2.6
        * @param 
        * @return void
        */
        void DoCancel();

        /**
        * From CActive Asynchronous initialization
        * @since 2.6
        * @param 
        * @return void
        */
        void RunL();

        /**
        * From CActive Handle Leave from RunL.
        * @since 2.6
        * @param aError The Leave error code.
        * @return void
        */
        TInt RunError(TInt aError);

    private:

        /**
        * C++ default constructor.
        */
        CViewAsyncInit();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
    
    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        // A pointer to the view, for the init callback
        CView* iView;
         
        // Reserved pointer for future extension
        //TAny* iReserved;

    };

#endif      // VIEWASYNCINIT_H
            
// End of File
