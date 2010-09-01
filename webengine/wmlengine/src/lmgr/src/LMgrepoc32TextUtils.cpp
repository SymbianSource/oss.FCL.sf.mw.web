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


#include "nw_lmgr_textutils.h"

#include "nwx_defs.h"
#include "nwx_string.h"
#include "GDIDeviceContext.h"
#include "BrsrStatusCodes.h"

#pragma message(__FILE__ " TODO:Currently just a copy of ISA implementation.  Change to epoc32 if necessary")

/* ------------------------------------------------------------------------- *
 * Static helper functions
 * ------------------------------------------------------------------------- */
static TBrowserStatusCode
Text_Truncate(NW_Text_t *text,
                       NW_Text_t *truncStr,
                       NW_GDI_Metric_t width,
                       CGDIDeviceContext  *deviceContext,
                       NW_Text_Length_t        *length)
{
  NW_GDI_Dimension2D_t extent;
  NW_Ucs2* bufferStorage;
  NW_Text_Length_t displayLength;
  TBrowserStatusCode status;
  NW_GDI_Dimension2D_t truncate_extent;
  NW_Text_UCS2_t* displayText;

  
  /* Copy text to temporary UCS2 text object */
  displayText =  NW_Text_UCS2_New(NULL, 0, 0) ;
  if (displayText == NULL)
    return KBrsrOutOfMemory;

  bufferStorage =
    NW_Text_GetUCS2Buffer(text, 0, &displayLength, NULL);
  if (displayLength == 0){
    NW_Object_Delete (displayText);
    return KBrsrFailure;
  }
  
  NW_Text_SetStorage (displayText, bufferStorage, displayLength, 0);
    
  /* Set the initial length */
  *length = displayLength;

  /* Get extent of string */
  extent.width = 0;extent.height = 0;
  status = deviceContext->GetTextExtent( 
                                              NW_TextOf(displayText),
                                              0, 
                                              NW_GDI_FlowDirection_LeftRight, 
                                              &extent);

  if (status != KBrsrSuccess){
    NW_Object_Delete (displayText);
    return KBrsrFailure;
  }

  /* if already fits in the display, return...no need to truncate */
  if (extent.width <= width){
    NW_Object_Delete (displayText);
    return KBrsrSuccess;
  }

  /* Loop until you find the appropriate place to clip the string */
  displayLength = (NW_Text_Length_t) (displayLength - 1);
  
  status = deviceContext->GetTextExtent( 
                                              truncStr,
                                              0, 
                                              NW_GDI_FlowDirection_LeftRight, 
                                              &truncate_extent);

  do{
    NW_Text_SetCharCount(displayText, displayLength);
    extent.width = 0;
    if (displayText){
      status = deviceContext->GetTextExtent( 
                                                  NW_TextOf(displayText),
                                                  0, 
                                                  NW_GDI_FlowDirection_LeftRight, 
                                                  &extent);
      if (status != KBrsrSuccess){
        NW_Object_Delete (displayText);
        return KBrsrFailure;
      }
    }
    
    if(extent.width <= (width - truncate_extent.width))
    {
      break;
    }
    displayLength--;
  }while (extent.width >= width - truncate_extent.width);

  *length = displayLength;
  NW_Object_Delete (displayText);
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
/* BreakText returns the position of the first break character that fits 
 * within the given constraint.  It returns a 0 if the text could not
 * be split to fit the constraint.
 *
 * Breaking can occur in one of three ways by using the flags parameter:
 * If flags is set to NW_LMgr_Text_Flags_WordWrap the text will be broken
 * along a word boundary.
 * If NW_LMgr_Text_Flags_WordWrap is not set the text will be clipped at a
 * character boundary.
 * If flags is set to NW_LMgr_Text_Flags_Truncate a truncation string (ie, "..."
 * must be specified in the truncStr parameter.  During truncation, the 
 * string will be broken at a character boundary.
 *
 * The space parameter will specify the width of the area to fit the text.
 *
 * The character position to break the text will be returned in the length
 * parameter.
 */  


TBrowserStatusCode
NW_LMgr_Text_BreakText(NW_Text_t *text,
                       NW_Text_t *truncStr,
                       NW_GDI_Metric_t space,
                       NW_Uint8 flags,
                       CGDIDeviceContext  *deviceContext,
                       NW_Text_Length_t        *length)
{
  TBrowserStatusCode           status;
  NW_Text_Length_t strLen;
  NW_Text_Length_t i;
  NW_GDI_Dimension2D_t size;
  NW_Text_Length_t guess;
  NW_Text_Length_t totalWidth;
  NW_Ucs2 ch;
  NW_Bool compressWS;
  
  compressWS = (NW_Bool)
    (flags & NW_LMgr_Text_Flags_SplitCompressed ? NW_TRUE : NW_FALSE);

  NW_ASSERT (length != NULL);


  *length = 0;  /* set return value to default */

  strLen = NW_Text_GetCharCount (text);

  status = NW_LMgr_Text_GetSubstringExtent(text, 
                                          strLen, 
                                          compressWS,
                                          &size,
                                          deviceContext);

  if (flags & NW_LMgr_Text_Flags_Truncate && !truncStr)
    return KBrsrFailure;

  if (status != KBrsrSuccess) {
    return status;
  }

  /* Word wrap the string */
  if (flags & NW_LMgr_Text_Flags_WordWrap)
  {
    NW_Text_Length_t temp = space;  // Avoids lint warning

    totalWidth = size.width;
    guess = 0;

    for (i = (NW_Text_Length_t) (temp * strLen / totalWidth); i < strLen; i++) {
      ch = (NW_Ucs2)NW_Text_GetAt (text,i);
      if (NW_LMgr_Text_CanSplit (ch, flags)) {

        status = NW_LMgr_Text_GetSubstringExtent(text, 
                                                (NW_Text_Length_t)(i+1),
                                                compressWS,
                                                &size,
                                                deviceContext);
        if (status != KBrsrSuccess) {
          return status;
        }

        if (space >= size.width) {
          guess = i;
        }
        else {
          break;
        }
      }
    }

    if (guess == 0) {
      for (i = (NW_Text_Length_t) (space * strLen / totalWidth); i > 0; i--) {
        ch = (NW_Ucs2)NW_Text_GetAt (text,i);
        if (NW_LMgr_Text_CanSplit (ch, flags)) {

          status = NW_LMgr_Text_GetSubstringExtent(text, 
                                                  (NW_Text_Length_t)(i+1),
                                                  compressWS,
                                                  &size,
                                                  deviceContext);
          if (status != KBrsrSuccess) {
            return status;
          }

          if (space >= size.width) {
            guess = i;
            break;
          }
        }
      }
    }

    /* Return result */
    *length = guess;
    status = KBrsrSuccess;
  }
  
  /* Truncate the string */
  else if (flags & NW_LMgr_Text_Flags_Truncate)
  {
    status = Text_Truncate(text,
                           truncStr,
                           space,
                           deviceContext,
                           length);
  }

  /* Clip the string */
  else
  {
    for (i=1; i<=strLen; i++) {
      status = NW_LMgr_Text_GetSubstringExtent(text, 
                                               i, 
                                               compressWS,
                                               &size,
                                               deviceContext);
      if ( status != KBrsrSuccess ) {
        return status;
      }
      if (size.width > space) {
        i--;
        break;
      }
    }

    *length = i;
    status = KBrsrSuccess;
  }

  return status;

}
