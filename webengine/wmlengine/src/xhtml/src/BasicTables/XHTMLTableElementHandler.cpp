/*
* Copyright (c) 2000 - 2001 Nokia Corporation and/or its subsidiary(-ies).
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


#include "XhtmlTableElementHandler.h"

#include "BrsrStatusCodes.h"
#include "GDIDeviceContext.h"
#include "HEDDocumentListener.h"
#include "LMgrAnonTableBox.h"

#include "nw_xhtml_xhtmlcontenthandler.h"
#include "nw_lmgr_rootbox.h"
#include "nw_lmgr_statictablebox.h"
#include "nw_lmgr_verticaltablebox.h"
#include "nw_basictables_trelementhandler.h"
#include "nw_basictables_captionelementhandler.h"
#include "nw_basictables_rowgroupelementhandler.h"
#include "nw_basicforms_formelementhandler.h"
#include "nw_basicforms_xhtmlformliaison.h"
#include "nw_basicforms_inputelementhandler.h"
//#include "nw_structure_scriptelementhandler.h"
#include "nw_object_exceptions.h"
#include "nw_dom_domvisitor.h"
#include "nwx_settings.h"
#include "nwx_statuscodeconvert.h"


#define GRID_MODE_MAX_NUMBER_OF_ELEMEMTS 500
#define GRID_MODE_MAX_TEXT_BLOCK 100
#define GRID_MODE_COL_RATIO 0.025
#define GRID_MODE_IMAGE_SCALE_RATIO 3


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXHTMLTableElementHandler::CXHTMLTableElementHandler
//
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXHTMLTableElementHandler::CXHTMLTableElementHandler(const NW_XHTML_ElementHandler_t* aElementHandler,
    NW_XHTML_ContentHandler_t* aContentHandler, NW_DOM_ElementNode_t* aElementNode)        
  {
  iElementHandler = aElementHandler;
  iContentHandler = aContentHandler;
  iElementNode = aElementNode;

  iIsGridMode = ETrue;
  }

// -----------------------------------------------------------------------------
// CXHTMLTableElementHandler::ConstructL
//
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXHTMLTableElementHandler::ConstructL(void)
  {
    NW_HED_DocumentRoot_t*  docRoot = NULL;
    CGDIDeviceContext*      deviceContext = NULL;

    // Get the doc-root, root-box and device context.
    docRoot = (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode(iContentHandler);
    NW_ASSERT(docRoot != NULL);

    iRootBox = NW_HED_DocumentRoot_GetRootBox(docRoot);
    NW_ASSERT(iRootBox != NULL);

    deviceContext = NW_LMgr_RootBox_GetDeviceContext(iRootBox);

    // Get the screen width from the device context.
    iScreenWidth = (NW_Float32) deviceContext->DisplayBounds()->dimension.width;
  }

// -----------------------------------------------------------------------------
// CXHTMLTableElementHandler::~CXHTMLTableElementHandler
//
// Deconstructor.
// -----------------------------------------------------------------------------
//
CXHTMLTableElementHandler::~CXHTMLTableElementHandler()
  {
  }

// -----------------------------------------------------------------------------
// CXHTMLTableElementHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXHTMLTableElementHandler* CXHTMLTableElementHandler::NewL(const NW_XHTML_ElementHandler_t* 
    aElementHandler, NW_XHTML_ContentHandler_t* aContentHandler, 
    NW_DOM_ElementNode_t* aElementNode)
  {
  NW_ASSERT(aElementHandler != NULL);
  NW_ASSERT(aContentHandler != NULL);
  NW_ASSERT(aElementNode != NULL);

  CXHTMLTableElementHandler* self = new(ELeave) CXHTMLTableElementHandler(aElementHandler,
      aContentHandler, aElementNode);

  CleanupStack::PushL(self);
  self->ConstructL();
  CleanupStack::Pop();
  
  return self;
  }

// -----------------------------------------------------------------------------
// CXHTMLTableElementHandler::Initialize
//
// Initializes the ElementHandler.
// -----------------------------------------------------------------------------
//
TBrowserStatusCode CXHTMLTableElementHandler::Initialize(void)
  {
  NW_TRY (status) 
    {
    // If the table has already been determined not to be a grid mode
    // table then just exit the method successfully.
    if (!iIsGridMode)
      {
      NW_THROW_SUCCESS(status);
      }

    // Evalulate the table to determine whether it's suitable for grid mode layout.
    // This does the pre-validation of the table (as the chunks arrive).
    status = PreEvalTable();
    _NW_THROW_ON_ERROR(status);

    // Stop initialzation if the entire table isn't available.
    if (!NW_XHTML_ElementHandler_NextSiblingExists (iContentHandler, iElementNode, NW_TRUE))
      {
      NW_THROW_SUCCESS (status);
      }

    // The entire table has arrived so clear iLastEvaluatedElement as it's not needed 
    // any more.
    iLastEvaluatedElement = NULL;

    // Evalulate the table to determine whether it's suitable for grid mode layout.
    // This does the post-validation of the table (after the entire table is available).
    status = PostEvalTable();
    _NW_THROW_ON_ERROR(status);
    }

  NW_CATCH (status) 
    {
    // Suppress the status if the exception occurred because 
    // the table was disqualified for grid mode.
    if (!iIsGridMode)
      {
      status = KBrsrSuccess;
      }
    }

  NW_FINALLY 
    {
    return status;
    } NW_END_TRY
  }

// -----------------------------------------------------------------------------
// CXHTMLTableElementHandler::CreateBoxTree
//
// Creates the box-tree associated with the tag.
// -----------------------------------------------------------------------------
//
TBrowserStatusCode CXHTMLTableElementHandler::CreateBoxTree(NW_LMgr_ContainerBox_t& aParentBox)
  {
  NW_LMgr_Box_t*          tableBox = NULL;
  NW_LMgr_Box_t*          tableContainer = NULL;
  NW_Bool                 ownstableBox = NW_TRUE;
  NW_Bool                 ownsTableContainer = NW_TRUE;
  NW_ADT_Vector_Metric_t  tableContainerIndex = 0;

  NW_TRY (status) 
    {
    NW_LMgr_Property_t prop;

    // Stop creation if the entire table isn't available -- iIsGridMode is
    // also checked because if it is set to false it should always allow the
    // method to continue.
    if (iIsGridMode && !NW_XHTML_ElementHandler_NextSiblingExists(
        iContentHandler, iElementNode, NW_FALSE))
      {
      NW_THROW_SUCCESS (status);
      }

    // If need be set the grid mode flag on the root-box.
    if (iIsGridMode)
      {
      iRootBox->iHasGridModeTables = NW_TRUE;
      }

    // Allocate and initialize the anonymous container for the table and 
    // an optional caption
    tableContainer = (NW_LMgr_Box_t*) LMgrAnonTableBox_New(1);
    NW_THROW_OOM_ON_NULL (tableContainer, status);

    // Set the table display property 
    prop.type = NW_CSS_ValueType_Token;
    prop.value.token = NW_CSS_PropValue_display_block;
    status = NW_LMgr_Box_SetProperty(tableContainer, NW_CSS_Prop_display, &prop);
    _NW_THROW_ON_ERROR(status);

    status = NW_LMgr_ContainerBox_AddChild(&aParentBox, tableContainer);
    _NW_THROW_ON_ERROR(status);
    ownsTableContainer = NW_FALSE;
  
    // Allocate and initialize the box for the table 
    if ((DoesGridModeApply() == NW_TRUE) ||
        (NW_LMgr_RootBox_GetSmallScreenOn(iRootBox) == NW_FALSE))

      {
      tableBox = (NW_LMgr_Box_t*) NW_LMgr_StaticTableBox_New(1);
      }
    else
      {
      tableBox = (NW_LMgr_Box_t*) NW_LMgr_VerticalTableBox_New(1);
      }
    NW_THROW_OOM_ON_NULL(tableBox, status);

    // Add the boxes to the tree 
    status = NW_LMgr_ContainerBox_AddChild(NW_LMgr_ContainerBoxOf(tableContainer), tableBox);
    _NW_THROW_ON_ERROR(status);
    ownstableBox = NW_FALSE;

    tableContainerIndex = NW_LMgr_ContainerBox_GetChildCount(&aParentBox);

    // Create the subtree of boxes for elements within the table element 
    status = CreateSubTree(iElementNode, tableBox, &aParentBox);
    _NW_THROW_ON_ERROR(status);

    // If form and input controls are not within td, they need to be put in the beginning of the table
    //  For e.g 
    //  <TABLE>
    //    <caption> Table Test </caption>
    //    <TR><TD>Search</TD><TD>Nokia</TD></TR>
    //
    //    <TR>
    //    <FORM>
    //    <INPUT type=text value=001 name=wsid1> 
    //    <TD>Search <INPUT type=text value=001 name=wsid2> 
    //    </TD><TD>Nokia</TD>
    //    <input type="reset"/>
    //    </FORM>
    //    </TR>
    //  </table>
    //
    //  Here wsid1 should be put before table box, but while adding boxes we added it after the tableContainerBox.
    //  So, we see that more boxes were added after tableContainerBox, we move this box to the end.
      
    if (NW_LMgr_ContainerBox_GetChildCount(&aParentBox) > tableContainerIndex)
      {
      NW_LMgr_ContainerBox_InsertChildAt(&aParentBox, tableContainer, NW_ADT_Vector_AtEnd);
      }

    status = NW_XHTML_ElementHandler_ApplyStyles(iElementHandler, iContentHandler, 
        iElementNode, &tableBox, 0);
    _NW_THROW_ON_ERROR(status);

    // Apply Styles will take care of replacing container box if display was block, list-item or marquee.
    // In case of 'display:none' or an error it removes the container box. That's why we check box
    if (tableBox == NULL)
      {
      NW_THROW_SUCCESS(status);
      }
    } 

  NW_CATCH (status) 
    {
    }
     
  NW_FINALLY 
    {
    if (ownstableBox) 
      {
      NW_Object_Delete(tableBox);
      }

    if (ownsTableContainer) 
      {
      NW_Object_Delete(tableContainer);
      }
    return status;
    } NW_END_TRY
  }

// -----------------------------------------------------------------------------
// CXHTMLTableElementHandler::DoesGridModeApply
// 
// Returns whether or not grid mode applies to this table.
// -----------------------------------------------------------------------------
//
TBool CXHTMLTableElementHandler::DoesGridModeApply(void)
  {
  return iIsGridMode;
  }

// -----------------------------------------------------------------------------
// CXHTMLTableElementHandler::CreateSubTree
// 
// -----------------------------------------------------------------------------
//
TBrowserStatusCode 
CXHTMLTableElementHandler::CreateSubTree(NW_DOM_ElementNode_t* aElementNode,
    NW_LMgr_Box_t* aTableBox, NW_LMgr_ContainerBox_t* aTableContainerParent)
  {
  NW_DOM_ElementNode_t* childNode;

  NW_TRY (status) 
    {
    /* We visit the children ourselves, rather than calling the superclass.
     * This is necessary so we can filter any unwanted elements from
     * the table. A table can only handle <tr> elements. */
    for (childNode = NW_DOM_Node_getFirstChild(aElementNode); childNode != NULL;
        childNode = NW_DOM_Node_getNextSibling(childNode)) 
      {
      const NW_XHTML_ElementHandler_t* elementHandler;

      /* get the ElementHandler for the aElementNode */
      elementHandler = NW_XHTML_ContentHandler_GetElementHandler(iContentHandler,
          (NW_DOM_ElementNode_t*) childNode);

      /* we want to ignore the entire script tag */
      if (elementHandler == NULL)
        {
        continue;
        }

      status = CreateSubTreeHelper(elementHandler, childNode,aTableBox, 
          aTableContainerParent);
      _NW_THROW_ON_ERROR(status);
      }
    } 
    
  NW_CATCH (status) 
    {
    } 
  
  NW_FINALLY 
    {
    return status;
    } NW_END_TRY
  }


// -----------------------------------------------------------------------------
// CXHTMLTableElementHandler::CreateSubTreeHelper
// 
// -----------------------------------------------------------------------------
//
TBrowserStatusCode 
CXHTMLTableElementHandler::CreateSubTreeHelper(const NW_XHTML_ElementHandler_t* aElementHandler, 
    NW_DOM_ElementNode_t* aElementNode, NW_LMgr_Box_t* aTableBox, 
    NW_LMgr_ContainerBox_t* aTableContainerParent)
  {
  NW_TRY (status) 
    {
    /* for handling forms embedded in tables */
    if (NW_Object_IsInstanceOf(aElementHandler, &NW_XHTML_formElementHandler_Class))
      {
      NW_XHTML_FormLiaison_t* formLiaison = (NW_XHTML_FormLiaison_t*)
          NW_XHTML_ContentHandler_GetFormLiaison(iContentHandler, aElementNode);
      NW_REQUIRED_PARAM(formLiaison);

      status = CreateSubTree(aElementNode, aTableBox, aTableContainerParent);
      _NW_THROW_ON_ERROR(status);
      }

    else if (NW_Object_IsInstanceOf(aElementHandler, &NW_XHTML_inputElementHandler_Class))
      {
      status = NW_XHTML_ElementHandler_CreateBoxTree(aElementHandler, iContentHandler,
          aElementNode, aTableContainerParent);
      _NW_THROW_ON_ERROR(status);
      }

    /* If this is not a table row or a caption, skip to next */
    else if (NW_Object_IsInstanceOf(aElementHandler, &NW_XHTML_trElementHandler_Class) ||
        NW_Object_IsInstanceOf(aElementHandler, &NW_XHTML_rowgroupElementHandler_Class) )
      {
      status = NW_XHTML_ElementHandler_CreateBoxTree (aElementHandler, iContentHandler,
          aElementNode, NW_LMgr_ContainerBoxOf(aTableBox));
      _NW_THROW_ON_ERROR(status);
      }
       
    else if (NW_Object_IsInstanceOf(aElementHandler, &NW_XHTML_captionElementHandler_Class)) 
      {
      status = NW_XHTML_ElementHandler_CreateBoxTree (aElementHandler, iContentHandler,
          aElementNode, aTableBox->parent);
      _NW_THROW_ON_ERROR(status);
      }
    } 
  
  NW_CATCH (status) 
    {
    } 
    
  NW_FINALLY 
    {
    return status;
    } NW_END_TRY
  }

// -----------------------------------------------------------------------------
// CXHTMLTableElementHandler::HasOnlyOneElementChild
// 
// Returns true if the parent has only one element, ignoring text nodes that
// only contain whitespace.
// -----------------------------------------------------------------------------
//
NW_Bool
CXHTMLTableElementHandler::HasOnlyOneElementChild(NW_DOM_Node_t* aParentNode)
  {
  TBrowserStatusCode  status;
  NW_DOM_Node_t*      child;
  NW_Int32            childCount = 0;
  
  NW_ASSERT(aParentNode != NULL);

  // Return false if it has no children.
  if (NW_DOM_Node_hasChildNodes(aParentNode) == NW_FALSE)
    {
    return NW_FALSE;
    }

  for (child = NW_DOM_Node_getFirstChild(aParentNode);
      child != NULL; child = NW_DOM_Node_getNextSibling(child))
    {
    if (NW_DOM_Node_getNodeType(child) == NW_DOM_ELEMENT_NODE)
      {
      childCount++;
      }

    else if (NW_DOM_Node_getNodeType(child) == NW_DOM_TEXT_NODE)
      {
      NW_String_t string;

      status = StatusCodeConvert(NW_DOM_TextNode_getData(child, &string));

      if (status == KBrsrSuccess)
        {
        NW_Ucs2*   temp = NULL;

        temp = NW_Str_TrimInPlace((NW_Ucs2*) string.storage, NW_Str_Both);
        if (temp && (*temp != 0x00))
          {
          childCount++;
          }
          
        NW_String_deleteStorage(&string);
        }
      }

    if (childCount > 1)
      {
      return NW_FALSE;
      }
    }

  return NW_TRUE;
  }

// -----------------------------------------------------------------------------
// CXHTMLTableElementHandler::PreEvalTable
// 
// This method is called each time a table chunk arrives.  It is used
// to determine whether or not that table should be laid out in grid mode.
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
CXHTMLTableElementHandler::PreEvalTable()
  {
  const NW_Int32 validTokens[] = 
      {
      NW_XHTML_ElementToken_td,
      NW_XHTML_ElementToken_tr,
      NW_XHTML_ElementToken_a,
      NW_XHTML_ElementToken_img,
      NW_XHTML_ElementToken_input,
      NW_XHTML_ElementToken_th,
      NW_XHTML_ElementToken_form,
      NW_XHTML_ElementToken_label,
      NW_XHTML_ElementToken_button,
      NW_XHTML_ElementToken_caption,
      NW_XHTML_ElementToken_tbody,
      NW_XHTML_ElementToken_tfoot,
      NW_XHTML_ElementToken_thead,
      NW_XHTML_ElementToken_span,
      NW_XHTML_ElementToken_font,
      NW_XHTML_ElementToken_b,
      NW_XHTML_ElementToken_big,
      NW_XHTML_ElementToken_i,
      NW_XHTML_ElementToken_p,
      NW_XHTML_ElementToken_center,
      NW_XHTML_ElementToken_br,
      NW_XHTML_ElementToken_nobr,
      NW_XHTML_ElementToken_small
      };
  
  NW_cXML_DOM_DOMVisitor_t   visitor;
  NW_DOM_Node_t*             domNode = NULL;
  NW_Int32                   tokenCount = sizeof(validTokens) / sizeof(validTokens[0]);
  NW_Ucs2*                   widthStr = NULL;
  NW_Ucs2*                   typeStr = NULL;

  NW_TRY (status) 
    {
    // Reject if mobile profile mode is enabled.
    if (NW_Settings_GetDisableSmallScreenLayout())
      {
      NW_THROW_STATUS(status, KBrsrFailure);
      }

    // Init the dom visitor.
    status = NW_cXML_DOM_DOMVisitor_Initialize(&visitor, (NW_DOM_Node_t*) iElementNode);
    NW_THROW_ON_ERROR(status);
    
    // iLastEvaluatedElement is null during the first call to PreEvalTable on a given
    // table.  In this condition a call to "next" is made to skip the table element.
    if (iLastEvaluatedElement == NULL)
      {
      (void) NW_cXML_DOM_DOMVisitor_Next(&visitor, NW_DOM_ELEMENT_NODE | NW_DOM_TEXT_NODE);
      }

    // Otherwise the vistor's current node is set to the last node processed during the
    // previous call to PreEvalTable.
    else
      {
      visitor.currentNode = iLastEvaluatedElement;
      }

    // Validate the children.
    while ((domNode = NW_cXML_DOM_DOMVisitor_Next(&visitor, 
        NW_DOM_ELEMENT_NODE | NW_DOM_TEXT_NODE)) != NULL) 
      {
      NW_Bool    tokenValid = NW_FALSE;
      NW_Uint16  token;

      // Update and test the element count, reject the table if there are
      // to many elements.
      iNumberOfElements++;

      if (iNumberOfElements >= GRID_MODE_MAX_NUMBER_OF_ELEMEMTS)
        {
        NW_THROW_STATUS(status, KBrsrFailure);
        }

      // Ensure that text nodes aren't to large.
      if (NW_DOM_Node_getNodeType(domNode) == NW_DOM_TEXT_NODE)
        {
        NW_String_t  text;
        NW_Uint16    length;

        status = StatusCodeConvert(NW_DOM_TextNode_getData(domNode, &text));
        NW_THROW_ON_ERROR(status);

        length = NW_String_getCharCount(&text, HTTP_iso_10646_ucs_2);

        NW_String_deleteStorage(&text);

        // The text block is to large, reject the table.
        if (length > GRID_MODE_MAX_TEXT_BLOCK)
          {
          NW_THROW_STATUS(status, KBrsrFailure);
          }
        
        // Otherwise skip to the next child -- no text relevant checks follow.
        else
          {
          continue;
          }
        }

      // Get the element's token.
      token = NW_HED_DomHelper_GetElementToken(domNode);

      // The element is valid if its token is found in validTokens.
      for (NW_Int32 i = 0; i < tokenCount; i++)
        {
        if (token == validTokens[i])
          {
          tokenValid = NW_TRUE;

          // Break out of the loop.
          break;
          }
        }

      // The token is invalid, reject the table.
      if (tokenValid == NW_FALSE)
        {
        NW_THROW_STATUS(status, KBrsrFailure);
        }

      // Do additional element-level validation.
      switch (token)
        {
        case NW_XHTML_ElementToken_img:
          {
          NW_Int32        width;
          NW_DOM_Node_t*  parentNode;
          NW_Bool         siblingCheckDone = NW_FALSE;

          // Both the hight and width attributes are needed.
          status = NW_HED_DomHelper_HasAttribute(domNode, NW_XHTML_AttributeToken_height);
          NW_THROW_ON_ERROR(status);
  
          status = NW_XHTML_ElementHandler_GetAttribute(iContentHandler, domNode,
              NW_XHTML_AttributeToken_width, &widthStr);
          NW_THROW_ON_ERROR(status);

          // Reject the table if the width is to large.
          width = NW_Str_Atoi(widthStr);
          if ((width / iScreenWidth) > GRID_MODE_IMAGE_SCALE_RATIO)
            {
            NW_THROW_STATUS(status, KBrsrFailure);
            }

          NW_Str_Delete(widthStr);
          widthStr = NULL;

          // Reject the table if the image has an usemap
          status = NW_HED_DomHelper_HasAttribute(domNode, NW_XHTML_AttributeToken_usemap);
          if (status == KBrsrSuccess)
            {
            NW_THROW_STATUS(status, KBrsrFailure);
            }

          // Reject the table if the image has any siblings
          parentNode = domNode;

          do
            {
            NW_Uint16  parentToken;

            parentNode = NW_DOM_Node_getParentNode(parentNode);
            NW_THROW_ON_NULL(parentNode, status, KBrsrUnexpectedError);

            parentToken = NW_HED_DomHelper_GetElementToken(parentNode);
            
            // If the parent node is either a td or th the sibling check is done.
            // tr is also check as to handle malformed content.
            if ((parentToken == NW_XHTML_ElementToken_td) ||
                (parentToken == NW_XHTML_ElementToken_th) ||
                (parentToken == NW_XHTML_ElementToken_tr))
              {
              siblingCheckDone = NW_TRUE;
              }

            // Reject, if the node has two or more child.
            if (HasOnlyOneElementChild(parentNode) == NW_FALSE)
              {
              NW_THROW_STATUS(status, KBrsrFailure);
              }
            }
          while (!siblingCheckDone);
          }
          break;

        case NW_XHTML_ElementToken_input:
          {
          // Get the type attribute.
          status = NW_XHTML_ElementHandler_GetAttribute(iContentHandler, domNode,
              NW_XHTML_AttributeToken_type, &typeStr);

          // Reject the table if type is missing.
          NW_THROW_ON_ERROR(status);

          // Reject the table if type equals "text", "password", "file", "image".
          if ((NW_Str_StricmpConst(typeStr, "text") == 0) || 
              (NW_Str_StricmpConst(typeStr, "image") == 0) ||
              (NW_Str_StricmpConst(typeStr, "file") == 0) ||
              (NW_Str_StricmpConst(typeStr, "password") == 0))
            {
            NW_THROW_STATUS(status, KBrsrFailure);
            }

          NW_Str_Delete(typeStr);
          typeStr = NULL;
          }

        default:
          break;
        }
      
      iLastEvaluatedElement = domNode;
      }
    }

  NW_CATCH (status) 
    {
    NW_Str_Delete(widthStr);
    NW_Str_Delete(typeStr);

    iIsGridMode = EFalse;
    iLastEvaluatedElement = NULL;
    }

  NW_FINALLY 
    {
    return status;
    } NW_END_TRY
  }

// -----------------------------------------------------------------------------
// CXHTMLTableElementHandler::PostEvalTable
// 
// This method is called after all of the chunks of a table arrive.  It is used
// to determine whether or not that table should be laid out in grid mode.
// 
// Note: This method should be restructured to be like PreEvalTable if any non
//       tr tests are added.  It's like this because it only needs to check the 
//       top level children to validate tr's.
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
CXHTMLTableElementHandler::PostEvalTable()
  {
  NW_DOM_Node_t*    domNode = NULL;
  NW_DOM_Node_t*    child = NULL;
  NW_Uint16         token;
  NW_Int32          childCount;
  NW_ADT_Vector_t*  secondLevelElementQueue = NULL; 
  NW_Bool           firstLevelElement = NW_TRUE;

  NW_TRY (status) 
    {
    // Create a vector to hold first level children that need their second
    // level children checked as well.
    secondLevelElementQueue = (NW_ADT_Vector_t*) NW_ADT_ResizableVector_New(
        sizeof(NW_DOM_Node_t*), 1, 2);
    NW_THROW_OOM_ON_NULL(secondLevelElementQueue, status);

    domNode = NW_DOM_Node_getFirstChild(iElementNode);

    while (domNode != NULL)
      {
      for (; domNode != NULL; domNode = NW_DOM_Node_getNextSibling(domNode))
        {
        // Skip non-element nodes.
        if (NW_DOM_Node_getNodeType(domNode) != NW_DOM_ELEMENT_NODE)
          {
          continue;
          }

        // Handle non-tr children.  A queue is used to handle malformed tables like:
        // <table><form><tr>...</tr><tr>...</tr></form></table> -- note: the 
        // trs are second level children not first level as they should be.  
        token = NW_HED_DomHelper_GetElementToken(domNode);
        if (token != NW_XHTML_ElementToken_tr)
          {
          // When processing the first level children append any non-tr node to 
          // the queue the children of these node will be checked in later iterations.
          if (firstLevelElement)
            {
            NW_ADT_DynamicVector_InsertAt(secondLevelElementQueue, &domNode, NW_ADT_Vector_AtEnd);
            continue;
            }

          // Otherwise reject the table as its too malformed to reliably handled.
          else
            {
            NW_THROW_STATUS(status, KBrsrFailure);
            }
          }
      
        // Skip tr's without children.
        if (NW_DOM_Node_hasChildNodes(domNode) == NW_FALSE)
          {
          continue;
          }

        // Count the number of columns in this row.
        childCount = 0;
      
        // Count the first child.     
        child = NW_DOM_Node_getFirstChild(domNode);
        if (NW_DOM_Node_getNodeType(child) == NW_DOM_ELEMENT_NODE)
          {
          childCount++;
          }

        // Count the other children.
        while ((child = NW_DOM_Node_getNextSibling(child)) != NULL)
          {
          if (NW_DOM_Node_getNodeType(child) == NW_DOM_ELEMENT_NODE)
            {
            childCount++;
            }
          }
    
        // Reject the table if there are to many columns.
        if ((childCount / iScreenWidth) > GRID_MODE_COL_RATIO)
          {
          NW_THROW_STATUS(status, KBrsrFailure);
          }
        }

      // If need be process the next queued first level non-tr node.
      if ((secondLevelElementQueue != NULL) && 
          (NW_ADT_Vector_GetSize(secondLevelElementQueue) > 0))
        {
        status = NW_ADT_Vector_GetElementAt(secondLevelElementQueue, 0, &domNode);
        NW_THROW_ON_ERROR(status);

        status = NW_ADT_DynamicVector_RemoveAt(secondLevelElementQueue, 0);
        NW_THROW_ON_ERROR(status);

        firstLevelElement = NW_FALSE;
        domNode = NW_DOM_Node_getFirstChild(domNode);
        }
      }
    }

  NW_CATCH (status) 
    {
    iIsGridMode = EFalse;
    }

  NW_FINALLY 
    {
    NW_Object_Delete(secondLevelElementQueue);

    return status;
    } NW_END_TRY
  }
