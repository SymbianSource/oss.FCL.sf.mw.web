/*
* Copyright (c) 2000 - 2003 Nokia Corporation and/or its subsidiary(-ies).
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


/*****************************************************************

 Description:
   Definintions of commonly used HTTP constant type strings

******************************************************************/

#include "nwx_http_defs.h"

/* Content Type Strings */
const unsigned char HTTP_all_all_string[] = {"*/*"};
const unsigned char HTTP_text_all_string[] = {"text/*"};
const unsigned char HTTP_text_html_string[] = {"text/html"};
const unsigned char HTTP_text_plain_string[] = {"text/plain"};
const unsigned char HTTP_text_x_hdml_string[] = {"text/x-hdml"};
const unsigned char HTTP_text_x_ttml_string[] = {"text/x-ttml"};
const unsigned char HTTP_text_x_vCalendar_string[] = {"text/x-vCalendar"};
const unsigned char HTTP_text_x_vCard_string[] = {"text/x-vCard"};
const unsigned char HTTP_text_vnd_wap_wml_string[] = {"text/vnd.wap.wml"};
const unsigned char HTTP_text_vnd_wap_wmlscript_string[] = {"text/vnd.wap.wmlscript"};
const unsigned char HTTP_application_x_javascript_string[] = {"application/x-javascript"};
const unsigned char HTTP_text_javascript_string[] = {"text/javascript"};
const unsigned char HTTP_text_ecmascript_string[] = {"text/ecmascript"};
const unsigned char HTTP_text_vnd_wap_wta_event_string[] = {"text/vnd.wap.wta-event"};
const unsigned char HTTP_multipart_all_string[] = {"multipart/*"};
const unsigned char HTTP_multipart_mixed_string[] = {"multipart/mixed"};
const unsigned char HTTP_multipart_form_data_string[] = {"multipart/form-data"};
const unsigned char HTTP_multipart_byteranges_string[] = {"multipart/byterantes"};
const unsigned char HTTP_multipart_alternative_string[] = {"multipart/alternative"};
const unsigned char HTTP_application_all_string[] = {"application/*"};
const unsigned char HTTP_application_java_vm_string[] = {"application/java-vm"};
const unsigned char HTTP_application_x_www_form_urlencoded_string[] = {"application/x-www-form-urlencoded"};
const unsigned char HTTP_application_x_hdmlc_string[] = {"application/x-hdmlc"};
const unsigned char HTTP_application_vnd_wap_wml_plus_xml_string[] = {"application/vnd.wap.wml+xml"};
const unsigned char HTTP_application_vnd_wap_wmlc_string[] = {"application/vnd.wap.wmlc"};
const unsigned char HTTP_application_vnd_wap_wmlscriptc_string[] = {"application/vnd.wap.wmlscriptc"};
const unsigned char HTTP_application_vnd_wap_wta_eventc_string[] = {"application/vnd.wap.wta-eventc"};
const unsigned char HTTP_application_vnd_wap_uaprof_string[] = {"application/vnd.wap.uaprof"};
const unsigned char HTTP_application_vnd_wap_wtls_ca_certificate_string[] = {"application/vnd.wap.wtls-ca-certificate"};
const unsigned char HTTP_application_vnd_wap_wtls_user_certificate_string[] = {"application/vnd.wap.wtls-user-certificate"};
const unsigned char HTTP_application_x_x509_ca_cert_string[] = {"application/x-x509-ca-cert"};
const unsigned char HTTP_application_x_x509_user_cert_string[] = {"application/x-x509-user-cert"};
const unsigned char HTTP_image_string[] = {"image"};
const unsigned char HTTP_image_all_string[] = {"image/*"};
const unsigned char HTTP_image_bmp_string[] = {"image/bmp"};
const unsigned char HTTP_image_gif_string[] = {"image/gif"};
const unsigned char HTTP_image_jpeg_string[] = {"image/jpeg"};
const unsigned char HTTP_image_tiff_string[] = {"image/tiff"};
const unsigned char HTTP_image_png_string[] = {"image/png"};
const unsigned char HTTP_image_vnd_wap_wbmp_string[] = {"image/vnd.wap.wbmp"};
const unsigned char HTTP_image_vnd_nokia_ota_bitmap_string[] = {"image/vnd.nokia.ota-bitmap"};
const unsigned char HTTP_image_x_ota_bitmap_string[] = {"image/x-ota-bitmap"};
const unsigned char HTTP_application_vnd_wap_multipart_all_string[] = {"application/vnd.wap.multipart.*"};
const unsigned char HTTP_application_vnd_wap_multipart_mixed_string[] = {"application/vnd.wap.multipart.mixed"};
const unsigned char HTTP_application_vnd_wap_multipart_form_data_string[] = {"application/vnd.wap.multipart.form-data"};
const unsigned char HTTP_application_vnd_wap_multipart_byteranges_string[] = {"application/vnd.wap.multipart.byteranges"};
const unsigned char HTTP_application_vnd_wap_multipart_alternative_string[] = {"application/vnd.wap.multipart.alternative"};
const unsigned char HTTP_application_xml_string[] = {"application/xml"};
const unsigned char HTTP_text_xml_string[] = {"text/xml"};
const unsigned char HTTP_application_vnd_wap_wbxml_string[] = {"application/vnd.wap.wbxml"};
const unsigned char HTTP_application_x_x968_cross_cert_string[] = {"application/x-x968-cross-cert"};
const unsigned char HTTP_application_x_x968_ca_cert_string[] = {"application/x-x968-ca-cert"};
const unsigned char HTTP_application_x_x968_user_cert_string[] = {"application/x-x968-user-cert"};
const unsigned char HTTP_text_vnd_wap_si_string[] = {"text/vnd.wap.si"};
const unsigned char HTTP_application_vnd_wap_sic_string[] = {"application/vnd.wap.sic"};
const unsigned char HTTP_text_vnd_wap_sl_string[] = {"text/vnd.wap.sl"};
const unsigned char HTTP_application_vnd_wap_slc_string[] = {"application/vnd.wap.slc"};
const unsigned char HTTP_text_vnd_wap_co_string[] = {"text/vnd.wap.co"};
const unsigned char HTTP_application_vnd_wap_coc_string[] = {"application/vnd.wap.coc"};
const unsigned char HTTP_application_vnd_wap_multipart_related_string[] = {"application/vnd.wap.multipart.related"};
const unsigned char HTTP_application_vnd_wap_sia_string[] = {"application/vnd.wap.sia"};
const unsigned char HTTP_text_vnd_wap_connectivity_xml_string[] = {"text/vnd.wap.connectivity-xml"};
const unsigned char HTTP_application_vnd_wap_connectivity_wbxml_string[] = {"application/vnd.wap.connectivity-wbxml"};
const unsigned char HTTP_application_pkcs7_mime_string[] = {"application/pkcs7-mime"};
const unsigned char HTTP_application_vnd_wap_hashed_certificate_string[] = {"application/vnd.wap.hashed-certificate"};
const unsigned char HTTP_application_vnd_wap_signed_certificate_string[] = {"application/vnd.wap.signed-certificate"};
const unsigned char HTTP_application_vnd_wap_cert_response_string[] = {"application/vnd.wap.cert-response"};
const unsigned char HTTP_application_xhtml_xml_string[] = {"application/xhtml+xml"};
const unsigned char HTTP_application_wml_xml_string[] = {"application/wml+xml"};
const unsigned char HTTP_text_css_string[] = {"text/css"};
const unsigned char HTTP_application_vnd_wap_mms_message_string[] = {"application/vnd.wap.mms-message"};
const unsigned char HTTP_application_vnd_wap_rollover_certificate_string[] = {"application/vnd.wap.rollover-certificate"};
const unsigned char HTTP_text_x_co_desc_string[] = {"text/x-co-desc"};
const unsigned char HTTP_application_vnd_wap_xhtml_xml_string[] = {"application/vnd.wap.xhtml+xml"};

const unsigned char HTTP_application_vnd_wap_syncml_wbxml_string[] = {"application/vnd.syncml+wbxml"};
const unsigned char HTTP_application_vnd_nokia_ringing_tone_string[] = {"application/vnd.nokia.ringing-tone"};
const unsigned char HTTP_application_x_nokiagamedata_string[] = {"application/x-NokiaGameData"};

/* Nokia defined content types string for ota push */
const unsigned char HTTP_application_wap_prov_browser_settings_string[] = {"application/x-wap-prov.browser-settings"};
const unsigned char HTTP_application_wap_prov_browser_bookmarks_string[] = {"application/x-wap-prov.browser-bookmarks"};

/* Special content type string to represent encoded xhtml resulting
   from save-to-file operation.
*/
const unsigned char HTTP_application_xhtml_xml_saved_string[] = {"application/xhtml_xml_saved"};

/* Special content type to represent wmlc resulting
   from save-to-file operation.
*/
const unsigned char HTTP_application_wmlc_saved_string[] = {"application/wmlc_saved"};

/* Special content type from save-to-file operation.
*/
const unsigned char HTTP_application_saved_string[] = {"application/x-browser-savedpage"};

#ifdef __TEST_TESTHARNESS_ENABLED
const unsigned char HTTP_application_x_nokia_test_harness_string[] = {"text/xml"};
#endif

// Charset strings
const char NW_Utf8_CharsetStr[]         = {"utf-8"};
const char NW_UsAscii_CharsetStr[]      = {"us-ascii"};
const char NW_Iso10646Ucs2_CharsetStr[] = {"iso-10646-ucs-2"};
const char NW_Ucs2_CharsetStr[]         = {"ucs-2"};
const char NW_Big5_CharsetStr[]         = {"big5"};
const char NW_Gb2312_CharsetStr[]       = {"gb2312"};
const char NW_Windows1255_CharsetStr[]  = {"windows-1255"};
const char NW_Windows1256_CharsetStr[]  = {"windows-1256"};
const char NW_Iso88591_CharsetStr[]     = {"iso-8859-1"};
const char NW_Iso88592_CharsetStr[]     = {"iso-8859-2"};
const char NW_Iso88593_CharsetStr[]     = {"iso-8859-3"};
const char NW_Iso88594_CharsetStr[]     = {"iso-8859-4"};
const char NW_Iso88595_CharsetStr[]     = {"iso-8859-5"};
const char NW_Iso88596_CharsetStr[]     = {"iso-8859-6"};
const char NW_Iso88597_CharsetStr[]     = {"iso-8859-7"};
const char NW_Iso88598_CharsetStr[]     = {"iso-8859-8"};
const char NW_Iso88598i_CharsetStr[]    = {"iso-8859-8i"};
const char NW_Iso88599_CharsetStr[]     = {"iso-8859-9"};
const char NW_Iso885915_CharsetStr[]     = {"iso-8859-15"};
const char NW_Windows1250_CharsetStr[]  = {"windows-1250"};
const char NW_Windows1251_CharsetStr[]  = {"windows-1251"};
const char NW_Windows1252_CharsetStr[]  = {"windows-1252"};
const char NW_Windows1253_CharsetStr[]  = {"windows-1253"};
const char NW_Windows1254_CharsetStr[]  = {"windows-1254"};
const char NW_Windows1257_CharsetStr[]  = {"windows-1257"};
const char NW_Tis_620_CharsetStr[]      = {"tis-620"};
const char NW_Unicode_CharsetStr[]      = {"unicode"};
const char NW_ShiftJis_CharsetStr[]     = {"shift_jis"};  // standard MIME name
const char NW_ShiftJis_CharsetStr2[]    = {"shift-jis"};  // common alias / misspelling
const char NW_X_SJis_CharsetStr[]       = {"x-sjis"};
const char NW_JisX0201_1997_CharsetStr[]= {"jis_x0201-1997"};
const char NW_JisX0208_1997_CharsetStr[]= {"jis_x0208-1997"};
const char NW_EucJp_CharsetStr[]        = {"euc-jp"};
const char NW_X_EucJp_CharsetStr[]      = {"x-euc-jp"};
const char NW_Iso2022Jp_CharsetStr[]    = {"iso-2022-jp"};
const char NW_Windows874_CharsetStr[]   = {"windows-874"};
const char NW_Koi8r_CharsetStr[]        = {"koi8-r"};
const char NW_Koi8u_CharsetStr[]        = {"koi8-u"};

 const char NW_EucKr_CharsetStr[]        = {"euc-kr"};
 const char NW_ksc5601_1987_CharsetStr[] = {"ks_c_5601-1987"};

/* End Content Type */


