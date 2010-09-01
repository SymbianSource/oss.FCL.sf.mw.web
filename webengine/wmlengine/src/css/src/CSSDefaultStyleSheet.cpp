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

#include <e32def.h>  // First to avoid NULL redefine warning (no #ifndef NULL).

#include "nw_css_defaultstylesheeti.h"

#include "nw_text_ucs2.h"
#include "nw_lmgr_cssproperties.h"
#include <nw_string_char.h>
#include "nw_lmgr_rootbox.h"
#include "CSSPropVal.h"
#include "nwx_string.h"
#include "CSSPropertyTable.h"
#include "CSSDeclListHandle.h"
#include "CSSPatternMatcher.h"
#include "BrsrStatusCodes.h"
#include "nw_xhtml_xhtml_1_0_tokens.h"

/* ------------------------------------------------------------------------- *
   MACRO - These make the tables more readable and easier to edit
   * ------------------------------------------------------------------------- */
#define _ELEM(tok) NW_XHTML_ElementToken_##tok
#define _PROP(tok) NW_CSS_Prop_##tok
#define _VAL(tok)  NW_CSS_PropValue_##tok
#define _TYPE(tok) NW_CSS_ValueType_##tok

/* ------------------------------------------------------------------------- *
   private types
   * ------------------------------------------------------------------------- */
static const NW_Ucs2 monospace[] = {'m', 'o', 'n', 'o', 's', 'p', 'a', 'c', 'e', '\0'};
static const NW_Ucs2 quotes_level1[] = { '\"', '\"', '\0' };
static const NW_Ucs2 quotes_level2[] = { '\'', '\'', '\0' };

/* ------------------------------------------------------------------------- */
static const
NW_Ucs2* const _NW_CSS_DefaultStyleSheet_StringVals[] = {
  monospace,
  quotes_level1,
  quotes_level2
};

/* ------------------------------------------------------------------------- */
/* tag ids are defined in nw_xhtml_xhtml_1_0_tokens.h */
/* property names are defined in nw_lmgr_cssproperties.h */
/* property values are defined in nw_lmgr_cssproperties.h */


static const
NW_CSS_DefaultStyleSheet_PropEntry_t _NW_CSS_DefaultStyleSheet_PropVals[] = {
  {_ELEM(a),        _PROP(color),            0x800080,              _TYPE(Color),  (NW_Byte*)"a:visited{}" },
/* default color for a elements applied in aElementHandler
   addActiveTextDecoration so non-link or invalid href anchors are not
   colored. */
{_ELEM(acronym),   _PROP(textTransform),    _VAL(uppercase),       _TYPE(Token),   NULL },
{_ELEM(address),   _PROP(display),          _VAL(display_block),   _TYPE(Token),   NULL },
{_ELEM(address),   _PROP(leftMargin),       10,                    _TYPE(Percentage), NULL },
{_ELEM(address),   _PROP(topMargin),        8,                     _TYPE(Px),      NULL },
{_ELEM(address),   _PROP(bottomMargin),     8,                     _TYPE(Px),      NULL },
{_ELEM(address),   _PROP(fontStyle),        _VAL(italic),          _TYPE(Token),   NULL },
{_ELEM(area),      _PROP(color),            0x800080,              _TYPE(Color),   (NW_Byte*)"area:visited{}" },
{_ELEM(b),         _PROP(fontWeight),       900,                   _TYPE(Integer), NULL },
{_ELEM(big),       _PROP(fontSize),         12,                    _TYPE(Pt),      NULL },
{_ELEM(blink),     _PROP(textDecoration),   _VAL(blink),           _TYPE(Token),   NULL },
{_ELEM(blockquote),_PROP(display),          _VAL(display_block),   _TYPE(Token),   NULL },
{_ELEM(blockquote),_PROP(topMargin),        2,                     _TYPE(Px),      NULL },
{_ELEM(blockquote),_PROP(bottomMargin),     2,                     _TYPE(Px),      NULL },
{_ELEM(blockquote),_PROP(leftMargin),       17,                    _TYPE(Px),      NULL },
{_ELEM(body),      _PROP(display),          _VAL(display_block),   _TYPE(Token),   NULL },
{_ELEM(body),      _PROP(fontWeight),       400,                   _TYPE(Integer), NULL },
{_ELEM(body),      _PROP(topMargin),        2,                     _TYPE(Px),      NULL },
{_ELEM(body),      _PROP(leftMargin),       2,                     _TYPE(Px),      NULL },
{_ELEM(body),      _PROP(padding),          2,                     _TYPE(Px),      NULL },
{_ELEM(body),      _PROP(fontSize),         7,                     _TYPE(Pt),      NULL },
{_ELEM(button),    _PROP(margin),           1,                     _TYPE(Px),      NULL },
{_ELEM(button),    _PROP(borderStyle),      _VAL(solid),           _TYPE(Token),   NULL },
{_ELEM(button),    _PROP(borderWidth),      1,                     _TYPE(Px),      NULL },
{_ELEM(button),    _PROP(backgroundColor),  0xcccccc,              _TYPE(Color),   NULL },
{_ELEM(caption),   _PROP(display),          _VAL(display_block),   _TYPE(Token),   NULL },
{_ELEM(caption),   _PROP(textAlign),        _VAL(center),          _TYPE(Token),   NULL },
{_ELEM(caption),   _PROP(fontStyle),        _VAL(italic),          _TYPE(Token),   NULL },
{_ELEM(center),    _PROP(display),          _VAL(display_block),   _TYPE(Token),   NULL },
{_ELEM(center),    _PROP(textAlign),        _VAL(center),          _TYPE(Token),   NULL },
{_ELEM(cite),      _PROP(fontStyle),        _VAL(italic),          _TYPE(Token),   NULL },
{_ELEM(code),      _PROP(fontFamily),       0,                     _TYPE(Text),    NULL },
{_ELEM(dd),        _PROP(display),          _VAL(display_block),   _TYPE(Token),   NULL },
{_ELEM(dd),        _PROP(leftMargin),       17,                    _TYPE(Px),      NULL },
{_ELEM(del),       _PROP(textDecoration),   _VAL(strikeout),       _TYPE(Token),   NULL },
{_ELEM(dfn),       _PROP(fontStyle),        _VAL(italic),          _TYPE(Token),   NULL },
{_ELEM(dir),       _PROP(display),          _VAL(display_block),   _TYPE(Token),   NULL },
{_ELEM(dir),       _PROP(listStyleType),    _VAL(square),          _TYPE(Token),   (NW_Byte*)"dir dir dir{}" },
{_ELEM(dir),       _PROP(listStyleType),    _VAL(circle),          _TYPE(Token),   (NW_Byte*)"dir dir{}" },
{_ELEM(dir),       _PROP(listStyleType),    _VAL(disc),            _TYPE(Token),   NULL },
{_ELEM(dir),       _PROP(leftMargin),       17,                    _TYPE(Px),      NULL },
{_ELEM(dir),       _PROP(topMargin),        10,                    _TYPE(Px),      NULL },
{_ELEM(dir),       _PROP(bottomMargin),     10,                    _TYPE(Px),      NULL },
{_ELEM(div),       _PROP(display),          _VAL(display_block),   _TYPE(Token),   NULL },
{_ELEM(dl),        _PROP(display),          _VAL(display_block),   _TYPE(Token),   NULL },
{_ELEM(dl),        _PROP(topMargin),        10,                    _TYPE(Px),      NULL },
{_ELEM(dl),        _PROP(bottomMargin),     10,                    _TYPE(Px),      NULL },
{_ELEM(dt),        _PROP(display),          _VAL(display_block),   _TYPE(Token),   NULL },
{_ELEM(em),        _PROP(fontStyle),        _VAL(italic),          _TYPE(Token),   NULL },
{_ELEM(fieldset),  _PROP(topMargin),        5,                     _TYPE(Px),      NULL },
{_ELEM(fieldset),  _PROP(bottomMargin),     5,                     _TYPE(Px),      NULL },
{_ELEM(fieldset),  _PROP(leftMargin),       3,                     _TYPE(Px),      NULL },
{_ELEM(fieldset),  _PROP(rightMargin),      3,                     _TYPE(Px),      NULL },
{_ELEM(fieldset),  _PROP(padding),          3,                     _TYPE(Px),      NULL },
{_ELEM(fieldset),  _PROP(display),          _VAL(display_block),   _TYPE(Token),   NULL },
{_ELEM(fieldset),  _PROP(borderStyle),      _VAL(solid),           _TYPE(Token),   NULL },
{_ELEM(fieldset),  _PROP(borderWidth),      1,                     _TYPE(Px),      NULL },
{_ELEM(fieldset),  _PROP(borderColor),      0x000000,              _TYPE(Color),   NULL },
{_ELEM(form),      _PROP(display),          _VAL(display_block),   _TYPE(Token),   NULL },
{_ELEM(frameset),  _PROP(display),          _VAL(display_block),   _TYPE(Token),   NULL },
{_ELEM(frameset),  _PROP(fontWeight),       400,                   _TYPE(Integer), NULL },
{_ELEM(frameset),  _PROP(topMargin),        2,                     _TYPE(Px),      NULL },
{_ELEM(frameset),  _PROP(leftMargin),       2,                     _TYPE(Px),      NULL },
{_ELEM(frameset),  _PROP(padding),          2,                     _TYPE(Px),      NULL },
{_ELEM(frameset),  _PROP(fontSize),         7,                     _TYPE(Pt),      NULL },
{_ELEM(h1),        _PROP(display),          _VAL(display_block),   _TYPE(Token),   NULL },
{_ELEM(h1),        _PROP(fontSize),         14,                    _TYPE(Pt),      NULL },
{_ELEM(h1),        _PROP(fontWeight),       900,                   _TYPE(Integer), NULL },
{_ELEM(h1),        _PROP(bottomMargin),     17,                    _TYPE(Px),      NULL },
{_ELEM(h1),        _PROP(topMargin),        17,                    _TYPE(Px),      NULL },
{_ELEM(h2),        _PROP(display),          _VAL(display_block),   _TYPE(Token),   NULL },
{_ELEM(h2),        _PROP(fontSize),         13,                    _TYPE(Pt),      NULL },
{_ELEM(h2),        _PROP(fontWeight),       400,                   _TYPE(Integer), NULL },
{_ELEM(h2),        _PROP(bottomMargin),     15,                    _TYPE(Px),      NULL },
{_ELEM(h2),        _PROP(topMargin),        15,                    _TYPE(Px),      NULL },
{_ELEM(h3),        _PROP(display),          _VAL(display_block),   _TYPE(Token),   NULL },
{_ELEM(h3),        _PROP(fontSize),         12,                    _TYPE(Pt),      NULL },
{_ELEM(h3),        _PROP(fontWeight),       900,                   _TYPE(Integer), NULL },
{_ELEM(h3),        _PROP(bottomMargin),     13,                    _TYPE(Px),      NULL },
{_ELEM(h3),        _PROP(topMargin),        13,                    _TYPE(Px),      NULL },
{_ELEM(h4),        _PROP(display),          _VAL(display_block),   _TYPE(Token),   NULL },
{_ELEM(h4),        _PROP(fontSize),         11,                    _TYPE(Pt),      NULL },
{_ELEM(h4),        _PROP(fontWeight),       400,                   _TYPE(Integer), NULL },
{_ELEM(h4),        _PROP(bottomMargin),     13,                    _TYPE(Px),      NULL },
{_ELEM(h4),        _PROP(topMargin),        13,                    _TYPE(Px),      NULL },
{_ELEM(h5),        _PROP(display),          _VAL(display_block),   _TYPE(Token),   NULL },
{_ELEM(h5),        _PROP(fontSize),         10,                    _TYPE(Pt),      NULL },
{_ELEM(h5),        _PROP(fontWeight),       900,                   _TYPE(Integer), NULL },
{_ELEM(h5),        _PROP(bottomMargin),     12,                    _TYPE(Px),      NULL },
{_ELEM(h5),        _PROP(topMargin),        12,                    _TYPE(Px),      NULL },
{_ELEM(h6),        _PROP(display),          _VAL(display_block),   _TYPE(Token),   NULL },
{_ELEM(h6),        _PROP(fontSize),         9,                     _TYPE(Pt),      NULL },
{_ELEM(h6),        _PROP(fontWeight),       400,                   _TYPE(Integer), NULL },
{_ELEM(h6),        _PROP(bottomMargin),     11,                    _TYPE(Px),      NULL },
{_ELEM(h6),        _PROP(topMargin),        11,                    _TYPE(Px),      NULL },
{_ELEM(head),      _PROP(display),          _VAL(none),            _TYPE(Token),   NULL },
{_ELEM(hr),        _PROP(display),          _VAL(display_block),   _TYPE(Token),   NULL },
{_ELEM(hr),        _PROP(height),           2,                     _TYPE(Px),      NULL },
{_ELEM(hr),        _PROP(textAlign),        _VAL(center),          _TYPE(Token),   NULL },
{_ELEM(hr),        _PROP(topMargin),        4,                     _TYPE(Px),      NULL },
{_ELEM(hr),        _PROP(bottomMargin),     4,                     _TYPE(Px),      NULL },
{_ELEM(html),      _PROP(display),          _VAL(display_block),   _TYPE(Token),   NULL },
{_ELEM(i),         _PROP(fontStyle),        _VAL(italic),          _TYPE(Token),   NULL },

{_ELEM(input),     _PROP(backgroundColor),  0xefefef,              _TYPE(Color),   (NW_Byte*)"input[type=submit]{}" },
{_ELEM(input),     _PROP(borderColor),      0x949694,              _TYPE(Color),   (NW_Byte*)"input[type=submit]{}" },
{_ELEM(input),     _PROP(borderStyle),      _VAL(solid),           _TYPE(Token),   (NW_Byte*)"input[type=submit]{}" },
{_ELEM(input),     _PROP(borderWidth),      1,                     _TYPE(Px),      (NW_Byte*)"input[type=submit]{}" },
{_ELEM(input),     _PROP(focusBehavior),    0,                     _TYPE(Token),   (NW_Byte*)"input[type=submit]{}" },
{_ELEM(input),     _PROP(margin),           2,                     _TYPE(Px),      (NW_Byte*)"input[type=submit]{}" },
{_ELEM(input),     _PROP(padding),          2,                     _TYPE(Px),      (NW_Byte*)"input[type=submit]{}" },

{_ELEM(input),     _PROP(backgroundColor),  0xefefef,              _TYPE(Color),   (NW_Byte*)"input[type=reset]{}" },
{_ELEM(input),     _PROP(borderColor),      0x949694,              _TYPE(Color),   (NW_Byte*)"input[type=reset]{}" },
{_ELEM(input),     _PROP(borderStyle),      _VAL(solid),           _TYPE(Token),   (NW_Byte*)"input[type=reset]{}" },
{_ELEM(input),     _PROP(borderWidth),      1,                     _TYPE(Px),      (NW_Byte*)"input[type=reset]{}" },
{_ELEM(input),     _PROP(focusBehavior),    0,                     _TYPE(Token),   (NW_Byte*)"input[type=reset]{}" },
{_ELEM(input),     _PROP(margin),           2,                     _TYPE(Px),      (NW_Byte*)"input[type=reset]{}" },
{_ELEM(input),     _PROP(padding),          2,                     _TYPE(Px),      (NW_Byte*)"input[type=reset]{}" },

{_ELEM(input),     _PROP(backgroundColor),  0xefefef,              _TYPE(Color),   (NW_Byte*)"input[type=button]{}" },
{_ELEM(input),     _PROP(borderColor),      0x949694,              _TYPE(Color),   (NW_Byte*)"input[type=button]{}" },
{_ELEM(input),     _PROP(borderStyle),      _VAL(solid),           _TYPE(Token),   (NW_Byte*)"input[type=button]{}" },
{_ELEM(input),     _PROP(borderWidth),      1,                     _TYPE(Px),      (NW_Byte*)"input[type=button]{}" },
{_ELEM(input),     _PROP(focusBehavior),    0,                     _TYPE(Token),   (NW_Byte*)"input[type=button]{}" },
{_ELEM(input),     _PROP(margin),           1,                     _TYPE(Px),      (NW_Byte*)"input[type=button]{}" },
{_ELEM(input),     _PROP(padding),          1,                     _TYPE(Px),      (NW_Byte*)"input[type=button]{}" },

{_ELEM(input),     _PROP(backgroundColor),  0xffffff,              _TYPE(Color),   (NW_Byte*)"input[type=password]{}" },
{_ELEM(input),     _PROP(borderColor),      0x000000,              _TYPE(Color),   (NW_Byte*)"input[type=password]{}" },
{_ELEM(input),     _PROP(borderStyle),      _VAL(solid),           _TYPE(Token),   (NW_Byte*)"input[type=password]{}" },
{_ELEM(input),     _PROP(borderWidth),      1,                     _TYPE(Px),      (NW_Byte*)"input[type=password]{}" },
{_ELEM(input),     _PROP(focusBehavior),    _VAL(growBorder),      _TYPE(Token),   (NW_Byte*)"input[type=password]{}" },
{_ELEM(input),     _PROP(margin),           0,                     _TYPE(Px),      (NW_Byte*)"input[type=password]{}" },
{_ELEM(input),     _PROP(padding),          2,                     _TYPE(Px),      (NW_Byte*)"input[type=password]{}" },

{_ELEM(input),     _PROP(backgroundColor),  0xffffff,              _TYPE(Color),   (NW_Byte*)"input[type=hidden]{}" },
{_ELEM(input),     _PROP(borderColor),      0x000000,              _TYPE(Color),   (NW_Byte*)"input[type=hidden]{}" },
{_ELEM(input),     _PROP(borderStyle),      _VAL(hidden),           _TYPE(Token),   (NW_Byte*)"input[type=hidden]{}" },
{_ELEM(input),     _PROP(borderWidth),      0,                     _TYPE(Px),      (NW_Byte*)"input[type=hidden]{}" },
{_ELEM(input),     _PROP(focusBehavior),    0,                     _TYPE(Token),   (NW_Byte*)"input[type=hidden]{}" },
{_ELEM(input),     _PROP(margin),           0,                     _TYPE(Px),      (NW_Byte*)"input[type=hidden]{}" },
{_ELEM(input),     _PROP(padding),          0,                     _TYPE(Px),      (NW_Byte*)"input[type=hidden]{}" },

{_ELEM(input),     _PROP(backgroundColor),  _VAL(transparent),     _TYPE(Token),   (NW_Byte*)"input[type=image]{}" },
{_ELEM(input),     _PROP(borderColor),      0x000000,              _TYPE(Color),   (NW_Byte*)"input[type=image]{}" },
{_ELEM(input),     _PROP(borderStyle),      _VAL(solid),           _TYPE(Token),   (NW_Byte*)"input[type=image]{}" },
{_ELEM(input),     _PROP(borderWidth),      0,                     _TYPE(Px),      (NW_Byte*)"input[type=image]{}" },
{_ELEM(input),     _PROP(focusBehavior),    _VAL(growBorder),      _TYPE(Token),   (NW_Byte*)"input[type=image]{}" },
{_ELEM(input),     _PROP(margin),           1,                     _TYPE(Px),      (NW_Byte*)"input[type=image]{}" },
{_ELEM(input),     _PROP(padding),          1,                     _TYPE(Px),      (NW_Byte*)"input[type=image]{}" },

{_ELEM(input),     _PROP(backgroundColor),  _VAL(transparent),     _TYPE(Token),   (NW_Byte*)"input[type=checkbox]{}" },
{_ELEM(input),     _PROP(borderColor),      0x000000,              _TYPE(Color),   (NW_Byte*)"input[type=checkbox]{}" },
{_ELEM(input),     _PROP(borderStyle),      0,                     _TYPE(Token),   (NW_Byte*)"input[type=checkbox]{}" },
{_ELEM(input),     _PROP(borderWidth),      1,                     _TYPE(Px),      (NW_Byte*)"input[type=checkbox]{}" },
{_ELEM(input),     _PROP(focusBehavior),    0,                     _TYPE(Token),   (NW_Byte*)"input[type=checkbox]{}" },
{_ELEM(input),     _PROP(margin),           1,                     _TYPE(Px),      (NW_Byte*)"input[type=checkbox]{}" },
{_ELEM(input),     _PROP(padding),          1,                     _TYPE(Px),      (NW_Byte*)"input[type=checkbox]{}" },

{_ELEM(input),     _PROP(backgroundColor),  _VAL(transparent),     _TYPE(Token),   (NW_Byte*)"input[type=radio]{}" },
{_ELEM(input),     _PROP(borderColor),      0x000000,              _TYPE(Color),   (NW_Byte*)"input[type=radio]{}" },
{_ELEM(input),     _PROP(borderStyle),      0,                     _TYPE(Token),   (NW_Byte*)"input[type=radio]{}" },
{_ELEM(input),     _PROP(borderWidth),      1,                     _TYPE(Px),      (NW_Byte*)"input[type=radio]{}" },
{_ELEM(input),     _PROP(focusBehavior),    0,                     _TYPE(Token),   (NW_Byte*)"input[type=radio]{}" },
{_ELEM(input),     _PROP(margin),           1,                     _TYPE(Px),      (NW_Byte*)"input[type=radio]{}" },
{_ELEM(input),     _PROP(padding),          1,                     _TYPE(Px),      (NW_Byte*)"input[type=radio]{}" },

{_ELEM(input),     _PROP(backgroundColor),  0xefefef,              _TYPE(Color),   (NW_Byte*)"input[type=file]{}" },
{_ELEM(input),     _PROP(borderColor),      0x000000,              _TYPE(Color),   (NW_Byte*)"input[type=file]{}" },
{_ELEM(input),     _PROP(borderStyle),      _VAL(solid),           _TYPE(Token),   (NW_Byte*)"input[type=file]{}" },
{_ELEM(input),     _PROP(borderWidth),      1,                     _TYPE(Px),      (NW_Byte*)"input[type=file]{}" },
{_ELEM(input),     _PROP(focusBehavior),    _VAL(growBorder),      _TYPE(Token),   (NW_Byte*)"input[type=file]{}" },
{_ELEM(input),     _PROP(margin),           0,                     _TYPE(Px),      (NW_Byte*)"input[type=file]{}" },
{_ELEM(input),     _PROP(padding),          2,                     _TYPE(Px),      (NW_Byte*)"input[type=file]{}" },

// Keep input-type-text and default input-type (below) in synch.
{_ELEM(input),     _PROP(backgroundColor),  0xffffff,              _TYPE(Color),   (NW_Byte*)"input[type=text]{}" },
{_ELEM(input),     _PROP(borderColor),      0x000000,              _TYPE(Color),   (NW_Byte*)"input[type=text]{}" },
{_ELEM(input),     _PROP(borderStyle),      _VAL(solid),           _TYPE(Token),   (NW_Byte*)"input[type=text]{}" },
{_ELEM(input),     _PROP(borderWidth),      1,                     _TYPE(Px),      (NW_Byte*)"input[type=text]{}" },
{_ELEM(input),     _PROP(focusBehavior),    _VAL(growBorder),      _TYPE(Token),   (NW_Byte*)"input[type=text]{}" },
{_ELEM(input),     _PROP(margin),           0,                     _TYPE(Px),      (NW_Byte*)"input[type=text]{}" },
{_ELEM(input),     _PROP(padding),          2,                     _TYPE(Px),      (NW_Byte*)"input[type=text]{}" },
// When no type specified, the default type is text; therefore, the values need to be the same as for 'type=text' above.
// Table order is important. These default 'input' entries must occur after all other 'input' entries. Also note, any
// additions to the default input will affect all other input elements that do not already have a table entry for
// that property; therefore, the corresponding entry may need to be added for other input types.
{_ELEM(input),     _PROP(backgroundColor),  0xffffff,              _TYPE(Color),   (NW_Byte*)"input{}" },
{_ELEM(input),     _PROP(borderColor),      0x000000,              _TYPE(Color),   (NW_Byte*)"input{}" },
{_ELEM(input),     _PROP(borderStyle),      _VAL(solid),           _TYPE(Token),   (NW_Byte*)"input{}" },
{_ELEM(input),     _PROP(borderWidth),      1,                     _TYPE(Px),      (NW_Byte*)"input{}" },
{_ELEM(input),     _PROP(focusBehavior),    _VAL(growBorder),      _TYPE(Token),   (NW_Byte*)"input{}" },
{_ELEM(input),     _PROP(margin),           0,                     _TYPE(Px),      (NW_Byte*)"input{}" },
{_ELEM(input),     _PROP(padding),          2,                     _TYPE(Px),      (NW_Byte*)"input{}" },

{_ELEM(ins),       _PROP(textDecoration),   _VAL(underline),       _TYPE(Token),   NULL },
{_ELEM(kbd),       _PROP(fontFamily),       0,                     _TYPE(Text),    NULL },
{_ELEM(kbd),       _PROP(display),          _VAL(display_block),   _TYPE(Token),   NULL },
{_ELEM(li),        _PROP(display),          _VAL(display_listItem),_TYPE(Token),   NULL },
{_ELEM(li),        _PROP(topMargin),        2,                     _TYPE(Px),      NULL },
{_ELEM(li),        _PROP(bottomMargin),     2,                     _TYPE(Px),      NULL },
{_ELEM(map),       _PROP(display),          _VAL(display_block),   _TYPE(Token),   NULL },
{_ELEM(marquee),   _PROP(display),          _VAL(display_marquee), _TYPE(Token),   NULL },
{_ELEM(marquee),   _PROP(topMargin),        2,                     _TYPE(Px),      NULL },
{_ELEM(marquee),   _PROP(bottomMargin),     2,                     _TYPE(Px),      NULL },
{_ELEM(menu),      _PROP(display),          _VAL(display_block),   _TYPE(Token),   NULL },
{_ELEM(menu),      _PROP(listStyleType),    _VAL(square),          _TYPE(Token),   (NW_Byte*)"menu menu menu{}" },
{_ELEM(menu),      _PROP(listStyleType),    _VAL(circle),          _TYPE(Token),   (NW_Byte*)"menu menu{}" },
{_ELEM(menu),      _PROP(listStyleType),    _VAL(disc),            _TYPE(Token),   NULL },
{_ELEM(menu),      _PROP(leftMargin),       17,                    _TYPE(Px),      NULL },
{_ELEM(menu),      _PROP(topMargin),        10,                    _TYPE(Px),      NULL },
{_ELEM(menu),      _PROP(bottomMargin),     10,                    _TYPE(Px),      NULL },
{_ELEM(nobr),      _PROP(whitespace),       _VAL(nowrap),          _TYPE(Token),   NULL },
{_ELEM(noscript),  _PROP(display),          _VAL(display_block),   _TYPE(Token),   NULL },
{_ELEM(ol),        _PROP(display),          _VAL(display_block),   _TYPE(Token),   NULL },
{_ELEM(ol),        _PROP(listStyleType),    _VAL(decimal),         _TYPE(Token),   NULL },
{_ELEM(ol),        _PROP(listMargin),       20,                    _TYPE(Px),      NULL },
{_ELEM(ol),        _PROP(topMargin),        10,                    _TYPE(Px),      NULL },
{_ELEM(ol),        _PROP(bottomMargin),     10,                    _TYPE(Px),      NULL },
{_ELEM(p),         _PROP(display),          _VAL(display_block),   _TYPE(Token),   NULL },
{_ELEM(p),         _PROP(topMargin),        10,                    _TYPE(Px),      NULL },
{_ELEM(p),         _PROP(bottomMargin),     10,                    _TYPE(Px),      NULL },
{_ELEM(plaintext), _PROP(display),          _VAL(display_block),   _TYPE(Token),   NULL },
{_ELEM(plaintext), _PROP(whitespace),       _VAL(pre),             _TYPE(Token),   NULL },
{_ELEM(plaintext), _PROP(fontFamily),       0,                     _TYPE(Text),    NULL },
{_ELEM(pre),       _PROP(display),          _VAL(display_block),   _TYPE(Token),   NULL },
{_ELEM(pre),       _PROP(whitespace),       _VAL(pre),             _TYPE(Token),   NULL },
{_ELEM(pre),       _PROP(fontFamily),       0,                     _TYPE(Text),    NULL },
{_ELEM(q),         _PROP(imodeQuotes),      2,                     _TYPE(Text),    (NW_Byte*)"q q{}" },
{_ELEM(q),         _PROP(imodeQuotes),      1,                     _TYPE(Text),    NULL },
{_ELEM(s),         _PROP(textDecoration),   _VAL(strikeout),       _TYPE(Token),   NULL },
{_ELEM(samp),      _PROP(fontFamily),       0,                     _TYPE(Text),    NULL },
{_ELEM(select),    _PROP(borderStyle),      _VAL(solid),           _TYPE(Token),   NULL },
{_ELEM(select),    _PROP(borderColor),      0x000000,              _TYPE(Color),   NULL },
{_ELEM(select),    _PROP(focusBehavior),    _VAL(growBorder),      _TYPE(Token),   NULL },
{_ELEM(select),    _PROP(borderWidth),      1,                     _TYPE(Px),      NULL },
{_ELEM(select),    _PROP(padding),          2,                     _TYPE(Px),      NULL },
{_ELEM(select),    _PROP(backgroundColor),  0xffffff,              _TYPE(Color),   NULL },
{_ELEM(small),     _PROP(fontSize),         6,                     _TYPE(Pt),      NULL },
{_ELEM(strike),    _PROP(textDecoration),   _VAL(strikeout),       _TYPE(Token),   NULL },
{_ELEM(strong),    _PROP(fontWeight),       900,                   _TYPE(Integer), NULL },
{_ELEM(table),     _PROP(display),          _VAL(display_block),   _TYPE(Token),   NULL },
{_ELEM(td),        _PROP(display),          _VAL(display_block),   _TYPE(Token),   NULL },
{_ELEM(td),        _PROP(verticalAlign),    _VAL(top),             _TYPE(Token),   NULL },
{_ELEM(td),        _PROP(borderWidth),      _VAL(medium),          _TYPE(Token),   NULL },
{_ELEM(td),        _PROP(borderColor),      0x000000,              _TYPE(Color),   NULL },
{_ELEM(td),        _PROP(overflow),         _VAL(hidden),          _TYPE(Token),   NULL },
{_ELEM(textarea),  _PROP(focusBehavior),    _VAL(growBorder),      _TYPE(Token),   NULL },
{_ELEM(textarea),  _PROP(padding),          2,                     _TYPE(Px),      NULL },
{_ELEM(textarea),  _PROP(borderWidth),      1,                     _TYPE(Px),      NULL },
{_ELEM(textarea),  _PROP(borderStyle),      _VAL(solid),           _TYPE(Token),   NULL },
{_ELEM(textarea),  _PROP(borderColor),      0x000000,              _TYPE(Color),   NULL },
{_ELEM(th),        _PROP(display),          _VAL(display_block),   _TYPE(Token),   NULL },
{_ELEM(th),        _PROP(textAlign),        _VAL(center),          _TYPE(Token),   NULL },
{_ELEM(th),        _PROP(verticalAlign),    _VAL(middle),          _TYPE(Token),   NULL },
{_ELEM(th),        _PROP(borderWidth),      1,                     _TYPE(Px),      NULL },
{_ELEM(th),        _PROP(borderStyle),      _VAL(none),            _TYPE(Token),   NULL },
{_ELEM(th),        _PROP(borderColor),      0x000000,              _TYPE(Color),   NULL },
{_ELEM(th),        _PROP(fontWeight),       700,                   _TYPE(Integer), NULL },
{_ELEM(th),        _PROP(overflow),         _VAL(hidden),          _TYPE(Token),   NULL },
{_ELEM(tr),        _PROP(fontSize),         7,                     _TYPE(Pt),      NULL },
{_ELEM(tt),        _PROP(fontFamily),       0,                     _TYPE(Text),    NULL },
{_ELEM(u),         _PROP(textDecoration),   _VAL(underline),       _TYPE(Token),   NULL },
{_ELEM(ul),        _PROP(display),          _VAL(display_block),   _TYPE(Token),   NULL },
{_ELEM(ul),        _PROP(listStyleType),    _VAL(square),          _TYPE(Token),   (NW_Byte*)"ul ul ul{}" },
{_ELEM(ul),        _PROP(listStyleType),    _VAL(circle),          _TYPE(Token),   (NW_Byte*)"ul ul{}" },
{_ELEM(ul),        _PROP(listStyleType),    _VAL(disc),            _TYPE(Token),   NULL },
{_ELEM(ul),        _PROP(listMargin),       20,                    _TYPE(Px),      NULL },
{_ELEM(ul),        _PROP(topMargin),        10,                    _TYPE(Px),      NULL },
{_ELEM(ul),        _PROP(bottomMargin),     10,                    _TYPE(Px),      NULL },
{_ELEM(var),       _PROP(fontStyle),        _VAL(italic),          _TYPE(Token),   NULL },
{0xFFFF,           _PROP(fontFamily),       0,                     _TYPE(Text),    NULL },
};

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_CSS_DefaultStyleSheet_Class_t NW_CSS_DefaultStyleSheet_Class = {
  { /* NW_Object_Core           */
    /* super                    */ &NW_Object_Core_Class,
    /* queryInterface           */ _NW_Object_Core_QueryInterface
  },
  { /* NW_CSS_DefaultStyleSheet */
    /* numPropertyVals           */ sizeof (_NW_CSS_DefaultStyleSheet_PropVals)
                                      / sizeof (NW_CSS_DefaultStyleSheet_PropEntry_t),
    /* propertyVals              */ _NW_CSS_DefaultStyleSheet_PropVals,
    /* numStringVals             */ 3,
    /* stringVals                */ _NW_CSS_DefaultStyleSheet_StringVals
  }
};

/* ------------------------------------------------------------------------- */
const NW_CSS_DefaultStyleSheet_t NW_CSS_DefaultStyleSheet = {
  { &NW_CSS_DefaultStyleSheet_Class }
};

