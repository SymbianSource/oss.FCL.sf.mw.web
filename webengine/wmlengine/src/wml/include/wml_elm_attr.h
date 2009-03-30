/*
* Copyright (c) 1999 - 2001 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef WML_ELM_ATTR_H
#define WML_ELM_ATTR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nw_wml1x_wml_1_3_tokens.h"

/* public element types */
#define STRING_ELEMENT        1
#define END_ELEMENT           2

#define UNKNOWN_ELEMENT       NW_Wml_1_3_ElementToken_zzzunknown
#define DO_ELEMENT            NW_Wml_1_3_ElementToken_do
#define INPUT_ELEMENT         NW_Wml_1_3_ElementToken_input
#define SELECT_ELEMENT        NW_Wml_1_3_ElementToken_select
#define CARD_ELEMENT          NW_Wml_1_3_ElementToken_card
#define GO_ELEMENT            NW_Wml_1_3_ElementToken_go
#define PREV_ELEMENT          NW_Wml_1_3_ElementToken_prev
#define REFRESH_ELEMENT       NW_Wml_1_3_ElementToken_refresh
#define NOOP_ELEMENT          NW_Wml_1_3_ElementToken_noop
#define SETVAR_ELEMENT        NW_Wml_1_3_ElementToken_setvar
#define ONEVENT_ELEMENT       NW_Wml_1_3_ElementToken_onevent
#define WML_ELEMENT           NW_Wml_1_3_ElementToken_wml
#define HEAD_ELEMENT          NW_Wml_1_3_ElementToken_head
#define TEMPLATE_ELEMENT      NW_Wml_1_3_ElementToken_template
#define OPTION_ELEMENT        NW_Wml_1_3_ElementToken_option
#define OPTGRP_ELEMENT        NW_Wml_1_3_ElementToken_optgroup
#define A_ELEMENT             NW_Wml_1_3_ElementToken_a
#define ANCHOR_ELEMENT        NW_Wml_1_3_ElementToken_anchor
#define FIELDSET_ELEMENT      NW_Wml_1_3_ElementToken_fieldset
#define IMAGE_ELEMENT         NW_Wml_1_3_ElementToken_img
#define TIMER_ELEMENT         NW_Wml_1_3_ElementToken_timer
#define ACCESS_ELEMENT        NW_Wml_1_3_ElementToken_access
#define META_ELEMENT          NW_Wml_1_3_ElementToken_meta
#define EMPHASIS_ELEMENT      NW_Wml_1_3_ElementToken_em
#define STRONG_ELEMENT        NW_Wml_1_3_ElementToken_strong
#define ITALIC_ELEMENT        NW_Wml_1_3_ElementToken_i
#define BOLD_ELEMENT          NW_Wml_1_3_ElementToken_b
#define UNDERLINE_ELEMENT     NW_Wml_1_3_ElementToken_u
#define BIG_ELEMENT           NW_Wml_1_3_ElementToken_big
#define SMALL_ELEMENT         NW_Wml_1_3_ElementToken_small
#define BREAK_ELEMENT         NW_Wml_1_3_ElementToken_br
#define P_ELEMENT             NW_Wml_1_3_ElementToken_p
#define TABLE_ELEMENT         NW_Wml_1_3_ElementToken_table
#define TR_ELEMENT            NW_Wml_1_3_ElementToken_tr
#define TD_ELEMENT            NW_Wml_1_3_ElementToken_td
#define POSTFIELD_ELEMENT     NW_Wml_1_3_ElementToken_postfield
#define PRE_ELEMENT           NW_Wml_1_3_ElementToken_pre

/* public attribute types */

#define UNKNOWN_ATTR              0
#define STR_VALUE                 1

#define ACCEPT_CHARSET_ATTR       NW_Wml_1_3_AttributeToken_accept_charset
#define ALIGN_ATTR                NW_Wml_1_3_AttributeToken_align
#define ALT_ATTR                  NW_Wml_1_3_AttributeToken_alt
#define CACHECONTROL_ATTR         NW_Wml_1_3_AttributeToken_cache_control
#define CLASS_ATTR                NW_Wml_1_3_AttributeToken_class
#define COLUMNS_ATTR              NW_Wml_1_3_AttributeToken_columns
#define CONTENT_ATTR              NW_Wml_1_3_AttributeToken_content
#define DOMAIN_ATTR               NW_Wml_1_3_AttributeToken_domain
#define EMPTYOK_ATTR              NW_Wml_1_3_AttributeToken_emptyok
#define ENCTYPE_ATTR              NW_Wml_1_3_AttributeToken_enctype
#define FORMAT_ATTR               NW_Wml_1_3_AttributeToken_format
#define FORUA_ATTR                NW_Wml_1_3_AttributeToken_forua
#define HEIGHT_ATTR               NW_Wml_1_3_AttributeToken_height
#define HREF_ATTR                 NW_Wml_1_3_AttributeToken_href
#define HSPACE_ATTR               NW_Wml_1_3_AttributeToken_hspace
#define HTTP_EQUIV_ATTR           NW_Wml_1_3_AttributeToken_http_equiv
#define ID_ATTR                   NW_Wml_1_3_AttributeToken_id
#define IVALUE_ATTR               NW_Wml_1_3_AttributeToken_ivalue
#define INAME_ATTR                NW_Wml_1_3_AttributeToken_iname
#define LABEL_ATTR                NW_Wml_1_3_AttributeToken_label
#define LOCALSRC_ATTR             NW_Wml_1_3_AttributeToken_localsrc
#define MAXLENGTH_ATTR            NW_Wml_1_3_AttributeToken_maxlength
#define METHOD_ATTR               NW_Wml_1_3_AttributeToken_method
#define MODE_ATTR                 NW_Wml_1_3_AttributeToken_mode
#define MULTIPLE_ATTR             NW_Wml_1_3_AttributeToken_multiple
#define NAME_ATTR                 NW_Wml_1_3_AttributeToken_name
#define NEWCONTEXT_ATTR           NW_Wml_1_3_AttributeToken_newcontext
#define ONENTERBACKWARD_ATTR      NW_Wml_1_3_AttributeToken_onenterbackward
#define ONENTERFORWARD_ATTR       NW_Wml_1_3_AttributeToken_onenterforward
#define ONPICK_ATTR               NW_Wml_1_3_AttributeToken_onpick
#define ONTIMER_ATTR              NW_Wml_1_3_AttributeToken_ontimer
#define OPTIONAL_ATTR             NW_Wml_1_3_AttributeToken_optional
#define ORDERED_ATTR              NW_Wml_1_3_AttributeToken_ordered
#define PATH_ATTR                 NW_Wml_1_3_AttributeToken_path
#define SCHEME_ATTR               NW_Wml_1_3_AttributeToken_scheme
#define SENDREFERER_ATTR          NW_Wml_1_3_AttributeToken_sendreferer
#define SIZE_ATTR                 NW_Wml_1_3_AttributeToken_size
#define SRC_ATTR                  NW_Wml_1_3_AttributeToken_src
#define TABINDEX_ATTR             NW_Wml_1_3_AttributeToken_tabindex
#define TITLE_ATTR                NW_Wml_1_3_AttributeToken_title
#define TYPE_ATTR                 NW_Wml_1_3_AttributeToken_type
#define VALUE_ATTR                NW_Wml_1_3_AttributeToken_value
#define VSPACE_ATTR               NW_Wml_1_3_AttributeToken_vspace
#define WIDTH_ATTR                NW_Wml_1_3_AttributeToken_width
#define XML_LANG_ATTR             NW_Wml_1_3_AttributeToken_xml_lang
#define XML_SPACE_ATTR            NW_Wml_1_3_AttributeToken_xml_space
#define ACCESSKEY_ATTR            NW_Wml_1_3_AttributeToken_accesskey

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /* WML_ELM_ATTR_H */
