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

/* Includes */
#include "nwx_string.h"
#include "nwx_url_utils.h"
#include "nw_errnotify.h"
#include "nwx_url_utils.h"
#include "nwx_http_header.h"
#include "nw_wml_api.h"
#include "nw_wml_core.h"
#include "nwx_logger.h"
#include "nw_hed_documentroot.h"
#include "nw_wml1x_wml1xcontenthandler.h"
#include "nw_wml1x_wml1xeventhandler.h"
#include "nw_wml1x_wml1xdefaultstylesheet.h"
#include "nw_lmgr_bidiflowbox.h"
#include "nw_lmgr_cssproperties.h"
#include "nw_lmgr_textbox.h"
#include "nw_lmgr_breakbox.h"
#include "nw_lmgr_emptybox.h"
#include "nw_lmgr_statictablebox.h"
#include "nw_lmgr_statictablerowbox.h"
#include "nw_lmgr_statictablecellbox.h"
#include "nw_lmgr_activecontainerbox.h"
#include "nw_hed_hedeventhandler.h"
#include "nw_image_cannedimages.h"
#include "nw_image_virtualimage.h"
#include "nw_text_ascii.h"
#include "nw_text_ucs2.h"
#include "nw_adt_map.h"
#include "nw_adt_dynamicvector.h"
#include "nw_fbox_formliaison.h"
#include "nw_fbox_checkbox.h"
#include "nw_fbox_radiobox.h"
#include "nw_fbox_inputbox.h"
#include "nw_fbox_passwordbox.h"
#include "nw_wml1x_wml1xactiveevent.h"
#include "nwx_settings.h"
#include "nw_lmgr_accesskey.h"
#include "wml_elm_attr.h"
#include "nw_markup_wmlvalidator.h"
#include "nw_system_optionlist.h"
#include "HEDDocumentListener.h"
#include "nw_lmgr_rootbox.h"
#include "nw_lmgr_imgcontainerbox.h"
#include "nw_lmgr_animatedimagebox.h"

#include "nw_hed_historyvisitor.h"
#include "urlloader_urlloaderint.h"
#include "MVCView.h"
#include "nw_wml1x_wml1xapi.h"
#include "nwx_http_defs.h"
#include "BrsrStatusCodes.h"
#include <bodypart.h>
#include "MVCShell.h"
#include "BrCtl.h"
#include <MemoryManager.h>

/*
**-------------------------------------------------------------------------
**  Local Macros
**-------------------------------------------------------------------------
*/
#define NW_MSEC_PER_WML_TICK 100
/*
**-------------------------------------------------------------------------
**  Internal Prototypes
**-------------------------------------------------------------------------
*/
/* display functions called by the WML Browser */
static TBrowserStatusCode NW_UI_CreateCard(void *wae);
static TBrowserStatusCode NW_UI_ShowCard(void *wae);
static TBrowserStatusCode NW_UI_DestroyCard(void *wae);
static TBrowserStatusCode NW_UI_AddElement(void *wae, NW_Wml_ElType_e elemType,
                                    NW_Int16 *elemId);
static TBrowserStatusCode NW_UI_CleanUp(void *wae);
static TBrowserStatusCode NW_UI_SetOptState(void *wae, NW_Uint16 elemId, NW_Bool st);
static TBrowserStatusCode NW_UI_GetOptState(void *wae, NW_Uint16 elemId, NW_Bool* st);
static TBrowserStatusCode NW_UI_Refresh(void *wae);

static TBrowserStatusCode NW_Wml1x_CreateTimer(void *usrAgent, NW_Uint32 period);
static TBrowserStatusCode NW_Wml1x_ReadTimer(void *usrAgent, NW_Uint32 *period);
static TBrowserStatusCode NW_Wml1x_DestroyTimer(void *usrAgent);
static TBrowserStatusCode NW_Wml1x_ResumeTimer(void *usrAgent);
static TBrowserStatusCode NW_Wml1x_StopTimer(void *usrAgent);
static TBrowserStatusCode NW_Wml1x_IsTimerRunning(void *usrAgent, NW_Bool *isRunning);
/*
**-------------------------------------------------------------------------
**  File Scoped Static Variables
**-------------------------------------------------------------------------
*/

/* WML Core Routines. */
static const NW_DisplayApi_t panel_api =
{
  NW_UI_CreateCard,
  NW_UI_ShowCard,
  NW_UI_DestroyCard,
  NW_UI_AddElement,
  NW_UI_CleanUp,
  NW_UI_GetOptState,
  NW_UI_SetOptState,
  NW_UI_Refresh,
};

/*Timer API wrappers for WML 1.x content handler*/
static const NW_TimerApi_t Wml1x_timer_api =
{
  NW_Wml1x_CreateTimer,
  NW_Wml1x_ReadTimer,
  NW_Wml1x_DestroyTimer,
  NW_Wml1x_ResumeTimer,
  NW_Wml1x_StopTimer,
  NW_Wml1x_IsTimerRunning
};


static const NW_WmlApi_t wml_api =
{
  &panel_api,
  &Wml1x_timer_api
};

/* Additional constants */
static const NW_Ucs2 monospace[] = {'m', 'o', 'n', 'o', 's', 'p', 'a', 'c', 'e', '\0'};
static const NW_Ucs2 spaceString[] = {NW_TEXT_UCS2_NBSP,'\0'};

/*
**-------------------------------------------------------------------------
**  Internal Functions
**-------------------------------------------------------------------------
*/


/*****************************************************************

  Name:         NW_UI_SetParagraphProperties()

  Description:  set wrap and alignment paragraph properties

  Parameters:   
    thisObj - In      - the content handler on behalf of which the work is being done
    elId    - In      - the element id of the <p> element being processed, used in the WML query service
    box     - In      - pointer to the box tree node representing the <p> element
    nowrap  - In/Out  - pointer to boolean in which to maintain current nowrap property value

  Algorithm:    wrap property is 'sticky' from previous paragraphs
                alignment is left unless attribute set otherwise

  Return Value: KBrsrOutOfMemory
                KBrsrSuccess

*****************************************************************/
static
TBrowserStatusCode
NW_UI_SetParagraphProperties (NW_Wml1x_ContentHandler_t *thisObj,
                              NW_Uint16                 elId,
                              NW_LMgr_Box_t             *box,
                              NW_Bool                   *nowrap,
                              NW_LMgr_Box_t             **deleteBox)
{

  NW_LMgr_ContainerBox_t  *container = NW_LMgr_ContainerBoxOf(box);
  NW_ADT_DynamicVector_t  *children = NW_LMgr_ContainerBoxOf(box)->children;
  NW_Ucs2                 *retString = NULL;
  NW_LMgr_PropertyValue_t value;
  NW_LMgr_Property_t      prop;

  NW_TRY (status) {
    /* We must ignore empty P elements, see [WAP WML] */
    if (NW_ADT_Vector_GetSize(children) == 0) {
      *deleteBox = box;
      NW_THROW_SUCCESS(status);
    }
    else if (NW_ADT_Vector_GetSize(children) == 1) {
      NW_LMgr_Box_t *child = NW_LMgr_ContainerBox_GetChild(container, 0);
      if (NW_Object_IsClass(child, &NW_LMgr_TextBox_Class)) {
        /* if the box tree were complete here, we could query the text box to see if it is blank
        but since the text box has not yet been visited by the ShowCard() process, such a query
        will always return blank!  Instead, query the wmlInterpreter for the STR_VALUE of the element */
        status = NW_LMgr_Box_GetPropertyValue(child, NW_CSS_Prop_elementId, NW_CSS_ValueType_Integer, &value);

        if (status == KBrsrSuccess) {
          NW_Uint16 childElId = 0;

          childElId = NW_UINT16_CAST(value.integer);
          status = NW_Wml_GetAttribute(&(thisObj->wmlInterpreter),
                                       childElId,
                                       STR_VALUE,
                                       &retString);
          NW_THROW_ON (status, KBrsrOutOfMemory);
          if (retString == NULL) {
            /* must delete the child of box here, before the ShowCard() box visitor accesses it. */
            status = NW_LMgr_ContainerBox_RemoveChild(container,
                                                      child);
            NW_Object_Delete(child);
            *deleteBox = box;
            NW_THROW_SUCCESS(status);
          }
          else {
            NW_Uint32 i;
            NW_Uint32 j;

            /* if all the characters are whitespace then throw(success)*/
            j = NW_Str_Strlen(retString);
            for (i = 0; i < j; i++) {
              if (!NW_Str_Isspace(retString[i])) {
                break;
              }
            }
            if (i == j) {
              NW_THROW_SUCCESS(status);
            }
            NW_Str_Delete(retString);
            retString = NULL;
          }
        }
      }
    }

    /* Get the wrap mode attribute */
    status = NW_Wml_GetAttribute(&(thisObj->wmlInterpreter), 
                                 elId, 
                                 MODE_ATTR, 
                                 &retString);
    NW_THROW_ON (status, KBrsrOutOfMemory);
    
    if (retString != NULL) {
      if (NW_Str_StricmpConst(retString,WAE_ASC_WRAP_STR)==0) {
        *nowrap = NW_FALSE;
      } else if (NW_Str_StricmpConst(retString,WAE_ASC_NOWRAP_STR)==0) {
        *nowrap = NW_TRUE;
      }

      NW_Str_Delete(retString);
      retString = NULL;
    }

    /* If no wrap mode is set, we inherit the wrap property of the
       previous paragraph */
    prop.type = NW_CSS_ValueType_Token;
    prop.value.token = (*nowrap) ? NW_CSS_PropValue_nowrap : NW_CSS_PropValue_normal;
    NW_LMgr_Box_SetProperty (box, NW_CSS_Prop_whitespace, &prop);   

    /* Get the alignment attribute */
    status = NW_Wml_GetAttribute(&(thisObj->wmlInterpreter),
                                 elId,
                                 ALIGN_ATTR,
                                 &retString);
    NW_THROW_ON (status, KBrsrOutOfMemory);

    if (retString != NULL) {
      if (NW_Str_StricmpConst(retString,WAE_ASC_CENTER_STR) == 0) {
        prop.value.token = NW_CSS_PropValue_center;
      }
      else if (NW_Str_StricmpConst(retString,WAE_ASC_RIGHT_STR) == 0) {
        prop.value.token = NW_CSS_PropValue_right;
      }
      else {
        prop.value.token = NW_CSS_PropValue_left;
      }

      NW_Str_Delete(retString);
    }
    else {
      prop.value.token = NW_CSS_PropValue_left;
    }

    NW_LMgr_Box_SetProperty (box, NW_CSS_Prop_textAlign, &prop);   
  }
  NW_CATCH (status) {
  }
  NW_FINALLY {
    return status;
  } NW_END_TRY
}


/*****************************************************************

  Name:         NW_UI_SetCellProperties()

  Description:  set cell border properties

  Parameters:   
    box     - In      - pointer to the box tree node representing the <td> element

  Algorithm:    

  Return Value: KBrsrSuccess

*****************************************************************/
static
TBrowserStatusCode
NW_UI_SetCellProperties (NW_LMgr_Box_t *box)
{
  NW_LMgr_Property_t  prop;

  NW_TRY (status) {
    prop.type = NW_CSS_ValueType_Token;
    prop.value.token = NW_CSS_PropValue_hidden;
    status = NW_LMgr_Box_SetProperty (box, NW_CSS_Prop_overflow, &prop);

    _NW_THROW_ON_ERROR (status);
  }
  NW_CATCH (status) {
  }
  NW_FINALLY {
    return status;
  }NW_END_TRY
}


/*****************************************************************

  Name:         NW_UI_NormalizeTable()

  Description:  perform WML table column width processing

  Parameters:   
    thisObj - In      - the content handler on behalf of which the work is being done
    elId    - In      - the element id of the element being processed, used in the WML query service
    box     - In      - pointer to the box tree node representing the element

  Algorithm:    

 * The behavior of WML Tables is not exactly like the behavior of CSS Tables.
 * Two issues in particular need to be addressed:
 * 1) Since the CSS Static Algorithm used in the Layout Manager
 *    calculates the dimensions of the table based on the head row, the table
 *    will contain only as many columns as there are in the first row.  In WML
 *    this information is passed in the "columns" attribute.  Here we pad the
 *    first row if it does not contain enough cells.
 * 2) in WML, any extra cells in a row are "aggregated" into the last cell.

  Return Value: KBrsrOutOfMemory
                KBrsrSuccess
                various other TBrowserStatusCode values 

*****************************************************************/
static
TBrowserStatusCode
NW_UI_NormalizeTable (NW_Wml1x_ContentHandler_t *thisObj,
                      NW_Uint16                 elId,
                      NW_LMgr_Box_t             *box)
{
  NW_LMgr_Box_t          *newBox = NULL;
  NW_LMgr_Box_t          *tmpBox = NULL;
  NW_Text_UCS2_t         *spaceText = NULL;
  NW_LMgr_ContainerBox_t *newCell = NULL;

  NW_TRY (status) {
  NW_Ucs2                *retString;
  NW_ADT_Vector_Metric_t cols = 1;
  NW_ADT_Vector_Metric_t numRows;
  NW_ADT_Vector_Metric_t numCells;
  NW_LMgr_ContainerBox_t *tableContainer = NW_LMgr_ContainerBoxOf(box);
  NW_LMgr_ContainerBox_t *cell = NULL;
  NW_LMgr_ContainerBox_t *lastCell = NULL;
  NW_LMgr_ContainerBox_t *rowContainer = NULL;
  NW_LMgr_Property_t     prop;
  NW_ADT_Vector_Metric_t i;
  NW_ADT_Vector_Metric_t j;

  /* WML table optimization */
  prop.type = NW_CSS_ValueType_Integer;
  prop.value.token = NW_CSS_PropValue_flags_wmlTable;
  NW_LMgr_Box_SetProperty (box, NW_CSS_Prop_flags, &prop);

  /* Set the margin for the table */
  prop.type = NW_CSS_ValueType_Px;
  prop.value.integer = 2;
  NW_LMgr_Box_SetProperty (box, NW_CSS_Prop_margin, &prop);

  /* Get the "columns" attribute */
  status = NW_Wml_GetAttribute(&(thisObj->wmlInterpreter),
                               elId,
                               COLUMNS_ATTR,
                               &retString);  

  if (status == KBrsrOutOfMemory) {
    return status;
  }

  if (retString != NULL) {
		if(NW_Str_StrIsValidLength(retString))
		{
			cols = (NW_ADT_Vector_Metric_t)NW_Str_Atoi (retString);
		}
    if (cols == 0) {
      /* If the attribute value is illegal, we will make the table one column wide */
      cols = 1;
    }

    NW_Str_Delete (retString);
  }

  numRows = NW_LMgr_ContainerBox_GetChildCount(tableContainer);

  for (i = 0; i < numRows; i++) {
    rowContainer = (NW_LMgr_ContainerBox_t*) NW_LMgr_ContainerBox_GetChild (tableContainer, i);

	if (rowContainer->children == NULL)
		NW_THROW_SUCCESS(status);

	numCells = NW_LMgr_ContainerBox_GetChildCount(rowContainer);

    if (numCells == cols) {
      continue;
    }

    /* If there is an insufficient number of cells in a row,
     * we must pad it with extra empty boxes */
    else if (numCells < cols) {
      for (j = numCells; j < cols; j++) {

        /* Create a new cell container and set its properties */
        newCell = (NW_LMgr_ContainerBox_t*)NW_LMgr_StaticTableCellBox_New(1);
          NW_THROW_OOM_ON_NULL (newCell, status);

        NW_UI_SetCellProperties (NW_LMgr_BoxOf(newCell));

        /* Create the text object with a single space */
        spaceText =
          NW_Text_UCS2_New((NW_Ucs2*) spaceString, 1, 0);
          NW_THROW_OOM_ON_NULL (spaceText, status);

        /* Create a text box to hold the space */
        newBox = (NW_LMgr_Box_t*)
          NW_LMgr_TextBox_New (1, NW_TextOf (spaceText));
          NW_THROW_OOM_ON_NULL (newBox, status);

        /* Add the empty box to the cell container */
        status = NW_LMgr_ContainerBox_AddChild (newCell, NW_LMgr_BoxOf(newBox));
          _NW_THROW_ON_ERROR(status);

        /* Add the cell to the row container */
        status = NW_LMgr_ContainerBox_AddChild (rowContainer, NW_LMgr_BoxOf(newCell));
        if (status != KBrsrSuccess) {
           (void)NW_LMgr_ContainerBox_RemoveChild (newCell, NW_LMgr_BoxOf(newBox));
             NW_THROW (status);
        }
      }
    }

    /* If there are extra cells in the row, we must aggregate them into
     * the last column
     */
    else if (numCells > cols) {
      /* Get the cell in the last column */
      lastCell = (NW_LMgr_ContainerBox_t*)NW_LMgr_ContainerBox_GetChild (rowContainer, 
                                            (NW_ADT_Vector_Metric_t)(cols-1));
      NW_ASSERT (NW_Object_IsInstanceOf(lastCell, &NW_LMgr_ContainerBox_Class));

      /* Aggregate the remaining cells */
      for (j = cols; j < numCells; j++) {
        /* Get the first extra cell */
        tmpBox = NW_LMgr_ContainerBox_GetChild (rowContainer, j);
        if (!NW_Object_IsInstanceOf(tmpBox, &NW_LMgr_StaticTableCellBox_Class))
        {
           status = NW_LMgr_ContainerBox_RemoveChild (rowContainer, NW_LMgr_BoxOf(tmpBox));
           _NW_THROW_ON_ERROR(status);
           j--;
           numCells--;
           NW_Object_Delete(tmpBox);
           continue;
        }

        cell = (NW_LMgr_ContainerBox_t*)tmpBox;

        /* Create the text object with a single space */
        spaceText =
          NW_Text_UCS2_New((NW_Ucs2*) spaceString, 1, 0);
          NW_THROW_OOM_ON_NULL (spaceText, status);

        newBox = (NW_LMgr_Box_t*)
          NW_LMgr_TextBox_New (1, NW_TextOf (spaceText));
          NW_THROW_OOM_ON_NULL (newBox, status);

        /* Add the empty box to the last legal cell */
        status = NW_LMgr_ContainerBox_AddChild (lastCell, NW_LMgr_BoxOf(newBox));
          _NW_THROW_ON_ERROR(status);

        /* Remove the extra box from the row */
        status = NW_LMgr_ContainerBox_RemoveChild (rowContainer, NW_LMgr_BoxOf(cell));
          _NW_THROW_ON_ERROR(status);
        j--;
        numCells--;

        /* Append the extra content */
        status = NW_LMgr_ContainerBox_AppendChildrenOf (lastCell, cell);
          _NW_THROW_ON_ERROR(status);

        /* We don't want to destroy the cell content, because we have copied it
         * to lastCell */
        status = NW_ADT_DynamicVector_Clear (
          (NW_ADT_DynamicVector_t*)NW_LMgr_ContainerBox_GetChildren(cell));
          _NW_THROW_ON_ERROR(status);

        /* Finally, delete the cell */
        NW_Object_Delete(tmpBox);
      }
    }
  }

  } NW_CATCH (status) {
  NW_Object_Delete(newBox);
  NW_Object_Delete(spaceText);
  NW_Object_Delete(newCell);

  } NW_FINALLY {
  return status;
  } NW_END_TRY
}


/*****************************************************************

  Name:         NW_UI_SetCellAlignment()

  Description:  set alignment cell properties for a table sub-tree

  Parameters:   
    thisObj - In      - the content handler on behalf of which the work is being done
    elId    - In      - the element id of the element being processed, used in the WML query service
    box     - In      - pointer to the box tree node representing the table

  Algorithm:    alignment is left unless attribute set otherwise

  Return Value: KBrsrOutOfMemory
                KBrsrSuccess

*****************************************************************/
static
TBrowserStatusCode
NW_UI_SetCellAlignment (NW_Wml1x_ContentHandler_t *thisObj,
                        NW_Uint16                 elId,
                        NW_LMgr_Box_t             *box)
{
  NW_LMgr_Property_t     prop;
  NW_Ucs2                *retString;
  NW_ADT_Vector_Metric_t numRows;
  NW_ADT_Vector_Metric_t numCells;
  NW_LMgr_ContainerBox_t *tableContainer = NW_LMgr_ContainerBoxOf(box);
  NW_LMgr_ContainerBox_t *cell = NULL;
  NW_LMgr_ContainerBox_t *rowContainer = NULL;
  TBrowserStatusCode            status;
  NW_ADT_Vector_Metric_t i;
  NW_ADT_Vector_Metric_t j;

  /* First get the alignment attribute */
  status = NW_Wml_GetAttribute(&(thisObj->wmlInterpreter),
                               elId,
                               ALIGN_ATTR,
                               &retString);  

  if (status == KBrsrOutOfMemory) {
    return status;
  }

  /* Get the number of rows */
  numRows = NW_LMgr_ContainerBox_GetChildCount(tableContainer);

  /* Set the alignment properties on the cells */
  prop.type = NW_CSS_ValueType_Token;
  if (retString != NULL) {
    for (i = 0; i < numRows; i++) {
      /* Get the row */
      rowContainer = (NW_LMgr_ContainerBox_t*)NW_LMgr_ContainerBox_GetChild (tableContainer, i);
      numCells = NW_LMgr_ContainerBox_GetChildCount(rowContainer);

      for (j = 0; j < numCells; j++) {
        /* Get the cell */
        cell = (NW_LMgr_ContainerBox_t*)NW_LMgr_ContainerBox_GetChild (rowContainer, j);

        if (j < NW_Str_Strlen(retString)) {
          switch (retString[j]) 
          {
          case 'R':
            prop.value.token = NW_CSS_PropValue_right;
            status = NW_LMgr_Box_SetProperty (NW_LMgr_BoxOf(cell), NW_CSS_Prop_textAlign, &prop);
            break;

          case 'C':
            prop.value.token = NW_CSS_PropValue_center;
            status = NW_LMgr_Box_SetProperty (NW_LMgr_BoxOf(cell), NW_CSS_Prop_textAlign, &prop);
            break;

          default:
            prop.value.token = NW_CSS_PropValue_left;
            status = NW_LMgr_Box_SetProperty (NW_LMgr_BoxOf(cell), NW_CSS_Prop_textAlign, &prop);
            break;
          }
        }
      }
    }
    
    NW_Str_Delete(retString);
  }     

  return status;
}


/*****************************************************************

  Name:         NW_UI_LoadImage()

  Description:  request WML <img> source or localsrc load

  Parameters:   
    thisObj - In      - the content handler on behalf of which the work is being done    
    elId    - In      - the element id of the <img> element being processed, used in the WML query service

  Algorithm:    

  Return Value: KBrsrOutOfMemory
                KBrsrSuccess
                TBrowserStatusCode value from NW_HED_DocumentRoot_StartLoad()

*****************************************************************/
TBrowserStatusCode 
NW_UI_LoadImage (NW_Wml1x_ContentHandler_t *thisObj,                                    
                 NW_Uint16 elId, NW_Bool showImage )
{
  NW_Ucs2                      *imageUrl = NULL;
  TBrowserStatusCode                  status;
  NW_Text_UCS2_t               *urlObj;
  NW_HED_DocumentRoot_t        *documentRoot;
  NW_HED_UrlRequest_LoadMode_t loadMode = NW_HED_UrlRequest_LoadNormal;
 	
  /* The loadMode flag is to trigger (src=)Url to be loaded after the 
     (localsrc=)Url load failed */


  /* NW_Settings_GetImageEnabled() called to determine whether 
  autoloading of images is turned on. If not, then just display
  the initial "x" and alt text, and don't issue any load requests.
  */

  if (!showImage)
  {
    if (!NW_Settings_GetImagesEnabled()) {
      return KBrsrSuccess;
    }
  }
  
  status = NW_Wml_GetLocalImageUrl(&(thisObj->wmlInterpreter),
                                     elId,
                                     &imageUrl);
  if (status == KBrsrSuccess) {
    loadMode = NW_HED_UrlRequest_LoadLocal;
  }
  else {

  status = NW_Wml_GetImageUrl(&(thisObj->wmlInterpreter),
                              elId,
                              &imageUrl);
  }

  if (status == KBrsrSuccess) {

    NW_ASSERT(imageUrl);

    urlObj =
      NW_Text_UCS2_New (imageUrl, 0, NW_Text_Flags_TakeOwnership);

    if (urlObj != NULL) {
      NW_HED_UrlRequest_t *urlRequest;
    
      /* get the documentRoot and invoke the Load method */

      documentRoot =
        (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode (thisObj);
      NW_ASSERT(documentRoot != NULL);

      /* submit the load request */
      urlRequest = NW_HED_UrlRequest_New (NW_TextOf (urlObj), NW_URL_METHOD_GET, NULL,
                                          NULL, NW_HED_UrlRequest_Reason_DocLoadChild, 
                                          loadMode, NW_UrlRequest_Type_Image);

      if (urlRequest != NULL) {
        NW_Uint32 elId32 = elId;
        if (NW_Wml1x_ContentHandler_IsSaveddeck(thisObj))
        {
          NW_HED_UrlRequest_SetCacheMode(urlRequest, NW_CACHE_ONLYCACHE);
        }
        else if (NW_Wml1x_ContentHandler_IsHistload(thisObj))
        {
          NW_HED_UrlRequest_SetCacheMode(urlRequest, NW_CACHE_HISTPREV);
        }
        else if (NW_Wml1x_ContentHandler_IsReload(thisObj))
        {
          NW_HED_UrlRequest_SetCacheMode(urlRequest, NW_CACHE_NOCACHE);
        }

        // compare url with body part
        NW_Uint8 freeNeeded;
        NW_Ucs2* resolvedUrlUcs2 = NW_Text_GetUCS2Buffer( NW_TextOf (urlObj), NULL, NULL, &freeNeeded);
        CBodyPart* bodyPart = NULL;
        TBool isUrlInMultipart = _NW_HED_CompositeContentHandler_IsUrlInMultipart( 
                                                 NW_HED_CompositeContentHandlerOf(thisObj),
                                                 resolvedUrlUcs2, &bodyPart );
        if( freeNeeded )
            {
            NW_Mem_Free( resolvedUrlUcs2 );
            }

        if( isUrlInMultipart )
            {
            TDataType dataType( bodyPart->ContentType() );

            TUint8* charset = (TUint8*) bodyPart->Charset().Ptr();
            TInt lenCh = bodyPart->Charset().Length();
            //R CShell* shell = REINTERPRET_CAST(CShell*, NW_Ctx_Get(NW_CTX_BROWSER_APP, 0));  
            CShell* shell = NULL;  //R
            NW_ASSERT(shell);
            TInt32 uidInt = shell->GetUidFromCharset( charset, lenCh );
            TUid uid = TUid::Uid( uidInt );

            CBrCtl* brCtl = shell->BrCtl();
            /*
            brCtl->LoadDataL( bodyPart->Url(), bodyPart->Body(), dataType, uid, 
                              NW_HED_DocumentNodeOf (thisObj), (void*)elId32, 
                              NULL, NULL, urlRequest->loadType, urlRequest->method );
            */                  
            }
        else
            {
            status = NW_HED_DocumentRoot_StartRequest (documentRoot, NW_HED_DocumentNodeOf (thisObj),
                urlRequest, (void*)elId32 );

            if (status != KBrsrSuccess) 
                {
                NW_Object_Delete (urlRequest);
                }
            }
      }
      else {
        status = KBrsrOutOfMemory;
      }

      NW_Object_Delete(urlObj);
    } else {
      status = KBrsrOutOfMemory; /* NW_Text_UCS2_New() takes care of deleting string when fails */
    }
  }
  
  if (status == KBrsrSuccess) {
    thisObj->wmlInterpreter.outstandingLoadCount++;
  }
  return status;
}


/*****************************************************************

  Name:         IsThereAltAttrib()

  Description:  Returns "alt" attribute of the "image" element

  Parameters:   
    thisObj - In      - the content handler on behalf of which the work is being done
    elId    - In      - the element id of the <img> element being processed
    altStr  - In/Out  - pointer to variable which will receive address of 
                UCS2_Text_t object with value of ALT_ATTR from <img> element

  Algorithm:    

  Return Value: -NW_TRUE when string for alt text successfully extructed
                 *altStr has the value of the string pointer.
                -NW_FALSE when the attribute does not exist or 
                 the string was not created. *altStr = NULL

*****************************************************************/
static NW_Bool IsThereAltAttrib (NW_Wml1x_ContentHandler_t  *thisObj, 
                                 NW_Uint16                  elId, 
                                 NW_Text_t                  **altStr)
{
  NW_Bool     ret = NW_FALSE;
  NW_Ucs2     *retString = NULL;
  
  (void) NW_Wml_GetAttribute(&(thisObj->wmlInterpreter),
		                         elId,
														 ALT_ATTR,
														 &retString);

  if (retString) {
    *altStr = (NW_Text_t*)
      NW_Text_UCS2_New (retString, NW_Str_Strlen(retString), 
                        NW_Text_Flags_TakeOwnership);
    if (*altStr) {
      ret = NW_TRUE;
    }
  }

  return ret;
}


/*
**-------------------------------------------------------------------------
**  Display API functions called by the WML Browser
**-------------------------------------------------------------------------
*/

/*****************************************************************

  Name:         NW_UI_CreateCard()

  Description:  callback for WML interpreter to initialize new card

  Parameters:
    wae     - In      - the content handler on behalf of which the work is being done

  Algorithm:

  Return Value:

*****************************************************************/
static TBrowserStatusCode NW_UI_CreateCard (void *wae)
{

  NW_Wml1x_ContentHandler_t *thisObj;
  NW_HED_CompositeNode_t*   compositeNode;
  NW_LMgr_Property_t        prop;
  NW_LMgr_BidiFlowBox_t       *bidiFlowBox;

  NW_TRY (status) {

    NW_LOG0(NW_LOG_LEVEL1, "<card>");

    thisObj = NW_Wml1x_ContentHandlerOf(wae);
    compositeNode = (NW_HED_CompositeNode_t*) 
      NW_Object_QueryAggregate (thisObj, &NW_HED_CompositeNode_Class);
    NW_ASSERT (compositeNode != NULL);

	//Note: There is a iCurrent in MVCView which points to one of the box that has the focus in the boxtree .
	// Since the boxtree will be deleted in next step, we need to set the iCurrent to null. Otherwise it will
	// be invalid and can cause problem (It had caused bug). 
    NW_HED_DocumentRoot_t*   docRoot = (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode( thisObj );
  	NW_ASSERT( docRoot != NULL );	
    //get the root box
	NW_LMgr_RootBox_t*       rootBox = NW_HED_DocumentRoot_GetRootBox( docRoot );
    NW_ASSERT( rootBox != NULL );
	//get the boxtree listener which is a MVCView object
	if(rootBox && (NW_Object_IsClass(rootBox, &NW_LMgr_RootBox_Class)))
	{
	  MBoxTreeListener* btlistener = rootBox->boxTreeListener;
	  //if the listener is not null, set its iCurrent to null
	  if(btlistener != NULL)
	  {
			  ((CView*)btlistener)->SetCurrentBox(NULL);
	  }
	}
    /* free up all our child nodes; ie.image content handlers */
    NW_HED_CompositeNode_DeleteChildren (compositeNode);

    /* get rid of the old data */
    NW_Object_Delete (NW_HED_ContentHandlerOf (thisObj)->boxTree);
    NW_HED_ContentHandlerOf (thisObj)->boxTree = NULL;
    NW_Object_Delete(thisObj->optionMap);
    thisObj->optionMap = NULL;
    /*
    ** create a new box tree for the card.  DON'T USE NEW METHOD INSIDE CAST
    ** AS IT WILL GET CALLED MULTIPLE TIMES WHEN MACRO IS EXPANDED
    */
    bidiFlowBox = NW_LMgr_BidiFlowBox_New(0);
    NW_THROW_OOM_ON_NULL (bidiFlowBox, status);

    prop.type = NW_CSS_ValueType_Token;
    prop.value.token = NW_CSS_PropValue_display_block;
    status = NW_LMgr_Box_SetProperty(NW_LMgr_BoxOf(bidiFlowBox), NW_CSS_Prop_display, &prop);

    NW_HED_ContentHandlerOf (thisObj)->boxTree = NW_LMgr_BoxOf(bidiFlowBox);
    thisObj->currentBox = NW_HED_ContentHandlerOf (thisObj)->boxTree;

    NW_Wml1x_DefaultCardStyle(&prop, NW_LMgr_ContainerBoxOf(bidiFlowBox));
    
    /* create a map to associate option element states with option element ids */
    thisObj->lastId = 0;

    thisObj->optionMap =
      NW_ADT_ResizableMap_New (sizeof (thisObj->lastId), /* sizeof an element id */
                              sizeof (NW_Bool),   /* sizeof an element state */
                              8,                  /* guess at initial number of options */
                              4);                 /* number to increment by when resizing */
    NW_THROW_OOM_ON_NULL (thisObj->optionMap, status);
  }
  NW_CATCH (status) {
  }
  NW_FINALLY {
    return status;
  } NW_END_TRY
}


/*****************************************************************

  Name:         NW_UI_ShowCard()

  Description:  callback for WML interpreter to render card

  Parameters:
    wae     - In      - the content handler on behalf of which the work is being done

  Algorithm:    finish populating the box tree and call
                  NW_HED_DocumentNode_NodeChanged()

  Return Value:

*****************************************************************/
static TBrowserStatusCode NW_UI_ShowCard (void *wae)
{
  NW_Wml1x_ContentHandler_t *thisObj;
  TBrowserStatusCode               status = KBrsrSuccess;
  NW_LMgr_BoxVisitor_t      boxVisitor;
  NW_LMgr_Box_t             *box;
  NW_LMgr_Box_t             *rootBox;
  NW_LMgr_ContainerBox_t    *parentBox;
  NW_LMgr_Box_t             *deleteBox = NULL;
  NW_LMgr_PropertyValue_t   value;
  NW_LMgr_Property_t        prop;
  NW_Wml_ElType_e           elType = UNKNOWN_ELEMENT;
  NW_Uint16                 elId = 0;
  NW_Wml_Element_t          *el;
  NW_Text_UCS2_t            *temptext;
  NW_Ucs2                   *retString = NULL;
  NW_HED_DocumentNode_t     *parent;
  NW_HED_DocumentRoot_t     *docRoot;
  NW_Bool                   currentSelectMultipleState = NW_FALSE;
  NW_Int32                  currentSelectTabIndexVal = 0;

  NW_Bool                   nowrap = NW_FALSE;
  const NW_Text_t           *formatText;
  NW_FBox_Validator_EmptyOk_t emptyOk;
  NW_Markup_WmlValidator_t  *validator;

  thisObj = NW_Wml1x_ContentHandlerOf(wae);

  rootBox = NW_HED_ContentHandlerOf (thisObj)->boxTree;

  NW_LOG0(NW_LOG_LEVEL1, "</card>");

  /* The reason to do this here instead of at time of content handler initialize is:
   * when we come from WML doc to WML doc, the old WML Content Handler is used, no new
   * content handler is created or initialized. */

  /* Initialize the dynamic item list in the OptionList
     This is needed for ISA, not for win32  */
  docRoot = (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode (thisObj);

  NW_ASSERT(docRoot != NULL);
  NW_ASSERT(docRoot->appServices != NULL);

  NW_System_OptionList_Initialize ();
  if (thisObj->optionItemList != NULL) {
    NW_Wml1x_ContentHandler_DeleteOptionItemList(thisObj->optionItemList);
    thisObj->optionItemList = NULL;
  }

  /* initialize the boxVisitor */
  status =
    NW_LMgr_BoxVisitor_Initialize (&boxVisitor, NW_LMgr_BoxOf (rootBox));
  if (status != KBrsrSuccess) {
    return status;
  }

  /* traverse the boxTree */
  while ((box = NW_LMgr_BoxVisitor_NextBox (&boxVisitor, NULL)) != NULL) 
  {

    /* get rid of leftover values */
    temptext = NULL;
    retString = NULL;

    /* check the deleteBox, remove and delete if necessary */
    if (deleteBox != NULL) 
    {
      parentBox = NW_LMgr_Box_GetParent (deleteBox);

      status = NW_LMgr_ContainerBox_RemoveChild(parentBox,
                                                deleteBox);
      NW_Object_Delete(deleteBox);

      deleteBox = NULL;
    }

    /* get into elId the element id we set in the box's properties */
    status = NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_elementId, NW_CSS_ValueType_Integer, &value);

    if (status == KBrsrSuccess) {
      elId = NW_UINT16_CAST(value.integer);
      status = NW_Wml_GetElementType(&(thisObj->wmlInterpreter),
                                     elId,
                                     &elType,
                                     &el);
    }

    /* don't switch on the element type if the return code was not success */
    if (status != KBrsrSuccess)
    {
      status = KBrsrSuccess;   /* reset status to avoid the failure return at the end of the while loop */
    }
    else
    {
      NW_ASSERT(elType != END_ELEMENT);

      switch (elType) {

      case  P_ELEMENT:
        status = NW_UI_SetParagraphProperties(thisObj, elId, box, &nowrap, &deleteBox);

        NW_Wml1x_ParagraphDefaultStyle(&prop, box);

        break;

      case  PRE_ELEMENT:
        /* Set the white-space property */
        prop.type = NW_CSS_ValueType_Token;
        prop.value.token = NW_CSS_PropValue_pre;
        NW_LMgr_Box_SetProperty (box, NW_CSS_Prop_whitespace, &prop);   

        /* Set the font-family to courier (PRE should be rendered
         * as fixed-width 
         */
        prop.type = NW_CSS_ValueType_Text;
        prop.value.object =
          NW_Text_UCS2_New ((NW_Ucs2*) monospace, 0, 0);
        NW_LMgr_Box_SetProperty (box, NW_CSS_Prop_fontFamily, &prop);   

        break;

      case  STRING_ELEMENT:
        status = NW_Wml_GetAttribute(&(thisObj->wmlInterpreter),
                                     elId,
                                     STR_VALUE,
                                     &retString);
        if (status == KBrsrOutOfMemory)
          return status;
        if (retString == NULL) {
          retString = NW_Str_NewcpyConst(" "); /* set some text if none returned */
        }
        if (retString == NULL) {
          return KBrsrOutOfMemory;
        }
        temptext =
          NW_Text_UCS2_New (retString, NW_Str_Strlen(retString), 
                            NW_Text_Flags_TakeOwnership);
        if (temptext == NULL) {
          return KBrsrOutOfMemory;
        }
        NW_LMgr_TextBox_SetText (NW_LMgr_TextBoxOf(box), NW_TextOf(temptext));

        break;

      case  A_ELEMENT:
      case  ANCHOR_ELEMENT:
        {
        NW_Text_t*          retUrl = NULL;
        NW_Bool             inCache = NW_FALSE;
        NW_Bool             isPrev = NW_FALSE;
        
        /* make sure there is some content on the anchor */
        status = NW_Wml1x_FixupAnchorText(&(thisObj->wmlInterpreter ), 
          elId, 
          NW_LMgr_ContainerBoxOf(box),
          docRoot->appServices);
        
        /* get the URL, looking in <go> or <prev> if needed */
        status = NW_Wml1x_GetURL(&(thisObj->wmlInterpreter), 
          elId, 
          &retString,
          &isPrev);
        
        if (isPrev)
          {
          const NW_HED_HistoryEntry_t* entry;
          NW_HED_HistoryStack_t* history;
          NW_HED_HistoryVisitor_t visitor;
          NW_Bool freePrevURL= NW_FALSE;
          
          if ((history = NW_HED_DocumentRoot_GetHistoryStack (docRoot)) != NULL)
            {
            if ((NW_HED_HistoryVisitor_Initialize(&visitor, history, 
              NW_HED_HistoryVisitor_Loacation_Current))  == KBrsrSuccess) 
              {
              entry = NW_HED_HistoryVisitor_Prev(&visitor);
              if(entry != NULL)
                {
                retString = NW_Text_GetUCS2Buffer(entry->urlRequest->url, 
                              NW_Text_Flags_Copy | NW_Text_Flags_NullTerminated,
                              NULL, &freePrevURL);
                }
              }
            }
          }
        if (retString != NULL && *retString != NULL)
          {
          NW_Text_t *url;
          NW_Ucs2* ucs2Href;
          NW_Bool freeNeeded;
          
          url = NW_Text_New (HTTP_iso_10646_ucs_2, retString, 
            NW_Str_Strlen(retString), NW_Text_Flags_TakeOwnership);
          retString = NULL;
          
          if (url == NULL)
            {
            return KBrsrOutOfMemory;
            }
          
          status = NW_HED_ContentHandler_ResolveURL (thisObj, url, &retUrl);
          /* We don't check explicitly for out of memory because if we are
          * out of memory, we will fail later on. All we are interested here
          * is to check if the url is in cache and set the color.
          */
          
          if (status == KBrsrSuccess)
            {
            /* give ownership to url */
            url = retUrl;
            retUrl = NULL;
            
            /* check whether it is in cache */
            ucs2Href = NW_Text_GetUCS2Buffer(url, 
              NW_Text_Flags_Aligned | NW_Text_Flags_NullTerminated,
              NULL, 
              &freeNeeded);
            if (ucs2Href == NULL)
              {
              return KBrsrOutOfMemory;
              }
            else
              {
              NW_Ucs2 *fragment = NW_Url_Fragment(ucs2Href);
              if (fragment != NULL)
                {
                /* strip off the fragment before looking in cache */
                --fragment;
                *fragment = 0;
                }
              inCache = UrlLoader_IsUrlInCache(ucs2Href);
              }
            if (freeNeeded)
              {
              NW_Mem_Free(ucs2Href);
              }
            } 
          NW_Object_Delete(url);
          }
		  else
		  {
			  NW_Str_Delete(retString);
		  }
        /* apply default style for anchor */
        status = NW_Wml1x_AnchorDefaultStyle(&prop, NW_LMgr_ActiveContainerBoxOf (box),
          inCache);
        
        
        /* get the ACCESSKEY_ATTR */
        status = NW_Wml_GetAttribute(&(thisObj->wmlInterpreter), 
          elId, 
          ACCESSKEY_ATTR,
          &retString);
          if (status == KBrsrOutOfMemory)
            return status;
        
        /* convert string to NW_Text_Length_t in inputSize */
        if (retString != NULL) 
          {
          NW_LMgr_AccessKey_t* ak;
          NW_LMgr_Property_t accesskeyProp;
          
          ak = NW_LMgr_AccessKey_New();
          if (ak){
            if (NW_LMgr_AccessKey_AddVal(ak, retString, NW_FALSE) == KBrsrSuccess)
              {                              
              /* set accesskey on the new box */
              accesskeyProp.type = NW_CSS_ValueType_Object;
              accesskeyProp.value.object = ak;
              NW_LMgr_Box_SetProperty(box, NW_CSS_Prop_accesskey, &accesskeyProp); 
              }
            else{
              NW_Object_Delete(ak);
              }
            }            
          NW_Str_Delete(retString);
          retString = NULL;
          if (!ak){
            return KBrsrOutOfMemory;
            }
          }
        }
        break;

      case  IMAGE_ELEMENT:
        {
         NW_Text_t         *altStr = NULL;

          /* Adding a text box with alternative text from "alt" attribute */
          if (IsThereAltAttrib(thisObj, elId, &altStr)) {
            NW_ASSERT(NW_Object_IsDerivedFrom(box, &NW_LMgr_ImgContainerBox_Class));
            (NW_LMgr_ImgContainerBoxOf(box))->altText = altStr;
          }
        }

        status = NW_UI_LoadImage(thisObj, elId, NW_FALSE);

        /* If the status is not KBrsrOutOfMemory, then just ignore it and go
         * onto the next item. Failure to load an image is not fatal*/
        if(status != KBrsrOutOfMemory)
          status = KBrsrSuccess;
        break;

      case  DO_ELEMENT:
        status = NW_Wml1x_HandleDo(thisObj, elId, box, &deleteBox);
        break;

      case  INPUT_ELEMENT:
        /* the input element is currently represented by a container box (although the 
         * input element should have no children)
         * if the attribute type="password", replace the container box with a password box
         * otherwise replace the container box with an input box
         */
        {
          NW_Wml1x_EventHandler_t *eventHandler;
          NW_Uint32               elIdAsWord      = elId;               /* NOTE: this exists solely to eliminate a compiler warning */
          void                    *elIdAsVoidStar = (void *)elIdAsWord; /* NOTE: this exists solely to eliminate a compiler warning */
          NW_LMgr_Box_t           *inputBox = NULL;
          NW_LMgr_Box_t           *newBox = NULL;
          NW_Uint16               inputSize = 0;
          NW_Int32                tabIndexVal;


          /* first, make sure that box is a plain vanilla container box
           * if this is not true, then ..ShowCard() has been called outside
           * the expected sequence of ..CreateCard() - [..AddElement()]* - ..ShowCard() - ..DestroyCard()
           */
          NW_ASSERT (&NW_LMgr_ContainerBox_Class == ((NW_Object_Core_t*) box)->objClass);

          eventHandler =
            NW_Wml1x_EventHandler_New (NW_Wml1x_ContentHandlerOf(thisObj));

          if (eventHandler != NULL) {

            /* get the SIZE_ATTR value if any */
            status = NW_Wml_GetAttribute(&(thisObj->wmlInterpreter), 
                                         elId, 
                                         SIZE_ATTR,
                                         &retString);
            if (status == KBrsrOutOfMemory)
              return status;

            /* convert string to NW_Text_Length_t in inputSize */
            if (retString != NULL) 
            {
							if(NW_Str_StrIsValidLength(retString))
							{
								inputSize = (NW_Uint16) NW_Str_Atoi(retString);
							}
              NW_Str_Delete(retString);
              retString = NULL;
            }

            parentBox = NW_LMgr_Box_GetParent (box);

            status = NW_Wml_GetAttribute(&(thisObj->wmlInterpreter), 
                                         elId, 
                                         TYPE_ATTR,
                                         &retString);
            if (status == KBrsrOutOfMemory)
              return status;

            if ((retString != NULL) && (NW_Str_StrcmpConst(retString, WAE_ASC_PASSWORD_STR) == 0) )
            {
              /* create the passwordBox */
              inputBox = (NW_LMgr_Box_t*)
                NW_FBox_PasswordBox_New(0,
                                       NW_LMgr_EventHandlerOf (eventHandler),
                                       elIdAsVoidStar,  /* elementNode, */
                                       NW_FBox_FormLiaisonOf (thisObj->formLiaison),
                                       inputSize,
                                       docRoot->appServices);
              if (inputBox == NULL) {
                NW_Str_Delete(retString);
                return KBrsrOutOfMemory;
              }
               
              newBox =  inputBox;
            }
            else
            {
              inputBox = (NW_LMgr_Box_t*)
                NW_FBox_InputBox_New(0,
                                     NW_LMgr_EventHandlerOf (eventHandler),
                                     elIdAsVoidStar,  /* elementNode, */
                                     NW_FBox_FormLiaisonOf (thisObj->formLiaison),
                                     inputSize,
                                     docRoot->appServices);  
              newBox = inputBox;

            }
            NW_Str_Delete(retString);
            retString = NULL;

            if (inputBox == NULL) {
              return KBrsrOutOfMemory;
            }

            /* get the ACCESSKEY_ATTR */
            status = NW_Wml_GetAttribute(&(thisObj->wmlInterpreter), 
                                         elId, 
                                         ACCESSKEY_ATTR,
                                         &retString);
            if (status == KBrsrOutOfMemory)
              return status;
            /* convert string to NW_Text_Length_t in inputSize */
            if (retString != NULL) 
            {
              NW_LMgr_AccessKey_t* ak;
              NW_LMgr_Property_t accesskeyProp;

              ak = NW_LMgr_AccessKey_New();
              if (ak){
                if (NW_LMgr_AccessKey_AddVal(ak, retString, NW_FALSE) == KBrsrSuccess)
                {                              
                  /* set accesskey on the new box */
                  accesskeyProp.type = NW_CSS_ValueType_Object;
                  accesskeyProp.value.object = ak;
                  NW_LMgr_Box_SetProperty(newBox, NW_CSS_Prop_accesskey, &accesskeyProp); 
                }
                else{
                  NW_Object_Delete(ak);
                }
              }
              NW_Str_Delete(retString);
              retString = NULL;
              if (!ak){
                return KBrsrOutOfMemory;
              }
            }

            /* get the TITLE_ATTR value if any */
            retString = NULL;
            status = NW_Wml_GetAttribute(&(thisObj->wmlInterpreter), 
                                         elId, 
                                         TITLE_ATTR,
                                         &retString);
            if (status == KBrsrOutOfMemory)
              return status;
            NW_FBox_InputBox_SetTitle(inputBox, retString);
              
            retString = NULL;
            /* get the MAXLENGTH_ATTR value if any */
            status = NW_Wml_GetAttribute(&(thisObj->wmlInterpreter), 
                                         elId, 
                                         MAXLENGTH_ATTR,
                                         &retString);
            if (status == KBrsrOutOfMemory)
              return status;
            if (retString != NULL) 
            {
							NW_Int32 maxlength = -1; /* Initialize to invalid length */
							if(NW_Str_StrIsValidLength(retString))
							{
								maxlength = NW_Str_Atoi(retString);
							}
              NW_Str_Delete(retString);
              retString = NULL;
							if(maxlength >= 0)
							{
								NW_FBox_InputBox_t *tmpbox = NW_FBox_InputBoxOf(inputBox);
								NW_FBox_InputBox_SetMaxChars(tmpbox,(NW_Text_Length_t)maxlength);
								NW_FBox_InputBox_SetIsMaxlength( tmpbox, NW_TRUE);
							}
            }

            /* Create a prefix string validator for the input box. */
            /* get the EMPTYOK_ATTR value if any */
            status = NW_Wml_GetAttribute(&(thisObj->wmlInterpreter), 
                                         elId, 
                                         EMPTYOK_ATTR,
                                         &retString);
            if (status == KBrsrOutOfMemory)
              return status;   
            //Allow scrolling on input boxes when empty. If it is not allowed then
            //let the EMPTYOK_ATTR decides this.
            emptyOk = NW_FBox_Validator_EmptyOk_True;

            if ( retString != NULL ) {
              if ( NW_Str_StricmpConst( retString, WAE_ASC_TRUE_STR ) == 0 ) {
                emptyOk = NW_FBox_Validator_EmptyOk_True;
              } else if ( NW_Str_StricmpConst( retString, WAE_ASC_FALSE_STR ) == 0 ) {
                emptyOk = NW_FBox_Validator_EmptyOk_False;
              }
              NW_Str_Delete(retString);
              retString = NULL;
            }

            /* get the FORMAT_ATTR value if any */
            status = NW_Wml_GetAttribute(&(thisObj->wmlInterpreter), 
                                         elId, 
                                         FORMAT_ATTR,
                                         &retString);
            if (status == KBrsrOutOfMemory)
              return status;
            if ( retString == NULL ) {
              /* Not format attribute.  Create an empty string. */
              formatText = (NW_Text_t*) NW_Text_UCS2_New(NULL, 0, 0); 
            } else {
              /* Is a format attribute. Convert to a Text object. */
              formatText = (const NW_Text_t*) NW_Text_UCS2_New(retString, 
                                                               NW_Str_Strlen(retString),
                                                               NW_Text_Flags_TakeOwnership);
              retString = NULL;
            }

            validator = NW_Markup_WmlValidator_New( formatText, 
                                                    NW_FBox_Validator_Mode_None,
                                                    emptyOk,
                                                    NW_FALSE);  /* Do a prefix match. */
            NW_Object_Delete((NW_Text_t*) formatText);

            /* Input box takes ownership of validator. */
            NW_FBox_InputBox_SetValidator( inputBox, 
                                           NW_FBox_ValidatorOf(validator) );

            /* get the TABINDEX_ATTR value if any */
            status = NW_Wml_GetAttribute(&(thisObj->wmlInterpreter), 
                                         elId, 
                                         TABINDEX_ATTR,
                                         &retString);
            if (status == KBrsrOutOfMemory)
              return status;
            /* convert string to NW_Text_Length_t in inputSize */
            if (retString != NULL) 
            {
              tabIndexVal =  NW_Str_Atoi(retString);
              NW_Str_Delete(retString);
              retString = NULL;

              /* set tabIndex on the new box */
               prop.type = NW_CSS_ValueType_Integer;
               prop.value.integer = (tabIndexVal > 0) ? tabIndexVal : 0;
               NW_LMgr_Box_SetProperty(inputBox, NW_CSS_Prop_tabIndex, &prop); 
            }

            parentBox = NW_LMgr_Box_GetParent (box);

            status = NW_LMgr_ContainerBox_InsertChild(parentBox,
                                                      newBox,
                                                      box);
            if (status != KBrsrSuccess) {
              NW_Object_Delete (newBox);
              return status;
            }

            /* removing box here causes problems for the BoxVisitor, 
             * since that object maintains a "currentBox" pointer which 
             * at this point is pointing to this box.  The alternative
             * is to set a pointer to the box to be deleted and then
             * remove from parent and delete after BoxVisitor has moved on.
             */
            deleteBox = box;

            /* replace the element Id on the new box */
            prop.type = NW_CSS_ValueType_Integer;
            value.integer = elId;
            prop.value = value;
            NW_LMgr_Box_SetProperty(inputBox, NW_CSS_Prop_elementId, &prop);   

            NW_Wml1x_InputDefaultStyle(inputBox);

          }
        }
        break;

      case TABLE_ELEMENT:

        status = NW_UI_NormalizeTable(thisObj, elId, box);
        if (status != KBrsrSuccess) {
          return status;
        }
        status = NW_UI_SetCellAlignment(thisObj, elId, box);
        if (status != KBrsrSuccess) {
          return status;
        }
        break;

      case  TD_ELEMENT:
        status = NW_UI_SetCellProperties(box);
        if (status != KBrsrSuccess) {
          return status;
        }
        break;

      case  SELECT_ELEMENT:
        status = NW_Wml1x_HandleSelect(thisObj,
                                      elId,
                                      box,
                                      &deleteBox,
                                      &currentSelectMultipleState,
                                      &currentSelectTabIndexVal);
        break;

      case  OPTGRP_ELEMENT:
        status = NW_Wml1x_HandleOptgroup(thisObj,
                                        elId,
                                        box);
        break;

      case  OPTION_ELEMENT:
        status = NW_Wml1x_HandleOption(thisObj,
                                      elId,
                                      box,
                                      currentSelectMultipleState,
                                      currentSelectTabIndexVal);
        break;

      case  FIELDSET_ELEMENT:
        /* The <fieldset> element is currently represented by a 
         * BidiFlowBox, and will (hopefully) have had one or more <input> 
         * children added. If the element has a TITLE attribute, create a text box
         * with the TITLE text and insert it as the first child of the BidiFlowBox
         */

        status = NW_Wml_GetAttribute(&(thisObj->wmlInterpreter),
                                     elId,
                                     TITLE_ATTR,
                                     &retString);
        if (status == KBrsrOutOfMemory)
           return status;
        if (retString != NULL) 
        {
          NW_LMgr_Box_t *titleBox;
          NW_LMgr_Box_t *breakBox;

          /* make a text object containing the title */
          temptext =
            NW_Text_UCS2_New (retString, NW_Str_Strlen(retString),
                              NW_Text_Flags_TakeOwnership);
          if (temptext == NULL) {
            return KBrsrOutOfMemory; /* NW_Text_UCS2_New() takes care of deleting string when fails */
          }

          titleBox = (NW_LMgr_Box_t*) NW_LMgr_TextBox_New(1, NW_TextOf(temptext));
          breakBox = (NW_LMgr_Box_t*)NW_LMgr_BreakBox_New(0);

          if (titleBox != NULL) 
          {
            if (NW_LMgr_ContainerBox_GetChildCount(NW_LMgr_ContainerBoxOf (box)) > 0)
            {
              if (breakBox){
                status = NW_LMgr_ContainerBox_InsertChildAt(NW_LMgr_ContainerBoxOf (box),
                                                            breakBox, 0);
              }
              status = NW_LMgr_ContainerBox_InsertChildAt(NW_LMgr_ContainerBoxOf (box),
                                                        titleBox, 0);
            }
            else
            {
              /* no children: just add the title box */
              status = NW_LMgr_ContainerBox_AddChild( NW_LMgr_ContainerBoxOf (box),
                                                      titleBox);
              if (breakBox){
                status = NW_LMgr_ContainerBox_AddChild(NW_LMgr_ContainerBoxOf (box),
                                                          breakBox);
              }

            }
            if (status != KBrsrSuccess) {
              return status;
            }
          }
          else 
          {
            NW_Object_Delete(temptext);
            return KBrsrOutOfMemory;
          }
        }
        NW_Wml1x_FieldsetDefaultStyle(box);
        break;

      case  STRONG_ELEMENT:
        /* strong default */
        NW_Wml1x_StrongDefaultStyle(&prop, NW_LMgr_ContainerBoxOf (box));
        break;

      case  BIG_ELEMENT:
        /* big default */
        NW_Wml1x_BigDefaultStyle(&prop, NW_LMgr_ContainerBoxOf (box));
        break;

      case  ITALIC_ELEMENT:
        /* italic default */
        NW_Wml1x_ItalicDefaultStyle(&prop, NW_LMgr_ContainerBoxOf (box));
        break;

      case  EMPHASIS_ELEMENT:
        /* emphasis default */
        NW_Wml1x_EmphasisDefaultStyle(&prop, NW_LMgr_ContainerBoxOf (box));
        break;

      case  BOLD_ELEMENT:
        /* bold default */
        NW_Wml1x_BoldDefaultStyle(&prop, NW_LMgr_ContainerBoxOf (box));
        break;

      case  UNDERLINE_ELEMENT:
        /* underline default */
        NW_Wml1x_UnderlineDefaultStyle(&prop, NW_LMgr_ContainerBoxOf (box));
        break;

      case  SMALL_ELEMENT:
        /* small default */
        NW_Wml1x_SmallDefaultStyle(&prop, NW_LMgr_ContainerBoxOf (box));
        break;

      default:
        break;
      }

      if (status != KBrsrSuccess) {
        return status;
      }
    }
  }
    /* finally we inform the document tree that our content has changed */

  parent = NW_HED_DocumentNode_GetParentNode(wae);

  status =
    NW_HED_DocumentNode_NodeChanged (wae, parent);

  return status;
}


/*****************************************************************

  Name:         NW_UI_DestroyCard()

  Description:  callback for WML interpreter to destroy card

  Parameters:
    wae     - In      - the content handler on behalf of which the work is being done

  Algorithm:

  Return Value:

*****************************************************************/
static TBrowserStatusCode NW_UI_DestroyCard (void *wae)
{
  NW_Wml1x_ContentHandler_t *thisObj;
  NW_HED_CompositeNode_t* compositeNode;

  thisObj = NW_Wml1x_ContentHandlerOf(wae);
  compositeNode = (NW_HED_CompositeNode_t*) 
    NW_Object_QueryAggregate (thisObj, &NW_HED_CompositeNode_Class);
  NW_ASSERT (compositeNode != NULL);

/*
** leak the box tree since I don't know how to free script content handler
** as child AND free the box tree without causing double delete of image
** content handler which gets freed BOTH in boxTree delete and DeleteChildren.


  if (NW_HED_ContentHandlerOf(thisObj)->boxTree != NULL) {
    NW_Object_Delete( NW_HED_ContentHandlerOf(thisObj)->boxTree );
    NW_HED_ContentHandlerOf(thisObj)->boxTree = NULL;
  }
*/

  if (thisObj->optionMap != NULL) {
    NW_Object_Delete(thisObj->optionMap);
    thisObj->optionMap = NULL;
  }
  /* free up all our child content handlers; eg:-images */
  NW_HED_CompositeNode_DeleteChildren(compositeNode);

  /* succesful completion */
  return KBrsrSuccess;
}


/*****************************************************************

  Name:         NW_UI_AddStringElement()

  Description:  called by NW_UI_AddElement() for text

  Parameters:
    thisObj   - In      - the content handler on behalf of which the work is being done
    newboxptr - In/Out  - pointer to variable which will receive address of new box obj

  Algorithm:    create the text box; actual text will be attached
                  by NW_UI_ShowCard()

  Return Value:

*****************************************************************/
static TBrowserStatusCode NW_UI_AddStringElement (NW_Wml1x_ContentHandler_t  *thisObj,
                                           NW_LMgr_Box_t              **newboxptr)
{
  NW_LMgr_TextBox_t* box;

  NW_TRY (status) {
    box = NW_LMgr_TextBox_New(1, NULL);
    NW_THROW_OOM_ON_NULL (box, status);

    status = NW_LMgr_ContainerBox_AddChild(NW_LMgr_ContainerBoxOf(thisObj->currentBox),
                                           NW_LMgr_BoxOf(box));
    _NW_THROW_ON_ERROR (status);

    *newboxptr = NW_LMgr_BoxOf(box);
  }
  NW_CATCH (status) {
    NW_Object_Delete (box);
  }
  NW_FINALLY {
    return status;
  } NW_END_TRY
}


/*****************************************************************

  Name:         NW_UI_AddFlowElement()

  Description:  called by NW_UI_AddElement() for P, PRE, TD,
                  SELECT, OPTGRP, and FIELDSET elements

  Parameters:
    thisObj   - In      - the content handler on behalf of which the work is being done
    newboxptr - In/Out  - pointer to variable which will receive address of new box obj

  Algorithm:

  Return Value:

*****************************************************************/
static TBrowserStatusCode NW_UI_AddFlowElement (NW_Wml1x_ContentHandler_t  *thisObj,
                                         NW_LMgr_Box_t              **newboxptr)
{
  NW_LMgr_BidiFlowBox_t*  box;
  NW_LMgr_Property_t    prop;

  NW_TRY (status) {
    box = NW_LMgr_BidiFlowBox_New(2);
    NW_THROW_OOM_ON_NULL (box, status);

    prop.type = NW_CSS_ValueType_Token;
    prop.value.token = NW_CSS_PropValue_display_block;
    NW_LMgr_Box_SetProperty(NW_LMgr_BoxOf(box), NW_CSS_Prop_display, &prop);

    status = NW_LMgr_ContainerBox_AddChild(NW_LMgr_ContainerBoxOf(thisObj->currentBox),
                                           NW_LMgr_BoxOf(box));
    _NW_THROW_ON_ERROR (status);
    
    *newboxptr = NW_LMgr_BoxOf(box);
  }
  NW_CATCH (status) {
    NW_Object_Delete (box);
  }
  NW_FINALLY {
    return status;
  } NW_END_TRY
}


/*****************************************************************

  Name:         NW_UI_AddStyleElement()

  Description:  called by NW_UI_AddElement() for EM, B, BIG,
                  STRONG, I, SMALL, and U elements

  Parameters:
    thisObj   - In      - the content handler on behalf of which the work is being done
    newboxptr - In/Out  - pointer to variable which will receive address of new box obj
    elemType  - In      - token value of element being processed

  Algorithm:

  Return Value:

*****************************************************************/
static TBrowserStatusCode NW_UI_AddStyleElement (NW_Wml1x_ContentHandler_t *thisObj,
                                          NW_LMgr_Box_t             **newboxptr,
                                          NW_Wml_ElType_e           elemType)
{
  NW_LMgr_ContainerBox_t*   containerBox;
  
  NW_REQUIRED_PARAM(elemType);

  NW_TRY (status) {
    /* add a raw container box then add style property to the container */
    containerBox = NW_LMgr_ContainerBox_New(2); /* 2: style property and element Id */
    NW_THROW_OOM_ON_NULL (containerBox, status);

    /* process the elemType */
    status = NW_LMgr_ContainerBox_AddChild (NW_LMgr_ContainerBoxOf(thisObj->currentBox),
                                              NW_LMgr_BoxOf (containerBox));
    _NW_THROW_ON_ERROR (status);
    *newboxptr = NW_LMgr_BoxOf(containerBox);
  }
  NW_CATCH (status) {
    NW_Object_Delete (containerBox);
  }
  NW_FINALLY {
    return status;
  } NW_END_TRY
}



/*****************************************************************

  Name:         NW_UI_AddActiveElement()

  Description:  called by NW_UI_AddElement() for A elements

  Parameters:
    thisObj   - In      - the content handler on behalf of which the work is being done
    newboxptr - In/Out  - pointer to variable which will receive address of new box obj
    elemType  - In      - token value of element being processed

  Algorithm:

  Return Value:

*****************************************************************/
static TBrowserStatusCode NW_UI_AddActiveElement (NW_Wml1x_ContentHandler_t  *thisObj,
                                           NW_LMgr_Box_t              **newboxptr,
                                           NW_Wml_ElType_e            elemType)
{
  NW_Wml1x_EventHandler_t*  eventHandler;
  NW_LMgr_Box_t             *activeBox = NULL;

  NW_REQUIRED_PARAM(elemType);

  NW_TRY (status) {
    /* create the eventHandler for the active box */
    eventHandler =
      NW_Wml1x_EventHandler_New (NW_Wml1x_ContentHandlerOf(thisObj));
    NW_THROW_OOM_ON_NULL(eventHandler, status);

    /* create the activeBox */
    switch (elemType)
    {
      case A_ELEMENT:
      case ANCHOR_ELEMENT:
        /* create a ActiveContainerBox to hold the active element and children*/
        activeBox = (NW_LMgr_Box_t*)NW_LMgr_ActiveContainerBox_New(
            0, NW_LMgr_EventHandlerOf (eventHandler), NW_LMgr_ActionType_OpenLink);
        NW_THROW_OOM_ON_NULL(activeBox, status);
        break;

      default:
        /* elemType must be handled before dropping through */
        NW_ASSERT(0);
        break;
    }

    status = NW_LMgr_ContainerBox_AddChild (NW_LMgr_ContainerBoxOf(thisObj->currentBox),
                                              NW_LMgr_BoxOf (activeBox));
    _NW_THROW_ON_ERROR (status);
    *newboxptr = NW_LMgr_BoxOf(activeBox);
  }
  NW_CATCH (status) {
    if(activeBox == NULL) {
      NW_Object_Delete (eventHandler);
    }
    NW_Object_Delete (activeBox);
  }
  NW_FINALLY {
    return status;
  } NW_END_TRY
}


/*****************************************************************

  Name:         NW_UI_AddContainerElement()

  Description:  called by NW_UI_AddElement() for DO, INPUT and 
                  OPTION elements

  Parameters:
    thisObj   - In      - the content handler on behalf of which the work is being done
    newboxptr - In/Out  - pointer to variable which will receive address of new box obj

  Algorithm:

  Return Value:

*****************************************************************/
static TBrowserStatusCode NW_UI_AddContainerElement (NW_Wml1x_ContentHandler_t *thisObj,
                                              NW_LMgr_Box_t             **newboxptr)
{
  NW_LMgr_ContainerBox_t  *containerBox;

  NW_TRY (status) {
    /* create the containerBox */
    containerBox = NW_LMgr_ContainerBox_New (1);
    NW_THROW_OOM_ON_NULL(containerBox, status);

    status = NW_LMgr_ContainerBox_AddChild (NW_LMgr_ContainerBoxOf(thisObj->currentBox),
                                              NW_LMgr_BoxOf (containerBox));
    _NW_THROW_ON_ERROR(status)
    *newboxptr = NW_LMgr_BoxOf(containerBox); 
  }

  NW_CATCH (status) {
    NW_Object_Delete (containerBox);
  }
  NW_FINALLY {
    return status;
  } NW_END_TRY
}


/*****************************************************************

  Name:         NW_UI_AddBreakElement()

  Description:  called by NW_UI_AddElement() for BR element

  Parameters:
    thisObj   - In      - the content handler on behalf of which the work is being done
    newboxptr - In/Out  - pointer to variable which will receive address of new box obj

  Algorithm:

  Return Value:

*****************************************************************/
static TBrowserStatusCode NW_UI_AddBreakElement (NW_Wml1x_ContentHandler_t *thisObj,
                                          NW_LMgr_Box_t             **newboxptr)
{
  NW_LMgr_Box_t*        box;

  NW_TRY (status) {
    box = (NW_LMgr_Box_t*)NW_LMgr_BreakBox_New((NW_ADT_Vector_Metric_t)1);
    NW_THROW_OOM_ON_NULL(box, status);

    status = NW_LMgr_ContainerBox_AddChild(NW_LMgr_ContainerBoxOf(thisObj->currentBox),
                                           NW_LMgr_BoxOf(box));
    _NW_THROW_ON_ERROR(status);
    *newboxptr = NW_LMgr_BoxOf(box);
  }
  NW_CATCH (status) {
    NW_Object_Delete (box);
  }
  NW_FINALLY {
    return status;
  } NW_END_TRY
}


/*****************************************************************

  Name:         NW_UI_AddImageElement()

  Description:  called by NW_UI_AddElement() for IMG element

  Parameters:
    thisObj   - In      - the content handler on behalf of which the work is being done
    newboxptr - In/Out  - pointer to variable which will receive address of new box obj

  Algorithm:

  Return Value:

*****************************************************************/
static TBrowserStatusCode NW_UI_AddImageElement (NW_Wml1x_ContentHandler_t *thisObj,
                                          NW_LMgr_Box_t             **newboxptr)
{
  NW_LMgr_Property_t            prop;
  NW_Image_Virtual_t* virtualImage = NULL;

  NW_TRY (status) 
  {
    /* as we have not downloaded the image, we create a
       substitute ImageBox in which we display our 'missing' image*/
    NW_HED_DocumentRoot_t* docRoot = NULL;
    NW_LMgr_RootBox_t* rootBox = NULL;
    MHEDDocumentListener* documentListener = NULL;
    NW_Image_AbstractImage_t* cannedImage = NULL;

    docRoot = (NW_HED_DocumentRoot_t*)NW_HED_DocumentNode_GetRootNode( thisObj );
  	NW_ASSERT( docRoot != NULL );

    documentListener = docRoot->documentListener;
  	NW_ASSERT( documentListener != NULL );

    rootBox = documentListener->GetRootBox();
    NW_ASSERT( rootBox != NULL );
    NW_ASSERT( rootBox->cannedImages != NULL );

    cannedImage = (NW_Image_AbstractImage_t*)
        NW_Image_CannedImages_GetImage( rootBox->cannedImages, NW_Image_Missing );
    NW_THROW_OOM_ON_NULL( cannedImage, status );

    virtualImage = NW_Image_Virtual_New( cannedImage );
    NW_THROW_OOM_ON_NULL( virtualImage, status );
    // pass image ownership
    *newboxptr = (NW_LMgr_Box_t*)NW_LMgr_AnimatedImageBox_New(0, NW_Image_AbstractImageOf( virtualImage ), NULL, NW_TRUE );
    NW_THROW_OOM_ON_NULL(*newboxptr, status);
    // image container takes image ownership
    virtualImage = NULL;
    
    status = NW_LMgr_ContainerBox_AddChild (NW_LMgr_ContainerBoxOf(thisObj->currentBox),
                                            *newboxptr);
    _NW_THROW_ON_ERROR(status);
      
    NW_Wml1x_ImageWithinAnchorDefaultStyle(&prop, *newboxptr);

  }
  NW_CATCH (status) {
    NW_Object_Delete (*newboxptr);
    NW_Object_Delete (virtualImage);
  }
  NW_FINALLY {
    return status;
  } NW_END_TRY
}


/*****************************************************************

  Name:         NW_UI_AddTableElement()

  Description:  called by NW_UI_AddElement() for TABLE element

  Parameters:
    thisObj   - In      - the content handler on behalf of which the work is being done
    newboxptr - In/Out  - pointer to variable which will receive address of new box obj

  Algorithm:

  Return Value:

*****************************************************************/
static TBrowserStatusCode NW_UI_AddTableElement (NW_Wml1x_ContentHandler_t *thisObj,
                                          NW_LMgr_Box_t             **newboxptr)
{
  NW_Bool ownsSTB = NW_TRUE;
  NW_LMgr_Box_t *stb = NULL;

  NW_TRY (status) {
    NW_LMgr_Property_t prop;

    /* Allocate and initialize the table itself */
    stb = (NW_LMgr_Box_t*)NW_LMgr_StaticTableBox_New(1);
    NW_THROW_OOM_ON_NULL (stb, status);

    /* Set the display prop */
    prop.type = NW_CSS_ValueType_Token;
    prop.value.token = NW_CSS_PropValue_display_block;
    NW_LMgr_Box_SetProperty (stb, NW_CSS_Prop_display, &prop);

    status = NW_LMgr_ContainerBox_AddChild(NW_LMgr_ContainerBoxOf(thisObj->currentBox),
                                            stb);
    _NW_THROW_ON_ERROR(status);
    ownsSTB = NW_FALSE;

    *newboxptr = stb;

  } NW_CATCH (status) {
  } NW_FINALLY {
    if (ownsSTB) {
      NW_Object_Delete (stb);
    }
    return status;
  } NW_END_TRY
}


/*****************************************************************

  Name:         NW_UI_AddTrElement()

  Description:  called by NW_UI_AddElement() for TR element

  Parameters:
    thisObj   - In      - the content handler on behalf of which the work is being done
    newboxptr - In/Out  - pointer to variable which will receive address of new box obj

  Algorithm:

  Return Value:

*****************************************************************/
static TBrowserStatusCode NW_UI_AddTrElement (NW_Wml1x_ContentHandler_t  *thisObj,
                                       NW_LMgr_Box_t              **newboxptr)
{
  NW_LMgr_Box_t* box = NULL;

  NW_TRY (status) {
    box = (NW_LMgr_Box_t*) NW_LMgr_StaticTableRowBox_New (1);
    NW_THROW_OOM_ON_NULL (box, status);

    /* Add the row to the box tree */
    status = NW_LMgr_ContainerBox_AddChild (NW_LMgr_ContainerBoxOf (thisObj->currentBox),
                                            box);
    NW_THROW_ON_ERROR (status);
    *newboxptr = box;

  } NW_CATCH (status) {
    NW_Object_Delete (box);
  } NW_FINALLY {
    return status;
  } NW_END_TRY
}

/*****************************************************************

  Name:         NW_UI_AddTdElement()

  Description:  called by NW_UI_AddElement() for TD element

  Parameters:
    thisObj   - In      - the content handler on behalf of which the work is being done
    newboxptr - In/Out  - pointer to variable which will receive address of new box obj

  Algorithm:

  Return Value:

*****************************************************************/
static TBrowserStatusCode NW_UI_AddTdElement (NW_Wml1x_ContentHandler_t  *thisObj,
                                       NW_LMgr_Box_t              **newboxptr)
{
  NW_LMgr_Box_t* box = NULL;

  NW_TRY (status) {
    box = (NW_LMgr_Box_t*)NW_LMgr_StaticTableCellBox_New(1);
    NW_THROW_OOM_ON_NULL (box, status);

    /* Add the row to the box tree */
    status = NW_LMgr_ContainerBox_AddChild (NW_LMgr_ContainerBoxOf (thisObj->currentBox),
                                            box);
    NW_THROW_ON_ERROR (status);
    *newboxptr = box;

  } NW_CATCH (status) {
    NW_Object_Delete (box);
  } NW_FINALLY {
    return status;
  } NW_END_TRY
}


/*****************************************************************

  Name:         NW_UI_AddUnknownElement()

  Description:  called by NW_UI_AddElement() when element is not
                  recognized

  Parameters:
    thisObj   - In      - the content handler on behalf of which the work is being done
    newboxptr - In/Out  - pointer to variable which will receive address of new box obj

  Algorithm:    create a basic container box for the element so
                  that any content may be properly rendered

  Return Value:

*****************************************************************/
static TBrowserStatusCode NW_UI_AddUnknownElement (NW_Wml1x_ContentHandler_t *thisObj,
                                            NW_LMgr_Box_t             **newboxptr)
{
  NW_LMgr_ContainerBox_t* box;

  NW_TRY (status) {
    box = NW_LMgr_ContainerBox_New(1);
    NW_THROW_OOM_ON_NULL (box, status);

    status = NW_LMgr_ContainerBox_AddChild(NW_LMgr_ContainerBoxOf(thisObj->currentBox),
                                           NW_LMgr_BoxOf(box));
    _NW_THROW_ON_ERROR (status);
    *newboxptr = NW_LMgr_BoxOf(box);
  }
  NW_CATCH(status) {
    NW_Object_Delete (box);
  }
  NW_FINALLY {
    if(status != KBrsrOutOfMemory) {
      status = KBrsrSuccess;
    }
    return status;
  } NW_END_TRY
}


/*****************************************************************

  Name:         NW_Element_endTagRendered()

  Description:  determines whether an explicit end-tag will be or
                  will have been generated by RenderCardElements()
                  for a given elemType.

  Parameters:
    elemType  - In      - token value of element being processed

  Algorithm:
    (TODO: This probably ought to be in a different module,
    like wml_ui.c)

  Return Value:

*****************************************************************/
NW_Bool NW_Element_endTagRendered (NW_Wml_ElType_e elemType)
{
  switch (elemType)
  {
  /* Elements for which RenderCardElements() in wml_ui.c generates end-tag: */
  case A_ELEMENT:
  case ANCHOR_ELEMENT:
  case EMPHASIS_ELEMENT:
  case STRONG_ELEMENT:
  case ITALIC_ELEMENT:
  case BOLD_ELEMENT:
  case UNDERLINE_ELEMENT:
  case BIG_ELEMENT:
  case SMALL_ELEMENT:
  case SELECT_ELEMENT:
  case P_ELEMENT:
  case FIELDSET_ELEMENT:
  case OPTGRP_ELEMENT:
  case TABLE_ELEMENT:
  case TR_ELEMENT:
  case TD_ELEMENT:
  case OPTION_ELEMENT:
  case PRE_ELEMENT:
    return NW_TRUE;

  /* Elements for which RenderCardElements() in wml_ui.c does not generate end-tag: */
  case DO_ELEMENT:
  case INPUT_ELEMENT:
  case BREAK_ELEMENT:
  case IMAGE_ELEMENT:
  case STRING_ELEMENT:
  case END_ELEMENT:
    return NW_FALSE;

  /* Elements not expected from RenderCardElements() in wml_ui.c */
  case CARD_ELEMENT:
  case GO_ELEMENT:
  case PREV_ELEMENT:
  case REFRESH_ELEMENT:
  case NOOP_ELEMENT:
  case SETVAR_ELEMENT:
  case ONEVENT_ELEMENT:
  case WML_ELEMENT:
  case HEAD_ELEMENT:
  case TEMPLATE_ELEMENT:
  case TIMER_ELEMENT:
  case ACCESS_ELEMENT:
  case META_ELEMENT:
  case POSTFIELD_ELEMENT:

  /* All known WML-tags should be handled explicitly in the switch */
  default:
    NW_ASSERT(NW_FALSE);
    return NW_FALSE;
  }
}

/*****************************************************************

  Name:         NW_UI_AddElement()

  Description:  callback for WML interpreter to add card elements

  Parameters:
    wae       - In      - the content handler on behalf of which the work is being done
    elemType  - In      - token value of element being processed
    elemId    - In/Out  - pointer to variable which will receive unique 
                    identifying value to be associated with this element 
                    for later use with the WML query service to obtain
                    attribute values

  Algorithm:    for element passed in, create the box on the box
                  tree for later rendering, and assign a unique
                  id number to the element

  Return Value:

*****************************************************************/
static TBrowserStatusCode NW_UI_AddElement (void             *wae, 
                                     NW_Wml_ElType_e  elemType,
                                     NW_Int16         *elemId)
{
  NW_LMgr_Property_t            prop;
  NW_LMgr_PropertyValue_t       val;
  NW_Wml1x_ContentHandler_t     *ch;
  NW_LMgr_Box_t                 *box = NULL;

  NW_TRY (status) {
    
    ch = NW_Wml1x_ContentHandlerOf(wae);
    *elemId = ch->lastId;

    switch( elemType ) {

    case  END_ELEMENT:
      ch->currentBox = (NW_LMgr_Box_t*) NW_LMgr_Box_GetParent(ch->currentBox);
      status = KBrsrSuccess;
      break;

    case  P_ELEMENT:
    case  PRE_ELEMENT:
    case  SELECT_ELEMENT:
    case  OPTGRP_ELEMENT:
    case  FIELDSET_ELEMENT:
      status = NW_UI_AddFlowElement(ch, &box);
      break;

    case  STRING_ELEMENT:
      status = NW_UI_AddStringElement(ch, &box);
      break;

    case  BREAK_ELEMENT:
      status = NW_UI_AddBreakElement(ch, &box);
      break;

    case  EMPHASIS_ELEMENT:
    case  STRONG_ELEMENT:
    case  ITALIC_ELEMENT:
    case  BOLD_ELEMENT:
    case  UNDERLINE_ELEMENT:
    case  BIG_ELEMENT:
    case  SMALL_ELEMENT:
      status = NW_UI_AddStyleElement(ch, &box, elemType);
      break;

    case  IMAGE_ELEMENT:
      status = NW_UI_AddImageElement(ch, &box);
      break;

    case  OPTION_ELEMENT:
    case  INPUT_ELEMENT:
    case  DO_ELEMENT:
      status = NW_UI_AddContainerElement(ch, &box);
      break;

    case  A_ELEMENT:
    case  ANCHOR_ELEMENT:
      status = NW_UI_AddActiveElement(ch, &box, elemType);
      break;

    case  TABLE_ELEMENT:
      status = NW_UI_AddTableElement(ch, &box);
      break;

    case  TR_ELEMENT:
      status = NW_UI_AddTrElement(ch, &box);
      break;

    case  TD_ELEMENT:
      status = NW_UI_AddTdElement(ch, &box);
      break;

    case  UNKNOWN_ELEMENT:
      status = NW_UI_AddUnknownElement(ch, &box);
      break;

    case  CARD_ELEMENT:
    case  GO_ELEMENT:
    case  PREV_ELEMENT:
    case  REFRESH_ELEMENT:
    case  NOOP_ELEMENT:
    case  SETVAR_ELEMENT:
    case  ONEVENT_ELEMENT:
    case  WML_ELEMENT:
    case  HEAD_ELEMENT:
    case  TEMPLATE_ELEMENT:
    case  TIMER_ELEMENT:
    case  ACCESS_ELEMENT:
    case  META_ELEMENT:
    case  POSTFIELD_ELEMENT:
    default:
      /* TODO: handle unexpected elements: do nothing? */
      NW_ASSERT( 0 );
      break;

    }

    _NW_THROW_ON_ERROR (status);

    if (NW_Element_endTagRendered (elemType) == NW_TRUE) {
      /* push currentBox pointer down to newly-created element box;
          End element will pop it up */
      ch->currentBox = box;
    }
    if ((elemType != END_ELEMENT) && (box != NULL)) {
      prop.type = NW_CSS_ValueType_Integer;
      val.integer = ch->lastId;
      prop.value = val;
      NW_LMgr_Box_SetProperty(box, NW_CSS_Prop_elementId, &prop);
    }
    else if ((elemType != END_ELEMENT) && (box == NULL)) {
    /* TODO:  handle box == NULL, in case Add_x_Element() failed to create a child box */
      NW_ASSERT( 0 );
    }

    ch->lastId += 1;

  /*#ifdef _DEBUG */
  #if 0 /* TODO figure out what to do here */
    {
      /* log the elements */
      NW_Ucs2 *debugStr;
      debugStr = NW_Str_NewcpyConst(elTypeNames[elemType]);
      NW_LOG1(NW_LOG_LEVEL1, "<%s>", debugStr);
      NW_Str_Delete(debugStr);
    }
  #endif
  }
  NW_CATCH (status) {

  }
  NW_FINALLY {
    return status;
  } NW_END_TRY

}

/*****************************************************************

  Name:         NW_UI_CleanUp()

  Description:  callback for WML interpreter

  Parameters:   
    wae       - In      - the content handler on behalf of which the work is being done

  Algorithm:   

  Return Value: KBrsrSuccess

*****************************************************************/
static TBrowserStatusCode NW_UI_CleanUp (void *wae)
{
  NW_REQUIRED_PARAM(wae);
  return KBrsrSuccess;
}

/*****************************************************************

  Name:         NW_UI_SetOptState()

  Description:  callback for WML interpreter to set state of
                  option element

  Parameters:
    wae       - In      - the content handler on behalf of which the work is being done
    elemId    - In      - the element Id of the <option> element affected
    st        - In      - the state to set the <option> element

  Algorithm:    use a map to associate element Id with state

  Return Value: KBrsrFailure iff unable to find or create
                      key entry in map vector;
                KBrsrSuccess otherwise

*****************************************************************/
static TBrowserStatusCode NW_UI_SetOptState (void *wae, NW_Uint16 elemId, NW_Bool st)
{
  return NW_ADT_Map_Set (NW_Wml1x_ContentHandlerOf(wae)->optionMap, &elemId, &st);
}

/*****************************************************************

  Name:         NW_UI_GetOptState()

  Description:  callback for WML interpreter to get state of
                  option element

  Parameters:   
    wae       - In      - the content handler on behalf of which the work is being done
    elemId    - In      - the element Id of the <option> element affected
    st        - In      - pointer to bool in which state will be copied

  Algorithm:    use the map populated during SetOptState
                  to return state associated with element Id
                NOTE that if elemId is not an existing key in
                  the map, *st will be set to NW_FALSE

  Return Value: KBrsrSuccess


*****************************************************************/
static TBrowserStatusCode NW_UI_GetOptState (void *wae, NW_Uint16 elemId, NW_Bool *st)
{
  NW_Bool* boolPtr;
  
  NW_ASSERT( wae != NULL );
  NW_ASSERT( st != NULL );

  /* fetch a pointer to the value from the map */
  /* Returns NULL if item doesn't exist. */
  boolPtr = (NW_Bool*) NW_ADT_Map_GetUnsafe(
                            NW_Wml1x_ContentHandlerOf(wae)->optionMap, 
                            &elemId);

  /* use byte-wise copy to get the unaligned data into the client's variable */
  /* copy the value into the client's variable */
  if ((boolPtr != NULL) && (st != NULL))
  {
    (void) NW_Mem_memcpy (st, boolPtr, sizeof *st);
  }
  else if (st != NULL)
  {
    *st = NW_FALSE;
  }

  return KBrsrSuccess;
}

/*****************************************************************

  Name:         NW_UI_Refresh()

  Description:  callback for WML interpreter

  Parameters:
    wae       - In      - the content handler on behalf of which the work is being done

  Algorithm:

  Return Value:

*****************************************************************/
static TBrowserStatusCode NW_UI_Refresh (void *wae)
{
  /* avoid 'unreferenced formal parameter' warnings */
  (void) wae;

  /* succesful completion */
  return KBrsrSuccess;
}



/*****************************************************************

  Name:         NW_Api_GetWml1xCB()

  Description:

  Parameters:

  Algorithm:

  Return Value:

*****************************************************************/
const NW_WmlApi_t* NW_Api_GetWml1xCB()
{
  return &wml_api;
}


/*===============================Timer API wrappers for WML 1.x content handler================*/

/*****************************************************************

  Name:         GetWmlTimer()

  Description:

  Parameters:

  Algorithm:

  Return Value:

*****************************************************************/
NW_System_Timer_t* GetWmlTimer (void *usrAgent)
{
  NW_Wml1x_ContentHandler_t *thisObj;

  NW_ASSERT(usrAgent!=NULL);
  thisObj = NW_Wml1x_ContentHandlerOf(usrAgent);
  return thisObj->wmlTimer;
}

/*****************************************************************

  Name:         SetWmlTimer()

  Description:

  Parameters:

  Algorithm:

  Return Value:

*****************************************************************/
void SetWmlTimer (void *usrAgent, NW_System_Timer_t* wmlTimer)
{
  NW_Wml1x_ContentHandler_t *thisObj;

  NW_ASSERT(usrAgent!=NULL);
  thisObj = NW_Wml1x_ContentHandlerOf(usrAgent);
  thisObj->wmlTimer = wmlTimer;
}


/*
**-------------------------------------------------------------------------
**  Timer API functions called by the WML Browser
**-------------------------------------------------------------------------
*/

/*****************************************************************

  Name:         NW_Wml1x_CreateTimer()

  Description:

  Parameters:
    usrAgent  - In      - the content handler on behalf of which the work is being done
    period    - In      - timer period

  Algorithm:

  Return Value:

*****************************************************************/
TBrowserStatusCode NW_Wml1x_CreateTimer (void *usrAgent, NW_Uint32 period)
{
  NW_Wml1x_ContentHandler_t *thisObj;
  thisObj = NW_Wml1x_ContentHandlerOf(usrAgent);


  thisObj->wmlTimer =
    NW_System_Timer_New (NW_Wml_Timeout, &(thisObj->wmlInterpreter),
                         period * NW_MSEC_PER_WML_TICK, NW_FALSE);
  if(thisObj->wmlTimer == NULL)
  {
    return KBrsrOutOfMemory;
  }

  return KBrsrSuccess;
}

/*****************************************************************

  Name:         NW_Wml1x_ReadTimer()

  Description:

  Parameters:
    usrAgent  - In      - the content handler on behalf of which the work is being done
    period    - In/Out  - pointer to variable to receive timer value

  Algorithm:

  Return Value:

*****************************************************************/
TBrowserStatusCode NW_Wml1x_ReadTimer (void *usrAgent, NW_Uint32 *period)
{
  NW_Uint32 period32;
  TBrowserStatusCode status;
  NW_System_Timer_t* wmlTimer = GetWmlTimer(usrAgent);

  if (wmlTimer == NULL) {
    return KBrsrBadInputParam;
  }
  status = NW_System_Timer_Read (wmlTimer, &period32);
  if(status == KBrsrSuccess)
  {
    *period = period32/NW_MSEC_PER_WML_TICK;
  }
  return status;
}

/*****************************************************************

  Name:         NW_Wml1x_DestroyTimer()

  Description:

  Parameters:
    usrAgent  - In      - the content handler on behalf of which the work is being done

  Algorithm:

  Return Value:

*****************************************************************/
TBrowserStatusCode NW_Wml1x_DestroyTimer (void *usrAgent)
{
  NW_System_Timer_t* wmlTimer = GetWmlTimer(usrAgent);

  if (wmlTimer == NULL) {
    return KBrsrBadInputParam;
  }
  NW_Object_Delete (wmlTimer);
  SetWmlTimer(usrAgent, NULL);

  return KBrsrSuccess;
}

/*****************************************************************

  Name:         NW_Wml1x_ResumeTimer()

  Description:

  Parameters:
    usrAgent  - In      - the content handler on behalf of which the work is being done

  Algorithm:

  Return Value:

*****************************************************************/
TBrowserStatusCode NW_Wml1x_ResumeTimer (void *usrAgent)
{
  NW_System_Timer_t* wmlTimer = GetWmlTimer(usrAgent);

  if (wmlTimer == NULL) {
    return KBrsrBadInputParam;
  }
  return NW_System_Timer_Resume (wmlTimer);
}

/*****************************************************************

  Name:         NW_Wml1x_StopTimer()

  Description:

  Parameters:
    usrAgent  - In      - the content handler on behalf of which the work is being done

  Algorithm:

  Return Value:

*****************************************************************/
TBrowserStatusCode NW_Wml1x_StopTimer (void *usrAgent)
{
  NW_System_Timer_t* wmlTimer = GetWmlTimer(usrAgent);

  if (wmlTimer == NULL) {
    return KBrsrBadInputParam;
  }
  return NW_System_Timer_Stop (wmlTimer);
}

/*****************************************************************

  Name:         NW_Wml1x_IsTimerRunning()

  Description:

  Parameters:
    usrAgent  - In      - the content handler on behalf of which the work is being done
    isRunning - In/Out  - pointer to variable to receive state of timer

  Algorithm:

  Return Value:

*****************************************************************/
TBrowserStatusCode NW_Wml1x_IsTimerRunning (void *usrAgent, NW_Bool *isRunning)
{
  NW_System_Timer_t* wmlTimer = GetWmlTimer(usrAgent);

  if (wmlTimer == NULL) {
    *isRunning = NW_FALSE;
  } else {
    (void) NW_System_Timer_IsRunning (wmlTimer, isRunning);
  }
  return KBrsrSuccess;
}
