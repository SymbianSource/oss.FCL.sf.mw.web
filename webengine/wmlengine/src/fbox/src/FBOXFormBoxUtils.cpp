/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies).
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


#include "nw_fbox_formboxutils.h"
#include "nwx_defs.h"
#include "nwx_string.h"
#include "nw_fbox_formbox.h"
#include "GDIDeviceContext.h"
#include "BrsrStatusCodes.h"


/* ------------------------------------------------------------------------- *
    private data typed
 * ------------------------------------------------------------------------- */

const NW_Ucs2 EscapeChar = '\\';
const NW_Ucs2 NW_FBox_FormBoxUtils_Truncate[] = {'.','.','.','\0'};
const NW_Ucs2 NW_FBox_FormBoxUtils_TrueStr[] = { 't', 'r', 'u', 'e', '\0' };
const NW_Ucs2 NW_FBox_FormBoxUtils_FalseStr[] = { 'f', 'a', 'l', 's', 'e', '\0' };

/* ------------------------------------------------------------------------- */
TBrowserStatusCode 
NW_FBox_FormBoxUtils_GetDisplayLength(CGDIDeviceContext* deviceContext,
																			NW_Text_t* text,
                                      CGDIFont* font,
																			NW_Text_Length_t totalLength,
																			NW_Uint16 width,
																			NW_Text_Length_t* displayLength,
																			NW_Bool isNumber)
  {
  NW_GDI_Dimension2D_t extent;
  NW_GDI_Dimension2D_t truncate_extent;
  NW_Text_UCS2_t* truncate_text;
  TBrowserStatusCode status = KBrsrSuccess;
  NW_Bool OverFlow = NW_FALSE;
  NW_Text_Length_t i;
	NW_Text_Length_t start = 0;
  truncate_text = NW_Text_UCS2_New ((NW_Ucs2*) NW_FBox_FormBoxUtils_Truncate, 
                                    NW_FBox_FormBoxUtils_Truncate_len, 
                                    0);
  if (truncate_text == NULL)
    {
    return KBrsrOutOfMemory;
    }
  status = deviceContext->GetTextExtent ( NW_TextOf(truncate_text),
                                          font,
                                          0, 
                                          NW_GDI_FlowDirection_LeftRight, 
                                          &truncate_extent);
  if (status != KBrsrSuccess)
    {
    NW_Object_Delete(truncate_text);
    return status;
    }
  
  NW_Object_Delete(truncate_text);
  extent.width = 0;
  extent.height = 0;
  for (i=1; i<=totalLength; i++) 
    {
		if(isNumber)
		  {
			start = (NW_Text_Length_t) (totalLength - i);
		  }
    status = deviceContext->GetSubstringExtent( text, 
                                                font,
                                                start, 
                                                i,
                                                0, 
                                                NW_GDI_FlowDirection_LeftRight, 
                                                &extent );
    if (status != KBrsrSuccess)
      {
      return status;
      }
    if(!OverFlow)
      {
      if(extent.width > width)
        {
        if(i > 3) 
          {
					i = (NW_Text_Length_t) (i - 3);
				  }
				else 
          {
					i = 0;
			  	}
        OverFlow = NW_TRUE;
        }
      continue;
      }
    else if ((extent.width + truncate_extent.width) > width) 
      {
      break;
      }
    }
  *displayLength = --i;
  return status;
}

/* ------------------------------------------------------------------------- */
NW_HED_DocumentRoot_t *
NW_FBox_FormBoxUtils_GetDocRoot(NW_LMgr_Box_t* box)
{
  NW_HED_DocumentRoot_t* docRoot = NULL;
  NW_FBox_FormLiaison_t  *fboxFormLiaison = NULL;
  NW_ASSERT(box != NULL);
  NW_ASSERT(NW_Object_IsInstanceOf(box, &NW_FBox_FormBox_Class));
  fboxFormLiaison = NW_FBox_FormBox_GetFormLiaison (box);
  (void) NW_FBox_FormLiaison_GetDocRoot(fboxFormLiaison, &docRoot );
  return docRoot;
}

/* ------------------------------------------------------------------------- */
NW_Uint16
NW_FBox_FormBoxUtils_GetNumEscapeChars(const NW_Ucs2* format)
{
	NW_Uint16 numEscapeChars = 0;
	if((!format) || (!*format))
	{
		return numEscapeChars;
	}
	while(*format)
	{
		if((*format == EscapeChar) && *(format + 1))
		{
			numEscapeChars++;
			format++;
		}
		format++;
	}
	return numEscapeChars;
}

/* ------------------------------------------------------------------------- */
void
NW_FBox_FormBoxUtils_FillEscapeChars(const NW_Ucs2* format, NW_Ucs2 *input, 
                                     NW_Ucs2 *retValue, NW_Uint16 retValueLen)
{
	if( (!format) || (!*format) || (!input))
	{
		return;
	}
	while(*format && retValueLen)
	{
		if((*format == EscapeChar) && *(format + 1))
		{
			format++;
			*retValue = *format;
			retValue++;
		}
		else if (*input)
		{
			*retValue = *input;
			retValue++;
			input++;
		}
		else
		{
			break;
		}
		format++;
		retValueLen --;
	}
	while(*input && retValueLen)
	{
        /*lint --e{794} Conceivable use of null pointer*/

		*retValue = *input;
		retValue++;
		input++;
		retValueLen --;
	}
}

/* ------------------------------------------------------------------------- */
void
NW_FBox_FormBoxUtils_StripEscapeChars(const NW_Ucs2* format, NW_Ucs2 *input, 
																			NW_Ucs2 *retValue, NW_Uint16 retValueLen)
{
	if( (!format) || (!*format) || (!input))
	{
		return;
	}
	while(*format && *input && retValueLen)
	{
		if((*format == EscapeChar) && *(format + 1))
		{
			format += 2;
		}
		else
		{
			*retValue = *input;
			retValue++;
			format++;
			retValueLen --;
		}
		input++;
	}
	while(*input && retValueLen)
	{
		*retValue = *input;
		retValue++;
		input++;
		retValueLen --;
	}
}
