/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Applies CSS property
*
*/



// INCLUDE FILES
#include "CSSPropertyTable.h"

#include "nw_text_ascii.h"
#include "nw_image_virtualimage.h"
#include <nwx_string.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

#define INTEGER_MASK        ((TUint32)(1<<NW_CSS_ValueType_Integer))
#define NUMBER_MASK         ((TUint32)(1<<NW_CSS_ValueType_Number))
#define PX_MASK             ((TUint32)(1<<NW_CSS_ValueType_Px))
#define EM_MASK             ((TUint32)(1<<NW_CSS_ValueType_Em))
#define EX_MASK             ((TUint32)(1<<NW_CSS_ValueType_Ex))
#define IN_MASK             ((TUint32)(1<<NW_CSS_ValueType_In))
#define CM_MASK             ((TUint32)(1<<NW_CSS_ValueType_Cm))
#define MM_MASK             ((TUint32)(1<<NW_CSS_ValueType_Mm))
#define PT_MASK             ((TUint32)(1<<NW_CSS_ValueType_Pt))
#define PC_MASK             ((TUint32)(1<<NW_CSS_ValueType_Pc))
#define LENGTH_MASK         ((TUint32)(PX_MASK | EM_MASK | EX_MASK | IN_MASK | CM_MASK | MM_MASK | PT_MASK | PC_MASK))
#define PERCENTAGE_MASK     ((TUint32)(1<<NW_CSS_ValueType_Percentage))
#define COLOR_MASK          ((TUint32)(1<<NW_CSS_ValueType_Color))
#define TOKEN_MASK          ((TUint32)(1<<NW_CSS_ValueType_Token))
#define TEXT_MASK           ((TUint32)(1<<NW_CSS_ValueType_Text))
#define IMAGE_MASK          ((TUint32)(1<<NW_CSS_ValueType_Image))
#define OBJECT_MASK         ((TUint32)(1<<NW_CSS_ValueType_Object))


static const TText16 PropName_background[] = {'b','a','c','k','g','r','o','u','n','d','\0'};

static const TText16 PropName_border[] = {'b','o','r','d','e','r','\0'};

static const TText16 PropName_border_top[] = {'b','o','r','d','e','r','-','t','o','p','\0'};
static const TText16 PropName_border_right[] = {'b','o','r','d','e','r','-','r','i','g','h','t','\0'};
static const TText16 PropName_border_bottom[] = {'b','o','r','d','e','r','-','b','o','t','t','o','m','\0'};
static const TText16 PropName_border_left[] = {'b','o','r','d','e','r','-','l','e','f','t','\0'};

static const TText16 PropName_border_width[] = {'b','o','r','d','e','r','-','w','i','d','t','h','\0'};
static const TText16 PropName_border_left_width[] = {'b','o','r','d','e','r','-','l','e','f','t','-','w','i','d','t','h','\0'};
static const TText16 PropName_border_right_width[] = {'b','o','r','d','e','r','-','r','i','g','h','t','-','w','i','d','t','h','\0'};
static const TText16 PropName_border_top_width[] = {'b','o','r','d','e','r','-','t','o','p','-','w','i','d','t','h','\0'};
static const TText16 PropName_border_bottom_width[] = {'b','o','r','d','e','r','-','b','o','t','t','o','m','-','w','i','d','t','h','\0'};

static const TText16 PropName_border_color[] = {'b','o','r','d','e','r','-','c','o','l','o','r','\0'};
static const TText16 PropName_border_left_color[] = {'b','o','r','d','e','r','-','l','e','f','t','-','c','o','l','o','r','\0'};
static const TText16 PropName_border_right_color[] = {'b','o','r','d','e','r','-','r','i','g','h','t','-','c','o','l','o','r','\0'};
static const TText16 PropName_border_top_color[] = {'b','o','r','d','e','r','-','t','o','p','-','c','o','l','o','r','\0'};
static const TText16 PropName_border_bottom_color[] = {'b','o','r','d','e','r','-','b','o','t','t','o','m','-','c','o','l','o','r','\0'};

static const TText16 PropName_border_style[] = {'b','o','r','d','e','r','-','s','t','y','l','e','\0'};
static const TText16 PropName_border_left_style[] = {'b','o','r','d','e','r','-','l','e','f','t','-','s','t','y','l','e','\0'};
static const TText16 PropName_border_right_style[] = {'b','o','r','d','e','r','-','r','i','g','h','t','-','s','t','y','l','e','\0'};
static const TText16 PropName_border_top_style[] = {'b','o','r','d','e','r','-','t','o','p','-','s','t','y','l','e','\0'};
static const TText16 PropName_border_bottom_style[] = {'b','o','r','d','e','r','-','b','o','t','t','o','m','-','s','t','y','l','e','\0'};

static const TText16 PropName_color[] = {'c','o','l','o','r','\0'};
static const TText16 PropName_display[] = {'d','i','s','p','l','a','y','\0'};
static const TText16 PropName_float[] = {'f','l','o','a','t','\0'};
static const TText16 PropName_font[] = {'f','o','n','t','\0'};
static const TText16 PropName_font_family[] = {'f','o','n','t','-','f','a','m','i','l','y','\0'};
static const TText16 PropName_font_size[] = {'f','o','n','t','-','s','i','z','e','\0'};
static const TText16 PropName_font_style[] = {'f','o','n','t','-','s','t','y','l','e','\0'};
static const TText16 PropName_font_weight[] = {'f','o','n','t','-','w','e','i','g','h','t','\0'};
static const TText16 PropName_font_variant[] = {'f','o','n','t','-','v','a','r','i','a','n','t','\0'};

static const TText16 PropName_list_style[] = {'l','i','s','t','-','s','t','y','l','e','\0'};
static const TText16 PropName_list_style_image[] = {'l','i','s','t','-','s','t','y','l','e','-','i','m','a','g','e','\0'};
static const TText16 PropName_list_style_type[] = {'l','i','s','t','-','s','t','y','l','e','-','t','y','p','e','\0'};
static const TText16 PropName_list_style_position[] = {'l','i','s','t','-','s','t','y','l','e','-','p','o','s','i','t','i','o','n','\0'};
static const TText16 PropName_list_margin[] = {'l','i','s','t','-','m','a','r','g','i','n','\0'};

static const TText16 PropName_padding[] = {'p','a','d','d','i','n','g','\0'};
static const TText16 PropName_page_break_after[] = {'p','a','g','e','-','b','r','e','a','k','-','a','f','t','e','r','\0'};
static const TText16 PropName_text_align[] = {'t','e','x','t','-','a','l','i','g','n','\0'};
static const TText16 PropName_text_decoration[] = {'t','e','x','t','-','d','e','c','o','r','a','t','i','o','n','\0'};
static const TText16 PropName_text_indent[] = {'t','e','x','t','-','i','n','d','e','n','t','\0'};
static const TText16 PropName_margin[] = {'m','a','r','g','i','n','\0'};
static const TText16 PropName_padding_left[] = {'p','a','d','d','i','n','g','-','l','e','f','t','\0'};
static const TText16 PropName_padding_right[] = {'p','a','d','d','i','n','g','-','r','i','g','h','t','\0'};
static const TText16 PropName_padding_top[] = {'p','a','d','d','i','n','g','-','t','o','p','\0'};
static const TText16 PropName_padding_bottom[] = {'p','a','d','d','i','n','g','-','b','o','t','t','o','m','\0'};
static const TText16 PropName_margin_left[] = {'m','a','r','g','i','n','-','l','e','f','t','\0'};
static const TText16 PropName_margin_right[] = {'m','a','r','g','i','n','-','r','i','g','h','t','\0'};
static const TText16 PropName_margin_top[] = {'m','a','r','g','i','n','-','t','o','p','\0'};
static const TText16 PropName_margin_bottom[] = {'m','a','r','g','i','n','-','b','o','t','t','o','m','\0'};
static const TText16 PropName_vertical_align[] = {'v','e','r','t','i','c','a','l','-','a','l','i','g','n','\0'};
static const TText16 PropName_height[] = {'h','e','i','g','h','t','\0'};
static const TText16 PropName_width[] = {'w','i','d','t','h','\0'};
static const TText16 PropName_text_transform[] = {'t','e','x','t','-','t','r','a','n','s','f','o','r','m','\0'};
static const TText16 PropName_white_space[] = {'w','h','i','t','e','-','s','p','a','c','e','\0'};
static const TText16 PropName_background_color[] = {'b','a','c','k','g','r','o','u','n','d','-','c','o','l','o','r','\0'};
static const TText16 PropName_background_image[] = {'b','a','c','k','g','r','o','u','n','d','-','i','m','a','g','e','\0'};
static const TText16 PropName_background_repeat[] = {'b','a','c','k','g','r','o','u','n','d','-','r','e','p','e','a','t','\0'};
static const TText16 PropName_background_position[] = {'b','a','c','k','g','r','o','u','n','d','-','p','o','s','i','t','i','o','n','\0'};
static const TText16 PropName_background_attachment[] = {'b','a','c','k','g','r','o','u','n','d','-','a','t','t','a','c','h','m','e','n','t','\0'};
static const TText16 PropName_background_position_x[] = {'b','a','c','k','g','r','o','u','n','d','-','p','o','s','i','t','i','o','n','-','x','\0'};
static const TText16 PropName_background_position_y[] = {'b','a','c','k','g','r','o','u','n','d','-','p','o','s','i','t','i','o','n','-','y','\0'};
static const TText16 PropName_clear[] = {'c','l','e','a','r','\0'};
static const TText16 PropName_visibility[] = {'v','i','s','i','b','i','l','i','t','y','\0'};
static const TText16 PropName_caption_side[] = {'c','a','p','t','i','o','n','-','s','i','d','e','\0'};
static const TText16 PropName_line_height[] = {'l','i','n','e','-','h','e','i','g','h','t','\0'};
static const TText16 PropName_overflow[] = {'o','v','e','r','f','l','o','w','\0'};
static const TText16 PropName_accesskey[] = {'-','w','a','p','-','a','c','c','e','s','s','k','e','y','\0'};
static const TText16 PropName_marquee_style[] = {'-','w','a','p','-','m','a','r','q','u','e','e','-','s','t','y','l','e','\0'};
static const TText16 PropName_marquee_loop[] = {'-','w','a','p','-','m','a','r','q','u','e','e','-','l','o','o','p','\0'};
static const TText16 PropName_marquee_dir[] = {'-','w','a','p','-','m','a','r','q','u','e','e','-','d','i','r','\0'};
static const TText16 PropName_marquee_speed[] = {'-','w','a','p','-','m','a','r','q','u','e','e','-','s','p','e','e','d','\0'};
static const TText16 PropName_marquee_amount[] = {'-','x','y','z','-','m','a','r','q','u','e','e','-','a','m','o','u','n','t','\0'};
static const TText16 PropName_focus_behavior[] = {'-','x','y','z','-','f','o','c','u','s','-','b','e','h','a','v','i','o','r','\0'};
static const TText16 PropName_input_format[] = {'-','w','a','p','-','i','n','p','u','t','-','f','o','r','m','a','t','\0'};
static const TText16 PropName_input_required[] = {'-','w','a','p','-','i','n','p','u','t','-','r','e','q','u','i','r','e','d','\0'};
static const TText16 PropName_imode_quotes[] = {'-','x','y','z','-','i','m','o','d','e','-','q','u','o','t','e','s','\0'};
static const TText16 PropName_table_align[] = {'-','x','y','z','-','t','a','b','l','e','-','a','l','i','g','n','\0'};
static const TText16 PropName_noshade[] = {'-','x','y','z','-','n','o','s','h','a','d','e','\0'};
static const TText16 PropName_text_dir[] = {'d','i','r','e','c','t','i','o','n','\0'};

/* ------------------------------------------------------------------------- */
static const
TCSSPropertyTablePropEntry TCSSPropertyTablePropEntries[] =
{
  {PropName_display, NW_CSS_Prop_display, TOKEN_MASK},
  {PropName_border, NW_CSS_Prop_border, LENGTH_MASK | PERCENTAGE_MASK | COLOR_MASK | TOKEN_MASK },
  {PropName_border_top, NW_CSS_Prop_borderTop, LENGTH_MASK | PERCENTAGE_MASK | COLOR_MASK | TOKEN_MASK },
  {PropName_border_right, NW_CSS_Prop_borderRight, LENGTH_MASK | PERCENTAGE_MASK | COLOR_MASK | TOKEN_MASK },
  {PropName_border_bottom, NW_CSS_Prop_borderBottom, LENGTH_MASK | PERCENTAGE_MASK | COLOR_MASK | TOKEN_MASK },
  {PropName_border_left, NW_CSS_Prop_borderLeft, LENGTH_MASK | PERCENTAGE_MASK | COLOR_MASK | TOKEN_MASK },
  {PropName_border_width, NW_CSS_Prop_borderWidth, LENGTH_MASK | PERCENTAGE_MASK },
  {PropName_border_left_width, NW_CSS_Prop_leftBorderWidth, LENGTH_MASK | PERCENTAGE_MASK },
  {PropName_border_right_width, NW_CSS_Prop_rightBorderWidth, LENGTH_MASK | PERCENTAGE_MASK },
  {PropName_border_top_width, NW_CSS_Prop_topBorderWidth, LENGTH_MASK | PERCENTAGE_MASK },
  {PropName_border_bottom_width, NW_CSS_Prop_bottomBorderWidth, LENGTH_MASK | PERCENTAGE_MASK },
  {PropName_border_style, NW_CSS_Prop_borderStyle, TOKEN_MASK },
  {PropName_border_left_style, NW_CSS_Prop_leftBorderStyle, TOKEN_MASK },
  {PropName_border_right_style, NW_CSS_Prop_rightBorderStyle, TOKEN_MASK },
  {PropName_border_top_style, NW_CSS_Prop_topBorderStyle, TOKEN_MASK },
  {PropName_border_bottom_style, NW_CSS_Prop_bottomBorderStyle, TOKEN_MASK },
  {PropName_border_color, NW_CSS_Prop_borderColor, COLOR_MASK | TOKEN_MASK },
  {PropName_border_left_color, NW_CSS_Prop_leftBorderColor, COLOR_MASK | TOKEN_MASK },
  {PropName_border_right_color, NW_CSS_Prop_rightBorderColor, COLOR_MASK | TOKEN_MASK },
  {PropName_border_top_color, NW_CSS_Prop_topBorderColor, COLOR_MASK | TOKEN_MASK },
  {PropName_border_bottom_color, NW_CSS_Prop_bottomBorderColor, COLOR_MASK | TOKEN_MASK },
  {PropName_float, NW_CSS_Prop_float, TOKEN_MASK},
  {PropName_font, NW_CSS_Prop_font, TOKEN_MASK | LENGTH_MASK | TEXT_MASK | PERCENTAGE_MASK},
  {PropName_font_family, NW_CSS_Prop_fontFamily, TEXT_MASK},
  {PropName_font_style, NW_CSS_Prop_fontStyle, TOKEN_MASK},
  {PropName_font_variant, NW_CSS_Prop_fontVariant, TOKEN_MASK},
  {PropName_font_size, NW_CSS_Prop_fontSize, LENGTH_MASK | PERCENTAGE_MASK},
  {PropName_font_weight, NW_CSS_Prop_fontWeight, INTEGER_MASK },
  {PropName_list_style, NW_CSS_Prop_listStyle, TOKEN_MASK | IMAGE_MASK },
  {PropName_list_style_type, NW_CSS_Prop_listStyleType, TOKEN_MASK },
  {PropName_list_style_image, NW_CSS_Prop_listStyleImage, TOKEN_MASK | IMAGE_MASK },
  {PropName_list_margin, NW_CSS_Prop_listMargin, LENGTH_MASK | PERCENTAGE_MASK | TOKEN_MASK },
  {PropName_noshade, NW_CSS_Prop_noshade, TOKEN_MASK },
  {PropName_padding, NW_CSS_Prop_padding, LENGTH_MASK | PERCENTAGE_MASK },
  {PropName_padding_left, NW_CSS_Prop_leftPadding, LENGTH_MASK | PERCENTAGE_MASK},
  {PropName_padding_right, NW_CSS_Prop_rightPadding, LENGTH_MASK | PERCENTAGE_MASK},
  {PropName_padding_top, NW_CSS_Prop_topPadding, LENGTH_MASK | PERCENTAGE_MASK},
  {PropName_padding_bottom, NW_CSS_Prop_bottomPadding, LENGTH_MASK | PERCENTAGE_MASK},
  {PropName_margin, NW_CSS_Prop_margin, LENGTH_MASK | PERCENTAGE_MASK | TOKEN_MASK },
  {PropName_margin_left, NW_CSS_Prop_leftMargin, LENGTH_MASK | PERCENTAGE_MASK | TOKEN_MASK },
  {PropName_margin_right, NW_CSS_Prop_rightMargin, LENGTH_MASK | PERCENTAGE_MASK | TOKEN_MASK },
  {PropName_margin_top, NW_CSS_Prop_topMargin, LENGTH_MASK | PERCENTAGE_MASK | TOKEN_MASK },
  {PropName_margin_bottom, NW_CSS_Prop_bottomMargin, LENGTH_MASK | PERCENTAGE_MASK},
  {PropName_page_break_after, NW_CSS_Prop_pageBreakAfter, TOKEN_MASK},
  {PropName_text_align, NW_CSS_Prop_textAlign, TOKEN_MASK},
  {PropName_text_decoration, NW_CSS_Prop_textDecoration, TOKEN_MASK},
  {PropName_text_indent, NW_CSS_Prop_textIndent, LENGTH_MASK | PERCENTAGE_MASK},
  {PropName_text_transform, NW_CSS_Prop_textTransform, TOKEN_MASK},
  {PropName_white_space, NW_CSS_Prop_whitespace, TOKEN_MASK},
  {PropName_vertical_align, NW_CSS_Prop_verticalAlign, TOKEN_MASK | LENGTH_MASK | PERCENTAGE_MASK},
  {PropName_height, NW_CSS_Prop_height, LENGTH_MASK | PERCENTAGE_MASK},
  {PropName_width, NW_CSS_Prop_width, LENGTH_MASK | PERCENTAGE_MASK},
  {PropName_color, NW_CSS_Prop_color, COLOR_MASK},
  {PropName_background, NW_CSS_Prop_background, COLOR_MASK | IMAGE_MASK},
  {PropName_background_color, NW_CSS_Prop_backgroundColor, TOKEN_MASK | COLOR_MASK},
  {PropName_background_image, NW_CSS_Prop_backgroundImage, TOKEN_MASK | IMAGE_MASK },
  {PropName_background_repeat, NW_CSS_Prop_backgroundRepeat, TOKEN_MASK },
  {PropName_background_attachment, NW_CSS_Prop_backgroundAttachment, TOKEN_MASK },
  {PropName_background_position, NW_CSS_Prop_backgroundPosition, TOKEN_MASK | NUMBER_MASK | LENGTH_MASK | PERCENTAGE_MASK},
  {PropName_background_position_x, NW_CSS_Prop_backgroundPosition_x, TOKEN_MASK | NUMBER_MASK | LENGTH_MASK | PERCENTAGE_MASK},
  {PropName_background_position_y, NW_CSS_Prop_backgroundPosition_y, TOKEN_MASK | NUMBER_MASK | LENGTH_MASK | PERCENTAGE_MASK},
  {PropName_clear, NW_CSS_Prop_clear, TOKEN_MASK},
  {PropName_visibility, NW_CSS_Prop_visibility, TOKEN_MASK},
  {PropName_list_style_position, NW_CSS_Prop_listStylePosition, TOKEN_MASK},
  {PropName_caption_side, NW_CSS_Prop_captionSide, TOKEN_MASK},
  {PropName_line_height, NW_CSS_Prop_lineHeight, TOKEN_MASK | NUMBER_MASK | LENGTH_MASK | PERCENTAGE_MASK},
  {PropName_overflow, NW_CSS_Prop_overflow, TOKEN_MASK},
  {PropName_accesskey, NW_CSS_Prop_accesskey, OBJECT_MASK},
  {PropName_marquee_style, NW_CSS_Prop_marqueeStyle, TOKEN_MASK},
  {PropName_marquee_loop, NW_CSS_Prop_marqueeLoop, INTEGER_MASK | TOKEN_MASK},
  {PropName_marquee_dir, NW_CSS_Prop_marqueeDir, TOKEN_MASK},
  {PropName_marquee_speed, NW_CSS_Prop_marqueeSpeed, TOKEN_MASK | INTEGER_MASK},
  {PropName_marquee_amount, NW_CSS_Prop_marqueeAmount, LENGTH_MASK | PERCENTAGE_MASK},
  {PropName_focus_behavior, NW_CSS_Prop_focusBehavior, TOKEN_MASK},
  {PropName_input_format, NW_CSS_Prop_inputFormat, TEXT_MASK},
  {PropName_input_required, NW_CSS_Prop_inputRequired, TOKEN_MASK},
  {PropName_imode_quotes, NW_CSS_Prop_imodeQuotes, TEXT_MASK},
  {PropName_table_align, NW_CSS_Prop_tableAlign, TOKEN_MASK},
  {PropName_text_dir, NW_CSS_Prop_textDirection, TOKEN_MASK}
};

/* ------------------------------------------------------------------------- */
static const TText16 PropVal_none[] = {'n','o','n','e','\0'};
static const TText16 PropVal_left[] = {'l','e','f','t','\0'};
static const TText16 PropVal_right[] = {'r','i','g','h','t','\0'};
static const TText16 PropVal_normal[] = {'n','o','r','m','a','l','\0'};

static const TText16 PropVal_always[] = {'a','l','w','a','y','s','\0'};
static const TText16 PropVal_avoid[] = {'a','v','o','i','d','\0'};
static const TText16 PropVal_center[] = {'c','e','n','t','e','r','\0'};
static const TText16 PropVal_justify[] = {'j','u','s','t','i','f','y','\0'};
static const TText16 PropVal_underline[] = {'u','n','d','e','r','l','i','n','e','\0'};
static const TText16 PropVal_blink[] = {'b','l','i','n','k','\0'};

static const TText16 PropVal_top[] = {'t','o','p','\0'};
static const TText16 PropVal_bottom[] = {'b','o','t','t','o','m','\0'};
static const TText16 PropVal_baseline[] = {'b','a','s','e','l','i','n','e','\0'};
static const TText16 PropVal_sub[] = {'s','u','b','\0'};
static const TText16 PropVal_super[] = {'s','u','p','e','r','\0'};
static const TText16 PropVal_middle[] = {'m','i','d','d','l','e','\0'};

/* list-style-type properties */
static const TText16 PropVal_disc[] = {'d','i','s','c','\0'};
static const TText16 PropVal_circle[] = {'c','i','r','c','l','e','\0'};
static const TText16 PropVal_square[] = {'s','q','u','a','r','e','\0'};
static const TText16 PropVal_decimal[] = {'d','e','c','i','m','a','l','\0'};
static const TText16 PropVal_lowerRoman[] = {'l','o','w','e','r','-','r','o','m','a','n','\0'};
static const TText16 PropVal_upperRoman[] = {'u','p','p','e','r','-','r','o','m','a','n','\0'};
static const TText16 PropVal_lowerAlpha[] = {'l','o','w','e','r','-','a','l','p','h','a','\0'};
static const TText16 PropVal_upperAlpha[] = {'u','p','p','e','r','-','a','l','p','h','a','\0'};

/* additional prop vals */
static const TText16 PropVal_inherit[] = {'i','n','h','e','r','i','t','\0'};
static const TText16 PropVal_auto[] =    {'a','u','t','o','\0'};
static const TText16 PropVal_both[] =    {'b','o','t','h','\0'};

/* visibility prop vals */
static const TText16 PropVal_visible[] = {'v','i','s','i','b','l','e','\0'};
static const TText16 PropVal_hidden[] =  {'h','i','d','d','e','n','\0'};
static const TText16 PropVal_collapse[] = {'c','o','l','l','a','p','s','e','\0'};

/* white-space prop vals */
static const TText16 PropVal_pre[] =  {'p','r','e','\0'};
static const TText16 PropVal_nowrap[] = {'n','o','w','r','a','p','\0'};

/* text-transform prop vals */
static const TText16 PropVal_capitalize[] = {'c','a','p','i','t','a','l','i','z','e','\0'};
static const TText16 PropVal_uppercase[] =  {'u','p','p','e','r','c','a','s','e','\0'};
static const TText16 PropVal_lowercase[] = {'l','o','w','e','r','c','a','s','e','\0'};

/* border-style vals */
static const TText16 PropVal_double[] = {'d','o','u','b','l','e','\0'};
static const TText16 PropVal_solid[] = {'s','o','l','i','d','\0'};
static const TText16 PropVal_dashed[] =  {'d','a','s','h','e','d','\0'};
static const TText16 PropVal_dotted[] =  {'d','o','t','t','e','d','\0'};
static const TText16 PropVal_ridge[] = {'r','i','d','g','e','\0'};
static const TText16 PropVal_groove[] = {'g','r','o','o','v','e','\0'};
static const TText16 PropVal_inset[] = {'i','n','s','e','t','\0'};
static const TText16 PropVal_outset[] = {'o','u','t','s','e','t','\0'};

/* border-width vals */
static const TText16 PropVal_thin[] = {'t','h','i','n','\0'};
static const TText16 PropVal_thick[] = {'t','h','i','c','k','\0'};

/* list position values */
static const TText16 PropVal_inside[] = {'i','n','s','i','d','e','\0'};
static const TText16 PropVal_outside[] = {'o','u','t','s','i','d','e','\0'};

/* background-color vals */
static const TText16 PropVal_transparent[] = {'t','r','a','n','s','p','a','r','e','n','t','\0'};

/* font-size prop vals */
const TText16 PropVal_xxSmall[] = {'x','x','-','s','m','a','l','l','\0'};
const TText16 PropVal_xSmall[] =  {'x','-','s','m','a','l','l','\0'};
const TText16 PropVal_small[] = {'s','m','a','l','l','\0'};
const TText16 PropVal_xxLarge[] = {'x','x','-','l','a','r','g','e','\0'};
const TText16 PropVal_xLarge[] = {'x','-','l','a','r','g','e','\0'};
const TText16 PropVal_large[] = {'l','a','r','g','e','\0'};
const TText16 PropVal_smaller[] = {'s','m','a','l','l','e','r','\0'};
const TText16 PropVal_larger[] = {'l','a','r','g','e','r','\0'};
const TText16 PropVal_medium[] = {'m','e','d','i','u','m','\0'};

/* font-style prop vals */
static const TText16 PropVal_italic[] = {'i','t','a','l','i','c','\0'};
static const TText16 PropVal_oblique[] = {'o','b','l','i','q','u','e','\0'};

/* font-weight prop vals */
static const TText16 PropVal_bold[] = {'b','o','l','d','\0'};
static const TText16 PropVal_bolder[] =  {'b','o','l','d','e','r','\0'};
static const TText16 PropVal_lighter[] = {'l','i','g','h','t','e','r','\0'};

/* font-variant prop vals */
static const TText16 PropVal_smallcaps[] = {'s','m','a','l','l','-','c','a','p','s','\0'};

/* color vals */
static const TText16 PropVal_aqua[] = {'a','q','u','a','\0'};
static const TText16 PropVal_black[] = {'b','l','a','c','k','\0'};
static const TText16 PropVal_blue[] = {'b','l','u','e','\0'};
static const TText16 PropVal_cyan[] = {'c','y','a','n','\0'};
static const TText16 PropVal_fuchsia[] = {'f','u','c','h','s','i','a','\0'};
static const TText16 PropVal_gray[] = {'g','r','a','y','\0'};
static const TText16 PropVal_green[] = {'g','r','e','e','n','\0'};
static const TText16 PropVal_lime[] = {'l','i','m','e','\0'};
static const TText16 PropVal_maroon[] = {'m','a','r','o','o','n','\0'};
static const TText16 PropVal_navy[] = {'n','a','v','y','\0'};
static const TText16 PropVal_olive[] = {'o','l','i','v','e','\0'};
static const TText16 PropVal_pink[] = {'p','i','n','k','\0'};
static const TText16 PropVal_purple[] = {'p','u','r','p','l','e','\0'};
static const TText16 PropVal_red[] = {'r','e','d','\0'};
static const TText16 PropVal_silver[] = {'s','i','l','v','e','r','\0'};
static const TText16 PropVal_teal[] = {'t','e','a','l','\0'};
static const TText16 PropVal_white[] = {'w','h','i','t','e','\0'};
static const TText16 PropVal_yellow[] = {'y','e','l','l','o','w','\0'};

/* marquee vals */
static const TText16 PropVal_scroll[] = {'s','c','r','o','l','l','\0'};
static const TText16 PropVal_slide[] = {'s','l','i','d','e','\0'};
static const TText16 PropVal_alternate[] = {'a','l','t','e','r','n','a','t','e','\0'};
static const TText16 PropVal_infinite[] = {'i','n','f','i','n','i','t','e','\0'};
static const TText16 PropVal_ltr[] = {'l','t','r','\0'};
static const TText16 PropVal_rtl[] = {'r','t','l','\0'};
static const TText16 PropVal_slow[] = {'s','l','o','w','\0'};
static const TText16 PropVal_fast[] = {'f','a','s','t','\0'};

/* display vals */
static const TText16 PropVal_inline[] = {'i','n','l','i','n','e','\0'};
static const TText16 PropVal_block[] = {'b','l','o','c','k','\0'};
static const TText16 PropVal_listItem[] = {'l','i','s','t','-','i','t','e','m','\0'};
static const TText16 PropVal_marquee[] = {'-','w','a','p','-','m','a','r','q','u','e','e','\0'};

/* Boolean values */
static const TText16 PropVal_true[] = {'t','r','u','e','\0'};
static const TText16 PropVal_false[] = {'f','a','l','s','e','\0'};

/* background-repeat values */
static const TText16 PropVal_background_repeat[] = {'r','e','p','e','a','t','\0'};
static const TText16 PropVal_background_repeat_x[] = {'r','e','p','e','a','t','-','x','\0'};
static const TText16 PropVal_background_repeat_y[] = {'r','e','p','e','a','t','-','y','\0'};
static const TText16 PropVal_background_norepeat[] = {'n','o','-','r','e','p','e','a','t','\0'};

/* background-attachment values */
static const TText16 PropVal_background_fixed[] = {'f','i','x','e','d','\0'};

/* ------------------------------------------------------------------------- */
static const
TCSSPropertyTablePropValEntry TCSSPropertyTablePropValEntries[] =
{
  {PropVal_none, NW_CSS_PropValue_none},
  {PropVal_left, NW_CSS_PropValue_left},
  {PropVal_right, NW_CSS_PropValue_right},
  {PropVal_normal, NW_CSS_PropValue_normal},
  {PropVal_disc, NW_CSS_PropValue_disc},
  {PropVal_circle, NW_CSS_PropValue_circle},
  {PropVal_square, NW_CSS_PropValue_square},
  {PropVal_decimal, NW_CSS_PropValue_decimal},
  {PropVal_lowerRoman, NW_CSS_PropValue_lowerRoman},
  {PropVal_upperRoman, NW_CSS_PropValue_upperRoman},
  {PropVal_lowerAlpha, NW_CSS_PropValue_lowerAlpha},
  {PropVal_upperAlpha, NW_CSS_PropValue_upperAlpha},
  {PropVal_always, NW_CSS_PropValue_always},
  {PropVal_avoid, NW_CSS_PropValue_avoid},
  {PropVal_center, NW_CSS_PropValue_center},
  {PropVal_justify, NW_CSS_PropValue_justify},
  {PropVal_underline, NW_CSS_PropValue_underline},
  {PropVal_blink, NW_CSS_PropValue_blink},
  {PropVal_top, NW_CSS_PropValue_top},
  {PropVal_bottom, NW_CSS_PropValue_bottom},
  {PropVal_sub, NW_CSS_PropValue_sub},
  {PropVal_super, NW_CSS_PropValue_super},
  {PropVal_baseline, NW_CSS_PropValue_baseline},
  {PropVal_middle, NW_CSS_PropValue_middle},
  {PropVal_inherit, NW_CSS_PropValue_inherit},
  {PropVal_auto, NW_CSS_PropValue_auto},
  {PropVal_both, NW_CSS_PropValue_both},
  {PropVal_visible, NW_CSS_PropValue_visible},
  {PropVal_hidden, NW_CSS_PropValue_hidden},
  {PropVal_collapse, NW_CSS_PropValue_collapse},
  {PropVal_pre, NW_CSS_PropValue_pre},
  {PropVal_nowrap, NW_CSS_PropValue_nowrap},
  {PropVal_capitalize, NW_CSS_PropValue_capitalize},
  {PropVal_uppercase, NW_CSS_PropValue_uppercase},
  {PropVal_lowercase, NW_CSS_PropValue_lowercase},
  {PropVal_solid, NW_CSS_PropValue_solid},
  {PropVal_double, NW_CSS_PropValue_solid},  /* Not supported */
  {PropVal_dashed, NW_CSS_PropValue_dashed},  
  {PropVal_dotted, NW_CSS_PropValue_dotted},  
  {PropVal_ridge, NW_CSS_PropValue_solid},   /* Not supported */
  {PropVal_groove, NW_CSS_PropValue_solid},  /* Not supported */
  {PropVal_inset, NW_CSS_PropValue_solid},   /* Not supported */
  {PropVal_outset, NW_CSS_PropValue_solid},  /* Not supported */
  {PropVal_xxSmall, NW_CSS_PropValue_xxSmall},
  {PropVal_xSmall, NW_CSS_PropValue_xSmall},
  {PropVal_small, NW_CSS_PropValue_small},
  {PropVal_xxLarge, NW_CSS_PropValue_xxLarge},
  {PropVal_xLarge, NW_CSS_PropValue_xLarge},
  {PropVal_large, NW_CSS_PropValue_large},
  {PropVal_smaller, NW_CSS_PropValue_smaller},
  {PropVal_larger, NW_CSS_PropValue_larger},
  {PropVal_medium, NW_CSS_PropValue_medium},
  {PropVal_thin, NW_CSS_PropValue_thin},
  {PropVal_thick, NW_CSS_PropValue_thick},
  {PropVal_bold, NW_CSS_PropValue_bold},
  {PropVal_bolder, NW_CSS_PropValue_bolder},
  {PropVal_lighter, NW_CSS_PropValue_lighter},
  {PropVal_italic, NW_CSS_PropValue_italic},
  {PropVal_oblique, NW_CSS_PropValue_oblique},
  {PropVal_smallcaps, NW_CSS_PropValue_smallcaps},
  {PropVal_inside, NW_CSS_PropValue_inside},
  {PropVal_outside, NW_CSS_PropValue_outside},
  {PropVal_transparent, NW_CSS_PropValue_transparent},
  {PropVal_aqua, NW_CSS_PropValue_aqua},
  {PropVal_black, NW_CSS_PropValue_black},
  {PropVal_blue, NW_CSS_PropValue_blue},
  {PropVal_cyan, NW_CSS_PropValue_cyan},
  {PropVal_fuchsia, NW_CSS_PropValue_fuchsia},
  {PropVal_gray, NW_CSS_PropValue_gray},
  {PropVal_green, NW_CSS_PropValue_green},
  {PropVal_lime, NW_CSS_PropValue_lime},
  {PropVal_maroon, NW_CSS_PropValue_maroon},
  {PropVal_navy, NW_CSS_PropValue_navy},
  {PropVal_olive, NW_CSS_PropValue_olive},
  {PropVal_pink, NW_CSS_PropValue_pink},
  {PropVal_purple, NW_CSS_PropValue_purple},
  {PropVal_red, NW_CSS_PropValue_red},
  {PropVal_silver, NW_CSS_PropValue_silver},
  {PropVal_teal, NW_CSS_PropValue_teal},
  {PropVal_white, NW_CSS_PropValue_white},
  {PropVal_yellow, NW_CSS_PropValue_yellow},
  {PropVal_scroll, NW_CSS_PropValue_scroll},
  {PropVal_slide, NW_CSS_PropValue_slide},
  {PropVal_alternate, NW_CSS_PropValue_alternate},
  {PropVal_infinite, NW_CSS_PropValue_infinite},
  {PropVal_ltr, NW_CSS_PropValue_ltr},
  {PropVal_rtl, NW_CSS_PropValue_rtl},
  {PropVal_slow, NW_CSS_PropValue_slow},
  {PropVal_fast, NW_CSS_PropValue_fast},
  {PropVal_inline, NW_CSS_PropValue_display_inline},
  {PropVal_block, NW_CSS_PropValue_display_block},
  {PropVal_listItem, NW_CSS_PropValue_display_listItem},
  {PropVal_marquee, NW_CSS_PropValue_display_marquee},
  {PropVal_background_repeat, NW_CSS_PropValue_background_repeat},
  {PropVal_background_repeat_x, NW_CSS_PropValue_background_repeat_x},
  {PropVal_background_repeat_y, NW_CSS_PropValue_background_repeat_y},
  {PropVal_background_norepeat, NW_CSS_PropValue_background_norepeat},
  {PropVal_background_fixed,  NW_CSS_PropValue_background_fixed },
  {PropVal_true, NW_CSS_PropValue_true},
  {PropVal_false, NW_CSS_PropValue_false}
};

/* ------------------------------------------------------------------------- */
/*
These values are used to create value type strings for debug purposes. If it is
obvious from the context( e.g. color) a null string will b used
*/
static const TText16 ValueType_Integer[] = {'\0'};
static const TText16 ValueType_Number[] = {'(','n','u','m','b','e','r',')','\0'};
static const TText16 ValueType_Px[] = {'p','x','\0'};
static const TText16 ValueType_Em[] = {'e','m','\0'};
static const TText16 ValueType_Ex[] = {'e','x','\0'};
static const TText16 ValueType_In[] = {'i','n','\0'};
static const TText16 ValueType_Cm[] = {'c','m','\0'};
static const TText16 ValueType_Mm[] = {'m','m','\0'};
static const TText16 ValueType_Pt[] = {'p','t','\0'};
static const TText16 ValueType_Pc[] = {'p','c','\0'};
static const TText16 ValueType_Percentage[] = {'%','\0'};
static const TText16 ValueType_Token[] = {'\0'};
static const TText16 ValueType_Color[] = {'\0'};
static const TText16 ValueType_Object[] = {'(','o','b','j','e','c','t',')','\0'};
static const TText16 ValueType_Text[] = {'\0'};
static const TText16 ValueType_Image[] = {'(','i','m','a','g','e',')','\0'};
static const TText16 ValueType_Font[] = {'(','f','o','n','t',')','\0'};
static const TText16 ValueType_Copy[] = {'(','c','o','p','y',')','\0'};
static const TText16 ValueType_DefaultStyle[] = {'(','d','e','f','a','u','l','t','S','t','y','l','e',')','\0'};
static const TText16 ValueType_Mask[] = {'(','m','a','s','k',')','\0'};

// MODULE DATA STRUCTURES

// FORWARD DECLARATIONS

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

const TCSSPropertyTablePropEntry*
TCSSPropertyTable::GetPropertyEntry(const TText16* aStrName)
{
  TUint32 numPropertyNames = sizeof(TCSSPropertyTablePropEntries)/sizeof(TCSSPropertyTablePropEntry);
  const TCSSPropertyTablePropEntry* propertyNames = TCSSPropertyTablePropEntries;

  TUint32 length = (NW_Str_Strlen(aStrName)+1)*2;

  /* traverse our list of token entries looking for a match for our token */
  for (TUint32 index = 0; index < numPropertyNames; index++)
  {
    if(!NW_Str_Strnicmp(aStrName, propertyNames[index].strName, length))
    {
      return &(propertyNames[index]);
    }
  }
  /* if no token entry is found we must return 0 */
  return NULL;
}

/* ------------------------------------------------------------------------- */
TBool
TCSSPropertyTable::GetPropertyValToken(const TText16* aVal,
                                       NW_LMgr_PropertyValueToken_t *aToken)
{
  TUint32 numPropertyVals = sizeof(TCSSPropertyTablePropValEntries)/sizeof(TCSSPropertyTablePropValEntry);
  const TCSSPropertyTablePropValEntry* propertyVals = TCSSPropertyTablePropValEntries;

  TUint32 length = (NW_Str_Strlen(aVal)+1)*2;

  /* traverse our list of token entries looking for a match for our token */
  for (TUint32 index = 0; index < numPropertyVals; index++)
  {
    if(!NW_Str_Strnicmp(aVal, propertyVals[index].strVal, length))
    {
      *aToken = propertyVals[index].tokenVal;
      return ETrue;
    }
  }
  /* if no token entry is found we must return NOT_FOUND */
  return EFalse;
}

/* ------------------------------------------------------------------------- */
const TCSSPropertyTablePropEntry*
TCSSPropertyTable::GetPropEntryByToken(NW_LMgr_PropertyName_t aTokenName)
{
  TUint32 numPropertyNames = sizeof(TCSSPropertyTablePropEntries)/sizeof(TCSSPropertyTablePropEntry);
  const TCSSPropertyTablePropEntry* propertyNames = TCSSPropertyTablePropEntries;

  /* traverse our list of token entries looking for a match for our token */
  for (TUint32 index = 0; index < numPropertyNames; index++)
  {
    if (propertyNames[index].tokenName == aTokenName)
    {
      return &(propertyNames[index]);
    }
  }
  /* if no token entry is found we must return 0 */
  return NULL;
}

/* ------------------------------------------------------------------------- */
const TCSSPropertyTablePropValEntry*
TCSSPropertyTable::GetPropValEntryByToken(NW_LMgr_PropertyValueToken_t aTokenVal)
{
  TUint32 numValueNames = sizeof(TCSSPropertyTablePropValEntries)/sizeof(TCSSPropertyTablePropValEntry);
  const TCSSPropertyTablePropValEntry* valueNames = TCSSPropertyTablePropValEntries;

  /* traverse our list of token entries looking for a match for our token */
  for (TUint32 index = 0; index < numValueNames; index++)
  {
    if (valueNames[index].tokenVal == aTokenVal)
    {
      return &(valueNames[index]);
    }
  }
  /* if no token entry is found we must return 0 */
  return NULL;
}


/* ------------------------------------------------------------------------- */
const TText16*
TCSSPropertyTable::GetPropValType(TUint8 aValueType)
{
  switch (aValueType){

  case NW_CSS_ValueType_Integer:    // integer
    return ValueType_Integer;
  case NW_CSS_ValueType_Number:     // decimal
    return ValueType_Number;
  case NW_CSS_ValueType_Px:         // integer
    return ValueType_Px;
  case NW_CSS_ValueType_Em:         // decimal
    return ValueType_Em;
  case NW_CSS_ValueType_Ex:         // decimal
    return ValueType_Ex;
  case NW_CSS_ValueType_In:         // decimal
    return ValueType_In;
  case NW_CSS_ValueType_Cm:         // decimal
    return ValueType_Cm;
  case NW_CSS_ValueType_Mm:         // decimal
    return ValueType_Mm;
  case NW_CSS_ValueType_Pt:         // decimal
    return ValueType_Pt;
  case NW_CSS_ValueType_Pc:         // decimal
    return ValueType_Pc;
  case NW_CSS_ValueType_Percentage: // decimal
    return ValueType_Percentage;
  case NW_CSS_ValueType_Token:      // integer
    return ValueType_Token;
  case NW_CSS_ValueType_Color:      // integer
    return ValueType_Color;
  case NW_CSS_ValueType_Object:     // object
    return ValueType_Object;
  case NW_CSS_ValueType_Text:       // object
    return ValueType_Text;
  case NW_CSS_ValueType_Image:      // object
    return ValueType_Image;
  case NW_CSS_ValueType_Font:       // object
    return ValueType_Font;
  case NW_CSS_ValueType_Copy:       // N/A
    return ValueType_Copy;
  case NW_CSS_ValueType_DefaultStyle://N/A
    return ValueType_DefaultStyle;
  case NW_CSS_ValueType_Mask:       // N/A
    return ValueType_Mask;
  default:                          // invalid entry
    return NULL;
  }
}




#undef PX_MASK
#undef EM_MASK
#undef EX_MASK
#undef IN_MASK
#undef CM_MASK
#undef MM_MASK
#undef PT_MASK
#undef PC_MASK
#undef LENGTH_MASK
#undef PERCENTAGE_MASK
#undef COLOR_MASK
#undef TOKEN_MASK
#undef TEXT_MASK
#undef IMAGE_MASK

