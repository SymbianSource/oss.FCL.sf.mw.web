/*
* Copyright (c) 1999 - 2001 Nokia Corporation and/or its subsidiary(-ies).
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


/*****************************************************************
**  File name:  nwx_generic_dlg.c
**  Part of: Debug
**  Version: 1.0
**  Description: Provides interfaces to building generic dialog boxes 
******************************************************************/
/*
**-------------------------------------------------------------------------
**  Component Generic Include
**-------------------------------------------------------------------------
*/
#include "nwx_defs.h"
#include "nwx_generic_dlg.h"

/*
**-------------------------------------------------------------------------
**  Global Variable Definitions
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  External Public (Exported) Functions
**-------------------------------------------------------------------------
*/

/*****************************************************************
**  Name:   NW_Dlg_ListSelectInit
**  Description:  Helper function which creates the data struct for a listbox
**  Parameters:   title         - title for the dialog box
**                prompt        - prompt for the dialog box 
**                itemsCount    - how many items in the list 
**                items         - strings to display in the select box
**                button0       - The text for the OK button
**                button1       - The text for the cancel button
**                selectedItem  - The item that was selected when the dialog box closed
**                selectedButton- The user closed with OK or cancel
**                context       - context for the calling 
**  Return Value: pointer to the struct or NULL if failure
******************************************************************/
NW_Dlg_ListSelect_t* NW_Dlg_ListSelectInit(const NW_Ucs2* title,const NW_Ucs2* prompt, NW_Uint8 uiOptions,
                                           NW_Uint16 itemsCount, NW_Ucs2 **items,
                                           NW_DialogButtons_t button0, NW_DialogButtons_t button1,
                                           NW_Uint8 selectedItem, NW_Uint8 selectedButton,void* context)
{
  NW_Dlg_ListSelect_t* data = 
        (NW_Dlg_ListSelect_t*) NW_Mem_Malloc(sizeof(NW_Dlg_ListSelect_t));

  if (data != NULL)
  {
    data->title=title;
    data->prompt=prompt;
    data->uiOptions = uiOptions;
    data->itemsCount=itemsCount;
    data->items=items;
    data->button0=button0;
    data->button1=button1;
    data->selectedItem=selectedItem;
    data->selectedButton=selectedButton;
    data->context=context;
  }
  return data;
}

/*****************************************************************
**  Name:   NW_Dlg_PromptInit
**  Description:  Helper function which creates the data struct for a listbox
**  Parameters:   title         - title for the dialog box
**                prompt        - prompt for the dialog box 
**                button0       - The text for the OK button
**                button1       - The text for the cancel button
**                selectedButton- The user closed with OK or cancel
**                context       - context for the calling 
**  Return Value: pointer to the struct or NULL if failure
******************************************************************/
NW_Dlg_Prompt_t* NW_Dlg_PromptInit(const NW_Ucs2* title,const NW_Ucs2* prompt, NW_Uint8 uiOptions,
                                   NW_DialogButtons_t button0, NW_DialogButtons_t button1,
                                   NW_Uint8 selectedButton,void* context)
{
  NW_Dlg_Prompt_t* data = 
        (NW_Dlg_Prompt_t*) NW_Mem_Malloc(sizeof(NW_Dlg_Prompt_t));

  if (data != NULL)
  {
    data->title=title;
    data->prompt=prompt;
    data->uiOptions = uiOptions;
    data->button0=button0;
    data->button1=button1;
    data->selectedButton=selectedButton;
    data->context=context;
  }
  return data;
}

/*****************************************************************
**  Name:   NW_Dlg_InputPromptInit
**  Description:  Helper function which creates the data struct for a listbox
**  Parameters:   title         - title for the dialog box
**                prompt        - prompt for the dialog box 
**                format        - WML input format string
**                emptyOKStr    - value of WML input attribute emptyok 
**                shouldHide    - flag if to display the text or asterisks, for PIN
**                isNumber      - flag if allow digital number only
**                button0       - The text for the OK button
**                button1       - The text for the cancel button
**                maxInputLen   - the size of the input buffer
**                input         - the input text 
**                selectedButton- The user closed with OK or cancel
**                context       - context for the calling 
**  Return Value: pointer to the struct or NULL if failure
******************************************************************/
NW_Dlg_InputPrompt_t* NW_Dlg_InputPromptInit(const NW_Ucs2* title, const NW_Ucs2* prompt, 
                                             const NW_Ucs2* format, const NW_Ucs2* emptyOKStr,
                                             NW_Uint8 uiOptions,
                                             NW_Bool shouldHide, NW_Bool isNumber, 
                                             NW_DialogButtons_t button0,
                                             NW_DialogButtons_t button1, NW_Uint16 maxInputLen,
                                             NW_Ucs2* input, NW_Uint8 selectedButton,void* context)
{
  NW_Dlg_InputPrompt_t* data = 
        (NW_Dlg_InputPrompt_t*) NW_Mem_Malloc(sizeof(NW_Dlg_InputPrompt_t));

  if (data != NULL)
  {
    data->title=title;
    data->prompt=prompt;
    data->format=(NW_Ucs2*)format; 
    data->emptyOKStr = emptyOKStr;
    data->uiOptions = uiOptions;
    data->shouldHide = shouldHide;
    data->isNumber = isNumber;
    data->button0=button0;
    data->button1=button1;
    data->maxInputLen=maxInputLen;
    data->input=input;
    data->selectedButton=selectedButton;
    data->context=context;
  }
  return data;
}

