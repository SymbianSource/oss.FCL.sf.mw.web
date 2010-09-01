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


#ifndef NW_ABSTRACTTEXTBOX_H
#define NW_ABSTRACTTEXTBOX_H

#include "nw_lmgr_mediabox.h"
#include "NW_Text_Abstract.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_LMgr_AbstractTextBox_Class_s NW_LMgr_AbstractTextBox_Class_t;
typedef struct NW_LMgr_AbstractTextBox_s NW_LMgr_AbstractTextBox_t;

/* ------------------------------------------------------------------------- *
   method prototypes
 * ------------------------------------------------------------------------- */
typedef
NW_Text_t*
(*NW_LMgr_AbstractTextBox_GetText_t)(NW_LMgr_AbstractTextBox_t *box);

typedef
NW_Text_Length_t
(*NW_LMgr_AbstractTextBox_GetStart_t)(NW_LMgr_AbstractTextBox_t *box);

typedef struct NW_LMgr_AbstractTextBox_ClassPart_s {
  NW_LMgr_AbstractTextBox_GetText_t getText;
  NW_LMgr_AbstractTextBox_GetStart_t getStart;
} NW_LMgr_AbstractTextBox_ClassPart_t;

/* ------------------------------------------------------------------------- */
struct NW_LMgr_AbstractTextBox_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_Box_ClassPart_t NW_LMgr_Box;
  NW_LMgr_MediaBox_ClassPart_t NW_LMgr_MediaBox;
  NW_LMgr_AbstractTextBox_ClassPart_t NW_LMgr_AbstractTextBox;
};

/* ------------------------------------------------------------------------- */

struct NW_LMgr_AbstractTextBox_s {
  NW_LMgr_MediaBox_t super;
  NW_Text_Length_t length;
};

/* ------------------------------------------------------------------------- */
#define NW_LMgr_AbstractTextBox_GetClassPart(_object) \
(NW_Object_GetClassPart(_object, NW_LMgr_AbstractTextBox))


#define NW_LMgr_AbstractTextBoxOf(_object) \
((NW_LMgr_AbstractTextBox_t*) (_object))

/* ------------------------------------------------------------------------- */
NW_LMGR_EXPORT const NW_LMgr_AbstractTextBox_Class_t NW_LMgr_AbstractTextBox_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_LMgr_AbstractTextBox_CompressLeading(_box) \
  ((_box)->length = (NW_Uint16)((_box)->length | 0x8000))
#define NW_LMgr_AbstractTextBox_CompressTrailing(_box) \
  ((_box)->length = (NW_Uint16)((_box)->length | 0x4000))
#define NW_LMgr_AbstractTextBox_PreserveLeading(_box) \
  ((_box)->length = (NW_Uint16)((_box)->length & 0x8000))
#define NW_LMgr_AbstractTextBox_PreserveTrailing(_box) \
  ((_box)->length = (NW_Uint16)((_box)->length & 0x4000))
#define NW_LMgr_AbstractTextBox_IsCompressedLeading(_box) \
  ((NW_Uint16)((_box)->length & 0x8000) ? NW_TRUE : NW_FALSE)
#define NW_LMgr_AbstractTextBox_IsCompressedTrailing(_box) \
  ((NW_Uint16)((_box)->length & 0x4000) ? NW_TRUE : NW_FALSE)
#define NW_LMgr_AbstractTextBox_GetLength(_box) \
  ((NW_Uint16)((_box)->length & 0x3fff))
#define NW_LMgr_AbstractTextBox_SetLength(_box, _length) \
  ((_box)->length = (NW_Uint16)(((_box)->length & 0xc000) + ((_length) & 0x3fff)))
#define NW_LMgr_AbstractTextBox_GetText(_object) \
  (NW_LMgr_AbstractTextBox_GetClassPart (_object).getText ((_object)))
#define NW_LMgr_AbstractTextBox_GetStart(_object) \
  (NW_LMgr_AbstractTextBox_GetClassPart (_object).getStart ((_object)))


NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_AbstractTextBox_GetDimensions (NW_LMgr_AbstractTextBox_t* box, NW_GDI_Dimension2D_t *size);

NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_AbstractTextBox_GetMinimumSizeByWord (NW_LMgr_AbstractTextBox_t* box, NW_GDI_Dimension2D_t *size);

NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_AbstractTextBox_GetWS (NW_LMgr_AbstractTextBox_t *textBox, 
                               NW_Bool *hasLeading,
                               NW_Bool *hasTrailing,
                               NW_Bool *endsInNewline); 

NW_LMGR_EXPORT
NW_Text_t*
NW_LMgr_AbstractTextBox_GetDisplayText (NW_LMgr_AbstractTextBox_t* textBox,
                                        NW_Bool skipWS);
NW_LMGR_EXPORT
NW_Bool
NW_LMgr_AbstractTextBox_IsBlank (NW_LMgr_AbstractTextBox_t* textBox);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_AbstractTextBox_H */
