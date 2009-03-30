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
#include "nw_wml1x_wml1xdefaultstylesheet.h"
#include "nw_lmgr_cssproperties.h"
#include "nw_text_ascii.h"
#include "nw_text_ucs2.h"
#include "nw_lmgr_textbox.h"
#include "BrsrStatusCodes.h"

/*****************************************************************

  Name:         NW_Wml1x_ParagraphDefaultStyle()

  Description:  

  Parameters:   
    prop          - In      - pointer to working variable
    containerBox  - In/Out  - pointer to box on which style will be set

  Algorithm:    

  Return Value: 

*****************************************************************/
void NW_Wml1x_ParagraphDefaultStyle (NW_LMgr_Property_t     *prop,
                                     NW_LMgr_Box_t *box)
{
  prop->type = NW_CSS_ValueType_Px;
  prop->value.integer = 10;
  NW_LMgr_Box_SetProperty(box, NW_CSS_Prop_topMargin, prop);

  
  prop->type = NW_CSS_ValueType_Px;
  prop->value.integer = 10;
  NW_LMgr_Box_SetProperty(box, NW_CSS_Prop_bottomMargin, prop);

  return;
}

/*****************************************************************

  Name:         NW_Wml1x_BigDefaultStyle()

  Description:  

  Parameters:   
    prop          - In      - pointer to working variable
    containerBox  - In/Out  - pointer to box on which style will be set

  Algorithm:    

  Return Value: 

*****************************************************************/
void NW_Wml1x_BigDefaultStyle (NW_LMgr_Property_t      *prop,
                               NW_LMgr_ContainerBox_t *containerBox)
{
  prop->type = NW_CSS_ValueType_Px;
  prop->value.integer = 17;
  NW_LMgr_Box_SetProperty(NW_LMgr_BoxOf (containerBox), 
                          NW_CSS_Prop_fontSize, prop);
  return;
}


/*****************************************************************

  Name:         NW_Wml1x_SmallDefaultStyle()

  Description:  

  Parameters:   
    prop          - In      - pointer to working variable
    containerBox  - In/Out  - pointer to box on which style will be set

  Algorithm:    

  Return Value: 

*****************************************************************/
void NW_Wml1x_SmallDefaultStyle (NW_LMgr_Property_t      *prop,
                                 NW_LMgr_ContainerBox_t *containerBox)
{
  prop->type = NW_CSS_ValueType_Px;
  prop->value.integer = 12;
  NW_LMgr_Box_SetProperty(NW_LMgr_BoxOf (containerBox), 
                          NW_CSS_Prop_fontSize, prop);
  return;
}


/*****************************************************************

  Name:         NW_Wml1x_StrongDefaultStyle()

  Description:  

  Parameters:   
    prop          - In      - pointer to working variable
    containerBox  - In/Out  - pointer to box on which style will be set

  Algorithm:    

  Return Value: 

*****************************************************************/
void NW_Wml1x_StrongDefaultStyle (NW_LMgr_Property_t       *prop,
                                  NW_LMgr_ContainerBox_t  *containerBox)
{
  prop->type = NW_CSS_ValueType_Integer;
  prop->value.integer = 900;
  NW_LMgr_Box_SetProperty(NW_LMgr_BoxOf (containerBox), 
                          NW_CSS_Prop_fontWeight, prop);
  return;
}


/*****************************************************************

  Name:         NW_Wml1x_BoldDefaultStyle()

  Description:  

  Parameters:   
    prop          - In      - pointer to working variable
    containerBox  - In/Out  - pointer to box on which style will be set

  Algorithm:    

  Return Value: 

*****************************************************************/
void NW_Wml1x_BoldDefaultStyle (NW_LMgr_Property_t       *prop,
                                NW_LMgr_ContainerBox_t  *containerBox)
{
  prop->type = NW_CSS_ValueType_Integer;
  prop->value.integer = 900;
  NW_LMgr_Box_SetProperty(NW_LMgr_BoxOf (containerBox), 
                          NW_CSS_Prop_fontWeight, prop);
  return;
}


/*****************************************************************

  Name:         NW_Wml1x_EmphasisDefaultStyle()

  Description:  

  Parameters:   
    prop          - In      - pointer to working variable
    containerBox  - In/Out  - pointer to box on which style will be set

  Algorithm:    

  Return Value: 

*****************************************************************/
void NW_Wml1x_EmphasisDefaultStyle (NW_LMgr_Property_t       *prop,
                                    NW_LMgr_ContainerBox_t  *containerBox)
{
  prop->type = NW_CSS_ValueType_Token;
  prop->value.token = NW_CSS_PropValue_italic;
  NW_LMgr_Box_SetProperty(NW_LMgr_BoxOf (containerBox), 
                          NW_CSS_Prop_fontStyle, prop);
  return;
}

/*****************************************************************

  Name:         NW_Wml1x_ItalicDefaultStyle()

  Description:  

  Parameters:   
    prop          - In      - pointer to working variable
    containerBox  - In/Out  - pointer to box on which style will be set

  Algorithm:    

  Return Value: 

*****************************************************************/
void NW_Wml1x_ItalicDefaultStyle (NW_LMgr_Property_t       *prop,
                                  NW_LMgr_ContainerBox_t  *containerBox)
{
  prop->type = NW_CSS_ValueType_Token;
  prop->value.token = NW_CSS_PropValue_italic;
  NW_LMgr_Box_SetProperty(NW_LMgr_BoxOf (containerBox), 
                          NW_CSS_Prop_fontStyle, prop);
  return;
}


/*****************************************************************

  Name:         NW_Wml1x_UnderlineDefaultStyle()

  Description:  

  Parameters:   
    prop          - In      - pointer to working variable
    containerBox  - In/Out  - pointer to box on which style will be set

  Algorithm:    

  Return Value: 

*****************************************************************/
void NW_Wml1x_UnderlineDefaultStyle (NW_LMgr_Property_t      *prop,
                                     NW_LMgr_ContainerBox_t *containerBox)
{
  prop->type = NW_CSS_ValueType_Token;
  prop->value.token = NW_CSS_PropValue_underline;
  NW_LMgr_Box_SetProperty(NW_LMgr_BoxOf (containerBox), 
                          NW_CSS_Prop_textDecoration,
                          prop);
  return;
}


/*****************************************************************

  Name:         NW_Wml1x_DefaultStyle()

  Description:  

  Parameters:   
    prop          - In      - pointer to working variable
    containerBox  - In/Out  - pointer to box on which style will be set

  Algorithm:    

  Return Value: 

*****************************************************************/
void NW_Wml1x_DefaultStyle (NW_LMgr_Property_t       *prop,
                            NW_LMgr_ContainerBox_t  *containerBox)
{
  prop->type = NW_CSS_ValueType_Token;
  prop->value.token = NW_CSS_PropValue_normal;
  NW_LMgr_Box_SetProperty(NW_LMgr_BoxOf (containerBox), NW_CSS_Prop_textDecoration,
                          prop);

  return;
}


/*****************************************************************

  Name:         NW_Wml1x_ImageWithinAnchorDefaultStyle()

  Description:  

  Parameters:   
    prop - In      - pointer to working variable
    box  - In/Out  - pointer to box on which style will be set

  Algorithm:    

  Return Value: 

*****************************************************************/
void NW_Wml1x_ImageWithinAnchorDefaultStyle (NW_LMgr_Property_t  *prop,
                                             NW_LMgr_Box_t      *box)
{
  NW_LMgr_Box_t *activeContainerBox = NULL;
  activeContainerBox = 
    NW_LMgr_Box_FindBoxOfClass ((NW_LMgr_Box_t*) NW_LMgr_Box_GetParent (box), 
                                &NW_LMgr_ActiveContainerBox_Class, NW_FALSE);
  if(activeContainerBox) 
  {
    prop->type = NW_CSS_ValueType_Px;
    prop->value.integer = 1;
    NW_LMgr_Box_SetProperty(box, 
                            NW_CSS_Prop_padding, prop);
   
    NW_LMgr_Box_GetPropertyFromList(activeContainerBox, 
                            NW_CSS_Prop_borderColor, prop);
    NW_LMgr_Box_SetProperty(box, 
                            NW_CSS_Prop_borderColor, prop);

    prop->type = NW_CSS_ValueType_Token;
    prop->value.token = NW_CSS_PropValue_growBorder;
    NW_LMgr_Box_SetProperty(box, 
                            NW_CSS_Prop_focusBehavior, prop);
  }

  return;
}


/*****************************************************************

  Name:         NW_Wml1x_DefaultCardStyle()

  Description:  

  Parameters:   
    prop          - In      - pointer to working variable
    containerBox  - In/Out  - pointer to box on which style will be set

  Algorithm:    

  Return Value: 

*****************************************************************/
void NW_Wml1x_DefaultCardStyle (NW_LMgr_Property_t       *prop,
                                NW_LMgr_ContainerBox_t  *containerBox)
{
  prop->type = NW_CSS_ValueType_Px;

  prop->value.integer = 2;
  NW_LMgr_Box_SetProperty(NW_LMgr_BoxOf (containerBox), 
                          NW_CSS_Prop_leftMargin, prop);
  NW_LMgr_Box_SetProperty(NW_LMgr_BoxOf (containerBox), 
                          NW_CSS_Prop_topMargin, prop);
  NW_LMgr_Box_SetProperty(NW_LMgr_BoxOf (containerBox), 
                          NW_CSS_Prop_padding, prop);    

  prop->value.integer = 13;
  NW_LMgr_Box_SetProperty(NW_LMgr_BoxOf (containerBox), 
                          NW_CSS_Prop_fontSize, prop);
  return;
}


/*****************************************************************

  Name:         NW_Wml1x_AnchorDefaultStyle()

  Description:  

  Parameters:   
    prop          - In      - pointer to working variable
    containerBox  - In/Out  - pointer to box on which style will be set

  Algorithm:    

  Return Value: KBrsrSuccess
                KBrsrOutOfMemory
                TBrowserStatusCode from NW_LMgr_ContainerBox_InsertChild()

*****************************************************************/
TBrowserStatusCode NW_Wml1x_AnchorDefaultStyle (NW_LMgr_Property_t            *prop,
                                         NW_LMgr_ActiveContainerBox_t *containerBox,
                                         NW_Bool inCache)
{
  TBrowserStatusCode status = KBrsrSuccess;
  NW_LMgr_Property_t   saveProp;               /* to check if <pre> element involved */

  /* apply style for anchor */
  prop->type = NW_CSS_ValueType_Token;
  prop->value.token = NW_CSS_PropValue_underline;
  NW_LMgr_Box_SetProperty(NW_LMgr_BoxOf (containerBox), 
                         NW_CSS_Prop_textDecoration, prop);   

  
  prop->type = NW_CSS_ValueType_Color;
  /* need to change this to some #define */
  if (!inCache){
    prop->value.integer = 0x0000ff; /* not cached */
  }
  else{
    prop->value.integer = 0x800080; /* cached */
  }
  NW_LMgr_Box_SetProperty(NW_LMgr_BoxOf (containerBox), 
                         NW_CSS_Prop_color, prop);

  prop->type = NW_CSS_ValueType_Token;
  prop->value.token = NW_CSS_PropValue_display_inline;
  NW_LMgr_Box_SetProperty (NW_LMgr_BoxOf (containerBox), NW_CSS_Prop_display, prop);

  /* if NO <pre> element precedes this anchor - insert a blank character before the link */
  NW_LMgr_Box_GetProperty (NW_LMgr_BoxOf (containerBox), NW_CSS_Prop_whitespace, &saveProp);
  if (saveProp.value.token != NW_CSS_PropValue_pre)
  {
    NW_LMgr_ContainerBox_t  *parentBox;
    NW_LMgr_TextBox_t       *newTextBox;
    NW_Ucs2                 *retString = NULL;
    NW_Text_UCS2_t          *temptext;

    /* create some leading and trailing space for the link text */

    /* The <a> element is currently represented by an active container box, and
     * it will (hopefully) have had one or more children added to represent the link.
     * Create a text box with text " " and insert in front of the active container box
     * Create a second text box with text " " and insert following the active container box
     */

    /* TODO: the "correct" way to do this would be to only add the extra space if needed
     */

    parentBox = NW_LMgr_Box_GetParent (containerBox);

    /* new box for the leading space */
    retString = NW_Str_NewcpyConst(" ");
    if (retString == NULL) {
      return KBrsrOutOfMemory;
    }
    temptext = NW_Text_UCS2_New (retString, (NW_Text_Length_t)NW_Str_Strlen(retString), 1/* takeOwnership */);
    if (temptext == NULL) {
      return KBrsrOutOfMemory; /* NW_Text_UCS2_New() takes care of deleting string when fails */
    }
    newTextBox = NW_LMgr_TextBox_New(1, NW_TextOf(temptext));
    if (newTextBox == NULL) {
      NW_Object_Delete(temptext);
      return KBrsrOutOfMemory;
    }

    /* hook in the new text box for the leading space */
    status = NW_LMgr_ContainerBox_InsertChild(NW_LMgr_ContainerBoxOf(parentBox),
                                              NW_LMgr_BoxOf(newTextBox),
                                              NW_LMgr_BoxOf(containerBox));
    if (status != KBrsrSuccess) {
      NW_Object_Delete(newTextBox);
      return status;
    }

    /* this is where another box with a trailing space would be added, if we had the memory */

  }
  return status;
}


/*****************************************************************

  Name:         NW_Wml1x_InputDefaultStyle()

  Description:  

  Parameters:   
    inputBox      - In/Out  - pointer to box on which style will be set

  Algorithm:    

  Return Value: none

*****************************************************************/
void NW_Wml1x_InputDefaultStyle (NW_LMgr_Box_t *inputBox)
{
  NW_LMgr_Property_t prop;

  prop.type = NW_CSS_ValueType_Px;
  prop.value.integer = 1;
  NW_LMgr_Box_SetProperty(inputBox, NW_CSS_Prop_borderWidth, &prop);

  prop.type = NW_CSS_ValueType_Token;
  prop.value.integer = NW_CSS_PropValue_solid;
  NW_LMgr_Box_SetProperty(inputBox, NW_CSS_Prop_borderStyle, &prop);

  prop.type = NW_CSS_ValueType_Px;
  prop.value.integer = 2;
  NW_LMgr_Box_SetProperty(inputBox, NW_CSS_Prop_padding, &prop);

  prop.type = NW_CSS_ValueType_Token;
  prop.value.integer = NW_CSS_PropValue_growBorder;
  NW_LMgr_Box_SetProperty(inputBox, NW_CSS_Prop_focusBehavior, &prop);

  return;
}


/*****************************************************************

  Name:         NW_Wml1x_SelectDefaultStyle()

  Description:  

  Parameters:   
    selectBox     - In/Out  - pointer to box on which style will be set

  Algorithm:    

  Return Value: none

*****************************************************************/
void NW_Wml1x_SelectDefaultStyle (NW_LMgr_Box_t *selectBox)
{
  NW_LMgr_Property_t prop;

  prop.type = NW_CSS_ValueType_Px;
  prop.value.integer = 1;
  NW_LMgr_Box_SetProperty(selectBox, NW_CSS_Prop_borderWidth, &prop);

  prop.type = NW_CSS_ValueType_Token;
  prop.value.integer = NW_CSS_PropValue_solid;
  NW_LMgr_Box_SetProperty(selectBox, NW_CSS_Prop_borderStyle, &prop);

  prop.type = NW_CSS_ValueType_Px;
  prop.value.integer = 2;
  NW_LMgr_Box_SetProperty(selectBox, NW_CSS_Prop_padding, &prop);

  prop.type = NW_CSS_ValueType_Token;
  prop.value.integer = NW_CSS_PropValue_growBorder;
  NW_LMgr_Box_SetProperty(selectBox, NW_CSS_Prop_focusBehavior, &prop);

  return;
}

void NW_Wml1x_FieldsetDefaultStyle(NW_LMgr_Box_t* box)
{
  NW_LMgr_Property_t prop;

  prop.type = NW_CSS_ValueType_Px;
  prop.value.integer = 10;
  NW_LMgr_Box_SetProperty(box, NW_CSS_Prop_topMargin, &prop);
  NW_LMgr_Box_SetProperty(box, NW_CSS_Prop_bottomMargin, &prop);

}
