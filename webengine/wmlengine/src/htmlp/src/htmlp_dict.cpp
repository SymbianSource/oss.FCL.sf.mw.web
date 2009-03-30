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

#ifndef FEA_RME_NOHTMLPARSER


#include "nwx_defs.h"
#include "CHtmlpParser.h"
#include "nw_htmlp_html_dict.h"
/*
must keep tags alphabetized by name

the form for the index define value is previous_tag + 1,
except for 0'th index

static const NW_Uint8 htmlp_tag_[] = {};
#define HTMLP_HTML_TAG_INDEX_ HTMLP_HTML_TAG_INDEX_ + 1
*/

static const NW_Uint8 NW_HTMLP_tag_A[] = {1, 'a'};

static const NW_Uint8 NW_HTMLP_tag_ABBR[] = {4, 'a','b','b','r'};

static const NW_Uint8 NW_HTMLP_tag_ACRONYM[] 
= {7, 'a','c','r','o', 'n', 'y', 'm'};

static const NW_Uint8 NW_HTMLP_tag_ADDRESS[] 
= {7, 'a','d','d','r', 'e', 's', 's'};

static const NW_Uint8 NW_HTMLP_tag_AREA[]
= {4, 'a','r','e','a'};

static const NW_Uint8 NW_HTMLP_tag_B[] = {1, 'b'};

static const NW_Uint8 NW_HTMLP_tag_BASE[] = {4, 'b','a','s','e'};

static const NW_Uint8 NW_HTMLP_tag_BIG[] = {3, 'b','i','g'};

static const NW_Uint8 NW_HTMLP_tag_BLINK[] = {5, 'b', 'l', 'i','n','k'};

static const NW_Uint8 NW_HTMLP_tag_BLOCKQUOTE[]
= {10, 'b','l','o','c','k','q','u','o','t','e'};

static const NW_Uint8 NW_HTMLP_tag_BODY[] =   {4, 'b','o','d','y'};

static const NW_Uint8 NW_HTMLP_tag_BR[] = {2, 'b','r'};

static const NW_Uint8 NW_HTMLP_tag_BUTTON[] = {6,'b','u','t','t','o','n'};

static const NW_Uint8 NW_HTMLP_tag_CAPTION[]
= {7, 'c','a','p','t','i','o', 'n'};

static const NW_Uint8 NW_HTMLP_tag_CENTER[] = {6, 'c','e','n','t','e','r'};

static const NW_Uint8 NW_HTMLP_tag_CITE[] = {4, 'c','i', 't','e'};

static const NW_Uint8 NW_HTMLP_tag_CODE[] = {4, 'c','o', 'd','e'};

static const NW_Uint8 NW_HTMLP_tag_DD[] = {2, 'd','d'};

static const NW_Uint8 NW_HTMLP_tag_DEL[] = {3,'d','e','l'};

static const NW_Uint8 NW_HTMLP_tag_DFN[] = {3, 'd','f','n'};

static const NW_Uint8 NW_HTMLP_tag_DIR[] = {3, 'd','i','r'};

static const NW_Uint8 NW_HTMLP_tag_DIV[] = {3, 'd','i','v'};

static const NW_Uint8 NW_HTMLP_tag_DL[] = {2, 'd','l'};

static const NW_Uint8 NW_HTMLP_tag_DT[] = {2, 'd','t'};

static const NW_Uint8 NW_HTMLP_tag_EM[] = {2, 'e','m'};

static const NW_Uint8 NW_HTMLP_tag_EMBED[] = {5, 'e','m','b','e','d'};

static const NW_Uint8 NW_HTMLP_tag_FIELDSET[] = {8, 'f','i','e','l','d','s','e','t'};

static const NW_Uint8 NW_HTMLP_tag_FONT[] = {4, 'f','o','n','t'};

static const NW_Uint8 NW_HTMLP_tag_FORM[] = {4, 'f','o','r','m'};

static const NW_Uint8 NW_HTMLP_tag_FRAME[] = {5, 'f','r','a','m','e'};

static const NW_Uint8 NW_HTMLP_tag_FRAMESET[] = {8, 'f','r','a','m','e','s','e','t'};

static const NW_Uint8 NW_HTMLP_tag_H1[] = {2, 'h','1'};

static const NW_Uint8 NW_HTMLP_tag_H2[] = {2, 'h','2'};

static const NW_Uint8 NW_HTMLP_tag_H3[] = {2, 'h','3'};

static const NW_Uint8 NW_HTMLP_tag_H4[] = {2, 'h','4'};

static const NW_Uint8 NW_HTMLP_tag_H5[] = {2, 'h','5'};

static const NW_Uint8 NW_HTMLP_tag_H6[] = {2, 'h','6'};

static const NW_Uint8 NW_HTMLP_tag_HEAD[] =   {4, 'h','e','a','d'};

static const NW_Uint8 NW_HTMLP_tag_HR[] = {2, 'h','r'};

static const NW_Uint8 NW_HTMLP_tag_HTML[] =   {4, 'h','t','m','l'};

static const NW_Uint8 NW_HTMLP_tag_I[] = {1, 'i'};

static const NW_Uint8 NW_HTMLP_tag_IFRAME[] = {6, 'i','f','r','a','m','e'};

static const NW_Uint8 NW_HTMLP_tag_IMG[] = {3, 'i','m','g'};

static const NW_Uint8 NW_HTMLP_tag_INPUT[] = {5, 'i','n','p','u','t'};

static const NW_Uint8 NW_HTMLP_tag_INS[] = {3, 'i','n','s'};

static const NW_Uint8 NW_HTMLP_tag_KBD[] = {3, 'k','b','d'};

static const NW_Uint8 NW_HTMLP_tag_LABEL[] = {5, 'l','a','b','e','l'};

static const NW_Uint8 NW_HTMLP_tag_LI[] = {2, 'l','i'};

static const NW_Uint8 NW_HTMLP_tag_LINK[] = {4, 'l','i','n','k'};

static const NW_Uint8 NW_HTMLP_tag_MAP[]
= {3, 'm','a','p'};

static const NW_Uint8 NW_HTMLP_tag_MARQUEE[]
= {7, 'm','a','r', 'q', 'u', 'e', 'e'};

static const NW_Uint8 NW_HTMLP_tag_MENU[] = {4, 'm','e','n','u'};

static const NW_Uint8 NW_HTMLP_tag_META[] = {4, 'm','e','t','a'};

static const NW_Uint8 NW_HTMLP_tag_NOBR[] = {4, 'n','o','b','r'};

static const NW_Uint8 NW_HTMLP_tag_NOEMBED[] = {7, 'n','o','e','m','b','e','d'};

static const NW_Uint8 NW_HTMLP_tag_NOFRAMES[]
= {8, 'n','o','f','r','a','m','e','s'};

static const NW_Uint8 NW_HTMLP_tag_NOSCRIPT[]
= {8, 'n','o','s','c','r','i','p','t'};

static const NW_Uint8 NW_HTMLP_tag_OBJECT[] = {6, 'o','b','j','e','c','t'};

static const NW_Uint8 NW_HTMLP_tag_OL[] = {2, 'o','l'};

static const NW_Uint8 NW_HTMLP_tag_OPTGROUP[]
= {8, 'o','p','t','g','r','o', 'u', 'p'};

static const NW_Uint8 NW_HTMLP_tag_OPTION[] = {6, 'o','p','t','i','o','n'};

static const NW_Uint8 NW_HTMLP_tag_P[] =   {1, 'p'};

static const NW_Uint8 NW_HTMLP_tag_PARAM[] =   {5, 'p', 'a', 'r', 'a', 'm'};

static const NW_Uint8 NW_HTMLP_tag_PLAINTEXT[]
= {9, 'p','l','a','i','n','t','e','x','t'};

static const NW_Uint8 NW_HTMLP_tag_PRE[] = {3, 'p','r','e'};

static const NW_Uint8 NW_HTMLP_tag_Q[] = {1, 'q'};

static const NW_Uint8 NW_HTMLP_tag_S[] = {1, 's'};

static const NW_Uint8 NW_HTMLP_tag_SAMP[] = {4, 's','a','m', 'p'};

static const NW_Uint8 NW_HTMLP_tag_SCRIPT[] = {6, 's','c','r','i','p','t'};

static const NW_Uint8 NW_HTMLP_tag_SELECT[] = {6, 's','e','l','e','c','t'};

static const NW_Uint8 NW_HTMLP_tag_SMALL[] = {5, 's','m','a','l','l'};

static const NW_Uint8 NW_HTMLP_tag_SPAN[] = {4, 's','p','a','n'};

static const NW_Uint8 NW_HTMLP_tag_STRIKE[] = {6, 's','t','r','i','k','e'};

static const NW_Uint8 NW_HTMLP_tag_STRONG[] = {6, 's','t','r','o','n','g'};

static const NW_Uint8 NW_HTMLP_tag_STYLE[] = {5, 's','t','y','l','e'};

static const NW_Uint8 NW_HTMLP_tag_SUB[]   = {3,'s','u','b'};

static const NW_Uint8 NW_HTMLP_tag_SUP[]   = {3,'s','u','p'};

static const NW_Uint8 NW_HTMLP_tag_TABLE[] =  {5, 't','a','b','l','e'};

static const NW_Uint8 NW_HTMLP_tag_TD[] =  {2, 't','d'};

static const NW_Uint8 NW_HTMLP_tag_TEXTAREA[]
= {8, 't','e','x','t','a','r','e','a'};

static const NW_Uint8 NW_HTMLP_tag_TH[] =  {2, 't','h'};

static const NW_Uint8 NW_HTMLP_tag_TITLE[] =  {5, 't','i','t','l','e'};

static const NW_Uint8 NW_HTMLP_tag_TR[] =  {2, 't','r'};

static const NW_Uint8 NW_HTMLP_tag_TT[] =  {2, 't','t'};

static const NW_Uint8 NW_HTMLP_tag_U[] =  {1, 'u'};

static const NW_Uint8 NW_HTMLP_tag_UL[] = {2, 'u','l'};

static const NW_Uint8 NW_HTMLP_tag_VAR[] = {3, 'v','a','r'};

static const NW_Uint8 NW_HTMLP_tag_WMLcVALUEdOF[]
= {12 ,'w','m','l',':','v','a','l','u','e','-','o','f'};


/* NOTE: The order in these next lists is not important. */

/* Don't allow another <a> tag before closing the earlier one */

static const NW_HTMLP_ElementTableIndex_t NW_HTMLP_closes_only_a[]
= {1,
  HTMLP_HTML_TAG_INDEX_A};

static const NW_HTMLP_ElementTableIndex_t NW_HTMLP_closes_only_head[]
= {1,
  HTMLP_HTML_TAG_INDEX_HEAD};

static const NW_HTMLP_ElementTableIndex_t NW_HTMLP_closes_list_items[]
= {4,
  HTMLP_HTML_TAG_INDEX_DD, HTMLP_HTML_TAG_INDEX_DT, HTMLP_HTML_TAG_INDEX_LI,
  HTMLP_HTML_TAG_INDEX_HEAD};

static const NW_HTMLP_ElementTableIndex_t NW_HTMLP_blocks_list_items[]
= {5,
  HTMLP_HTML_TAG_INDEX_DIR, HTMLP_HTML_TAG_INDEX_DL, HTMLP_HTML_TAG_INDEX_MENU,
  HTMLP_HTML_TAG_INDEX_OL, HTMLP_HTML_TAG_INDEX_UL};

static const NW_HTMLP_ElementTableIndex_t NW_HTMLP_closes_TR[]
= {2,
  HTMLP_HTML_TAG_INDEX_TR, HTMLP_HTML_TAG_INDEX_HEAD};

static const NW_HTMLP_ElementTableIndex_t NW_HTMLP_blocks_TR[]
= {1,
  HTMLP_HTML_TAG_INDEX_TABLE};

static const NW_HTMLP_ElementTableIndex_t NW_HTMLP_closes_TD_TH[]
= {4,
  HTMLP_HTML_TAG_INDEX_CAPTION, HTMLP_HTML_TAG_INDEX_TD, HTMLP_HTML_TAG_INDEX_TH,
  HTMLP_HTML_TAG_INDEX_HEAD};

static const NW_HTMLP_ElementTableIndex_t NW_HTMLP_blocks_TD_TH[]
= {2,
  HTMLP_HTML_TAG_INDEX_TABLE, HTMLP_HTML_TAG_INDEX_TR};

static const NW_HTMLP_ElementTableIndex_t NW_HTMLP_closes_OPTGROUP[]
= {3,
  HTMLP_HTML_TAG_INDEX_OPTGROUP, HTMLP_HTML_TAG_INDEX_HEAD, HTMLP_HTML_TAG_INDEX_OPTION};

static const NW_HTMLP_ElementTableIndex_t NW_HTMLP_blocks_OPTGROUP[]
= {1,
  HTMLP_HTML_TAG_INDEX_SELECT};

static const NW_HTMLP_ElementTableIndex_t NW_HTMLP_closes_OPTION[]
= {2,
  HTMLP_HTML_TAG_INDEX_OPTION, HTMLP_HTML_TAG_INDEX_HEAD};

static const NW_HTMLP_ElementTableIndex_t NW_HTMLP_blocks_OPTION[]
= {2,
  HTMLP_HTML_TAG_INDEX_SELECT, HTMLP_HTML_TAG_INDEX_OPTGROUP};

static const NW_HTMLP_ElementTableIndex_t NW_HTMLP_closes_P[]
= {2,
  HTMLP_HTML_TAG_INDEX_P, HTMLP_HTML_TAG_INDEX_HEAD};

static const NW_HTMLP_ElementTableIndex_t NW_HTMLP_blocks_P[]
= {37,
  HTMLP_HTML_TAG_INDEX_A, HTMLP_HTML_TAG_INDEX_ADDRESS, HTMLP_HTML_TAG_INDEX_BLOCKQUOTE,
  HTMLP_HTML_TAG_INDEX_BODY, HTMLP_HTML_TAG_INDEX_CAPTION, HTMLP_HTML_TAG_INDEX_CENTER,
  HTMLP_HTML_TAG_INDEX_CODE, HTMLP_HTML_TAG_INDEX_DD, HTMLP_HTML_TAG_INDEX_DIR,
  HTMLP_HTML_TAG_INDEX_DIV, HTMLP_HTML_TAG_INDEX_DL, HTMLP_HTML_TAG_INDEX_DT,
  HTMLP_HTML_TAG_INDEX_FONT, HTMLP_HTML_TAG_INDEX_FORM, HTMLP_HTML_TAG_INDEX_H1,
  HTMLP_HTML_TAG_INDEX_H2, HTMLP_HTML_TAG_INDEX_H3, HTMLP_HTML_TAG_INDEX_H4,
  HTMLP_HTML_TAG_INDEX_H5, HTMLP_HTML_TAG_INDEX_H6, HTMLP_HTML_TAG_INDEX_FIELDSET,
  HTMLP_HTML_TAG_INDEX_LABEL, HTMLP_HTML_TAG_INDEX_LI, HTMLP_HTML_TAG_INDEX_MARQUEE,
  HTMLP_HTML_TAG_INDEX_MENU,  HTMLP_HTML_TAG_INDEX_OBJECT, HTMLP_HTML_TAG_INDEX_OL,
  HTMLP_HTML_TAG_INDEX_OPTGROUP,  HTMLP_HTML_TAG_INDEX_OPTION, HTMLP_HTML_TAG_INDEX_PRE,
  HTMLP_HTML_TAG_INDEX_SELECT, HTMLP_HTML_TAG_INDEX_SPAN, HTMLP_HTML_TAG_INDEX_TABLE,
  HTMLP_HTML_TAG_INDEX_TD,  HTMLP_HTML_TAG_INDEX_TH, HTMLP_HTML_TAG_INDEX_TR,
  HTMLP_HTML_TAG_INDEX_UL};

static const NW_HTMLP_ElementTableIndex_t NW_HTMLP_blocks_A[]
= {37,
  HTMLP_HTML_TAG_INDEX_ADDRESS, HTMLP_HTML_TAG_INDEX_BLOCKQUOTE,
  HTMLP_HTML_TAG_INDEX_BODY, HTMLP_HTML_TAG_INDEX_CAPTION, HTMLP_HTML_TAG_INDEX_CENTER,
  HTMLP_HTML_TAG_INDEX_CODE, HTMLP_HTML_TAG_INDEX_DD, HTMLP_HTML_TAG_INDEX_DIR,
  HTMLP_HTML_TAG_INDEX_DIV, HTMLP_HTML_TAG_INDEX_DL, HTMLP_HTML_TAG_INDEX_DT,
  HTMLP_HTML_TAG_INDEX_FONT, HTMLP_HTML_TAG_INDEX_FORM, HTMLP_HTML_TAG_INDEX_H1,
  HTMLP_HTML_TAG_INDEX_H2, HTMLP_HTML_TAG_INDEX_H3, HTMLP_HTML_TAG_INDEX_H4,
  HTMLP_HTML_TAG_INDEX_H5, HTMLP_HTML_TAG_INDEX_H6, HTMLP_HTML_TAG_INDEX_FIELDSET,
  HTMLP_HTML_TAG_INDEX_LABEL, HTMLP_HTML_TAG_INDEX_LI, HTMLP_HTML_TAG_INDEX_MARQUEE,
  HTMLP_HTML_TAG_INDEX_MENU,  HTMLP_HTML_TAG_INDEX_OBJECT, HTMLP_HTML_TAG_INDEX_OL,
  HTMLP_HTML_TAG_INDEX_OPTGROUP,  HTMLP_HTML_TAG_INDEX_OPTION, HTMLP_HTML_TAG_INDEX_P,
  HTMLP_HTML_TAG_INDEX_PRE,
  HTMLP_HTML_TAG_INDEX_SELECT, HTMLP_HTML_TAG_INDEX_SPAN, HTMLP_HTML_TAG_INDEX_TABLE,
  HTMLP_HTML_TAG_INDEX_TD,  HTMLP_HTML_TAG_INDEX_TH, HTMLP_HTML_TAG_INDEX_TR,
  HTMLP_HTML_TAG_INDEX_UL};



/* must keep alphabetized by tag name 
 * */
 
/* In some sites, <a> tag can occur inside the <head> elements. So, 
 * NW_HTMLP_closes_only_head is removed from the <a> element list.
 */

NW_HTMLP_ElementDescriptionConst_t NW_HTMLP_ElementDescriptionTable[] = {
/* tag                      closes                        blocks     contentType  splHandling */
  {NW_HTMLP_tag_A,          NW_HTMLP_closes_only_a,       NW_HTMLP_blocks_A,       ANY, NW_TRUE},
  {NW_HTMLP_tag_ABBR,       NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_ACRONYM,    NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_ADDRESS,    NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_AREA,       NW_HTMLP_closes_only_head,    NULL,       EMPTY, NW_FALSE},
  {NW_HTMLP_tag_B,          NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_BASE,       NULL,                         NULL,       EMPTY, NW_TRUE},
  {NW_HTMLP_tag_BIG,        NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_BLINK,      NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_BLOCKQUOTE, NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_BODY,       NW_HTMLP_closes_only_head,    NULL,       ANY, NW_TRUE},
  {NW_HTMLP_tag_BR,         NW_HTMLP_closes_only_head,    NULL,       EMPTY, NW_FALSE},


  {NW_HTMLP_tag_BUTTON,     NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},

  {NW_HTMLP_tag_CAPTION,    NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_CENTER,     NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_CITE,       NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_CODE,       NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_DD,
    NW_HTMLP_closes_list_items, NW_HTMLP_blocks_list_items,           ANY, NW_FALSE},


  {NW_HTMLP_tag_DEL,        NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},

  {NW_HTMLP_tag_DFN,        NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_DIR,        NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_DIV,        NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_DL,         NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_DT,
    NW_HTMLP_closes_list_items, NW_HTMLP_blocks_list_items,           ANY, NW_FALSE},
  {NW_HTMLP_tag_EM,         NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_EMBED,      NW_HTMLP_closes_only_head,    NULL,       EMPTY, NW_FALSE},
  {NW_HTMLP_tag_FIELDSET,   NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_FONT,       NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_FORM,       NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_FRAME,      NW_HTMLP_closes_only_head,    NULL,       EMPTY, NW_FALSE},
  {NW_HTMLP_tag_FRAMESET,   NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_H1,         NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_H2,         NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_H3,         NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_H4,         NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_H5,         NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_H6,         NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_HEAD,       NULL,                         NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_HR,         NW_HTMLP_closes_only_head,    NULL,       EMPTY, NW_FALSE},
  {NW_HTMLP_tag_HTML,       NULL,                         NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_I,          NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_IFRAME,     NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_IMG,        NW_HTMLP_closes_only_head,    NULL,       EMPTY, NW_FALSE},
  {NW_HTMLP_tag_INPUT,      NW_HTMLP_closes_only_head,    NULL,       EMPTY, NW_FALSE},

  {NW_HTMLP_tag_INS,        NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},


  {NW_HTMLP_tag_KBD,        NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_LABEL,      NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_LI,
    NW_HTMLP_closes_list_items, NW_HTMLP_blocks_list_items,           ANY, NW_FALSE},
  {NW_HTMLP_tag_LINK,       NULL,                         NULL,       EMPTY, NW_FALSE},
  {NW_HTMLP_tag_MAP,        NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_MARQUEE,    NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_MENU,       NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
 {NW_HTMLP_tag_META,       NULL,                         NULL,       EMPTY, NW_TRUE},
  {NW_HTMLP_tag_NOBR,       NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_NOEMBED,    NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_NOFRAMES,   NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_NOSCRIPT,   NULL,                         NULL,       ANY, NW_TRUE},
  {NW_HTMLP_tag_OBJECT,     NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_OL,         NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_OPTGROUP,
    NW_HTMLP_closes_OPTGROUP, NW_HTMLP_blocks_OPTGROUP,               ANY, NW_FALSE},
  {NW_HTMLP_tag_OPTION,
    NW_HTMLP_closes_OPTION, NW_HTMLP_blocks_OPTION,                   ANY, NW_FALSE},
  {NW_HTMLP_tag_P,
    NW_HTMLP_closes_P, NW_HTMLP_blocks_P,                             ANY, NW_FALSE},
  {NW_HTMLP_tag_PARAM,      NW_HTMLP_closes_only_head,    NULL,       EMPTY, NW_FALSE},
  {NW_HTMLP_tag_PLAINTEXT,  NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_PRE,        NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_Q,          NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},

  {NW_HTMLP_tag_S,          NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},

  {NW_HTMLP_tag_SAMP,       NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_SCRIPT,     NULL,                         NULL,       PCDATA, NW_TRUE},
  {NW_HTMLP_tag_SELECT,     NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_SMALL,      NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_SPAN  ,     NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},

  {NW_HTMLP_tag_STRIKE,     NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},

  {NW_HTMLP_tag_STRONG,     NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_STYLE,      NULL,                         NULL,       PCDATA, NW_FALSE},

  {NW_HTMLP_tag_SUB,        NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_SUP,        NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  

  {NW_HTMLP_tag_TABLE,      NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_TD,
    NW_HTMLP_closes_TD_TH, NW_HTMLP_blocks_TD_TH,                     ANY, NW_FALSE},
  {NW_HTMLP_tag_TEXTAREA,   NW_HTMLP_closes_only_head,    NULL,       PCDATA, NW_FALSE},
  {NW_HTMLP_tag_TH,
    NW_HTMLP_closes_TD_TH, NW_HTMLP_blocks_TD_TH,                     ANY, NW_FALSE},
  {NW_HTMLP_tag_TITLE,      NULL,                         NULL,       PCDATA, NW_TRUE},
  {NW_HTMLP_tag_TR,
    NW_HTMLP_closes_TR, NW_HTMLP_blocks_TR,                           ANY, NW_FALSE},

  {NW_HTMLP_tag_TT,          NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},

  {NW_HTMLP_tag_U,          NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_UL,         NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_VAR,        NW_HTMLP_closes_only_head,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_WMLcVALUEdOF, NW_HTMLP_closes_only_head,  NULL,       ANY, NW_FALSE}
};


const NW_HTMLP_ElementTableIndex_t NW_HTMLP_ElementTableCount
= (sizeof(NW_HTMLP_ElementDescriptionTable)
   / sizeof(NW_HTMLP_ElementDescriptionConst_t));


/* 
 * Access routines to get pointers to NW_HTMLP_ElementDescriptionTable &
 * NW_HTMLP_ElementTableCount from outside the XmlParserDLL
 */
NW_HTMLP_ElementDescriptionConst_t* NW_HTMLP_Get_ElementDescriptionTable()
{ 
  return (NW_HTMLP_ElementDescriptionTable);
}

NW_HTMLP_ElementTableIndex_t NW_HTMLP_Get_ElementTableCount()
{
  return (NW_HTMLP_ElementTableCount);
}

#else

void FeaRmeNoHTMLParser_htmlp_dict(){
   int i = 0;
   i+=1;
}
#endif /* FEA_RME_NOHTMLPARSER */
