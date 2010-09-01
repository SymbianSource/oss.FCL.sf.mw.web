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
* Description:  Applies CSS for the box tree
*
*/



#ifndef CCSSHANDLER_H
#define CCSSHANDLER_H

//  INCLUDES
#include <e32base.h>
#include "nw_css_processor.h"
#include "nw_lmgr_boxvisitor.h"


// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  This class applies the CSS styles to the box tree. 
*  It breaks each box ApplyStyle into separate RunL
*
*  @lib css.lib
*  @since 2.1
*/
class CCSSHandler : public CActive
  {
   public:  // Constructors and destructor
        
     /**
      * Two-phased constructor.
      * @param aProcessor A pointer to processor object that owns this object
      * @return CCSSHandler*
	    */
      static CCSSHandler* NewL(NW_CSS_Processor_t* aProcessor);

      /**
      * Destructor.
      */
      virtual ~CCSSHandler();

        
    public: // New functions
      
      /**
      * This is called from the processor when a new stylesheet needs to be applied
      * @since 2.1
      * @return void
      */
      void ApplyStylesL();

    public: // Functions from base classes

      /**
      * From CActive 
      * @since 2.1
      * @param 
      * @return void
      */
      void RunL();
       
    private:

       /**
      * From CActive Noop. 
      * @since 2.1
      * @param 
      * @return void
      */
      void DoCancel();

     /**
      * C++ default constructor.
      * @param aProcessor A pointer to processor object that owns this object
      * @return CCSSHandler*
      */
      CCSSHandler(NW_CSS_Processor_t* aProcessor);

      /**
      * By default Symbian 2nd phase constructor is private.
      */
      void ConstructL();

      /**
      * This method applies styles to each box in the box tree and stores it as backup.
      * It is called from the RunL method.
      * @since 2.1
      * @return ETrue if styles have been applied to all the boxes, otherwise EFalse
      */
      TBool ApplyBoxStylesL();

      /**
      * After the styles have been applied, this method replace the property list of all boxes 
      * with the box backup property list, reformats the box tree and redraws it. Called from RunL
      * @since 2.1
      * @return void
      */
      void ApplyStylesCompleteL();

      /**
      * This method swaps the property list of all boxes with the box backup property list
      * @since 2.1
      * @return void
      */
      void SwapPropListL();

      /**
      * Gets the next box (using box visitor) which has the element node set up
      * @since 2.1
      * @param (OUT) element node which has been set on the box
      * @return pointer to the next box
      */
      NW_LMgr_Box_t* GetNextBox(NW_DOM_ElementNode_t** node);

    private:    // Data

      // box visitor to visit each box of the box tree to which styles are applied
      NW_LMgr_BoxVisitor_t iBoxVisitor;
      // processor whose styles need to be applied
      NW_CSS_Processor_t* iProcessor;
    };

#endif      // CCSSHandler
            
// End of CSS
