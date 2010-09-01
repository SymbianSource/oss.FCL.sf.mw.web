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


#ifndef NW_CSSPROPERTIES_H
#define NW_CSSPROPERTIES_H

#include "nwx_defs.h"
#include "nw_lmgr_propertylist.h"

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */


/* Property names are encoded in a single NW_Byte. Two bits are inheritance flags,
 * and the rest specify the property itself. This should be sufficient space
 * to encode all of the properties specified by the CSS mobile profile.
 */

/* The top bit of a property name signifies that it is inherited normally */

#define NW_CSS_PropInherit            ((NW_LMgr_PropertyName_t) 0x80)

#define NW_CSS_CachedProperty_Num     14

/* ----Property name tokens-------------------------------------------------------- */

/* Non-inherited properties */

//!!! cached non-inherited properties !!!
#define NW_CSS_Prop_padding             ((NW_LMgr_PropertyName_t)  1)
#define NW_CSS_Prop_display             ((NW_LMgr_PropertyName_t)  2)
#define NW_CSS_Prop_borderWidth         ((NW_LMgr_PropertyName_t)  3)
#define NW_CSS_Prop_sibling             ((NW_LMgr_PropertyName_t)  4)
#define NW_CSS_Prop_borderStyle         ((NW_LMgr_PropertyName_t)  5) 
#define NW_CSS_Prop_float               ((NW_LMgr_PropertyName_t)  6)
//!!! end of cached non-inherited properties !!!

//!!! non-cached non-inherited properties!!! numbering MUST start with the last cached inherited ID + 1
#define NW_CSS_Prop_borderColor         ((NW_LMgr_PropertyName_t) 15)
#define NW_CSS_Prop_overflow            ((NW_LMgr_PropertyName_t) 16)
#define NW_CSS_Prop_leftPadding         ((NW_LMgr_PropertyName_t) 17)
#define NW_CSS_Prop_rightPadding        ((NW_LMgr_PropertyName_t) 18)
#define NW_CSS_Prop_topPadding          ((NW_LMgr_PropertyName_t) 19)
#define NW_CSS_Prop_bottomPadding       ((NW_LMgr_PropertyName_t) 20)
#define NW_CSS_Prop_margin              ((NW_LMgr_PropertyName_t) 21)
#define NW_CSS_Prop_leftMargin          ((NW_LMgr_PropertyName_t) 22)
#define NW_CSS_Prop_rightMargin         ((NW_LMgr_PropertyName_t) 23)
#define NW_CSS_Prop_topMargin           ((NW_LMgr_PropertyName_t) 24)
#define NW_CSS_Prop_bottomMargin        ((NW_LMgr_PropertyName_t) 25)
#define NW_CSS_Prop_pageBreakAfter      ((NW_LMgr_PropertyName_t) 26)
#define NW_CSS_Prop_height              ((NW_LMgr_PropertyName_t) 27)
#define NW_CSS_Prop_width               ((NW_LMgr_PropertyName_t) 28)
#define NW_CSS_Prop_backgroundColor     ((NW_LMgr_PropertyName_t) 29)
#define NW_CSS_Prop_clear               ((NW_LMgr_PropertyName_t) 30)
#define NW_CSS_Prop_leftBorderWidth     ((NW_LMgr_PropertyName_t) 31)
#define NW_CSS_Prop_rightBorderWidth    ((NW_LMgr_PropertyName_t) 32)
#define NW_CSS_Prop_topBorderWidth      ((NW_LMgr_PropertyName_t) 33)
#define NW_CSS_Prop_bottomBorderWidth   ((NW_LMgr_PropertyName_t) 34)
#define NW_CSS_Prop_leftBorderColor     ((NW_LMgr_PropertyName_t) 35)
#define NW_CSS_Prop_rightBorderColor    ((NW_LMgr_PropertyName_t) 36)
#define NW_CSS_Prop_topBorderColor      ((NW_LMgr_PropertyName_t) 37)
#define NW_CSS_Prop_bottomBorderColor   ((NW_LMgr_PropertyName_t) 38)
#define NW_CSS_Prop_leftBorderStyle     ((NW_LMgr_PropertyName_t) 39)
#define NW_CSS_Prop_rightBorderStyle    ((NW_LMgr_PropertyName_t) 40)
#define NW_CSS_Prop_topBorderStyle      ((NW_LMgr_PropertyName_t) 41)
#define NW_CSS_Prop_bottomBorderStyle   ((NW_LMgr_PropertyName_t) 42)
#define NW_CSS_Prop_border              ((NW_LMgr_PropertyName_t) 43)
#define NW_CSS_Prop_borderTop           ((NW_LMgr_PropertyName_t) 44)
#define NW_CSS_Prop_borderRight         ((NW_LMgr_PropertyName_t) 45)
#define NW_CSS_Prop_borderBottom        ((NW_LMgr_PropertyName_t) 46)
#define NW_CSS_Prop_borderLeft          ((NW_LMgr_PropertyName_t) 47)
#define NW_CSS_Prop_rowSpan             ((NW_LMgr_PropertyName_t) 48)  /* Non-standard */
#define NW_CSS_Prop_colSpan             ((NW_LMgr_PropertyName_t) 49)  /* Non-standard */
#define NW_CSS_Prop_captionSide         ((NW_LMgr_PropertyName_t) 50)
#define NW_CSS_Prop_flags               ((NW_LMgr_PropertyName_t) 51)  /* Internal */
#define NW_CSS_Prop_accesskey           ((NW_LMgr_PropertyName_t) 52)  /* -wap-accesskey */
#define NW_CSS_Prop_inputFormat         ((NW_LMgr_PropertyName_t) 53)  /* -wap-input-format */
#define NW_CSS_Prop_inputRequired       ((NW_LMgr_PropertyName_t) 54)  /* -wap-input-required */
#define NW_CSS_Prop_flowBaseline        ((NW_LMgr_PropertyName_t) 55)  /* Internal */  
#define NW_CSS_Prop_listValue           ((NW_LMgr_PropertyName_t) 56)  /* Internal, used by IMODE */
#define NW_CSS_Prop_focusBehavior       ((NW_LMgr_PropertyName_t) 57)  /* Internal */
#define NW_CSS_Prop_background          ((NW_LMgr_PropertyName_t) 58)  
#define NW_CSS_Prop_marqueeStyle        ((NW_LMgr_PropertyName_t) 59)  
#define NW_CSS_Prop_marqueeLoop         ((NW_LMgr_PropertyName_t) 60)  
#define NW_CSS_Prop_marqueeDir          ((NW_LMgr_PropertyName_t) 61)  
#define NW_CSS_Prop_marqueeSpeed        ((NW_LMgr_PropertyName_t) 62)  
#define NW_CSS_Prop_marqueeAmount       ((NW_LMgr_PropertyName_t) 63)  
#define NW_CSS_Prop_tableAlign          ((NW_LMgr_PropertyName_t) 64)  /* Internal, used by IMODE */
#define NW_CSS_Prop_cellPadding         ((NW_LMgr_PropertyName_t) 65)  /* Internal, used by IMODE */
#define NW_CSS_Prop_imodeQuotes         ((NW_LMgr_PropertyName_t) 66)  /* Internal, used by IMODE */
#define NW_CSS_Prop_noshade             ((NW_LMgr_PropertyName_t) 67)  /* Internal, used by IMODE */  
#define NW_CSS_Prop_backgroundImage     ((NW_LMgr_PropertyName_t) 68)
#define NW_CSS_Prop_backgroundRepeat    ((NW_LMgr_PropertyName_t) 69)
#define NW_CSS_Prop_backgroundPosition  ((NW_LMgr_PropertyName_t) 70)
#define NW_CSS_Prop_backgroundAttachment  ((NW_LMgr_PropertyName_t) 71)
#define NW_CSS_Prop_backgroundPosition_x  ((NW_LMgr_PropertyName_t) 72)
#define NW_CSS_Prop_backgroundPosition_y  ((NW_LMgr_PropertyName_t) 73)
#define NW_CSS_Prop_contentLength       ((NW_LMgr_PropertyName_t) 74)  /* Internal */
#define NW_CSS_Prop_contentType         ((NW_LMgr_PropertyName_t) 75)  /* Internal */


/* application-defined non-inherited properties */
#define NW_CSS_Prop_elementId           ((NW_LMgr_PropertyName_t) 76)
#define NW_CSS_Prop_tabIndex            ((NW_LMgr_PropertyName_t) 77)
#define NW_CSS_Prop_boxName             ((NW_LMgr_PropertyName_t) 78)
#define NW_CSS_Prop_listMargin          ((NW_LMgr_PropertyName_t) 79)

// support for JSKY attributes
#define NW_CSS_Prop_directkey           ((NW_LMgr_PropertyName_t) 80) 
#define NW_CSS_Prop_istyle              ((NW_LMgr_PropertyName_t) 81)
#define NW_CSS_Prop_mode                ((NW_LMgr_PropertyName_t) 82)

#define NW_CSS_Prop_verticalAlign       ((NW_LMgr_PropertyName_t) 83)
#define NW_CSS_Prop_contentHandlerId    ((NW_LMgr_PropertyName_t) 84)

// support for customer soundstart attributes
#define NW_CSS_Prop_soundstartAction    ((NW_LMgr_PropertyName_t) 85)
#define NW_CSS_Prop_soundstartLoop      ((NW_LMgr_PropertyName_t) 86)
#define NW_CSS_Prop_soundstartVolume    ((NW_LMgr_PropertyName_t) 87)

// support for CSS position
#define NW_CSS_Prop_position            ((NW_LMgr_PropertyName_t) 88)
#define NW_CSS_Prop_left                ((NW_LMgr_PropertyName_t) 89)
#define NW_CSS_Prop_right               ((NW_LMgr_PropertyName_t) 90)
#define NW_CSS_Prop_top                 ((NW_LMgr_PropertyName_t) 91)
#define NW_CSS_Prop_bottom              ((NW_LMgr_PropertyName_t) 92)

/* Inherited properties */

//!!! cached inherited properties!!! numbering MUST start with the last non-inherited cached ID + 1
#define NW_CSS_Prop_textAlign           ((NW_LMgr_PropertyName_t)  7 | NW_CSS_PropInherit)
#define NW_CSS_Prop_textIndent          ((NW_LMgr_PropertyName_t)  8 | NW_CSS_PropInherit)
#define NW_CSS_Prop_textTransform       ((NW_LMgr_PropertyName_t)  9 | NW_CSS_PropInherit)
#define NW_CSS_Prop_whitespace          ((NW_LMgr_PropertyName_t)  10| NW_CSS_PropInherit)
#define NW_CSS_Prop_visibility          ((NW_LMgr_PropertyName_t)  11| NW_CSS_PropInherit)
#define NW_CSS_Prop_lineHeight          ((NW_LMgr_PropertyName_t)  12| NW_CSS_PropInherit)
#define NW_CSS_Prop_textDecoration      ((NW_LMgr_PropertyName_t)  13| NW_CSS_PropInherit)
#define NW_CSS_Prop_textDirection       ((NW_LMgr_PropertyName_t)  14| NW_CSS_PropInherit) /* internal */
//!!! end of cached inherited properties!!! 


#define NW_CSS_Prop_fontFamily          ((NW_LMgr_PropertyName_t)  15| NW_CSS_PropInherit)
#define NW_CSS_Prop_fontSize            ((NW_LMgr_PropertyName_t)  16| NW_CSS_PropInherit)
#define NW_CSS_Prop_fontStyle           ((NW_LMgr_PropertyName_t)  17| NW_CSS_PropInherit)
#define NW_CSS_Prop_fontWeight          ((NW_LMgr_PropertyName_t)  18| NW_CSS_PropInherit)
#define NW_CSS_Prop_listStyleType       ((NW_LMgr_PropertyName_t)  19| NW_CSS_PropInherit)
#define NW_CSS_Prop_listStyleImage      ((NW_LMgr_PropertyName_t)  20| NW_CSS_PropInherit)
#define NW_CSS_Prop_color               ((NW_LMgr_PropertyName_t)  21| NW_CSS_PropInherit)
#define NW_CSS_Prop_fontVariant         ((NW_LMgr_PropertyName_t)  22| NW_CSS_PropInherit)
#define NW_CSS_Prop_listStylePosition   ((NW_LMgr_PropertyName_t)  23| NW_CSS_PropInherit)
#define NW_CSS_Prop_font                ((NW_LMgr_PropertyName_t)  24| NW_CSS_PropInherit)
#define NW_CSS_Prop_listStyle           ((NW_LMgr_PropertyName_t)  25| NW_CSS_PropInherit)
#define NW_CSS_Prop_fontObject          ((NW_LMgr_PropertyName_t)  26| NW_CSS_PropInherit) /* internal */
#define NW_CSS_Prop_blinkState          ((NW_LMgr_PropertyName_t)  27| NW_CSS_PropInherit) /* internal */
#define NW_CSS_Prop_gridModeApplied     ((NW_LMgr_PropertyName_t)  28| NW_CSS_PropInherit) /* internal */
#define NW_CSS_Prop_gridModeOrigWidth   ((NW_LMgr_PropertyName_t)  29| NW_CSS_PropInherit) /* internal */

/* ----Property value tokens------------------------------------------------------ */

#define NW_CSS_PropValue_none                           ((NW_LMgr_PropertyValueToken_t)  0)
#define NW_CSS_PropValue_left                           ((NW_LMgr_PropertyValueToken_t)  1)
#define NW_CSS_PropValue_right                          ((NW_LMgr_PropertyValueToken_t)  2)
#define NW_CSS_PropValue_normal                         ((NW_LMgr_PropertyValueToken_t)  3)
#define NW_CSS_PropValue_italic                         ((NW_LMgr_PropertyValueToken_t)  4)
#define NW_CSS_PropValue_bold                           ((NW_LMgr_PropertyValueToken_t)  5)
#define NW_CSS_PropValue_disc                           ((NW_LMgr_PropertyValueToken_t)  6)
#define NW_CSS_PropValue_circle                         ((NW_LMgr_PropertyValueToken_t)  7)
#define NW_CSS_PropValue_square                         ((NW_LMgr_PropertyValueToken_t)  8)
#define NW_CSS_PropValue_decimal                        ((NW_LMgr_PropertyValueToken_t)  9)
#define NW_CSS_PropValue_lowerRoman                     ((NW_LMgr_PropertyValueToken_t) 10)
#define NW_CSS_PropValue_upperRoman                     ((NW_LMgr_PropertyValueToken_t) 11)
#define NW_CSS_PropValue_lowerAlpha                     ((NW_LMgr_PropertyValueToken_t) 12)
#define NW_CSS_PropValue_upperAlpha                     ((NW_LMgr_PropertyValueToken_t) 13)

#define NW_CSS_PropValue_always                         ((NW_LMgr_PropertyValueToken_t) 20)
#define NW_CSS_PropValue_avoid                          ((NW_LMgr_PropertyValueToken_t) 21)
#define NW_CSS_PropValue_center                         ((NW_LMgr_PropertyValueToken_t) 22)
#define NW_CSS_PropValue_justify                        ((NW_LMgr_PropertyValueToken_t) 23)
#define NW_CSS_PropValue_underline                      ((NW_LMgr_PropertyValueToken_t) 24)
#define NW_CSS_PropValue_blink                          ((NW_LMgr_PropertyValueToken_t) 25)
#define NW_CSS_PropValue_strikeout                      ((NW_LMgr_PropertyValueToken_t) 26)

/* Display prop values */
#define NW_CSS_PropValue_display_inline                 ((NW_LMgr_PropertyValueToken_t) 27)
#define NW_CSS_PropValue_display_block                  ((NW_LMgr_PropertyValueToken_t) 28)
#define NW_CSS_PropValue_display_listItem               ((NW_LMgr_PropertyValueToken_t) 30)
#define NW_CSS_PropValue_display_marquee                ((NW_LMgr_PropertyValueToken_t) 31)

/* In CSS float is a property, not a value of display */
#define NW_CSS_PropValue_display_table                  ((NW_LMgr_PropertyValueToken_t) 32)
#define NW_CSS_PropValue_display_inlineTable            ((NW_LMgr_PropertyValueToken_t) 33)

/* JSKY display value */
#define NW_CSS_PropValue_display_directKey              ((NW_LMgr_PropertyValueToken_t) 34)

/* Alignment values */
#define NW_CSS_PropValue_top                            ((NW_LMgr_PropertyValueToken_t) 36)
#define NW_CSS_PropValue_bottom                         ((NW_LMgr_PropertyValueToken_t) 37)
#define NW_CSS_PropValue_baseline                       ((NW_LMgr_PropertyValueToken_t) 38)

/* additional prop vals */
#define NW_CSS_PropValue_inherit                        ((NW_LMgr_PropertyValueToken_t) 39)
#define NW_CSS_PropValue_auto                           ((NW_LMgr_PropertyValueToken_t) 40)
#define NW_CSS_PropValue_both                           ((NW_LMgr_PropertyValueToken_t) 41)

/* visibility prop vals */
#define NW_CSS_PropValue_visible                        ((NW_LMgr_PropertyValueToken_t) 43)
#define NW_CSS_PropValue_hidden                         ((NW_LMgr_PropertyValueToken_t) 44)
#define NW_CSS_PropValue_collapse                       ((NW_LMgr_PropertyValueToken_t) 45)

/* white-space prop vals */
#define NW_CSS_PropValue_pre                            ((NW_LMgr_PropertyValueToken_t) 46)
#define NW_CSS_PropValue_nowrap                         ((NW_LMgr_PropertyValueToken_t) 47)

/* text-transform prop vals */
#define NW_CSS_PropValue_capitalize                     ((NW_LMgr_PropertyValueToken_t) 48)
#define NW_CSS_PropValue_uppercase                      ((NW_LMgr_PropertyValueToken_t) 49)
#define NW_CSS_PropValue_lowercase                      ((NW_LMgr_PropertyValueToken_t) 50)

/* font-variant prop vals */
#define NW_CSS_PropValue_smallcaps                      ((NW_LMgr_PropertyValueToken_t) 51)

/* font-weight prop vals */
#define NW_CSS_PropValue_bolder                         ((NW_LMgr_PropertyValueToken_t) 52)
#define NW_CSS_PropValue_lighter                        ((NW_LMgr_PropertyValueToken_t) 53)

/* border-style vals */
/* Note:  Do not change the order of these values because
   the border collapsing algorithm for static tables relies on them */
#define NW_CSS_PropValue_dotted                         ((NW_LMgr_PropertyValueToken_t) 54)
#define NW_CSS_PropValue_dashed                         ((NW_LMgr_PropertyValueToken_t) 55)
#define NW_CSS_PropValue_solid                          ((NW_LMgr_PropertyValueToken_t) 56)
#define NW_CSS_PropValue_double                         ((NW_LMgr_PropertyValueToken_t) 57)

/* font-size prop vals */
#define NW_CSS_PropValue_xxSmall                        ((NW_LMgr_PropertyValueToken_t) 58)
#define NW_CSS_PropValue_xSmall                         ((NW_LMgr_PropertyValueToken_t) 59)
#define NW_CSS_PropValue_small                          ((NW_LMgr_PropertyValueToken_t) 60)
#define NW_CSS_PropValue_xxLarge                        ((NW_LMgr_PropertyValueToken_t) 61)
#define NW_CSS_PropValue_xLarge                         ((NW_LMgr_PropertyValueToken_t) 62)
#define NW_CSS_PropValue_large                          ((NW_LMgr_PropertyValueToken_t) 63)
#define NW_CSS_PropValue_smaller                        ((NW_LMgr_PropertyValueToken_t) 64)
#define NW_CSS_PropValue_larger                         ((NW_LMgr_PropertyValueToken_t) 65)
#define NW_CSS_PropValue_medium                         ((NW_LMgr_PropertyValueToken_t) 66)

/* font-style prop vals */
#define NW_CSS_PropValue_oblique                        ((NW_LMgr_PropertyValueToken_t) 67)

/* border width types */
#define NW_CSS_PropValue_thin                           ((NW_LMgr_PropertyValueToken_t) 68)
#define NW_CSS_PropValue_thick                          ((NW_LMgr_PropertyValueToken_t) 69)

/* list-style-position */
#define NW_CSS_PropValue_inside                         ((NW_LMgr_PropertyValueToken_t) 70)

/* background-color */
#define NW_CSS_PropValue_transparent                    ((NW_LMgr_PropertyValueToken_t) 71)

/* colors */
#define NW_CSS_PropValue_aqua                           ((NW_LMgr_PropertyValueToken_t) 72)
#define NW_CSS_PropValue_black                          ((NW_LMgr_PropertyValueToken_t) 73)
#define NW_CSS_PropValue_blue                           ((NW_LMgr_PropertyValueToken_t) 74)
#define NW_CSS_PropValue_cyan                           ((NW_LMgr_PropertyValueToken_t) 75)
#define NW_CSS_PropValue_fuchsia                        ((NW_LMgr_PropertyValueToken_t) 76)
#define NW_CSS_PropValue_gray                           ((NW_LMgr_PropertyValueToken_t) 77)
#define NW_CSS_PropValue_green                          ((NW_LMgr_PropertyValueToken_t) 78)
#define NW_CSS_PropValue_lime                           ((NW_LMgr_PropertyValueToken_t) 79)
#define NW_CSS_PropValue_maroon                         ((NW_LMgr_PropertyValueToken_t) 80)
#define NW_CSS_PropValue_navy                           ((NW_LMgr_PropertyValueToken_t) 81)
#define NW_CSS_PropValue_olive                          ((NW_LMgr_PropertyValueToken_t) 82)
#define NW_CSS_PropValue_pink                           ((NW_LMgr_PropertyValueToken_t) 73)
#define NW_CSS_PropValue_purple                         ((NW_LMgr_PropertyValueToken_t) 84)
#define NW_CSS_PropValue_red                            ((NW_LMgr_PropertyValueToken_t) 85)
#define NW_CSS_PropValue_silver                         ((NW_LMgr_PropertyValueToken_t) 86)
#define NW_CSS_PropValue_teal                           ((NW_LMgr_PropertyValueToken_t) 87)
#define NW_CSS_PropValue_white                          ((NW_LMgr_PropertyValueToken_t) 88)
#define NW_CSS_PropValue_yellow                         ((NW_LMgr_PropertyValueToken_t) 89)

/* additional valign */
#define NW_CSS_PropValue_middle                         ((NW_LMgr_PropertyValueToken_t) 101)
#define NW_CSS_PropValue_sub                            ((NW_LMgr_PropertyValueToken_t) 102)
#define NW_CSS_PropValue_super                          ((NW_LMgr_PropertyValueToken_t) 103)

/* additional liststyleposition */
#define NW_CSS_PropValue_outside                        ((NW_LMgr_PropertyValueToken_t) 111)

/* marquee values */
#define NW_CSS_PropValue_scroll                         ((NW_LMgr_PropertyValueToken_t) 121)
#define NW_CSS_PropValue_slide                          ((NW_LMgr_PropertyValueToken_t) 122)
#define NW_CSS_PropValue_alternate                      ((NW_LMgr_PropertyValueToken_t) 123)
#define NW_CSS_PropValue_infinite                       ((NW_LMgr_PropertyValueToken_t) 124)
#define NW_CSS_PropValue_ltr                            ((NW_LMgr_PropertyValueToken_t) 125)
#define NW_CSS_PropValue_rtl                            ((NW_LMgr_PropertyValueToken_t) 126)
#define NW_CSS_PropValue_slow                           ((NW_LMgr_PropertyValueToken_t) 127)
#define NW_CSS_PropValue_fast                           ((NW_LMgr_PropertyValueToken_t) 128)

/* focus behaviors */
#define NW_CSS_PropValue_growBorder                     ((NW_LMgr_PropertyValueToken_t) 141)
#define NW_CSS_PropValue_altText                        ((NW_LMgr_PropertyValueToken_t) 142)

/* Internal border style */
#define NW_CSS_PropValue_bevelled                       ((NW_LMgr_PropertyValueToken_t) 151)

/* Boolean values */
#define NW_CSS_PropValue_true                           ((NW_LMgr_PropertyValueToken_t) 161)
#define NW_CSS_PropValue_false                          ((NW_LMgr_PropertyValueToken_t) 162)

/* Intenal text decoration value */
#define NW_CSS_PropValue_blinkUnderline                 ((NW_LMgr_PropertyValueToken_t) 171)

/* Background repeat values */
#define NW_CSS_PropValue_background_repeat              ((NW_LMgr_PropertyValueToken_t) 181)
#define NW_CSS_PropValue_background_repeat_x            ((NW_LMgr_PropertyValueToken_t) 182)
#define NW_CSS_PropValue_background_repeat_y            ((NW_LMgr_PropertyValueToken_t) 183)
#define NW_CSS_PropValue_background_norepeat            ((NW_LMgr_PropertyValueToken_t) 184)

/* Background attachment values */
#define NW_CSS_PropValue_background_fixed               ((NW_LMgr_PropertyValueToken_t) 191)

/* J-Sky editor input mode values (J-Sky phase2) */
#define NW_CSS_PropValue_hiragana                       ((NW_LMgr_PropertyValueToken_t) 211)
#define NW_CSS_PropValue_katakana                       ((NW_LMgr_PropertyValueToken_t) 212)
#define NW_CSS_PropValue_hankakukana                    ((NW_LMgr_PropertyValueToken_t) 213)
#define NW_CSS_PropValue_alphabet                       ((NW_LMgr_PropertyValueToken_t) 214)
#define NW_CSS_PropValue_numeric                        ((NW_LMgr_PropertyValueToken_t) 215)

/* CSS position properties */
#define NW_CSS_PropValue_position_absolute              ((NW_LMgr_PropertyValueToken_t) 221)
#define NW_CSS_PropValue_position_relative              ((NW_LMgr_PropertyValueToken_t) 222)

/* internal flags */
#define NW_CSS_PropValue_flags_clone                    ((NW_Int32)0x01)
#define NW_CSS_PropValue_flags_wmlTable                 ((NW_Int32)0x02)
#define NW_CSS_PropValue_flags_floatWidth               ((NW_Int32)0x03) /* used to signify when 
                                                        width is set the float algorithm */
#define NW_CSS_PropValue_flags_bodyElement              ((NW_Int32)0x04)
#define NW_CSS_PropValue_flags_thirdBr                  ((NW_Int32)0x05)
#define NW_CSS_PropValue_flags_htmlElement              ((NW_Int32)0x06)
#define NW_CSS_PropValue_flags_smallScreenPre           ((NW_Int32)0x07)


#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_CSSPROPERTIES_H */
