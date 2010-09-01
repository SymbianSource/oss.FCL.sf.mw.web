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

#include "nw_fbox_epoc32inlineformimagesi.h"
#include "nw_browser_browserimages.h"

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
static const
NW_FBox_FormImages_Entry_t _NW_FBox_InlineFormImages_CheckBoxEntries[] = {
  {NW_TRUE,  NW_TRUE,   qgn_prop_wml_checkbox_on_sel, qgn_prop_wml_checkbox_on_sel_mask},
  {NW_FALSE, NW_TRUE,   qgn_prop_wml_checkbox_on, qgn_prop_wml_checkbox_on_mask },
  {NW_TRUE,  NW_FALSE,  qgn_prop_wml_checkbox_off_sel, qgn_prop_wml_checkbox_off_sel_mask},
  {NW_FALSE, NW_FALSE,  qgn_prop_wml_checkbox_off, qgn_prop_wml_checkbox_off_mask},
};

/* ------------------------------------------------------------------------- */
static const
NW_FBox_FormImages_Entry_t _NW_FBox_InlineFormImages_RadioBoxEntries[] = {
  {NW_TRUE,  NW_TRUE,   qgn_prop_wml_radiobutt_on_sel, qgn_prop_wml_radiobutt_on_sel_mask},
  {NW_FALSE, NW_TRUE,   qgn_prop_wml_radiobutt_on, qgn_prop_wml_radiobutt_on_mask},
  {NW_TRUE,  NW_FALSE,  qgn_prop_wml_radiobutt_off_sel, qgn_prop_wml_radiobutt_off_sel_mask},
  {NW_FALSE, NW_FALSE,  qgn_prop_wml_radiobutt_off, qgn_prop_wml_radiobutt_off_mask}
};

/* ------------------------------------------------------------------------- */
const
NW_FBox_InlineFormImages_Class_t NW_FBox_InlineFormImages_Class = {
  { /* NW_Object_Core       */
    /* super                */ &NW_FBox_FormImages_Class,
    /* queryInterface       */ _NW_Object_Base_QueryInterface
  },
  { /* NW_FBox_FormImages   */
    /* numCheckBoxEntries   */ 4,
    /* checkBoxEntries      */ _NW_FBox_InlineFormImages_CheckBoxEntries,
    /* numCheckBoxEntries   */ 4,
    /* checkBoxEntries      */ _NW_FBox_InlineFormImages_RadioBoxEntries
  },
  {/* NW_FBox_InlineFormImages   */
    /* unused               */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- */
const NW_FBox_FormImages_t NW_FBox_FormImages = {
  {&NW_FBox_InlineFormImages_Class}
};

