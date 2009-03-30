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
* Description:  Handles table elements in XHTML documents.
*
*/



#ifndef CXHTMLTABLEELEMENTHANDLER__H
#define CXHTMLTABLEELEMENTHANDLER__H

// INCLUDES
#include <e32def.h>
#include <e32base.h>

#include "nwx_defs.h"
#include "nw_dom_node.h"


// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct NW_XHTML_ContentHandler_s NW_XHTML_ContentHandler_t;
typedef struct NW_XHTML_ElementHandler_s NW_XHTML_ElementHandler_t;
typedef struct NW_LMgr_Box_s NW_LMgr_Box_t;
typedef struct NW_LMgr_RootBox_s NW_LMgr_RootBox_t;
typedef struct NW_LMgr_ContainerBox_s NW_LMgr_ContainerBox_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

// CLASS DECLARATION

// TODO: This class should be derived from CElementHandler once the browser is
//       converted to c++.

class CXHTMLTableElementHandler : public CBase
  {
  public:  // Constructors and destructor
    /**
    * Two-phased constructor.
    */
    static CXHTMLTableElementHandler* NewL(const NW_XHTML_ElementHandler_t* aElementHandler,
        NW_XHTML_ContentHandler_t* aContentHandler, NW_DOM_ElementNode_t* aElementNode);

    /**
    * Destructor.
    */
    virtual ~CXHTMLTableElementHandler();


  public:  // New Methods
    /**
    * Initializes the ElementHandler.
    *
    * @return status code
    */
    virtual TBrowserStatusCode Initialize(void);

    /**
    * Creates the box-tree associated with the tag.
    *
    * @param aParentBox the parent to attach the new box-tree to.
    * @return status code
    */
    virtual TBrowserStatusCode CreateBoxTree(NW_LMgr_ContainerBox_t& aParentBox);  

    /**
    * Returns whether or not grid mode applies to this table.
    *
    * @return ETrue or EFalse.
    */
    TBool DoesGridModeApply(void);


  private:  // Private Methods
    /**
    * C++ default constructor.
    */
    CXHTMLTableElementHandler(const NW_XHTML_ElementHandler_t* aElementHandler,
        NW_XHTML_ContentHandler_t* aContentHandler, NW_DOM_ElementNode_t* aElementNode);

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL(void);

    /**
    * A simple helper method to create the box-tree.
    *
    * @param aElementNode the table's element node.
    * @param aTableBox the table's box.
    * @param aTableContainerParent the table's parent's box.
    *
    * @return status code
    */
    TBrowserStatusCode CreateSubTree(NW_DOM_ElementNode_t* aElementNode,
        NW_LMgr_Box_t* aTableBox, NW_LMgr_ContainerBox_t* aTableContainerParent);

    /**
    * A simple helper method to create the box-tree.
    *
    * @param aElementHandler the table's element handler.
    * @param aElementNode the table's element node.
    * @param aTableBox the table's box.
    * @param aTableContainerParent the table's parent's box.
    *
    * @return status code
    */
    TBrowserStatusCode CreateSubTreeHelper(const NW_XHTML_ElementHandler_t* aElementHandler, 
        NW_DOM_ElementNode_t* aElementNode, NW_LMgr_Box_t* aTableBox, 
        NW_LMgr_ContainerBox_t* aTableContainerParent);

    /**
    * Returns true if the parent has only one element, ignoring text nodes that
    * only contain whitespace.
    *
    * @return true or false.
    */
    NW_Bool HasOnlyOneElementChild(NW_DOM_Node_t* aParentNode);

    /**
    * This method is called each time a table chunk arrives.  It is used
    * to determine whether or not that table should be laid out in grid mode.
    *
    * @return status code
    */
    TBrowserStatusCode PreEvalTable();

    /**
    * This method is called after all of the chunks of a table arrive.  It is used
    * to determine whether or not that table should be laid out in grid mode.
    *
    * @return status code
    */
    TBrowserStatusCode PostEvalTable();


  private:  // Data
    const NW_XHTML_ElementHandler_t*  iElementHandler;
    NW_XHTML_ContentHandler_t*        iContentHandler;
    NW_DOM_ElementNode_t*             iElementNode;
    NW_LMgr_RootBox_t*                iRootBox;
    NW_Float32                        iScreenWidth;

    TInt8                             iIsGridMode;
    NW_DOM_Node_t*                    iLastEvaluatedElement;
    TInt32                            iNumberOfElements;
  };

#endif  // CXHTMLTABLEELEMENTHANDLER__H
