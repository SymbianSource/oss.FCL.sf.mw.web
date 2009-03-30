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


#include "nwx_string.h"
#include "nwx_settings.h"
#include "BrsrStatusCodes.h"
#include "nw_dom_node.h"
#include "nw_css_tokentoint.h"
#include "nw_lmgr_cssproperties.h"
#include "nw_lmgr_propertylist.h"
#include "nw_lmgr_accesskey.h"
#include "LMgrObjectBoxOOC.h"
#include "nw_lmgr_statictablebox.h"
#include "nw_lmgr_statictablecellbox.h"
#include "nw_lmgr_rootbox.h"
#include "nw_lmgr_animatedimagebox.h"
#include "nw_lmgr_activecontainerbox.h"
#include "nw_lmgr_Verticaltablebox.h"
#include "nw_lmgr_Verticaltablecellbox.h"
#include "nw_fbox_inputbox.h"
#include "nw_xhtml_attributepropertytablei.h"
#include "nw_xhtml_xhtml_1_0_tokens.h"
#include "nw_xhtml_xhtmlcontenthandler.h"
//#include "XhtmlObjectElementHandlerOOC.h"
#include "XhtmlTableElementHandlerOOC.h"
#include "nw_hed_domhelper.h"
#include "HEDDocumentListener.h"
#include "GDIDeviceContext.h"
#include "CSSImageList.h"

/* ------------------------------------------------------------------------- */

#define ASCII_HASH                '#'
/*#define ASCII_DOT                 '.'*/
#define ASCII_COMMA               ','
#define ASCII_PERCENT             '%'
#define ASCII_RIGHT_PARENTHESIS   ')'

static const NW_Ucs2 Submit[] = {'S','u','b','m','i','t','\0'};

static const NW_Ucs2 PropVal_none[] = {'n','o','n','e','\0'};
static const NW_Ucs2 PropVal_1[] = {'1','\0'};
static const NW_Ucs2 PropVal_a[] = {'a','\0'};
static const NW_Ucs2 PropVal_A[] = {'A','\0'};
static const NW_Ucs2 PropVal_i[] = {'i','\0'};
static const NW_Ucs2 PropVal_I[] = {'I','\0'};
static const NW_Ucs2 PropVal_all[] = {'a','l','l','\0'};
static const NW_Ucs2 PropVal_bottom[] = {'b','o','t','t','o','m','\0'};
static const NW_Ucs2 PropVal_center[] = {'c','e','n','t','e','r','\0'};
static const NW_Ucs2 PropVal_left[] = {'l','e','f','t','\0'};
static const NW_Ucs2 PropVal_middle[] = {'m','i','d','d','l','e','\0'};
static const NW_Ucs2 PropVal_right[] = {'r','i','g','h','t','\0'};
static const NW_Ucs2 PropVal_top[] = {'t','o','p','\0'};
static const NW_Ucs2 PropVal_scroll[] = {'s','c','r','o','l','l','\0'};
static const NW_Ucs2 PropVal_slide[] = {'s','l','i','d','e','\0'};
static const NW_Ucs2 PropVal_alternate[] = {'a','l','t','e','r','n','a','t','e','\0'};
static const NW_Ucs2 PropVal_noshade[] = {'n','o','s','h','a','d','e','\0'};
static const NW_Ucs2 PropVal_fixed[] = {'f','i','x','e','d','\0'};

/* color vals */
static const NW_Ucs2 PropVal_aqua[] = {'a','q','u','a','\0'};
static const NW_Ucs2 PropVal_black[] = {'b','l','a','c','k','\0'};
static const NW_Ucs2 PropVal_blue[] = {'b','l','u','e','\0'};
static const NW_Ucs2 PropVal_cyan[] = {'c','y','a','n','\0'};
static const NW_Ucs2 PropVal_fuchsia[] = {'f','u','c','h','s','i','a','\0'};
static const NW_Ucs2 PropVal_gray[] = {'g','r','a','y','\0'};
static const NW_Ucs2 PropVal_green[] = {'g','r','e','e','n','\0'};
static const NW_Ucs2 PropVal_lime[] = {'l','i','m','e','\0'};
static const NW_Ucs2 PropVal_maroon[] = {'m','a','r','o','o','n','\0'};
static const NW_Ucs2 PropVal_navy[] = {'n','a','v','y','\0'};
static const NW_Ucs2 PropVal_olive[] = {'o','l','i','v','e','\0'};
static const NW_Ucs2 PropVal_pink[] = {'p','i','n','k','\0'};
static const NW_Ucs2 PropVal_purple[] = {'p','u','r','p','l','e','\0'};
static const NW_Ucs2 PropVal_red[] = {'r','e','d','\0'};
static const NW_Ucs2 PropVal_silver[] = {'s','i','l','v','e','r','\0'};
static const NW_Ucs2 PropVal_teal[] = {'t','e','a','l','\0'};
static const NW_Ucs2 PropVal_white[] = {'w','h','i','t','e','\0'};
static const NW_Ucs2 PropVal_yellow[] = {'y','e','l','l','o','w','\0'};
static const NW_Ucs2 PropVal_transparent[] = {'t','r','a','n','s','p','a','r','e','n','t','\0'};
static const NW_Ucs2 PropVal_disc[] = {'d','i','s','c','\0'};
static const NW_Ucs2 PropVal_square[] = {'s','q','u','a','r','e','\0'};
static const NW_Ucs2 PropVal_circle[] = {'c','i','r','c','l','e','\0'};
static const NW_Ucs2 PropVal_infinite[] = {'i','n','f','i','n','i','t','e','\0'};

static const NW_Ucs2 rgb[] = {'r','g','b','(','\0'};

static const NW_Ucs2 PropVal_ltr[] = {'l','t','r','\0'};
static const NW_Ucs2 PropVal_rtl[] = {'r','t','l','\0'};

static const NW_Ucs2 PropVal_nowrap[] = {'n','o','w','r','a','p','\0'};


static const NW_Ucs2 PropVal_hiragana[] = {'h','i','r','a','g','a','n','a','\0'};
static const NW_Ucs2 PropVal_katakana[] = {'k','a','t','a','k','a','n','a','\0'};
static const NW_Ucs2 PropVal_hankakukana[] = {'h','a','n','k','a','k','u','k','a','n','a','\0'};
static const NW_Ucs2 PropVal_alphabet[] = {'a','l','p','h','a','b','e','t','\0'};
static const NW_Ucs2 PropVal_numeric[] = {'n','u','m','e','r','i','c','\0'};

/* NOTE: Attribute-specific PropVal entries should PRECEDE
         the default value (e.g., PropVal_left).                */
static const NW_XHTML_AttributePropertyTable_PropValEntry_t _NW_XHTML_AttributePropertyTable_PropValEntries[] = {
/* Fields: attrValStr  elementToken  attributeToken  CSSPropertyVal  case_sensitive */
  {PropVal_none, 0, 0, NW_CSS_PropValue_none, NW_FALSE},
  {PropVal_all, 0, 0, NW_CSS_PropValue_both, NW_FALSE},
  {PropVal_1, 0, 0, NW_CSS_PropValue_decimal, NW_FALSE},
  {PropVal_a, 0, 0, NW_CSS_PropValue_lowerAlpha, NW_TRUE},
  {PropVal_A, 0, 0, NW_CSS_PropValue_upperAlpha, NW_TRUE},
  {PropVal_i, 0, 0, NW_CSS_PropValue_lowerRoman, NW_TRUE},
  {PropVal_I, 0, 0, NW_CSS_PropValue_upperRoman, NW_TRUE},
  {PropVal_disc, 0, 0, NW_CSS_PropValue_disc, NW_FALSE},
  {PropVal_square, 0, 0, NW_CSS_PropValue_square, NW_FALSE},
  {PropVal_circle, 0, 0, NW_CSS_PropValue_circle, NW_FALSE},
  {PropVal_infinite, 0, 0, NW_CSS_PropValue_infinite, NW_FALSE},
  {PropVal_bottom, NW_XHTML_ElementToken_img, NW_XHTML_AttributeToken_align, NW_CSS_PropValue_baseline, NW_FALSE},
  {PropVal_bottom, 0, 0, NW_CSS_PropValue_bottom, NW_FALSE},
  {PropVal_center, 0, 0, NW_CSS_PropValue_center, NW_FALSE},
  {PropVal_left, NW_XHTML_ElementToken_marquee, NW_XHTML_AttributeToken_direction, NW_CSS_PropValue_rtl, NW_FALSE},
  {PropVal_left, 0, 0, NW_CSS_PropValue_left, NW_FALSE},
  {PropVal_middle, 0, 0, NW_CSS_PropValue_middle, NW_FALSE},
  {PropVal_right, NW_XHTML_ElementToken_marquee, NW_XHTML_AttributeToken_direction, NW_CSS_PropValue_ltr, NW_FALSE},
  {PropVal_right, 0, 0, NW_CSS_PropValue_right, NW_FALSE},
  {PropVal_top, 0, 0, NW_CSS_PropValue_top, NW_FALSE},
  {PropVal_scroll, 0, 0, NW_CSS_PropValue_scroll, NW_FALSE},
  {PropVal_slide, 0, 0, NW_CSS_PropValue_slide, NW_FALSE},
  {PropVal_alternate, 0, 0, NW_CSS_PropValue_alternate, NW_FALSE},
  {PropVal_noshade, 0, 0, NW_CSS_PropValue_true, NW_FALSE},
  {PropVal_aqua, 0, 0, NW_CSS_PropValue_aqua, NW_FALSE},
  {PropVal_black, 0, 0, NW_CSS_PropValue_black, NW_FALSE},
  {PropVal_blue, 0, 0, NW_CSS_PropValue_blue, NW_FALSE},
  {PropVal_cyan, 0, 0, NW_CSS_PropValue_cyan, NW_FALSE},
  {PropVal_fuchsia, 0, 0, NW_CSS_PropValue_fuchsia, NW_FALSE},
  {PropVal_gray, 0, 0, NW_CSS_PropValue_gray, NW_FALSE},
  {PropVal_green, 0, 0, NW_CSS_PropValue_green, NW_FALSE},
  {PropVal_lime, 0, 0, NW_CSS_PropValue_lime, NW_FALSE},
  {PropVal_maroon, 0, 0, NW_CSS_PropValue_maroon, NW_FALSE},
  {PropVal_navy, 0, 0, NW_CSS_PropValue_navy, NW_FALSE},
  {PropVal_olive, 0, 0, NW_CSS_PropValue_olive, NW_FALSE},
  {PropVal_pink, 0, 0, NW_CSS_PropValue_pink, NW_FALSE},
  {PropVal_purple, 0, 0, NW_CSS_PropValue_purple, NW_FALSE},
  {PropVal_red, 0, 0, NW_CSS_PropValue_red, NW_FALSE},
  {PropVal_silver, 0, 0, NW_CSS_PropValue_silver, NW_FALSE},
  {PropVal_teal, 0, 0, NW_CSS_PropValue_teal, NW_FALSE},
  {PropVal_white, 0, 0, NW_CSS_PropValue_white, NW_FALSE},
  {PropVal_yellow, 0, 0, NW_CSS_PropValue_yellow, NW_FALSE},
  {PropVal_transparent, 0, 0, NW_CSS_PropValue_transparent, NW_FALSE},
  {PropVal_ltr,  0, 0, NW_CSS_PropValue_ltr, NW_FALSE},
  {PropVal_rtl,  0, 0, NW_CSS_PropValue_rtl, NW_FALSE},
  {PropVal_fixed, 0, 0, NW_CSS_PropValue_background_fixed, NW_FALSE},
  {PropVal_nowrap, NW_XHTML_ElementToken_td, NW_XHTML_AttributeToken_nowrap, NW_CSS_PropValue_nowrap, NW_FALSE},
  {PropVal_hiragana, 0, 0, NW_CSS_PropValue_hiragana, NW_FALSE},
  {PropVal_katakana, 0, 0, NW_CSS_PropValue_katakana, NW_FALSE},
  {PropVal_hankakukana, 0, 0, NW_CSS_PropValue_hankakukana, NW_FALSE},
  {PropVal_alphabet, 0, 0, NW_CSS_PropValue_alphabet, NW_FALSE},
  {PropVal_numeric, 0, 0, NW_CSS_PropValue_numeric, NW_FALSE}
};

/* In order for border width to take effect, the border style must be solid */
static const NW_XHTML_AttributePropertyTable_CSSPropRequires_t NW_XHTML_PropertyTable_borderCSSRequires =
{ NW_CSS_Prop_borderStyle, {{NW_CSS_PropValue_solid}, NW_CSS_ValueType_Token} };

/* In order for image alignment to take effect, the display type must be block */
static const NW_XHTML_AttributePropertyTable_CSSPropRequires_t NW_XHTML_PropertyTable_floatCSSRequires =
{ NW_CSS_Prop_display, {{NW_CSS_PropValue_display_block}, NW_CSS_ValueType_Token} };

static const NW_LMgr_PropertyValueToken_t NW_XHTML_APT_AlignValues[] =
{ NW_CSS_PropValue_left, NW_CSS_PropValue_right, NW_CSS_PropValue_center, NW_CSS_PropValue_none };

static const NW_LMgr_PropertyValueToken_t NW_XHTML_APT_AttachValues[] =
{ NW_CSS_PropValue_background_fixed };

static const NW_LMgr_PropertyValueToken_t NW_XHTML_APT_FloatValues[] =
{ NW_CSS_PropValue_left, NW_CSS_PropValue_right, NW_CSS_PropValue_none };

static const NW_LMgr_PropertyValueToken_t NW_XHTML_APT_NoshadeValues[] =
{ NW_CSS_PropValue_true };

/* In order for the caption to be top/bottom aligned, it should also be centered */
static const  NW_XHTML_AttributePropertyTable_CSSPropRequires_t NW_XHTML_PropertyTable_captionAlignCSSRequires =
{ NW_CSS_Prop_textAlign, {{NW_CSS_PropValue_center}, NW_CSS_ValueType_Token} };

static const NW_LMgr_PropertyValueToken_t NW_XHTML_APT_CaptionValignValues[] =
{ NW_CSS_PropValue_top, NW_CSS_PropValue_bottom, NW_CSS_PropValue_none };

static const NW_LMgr_PropertyValueToken_t NW_XHTML_APT_ValignValues[] =
{ NW_CSS_PropValue_top, NW_CSS_PropValue_middle, NW_CSS_PropValue_bottom, NW_CSS_PropValue_none };

static const NW_LMgr_PropertyValueToken_t NW_XHTML_APT_ClearValues[] =
{ NW_CSS_PropValue_left, NW_CSS_PropValue_right, NW_CSS_PropValue_both, NW_CSS_PropValue_none };

static const NW_LMgr_PropertyValueToken_t NW_XHTML_APT_OLTypeValues[] =
{ NW_CSS_PropValue_decimal, NW_CSS_PropValue_lowerAlpha, NW_CSS_PropValue_upperAlpha, NW_CSS_PropValue_lowerRoman, NW_CSS_PropValue_upperRoman, NW_CSS_PropValue_none };

static const NW_LMgr_PropertyValueToken_t NW_XHTML_APT_ULTypeValues[] =
{ NW_CSS_PropValue_disc, NW_CSS_PropValue_square, NW_CSS_PropValue_circle, NW_CSS_PropValue_none };

static const NW_LMgr_PropertyValueToken_t NW_XHTML_APT_MarqueeBehValues[] =
{ NW_CSS_PropValue_scroll, NW_CSS_PropValue_slide, NW_CSS_PropValue_alternate, NW_CSS_PropValue_none  };

static const NW_LMgr_PropertyValueToken_t NW_XHTML_APT_MarqueeDirValues[] =
{ NW_CSS_PropValue_ltr, NW_CSS_PropValue_rtl, NW_CSS_PropValue_none };

static const NW_LMgr_PropertyValueToken_t NW_XHTML_APT_MarqueeLoopValues[] =
{ NW_CSS_PropValue_infinite, NW_CSS_PropValue_none };

static const NW_LMgr_PropertyValueToken_t NW_XHTML_APT_TextDirValues[] =
{ NW_CSS_PropValue_ltr, NW_CSS_PropValue_rtl, NW_CSS_PropValue_none };

static const NW_LMgr_PropertyValueToken_t NW_XHTML_APT_NoWrapValues[] =
{ NW_CSS_PropValue_nowrap, NW_CSS_PropValue_none };


static const NW_LMgr_PropertyValueToken_t NW_XHTML_APT_ModeValues[] =
{ NW_CSS_PropValue_hiragana, NW_CSS_PropValue_katakana, NW_CSS_PropValue_hankakukana,
  NW_CSS_PropValue_alphabet, NW_CSS_PropValue_numeric, NW_CSS_PropValue_none };


static const NW_XHTML_AttributePropertyTable_ElementEntry_t _NW_XHTML_AttributePropertyTable_Elements[] =
{   /* elementToken                  attributeToken                        CSSPropName                 type                   CSSPropVals                   requires */
    { NW_XHTML_ElementToken_zzzunknown,NW_XHTML_AttributeToken_id,         NW_CSS_Prop_boxName,        NW_XHTML_APT_STRING,   NULL,                         NULL },
    { NW_XHTML_ElementToken_a,       NW_XHTML_AttributeToken_name,         NW_CSS_Prop_boxName,        NW_XHTML_APT_STRING,   NULL,                         NULL },
    { NW_XHTML_ElementToken_a,       NW_XHTML_AttributeToken_tabindex,     NW_CSS_Prop_tabIndex,       NW_XHTML_APT_INTEGER,  NULL,                         NULL },
    { NW_XHTML_ElementToken_a,       NW_XHTML_AttributeToken_align,        NW_CSS_Prop_textAlign,      NW_XHTML_APT_TOKEN,    NW_XHTML_APT_AlignValues,     NULL },
    { NW_XHTML_ElementToken_a,       NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_abbr,    NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_acronym, NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_address, NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_area,    NW_XHTML_AttributeToken_shape,        NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_area,    NW_XHTML_AttributeToken_coords,       NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_b,       NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_big,     NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_body,    NW_XHTML_AttributeToken_bgcolor,      NW_CSS_Prop_backgroundColor,NW_XHTML_APT_COLOR,    NULL,                         NULL },
    { NW_XHTML_ElementToken_body,    NW_XHTML_AttributeToken_text,         NW_CSS_Prop_color,          NW_XHTML_APT_COLOR,    NULL,                         NULL },
    { NW_XHTML_ElementToken_body,    NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_body,    NW_XHTML_AttributeToken_background,   NW_CSS_Prop_backgroundImage,NW_XHTML_APT_STRING,   NULL,                         NULL },
    { NW_XHTML_ElementToken_body,    NW_XHTML_AttributeToken_bgproperties, NW_CSS_Prop_backgroundAttachment,     NW_XHTML_APT_TOKEN,    NW_XHTML_APT_AttachValues,    NULL },
    { NW_XHTML_ElementToken_body,    NW_XHTML_AttributeToken_color,        NW_CSS_Prop_color,          NW_XHTML_APT_COLOR,    NULL,                         NULL },
/* Coming soon, should be in RIM-2
    { NW_XHTML_ElementToken_body,    NW_XHTML_AttributeToken_link,         NW_CSS_Prop_color,          NW_XHTML_APT_COLOR,    NULL,                         NULL },
    { NW_XHTML_ElementToken_body,    NW_XHTML_AttributeToken_vlink,        NW_CSS_Prop_color,          NW_XHTML_APT_COLOR,    NULL,                         NULL },
    { NW_XHTML_ElementToken_body,    NW_XHTML_AttributeToken_leftmargin,   NW_CSS_Prop_leftMargin,     NW_XHTML_APT_PIXEL,    NULL,                         NULL },
    { NW_XHTML_ElementToken_body,    NW_XHTML_AttributeToken_topmargin,    NW_CSS_Prop_topMargin,      NW_XHTML_APT_PIXEL,    NULL,                         NULL },
*/  { NW_XHTML_ElementToken_br,      NW_XHTML_AttributeToken_clear,        NW_CSS_Prop_clear,          NW_XHTML_APT_TOKEN,    NW_XHTML_APT_ClearValues,     NULL },
    { NW_XHTML_ElementToken_blink,   NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_blink,   NW_XHTML_AttributeToken_bgcolor,      NW_CSS_Prop_backgroundColor,NW_XHTML_APT_COLOR,    NULL,                         NULL },
    { NW_XHTML_ElementToken_blockquote,   NW_XHTML_AttributeToken_dir,     NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_caption, NW_XHTML_AttributeToken_align,        NW_CSS_Prop_textAlign,      NW_XHTML_APT_TOKEN,    NW_XHTML_APT_AlignValues,     NULL },
    { NW_XHTML_ElementToken_caption, NW_XHTML_AttributeToken_align,        NW_CSS_Prop_captionSide,    NW_XHTML_APT_TOKEN,    NW_XHTML_APT_CaptionValignValues, &NW_XHTML_PropertyTable_captionAlignCSSRequires },
    { NW_XHTML_ElementToken_caption, NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_center,  NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_cite,    NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_code,    NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_del,     NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_dfn,     NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_div,     NW_XHTML_AttributeToken_align,        NW_CSS_Prop_textAlign,      NW_XHTML_APT_TOKEN,    NW_XHTML_APT_AlignValues,     NULL },
    { NW_XHTML_ElementToken_div,     NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_dd,      NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_dl,      NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_dt,      NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_em,      NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_embed,   NW_XHTML_AttributeToken_align,        NW_CSS_Prop_float,          NW_XHTML_APT_TOKEN,    NW_XHTML_APT_FloatValues,     &NW_XHTML_PropertyTable_floatCSSRequires },
    { NW_XHTML_ElementToken_embed,   NW_XHTML_AttributeToken_align,        NW_CSS_Prop_verticalAlign,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_ValignValues,    NULL },
    { NW_XHTML_ElementToken_embed,   NW_XHTML_AttributeToken_border,       NW_CSS_Prop_borderWidth,    NW_XHTML_APT_PIXEL,    NULL,                         &NW_XHTML_PropertyTable_borderCSSRequires },
    { NW_XHTML_ElementToken_embed,   NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_embed,   NW_XHTML_AttributeToken_hspace,       NW_CSS_Prop_leftMargin,     NW_XHTML_APT_PIXEL,    NULL,                         NULL },
    { NW_XHTML_ElementToken_embed,   NW_XHTML_AttributeToken_hspace,       NW_CSS_Prop_rightMargin,    NW_XHTML_APT_PIXEL,    NULL,                         NULL },
    { NW_XHTML_ElementToken_embed,   NW_XHTML_AttributeToken_id,           NW_CSS_Prop_boxName,        NW_XHTML_APT_STRING,   NULL,                         NULL },
    { NW_XHTML_ElementToken_embed,   NW_XHTML_AttributeToken_name,         NW_CSS_Prop_boxName,        NW_XHTML_APT_STRING,   NULL,                         NULL },
    { NW_XHTML_ElementToken_embed,   NW_XHTML_AttributeToken_tabindex,     NW_CSS_Prop_tabIndex,       NW_XHTML_APT_INTEGER,  NULL,                         NULL },
    { NW_XHTML_ElementToken_embed,   NW_XHTML_AttributeToken_vspace,       NW_CSS_Prop_bottomMargin,   NW_XHTML_APT_PIXEL,    NULL,                         NULL },
    { NW_XHTML_ElementToken_embed,   NW_XHTML_AttributeToken_vspace,       NW_CSS_Prop_topMargin,      NW_XHTML_APT_PIXEL,    NULL,                         NULL },
    { NW_XHTML_ElementToken_h1,      NW_XHTML_AttributeToken_align,        NW_CSS_Prop_textAlign,      NW_XHTML_APT_TOKEN,    NW_XHTML_APT_AlignValues,     NULL },
    { NW_XHTML_ElementToken_h1,      NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_h2,      NW_XHTML_AttributeToken_align,        NW_CSS_Prop_textAlign,      NW_XHTML_APT_TOKEN,    NW_XHTML_APT_AlignValues,     NULL },
    { NW_XHTML_ElementToken_h2,      NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_h3,      NW_XHTML_AttributeToken_align,        NW_CSS_Prop_textAlign,      NW_XHTML_APT_TOKEN,    NW_XHTML_APT_AlignValues,     NULL },
    { NW_XHTML_ElementToken_h3,      NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_h4,      NW_XHTML_AttributeToken_align,        NW_CSS_Prop_textAlign,      NW_XHTML_APT_TOKEN,    NW_XHTML_APT_AlignValues,     NULL },
    { NW_XHTML_ElementToken_h4,      NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_h5,      NW_XHTML_AttributeToken_align,        NW_CSS_Prop_textAlign,      NW_XHTML_APT_TOKEN,    NW_XHTML_APT_AlignValues,     NULL },
    { NW_XHTML_ElementToken_h5,      NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_h6,      NW_XHTML_AttributeToken_align,        NW_CSS_Prop_textAlign,      NW_XHTML_APT_TOKEN,    NW_XHTML_APT_AlignValues,     NULL },
    { NW_XHTML_ElementToken_h6,      NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_head,    NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_hr,      NW_XHTML_AttributeToken_align,        NW_CSS_Prop_textAlign,      NW_XHTML_APT_TOKEN,    NW_XHTML_APT_AlignValues,     NULL },
    { NW_XHTML_ElementToken_hr,      NW_XHTML_AttributeToken_size,         NW_CSS_Prop_height,         NW_XHTML_APT_PIXEL,    NULL,                         NULL },
    { NW_XHTML_ElementToken_hr,      NW_XHTML_AttributeToken_width,        NW_CSS_Prop_width,          NW_XHTML_APT_LENGTH,   NULL,                         NULL },
    { NW_XHTML_ElementToken_hr,      NW_XHTML_AttributeToken_noshade,      NW_CSS_Prop_noshade,        NW_XHTML_APT_TOKEN,    NW_XHTML_APT_NoshadeValues,   NULL },
    { NW_XHTML_ElementToken_hr,      NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_hr,      NW_XHTML_AttributeToken_color,        NW_CSS_Prop_color,          NW_XHTML_APT_COLOR,    NULL,                         NULL },
    { NW_XHTML_ElementToken_html,    NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_fieldset,NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_font,    NW_XHTML_AttributeToken_color,        NW_CSS_Prop_color,          NW_XHTML_APT_COLOR,    NULL,                         NULL },
    { NW_XHTML_ElementToken_font,    NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_form,    NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_form,    NW_XHTML_AttributeToken_name,         NW_CSS_Prop_boxName,        NW_XHTML_APT_STRING,   NULL,                         NULL },
    { NW_XHTML_ElementToken_frameset,NW_XHTML_AttributeToken_bgcolor,      NW_CSS_Prop_backgroundColor,NW_XHTML_APT_COLOR,    NULL,                         NULL },
    { NW_XHTML_ElementToken_i,       NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_img,     NW_XHTML_AttributeToken_align,        NW_CSS_Prop_float,          NW_XHTML_APT_TOKEN,    NW_XHTML_APT_FloatValues,     &NW_XHTML_PropertyTable_floatCSSRequires },
    { NW_XHTML_ElementToken_img,     NW_XHTML_AttributeToken_align,        NW_CSS_Prop_verticalAlign,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_ValignValues,    NULL },
    { NW_XHTML_ElementToken_img,     NW_XHTML_AttributeToken_height,       NW_CSS_Prop_height,         NW_XHTML_APT_LENGTH,   NULL,                         NULL },
    { NW_XHTML_ElementToken_img,     NW_XHTML_AttributeToken_width,        NW_CSS_Prop_width,          NW_XHTML_APT_LENGTH,   NULL,                         NULL },
    { NW_XHTML_ElementToken_img,     NW_XHTML_AttributeToken_border,       NW_CSS_Prop_borderWidth,    NW_XHTML_APT_PIXEL,    NULL,                         &NW_XHTML_PropertyTable_borderCSSRequires },
    { NW_XHTML_ElementToken_img,     NW_XHTML_AttributeToken_hspace,       NW_CSS_Prop_leftMargin,     NW_XHTML_APT_PIXEL,    NULL,                         NULL },
    { NW_XHTML_ElementToken_img,     NW_XHTML_AttributeToken_hspace,       NW_CSS_Prop_rightMargin,    NW_XHTML_APT_PIXEL,    NULL,                         NULL },
    { NW_XHTML_ElementToken_img,     NW_XHTML_AttributeToken_vspace,       NW_CSS_Prop_bottomMargin,   NW_XHTML_APT_PIXEL,    NULL,                         NULL },
    { NW_XHTML_ElementToken_img,     NW_XHTML_AttributeToken_vspace,       NW_CSS_Prop_topMargin,      NW_XHTML_APT_PIXEL,    NULL,                         NULL },
    { NW_XHTML_ElementToken_img,     NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_input,   NW_XHTML_AttributeToken_tabindex,     NW_CSS_Prop_tabIndex,       NW_XHTML_APT_INTEGER,  NULL,                         NULL },
    { NW_XHTML_ElementToken_input,   NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_input,   NW_XHTML_AttributeToken_height,       NW_CSS_Prop_height,         NW_XHTML_APT_LENGTH,   NULL,                         NULL },
    { NW_XHTML_ElementToken_input,   NW_XHTML_AttributeToken_width,        NW_CSS_Prop_width,          NW_XHTML_APT_LENGTH,   NULL,                         NULL },
    { NW_XHTML_ElementToken_input,   NW_XHTML_AttributeToken_istyle,       NW_CSS_Prop_istyle,         NW_XHTML_APT_INTEGER,  NULL,                         NULL },
    { NW_XHTML_ElementToken_input,   NW_XHTML_AttributeToken_mode,         NW_CSS_Prop_mode,           NW_XHTML_APT_TOKEN,    NW_XHTML_APT_ModeValues,      NULL },
    { NW_XHTML_ElementToken_ins,     NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_kbd,     NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_map,     NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_map,     NW_XHTML_AttributeToken_name,         NW_CSS_Prop_boxName,        NW_XHTML_APT_STRING,   NULL,                         NULL },
    { NW_XHTML_ElementToken_map,     NW_XHTML_AttributeToken_id,           NW_CSS_Prop_boxName,        NW_XHTML_APT_STRING,   NULL,                         NULL },
    { NW_XHTML_ElementToken_marquee, NW_XHTML_AttributeToken_behavior,     NW_CSS_Prop_marqueeStyle,   NW_XHTML_APT_TOKEN,    NW_XHTML_APT_MarqueeBehValues,NULL },
    { NW_XHTML_ElementToken_marquee, NW_XHTML_AttributeToken_direction,    NW_CSS_Prop_marqueeDir,     NW_XHTML_APT_TOKEN,    NW_XHTML_APT_MarqueeDirValues,NULL },
    { NW_XHTML_ElementToken_marquee, NW_XHTML_AttributeToken_scrollamount, NW_CSS_Prop_marqueeAmount,  NW_XHTML_APT_PIXEL,    NULL,                         NULL },
    { NW_XHTML_ElementToken_marquee, NW_XHTML_AttributeToken_scrolldelay,  NW_CSS_Prop_marqueeSpeed,   NW_XHTML_APT_INTEGER,  NULL,                         NULL },
    { NW_XHTML_ElementToken_marquee, NW_XHTML_AttributeToken_loop,         NW_CSS_Prop_marqueeLoop,    NW_XHTML_APT_INTEGER,  NULL,                         NULL },
    { NW_XHTML_ElementToken_marquee, NW_XHTML_AttributeToken_loop,         NW_CSS_Prop_marqueeLoop,    NW_XHTML_APT_TOKEN,    NW_XHTML_APT_MarqueeLoopValues,NULL },
    { NW_XHTML_ElementToken_marquee, NW_XHTML_AttributeToken_bgcolor,      NW_CSS_Prop_backgroundColor,NW_XHTML_APT_COLOR,    NULL,                         NULL },
    { NW_XHTML_ElementToken_marquee, NW_XHTML_AttributeToken_width,        NW_CSS_Prop_width,          NW_XHTML_APT_LENGTH,   NULL,                         NULL },
    { NW_XHTML_ElementToken_marquee, NW_XHTML_AttributeToken_height,       NW_CSS_Prop_height,         NW_XHTML_APT_LENGTH,   NULL,                         NULL },
    { NW_XHTML_ElementToken_marquee, NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_marquee, NW_XHTML_AttributeToken_hspace,       NW_CSS_Prop_leftMargin,     NW_XHTML_APT_PIXEL,    NULL,                         NULL },
    { NW_XHTML_ElementToken_marquee, NW_XHTML_AttributeToken_hspace,       NW_CSS_Prop_rightMargin,    NW_XHTML_APT_PIXEL,    NULL,                         NULL },
    { NW_XHTML_ElementToken_marquee, NW_XHTML_AttributeToken_vspace,       NW_CSS_Prop_bottomMargin,   NW_XHTML_APT_PIXEL,    NULL,                         NULL },
    { NW_XHTML_ElementToken_marquee, NW_XHTML_AttributeToken_vspace,       NW_CSS_Prop_topMargin,      NW_XHTML_APT_PIXEL,    NULL,                         NULL },
    { NW_XHTML_ElementToken_menu,    NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_menu,    NW_XHTML_AttributeToken_type,         NW_CSS_Prop_listStyleType,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_ULTypeValues,    NULL },
    { NW_XHTML_ElementToken_label,   NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_li,      NW_XHTML_AttributeToken_type,         NW_CSS_Prop_listStyleType,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_OLTypeValues,    NULL },
    { NW_XHTML_ElementToken_li,      NW_XHTML_AttributeToken_type,         NW_CSS_Prop_listStyleType,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_ULTypeValues,    NULL },
    { NW_XHTML_ElementToken_li,      NW_XHTML_AttributeToken_value,        NW_CSS_Prop_listValue,      NW_XHTML_APT_INTEGER,  NULL,                         NULL },
    { NW_XHTML_ElementToken_li,      NW_XHTML_AttributeToken_clear,        NW_CSS_Prop_clear,          NW_XHTML_APT_TOKEN,    NW_XHTML_APT_ClearValues,     NULL },
    { NW_XHTML_ElementToken_li,      NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_object,  NW_XHTML_AttributeToken_align,        NW_CSS_Prop_float,          NW_XHTML_APT_TOKEN,    NW_XHTML_APT_FloatValues,     &NW_XHTML_PropertyTable_floatCSSRequires },
    { NW_XHTML_ElementToken_object,  NW_XHTML_AttributeToken_align,        NW_CSS_Prop_verticalAlign,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_ValignValues,    NULL },
    { NW_XHTML_ElementToken_object,  NW_XHTML_AttributeToken_border,       NW_CSS_Prop_borderWidth,    NW_XHTML_APT_PIXEL,    NULL,                         &NW_XHTML_PropertyTable_borderCSSRequires },
    { NW_XHTML_ElementToken_object,  NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_object,  NW_XHTML_AttributeToken_hspace,       NW_CSS_Prop_leftMargin,     NW_XHTML_APT_PIXEL,    NULL,                         NULL },
    { NW_XHTML_ElementToken_object,  NW_XHTML_AttributeToken_hspace,       NW_CSS_Prop_rightMargin,    NW_XHTML_APT_PIXEL,    NULL,                         NULL },
    { NW_XHTML_ElementToken_object,  NW_XHTML_AttributeToken_id,           NW_CSS_Prop_boxName,        NW_XHTML_APT_STRING,   NULL,                         NULL },
    { NW_XHTML_ElementToken_object,  NW_XHTML_AttributeToken_name,         NW_CSS_Prop_boxName,        NW_XHTML_APT_STRING,   NULL,                         NULL },
    { NW_XHTML_ElementToken_object,  NW_XHTML_AttributeToken_tabindex,     NW_CSS_Prop_tabIndex,       NW_XHTML_APT_INTEGER,  NULL,                         NULL },
    { NW_XHTML_ElementToken_object,  NW_XHTML_AttributeToken_vspace,       NW_CSS_Prop_bottomMargin,   NW_XHTML_APT_PIXEL,    NULL,                         NULL },
    { NW_XHTML_ElementToken_object,  NW_XHTML_AttributeToken_vspace,       NW_CSS_Prop_topMargin,      NW_XHTML_APT_PIXEL,    NULL,                         NULL },
    { NW_XHTML_ElementToken_ol,      NW_XHTML_AttributeToken_type,         NW_CSS_Prop_listStyleType,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_OLTypeValues,    NULL },
    { NW_XHTML_ElementToken_ol,      NW_XHTML_AttributeToken_start,        NW_CSS_Prop_listValue,      NW_XHTML_APT_INTEGER,  NULL,                         NULL },
    { NW_XHTML_ElementToken_ol,      NW_XHTML_AttributeToken_clear,        NW_CSS_Prop_clear,          NW_XHTML_APT_TOKEN,    NW_XHTML_APT_ClearValues,     NULL },
    { NW_XHTML_ElementToken_ol,      NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_p,       NW_XHTML_AttributeToken_align,        NW_CSS_Prop_textAlign,      NW_XHTML_APT_TOKEN,    NW_XHTML_APT_AlignValues,     NULL },
    { NW_XHTML_ElementToken_p,       NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_plaintext,NW_XHTML_AttributeToken_dir,         NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_pre,     NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_q,       NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_s,       NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_samp,    NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_select,  NW_XHTML_AttributeToken_tabindex,     NW_CSS_Prop_tabIndex,       NW_XHTML_APT_INTEGER,  NULL,                         NULL },
    { NW_XHTML_ElementToken_select,  NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_small,   NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_span,    NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_strike,  NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_strong,  NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_style,   NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_table,   NW_XHTML_AttributeToken_border,       NW_CSS_Prop_borderWidth,    NW_XHTML_APT_PIXEL,    NULL,                         &NW_XHTML_PropertyTable_borderCSSRequires },
    { NW_XHTML_ElementToken_table,   NW_XHTML_AttributeToken_width,        NW_CSS_Prop_width,          NW_XHTML_APT_LENGTH,   NULL,                         NULL },
    { NW_XHTML_ElementToken_table,   NW_XHTML_AttributeToken_height,       NW_CSS_Prop_height,         NW_XHTML_APT_LENGTH,   NULL,                         NULL },
    { NW_XHTML_ElementToken_table,   NW_XHTML_AttributeToken_align,        NW_CSS_Prop_tableAlign,     NW_XHTML_APT_TOKEN,    NW_XHTML_APT_AlignValues,     NULL },
    { NW_XHTML_ElementToken_table,   NW_XHTML_AttributeToken_cellpadding,  NW_CSS_Prop_cellPadding,    NW_XHTML_APT_LENGTH,   NULL,                         NULL },
    { NW_XHTML_ElementToken_table,   NW_XHTML_AttributeToken_bgcolor,      NW_CSS_Prop_backgroundColor,NW_XHTML_APT_COLOR,    NULL,                         NULL },
    { NW_XHTML_ElementToken_table,   NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_table,   NW_XHTML_AttributeToken_bordercolor,  NW_CSS_Prop_borderColor,    NW_XHTML_APT_TRANSCOLOR,    NULL,                         NULL },
    { NW_XHTML_ElementToken_td,      NW_XHTML_AttributeToken_align,        NW_CSS_Prop_textAlign,      NW_XHTML_APT_TOKEN,    NW_XHTML_APT_AlignValues,     NULL },
    { NW_XHTML_ElementToken_td,      NW_XHTML_AttributeToken_valign,       NW_CSS_Prop_verticalAlign,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_ValignValues,    NULL },
    { NW_XHTML_ElementToken_td,      NW_XHTML_AttributeToken_colspan,      NW_CSS_Prop_colSpan,        NW_XHTML_APT_INTEGER,  NULL,                         NULL },
    { NW_XHTML_ElementToken_td,      NW_XHTML_AttributeToken_rowspan,      NW_CSS_Prop_rowSpan,        NW_XHTML_APT_INTEGER,  NULL,                         NULL },
    { NW_XHTML_ElementToken_td,      NW_XHTML_AttributeToken_width,        NW_CSS_Prop_width,          NW_XHTML_APT_LENGTH,   NULL,                         NULL },
    { NW_XHTML_ElementToken_td,      NW_XHTML_AttributeToken_height,       NW_CSS_Prop_height,         NW_XHTML_APT_LENGTH,   NULL,                         NULL },
    { NW_XHTML_ElementToken_td,      NW_XHTML_AttributeToken_bgcolor,      NW_CSS_Prop_backgroundColor,NW_XHTML_APT_COLOR,    NULL,                         NULL },
    { NW_XHTML_ElementToken_td,      NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_td,      NW_XHTML_AttributeToken_nowrap,       NW_CSS_Prop_whitespace,     NW_XHTML_APT_TOKEN,    NW_XHTML_APT_NoWrapValues,    NULL },
    { NW_XHTML_ElementToken_td,      NW_XHTML_AttributeToken_bordercolor,  NW_CSS_Prop_borderColor,    NW_XHTML_APT_TRANSCOLOR,    NULL,                         NULL },
    { NW_XHTML_ElementToken_textarea,NW_XHTML_AttributeToken_tabindex,     NW_CSS_Prop_tabIndex,       NW_XHTML_APT_INTEGER,  NULL,                         NULL },
    { NW_XHTML_ElementToken_textarea, NW_XHTML_AttributeToken_dir,         NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_textarea,NW_XHTML_AttributeToken_istyle,       NW_CSS_Prop_istyle,         NW_XHTML_APT_INTEGER,  NULL,                         NULL },
    { NW_XHTML_ElementToken_textarea,NW_XHTML_AttributeToken_mode,         NW_CSS_Prop_mode,           NW_XHTML_APT_TOKEN,    NW_XHTML_APT_ModeValues,      NULL },
    { NW_XHTML_ElementToken_th,      NW_XHTML_AttributeToken_align,        NW_CSS_Prop_textAlign,      NW_XHTML_APT_TOKEN,    NW_XHTML_APT_AlignValues,     NULL },
    { NW_XHTML_ElementToken_th,      NW_XHTML_AttributeToken_valign,       NW_CSS_Prop_verticalAlign,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_ValignValues,    NULL },
    { NW_XHTML_ElementToken_th,      NW_XHTML_AttributeToken_colspan,      NW_CSS_Prop_colSpan,        NW_XHTML_APT_INTEGER,  NULL,                         NULL },
    { NW_XHTML_ElementToken_th,      NW_XHTML_AttributeToken_rowspan,      NW_CSS_Prop_rowSpan,        NW_XHTML_APT_INTEGER,  NULL,                         NULL },
    { NW_XHTML_ElementToken_th,      NW_XHTML_AttributeToken_width,        NW_CSS_Prop_width,          NW_XHTML_APT_LENGTH,   NULL,                         NULL },
    { NW_XHTML_ElementToken_th,      NW_XHTML_AttributeToken_height,       NW_CSS_Prop_height,         NW_XHTML_APT_LENGTH,   NULL,                         NULL },
    { NW_XHTML_ElementToken_th,      NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_th,      NW_XHTML_AttributeToken_bgcolor,      NW_CSS_Prop_backgroundColor,NW_XHTML_APT_COLOR,    NULL,                         NULL },
    { NW_XHTML_ElementToken_tr,      NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_tr,      NW_XHTML_AttributeToken_bgcolor,      NW_CSS_Prop_backgroundColor,NW_XHTML_APT_COLOR,    NULL,                         NULL },
    { NW_XHTML_ElementToken_tt,      NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_u,       NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_ul,      NW_XHTML_AttributeToken_type,         NW_CSS_Prop_listStyleType,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_ULTypeValues,    NULL },
    { NW_XHTML_ElementToken_ul,      NW_XHTML_AttributeToken_clear,        NW_CSS_Prop_clear,          NW_XHTML_APT_TOKEN,    NW_XHTML_APT_ClearValues,     NULL },
    { NW_XHTML_ElementToken_ul,      NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL },
    { NW_XHTML_ElementToken_var,     NW_XHTML_AttributeToken_dir,          NW_CSS_Prop_textDirection,  NW_XHTML_APT_TOKEN,    NW_XHTML_APT_TextDirValues,   NULL }
};


/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */

const
NW_XHTML_AttributePropertyTable_Class_t NW_XHTML_AttributePropertyTable_Class = {
  { /* NW_Object_Core                  */
    /* super                           */ &NW_Object_Core_Class,
    /* queryInterface                  */ _NW_Object_Core_QueryInterface
  },
  { /* NW_XHTML_AttributePropertyTable */
    /* numPropertyVals                 */ sizeof(_NW_XHTML_AttributePropertyTable_PropValEntries)
                                            / sizeof (_NW_XHTML_AttributePropertyTable_PropValEntries[0]),
    /* propertyVals                    */ _NW_XHTML_AttributePropertyTable_PropValEntries,
    /* numElementEntries               */ sizeof (_NW_XHTML_AttributePropertyTable_Elements)
                                            / sizeof (NW_XHTML_AttributePropertyTable_ElementEntry_t),
    /* ElementEntries                  */ _NW_XHTML_AttributePropertyTable_Elements
  }
};

/* ------------------------------------------------------------------------- */
const NW_XHTML_AttributePropertyTable_t NW_XHTML_AttributePropertyTable = {
  { &NW_XHTML_AttributePropertyTable_Class }
};

/* -------------------------------------------------------------------------
            private methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
static TBrowserStatusCode
NW_XHTML_AttributePropertyTable_GetCSSPropertyValToken(const NW_XHTML_AttributePropertyTable_t* thisObj,
                                        const NW_Ucs2* attrValStr,
                                        NW_Uint16 element,
                                        NW_Uint16 attribute,
                                        NW_LMgr_PropertyValueToken_t* token)
{
  NW_Uint32 numPropertyVals;
  const NW_XHTML_AttributePropertyTable_PropValEntry_t* propertyVals;
  NW_Uint32 index;

  numPropertyVals =
    NW_XHTML_AttributePropertyTable_GetClassPart(thisObj).numPropertyVals;
  propertyVals =
    NW_XHTML_AttributePropertyTable_GetClassPart(thisObj).propertyVals;

  /* Traverse our list of token entries looking for a match for input string */
  for (index = 0; index < numPropertyVals; index++) {
    if (propertyVals[index].case_sensitive) {
      if (NW_Str_Strcmp(attrValStr, propertyVals[index].attrValStr)) {
        continue;
      }
    }
    else {
      if (NW_Str_Stricmp(attrValStr, propertyVals[index].attrValStr)) {
        continue;
      }
    }

    if ((propertyVals[index].elementToken == element) || (propertyVals[index].elementToken == 0)) {
      if ((propertyVals[index].attributeToken == attribute) || (propertyVals[index].attributeToken == 0)) {
        *token = propertyVals[index].CSSPropertyVal;
        return KBrsrSuccess;
      }
    }
  }
  /* No matching token found for this attribute value. */
  return KBrsrFailure;
}

/* ------------------------------------------------------------------------- */
static TBrowserStatusCode
NW_XHTML_AttributePropertyTable_ValidateToken(
           const NW_XHTML_AttributePropertyTable_ElementEntry_t* elementEntry,
           NW_LMgr_PropertyValueToken_t token)
{
    const NW_LMgr_PropertyValueToken_t* CSSPropVals;
    NW_Uint32 index;

    CSSPropVals = elementEntry->CSSPropVals;

    /* traverse our list of valid tokens looking for a match for input string */
    for (index = 0; CSSPropVals[index] != NW_CSS_PropValue_none; index++) {
        if (token == CSSPropVals[index]) {
            return KBrsrSuccess;
        }
    }

    /* Token is not valid for this attribute */
    return KBrsrFailure;
}

/* ------------------------------------------------------------------------- */
static TBrowserStatusCode
NW_XHTML_AttributePropertyTable_SetTokenProperty(const NW_XHTML_AttributePropertyTable_t* thisObj,
                        const NW_XHTML_AttributePropertyTable_ElementEntry_t* elementEntry,
                        NW_Ucs2* attrValStr, NW_LMgr_Property_t* prop)
{
    prop->type = NW_CSS_ValueType_Token;
    if (NW_XHTML_AttributePropertyTable_GetCSSPropertyValToken(
        thisObj, attrValStr, elementEntry->elementToken, elementEntry->attributeToken, &prop->value.token) != KBrsrSuccess) {
        return KBrsrFailure;
    }

    return NW_XHTML_AttributePropertyTable_ValidateToken(elementEntry, prop->value.token);
}

/* -------------------------------------------------------------------------*/
static
TBrowserStatusCode
NW_XHTML_AttributePropertyTable_SetHexColor(NW_LMgr_Property_t *prop,
                           const NW_Ucs2* colorVal)
{
  NW_Uint8 index;
  NW_Uint8 v;
  NW_Uint32 color = 0;
  NW_Ucs2 ch;
  NW_Uint32 length;
  NW_Ucs2* ptr;

  /* Hex values are of the form #NNNNNN or #NNN */
  length = NW_Str_Strlen(colorVal);
  if ((length != 3) && (length != 6)){
    return KBrsrFailure;
  }
  for (ptr= (NW_Ucs2*) colorVal, index=0; index<length; index++, ptr++)
  {
    ch = *ptr;

    if (NW_Str_Isdigit(ch)){
      v = (NW_Uint8)(ch - '0');
    }
    else if ((ch >= 'A') && (ch <= 'F')){
      v = (NW_Uint8)(10 + (ch - 'A'));
    }else if ((ch >= 'a') && (ch <= 'f')){
      v = (NW_Uint8)(10 + (ch - 'a'));
    }else{
      return KBrsrFailure;
    }
    if (index == 0){
      color = color | v;
    }else{
      color = (color << 4) | v;
    }
    if (length == 3){
      color = (color << 4) | v;
    }
  }
  prop->value.integer = color;
  prop->type = NW_CSS_ValueType_Color;
  return KBrsrSuccess;
}
/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_XHTML_AttributePropertyTable_GetRGBVal(NW_Ucs2** ptr, NW_Uint8* val)
{
  NW_Ucs2 *endPtr = NULL;
  NW_Int32 number;

  NW_Ucs2* startPtr = *ptr;

  while (NW_Str_Isdigit(**ptr)) {
      (*ptr)++;
  }

  number = NW_Str_Strtol (startPtr, &endPtr, 10 );

  if (**ptr == ASCII_PERCENT){
    if (number > 100){
      number = 100;
    }
    number = (NW_Int32)(number*255/100);
    (*ptr)++;
  }
  else{
    if (number > 255){
      number = 255;
    }
  }
  *val = (NW_Uint8)number;
  return KBrsrSuccess;
}


/* ------------------------------------------------------------------------- */

void
NW_XHTML_AttributePropertyTable_SkipWhiteSpaces(NW_Ucs2** ptr)
{
  while (NW_Str_Isspace(**ptr)) {
      (*ptr)++;
  }
}
/* ------------------------------------------------------------------------- */


TBrowserStatusCode
NW_XHTML_AttributePropertyTable_ParseRGB(const NW_Ucs2* colorVal,
                                         NW_LMgr_Property_t* prop)

{
  NW_Uint8 rVal;
  NW_Uint8 gVal;
  NW_Uint8 bVal;
  NW_Uint32 color = 0;
  NW_Ucs2* ptr;

  /* Format is rgb(rval, gval, bval) */
  /* Allowed values are integers 0 - 255 or percentages 0 - 100% */

  ptr = (NW_Ucs2*) colorVal;
  NW_XHTML_AttributePropertyTable_SkipWhiteSpaces(&ptr);
  if (NW_XHTML_AttributePropertyTable_GetRGBVal(&ptr, &rVal) != KBrsrSuccess) {
    return KBrsrFailure;
  }
  NW_XHTML_AttributePropertyTable_SkipWhiteSpaces(&ptr);
  if (*ptr != ASCII_COMMA){
    return KBrsrFailure;
  }
  ptr++;

  NW_XHTML_AttributePropertyTable_SkipWhiteSpaces(&ptr);
  if (NW_XHTML_AttributePropertyTable_GetRGBVal(&ptr, &gVal) != KBrsrSuccess) {
    return KBrsrFailure;
  }
  NW_XHTML_AttributePropertyTable_SkipWhiteSpaces(&ptr);
  if (*ptr != ASCII_COMMA){
    return KBrsrFailure;
  }
  ptr++;

  NW_XHTML_AttributePropertyTable_SkipWhiteSpaces(&ptr);
  if (NW_XHTML_AttributePropertyTable_GetRGBVal(&ptr, &bVal) != KBrsrSuccess) {
    return KBrsrFailure;
  }
  NW_XHTML_AttributePropertyTable_SkipWhiteSpaces(&ptr);
  if (*ptr !=  ASCII_RIGHT_PARENTHESIS){
    return KBrsrFailure;
  }

  color = color| rVal;
  color = (color << 8)| gVal;
  color = (color << 8)| bVal;
  prop->value.integer = color;
  prop->type = NW_CSS_ValueType_Color;
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */


static TBrowserStatusCode
NW_XHTML_AttributePropertyTable_SetColorProperty(const NW_XHTML_AttributePropertyTable_t* thisObj,
                                                 const NW_XHTML_AttributePropertyTable_ElementEntry_t* elementEntry,
                                                 NW_Ucs2 *attrValStr,
                                                 NW_Bool transparentAllowed,
                                                 NW_LMgr_Property_t* prop)
{
    NW_Int32 color;
    NW_Uint32 token;
    NW_Ucs2 *colorVal;
    TBrowserStatusCode status;
    NW_CSS_TokenToInt_t* tokenToInt = (NW_CSS_TokenToInt_t *)(&NW_CSS_TokenToInt);


    /* Colors are in hex or rgb format, or are given as token names */
    if (*attrValStr == ASCII_HASH) {
        colorVal = attrValStr+1;
        return NW_XHTML_AttributePropertyTable_SetHexColor(prop, colorVal);
    }

    /* Sometime ASCII_HASH is missing from the color value but rest of value is correct.
     * The function NW_XHTML_AttributePropertyTable_SetHexColor(..) is used to check this,
     * if this function returns failure then it means that continue otherwise return.
     */

    if(NW_XHTML_AttributePropertyTable_SetHexColor(prop,attrValStr) == KBrsrSuccess)
    {
     return KBrsrSuccess;
    }



    if (!NW_Str_Strnicmp(attrValStr, rgb, NW_Str_Strlen(rgb))) {
        colorVal = attrValStr + NW_Str_Strlen(rgb);
        return NW_XHTML_AttributePropertyTable_ParseRGB(colorVal, prop);
    }

        status = NW_XHTML_AttributePropertyTable_GetCSSPropertyValToken(
            thisObj, attrValStr, elementEntry->elementToken, elementEntry->attributeToken, &token);
        if (status != KBrsrSuccess) {
            return status;
        }
        color = NW_CSS_TokenToInt_GetColorVal(tokenToInt, transparentAllowed, token);
        if (color == -1){
            return KBrsrFailure;
        }
        prop->value.integer = (NW_Uint32)color;
        prop->type = NW_CSS_ValueType_Color;
        return KBrsrSuccess;
    }

/* ------------------------------------------------------------------------- */
/* Numbers consist of 1 or more decimal digits. (0-9)+                       */

static TBrowserStatusCode
NW_XHTML_AttributePropertyTable_SetPixelProperty(NW_Ucs2 *attrValStr,
                                                  NW_LMgr_Property_t* prop)
{
    NW_Ucs2* endPtr = NULL;

    prop->type = NW_CSS_ValueType_Px;
    prop->value.integer = (NW_Int32) NW_Str_Strtoul(attrValStr, &endPtr, 10);
    return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */

static TBrowserStatusCode
NW_XHTML_AttributePropertyTable_SetIntegerProperty(NW_Ucs2 *attrValStr,
                                                   NW_LMgr_Property_t* prop)
{
	NW_Int32 size = 0;
  NW_Ucs2* testStr= attrValStr;
  NW_Ucs2 c;

  while ((c = *testStr++) != 0) {
      if (NW_Str_Isalpha(c)) {
          return KBrsrFailure;
      }
  }

  size = NW_Str_Atoi(attrValStr);
  if (size < 0) {
      return KBrsrFailure;
  }

  prop->type = NW_CSS_ValueType_Integer;
  prop->value.integer = size;
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */

static TBrowserStatusCode
NW_XHTML_AttributePropertyTable_SetLengthProperty(NW_Ucs2 *attrValStr,
                                                  NW_LMgr_Property_t* prop)
{
  TBrowserStatusCode status = KBrsrSuccess;
  const NW_Ucs2 *pStart = NULL;
  const NW_Ucs2 *pEnd   = NULL;
  NW_Ucs2* numberString = NULL;
  NW_Uint32 sublen = 0;
  NW_Ucs2* endPtr = NULL;
  NW_Float32 returnSize;
  NW_Ucs2 token = '%';

  pStart = attrValStr;

  pEnd = NW_Str_Strchr(pStart, token);

  if (pEnd != NULL)
  {
    sublen = (NW_Uint32)(pEnd - pStart);

    numberString = NW_Str_Substr(pStart, 0, sublen);

    status = NW_Str_CvtToFloat32(numberString, &endPtr, &returnSize);
    
    free(numberString);

    if (status == KBrsrSuccess)
    {
      if (returnSize < 0.0)
      {
        returnSize = 0.0;
      }

      prop->type = NW_CSS_ValueType_Percentage;
      prop->value.decimal = returnSize;
    }
    else
    {
      return status;
    }
  }
  else
  {
    prop->type = NW_CSS_ValueType_Px;
    prop->value.integer = NW_Str_Strtoul(pStart, &endPtr, 10);
  }

  return status;
}

/* ------------------------------------------------------------------------- */
static
NW_Text_t*
NW_XHTML_AttributePropertyTable_GetAElemAccessKeyLabel(
                                NW_XHTML_ContentHandler_t* contentHandler,
                                NW_DOM_ElementNode_t* elementNode)
{
  NW_DOM_ElementNode_t* childNode;
  NW_Text_t* aElemAccessKeyLabel = NULL;

  childNode = (NW_DOM_ElementNode_t*) NW_DOM_Node_getFirstChild (elementNode);
  if (childNode == NULL) {
    return NULL;
  }
   if (NW_DOM_Node_getNodeType(childNode) == NW_DOM_TEXT_NODE){
    return NW_XHTML_GetDOMTextNodeData (contentHandler, childNode);
  }
  if (NW_HED_DomHelper_GetElementToken(childNode) == NW_XHTML_ElementToken_img){
    NW_XHTML_GetDOMAttribute (contentHandler, childNode, NW_XHTML_AttributeToken_alt, &aElemAccessKeyLabel);
    return aElemAccessKeyLabel;
  }
  return NULL;
}

/* ------------------------------------------------------------------------- */
static
NW_Text_t*
NW_XHTML_AttributePropertyTable_GetInputElemAccessKeyLabel(
                                NW_XHTML_ContentHandler_t* contentHandler,
                                NW_DOM_ElementNode_t* elementNode)
{
  NW_Ucs2* typeAttr = NULL;
  TBrowserStatusCode status;
  NW_Text_t* label = NULL;
  NW_Bool match;

  status =
    NW_XHTML_ElementHandler_GetAttribute (contentHandler, elementNode,
                                          NW_XHTML_AttributeToken_type,
                                          &typeAttr);
  if (status != KBrsrSuccess) {
    return NULL;
  }

  match = (NW_Bool) !NW_Str_StricmpConst (typeAttr, "submit");
  NW_Mem_Free (typeAttr);

  if (!match) {
    return NULL;
  }

  NW_XHTML_GetDOMAttribute (contentHandler,  elementNode,
                                   NW_XHTML_AttributeToken_title, &label);
  if (label != NULL) {
      return label;
  }

  NW_XHTML_GetDOMAttribute (contentHandler,  elementNode,
                                   NW_XHTML_AttributeToken_value, &label);
  if (label != NULL) {
      return label;
  }
  return NW_Text_New (HTTP_iso_10646_ucs_2, (NW_Ucs2*)Submit,
                     NW_Str_Strlen(Submit), NW_FALSE);
}
/* ------------------------------------------------------------------------- */
static TBrowserStatusCode
NW_XHTML_AttributePropertyTable_ApplyAccessKey(NW_XHTML_ContentHandler_t* contentHandler,
                            NW_DOM_ElementNode_t* elementNode,
                            NW_LMgr_Box_t* box)
{
  NW_Text_t* accessKey = NULL;
  NW_LMgr_Property_t  accesskeyProp;
  NW_Uint16 elementToken;
  NW_Text_t* accesskeyLabel;

  NW_TRY(status)
  {
    /* is accesskey attribute already set - if yes don't check for accesskey */
    status = NW_LMgr_Box_GetPropertyFromList(box, NW_CSS_Prop_accesskey, &accesskeyProp);
    if (status != KBrsrNotFound)
      NW_THROW (status);

    /* is directkey attribute already set - if yes don't check for accesskey */
    status = NW_LMgr_Box_GetPropertyFromList(box, NW_CSS_Prop_directkey, &accesskeyProp);
    if (status != KBrsrNotFound)
      NW_THROW (status);

    elementToken = NW_HED_DomHelper_GetElementToken(elementNode);
    /* find the 'accesskey' attribute and get its string value */
    NW_XHTML_GetDOMAttribute(contentHandler, elementNode,
                                       NW_XHTML_AttributeToken_accesskey, &accessKey);
    accesskeyLabel = NULL;

    if (accessKey != NULL)
    {
      NW_LMgr_AccessKey_t* ak;

      ak = NW_LMgr_AccessKey_New();
      /* ak == NULL? out of memory error! */
      NW_THROW_OOM_ON_NULL (ak, status);

      if (NW_LMgr_AccessKey_SetFromTextAttr(ak, accessKey) == KBrsrSuccess)
      {
        /* set accesskey on the new box */
        if (elementToken == NW_XHTML_ElementToken_a){
          accesskeyLabel = NW_XHTML_AttributePropertyTable_GetAElemAccessKeyLabel(
            contentHandler, elementNode);
        }
        else if (elementToken == NW_XHTML_ElementToken_input){
          accesskeyLabel = NW_XHTML_AttributePropertyTable_GetInputElemAccessKeyLabel(
            contentHandler, elementNode);
        }
        /* set accesskey Label on the new box */
        NW_LMgr_AccessKey_SetLabel(ak, accesskeyLabel);

        accesskeyProp.type = NW_CSS_ValueType_Object;
        accesskeyProp.value.object = ak;
        /* store the status of the set: may cause out of memory error */
        status = NW_LMgr_Box_SetProperty(box, NW_CSS_Prop_accesskey, &accesskeyProp);
      }
      else
      {
        NW_Object_Delete(ak);
      }
    }
  }

  NW_CATCH (status)
  {
  }

  NW_FINALLY
  {
    NW_Object_Delete(accessKey);
    return status;
  }
  NW_END_TRY
}


/* ------------------------------------------------------------------------- */
static TBrowserStatusCode
NW_XHTML_AttributePropertyTable_ApplyDirectKey(NW_XHTML_ContentHandler_t* contentHandler,
                            NW_DOM_ElementNode_t* elementNode,
                            NW_LMgr_Box_t* box)
{
  NW_Text_t* accessKey = NULL;
  NW_LMgr_Property_t  accesskeyProp;
  NW_LMgr_Property_t displayProp;
  NW_Uint16 elementToken;
  NW_Text_t* accesskeyLabel;

  NW_TRY(status)
  {
    /* is accesskey attribute already set - if yes don't check for accesskey */
    status = NW_LMgr_Box_GetPropertyFromList(box, NW_CSS_Prop_accesskey, &accesskeyProp);
    if (status != KBrsrNotFound)
      NW_THROW (status);

    /* is directkey attribute already set - if yes don't check for accesskey */
    status = NW_LMgr_Box_GetPropertyFromList(box, NW_CSS_Prop_directkey, &accesskeyProp);
    if (status != KBrsrNotFound)
      NW_THROW (status);

    elementToken = NW_HED_DomHelper_GetElementToken(elementNode);
    /* find the 'directkey' attribute and get its string value */
    NW_XHTML_GetDOMAttribute(contentHandler, elementNode,
                                       NW_XHTML_AttributeToken_directkey, &accessKey);

    accesskeyLabel = NULL;

    if (accessKey != NULL)
    {
      NW_LMgr_AccessKey_t* ak;

      ak = NW_LMgr_AccessKey_New();
      /* ak == NULL? out of memory error! */
      NW_THROW_OOM_ON_NULL (ak, status);

      if (NW_LMgr_AccessKey_SetFromTextAttr(ak, accessKey) == KBrsrSuccess)
      {
        /* set accesskey on the new box */
        if (elementToken == NW_XHTML_ElementToken_a){
          accesskeyLabel = NW_XHTML_AttributePropertyTable_GetAElemAccessKeyLabel(
            contentHandler, elementNode);
          displayProp.type = NW_CSS_ValueType_Token;
          displayProp.value.token = NW_CSS_PropValue_display_directKey;
          status = NW_LMgr_Box_SetProperty(box, NW_CSS_Prop_display, &displayProp);
        }
        else if (elementToken == NW_XHTML_ElementToken_input){
          accesskeyLabel = NW_XHTML_AttributePropertyTable_GetInputElemAccessKeyLabel(
            contentHandler, elementNode);
        }
        /* set accesskey Label on the new box */
        NW_LMgr_AccessKey_SetLabel(ak, accesskeyLabel);

        accesskeyProp.type = NW_CSS_ValueType_Object;
        accesskeyProp.value.object = ak;
        /* store the status of the set: may cause out of memory error */
        status = NW_LMgr_Box_SetProperty(box, NW_CSS_Prop_accesskey, &accesskeyProp);

      }
      else
      {
        NW_Object_Delete(ak);
      }
    }
  }

  NW_CATCH (status)
  {
  }

  NW_FINALLY
  {
    NW_Object_Delete(accessKey);
    return status;
  }
  NW_END_TRY
}

/* ------------------------------------------------------------------------- */
static TBrowserStatusCode
NW_XHTML_AttributePropertyTable_ApplyTableBorder(NW_XHTML_ContentHandler_t* contentHandler,
                                                 NW_DOM_ElementNode_t* elementNode,
                                                 NW_LMgr_Box_t* box)
  {
  NW_LMgr_Property_t     borderWidth;
  NW_LMgr_Property_t     borderStyle;
  NW_LMgr_Property_t     borderColor;
  NW_Ucs2*               endPtr = NULL;
  NW_Int32               tableBorderWidth;
  NW_Int32               tableBorderColor;
  NW_LMgr_BoxVisitor_t*  visitor = NULL;
  NW_LMgr_Box_t*         child = NULL;
  NW_Ucs2*               tableBorderVal = NULL;
  NW_Ucs2*               tableBorderColorVal = NULL;

  NW_TRY(status)
    {
    // Get border attribute.
    status = NW_XHTML_ElementHandler_GetAttribute (contentHandler, elementNode, 
        NW_XHTML_AttributeToken_border, &tableBorderVal);
    if ((status != KBrsrSuccess) || (tableBorderVal == NULL))
      {
      NW_THROW_SUCCESS(status);
      }

    // Convert it to a int.
    status = NW_Str_CvtToInt32(tableBorderVal, &endPtr, 10, &tableBorderWidth);
    _NW_THROW_ON_ERROR(status);

    if (tableBorderWidth < 1)
      {
      NW_THROW_SUCCESS(status);
      }

    // Rather than convert the color attribute to an RGB value, just get the 
    // the color from the table box.
    tableBorderColor = 0x000000;
    borderColor.value.integer = tableBorderColor;
    borderColor.type = NW_CSS_ValueType_Color;

    status = NW_LMgr_PropertyList_Get(box->propList, NW_CSS_Prop_borderColor, &borderColor);
    if (status == KBrsrSuccess)
      {
      tableBorderColor = borderColor.value.integer;
      }

    // If need be set the default border on the cells.
    visitor = NW_LMgr_ContainerBox_GetBoxVisitor(box);
    NW_THROW_OOM_ON_NULL (visitor, status);

    while ((child = NW_LMgr_BoxVisitor_NextBox(visitor, NULL)) != NULL)
      {
      // Ignore non table cell boxes.
      if (!NW_Object_IsInstanceOf(child, &NW_LMgr_StaticTableCellBox_Class) &&
          !NW_Object_IsInstanceOf(child, &NW_LMgr_VerticalTableCellBox_Class))
        {
        continue;
        }

      // If need be apply the default border style.
      borderStyle.value.token = NW_CSS_PropValue_none;
      borderStyle.type = NW_CSS_ValueType_Token;

      status = NW_LMgr_PropertyList_Get(child->propList, NW_CSS_Prop_borderStyle, &borderStyle);
      if ((status == KBrsrNotFound) || (borderStyle.type & NW_CSS_ValueType_DefaultStyle))
        {
        borderStyle.value.token = NW_CSS_PropValue_solid;
        NW_LMgr_Box_SetProperty(child, NW_CSS_Prop_borderStyle, &borderStyle);
        }

      // If need be apply the default border width.
      borderWidth.value.integer = 0;
      borderWidth.type = NW_CSS_ValueType_Integer;

      status = NW_LMgr_PropertyList_Get(child->propList, NW_CSS_Prop_borderWidth, &borderWidth);
      if ((status == KBrsrNotFound) || (borderWidth.type & NW_CSS_ValueType_DefaultStyle))
        {
        borderWidth.value.integer = tableBorderWidth;
        borderWidth.type = NW_CSS_ValueType_Px;
        NW_LMgr_Box_SetProperty(child, NW_CSS_Prop_borderWidth, &borderWidth);
        }

      // If need be apply the default border color.
      borderColor.value.integer = 0x000000;
      borderColor.type = NW_CSS_ValueType_Color;

      status = NW_LMgr_PropertyList_Get(child->propList, NW_CSS_Prop_borderColor, &borderColor);
      if ((status == KBrsrNotFound) || (borderColor.type & NW_CSS_ValueType_DefaultStyle))
        {
        borderColor.value.integer = tableBorderColor;
        NW_LMgr_Box_SetProperty(child, NW_CSS_Prop_borderColor, &borderColor);
        }
      }
    }

  NW_CATCH (status)
    {
    }

  NW_FINALLY
    {
    NW_Object_Delete(visitor);
    NW_Mem_Free(tableBorderVal);
    NW_Mem_Free(tableBorderColorVal);

    return status;
    } NW_END_TRY
  }

/* ------------------------------------------------------------------------- */

static TBrowserStatusCode
NW_XHTML_AttributePropertyTable_SetStringProperty(NW_Ucs2 *attrValStr,
                                                  NW_LMgr_Property_t* prop)
{
    NW_Text_t* propValue;

    propValue = NW_Text_New (HTTP_iso_10646_ucs_2, attrValStr,
        NW_Str_Strlen (attrValStr), NW_Text_Flags_Copy);

    if (propValue == NULL) {
        return KBrsrOutOfMemory;
    }

    prop->type = NW_CSS_ValueType_Text;
    prop->value.object = propValue;

    return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */

static TBrowserStatusCode
NW_XHTML_AttributePropertyTable_SetUrlProperty(NW_XHTML_ContentHandler_t* contentHandler,
                                               NW_Ucs2 *attrValStr,
                                               NW_LMgr_Property_t* prop,
                                               NW_LMgr_PropertyName_t propName,
                                               NW_LMgr_Box_t *box )
{
  NW_Text_t* propValue = NULL;
  NW_CSS_Processor_t* processor;
  NW_Image_AbstractImage_t* image;

  NW_ASSERT( contentHandler );
  NW_TRY (status)
  {
    propValue = NW_Text_New (HTTP_iso_10646_ucs_2, attrValStr,
      NW_Str_Strlen (attrValStr), NW_Text_Flags_Copy);
    NW_THROW_OOM_ON_NULL (propValue, status);
    processor = NW_XHTML_ContentHandler_CreateCSSProcessor(contentHandler);
    NW_THROW_OOM_ON_NULL (processor, status);
    status = NW_CSS_Processor_GetImage(processor, propValue, box, propName, &image , 
        /* No buffer index for inline style */ -1);
    NW_THROW_ON(status, KBrsrOutOfMemory);
    prop->value.object = (NW_Object_t*)image;
    prop->type = NW_CSS_ValueType_Image;

    /* Remove ownership of the entry. It has been handed off to the simple
     property list which will handle deletion.*/
    if (image)
        {
        CCSSImageList* imageList = (CCSSImageList*)(processor->imageList);
        if(imageList)
            {
            NW_DOM_ElementNode_t* imageNode = NULL;
            CCSSImageListEntry* imageEntry = imageList->EntryByVirtualImage( image, &imageNode );
            if (imageEntry)
                {
                imageList->RemoveVirtualImageOwnership( *imageEntry, image );
                }
            }
        }

  }
  NW_CATCH( status)
  {
    if (propValue)
    {
      NW_Object_Delete(propValue);
    }
    status = KBrsrOutOfMemory;
  }
  NW_FINALLY
  {
    return status;
  }
  NW_END_TRY
}

/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_XHTML_AttributePropertyTable_CheckForFloat(NW_LMgr_Box_t* box, NW_Bool isTable)
{
  NW_LMgr_Property_t  displayProp;
  NW_LMgr_Property_t  floatProp;
  NW_LMgr_Property_t  vAlignProp;

  NW_TRY (status)
    {
    // Get the float property.
    floatProp.value.token = NW_CSS_PropValue_none;
    floatProp.type = NW_CSS_ValueType_Token;
    status = NW_LMgr_Box_GetProperty(box, NW_CSS_Prop_float, &floatProp);
    NW_THROW_ON(status, KBrsrOutOfMemory);

    // Get the vertical align property.
    vAlignProp.value.token = NW_CSS_PropValue_none;
    vAlignProp.type = NW_CSS_ValueType_Token;
    status = NW_LMgr_Box_GetProperty(box, NW_CSS_Prop_verticalAlign, &vAlignProp);
    NW_THROW_ON(status, KBrsrOutOfMemory);

    // Return successfully if there are no align properties.
    if ((floatProp.value.token == NW_CSS_PropValue_none) &&
        (vAlignProp.value.token == NW_CSS_PropValue_none))
      {
      NW_THROW_SUCCESS(status);
      }
  
    // The float property is not allowed on boxes that are inside a table,
    // so remove it.
    NW_LMgr_Box_t* parent;

    parent = box;
    while ((parent = (NW_LMgr_Box_t*) NW_LMgr_Box_GetParent(parent)) != NULL)
      {
      if (NW_Object_IsClass(parent, &NW_LMgr_VerticalTableBox_Class) || 
          NW_Object_IsClass(parent, &NW_LMgr_StaticTableBox_Class))
        {
        NW_LMgr_Box_RemoveProperty(box, NW_CSS_Prop_float);            
        NW_THROW_SUCCESS(status);
        }
      }

    // If this is not a table box change the box's display
    // property to block.
    if (!isTable)
      {
      displayProp.type = NW_CSS_ValueType_Token;
      displayProp.value.token = NW_CSS_PropValue_display_block;
      status = NW_LMgr_Box_SetProperty(box, NW_CSS_Prop_display, &displayProp);
      _NW_THROW_ON_ERROR(status);
      }

    // Otherwise move the float and width properties to the parent.
    else
      {
      NW_LMgr_Box_t*      tableContainer = (NW_LMgr_Box_t*) box->parent;
      NW_LMgr_Property_t  prop;

      NW_LMgr_Box_RemoveProperty(box, NW_CSS_Prop_float);
      status = NW_LMgr_Box_SetProperty(tableContainer, NW_CSS_Prop_float, &floatProp);
      _NW_THROW_ON_ERROR(status);

      /* If the table is floating, the width property should be applied
         to the container, too */
      prop.value.token = NW_CSS_PropValue_auto;
      prop.type = NW_CSS_ValueType_Token;
      status = NW_LMgr_Box_GetRawProperty(box, NW_CSS_Prop_width, &prop);
      
      if (status == KBrsrSuccess) 
        {
        NW_LMgr_Box_RemoveProperty(box, NW_CSS_Prop_width);
        status = NW_LMgr_Box_SetProperty(tableContainer, NW_CSS_Prop_width, &prop);
        _NW_THROW_ON_ERROR(status);
        }
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

/* ------------------------------------------------------------------------- */
/* returns KBrsrSuccess or KBrsrOutOfMemory */
TBrowserStatusCode
NW_XHTML_AttributePropertyTable_AddActiveBoxDecoration(NW_LMgr_Box_t* box, NW_Bool growBorder)
{
  TBrowserStatusCode status;
  NW_LMgr_Property_t prop;

  NW_ASSERT( box != NULL );

  if (growBorder)
  {
    /* Add the growBorder for active box containing images or object */
    prop.type = NW_CSS_ValueType_Token;
    prop.value.token = NW_CSS_PropValue_growBorder;
    status = NW_LMgr_Box_SetProperty(box,NW_CSS_Prop_focusBehavior, &prop);
  }
  else
  {
    NW_LMgr_PropertyValue_t propVal;

    /* Add underlining of text. */
    propVal.token = NW_CSS_PropValue_none;
    status = NW_LMgr_Box_GetPropertyValue(NW_LMgr_BoxOf(box), NW_CSS_Prop_textDecoration, NW_CSS_ValueType_Token, &propVal);
    if (status != KBrsrSuccess)
    {
      prop.type = NW_CSS_ValueType_Token;
      prop.value.token = NW_CSS_PropValue_underline;
      status = NW_LMgr_Box_SetProperty(box, NW_CSS_Prop_textDecoration, &prop);
    }

    status = NW_LMgr_Box_GetPropertyFromList(NW_LMgr_BoxOf(box), NW_CSS_Prop_color, &prop);
    if (status != KBrsrSuccess)
    {
      prop.type = NW_CSS_ValueType_Color;
      prop.value.integer = 0x0000FF; // setting color blue
      status = NW_LMgr_Box_SetProperty(box, NW_CSS_Prop_color, &prop);
    }
  }
  return status;
}

/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_XHTML_AttributePropertyTable_SetHtWidth(NW_LMgr_Box_t* box,
                                           NW_XHTML_ContentHandler_t* xhtmlContentHandler)
{
  TBrowserStatusCode status;
  NW_LMgr_Property_t heightVal;
  NW_LMgr_Property_t widthVal;
  CGDIDeviceContext* deviceContext;
  const NW_GDI_Rectangle_t* rectangle;
  NW_HED_DocumentRoot_t *documentRoot;
  NW_LMgr_RootBox_t *rootBox;

  documentRoot =
    (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode (xhtmlContentHandler);
  NW_ASSERT(documentRoot != NULL);

  rootBox = NW_HED_DocumentRoot_GetRootBox(documentRoot);

  /* For convenience */
  if (box->propList == NULL){
    return KBrsrSuccess;
  }

  heightVal.type = widthVal.type = 0;
  heightVal.value.decimal = widthVal.value.decimal = 0;

  status = NW_LMgr_PropertyList_Get(box->propList, NW_CSS_Prop_height, &heightVal);
  if (status == KBrsrSuccess)
  {
    heightVal.type = (NW_LMgr_PropertyValueType_t)(heightVal.type & NW_CSS_ValueType_Mask);
    if (heightVal.type == NW_CSS_ValueType_Percentage)
    {
      deviceContext = NW_LMgr_RootBox_GetDeviceContext (rootBox);
      rectangle = deviceContext->DisplayBounds();
      heightVal.value.integer = (NW_GDI_Metric_t)((heightVal.value.decimal*rectangle->dimension.height)/100);
      heightVal.type = NW_CSS_ValueType_Px;
      status = NW_LMgr_Box_SetProperty(box, NW_CSS_Prop_height, &heightVal);
      if (status == KBrsrOutOfMemory){
        return status;
      }
    }
  }
  status = NW_LMgr_PropertyList_Get(box->propList, NW_CSS_Prop_width, &widthVal);
  if (status == KBrsrSuccess)
  {
    widthVal.type = (NW_LMgr_PropertyValueType_t)(widthVal.type & NW_CSS_ValueType_Mask);
    if (widthVal.type == NW_CSS_ValueType_Percentage)
    {
      deviceContext = NW_LMgr_RootBox_GetDeviceContext (rootBox);
      rectangle = deviceContext->DisplayBounds();
      widthVal.value.integer = (NW_GDI_Metric_t)((widthVal.value.decimal*rectangle->dimension.width)/100);
      widthVal.type = NW_CSS_ValueType_Px;
      status = NW_LMgr_Box_SetProperty(box, NW_CSS_Prop_width, &widthVal);
      if (status == KBrsrOutOfMemory){
        return status;
      }
    }
  }
  return status;
}


/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_XHTML_AttributePropertyTable_NeedFlowBox(NW_DOM_ElementNode_t* elementNode, NW_LMgr_Box_t* box)
{
  NW_DOM_Node_t* childNode = elementNode;
  NW_Bool needFlowBox = NW_FALSE;

  for (childNode = NW_DOM_Node_getFirstChild (childNode);
       childNode != NULL;
       childNode = NW_DOM_Node_getNextSibling (childNode))
  {
    NW_Bool isBlockLevelTag = NW_FALSE;

    NW_Uint16 tagToken = NW_HED_DomHelper_GetElementToken (childNode);

    isBlockLevelTag = NW_XHTML_ElementHandler_IsBlockLevelTag(tagToken);

    if (isBlockLevelTag)
    {
      needFlowBox = NW_TRUE;
      break;
    }
    else
    {
      continue;
    }
  }
  if (needFlowBox)
  {
    NW_LMgr_Property_t displayProp;

    displayProp.type = NW_CSS_ValueType_Token;
    displayProp.value.token = NW_CSS_PropValue_display_block;
    return NW_LMgr_Box_SetProperty(box, NW_CSS_Prop_display, &displayProp);
  }
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_XHTML_AttributePropertyTable_HandleCaption(NW_LMgr_Box_t* box)
{
  NW_LMgr_Property_t prop;
  TBrowserStatusCode status = KBrsrSuccess;
  NW_LMgr_ContainerBox_t* parentBox = box->parent;

  prop.type = NW_CSS_ValueType_Token;
  prop.value.token = NW_CSS_PropValue_top;
  NW_LMgr_Box_GetProperty(box, NW_CSS_Prop_captionSide, &prop);

  if (prop.value.token == NW_CSS_PropValue_bottom)
  {
    status = NW_LMgr_ContainerBox_RemoveChild(parentBox, box);
    if (status == KBrsrSuccess)
    {
      status = NW_LMgr_ContainerBox_AddChild(parentBox, box);
    }
  }
  return status;
}



/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_XHTML_AttributePropertyTable_ApplyStyleTable(NW_XHTML_ContentHandler_t* aContentHandler,
                                                NW_DOM_ElementNode_t* aElementNode,
                                                NW_LMgr_Box_t* aBox)
  {
  NW_Bool gridMode;
  NW_LMgr_Property_t prop;

  NW_ASSERT(aContentHandler != NULL);
  NW_ASSERT(aElementNode != NULL);
  NW_ASSERT(aBox != NULL);

  NW_TRY (status)
    {
    // Determine if this is a grid mode table.
    gridMode = NW_XHTML_tableElementHandler_DoesGridModeApply(aContentHandler,
        aElementNode);

    // Adjust various properties to honor grid mode.
    if (gridMode)
      {
      // If this the first time this method is called on this box then
      // apply the NW_CSS_Prop_gridModeApplied property and store the 
      // table's original width in NW_CSS_Prop_gridModeOrigWidth.
      if (NW_LMgr_Box_GetProperty(aBox, NW_CSS_Prop_gridModeApplied, &prop) != KBrsrSuccess)
        {
        prop.type = NW_CSS_ValueType_Integer;
        prop.value.integer = 1;
        status = NW_LMgr_Box_SetProperty(aBox, NW_CSS_Prop_gridModeApplied, &prop);
        NW_THROW_ON(status, KBrsrOutOfMemory);

        status = NW_LMgr_Box_GetProperty(aBox, NW_CSS_Prop_width, &prop);
        NW_THROW_ON(status, KBrsrOutOfMemory);

        if (status == KBrsrSuccess)
          {
          status = NW_LMgr_Box_SetProperty(aBox, NW_CSS_Prop_gridModeOrigWidth, &prop);
          NW_THROW_ON(status, KBrsrOutOfMemory);

          status = NW_LMgr_Box_RemoveProperty(aBox, NW_CSS_Prop_width);
          NW_THROW_ON(status, KBrsrOutOfMemory);
          }
        }

      status = NW_XHTML_AttributePropertyTable_ApplyGridModeTableStyle(aBox);
      _NW_THROW_ON_ERROR(status);
      }

    // Apply other styles relevant for both grid mode and non-grid mode tables
    status = NW_XHTML_AttributePropertyTable_CheckForFloat(aBox, NW_TRUE);
    NW_THROW_ON(status, KBrsrOutOfMemory);

    status = NW_XHTML_AttributePropertyTable_ApplyTableBorder(aContentHandler,
        aElementNode, aBox);
    NW_THROW_ON(status, KBrsrOutOfMemory);
    }

  NW_CATCH (status)
    {
    }

  NW_FINALLY
    {
    return status;
    } NW_END_TRY
  }

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_XHTML_AttributePropertyTable_ApplyGridModeTableStyle(NW_LMgr_Box_t* aBox)
  {
  NW_LMgr_Property_t prop;

  NW_TRY (status)
    {
    // Don't do anything if this isn't a table.
    if (NW_Object_IsInstanceOf(aBox, &NW_LMgr_StaticTableBox_Class) == NW_FALSE)
      {
      NW_THROW_SUCCESS(status);
      }

    // Don't do anything if the table is an non-grid-mode table.
    status = NW_LMgr_Box_GetProperty(aBox, NW_CSS_Prop_gridModeApplied, &prop);
    if (status == KBrsrNotFound)
      {
      NW_THROW_SUCCESS(status);
      }
    _NW_THROW_ON_ERROR(status);

    // If in SSM set the table's width to 100%
    if (NW_Settings_GetVerticalLayoutEnabled())
      {
      // Set the width of the table to 100%.
      prop.type = NW_CSS_ValueType_Percentage;
      prop.value.decimal = 100.0;
      status = NW_LMgr_Box_SetProperty(aBox, NW_CSS_Prop_width, &prop);
      NW_THROW_ON(status, KBrsrOutOfMemory);
      }

    // Otherwise restore the table's original width.
    else
      {
      // Get the original width.
      status = NW_LMgr_Box_GetProperty(aBox, NW_CSS_Prop_gridModeOrigWidth, &prop);
      NW_THROW_ON(status, KBrsrOutOfMemory);

      // Restore the table's original width to the value found in 
      // NW_CSS_Prop_gridModeOrigWidth.
      if (status == KBrsrSuccess)
        {
        status = NW_LMgr_Box_SetProperty(aBox, NW_CSS_Prop_width, &prop);
        NW_THROW_ON(status, KBrsrOutOfMemory);
        }
      
      // Otherwise restore the table's original width by removing the width property
      // as it didn't originally have a width.
      else if (status == KBrsrNotFound)
        {
        status = NW_LMgr_Box_RemoveProperty(aBox, NW_CSS_Prop_width);
        NW_THROW_ON(status, KBrsrOutOfMemory);
        }
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

/* ------------------------------------------------------------------------- */
static TBrowserStatusCode
NW_XHTML_AttributePropertyTable_ApplyStyles_Helper (NW_XHTML_ContentHandler_t* contentHandler,
                                                    NW_DOM_ElementNode_t* elementNode,
                                                    NW_Uint16 elementToken,
                                                    NW_LMgr_Box_t* box)
  {
  TBrowserStatusCode status = KBrsrSuccess;
  NW_Uint16 attributeToken;

  NW_LMgr_PropertyName_t CSSPropName;
  NW_LMgr_Property_t p, prop;
  NW_Ucs2 *attrValStr = NULL;
  NW_Uint32 numElementEntries;
  NW_Uint32 index;
  const NW_XHTML_AttributePropertyTable_ElementEntry_t* elementEntries;
  NW_Bool freeNeeded = NW_FALSE;

  numElementEntries =
    NW_XHTML_AttributePropertyTable_GetClassPart(&NW_XHTML_AttributePropertyTable).numElementEntries;
  elementEntries =
    NW_XHTML_AttributePropertyTable_GetClassPart(&NW_XHTML_AttributePropertyTable).elementEntries;

  // Find the first match for the element token. It is a requirement
  // that all occurences of the element token must be grouped together.
  for (index = 0; index < numElementEntries; index++, elementEntries++)
    {
    if (elementEntries->elementToken == elementToken)
      {
      break;
      }
    }
  // Traverse the entries which match the element token
  // until the first occurence of the next element token is found.
  for (; index < numElementEntries; index++, elementEntries++)
    {
    freeNeeded = NW_FALSE;
    if (elementEntries->elementToken != elementToken)
      {
      break;
      }
    status = KBrsrSuccess;
    attributeToken = elementEntries->attributeToken;

    // This is a hack to avoid having to add another field to the element
    // entries.  IMODE is not compatible with CSS in the way lists
    // are handled
    if ((elementToken == NW_XHTML_ElementToken_li) &&
      (attributeToken == NW_XHTML_AttributeToken_type))
      {
      NW_DOM_ElementNode_t* parentNode;
      NW_Uint16 parentToken;

      parentNode = (NW_DOM_ElementNode_t*)NW_DOM_Node_getParentNode((NW_DOM_Node_t*)elementNode);
      parentToken = (NW_Uint16) (NW_HED_DomHelper_GetElementToken (parentNode));

      if ((parentToken == NW_XHTML_ElementToken_ol) &&
        (elementEntries->CSSPropVals != NW_XHTML_APT_OLTypeValues))
        {
        continue;
        }
      if ((parentToken == NW_XHTML_ElementToken_ul) &&
        (elementEntries->CSSPropVals != NW_XHTML_APT_ULTypeValues))
        {
        continue;
        }
      }

    // Look for the attribute on the current dom node and get its value.
    // Requirement: The HTML parser strips leading and trailing whitespaces
    // from attribute value strings before returning them.
    if (NW_XHTML_ElementHandler_GetAttribute(contentHandler,
        elementNode, attributeToken, &attrValStr) == KBrsrSuccess)
        {
        freeNeeded = NW_TRUE;
        }
    else
        {
        // This is code for handling attributes (align, bordercolor) is
        // set on the table or table row, but not on the cell.
        // This could be moved somewhere better in the future.
        if (((elementToken == NW_XHTML_ElementToken_td) || (elementToken == NW_XHTML_ElementToken_th)) &&
            ((attributeToken == NW_XHTML_AttributeToken_align) ||
            (attributeToken == NW_XHTML_AttributeToken_valign) ||
            (attributeToken == NW_XHTML_AttributeToken_bordercolor)))
            {
            // Check Parent, which must be a TableRow, for align, valign, or bordercolor
            NW_DOM_ElementNode_t* parentNode = (NW_DOM_ElementNode_t*)NW_DOM_Node_getParentNode((NW_DOM_Node_t*)elementNode);
            if (NW_XHTML_ElementHandler_GetAttribute(contentHandler,
                parentNode, attributeToken, &attrValStr) == KBrsrSuccess)
                {
                freeNeeded = NW_TRUE;
                }

            if (freeNeeded == NW_FALSE &&
                attributeToken == NW_XHTML_AttributeToken_bordercolor)
                {
                // Check Grandparent, which must be a Table, for bordercolor.
                parentNode = (NW_DOM_ElementNode_t*)NW_DOM_Node_getParentNode((NW_DOM_Node_t*)parentNode);
                if (NW_XHTML_ElementHandler_GetAttribute(
                    contentHandler, parentNode, attributeToken, &attrValStr) == KBrsrSuccess)
                    {
                    freeNeeded = NW_TRUE;
                    }
                }
            }
        }

    if (freeNeeded == NW_FALSE)
        {
        continue;
        }

    CSSPropName = elementEntries->CSSPropName;

    // If property is already set by CSS, IHTML attributes will not override it
    if (NW_LMgr_Box_GetPropertyFromList(box, CSSPropName, &p) == KBrsrSuccess)
      {
      //lint -e{774} Boolean within 'if' always evaluates to False
      if (freeNeeded)
        {
        NW_Str_Delete (attrValStr);
        }
      continue;
      }

    // Each value type has its own subroutine to setup the prop structure.
    switch (elementEntries->type)
      {
      case NW_XHTML_APT_TOKEN:
        {
        status = NW_XHTML_AttributePropertyTable_SetTokenProperty(&NW_XHTML_AttributePropertyTable,
          elementEntries, attrValStr, &prop);
        break;
        }
      case NW_XHTML_APT_TRANSCOLOR:
        {
        status = NW_XHTML_AttributePropertyTable_SetColorProperty(&NW_XHTML_AttributePropertyTable,
          elementEntries, attrValStr, NW_TRUE, &prop);
        break;
        }
      case NW_XHTML_APT_COLOR:
        {
        status = NW_XHTML_AttributePropertyTable_SetColorProperty(&NW_XHTML_AttributePropertyTable,
          elementEntries, attrValStr, NW_FALSE, &prop);
        break;
        }
      case NW_XHTML_APT_PIXEL:
        {
        status = NW_XHTML_AttributePropertyTable_SetPixelProperty(attrValStr, &prop);
        break;
        }
      case NW_XHTML_APT_INTEGER:
        {
        status = NW_XHTML_AttributePropertyTable_SetIntegerProperty(attrValStr, &prop);
        break;
        }
      case NW_XHTML_APT_LENGTH:
        {
        status = NW_XHTML_AttributePropertyTable_SetLengthProperty(attrValStr, &prop);
        break;
        }
      case NW_XHTML_APT_STRING:
        {
        if (CSSPropName == NW_CSS_Prop_backgroundImage)
          {
          status = NW_XHTML_AttributePropertyTable_SetUrlProperty(contentHandler, attrValStr, &prop,
            NW_CSS_Prop_backgroundImage ,box );
          }
        else
          {
          status = NW_XHTML_AttributePropertyTable_SetStringProperty(attrValStr, &prop);
          }
        break;
        }
      default:
        {
        status = KBrsrFailure;
        break;
        }
      }

    // Set the property on the layout manager box.
    if (status == KBrsrSuccess)
      {
      status = NW_LMgr_Box_SetProperty (box, CSSPropName, &prop);
      // Some properties require an additional property to be set
      if (status == KBrsrSuccess && elementEntries->requires != NULL)
        {
        NW_LMgr_Property_t p1 = elementEntries->requires->setting;
        status = NW_LMgr_Box_SetProperty (box,
          elementEntries->requires->CSSPropName,
          &p1);
        }
      }

    //lint -e{774} Boolean within 'if' always evaluates to False
    if( freeNeeded )
      {
      NW_Str_Delete( attrValStr );
      }
    }
  switch( elementToken )
    {
    case NW_XHTML_ElementToken_a:
      {
      status = NW_XHTML_AttributePropertyTable_ApplyAccessKey(contentHandler,
        elementNode, box);

      if (status == KBrsrOutOfMemory)
      {
        return status;
      }
      status = NW_XHTML_AttributePropertyTable_ApplyDirectKey(contentHandler,
        elementNode, box);
      break;
      }
    case NW_XHTML_ElementToken_textarea:
    case NW_XHTML_ElementToken_input:
     {
       status = NW_XHTML_AttributePropertyTable_ApplyAccessKey(contentHandler,
                                                               elementNode, box);
       if (status == KBrsrOutOfMemory)
         {
           return status;
         }

       if (elementToken == NW_XHTML_ElementToken_input)
         {
           status = NW_XHTML_AttributePropertyTable_SetHtWidth(box, contentHandler);
           if (status == KBrsrOutOfMemory)
             {
               return status;
             }

           status = NW_XHTML_AttributePropertyTable_ApplyDirectKey(contentHandler,
                                                                   elementNode, box);
           if (status == KBrsrOutOfMemory)
             {
               return status;
             }
         }

      break;
      }
    case NW_XHTML_ElementToken_select:
      {
      status = NW_XHTML_AttributePropertyTable_ApplyAccessKey( contentHandler,
        elementNode, box );
      break;
      }
    case NW_XHTML_ElementToken_table:
      {
      status = NW_XHTML_AttributePropertyTable_ApplyStyleTable(contentHandler, elementNode, box);
      break;
      }
    case NW_XHTML_ElementToken_img:
      {
      status = NW_XHTML_AttributePropertyTable_CheckForFloat(box, NW_FALSE);

      // Ignore non-out-of-memory errors.
      if (status != KBrsrOutOfMemory)
        {
        status = KBrsrSuccess;
        }
      break;
      }
    case NW_XHTML_ElementToken_frameset:
    case NW_XHTML_ElementToken_body:
      {
      status = NW_XHTML_AttributePropertyTable_CheckForFloat(box, NW_FALSE);

      // Ignore non-out-of-memory errors.
      if (status != KBrsrOutOfMemory)
        {
        NW_LMgr_Property_t p1;
        p1.type = NW_CSS_ValueType_Token;
        p1.value.token = NW_CSS_PropValue_flags_bodyElement;
        status = NW_LMgr_Box_SetProperty(box, NW_CSS_Prop_flags, &p1);
        }
      break;
      }
    case NW_XHTML_ElementToken_font:
    case NW_XHTML_ElementToken_b:
      {
      status = NW_XHTML_AttributePropertyTable_CheckForFloat(box, NW_FALSE);

      // Ignore non-out-of-memory errors.
      if (status != KBrsrOutOfMemory)
        {
        status = NW_XHTML_AttributePropertyTable_NeedFlowBox(elementNode, box);
        }
      break;
      }
    case NW_XHTML_ElementToken_caption:
      {
      status = NW_XHTML_AttributePropertyTable_HandleCaption(box);
      break;
      }
    case NW_XHTML_ElementToken_area:
      {
      status = NW_XHTML_AttributePropertyTable_ApplyAccessKey(contentHandler, elementNode, box);
      break;
      }
    default:
      {
      status = NW_XHTML_AttributePropertyTable_CheckForFloat(box, NW_FALSE);

      // Ignore non-out-of-memory errors.
      if (status != KBrsrOutOfMemory)
        {
        status = KBrsrSuccess;
        }
      break;
      }
    }
    return status;
  }

/* -------------------------------------------------------------------------
            public methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_XHTML_AttributePropertyTable_ApplyStyles(NW_XHTML_ContentHandler_t* contentHandler,
                                            NW_DOM_ElementNode_t* elementNode,
                                            NW_LMgr_Box_t* box)
{
  TBrowserStatusCode stat;
  NW_Uint16 elementToken = NW_HED_DomHelper_GetElementToken (elementNode);

  /* apply attributes that apply to the element */
  stat = NW_XHTML_AttributePropertyTable_ApplyStyles_Helper(contentHandler, elementNode,
      elementToken, box);

  /* apply attributes that apply to ALL elements */
  if (stat != KBrsrOutOfMemory)
  {
    stat = NW_XHTML_AttributePropertyTable_ApplyStyles_Helper(contentHandler, elementNode,
        NW_XHTML_ElementToken_zzzunknown, box);
  }

  return stat;
}

