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

#include "nw_wml_core.h"
#include "nw_wae.h"
#include "nwx_ctx.h"
#include "nw_scrproxy.h"
#include "nw_wml1x_wml1xcontenthandleri.h"
#include "nw_wml1x_wml1xapi.h"
#include "nw_wmlscriptch_wmlscriptcontenthandler.h"  /* to run Wml Scripts */
#include "nw_lmgr_textbox.h"
#include "nw_hed_documentrooti.h"
#include "nw_hed_appservices.h"
#include "nw_hed_contextvisitor.h"
#include "nw_hed_inumbercollectori.h"
#include "nw_adt_segmentedvector.h"
#include "nw_lmgr_cssproperties.h"
#include "nw_imagech_epoc32contenthandler.h"
#include "nwx_string.h"
#include "stdarg.h"
#include "nw_lmgr_bidiflowbox.h"
#include "nw_wml1x_wml1xdefaultstylesheet.h"
#include "nw_image_cannedimages.h"
#include "nw_text_ascii.h"
#include "nwx_osu.h"
#include "wml_url_utils.h"
#include "wml_history.h"
#include "nw_xmlp_xmlp2wbxml.h"
#include "nwx_settings.h"
#include "nw_wml1x_wml_1_3_tokens.h"
#include "wml_elm_attr.h"
#include "wml_task.h"
#include "nw_adt_types.h"
#include "nw_lmgr_slavepropertylist.h"
#include "nw_lmgr_rootbox.h"

#include "nw_markup_numbercollector.h"
#include "nw_system_optionlist.h"
#include "nw_lmgr_imgcontainerbox.h"

#include "nw_htmlp_wml_dict.h"
#include "nw_htmlp_to_wbxml.h"
#include "nwx_osu_file.h"
#include "nwx_logger.h"

#include <stddef.h>
#include "nwx_http_defs.h"
#include "GDIDevicecontext.h"
#include "BrsrStatusCodes.h"
#include "HEDDocumentListener.h"


#include "WmlControl.h"

/* ------------------------------------------------------------------------- *
   private methods
 * ------------------------------------------------------------------------- */
/************************************************************************
  Function: HedToWmlTask

  Purpose: Converts HED Url request reason to Wml task

  Parameters: HED reason
  
  Return Values: Wml task

**************************************************************************/

static NW_Wml_Task_e HedToWmlTask(NW_Uint8 reason)
{
  NW_Wml_Task_e ret = GO_TASK;

  if ((reason == NW_HED_UrlRequest_Reason_DocPrev) ||
      (reason == NW_HED_UrlRequest_Reason_ShellPrev))
  {
    ret= PREV_TASK;
  }

  return ret;
}
/*****************************************************************

  Name:         GetAlignProp

  Description:  Looks in Wml img element for the align attr. 
                Converts it to the LManager box property values
    
  Return Value: NW_TRUE when the align attr is present
                Value of the attribute

*****************************************************************/
static NW_Bool GetAlignProp(NW_Wml1x_ContentHandler_t *thisObj,
                            NW_Uint16      elId, 
                            NW_ADT_Token_t *token) 
{
  NW_Bool     align = NW_TRUE;
  NW_Ucs2     *retString = NULL;
  TBrowserStatusCode status;

  status = NW_Wml_GetAttribute(&(thisObj->wmlInterpreter),
                               elId,
                               ALIGN_ATTR,
                               &retString);

  if (status != KBrsrSuccess || !retString) {
    align = NW_FALSE;
    goto funcEnd;
  }

  if (!NW_Str_StrcmpConst(retString, WAE_ASC_TOP_STR)) {
    *token = NW_CSS_PropValue_top;
  }
  else if (!NW_Str_StrcmpConst(retString, WAE_ASC_MIDDLE_STR)) {
    *token = NW_CSS_PropValue_middle;
  }
  else if (!NW_Str_StrcmpConst(retString, WAE_ASC_BOTTOM_STR)) {
    /* map WML bottom to CSS baseline to conform to WML spec. */
    *token = NW_CSS_PropValue_baseline;
  }
  else {
    align = NW_FALSE;
  }

funcEnd:
  NW_Str_Delete(retString);
  return align;
}

/*****************************************************************

  Name:         NW_Bool GetVHspaceHWprop

  Description:  Looks in Wml img element for the vspace, hspace, height, width
                attributes. Converts them to the LManager box property values   

  Return Value: NW_TRUE when the attributes are present.
                Value type and value of the attribute

*****************************************************************/
static NW_Bool GetVHspaceHWprop(NW_Wml1x_ContentHandler_t *thisObj,
                                NW_Uint16          elId,
                                NW_Wml_Attr_t      tag,
                                NW_Int32           *val,
                                NW_ADT_ValueType_t *type)
{
  NW_Ucs2     *ptr;
  TBrowserStatusCode status;
  NW_Ucs2     *retString = NULL;
  NW_Bool     ret = NW_TRUE;

  status = NW_Wml_GetAttribute(&(thisObj->wmlInterpreter),
                               elId,
                               tag,
                               &retString);

  if (status != KBrsrSuccess || !retString) {
    ret = NW_FALSE;
    goto funcEnd;
  }

  if ((ptr=NW_Str_Strchr(retString, '%')) != NULL) {
    *ptr = 0;
    *type = NW_CSS_ValueType_Percentage;
  }
  else {
    *type = NW_CSS_ValueType_Px;
  }
  *val = NW_Str_Atoi(retString);
 
funcEnd:
  NW_Str_Delete(retString);
  return ret;
}

/*****************************************************************

  Name:         AddImageProperties

  Description:  when attributes are present it sets box properties 
                related to the attributes:
                align, vspace, hspace, height, and width  

  Return Value: <none>
*****************************************************************/
void AddImageProperties(NW_Wml1x_ContentHandler_t* thisObj,
                               NW_LMgr_Box_t *box,
                               NW_LMgr_PropertyValue_t value)
{
  
  NW_LMgr_Property_t prop;
  NW_Uint16         elId = NW_UINT16_CAST(value.integer);
  NW_LMgr_Property_t heightVal;
  NW_LMgr_Property_t widthVal;
  NW_LMgr_RootBox_t* rootBox = NULL;
  CGDIDeviceContext* deviceContext = NULL;
  const NW_GDI_Rectangle_t* rectangle = NULL;
  NW_HED_DocumentRoot_t* documentRoot = NULL;
  
  /* first must set the element Id on the new box */
  prop.type = NW_CSS_ValueType_Integer;
  prop.value = value;
  NW_LMgr_Box_SetProperty(box, NW_CSS_Prop_elementId, &prop);
  
  if (GetAlignProp(thisObj, elId, &prop.value.token)) 
  {
    prop.type = NW_CSS_ValueType_Token ;
    NW_LMgr_Box_SetProperty(box, NW_CSS_Prop_verticalAlign, &prop);
  }

  /* get the RootBox from through IDocumentListener interface */
  documentRoot = (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode (thisObj);
  NW_ASSERT( documentRoot != NULL );

  rootBox = NW_HED_DocumentRoot_GetRootBox( documentRoot );
  NW_ASSERT(rootBox);
  deviceContext = NW_LMgr_RootBox_GetDeviceContext (rootBox);
  rectangle = deviceContext->DisplayBounds();

  if (GetVHspaceHWprop(thisObj, elId, VSPACE_ATTR, &prop.value.integer, &prop.type)) 
  {
    if (prop.type == NW_CSS_ValueType_Percentage) 
    {
      prop.value.decimal = (NW_Float32)prop.value.integer;
      prop.value.integer = (NW_GDI_Metric_t)((prop.value.decimal*rectangle->dimension.height)/100);
      prop.type = NW_CSS_ValueType_Px;
    }
    NW_LMgr_Box_SetProperty(box, NW_CSS_Prop_topPadding,    &prop);
    NW_LMgr_Box_SetProperty(box, NW_CSS_Prop_bottomPadding, &prop);
  }

  if (GetVHspaceHWprop(thisObj, elId, HSPACE_ATTR, &prop.value.integer, &prop.type)) 
  {
    if (prop.type == NW_CSS_ValueType_Percentage) 
    {
      prop.value.decimal = (NW_Float32)prop.value.integer;
      prop.value.integer = (NW_GDI_Metric_t)((prop.value.decimal*rectangle->dimension.width)/100);
      prop.type = NW_CSS_ValueType_Px;
    }
    NW_LMgr_Box_SetProperty(box, NW_CSS_Prop_leftPadding, &prop);
    NW_LMgr_Box_SetProperty(box, NW_CSS_Prop_rightPadding,&prop);
  }
  
  if (GetVHspaceHWprop(thisObj, elId, HEIGHT_ATTR, &heightVal.value.integer, &heightVal.type)) 
  {
    if (heightVal.type == NW_CSS_ValueType_Percentage) 
    {
      heightVal.value.decimal = (NW_Float32)heightVal.value.integer;
      heightVal.value.integer = (NW_GDI_Metric_t)((heightVal.value.decimal*rectangle->dimension.height)/100);
      heightVal.type = NW_CSS_ValueType_Px;
    }
    NW_LMgr_Box_SetProperty(box, NW_CSS_Prop_height, &heightVal);
  }

  if (GetVHspaceHWprop(thisObj, elId, WIDTH_ATTR, &widthVal.value.integer, &widthVal.type)) 
  {
    if (widthVal.type == NW_CSS_ValueType_Percentage) 
    {
      widthVal.value.decimal = (NW_Float32)widthVal.value.integer;
      widthVal.value.integer = (NW_GDI_Metric_t)((widthVal.value.decimal*rectangle->dimension.width)/100);
      widthVal.type = NW_CSS_ValueType_Px;
    }
    NW_LMgr_Box_SetProperty(box, NW_CSS_Prop_width, &widthVal);
  }
  
  return;
}

/*****************************************************************

  Name:         LoadSrcImage

  Description:  Invokes image load for the WML 'src" attribute of
                the "img: element.

  Parameters:   ContentHandler
                Wrapper box for the image to be loaded
                element Id

  Return Value: KBrsrSuccess
                KBrsrOutOfMemory
                KBrsrFailure

*****************************************************************/
TBrowserStatusCode LoadSrcImage(NW_Wml1x_ContentHandler_t *thisObj,
                                NW_Uint16                 elId)
{
  NW_Ucs2               *imageUrl = NULL;
  TBrowserStatusCode           status;
  NW_Text_UCS2_t        *urlObj;
  NW_HED_DocumentRoot_t *documentRoot;

  status = NW_Wml_GetImageUrl(&(thisObj->wmlInterpreter),
                                elId,
                                &imageUrl);

  if (status == KBrsrSuccess) {

    NW_ASSERT(imageUrl);

    urlObj = NW_Text_UCS2_New (imageUrl, 0, NW_Text_Flags_TakeOwnership);

    if (urlObj != NULL) {
      NW_Uint32 elId32 = elId; 
      /* get the documentRoot and invoke the Load method */
      documentRoot =
        (NW_HED_DocumentRoot_t*)NW_HED_DocumentNode_GetRootNode (thisObj);

      status =
        NW_HED_DocumentRoot_StartLoad (documentRoot,
                                      NW_HED_DocumentNodeOf (thisObj),
                                      NW_TextOf (urlObj),
                                      NW_HED_UrlRequest_Reason_DocLoadChild,
                                      (void*)elId32, NW_UrlRequest_Type_Image,
									  NW_CACHE_NORMAL);
      NW_Object_Delete(urlObj);
    } else {
      status = KBrsrOutOfMemory; /* NW_Text_UCS2_New() takes care of deleting string when fails */
    }
  }
  return status;
}

/*****************************************************************

  Name:         StripInsecureVariables()

  Description:  

  Parameters:   

  Algorithm:    

  Return Value: 

*****************************************************************/
static TBrowserStatusCode StripInsecureVariables (NW_HED_Context_t* context)
{
  static const NW_Ucs2 ecom[] = {'E', 'c', 'o', 'm', '_', 0};
  NW_Uint32 len = NW_Str_Strlen(ecom);
  const NW_HED_Context_Variable_t* var;
  NW_Uint32 i = 0;

  /* while there is another entry */
  while ((var = NW_HED_Context_GetEntry (context, i)) != NULL) {
    /* if the variable's name starts with "Ecom_" remove it -- don't increment i */
    if (!NW_Str_Strncmp ((NW_Ucs2*) var->name.storage, ecom, len)) {
      NW_HED_Context_RemoveEntry (context, i);
    }

    /* otherwise just increment i */
    else {
      i++;
    }
  }

  return KBrsrSuccess;
}

/*****************************************************************

  Name:         CopyInGlobalVariables()

  Description:  

  Parameters:   

  Algorithm:    

  Return Value: 

*****************************************************************/
static TBrowserStatusCode CopyInGlobalVariables(NW_Wml1x_ContentHandler_t* thisObj)
{
  NW_HED_DocumentRoot_t* docRoot;
  NW_HED_Context_t* globalContext;
  TBrowserStatusCode status = KBrsrSuccess;

  /* get the global context from the doc-root */
  docRoot =
    (NW_HED_DocumentRoot_t*)NW_HED_DocumentNode_GetRootNode (thisObj);

  globalContext = NW_HED_DocumentRoot_GetContext (docRoot, NW_HED_DocumentRoot_ContextGlobalId);
  NW_ASSERT (globalContext);
  
  /* apply the global variables to the wml interpreter */
  NW_DeckDecoder_ContextToVarList (globalContext, thisObj->wmlInterpreter.var_list,
      thisObj->wmlInterpreter.mem_segment_general);

  /* clear the global variable context */
  NW_HED_Context_Reset (globalContext);

  return status;
}
  
/*****************************************************************

  Name:         CopyOutGlobalVariables()

  Description:  

  Parameters:   

  Algorithm:    

  Return Value: 

*****************************************************************/
static TBrowserStatusCode CopyOutGlobalVariables(NW_Wml1x_ContentHandler_t* thisObj)
{
  NW_HED_DocumentRoot_t* docRoot;
  NW_HED_Context_t* oldGlobalContext;
  NW_HED_Context_t* newGlobalContext;

  /* get the global context from the doc-root */
  docRoot =
    (NW_HED_DocumentRoot_t*)NW_HED_DocumentNode_GetRootNode (thisObj);
  oldGlobalContext = NW_HED_DocumentRoot_GetContext (docRoot, NW_HED_DocumentRoot_ContextGlobalId);

  /* convert the varlist into a new context */
  newGlobalContext = NW_DeckDecoder_VarListToContext(thisObj->wmlInterpreter.var_list);
 
  /* strip insecure variables */
  StripInsecureVariables (newGlobalContext);

  /* replace the old context with the new one */
  if (newGlobalContext != NULL) {
    NW_Int8 id;

    NW_HED_DocumentRoot_RemoveContext (docRoot, NW_HED_DocumentRoot_ContextGlobalId);

    NW_Object_Delete (oldGlobalContext);

    id = NW_HED_DocumentRoot_ContextGlobalId;
    NW_HED_DocumentRoot_AddContext (docRoot, newGlobalContext, &id);
  }

  /* otherwise just reset the current old global context */
  else {
    NW_HED_Context_Reset (oldGlobalContext);
  }

  return KBrsrSuccess;
}
  
/*****************************************************************

  Name:         CopyInTaskVariables()

  Description:  

  Parameters:   

  Algorithm:    

  Return Value: 

*****************************************************************/
static TBrowserStatusCode CopyInTaskVariables(NW_Wml1x_ContentHandler_t* thisObj)
{
  NW_HED_DocumentRoot_t* docRoot;
  NW_HED_Context_t* context;
  TBrowserStatusCode status = KBrsrSuccess;

  /* get the global context from the doc-root */
  docRoot =
    (NW_HED_DocumentRoot_t*)NW_HED_DocumentNode_GetRootNode (thisObj);
  NW_ASSERT (docRoot);

  context = NW_HED_DocumentRoot_GetContext (docRoot, NW_Wml1x_ContentHandler_ContextTaskId);
  if (context == NULL) {
    return KBrsrSuccess;
  }
  
  /* apply the global variables to the wml interpreter */
  NW_DeckDecoder_ContextToVarList (context, thisObj->wmlInterpreter.param_list,
      thisObj->wmlInterpreter.mem_segment_general);

  /* clear the global variable context */
  NW_HED_Context_Reset (context);

  return status;
}
  

/*****************************************************************

  Name:         CopyOutTaskVariables()

  Description:  

  Parameters:   

  Algorithm:    

  Return Value: 

*****************************************************************/
static TBrowserStatusCode CopyOutTaskVariables(NW_Wml1x_ContentHandler_t* thisObj)
{
  NW_HED_DocumentRoot_t* docRoot;
  NW_HED_Context_t* oldContext;
  NW_HED_Context_t* newContext;

  /* get the global context from the doc-root */
  docRoot =
    (NW_HED_DocumentRoot_t*)NW_HED_DocumentNode_GetRootNode (thisObj);
  oldContext = NW_HED_DocumentRoot_GetContext (docRoot, NW_Wml1x_ContentHandler_ContextTaskId);

  /* convert the varlist into a new context */
  newContext = NW_DeckDecoder_VarListToContext(thisObj->wmlInterpreter.param_list);

  /* strip insecure variables */
  StripInsecureVariables (newContext);

  /* replace the old context with the new one */
  if (newContext != NULL) {
    NW_Int8 id;

    NW_HED_DocumentRoot_RemoveContext (docRoot, NW_Wml1x_ContentHandler_ContextTaskId);

    NW_Object_Delete (oldContext);

    id = NW_Wml1x_ContentHandler_ContextTaskId;
    NW_HED_DocumentRoot_AddContext (docRoot, newContext, &id);
  }

  /* otherwise just reset the current old global context */
  else {
    NW_HED_Context_Reset (oldContext);
  }

  return KBrsrSuccess;
}




/****************************************************************************
* IMAGE RELATED STATIC MEATHODS
*****************************************************************************/

/*****************************************************************

  Name:         GetAltTextBox ()

  Description:  

  Parameters:   

  Algorithm:    

  Return Value: 
    
*****************************************************************/
static 
TBrowserStatusCode GetAltTextBox (NW_Wml1x_ContentHandler_t *thisObj,
                           NW_Uint16                 elId,
                           NW_LMgr_TextBox_t         **textBox)

{
  NW_Text_t              *altStr = NULL;
  NW_Ucs2                *retString = NULL;
  TBrowserStatusCode            ret_status = KBrsrFailure;


  *textBox = NULL;

  /* find the 'alt' attribute and get its string value */
  ret_status = NW_Wml_GetAttribute(&(thisObj->wmlInterpreter),
                                     elId,
                                     ALT_ATTR,
                                     &retString);
  if (ret_status == KBrsrOutOfMemory)
    return ret_status;
  
  if((ret_status  == KBrsrSuccess) && (retString != NULL)) {
    /*alt string exists*/
    altStr = (NW_Text_t*)NW_Text_UCS2_New (retString,
                                           NW_Str_Strlen(retString), 
                                           NW_Text_Flags_TakeOwnership);
    if(altStr == NULL) {
      return KBrsrOutOfMemory;
    }
    
    /* create a text box with alternative text from "alt" attribute */
    *textBox = NW_LMgr_TextBox_New(1, altStr);

    if(*textBox == NULL) {
      NW_Object_Delete (altStr);
      return KBrsrOutOfMemory;
    }  

  }
  return ret_status;
}

/*****************************************************************

  Name:         HandleInvalidImage()

  Description:  

  Parameters:   

  Algorithm:    

  Return Value:
    ret_status = HandleInvalidImage(contentHandler,thisObj,context,response,parentBox);

  Kimono modification (see HandleImageLoad):
    Added output parameter (NW_LMgr_Box_t** box)
	Removed calls to NodeChanged, NW_Wml1x_ImageWithinAnchorDefaultStyle.
	Removed assert on response param.
*****************************************************************/
TBrowserStatusCode HandleInvalidImage( NW_Wml1x_ContentHandler_t  *thisObj,
                                                     NW_HED_DocumentNode_t* documentNode,
                                                     NW_HED_DocumentNode_t* childNode,
                               NW_Url_Resp_t*             response,
                                                     void* pElId)
{
  /*image is not valid*/ 
  NW_LMgr_Box_t*               box = NULL;
  NW_LMgr_Box_t*               contextBox = NULL;
  NW_LMgr_BoxVisitor_t         boxVisitor;
  NW_LMgr_PropertyValue_t      value;  
  NW_Uint32                    elId32 = (NW_Uint32)pElId;
  NW_LMgr_Property_t           prop;
  TBrowserStatusCode            ret_status;
  NW_LMgr_TextBox_t      *textBox = NULL;
  NW_HED_ContentHandler_t*   contentHandler;

  NW_ASSERT( thisObj );  
  NW_ASSERT( childNode );
  NW_REQUIRED_PARAM(response);
  
  /* for convenience */
  contentHandler = NW_HED_ContentHandlerOf( childNode );
	/*check if we have a valid context*/
  ret_status = _NW_Wml1x_ContentHandler_GetBoxTree( documentNode, &box );   

  /* initialize the boxVisitor */
  value.integer = 0;
  if( ret_status == KBrsrSuccess && NW_LMgr_BoxVisitor_Initialize (&boxVisitor, NW_LMgr_BoxOf (box)) 
      == KBrsrSuccess )
    {
    while ((box = NW_LMgr_BoxVisitor_NextBox (&boxVisitor, 0)) != NULL) 
    {
      if (NW_Object_IsDerivedFrom(box, &NW_LMgr_ImgContainerBox_Class))
      {
        (void)NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_elementId, NW_CSS_ValueType_Integer, &value);
        if (NW_UINT16_CAST(value.integer) == NW_UINT16_CAST(elId32))
        {
          /* We found a box that has element Id same as context*/
          contextBox = box;
          break;        
        }
      }
    }
  }
  /*check for local src attrib */
  if (NW_ImageCH_Epoc32ContentHandler_GetLoadMode(NW_ImageCH_Epoc32ContentHandlerOf (contentHandler)) == 
      NW_HED_UrlRequest_LoadLocal) {
    
    return LoadSrcImage(thisObj, NW_UINT16_CAST (value.integer));
  }
  
  /* No local src specified display broken canned image with alt text if present */
  ret_status = NW_HED_DocumentNode_GetBoxTree (contentHandler, &box);
  if (ret_status != KBrsrSuccess) {
    UrlLoader_UrlResponseDelete(response);
    NW_Object_Delete (contentHandler);
    return ret_status;
  }    
  
  /*Get the alt textBox if it exists*/
  ret_status = GetAltTextBox (thisObj,NW_UINT16_CAST (value.integer),&textBox);
  if(ret_status == KBrsrOutOfMemory) {
    NW_Object_Delete (box);    
    UrlLoader_UrlResponseDelete(response);
    NW_Object_Delete (contentHandler);
    return ret_status;
  }

  if ((ret_status == KBrsrSuccess) && (textBox != NULL)) {
    ret_status = NW_LMgr_ContainerBox_InsertChildAft(contextBox->parent, (NW_LMgr_Box_t*)textBox, contextBox);
    if(ret_status  == KBrsrOutOfMemory) {
      NW_Object_Delete (textBox);
      return ret_status;
    }
  }
  
  ret_status = NW_LMgr_ContainerBox_InsertChildAft(contextBox->parent, box, contextBox);  

  if(ret_status == KBrsrSuccess) {
    
    AddImageProperties (thisObj, box, value);
    NW_Wml1x_ImageWithinAnchorDefaultStyle(&prop, box);
  }
  NW_Object_Delete(contextBox);
  return ret_status;
}

/*****************************************************************

  Name:         HandleValidImage()

  Description:  

  Parameters:   

  Algorithm:    

  Return Value: 
    ret_status = HandleValidImage(contentHandler,thisObj,context,response,parentBox);

  Kimono modification (see HandleImageLoad):
    Added output parameter (NW_LMgr_Box_t** box)
	Removed calls to NodeChanged, NW_Wml1x_ImageWithinAnchorDefaultStyle.
*****************************************************************
TBrowserStatusCode HandleValidImage (NW_HED_ContentHandler_t*    contentHandler,
                              NW_Wml1x_ContentHandler_t*  thisObj,
                              NW_ADT_Vector_Metric_t      index,
                              NW_LMgr_PropertyValue_t     imgPropVal,
                              NW_Url_Resp_t*              response,
			                        NW_LMgr_ContainerBox_t*     parentBox,
			                        NW_LMgr_Box_t**             box)
{
  TBrowserStatusCode  ret_status;

  NW_REQUIRED_PARAM(response);
  NW_ASSERT(contentHandler);
  NW_ASSERT(box);

  *box = NULL;

  ret_status = NW_HED_DocumentNode_GetBoxTree (contentHandler, box);
  if (ret_status != KBrsrSuccess) {
    NW_Url_Resp_Delete(response);
    NW_Object_Delete (contentHandler);
    return ret_status;
  }
  
  ret_status = NW_LMgr_ContainerBox_InsertChildAt(parentBox, *box, index);
  
  if(ret_status == KBrsrSuccess) {  
    AddImageProperties (thisObj, *box, imgPropVal);
    NW_Wml1x_ImageWithinAnchorDefaultStyle(&prop, *box);
  }  
  
  return ret_status;
}*/



/* ------------------------------------------------------------------------- */
static
NW_Bool
NW_Wml1x_ContentHandler_IsIntraDeck (NW_HED_ContentHandler_t *thisObj,
                                     const NW_Text_UCS2_t    *url)
{
  NW_Text_Length_t urlLength;
  const NW_Ucs2    *urlStorage        = NULL;
  const NW_Ucs2    *currentUrlStorage = NULL;
  NW_Ucs2          *urlBase           = NULL;
  NW_Ucs2          *currentUrlBase    = NULL;
  NW_Uint8         urlFreeNeeded      = NW_FALSE;
  NW_Bool          match              = NW_FALSE;

  /* pram check */
  NW_THROWIF_NULL (url);

  /* get the base urls */
  NW_THROWIF_NULL (urlStorage = NW_Text_GetUCS2Buffer(url, NW_Text_Flags_Aligned, &urlLength, &urlFreeNeeded));

  NW_THROWIF_NULL (currentUrlStorage = NW_HED_UrlRequest_GetRawUrl (
      NW_HED_ContentHandler_GetUrlRequest (thisObj))); /* TODO change this to NW_HED_ContentHandler_GetUrlResponse */

  NW_THROWIF_ERROR (NW_Url_GetBase(urlStorage, &urlBase));
  NW_THROWIF_ERROR (NW_Url_GetBase(currentUrlStorage, &currentUrlBase));

  /* compare the current url with the specified */
  if (0 == NW_Str_Strcmp(currentUrlBase, urlBase)) {
      match = NW_TRUE;
  }

  /* Fall through */

  /* Clean up is the same for failure and success */
NW_CATCH_ERROR
  if (urlFreeNeeded) {
    NW_Str_Delete((NW_Ucs2 *) urlStorage);
  }
  NW_Str_Delete((NW_Ucs2 *) urlBase);
  NW_Str_Delete(currentUrlBase);

  return match;
}

/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_Wml1x_ContentHandler_CreateDocument(NW_Wml1x_ContentHandler_t* thisObj )
{
  TBrowserStatusCode status = KBrsrFailure;
  NW_Buffer_t* domBuffer = NULL;
  NW_Bool plainText = NW_FALSE;
  NW_HED_ContentHandler_t* contentHandler;  
  NW_HED_DomTree_t* domTree;
  NW_Bool encodingFound;
  NW_Uint8 parse_number = 0;

  NW_LOG0(NW_LOG_LEVEL2, "NW_WML1x_ContentHandler_CreateDocument STARTS.");

  contentHandler = NW_HED_ContentHandlerOf( thisObj );
  status = _NW_HED_CompositeContentHandler_ComputeEncoding(NW_HED_CompositeContentHandlerOf(thisObj),
    contentHandler->response, &encodingFound);
  if (status != KBrsrSuccess)
  {
    return status;
  }


  /* convert a plain-text response into a wbxml buffer */
htmlp_parse:
  if(0 == NW_Asc_stricmp((char*)HTTP_text_vnd_wap_wml_string, (char*)contentHandler->response->contentTypeString))
  {
    NW_Uint32 line = 0;
    NW_HED_CharsetConvContext ctx;
    ctx.contentHandler = NW_HED_CompositeContentHandlerOf(thisObj);
    ctx.response = contentHandler->response;
    NW_LOG0(NW_LOG_LEVEL2, "NW_WML1x_ContentHandler_CreateDocument before HtmlToWbxml.");
    parse_number++;
    if (!encodingFound) // Automatic settings
    {
        status = NW_HTMLP_HtmlToWbxml( contentHandler->response->body, contentHandler->response->charset,
            contentHandler->response->byteOrder,
            &domBuffer, &line, NW_Wml_1_3_PublicId, 
            NW_HTMLP_Get_WML_ElementTableCount(),NW_HTMLP_Get_WML_ElementDescriptionTable(),
            NW_TRUE, NW_TRUE, &ctx, NW_HED_CompositeContentHandler_CharConvCB, NW_FALSE);
    }
    else // Use user selection or BOM
    {
    status = NW_HTMLP_HtmlToWbxml( contentHandler->response->body, contentHandler->response->charset, 
        contentHandler->response->byteOrder,
        &domBuffer, &line, NW_Wml_1_3_PublicId, 
        NW_HTMLP_Get_WML_ElementTableCount(),
        NW_HTMLP_Get_WML_ElementDescriptionTable(),
        NW_TRUE, NW_FALSE, &ctx, NW_HED_CompositeContentHandler_CharConvCB,
        NW_FALSE);
    }
    NW_LOG0(NW_LOG_LEVEL2, "NW_WML1x_ContentHandler_CreateDocument after HtmlToWbxml.");
    if (domBuffer)
      NW_LOG1(NW_LOG_LEVEL2, "NW_WML1x_ContentHandler_CreateDocument, Length of domBuffer %d", domBuffer->length);
    if (BRSR_STAT_IS_FAILURE(status)) {
      NW_LOG0(NW_LOG_LEVEL2, "NW_WML1x_ContentHandler_CreateDocument, HtmlToWbxml returns BadContent error.");
      status = KBrsrXhtmlBadContent;
    }

    plainText = NW_TRUE;
  }
  else
  {
    /* Otherwise stick our wbxml content in a NW_Buffer */

    /* init our domBuffer */
    domBuffer = NW_Buffer_New(0);

    if (domBuffer != NULL && contentHandler->response->body != NULL)
    {
      domBuffer->allocatedLength = contentHandler->response->body->allocatedLength;
      domBuffer->length = contentHandler->response->body->length;
      domBuffer->data = contentHandler->response->body->data;

      if( 0 == NW_Asc_stricmp((char*)HTTP_application_wmlc_saved_string, 
                              (char*)contentHandler->response->contentTypeString) ||
          0 == NW_Asc_stricmp((char*)HTTP_application_saved_string, 
                              (char*)contentHandler->response->contentTypeString) )
      {
        plainText = NW_TRUE;

        if(contentHandler->response->charset == HTTP_iso_10646_ucs_2)
        {
          /* This block is only used to switch the byte order of domBuffer. Each time the 
          function NW_HED_DomTree_New is called, the byte order of domBuffer is changed.
          For a saved page, the domBuffer was saved in the byte order after the change when 
          this page was loaded from web. So this order is wrong when call function
          NW_Wml_LoadWmlResp below.  Thus we call NW_HED_DomTree_New here to change the
          byte order back for saved page loading */
          domTree = NW_HED_DomTree_New (domBuffer, contentHandler->response->charset, 
                                        NW_Wml_1_3_PublicId, NULL);
          if (domTree != NULL)  
          {
            /* Orphan the Buffer so it isn't deleted then delete the temp DomTree */
            domBuffer = NW_HED_DomTree_OrphanDomBuffer (domTree);

            NW_HED_DomTree_Delete (domTree);
            domTree = NULL;
          }
          /* end of the byte order switch */
      }
      }

     delete contentHandler->response->body;   
     contentHandler->response->body = NULL;    
    }
    else
    {
      status = KBrsrOutOfMemory;
    }
  }

  /* transfer ownership of reqUrl, redirected and content to the wml interpreter */
  if (domBuffer != NULL)
  {
    status = NW_Wml_LoadWmlResp(&thisObj->wmlInterpreter, domBuffer, plainText, contentHandler->response);

    // if the processing of domBuffer failed due to user selecting the wrong encoding,
    // set the default encoding to latin-1 and set the encodingFound to false so 
    // charset detection is enabled to find the right charset.
    // call the htmlp parser again to reparse it.
    if (status != KBrsrSuccess && encodingFound) 
      {
      encodingFound = NW_FALSE;
      contentHandler->response->charset = HTTP_iso_8859_1;
      if (parse_number <2)
        {
        goto htmlp_parse;
        }
      }
  }

#ifdef _DEBUG
  {
    NW_HED_DomHelper_t* domHelper;
    NW_HED_DomTree_t* domTree2;

    domHelper = NW_Wml1x_ContentHandler_GetDomHelper (thisObj, NULL);
    domTree2 = NW_Wml1x_ContentHandler_GetDomTree (thisObj);

    if ((domHelper != NULL) && (domTree2 != NULL))
    {
      NW_HED_DomHelper_PrintTree (domHelper, NW_HED_DomTree_GetRootNode (domTree2));
    }
  }
#endif
  NW_LOG0(NW_LOG_LEVEL2, "NW_WML1x_ContentHandler_CreateDocument ENDS.");

  return status;
}

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_Wml1x_ContentHandler_Class_t NW_Wml1x_ContentHandler_Class = {
  { /* NW_Object_Core                       */
    /* super                                */ &NW_HED_CompositeContentHandler_Class,
    /* queryInterface                       */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base                       */
    /* interfaceList                        */ _NW_Wml1x_ContentHandler_InterfaceList
  },
  { /* NW_Object_Dynamic                    */
    /* instanceSize                         */ sizeof (NW_Wml1x_ContentHandler_t),
    /* construct                            */ _NW_Wml1x_ContentHandler_Construct,
    /* destruct                             */ _NW_Wml1x_ContentHandler_Destruct
  },
  { /* NW_HED_DocumentNode                  */
    /* cancel                               */ _NW_Wml1x_ContentHandler_Cancel,
    /* partialLoadCallback	                */ NULL,
    /* initialize                           */ _NW_Wml1x_ContentHandler_Initialize,
    /* nodeChanged                          */ _NW_HED_DocumentNode_NodeChanged,
    /* getBoxTree                           */ _NW_Wml1x_ContentHandler_GetBoxTree,
    /* processEvent                         */ _NW_HED_DocumentNode_ProcessEvent,
    /* handleError                          */ _NW_HED_DocumentNode_HandleError,
    /* suspend                              */ _NW_Wml1x_ContentHandler_Suspend,
    /* resume                               */ _NW_Wml1x_ContentHandler_Resume,
    /* allLoadsCompleted                    */ _NW_Wml1x_ContentHandler_AllLoadsCompleted,
    /* intraPageNavigationCompleted         */ _NW_Wml1x_ContentHandler_IntraPageNavigationCompleted,
    /* loseFocus                            */ _NW_Wml1x_ContentHandler_LoseFocus,
    /* gainFocus                            */ _NW_Wml1x_ContentHandler_GainFocus,
    /* handleLoadComplete                   */ _NW_HED_DocumentNode_HandleLoadComplete,
  },
  { /* NW_HED_ContentHandler                */
    /* partialNextChunk                     */ _NW_HED_ContentHandler_PartialNextChunk,
    /* getTitle                             */ _NW_Wml1x_ContentHandler_GetTitle,
    /* getUrl                               */ _NW_HED_ContentHandler_GetURL,
    /* resolveUrl                           */ _NW_HED_ContentHandler_ResolveURL,
    /* createHistoryEntry                   */ _NW_HED_ContentHandler_CreateHistoryEntry,
    /* createIntraHistoryEntry              */ _NW_HED_ContentHandler_CreateIntraDocumentHistoryEntry,
    /* newUrlResponse                       */ _NW_HED_ContentHandler_NewUrlResponse,
    /* createBoxTree                        */ NULL,
    /* handleRequest                        */ _NW_Wml1x_ContentHandler_HandleRequest,
    /* featureQuery                         */ _NW_Wml1x_ContentHandler_FeatureQuery,
    /* responseComplete                     */ _NW_HED_ContentHandler_ResponseComplete
  },
  { /* NW_HED_CompositeContentHandler       */
    /* documentDisplayed                    */ _NW_HED_CompositeContentHandler_DocumentDisplayed
  },
  { /* NW_Wml1x_ContentHandler              */
    /* unused                               */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- */
const
NW_Object_Class_t* const _NW_Wml1x_ContentHandler_InterfaceList[] = {
  &NW_Wml1x_ContentHandler_IWmlBrowserLib_Class,
  &NW_Wml1x_ContentHandler_IWmlScriptListener_Class,
  &NW_Wml1x_ContentHandler_NumberCollector_Class,
  NULL
};

/* ------------------------------------------------------------------------- */
const
NW_WmlsCh_IWmlBrowserLib_Class_t NW_Wml1x_ContentHandler_IWmlBrowserLib_Class = {
  { /* NW_Object_Core           */
    /* super                    */ &NW_WmlsCh_IWmlBrowserLib_Class,
    /* queryInterface           */ _NW_Object_Interface_QueryInterface
  },
  { /* NW_Object_Interface      */
    /* offset                   */ offsetof (NW_Wml1x_ContentHandler_t, NW_WmlsCh_IWmlBrowserLib)
  },
  { /* NW_WmlsCh_IWmlBrowserLib */
    /* init - setup task var    */ _NW_Wml1x_ContentHandler_IWmlBrowserLib_Init,
    /* getVar                   */ _NW_Wml1x_ContentHandler_IWmlBrowserLib_GetVar,
    /* setVar                   */ _NW_Wml1x_ContentHandler_IWmlBrowserLib_SetVar,
    /* go                       */ _NW_Wml1x_ContentHandler_IWmlBrowserLib_Go,
    /* prev                     */ _NW_Wml1x_ContentHandler_IWmlBrowserLib_Prev,
    /* newContext               */ _NW_Wml1x_ContentHandler_IWmlBrowserLib_NewContext,
    /* refresh                  */ _NW_Wml1x_ContentHandler_IWmlBrowserLib_Refresh
  }
};

/* ------------------------------------------------------------------------- */
const
NW_WmlsCh_IWmlScriptListener_Class_t NW_Wml1x_ContentHandler_IWmlScriptListener_Class = {
  { /* NW_Object_Core           */
    /* super                    */ &NW_WmlsCh_IWmlScriptListener_Class,
    /* queryInterface           */ _NW_Object_Interface_QueryInterface
  },
  { /* NW_Object_Interface      */
    /* offset                   */ offsetof (NW_Wml1x_ContentHandler_t, NW_WmlsCh_IWmlScriptListener)
  },
  { /* NW_WmlsCh_IWmlBrowserLib */
    /* start                    */ _NW_Wml1x_ContentHandler_IWmlScriptListener_Start,
    /* finish                   */ _NW_Wml1x_ContentHandler_IWmlScriptListener_Finish,
  }
};

/* ------------------------------------------------------------------------- */
const
NW_Markup_NumberCollector_Class_t NW_Wml1x_ContentHandler_NumberCollector_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_Markup_NumberCollector_Class,
    /* querySecondary            */ _NW_Object_Core_QuerySecondary
  },
  { /* NW_Object_Secondary       */
    /* offset                    */ offsetof (NW_Wml1x_ContentHandler_t,
                                              NW_Markup_NumberCollector)
  },
  { /* NW_Object_Aggregate       */
    /* secondaryList             */ _NW_Markup_NumberCollector_SecondaryList,
    /* construct                 */ NULL,
    /* destruct                  */ NULL
  },
  { /* NW_Markup_NumberCollector */
    /* getBoxTree                */ _NW_Wml1x_ContentHandler_NumberCollector_GetBoxTree,
    /* boxIsValid                */ _NW_Markup_NumberCollector_BoxIsValid,
    /* getHrefAttr               */ _NW_Wml1x_ContentHandler_NumberCollector_GetHrefAttr,
    /* getAltAttr                */ _NW_Wml1x_ContentHandler_NumberCollector_GetAltAttr
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/*****************************************************************

  Name:         _NW_Wml1x_ContentHandler_Construct()

  Description:  

  Parameters:   

  Algorithm:    

  Return Value: 

*****************************************************************/
TBrowserStatusCode
_NW_Wml1x_ContentHandler_Construct (NW_Object_Dynamic_t* dynamicObject,
                                    va_list* argp)
{
  NW_Wml1x_ContentHandler_t     *thisObj;
  TBrowserStatusCode                   status = KBrsrSuccess;
  const NW_WmlApi_t                   *wml_api;
  NW_Ucs2                       *reqUrlCpy;
  const NW_Text_t               *url;
  NW_LMgr_BidiFlowBox_t*        bidiFlowBox;
  const NW_HED_UrlRequest_t*    urlRequest;

  /* for convenience */
  thisObj = NW_Wml1x_ContentHandlerOf (dynamicObject);

  /* invoke our superclass constructor */
  status = _NW_HED_ContentHandler_Construct (dynamicObject, argp);
  if (status != KBrsrSuccess) {
    return status;
  }

  /* initialize the member variables */

  /* initialize the WML interpreter */
  urlRequest = va_arg (*argp, NW_HED_UrlRequest_t*);
  NW_ASSERT (urlRequest);
  NW_ASSERT (NW_Object_IsInstanceOf (urlRequest, &NW_HED_UrlRequest_Class));

  url = NW_HED_UrlRequest_GetUrl(urlRequest);
  reqUrlCpy = NW_Text_GetUCS2Buffer (url, NW_Text_Flags_Copy, NULL, NULL);
  
  if (reqUrlCpy != NULL) {
    wml_api = NW_Api_GetWml1xCB();
    NW_ASSERT(wml_api != NULL);
    status = NW_Wml_InitFromShell(&thisObj->wmlInterpreter,
                          reqUrlCpy, wml_api, thisObj);
    NW_Mem_Free(reqUrlCpy);
  } else {
    status = KBrsrOutOfMemory;
  }

  /* create the Wml1x_FormLiaison object */
  thisObj->formLiaison = NW_Wml1x_FormLiaison_New (thisObj);


  /* ensure destructor will not break anything */
  thisObj->title = NULL;
  /* thisObj->currentBox = NULL; */
  thisObj->wmlTimer = NULL;
  thisObj->optionMap = NULL;
  thisObj->optionItemList = NULL;
  thisObj->contextSuspended = NULL;
  thisObj->saveddeck = NW_FALSE;
  thisObj->histload = NW_FALSE;
  thisObj->reload = NW_FALSE;
  thisObj->allowTimer = NW_TRUE;

  /* create boxtree here, this fix the bug in this case:

  <wml> 
    <template> 
      <!-- Template implementation here. --> 
      <do type="prev"><prev/></do> 
    </template> 
    <card id="card1" title="Card #1" newcontext="true"> 
      <onevent type="onenterforward">
      <go href="opg.wmls#main('card2')"/>
      </onevent>
    </card>
    <card id="card2" title="Card #2"> 
      <p> 
        Test result: $test
        Literal Value: $literal
      </p> 
    </card>
  This WML page cause loading a script page without having any boxtree in the Wml1x Content Handler */
    
  bidiFlowBox = NW_LMgr_BidiFlowBox_New(0);
  NW_HED_ContentHandlerOf (thisObj)->boxTree = NW_LMgr_BoxOf(bidiFlowBox);
  thisObj->currentBox = NW_HED_ContentHandlerOf (thisObj)->boxTree;
  if (thisObj->currentBox == NULL) {
    return KBrsrFailure;
  }

  return status;
}

/*****************************************************************

  Name:         _NW_Wml1x_ContentHandler_Destruct()

  Description:  

  Parameters:   

  Algorithm:    

  Return Value: 

*****************************************************************/
void
_NW_Wml1x_ContentHandler_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_Wml1x_ContentHandler_t *thisObj;

  /* for convenience */
  thisObj = NW_Wml1x_ContentHandlerOf (dynamicObject);

  NW_Object_Delete (thisObj->title);
  NW_Object_Delete (thisObj->currentUrl);
  NW_Object_Delete (thisObj->optionMap);
  NW_Object_Delete (thisObj->formLiaison);

  thisObj->title = NULL;
  thisObj->currentUrl    = NULL;
  thisObj->optionMap = NULL;
  thisObj->formLiaison   = NULL;
  
  /* ensure that the content handler is suspended */
  _NW_Wml1x_ContentHandler_Suspend (NW_HED_DocumentNodeOf (thisObj), NW_TRUE);

  /* unregister with the wae user agent */
  TBrowserStatusCode status = NW_Wml_Exit(&thisObj->wmlInterpreter);
  NW_ASSERT(status == KBrsrSuccess);

	/* NW_Object_Delete handles NULL pointer */
	NW_Object_Delete (thisObj->contextSuspended);
}

/*****************************************************************

  Name:         _NW_Wml1x_ContentHandler_Initialize()

  Description:  

  Parameters:   

  Algorithm:    

  Return Value: 

*****************************************************************/
TBrowserStatusCode
_NW_Wml1x_ContentHandler_Initialize( NW_HED_DocumentNode_t *documentNode,
    TBrowserStatusCode aInitStatus )
{
  TBrowserStatusCode               status; 
  NW_Wml1x_ContentHandler_t *thisObj;
  NW_HED_ContentHandler_t* contentHandler;
  const NW_HED_UrlRequest_t *urlRequest;
  NW_WaeUsrAgent_t              *wae;

  NW_REQUIRED_PARAM( aInitStatus );

  /* for convenience */
  thisObj = NW_Wml1x_ContentHandlerOf (documentNode);
  contentHandler = NW_HED_ContentHandlerOf( thisObj );

  /* register the WML callback with the Wml Interpreter */
  wae = (NW_WaeUsrAgent_t*)NW_Ctx_Get(NW_CTX_WML_CORE, 0);
  NW_ASSERT(wae != NULL);

  urlRequest = NW_HED_ContentHandler_GetUrlRequest ( thisObj );
  NW_ASSERT(urlRequest);
  /* 
  ** copy the global variables from the document-root.  Don't
  ** do this if the request that created the content-handler
  ** was started from a shell request, like the goto dialog or
  ** hotlist dialog.  See WML June2000 Specification section 10.4. 
  */
  // ZAL: add comment why it has been moved here from construct
  if (NW_HED_UrlRequest_GetReason( urlRequest ) != NW_HED_UrlRequest_Reason_ShellLoad ) 
    {
    CopyInGlobalVariables (thisObj);

    CopyInTaskVariables (thisObj);
    NW_Wml_ApplyTaskVariables(&thisObj->wmlInterpreter);
    }

 if( contentHandler->response )
   {
    /* set the correct saveddeck boolean value */
    if( 0 == NW_Asc_stricmp( (char*)HTTP_application_wmlc_saved_string, 
		                         (char*)contentHandler->response->contentTypeString ) ||
        0 == NW_Asc_stricmp( (char*)HTTP_application_saved_string, 
                             (char*)contentHandler->response->contentTypeString ) )
    {
      NW_Wml1x_ContentHandler_SetSaveddeck (thisObj, NW_TRUE);
    }

    if(NW_HED_UrlRequest_GetReason( urlRequest ) != NW_HED_UrlRequest_Reason_ShellPrev ) 
    {
      NW_Wml1x_ContentHandler_SetHistload (thisObj, NW_TRUE);
    }

    if(NW_HED_UrlRequest_GetReason( urlRequest ) != NW_HED_UrlRequest_Reason_ShellReload ) 
    {
      NW_Wml1x_ContentHandler_SetReload (thisObj, NW_TRUE);
    }

    
	/* initialize the WML interpreter */
    NW_HED_DocumentRoot_t* documentRoot = NULL;
	NW_HED_HistoryStack_t* history = NULL;
    NW_Uint8 reason = NW_HED_UrlRequest_GetReason(urlRequest);
    documentRoot = (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode (thisObj);
    if (documentRoot)
        {
	    history = NW_HED_DocumentRoot_GetHistoryStack (documentRoot);
        }
    if (history)
        {
        NW_Bool possiblyOEB  = history->iWmlCtrl->HistoryController().possibleWmlOEB();
        if (possiblyOEB)
            {
            reason = NW_HED_UrlRequest_Reason_DocPrev;
            history->iWmlCtrl->HistoryController().setPossibleWmlOEB(EFalse);
            }
        }
    /* Set the actual task in Wml */
    thisObj->wmlInterpreter.curr_task = HedToWmlTask(reason);

    status = NW_Wae_RegisterWml(wae, &thisObj->wmlInterpreter);

    /* create the DOM document */
    status = NW_Wml1x_ContentHandler_CreateDocument (thisObj );

    /* disables small scrren layout */
    NW_Settings_SetDisableSmallScreenLayout(NW_TRUE);
   }
 else
   {
   status = KBrsrUnexpectedError;
   }
 // ZAL: why don't we call superclass init just like xhtmlCH does?
 // temp
 if( status == KBrsrSuccess && contentHandler )
   {
   contentHandler->initialized = NW_TRUE;
   }
  return status;
}

/*****************************************************************

  Name:         _NW_Wml1x_ContentHandler_GetBoxTree()

  Description:  

  Parameters:   

  Algorithm:    

  Return Value: 

*****************************************************************/
TBrowserStatusCode
_NW_Wml1x_ContentHandler_GetBoxTree (NW_HED_DocumentNode_t* documentNode,
                                     NW_LMgr_Box_t** boxTree)
{

  NW_Wml1x_ContentHandler_t* thisObj;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (documentNode,
                                     &NW_HED_DocumentNode_Class));

  /* for convenience */
  thisObj = NW_Wml1x_ContentHandlerOf (documentNode);

  /* successful completion */
  *boxTree = NW_HED_ContentHandlerOf (thisObj)->boxTree;
  if (*boxTree == NULL) {
    return KBrsrFailure;
  }
  return KBrsrSuccess;
}

/*****************************************************************

  Name:         _NW_Wml1x_ContentHandler_Suspend()

  Description:  

  Parameters:   

  Algorithm:    

  Return Value: 

*****************************************************************/
void
_NW_Wml1x_ContentHandler_Suspend (NW_HED_DocumentNode_t* documentNode, NW_Bool aDestroyBoxTree)
{
  NW_Wml1x_ContentHandler_t* thisObj;
  NW_WaeUsrAgent_t              *wae;
  NW_HED_DocumentRoot_t     *docRoot;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (documentNode, &NW_Wml1x_ContentHandler_Class));

  /* for convenience */
  thisObj = NW_Wml1x_ContentHandlerOf (documentNode);

  /* just return if we are already suspended */
  if (thisObj->isSuspended == NW_TRUE) {
    return;
  }

  thisObj->isSuspended = NW_TRUE;

  /* call our super class to suspend the children */
  NW_HED_CompositeContentHandler_Class.NW_HED_DocumentNode.
    suspend (documentNode, aDestroyBoxTree);

  /* copy the global variables out */
  (void) CopyOutGlobalVariables (thisObj);

  /* Copy out task variable */
  (void) CopyOutTaskVariables (thisObj);

  /* stop our timers */
  if (((thisObj->wmlInterpreter).wml_api != NULL) && 
      ((thisObj->wmlInterpreter).wml_api->timer)) {
    (void)(thisObj->wmlInterpreter).wml_api->timer->destroy(thisObj);
  }

  /* cleanup the dynamic item list in the OptionList */
  docRoot =
    (NW_HED_DocumentRoot_t*)NW_HED_DocumentNode_GetRootNode (thisObj);

  NW_ASSERT(docRoot->appServices != NULL);

  if (docRoot->appServices->pictureViewApi.initDynList != NULL) {
    (docRoot->appServices->pictureViewApi.initDynList)(docRoot->browserApp_Ctx);
  }
   
  if (thisObj->optionItemList != NULL) {
    NW_Wml1x_ContentHandler_DeleteOptionItemList(thisObj->optionItemList);
    thisObj->optionItemList = NULL;
  }
  /* unregister wae */
  wae = (NW_WaeUsrAgent_t*)NW_Ctx_Get(NW_CTX_WML_CORE, 0);
  NW_ASSERT(wae != NULL); 

  TBrowserStatusCode status = NW_Wae_UnRegisterWml(wae);
  NW_ASSERT(status == KBrsrSuccess);
}

/*****************************************************************

  Name:         _NW_Wml1x_ContentHandler_Resume()

  Description:  

  Parameters:   

  Algorithm:    

  Return Value: 

*****************************************************************/
void
_NW_Wml1x_ContentHandler_Resume (NW_HED_DocumentNode_t* documentNode)
{
  NW_Wml1x_ContentHandler_t* thisObj;
  NW_WaeUsrAgent_t              *wae;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (documentNode, &NW_Wml1x_ContentHandler_Class));

  /* for convenience */
  thisObj = NW_Wml1x_ContentHandlerOf (documentNode);

  /* just return if we are not suspended */
  if (thisObj->isSuspended == NW_FALSE) {
    return;
  }

  thisObj->isSuspended = NW_FALSE;

  /* call our super class to resume the children */
  NW_HED_CompositeContentHandler_Class.NW_HED_DocumentNode.
      resume (documentNode);

  /* register wae */
  wae = (NW_WaeUsrAgent_t*)NW_Ctx_Get(NW_CTX_WML_CORE, 0);
  NW_ASSERT(wae != NULL);
  (void) NW_Wae_RegisterWml(wae, &thisObj->wmlInterpreter);

  /* tickle the content handler */
  TBrowserStatusCode status = NW_Wml_HandleIntraDocRequest (&thisObj->wmlInterpreter, RESTORE_TASK, NW_TRUE, NULL);
  NW_ASSERT(status == KBrsrSuccess);
}

/*****************************************************************

  Name:         _NW_Wml1x_ContentHandler_AllLoadsCompleted()

  Description:  

  Parameters:   

  Algorithm:    

  Return Value: 

*****************************************************************/
void
_NW_Wml1x_ContentHandler_AllLoadsCompleted (NW_HED_DocumentNode_t* documentNode)
{
  NW_Wml1x_ContentHandler_t* thisObj;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (documentNode, &NW_Wml1x_ContentHandler_Class));

  /* for convenience */
  thisObj = NW_Wml1x_ContentHandlerOf (documentNode);

  /* call our super class to notify the children */
  NW_HED_CompositeContentHandler_Class.NW_HED_DocumentNode.
    allLoadsCompleted (documentNode);

  /* TODO here we can start timers, animations, etc. */

  if (thisObj->allowTimer)
  {
      /* set timer after all loads associated with the pages are completed. 
     Exception is when the load was for WmlScript. In this case 
         "outstandingLoadCount" is negative value */
      if (thisObj->wmlInterpreter.outstandingLoadCount >= 0) {
        NW_Wml_HandleTimer (&thisObj->wmlInterpreter);
      }
  }
}

/*****************************************************************

  Name:         _NW_Wml1x_ContentHandler_IntraPageNavigationCompleted()

  Description:  

  Parameters:   

  Algorithm:    

  Return Value: 

*****************************************************************/
TBrowserStatusCode
_NW_Wml1x_ContentHandler_IntraPageNavigationCompleted (NW_HED_DocumentNode_t* documentNode)
{
  TBrowserStatusCode status = KBrsrSuccess;
  NW_Wml1x_ContentHandler_t* thisObj;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (documentNode, &NW_Wml1x_ContentHandler_Class));

  /* for convenience */
  thisObj = NW_Wml1x_ContentHandlerOf (documentNode);

  /* call our super class to notify the children */
  status = NW_HED_CompositeContentHandler_Class.NW_HED_DocumentNode.
      intraPageNavigationCompleted (documentNode);
  
  /* set timer after all loads associated with the pages are completed*/
  /*lint -e{666} Expression with side effects passed to repeated parameter in macro*/
  if(!NW_HED_DocumentRoot_IsLoading(NW_HED_DocumentNode_GetRootNode (documentNode))) {
    status = NW_Wml_HandleTimer(&thisObj->wmlInterpreter);
  }


  return status;
}
/*****************************************************************

  Name:         _NW_Wml1x_ContentHandler_LoseFocus()

  Description:  

  Parameters:   

  Algorithm:    

  Return Value: 

*****************************************************************/
void
_NW_Wml1x_ContentHandler_LoseFocus (NW_HED_DocumentNode_t* documentNode)
{
  NW_Wml1x_ContentHandler_t* thisObj;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (documentNode, &NW_Wml1x_ContentHandler_Class));

  /* for convenience */
  thisObj = NW_Wml1x_ContentHandlerOf (documentNode);

  /* call our super class to suspend the children */
  NW_HED_CompositeContentHandler_Class.NW_HED_DocumentNode.
    loseFocus (documentNode);

  /* stop our timers */
  if (((thisObj->wmlInterpreter).wml_api != NULL) && 
      ((thisObj->wmlInterpreter).wml_api->timer)) {
    (void)(thisObj->wmlInterpreter).wml_api->timer->stop(thisObj);
  }
}

/*****************************************************************

  Name:         _NW_Wml1x_ContentHandler_GainFocus()

  Description:  

  Parameters:   

  Algorithm:    

  Return Value: 

*****************************************************************/
void
_NW_Wml1x_ContentHandler_GainFocus (NW_HED_DocumentNode_t* documentNode)
{
  NW_Wml1x_ContentHandler_t* thisObj;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (documentNode, &NW_Wml1x_ContentHandler_Class));

  /* for convenience */
  thisObj = NW_Wml1x_ContentHandlerOf (documentNode);

  /* call our super class to resume the children */
  NW_HED_CompositeContentHandler_Class.NW_HED_DocumentNode.
    gainFocus (documentNode);

  /* stop our timers */
  if (((thisObj->wmlInterpreter).wml_api != NULL) && 
      ((thisObj->wmlInterpreter).wml_api->timer)) {
    (void)(thisObj->wmlInterpreter).wml_api->timer->resume(thisObj);
  }
}

/*****************************************************************

  Name:         _NW_Wml1x_ContentHandler_GetTitle()

  Description:  

  Parameters:   

  Algorithm:    

  Return Value: 

*****************************************************************/
const NW_Text_t*
_NW_Wml1x_ContentHandler_GetTitle (NW_HED_ContentHandler_t* contentHandler)
{
  NW_Wml1x_ContentHandler_t*    thisObj;
  NW_Ucs2*                    cardTitle = NULL;
  TBrowserStatusCode                   status;

  
  /* for convenience */
  thisObj = NW_Wml1x_ContentHandlerOf (contentHandler);

  /* we create the title just-in-time */
  if (thisObj->title != NULL) {
    NW_Object_Delete (thisObj->title);
    thisObj->title = NULL;
  }

  if(thisObj->wmlInterpreter.decoder != NULL)
    {
    status = NW_DeckDecoder_GetCardTitle(thisObj->wmlInterpreter.decoder, 
      thisObj->wmlInterpreter.var_list, &cardTitle);
    if (status == KBrsrSuccess)
        {
        thisObj->title =
        (NW_Text_t*) NW_Text_UCS2_New (cardTitle, 0, NW_Text_Flags_TakeOwnership);
        }
    }
     
  return thisObj->title;
}

/*****************************************************************

  Name:         _NW_Wml1x_ContentHandler_Cancel()

  Description:  

  Parameters:   

  Algorithm:    

  Return Value: 

*****************************************************************/
TBrowserStatusCode
_NW_Wml1x_ContentHandler_Cancel (NW_HED_DocumentNode_t* documentNode, 
				 NW_HED_CancelType_t cancelType)
{
  TBrowserStatusCode                 status  = KBrsrSuccess;
  NW_Wml1x_ContentHandler_t*  thisObj = NULL;
  NW_HED_DocumentRoot_t*      docRoot = NULL;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (documentNode,
                                     &NW_Wml1x_ContentHandler_Class));

  /* for convenience */
  thisObj = NW_Wml1x_ContentHandlerOf (documentNode);
  docRoot = (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode (thisObj);
  
  /* We would like to stop all the timer in case of Cancel, except the case
     where user initiated the cancel.  User can hit cancel button even where 
     there are no pending loads and in that case we don't want to stop the 
     timers.  

     Timer in a wml card is started only after all the loads in the page 
     complete */
  if(cancelType != NW_HED_CancelType_User) {
     (void)(thisObj->wmlInterpreter).wml_api->timer->stop(thisObj);
  }

  /* if there has been an onEnterForward or onEnterBackward navigation and there is an error, display
     the current content (ie, last navigated card).  It is desirable to limit re-rendering the content
     to oef error cases only as single navigations have ALREADY rendered the last good card. */
  if (docRoot->documentErrorClass != 0 && thisObj->wmlInterpreter.oefNavigation == NW_TRUE) {
       NW_Wml_DisplayCard(&(thisObj->wmlInterpreter), NW_FALSE);
  }

  status = NW_HED_CompositeContentHandler_Class.NW_HED_DocumentNode.
    cancel (documentNode, cancelType);

  return status;
}
  
/*****************************************************************

  Name:         _NW_Wml1x_ContentHandler_HandleRequest()

  Description:  Method is called to handle url-requests that are 
                originated outside of wml -- for example the shell.
                Requests that are started from inside of wml, link
                activation, wml-prev, scripts, etc do NOT go through
                this method but instead are handled internally.
                Although both internal and external requests go through
                NW_Wml_HandleIntraDocRequest.

  Parameters:   

  Algorithm:    

  Return Value: 

*****************************************************************/
TBrowserStatusCode
_NW_Wml1x_ContentHandler_HandleRequest (NW_HED_ContentHandler_t *thisObj,
                                        NW_HED_UrlRequest_t     *urlRequest)
{
  NW_Wml1x_ContentHandler_t *wmlObj;
  NW_Uint8                  reason;
  NW_Wml_Task_e             task;
  const NW_Ucs2             *url;

  TBrowserStatusCode status = KBrsrFailure;

  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_Wml1x_ContentHandler_Class));
  NW_ASSERT (urlRequest != NULL);

  /* don't handle reload requests here */
  if (NW_HED_UrlRequest_GetReason (urlRequest) == NW_HED_UrlRequest_Reason_ShellReload) {
    return KBrsrFailure;
  }

  /* don't handle post requests here */
  if (NW_HED_UrlRequest_GetMethod (urlRequest) == NW_URL_METHOD_POST) {
    return KBrsrFailure;
  }

  wmlObj = NW_Wml1x_ContentHandlerOf (thisObj);

  /* get the reason and set the task type */
  reason = NW_HED_UrlRequest_GetReason(urlRequest);

  /* Make sure that setvar's are executed for _ShellPrev */
  if (reason == NW_HED_UrlRequest_Reason_ShellPrev) {
    NW_THROWIF_ERROR (status = NW_Wml_ProcessSetvarElements(&wmlObj->wmlInterpreter));
  }
  
  /* if this request is not targeted to this deck or 
     no-cache is requested then return failure */
  if (urlRequest->cacheMode == NW_CACHE_NOCACHE || 
      (NW_Wml1x_ContentHandler_IsIntraDeck (thisObj,
        (NW_Text_UCS2_t*)NW_HED_UrlRequest_GetUrl(urlRequest)) == NW_FALSE) || reason == NW_HED_UrlRequest_Reason_ShellLoad ||
      reason == NW_HED_UrlRequest_Reason_DocLoad || (urlRequest->loadType == NW_UrlRequest_Type_Image)) {
    return KBrsrFailure;
  }
  
  switch (reason) {
    case NW_HED_UrlRequest_Reason_ShellPrev:
    case NW_HED_UrlRequest_Reason_DocPrev:
      task = PREV_TASK;
      break;
    
    case NW_HED_UrlRequest_Reason_RestorePrev:
    case NW_HED_UrlRequest_Reason_RestoreNext:
      task = RESTORE_TASK;
      break;

    default:
      task = GO_TASK;
      break;
  }

  /* get the url */
  url = NW_HED_UrlRequest_GetRawUrl (urlRequest);

  NW_THROWIF_ERROR (status = NW_Wml_HandleIntraDocRequest(&wmlObj->wmlInterpreter,
      task, NW_HED_UrlRequest_IsHistoricRequest (urlRequest), url));

  return KBrsrSuccess;

NW_CATCH_ERROR
  return status;
}

/*****************************************************************

  Name:         _NW_Wml1x_ContentHandler_FeatureQuery()

  Description:  Method is called to verify if a particular feature 
                required by this content handler

  Parameters:   

  Algorithm:    

  Return Value: 

*****************************************************************/
TBrowserStatusCode
_NW_Wml1x_ContentHandler_FeatureQuery (NW_HED_ContentHandler_t *thisObj,
                                       NW_HED_ContentHandler_Feature_t featureId)
{
  TBrowserStatusCode nwStatus;

  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_Wml1x_ContentHandler_Class));
	NW_REQUIRED_PARAM(thisObj);

  switch (featureId)
  {
    case NW_HED_ContentHandler_HitoryPrevUseStale:
      nwStatus = KBrsrSuccess;
      break;
    default:
      nwStatus = KBrsrFailure;
      break;
  }
  return nwStatus;
}
/*****************************************************************

  Name:         _NW_Wml1x_ContentHandler_IWmlBrowserLib_Init()

  Description:

  Parameters:   

  Algorithm:    

  Return Value: 

*****************************************************************/
TBrowserStatusCode
_NW_Wml1x_ContentHandler_IWmlBrowserLib_Init(NW_WmlsCh_IWmlBrowserLib_t *wmlBrowserLib)
{
  NW_Wml1x_ContentHandler_t   *thisObj;

  NW_ASSERT(wmlBrowserLib != NULL);
  
  /* for convenience */
  thisObj = (NW_Wml1x_ContentHandler_t*)NW_Object_Interface_GetImplementer (wmlBrowserLib);

  if (thisObj != NULL) {
    return NW_Wml_ApplyTaskVariables(&thisObj->wmlInterpreter);
  } else {
    return KBrsrFailure;
  }
}

/*****************************************************************

  Name:         _NW_Wml1x_ContentHandler_IWmlBrowserLib_GetVar()

  Description:  

  Parameters:   

  Algorithm:    

  Return Value: 

*****************************************************************/
TBrowserStatusCode
_NW_Wml1x_ContentHandler_IWmlBrowserLib_GetVar (NW_WmlsCh_IWmlBrowserLib_t *wmlBrowserLib,
                                                const NW_Ucs2 *var, NW_Ucs2 **ret_string)
{
  NW_Wml1x_ContentHandler_t   *thisObj;
  TBrowserStatusCode                 status;

  /* for convenience */
  thisObj = (NW_Wml1x_ContentHandler_t*)NW_Object_Interface_GetImplementer (wmlBrowserLib);

  status = NW_Wml_GetVar(&thisObj->wmlInterpreter, var, ret_string);
  
  return status;
}

/*****************************************************************

  Name:         _NW_Wml1x_ContentHandler_IWmlBrowserLib_SetVar()

  Description:  

  Parameters:   

  Algorithm:    

  Return Value: 

*****************************************************************/
TBrowserStatusCode
_NW_Wml1x_ContentHandler_IWmlBrowserLib_SetVar (NW_WmlsCh_IWmlBrowserLib_t *wmlBrowserLib,
                                                const NW_Ucs2 *var, const NW_Ucs2 *value)
{
  NW_Wml1x_ContentHandler_t   *thisObj;

  /* for convenience */
  thisObj = (NW_Wml1x_ContentHandler_t*)NW_Object_Interface_GetImplementer (wmlBrowserLib);

  return NW_Wml_SetVar(&thisObj->wmlInterpreter, var, value);
}

/*****************************************************************

  Name:         _NW_Wml1x_ContentHandler_IWmlBrowserLib_Go()

  Description:  

  Parameters:   

  Algorithm:    

  Return Value: 

*****************************************************************/
TBrowserStatusCode
_NW_Wml1x_ContentHandler_IWmlBrowserLib_Go (NW_WmlsCh_IWmlBrowserLib_t *wmlBrowserLib,
                                            const NW_Ucs2 *url, const NW_Ucs2 *param,
                                            NW_Http_Header_t *header)
{
  NW_Ucs2 *url_cpy = NULL;
  NW_Ucs2 *param_cpy = NULL;
  NW_Http_Header_t *header_cpy = NULL;
  NW_Wml1x_ContentHandler_t   *thisObj;

  /* for convenience */
  thisObj = (NW_Wml1x_ContentHandler_t*)NW_Object_Interface_GetImplementer (wmlBrowserLib);

  /* make a copy for the wml interpreter */
  if (url != NULL) {
    url_cpy = NW_Str_Newcpy(url);
    if (url_cpy == NULL) {
      return KBrsrOutOfMemory;
    }
  }

  if (param != NULL) {
    param_cpy = NW_Str_Newcpy(param);
    if (param_cpy == NULL) {
      NW_Str_Delete(url_cpy);
      return KBrsrOutOfMemory;
    }
  }

  if (header != NULL) {
    //header_cpy = UrlLoader_HeadersCopy(header);
    if (header_cpy == NULL) {
      NW_Str_Delete(url_cpy);
      NW_Str_Delete(param_cpy);
      return KBrsrOutOfMemory;
    }
  }

  NW_Wml_LoadUrl(&thisObj->wmlInterpreter, url_cpy, param_cpy, header_cpy);
  return KBrsrSuccess;
}

/*****************************************************************

  Name:         _NW_Wml1x_ContentHandler_IWmlBrowserLib_Prev()

  Description:  

  Parameters:   

  Algorithm:    

  Return Value: 

*****************************************************************/
TBrowserStatusCode
_NW_Wml1x_ContentHandler_IWmlBrowserLib_Prev (NW_WmlsCh_IWmlBrowserLib_t *wmlBrowserLib)
{
  NW_Wml1x_ContentHandler_t   *thisObj;

  /* for convenience */
  thisObj = (NW_Wml1x_ContentHandler_t*)NW_Object_Interface_GetImplementer (wmlBrowserLib);

  NW_Wml_LoadPrev(&thisObj->wmlInterpreter);
  return KBrsrSuccess;
}

/*****************************************************************

  Name:         _NW_Wml1x_ContentHandler_IWmlBrowserLib_NewContext()

  Description:  

  Parameters:   

  Algorithm:    

  Return Value: 

*****************************************************************/
TBrowserStatusCode
_NW_Wml1x_ContentHandler_IWmlBrowserLib_NewContext (NW_WmlsCh_IWmlBrowserLib_t *wmlBrowserLib)
{
  NW_Wml1x_ContentHandler_t   *thisObj;

  /* for convenience */
  thisObj = (NW_Wml1x_ContentHandler_t*)NW_Object_Interface_GetImplementer (wmlBrowserLib);

  NW_Wml_NewContextFromScript(&thisObj->wmlInterpreter);
  
  return KBrsrSuccess;
}

/*****************************************************************

  Name:         _NW_Wml1x_ContentHandler_IWmlBrowserLib_Refresh()

  Description:  

  Parameters:   

  Algorithm:    

  Return Value: 

*****************************************************************/
TBrowserStatusCode
_NW_Wml1x_ContentHandler_IWmlBrowserLib_Refresh (NW_WmlsCh_IWmlBrowserLib_t *wmlBrowserLib)
{
  /* Refresh is a NOOP */
  NW_REQUIRED_PARAM(wmlBrowserLib);
  return KBrsrSuccess;
}

/*****************************************************************

  Name:         _NW_Wml1x_ContentHandler_IWmlScriptListener_Start()

  Description:  

  Parameters:   

  Algorithm:    

  Return Value: 

*****************************************************************/
TBrowserStatusCode
_NW_Wml1x_ContentHandler_IWmlScriptListener_Start (NW_WmlsCh_IWmlScriptListener_t *wmlScrListener, 
                                                   const NW_Ucs2 *url)
{
  NW_Wml1x_ContentHandler_t   *thisObj;
  NW_HED_DocumentRoot_t* docRoot = NULL;
  void *browserApp = NULL;
 
  /* for convenience */
  thisObj = (NW_Wml1x_ContentHandler_t*)NW_Object_Interface_GetImplementer (wmlScrListener);

  /* get the global context from the doc-root */
  docRoot = (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode (thisObj);
  if(docRoot == NULL) {
    return KBrsrFailure;
  }

  browserApp = NW_Ctx_Get(NW_CTX_BROWSER_APP, 0);

  (void)docRoot->appServices->loadProgress_api.start(browserApp, url);

  return KBrsrSuccess;
}

/*****************************************************************

  Name:         _NW_Wml1x_ContentHandler_IWmlScriptListener_Finish()

  Description:  

  Parameters:   

  Algorithm:    

  Return Value: 

*****************************************************************/
TBrowserStatusCode
_NW_Wml1x_ContentHandler_IWmlScriptListener_Finish (NW_WmlsCh_IWmlScriptListener_t *wmlScrListener,
                                                    const TBrowserStatusCode scrStatus, const NW_Ucs2 *message)
{
  NW_Wml1x_ContentHandler_t   *thisObj;
  NW_HED_CompositeNode_t*     compositeNode;
  NW_HED_DocumentRoot_t       *docRoot = NULL;
  NW_HED_DocumentNode_t       *docNode = NULL;
  NW_LMgr_RootBox_t           *rootBox = NULL;
  NW_HED_ContentHandler_t     *scriptContentHandler = NULL;
  NW_Ucs2                     *url = NULL;
  void *browserApp = NULL;

  NW_TRY (status) {
    NW_REQUIRED_PARAM(message);

    /* for convenience */
    thisObj = (NW_Wml1x_ContentHandler_t*)NW_Object_Interface_GetImplementer (wmlScrListener);
    docNode = NW_HED_DocumentNodeOf (thisObj);
    /* get the global context from the doc-root */
    docRoot = (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode (docNode);
    rootBox = NW_HED_DocumentRoot_GetRootBox (docRoot);

    compositeNode = (NW_HED_CompositeNode_t*)
      NW_Object_QueryAggregate (thisObj, &NW_HED_CompositeNode_Class);
    NW_ASSERT (compositeNode != NULL);

    scriptContentHandler = (NW_HED_ContentHandler_t*)
      NW_HED_CompositeNode_LookupChild (compositeNode,
                                        (void*) &NW_WmlScript_ContentHandler_Class);
    if (scriptContentHandler != NULL) {
      url = NW_Str_Newcpy (NW_WmlScript_ContentHandler_Get_Url (NW_WmlScript_ContentHandlerOf(scriptContentHandler)));
      NW_THROW_OOM_ON_NULL (url, status);
    }

    /* done with the script content handler, get rid of it */
    NW_HED_CompositeNode_DeleteChild (compositeNode,
                                      (void*) &NW_WmlScript_ContentHandler_Class);

	/* reset variable before returning to WML */
    rootBox->iYScrollChange = 0;

	/* 
     * Note: Restarting the Wml interpreter could result in navigation to a URL. 
     * When the script aborts, the script state on the wml interpreter is not purged.
     */
    status = NW_Wml_ReStart(&thisObj->wmlInterpreter, scrStatus);

    _NW_THROW_ON_ERROR(status);

  }
  NW_CATCH (status) {
  }
  NW_FINALLY {
    browserApp = NW_Ctx_Get(NW_CTX_BROWSER_APP, 0);
    if(docRoot == NULL) {
      NW_Str_Delete(url);
      return KBrsrFailure;
    }
    (void)docRoot->appServices->loadProgress_api.finish(browserApp, url);
    NW_Str_Delete(url);
    return status;
  } NW_END_TRY
}

/*****************************************************************

  Name:         NW_Wml1x_ContentHandler_DeleteOptionItemList()

  Description:  

  Parameters:   

  Algorithm:    

  Return Value: 

*****************************************************************/
TBrowserStatusCode 
NW_Wml1x_ContentHandler_DeleteOptionItemList(NW_Ds_DynamicArray_t *itemList)
{
  NW_Uint16 i;
  NW_Uint16 numStrings;
  NW_Ucs2 *itemString = NULL;

  NW_ASSERT(itemList != NULL);

  numStrings = NW_Ds_DarGetCount(itemList);
  for (i = 0; i < numStrings; i++) {
    itemString = (NW_Ucs2*)NW_Ds_DarGetElement(itemList, i);
    NW_Str_Delete(itemString);
  }

  NW_Ds_DarDelete(itemList);

  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
NW_HED_DomHelper_t*
NW_Wml1x_ContentHandler_GetDomHelper (NW_Wml1x_ContentHandler_t* thisObj,
                                      NW_Wml_VarList_t* varList)
{
  if (thisObj->wmlInterpreter.decoder != NULL) {
    return NW_Wml_Deck_GetDomHelper (thisObj->wmlInterpreter.decoder->domDeck, varList);
  }
  else {
    return NULL;
  }
}

/* ------------------------------------------------------------------------- */
NW_HED_DomTree_t*
NW_Wml1x_ContentHandler_GetDomTree (NW_Wml1x_ContentHandler_t *thisObj)
{
  if (thisObj->wmlInterpreter.decoder != NULL) {
    return NW_Wml_Deck_GetDomTree (thisObj->wmlInterpreter.decoder->domDeck);
  }
  else {
    return NULL;
  }
}

/* ------------------------------------------------------------------------- *
   NW_Markup_NumberCollector methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_LMgr_Box_t*
_NW_Wml1x_ContentHandler_NumberCollector_GetBoxTree (NW_Markup_NumberCollector_t* numberCollector)
{
  NW_Wml1x_ContentHandler_t* thisObj;
  NW_LMgr_Box_t* boxTree;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (numberCollector,
                                     &NW_Wml1x_ContentHandler_NumberCollector_Class));

  /* for convenience */
  thisObj = (NW_Wml1x_ContentHandler_t*)NW_Object_Aggregate_GetAggregator (numberCollector);
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_Wml1x_ContentHandler_Class));

  /* return the box tree */
  (void)NW_HED_DocumentNode_GetBoxTree (thisObj, &boxTree);
  return boxTree;
}


NW_Text_t*
_NW_Wml1x_ContentHandler_NumberCollector_GetHrefAttr (NW_Markup_NumberCollector_t* numberCollector,
                                                      NW_LMgr_Box_t* box)
{
  NW_LMgr_PropertyValue_t      value;
  NW_Wml1x_ContentHandler_t* thisObj;
  NW_Ucs2     *retString = NULL;
  TBrowserStatusCode status = KBrsrFailure;
  NW_Text_t* text = NULL;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (numberCollector,
                                     &NW_Wml1x_ContentHandler_NumberCollector_Class));

  /* for convenience */
  thisObj = (NW_Wml1x_ContentHandler_t*)NW_Object_Aggregate_GetAggregator (numberCollector);
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_Wml1x_ContentHandler_Class));

  value.integer = 0;
  (void)NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_elementId, NW_CSS_ValueType_Integer, &value);
  status = NW_Wml_GetAttribute(&(thisObj->wmlInterpreter),
                               NW_UINT16_CAST(value.integer),
                               HREF_ATTR,
                               &retString);
  if((status  == KBrsrSuccess) && (retString != NULL)) {
    /*alt string exists*/
    NW_Text_t* retUrl = NULL;
                                   
    text = (NW_Text_t*)NW_Text_UCS2_New (retString,
                                         NW_Str_Strlen(retString), 
                                         NW_Text_Flags_TakeOwnership);
    if (text)
    {
      status = NW_HED_ContentHandler_ResolveURL ((NW_HED_ContentHandler_t *)thisObj, text, &retUrl);
      if (status == KBrsrSuccess)
      {
        text = retUrl; /* NW_HED_ContentHandler_ResolveURL handles text and retUrl objects correctly */
      }
    }
  
  }
  return text;
}

/* ------------------------------------------------------------------------- */
NW_Text_t*
_NW_Wml1x_ContentHandler_NumberCollector_GetAltAttr (NW_Markup_NumberCollector_t* numberCollector,
                                                      NW_LMgr_Box_t* box)
{
  NW_LMgr_PropertyValue_t      value;
  NW_Wml1x_ContentHandler_t* thisObj;
  NW_Ucs2     *retString = NULL;
  TBrowserStatusCode status = KBrsrFailure;
  NW_Text_t* text = NULL;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (numberCollector,
                                     &NW_Wml1x_ContentHandler_NumberCollector_Class));

  /* for convenience */
  thisObj = (NW_Wml1x_ContentHandler_t*)NW_Object_Aggregate_GetAggregator (numberCollector);
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_Wml1x_ContentHandler_Class));

  value.integer = 0;
  (void)NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_elementId, NW_CSS_ValueType_Integer, &value);
  status = NW_Wml_GetAttribute(&(thisObj->wmlInterpreter),
                               NW_UINT16_CAST(value.integer),
                               ALT_ATTR,
                               &retString);
  if((status  == KBrsrSuccess) && (retString != NULL)) {
    /*alt string exists*/
    text = (NW_Text_t*)NW_Text_UCS2_New (retString,
                                         NW_Str_Strlen(retString), 
                                         NW_Text_Flags_TakeOwnership);
  }
  return text;
}
/* retrieve the boolean whether content is saved content */
/* ------------------------------------------------------------------------- */
NW_Bool
NW_Wml1x_ContentHandler_IsSaveddeck (NW_Wml1x_ContentHandler_t * thisObj)
{
  NW_ASSERT(thisObj);
  return thisObj->saveddeck;
}

/* retrieve the boolean whether content is history load */
/* ------------------------------------------------------------------------- */
NW_Bool
NW_Wml1x_ContentHandler_IsHistload (NW_Wml1x_ContentHandler_t * thisObj)
{
  NW_ASSERT(thisObj);
  return thisObj->histload;
}


/* retrieve the boolean whether content is reload */
/* ------------------------------------------------------------------------- */
NW_Bool
NW_Wml1x_ContentHandler_IsReload (NW_Wml1x_ContentHandler_t * thisObj)
{
  NW_ASSERT(thisObj);
  return thisObj->reload;
}

/* set the boolean whether content is the saved content */
/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_Wml1x_ContentHandler_SetSaveddeck (NW_Wml1x_ContentHandler_t * thisObj, NW_Bool isSaveddeck)
{
  NW_ASSERT(thisObj);
  thisObj->saveddeck = isSaveddeck;
  return KBrsrSuccess;
}

/* set the boolean whether content is history load */
/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_Wml1x_ContentHandler_SetHistload (NW_Wml1x_ContentHandler_t * thisObj, NW_Bool isHistload)
{
  NW_ASSERT(thisObj);
  thisObj->histload = isHistload;
  return KBrsrSuccess;
}

/* set the boolean whether content is reload */
/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_Wml1x_ContentHandler_SetReload (NW_Wml1x_ContentHandler_t * thisObj, NW_Bool isReload)
{
  NW_ASSERT(thisObj);
  thisObj->reload = isReload;
  return KBrsrSuccess;
}
