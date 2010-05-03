/*
* Copyright (c) 2000-2004 Nokia Corporation and/or its subsidiary(-ies).
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


#include "nw_fbox_skini.h"

#include "nw_fbox_formbox.h"
#include "nwx_string.h"
#include "nw_text_ucs2.h"
#include "GDIDevicecontext.h"
#include "BrsrStatusCodes.h"

static const NW_Ucs2 _NW_LMgr_Text_Truncate[] = {'.','.','.','\0'};
#define NW_LMgr_Text_Truncate_len ((sizeof(_NW_LMgr_Text_Truncate)\
                                             /sizeof(NW_Ucs2))-1)

/* ----------------------------------------------------------------------- *
   This routine will replace all non printable characters with spaces.
 * ----------------------------------------------------------------------- */
NW_Text_t*
NW_FBox_Skin_NonprintableToSpace(NW_Text_t *text)
{
  NW_Ucs2* string;
  NW_Text_Length_t numCharacters;
  NW_Text_Length_t index;
  NW_Bool modified;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (text, &NW_Text_Abstract_Class));

  /* get a copy of the string managed by the text object */
  string = NW_Text_GetUCS2Buffer (text, NW_Text_Flags_Copy,
                                  &numCharacters, NULL);
  if (string == NULL) {
    return NULL;
  }
  
  /* iterate through each character converting them as necessary */
  modified = NW_FALSE;
  for (index = 0; index < numCharacters; index++) {
    if (string[index] != 0x0020 /* <SP> */ &&
        NW_Str_Isspace (string[index])) {
      string[index] = 0x0020; /* <SP> */
      modified = NW_TRUE;
    }
  }

  /* if we didn't modify the string, we can simply return the original text
     object */
  if (!modified) {
    NW_Mem_Free (string);
    return text;
    }

  return (NW_Text_t*)
    NW_Text_UCS2_New (string, numCharacters, NW_Text_Flags_TakeOwnership);
}


/* ------------------------------------------------------------------------- */
TBrowserStatusCode 
NW_LMgr_Text_GetDisplayLength(CGDIDeviceContext* deviceContext,
															NW_Text_t* text,
                              CGDIFont* font,
															NW_Text_Length_t totalLength,
															NW_Uint16 width,
															NW_Text_Length_t* displayLength)
  {
  NW_GDI_Dimension2D_t extent;
  NW_GDI_Dimension2D_t truncateExtent;
  NW_Text_UCS2_t* truncateText;
  TBrowserStatusCode status = KBrsrSuccess;
  NW_Bool OverFlow = NW_FALSE;
  NW_Text_Length_t i;
  
  truncateText = NW_Text_UCS2_New ((NW_Ucs2*) _NW_LMgr_Text_Truncate, NW_LMgr_Text_Truncate_len, 0);
  
  if (truncateText == NULL)
    {
    return KBrsrOutOfMemory;
    }
  
  status =
    deviceContext->GetTextExtent ( NW_TextOf(truncateText),
                                   font,
                                   0, 
                                   NW_GDI_FlowDirection_LeftRight, 
                                   &truncateExtent );
  if (status != KBrsrSuccess)
    {
    NW_Object_Delete(truncateText);
    return status;
    }
  
  NW_Object_Delete(truncateText);
  extent.width = 0;
  extent.height = 0;
  for (i=1; i<=totalLength; i++) 
    {
    status = deviceContext->GetSubstringExtent(text, 
                                               font,
                                               0, 
                                               i, 
                                               0, 
                                               NW_GDI_FlowDirection_LeftRight, 
                                               &extent);
    if (status != KBrsrSuccess)
      {
      return status;
      }
    if(!OverFlow)
      {
      if(extent.width > width)
        {
        i = (NW_Text_Length_t)((i > 3) ? (i - 3) : 0);
        OverFlow = NW_TRUE;
        }
      continue;
      }
    else if ((extent.width + truncateExtent.width) > width) 
      {
      break;
      }
    }
  *displayLength = --i;
  return status;
  }

/* ------------------------------------------------------------------------- */
const
NW_FBox_Skin_Class_t  NW_FBox_Skin_Class = {
  { /* NW_Object_Core             */
    /* super                      */ &NW_Object_Dynamic_Class,
    /* queryInterface             */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base             */
    /* interfaceList              */ NULL
  },
  { /* NW_Object_Dynamic          */
    /* instanceSize               */ sizeof (NW_FBox_Skin_t),
    /* construct                  */ _NW_FBox_Skin_Construct,
    /* destruct                   */ NULL
  },
  { /* NW_FBox_Skin               */
    /* NW_FBox_Skin_Draw_t        */ NULL,
    /* NW_FBox_Skin_GetSize_t     */ NULL
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_FBox_Skin_Construct (NW_Object_Dynamic_t* dynamicObject,
                         va_list* argp)
{
  NW_FBox_Skin_t* thisObj;

  /* for convenience */
  thisObj = NW_FBox_SkinOf (dynamicObject);

  /* initialize the member variables */
  thisObj->formBox = va_arg (*argp, NW_FBox_FormBox_t*);

  /* successful completion */
  return KBrsrSuccess;
}

