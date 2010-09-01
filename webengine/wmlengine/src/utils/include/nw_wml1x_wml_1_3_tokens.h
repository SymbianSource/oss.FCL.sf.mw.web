/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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

/*
** WARNING
**
** DO NOT EDIT - THIS CODE IS AUTOMATICALLY GENERATED
**               FROM A DATA FILE BY THE DICTIONARY CREATION PROGRAM
**
** This file generated on Wed Apr 24 14:59:12 2002
**                        (coordinated universal time)
**
** Command line: r:\Shared\shaun\dict_creator\dict_creator.exe wml_1_3.dict wml_1_3_tokendict.c wml_1_3_tokendict.h
*/
/*
** This file was generated by the dictionary creator but includes some hand-edits.
** If you need any help to edit this file please see Shaun Keller/Frank Rchichi/Ramesh Bapanapalli.
*/

#ifndef HEADER_GUARD_nw_wml1x_wml_1_3_tokens_h
#define HEADER_GUARD_nw_wml1x_wml_1_3_tokens_h


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


#define NW_Wml_1_0_PublicId 0x02
#define NW_Wml_1_1_PublicId 0x04
#define NW_Wml_1_2_PublicId 0x09
#define NW_Wml_1_3_PublicId 0x0A

typedef enum NW_Wml_1_3_ElementToken_0_e{
	NW_Wml_1_3_ElementToken_pre = 0x001b,
	NW_Wml_1_3_ElementToken_a = 0x001c,
	NW_Wml_1_3_ElementToken_td = 0x001d,
	NW_Wml_1_3_ElementToken_tr = 0x001e,
	NW_Wml_1_3_ElementToken_table = 0x001f,
	NW_Wml_1_3_ElementToken_p = 0x0020,
	NW_Wml_1_3_ElementToken_postfield = 0x0021,
	NW_Wml_1_3_ElementToken_anchor = 0x0022,
	NW_Wml_1_3_ElementToken_access = 0x0023,
	NW_Wml_1_3_ElementToken_b = 0x0024,
	NW_Wml_1_3_ElementToken_big = 0x0025,
	NW_Wml_1_3_ElementToken_br = 0x0026,
	NW_Wml_1_3_ElementToken_card = 0x0027,
	NW_Wml_1_3_ElementToken_do = 0x0028,
	NW_Wml_1_3_ElementToken_em = 0x0029,
	NW_Wml_1_3_ElementToken_fieldset = 0x002a,
	NW_Wml_1_3_ElementToken_go = 0x002b,
	NW_Wml_1_3_ElementToken_head = 0x002c,
	NW_Wml_1_3_ElementToken_i = 0x002d,
	NW_Wml_1_3_ElementToken_img = 0x002e,
	NW_Wml_1_3_ElementToken_input = 0x002f,
	NW_Wml_1_3_ElementToken_meta = 0x0030,
	NW_Wml_1_3_ElementToken_noop = 0x0031,
	NW_Wml_1_3_ElementToken_prev = 0x0032,
	NW_Wml_1_3_ElementToken_onevent = 0x0033,
	NW_Wml_1_3_ElementToken_optgroup = 0x0034,
	NW_Wml_1_3_ElementToken_option = 0x0035,
	NW_Wml_1_3_ElementToken_refresh = 0x0036,
	NW_Wml_1_3_ElementToken_select = 0x0037,
	NW_Wml_1_3_ElementToken_small = 0x0038,
	NW_Wml_1_3_ElementToken_strong = 0x0039,
	NW_Wml_1_3_ElementToken_template = 0x003b,
	NW_Wml_1_3_ElementToken_timer = 0x003c,
	NW_Wml_1_3_ElementToken_u = 0x003d,
	NW_Wml_1_3_ElementToken_setvar = 0x003e,
	NW_Wml_1_3_ElementToken_wml = 0x003f
}NW_Wml_1_3_ElementToken_0_t;


typedef enum NW_Wml_1_3_ElementToken_1_e{
	NW_Wml_1_3_ElementToken_zzzunknown = 0x0118
}NW_Wml_1_3_ElementToken_1_t;


typedef enum NW_Wml_1_3_AttributeToken_0_e{
	NW_Wml_1_3_AttributeToken_accept_charset = 0x0005,
	NW_Wml_1_3_AttributeToken_align_bottom = 0x0006,
	NW_Wml_1_3_AttributeToken_align_center = 0x0007,
	NW_Wml_1_3_AttributeToken_align_left = 0x0008,
	NW_Wml_1_3_AttributeToken_align_middle = 0x0009,
	NW_Wml_1_3_AttributeToken_align_right = 0x000a,
	NW_Wml_1_3_AttributeToken_align_top = 0x000b,
	NW_Wml_1_3_AttributeToken_alt = 0x000c,
	NW_Wml_1_3_AttributeToken_content = 0x000d,
	NW_Wml_1_3_AttributeToken_domain = 0x000f,
	NW_Wml_1_3_AttributeToken_emptyok_false = 0x0010,
	NW_Wml_1_3_AttributeToken_emptyok_true = 0x0011,
	NW_Wml_1_3_AttributeToken_format = 0x0012,
	NW_Wml_1_3_AttributeToken_height = 0x0013,
	NW_Wml_1_3_AttributeToken_hspace = 0x0014,
	NW_Wml_1_3_AttributeToken_ivalue = 0x0015,
	NW_Wml_1_3_AttributeToken_iname = 0x0016,
	NW_Wml_1_3_AttributeToken_label = 0x0018,
	NW_Wml_1_3_AttributeToken_localsrc = 0x0019,
	NW_Wml_1_3_AttributeToken_maxlength = 0x001a,
	NW_Wml_1_3_AttributeToken_method_get = 0x001b,
	NW_Wml_1_3_AttributeToken_method_post = 0x001c,
	NW_Wml_1_3_AttributeToken_mode_nowrap = 0x001d,
	NW_Wml_1_3_AttributeToken_mode_wrap = 0x001e,
	NW_Wml_1_3_AttributeToken_multiple_false = 0x001f,
	NW_Wml_1_3_AttributeToken_multiple_true = 0x0020,
	NW_Wml_1_3_AttributeToken_name = 0x0021,
	NW_Wml_1_3_AttributeToken_newcontext_false = 0x0022,
	NW_Wml_1_3_AttributeToken_newcontext_true = 0x0023,
	NW_Wml_1_3_AttributeToken_onpick = 0x0024,
	NW_Wml_1_3_AttributeToken_onenterbackward = 0x0025,
	NW_Wml_1_3_AttributeToken_onenterforward = 0x0026,
	NW_Wml_1_3_AttributeToken_ontimer = 0x0027,
	NW_Wml_1_3_AttributeToken_optional_false = 0x0028,
	NW_Wml_1_3_AttributeToken_optional_true = 0x0029,
	NW_Wml_1_3_AttributeToken_path = 0x002a,
	NW_Wml_1_3_AttributeToken_scheme = 0x002e,
	NW_Wml_1_3_AttributeToken_sendreferer_false = 0x002f,
	NW_Wml_1_3_AttributeToken_sendreferer_true = 0x0030,
	NW_Wml_1_3_AttributeToken_size = 0x0031,
	NW_Wml_1_3_AttributeToken_src = 0x0032,
	NW_Wml_1_3_AttributeToken_ordered_true = 0x0033,
	NW_Wml_1_3_AttributeToken_ordered_false = 0x0034,
	NW_Wml_1_3_AttributeToken_tabindex = 0x0035,
	NW_Wml_1_3_AttributeToken_title = 0x0036,
	NW_Wml_1_3_AttributeToken_type = 0x0037,
	NW_Wml_1_3_AttributeToken_type_accept = 0x0038,
	NW_Wml_1_3_AttributeToken_type_delete = 0x0039,
	NW_Wml_1_3_AttributeToken_type_help = 0x003a,
	NW_Wml_1_3_AttributeToken_type_password = 0x003b,
	NW_Wml_1_3_AttributeToken_type_onpick = 0x003c,
	NW_Wml_1_3_AttributeToken_type_onenterbackward = 0x003d,
	NW_Wml_1_3_AttributeToken_type_onenterforward = 0x003e,
	NW_Wml_1_3_AttributeToken_type_ontimer = 0x003f,
	NW_Wml_1_3_AttributeToken_zzzunknown = 0x0040,
	NW_Wml_1_3_AttributeToken_type_options = 0x0045,
	NW_Wml_1_3_AttributeToken_type_prev = 0x0046,
	NW_Wml_1_3_AttributeToken_type_reset = 0x0047,
	NW_Wml_1_3_AttributeToken_type_text = 0x0048,
	NW_Wml_1_3_AttributeToken_type_vnd_ = 0x0049,
	NW_Wml_1_3_AttributeToken_href = 0x004a,
	NW_Wml_1_3_AttributeToken_href_http___ = 0x004b,
	NW_Wml_1_3_AttributeToken_href_https___ = 0x004c,
	NW_Wml_1_3_AttributeToken_value = 0x004d,
	NW_Wml_1_3_AttributeToken_vspace = 0x004e,
	NW_Wml_1_3_AttributeToken_width = 0x004f,
	NW_Wml_1_3_AttributeToken_xml_lang = 0x0050,
	NW_Wml_1_3_AttributeToken_align = 0x0052,
	NW_Wml_1_3_AttributeToken_columns = 0x0053,
	NW_Wml_1_3_AttributeToken_class = 0x0054,
	NW_Wml_1_3_AttributeToken_id = 0x0055,
	NW_Wml_1_3_AttributeToken_forua_false = 0x0056,
	NW_Wml_1_3_AttributeToken_forua_true = 0x0057,
	NW_Wml_1_3_AttributeToken_src_http___ = 0x0058,
	NW_Wml_1_3_AttributeToken_src_https___ = 0x0059,
	NW_Wml_1_3_AttributeToken_http_equiv = 0x005a,
	NW_Wml_1_3_AttributeToken_http_equiv_Content_Type = 0x005b,
	NW_Wml_1_3_AttributeToken_content_application_vnd_wap_wmlc_charset_ = 0x005c,
	NW_Wml_1_3_AttributeToken_http_equiv_Expires = 0x005d,
	NW_Wml_1_3_AttributeToken_accesskey = 0x005e,
	NW_Wml_1_3_AttributeToken_enctype = 0x005f,
	NW_Wml_1_3_AttributeToken_enctype_application_x_www_form_urlencoded = 0x0060,
	NW_Wml_1_3_AttributeToken_enctype_multipart_form_data = 0x0061,
	NW_Wml_1_3_AttributeToken_xml_space_preserve = 0x0062,
	NW_Wml_1_3_AttributeToken_xml_space_default = 0x0063,
	NW_Wml_1_3_AttributeToken_cache_control_no_cache = 0x0064,
	NW_Wml_1_3_AttributeToken_cache_control = 0x0065,
	NW_Wml_1_3_AttributeToken_emptyok = 0x0066,
	NW_Wml_1_3_AttributeToken_forua = 0x0067,
	NW_Wml_1_3_AttributeToken_method = 0x0068,
	NW_Wml_1_3_AttributeToken_mode = 0x0069,
	NW_Wml_1_3_AttributeToken_multiple = 0x006a,
	NW_Wml_1_3_AttributeToken_optional = 0x006b,
	NW_Wml_1_3_AttributeToken_newcontext = 0x006c,
	NW_Wml_1_3_AttributeToken_ordered = 0x006d,
	NW_Wml_1_3_AttributeToken_sendreferer = 0x006e,
	NW_Wml_1_3_AttributeToken_xml_space = 0x006f,
	NW_Wml_1_3_AttributeToken__com_ = 0x0085,
	NW_Wml_1_3_AttributeToken__edu_ = 0x0086,
	NW_Wml_1_3_AttributeToken__net_ = 0x0087,
	NW_Wml_1_3_AttributeToken__org_ = 0x0088,
	NW_Wml_1_3_AttributeToken_accept = 0x0089,
	NW_Wml_1_3_AttributeToken_bottom = 0x008a,
	NW_Wml_1_3_AttributeToken_clear = 0x008b,
	NW_Wml_1_3_AttributeToken_delete = 0x008c,
	NW_Wml_1_3_AttributeToken_help = 0x008d,
	NW_Wml_1_3_AttributeToken_http___ = 0x008e,
	NW_Wml_1_3_AttributeToken_http___www_ = 0x008f,
	NW_Wml_1_3_AttributeToken_https___ = 0x0090,
	NW_Wml_1_3_AttributeToken_https___www_ = 0x0091,
	NW_Wml_1_3_AttributeToken_middle = 0x0093,
	NW_Wml_1_3_AttributeToken_nowrap = 0x0094,
	NW_Wml_1_3_AttributeValueToken_onpick = 0x0095,
	NW_Wml_1_3_AttributeValueToken_onenterbackward = 0x0096,
	NW_Wml_1_3_AttributeValueToken_onenterforward = 0x0097,
	NW_Wml_1_3_AttributeValueToken_ontimer = 0x0098,
	NW_Wml_1_3_AttributeToken_options = 0x0099,
	NW_Wml_1_3_AttributeToken_password = 0x009a,
	NW_Wml_1_3_AttributeToken_reset = 0x009b,
	NW_Wml_1_3_AttributeToken_text = 0x009d,
	NW_Wml_1_3_AttributeToken_top = 0x009e,
	NW_Wml_1_3_AttributeToken_unknown = 0x009f,
	NW_Wml_1_3_AttributeToken_wrap = 0x00a0,
	NW_Wml_1_3_AttributeToken_www_ = 0x00a1
}NW_Wml_1_3_AttributeToken_0_t;

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif

/*
** WARNING
**
** DO NOT EDIT - THIS CODE IS AUTOMATICALLY GENERATED
**               FROM A DATA FILE BY THE DICTIONARY CREATION PROGRAM
**
** This file generated on Wed Apr 24 14:59:12 2002
**                        (coordinated universal time)
**
** Command line: r:\Shared\shaun\dict_creator\dict_creator.exe wml_1_3.dict wml_1_3_tokendict.c wml_1_3_tokendict.h
*/
