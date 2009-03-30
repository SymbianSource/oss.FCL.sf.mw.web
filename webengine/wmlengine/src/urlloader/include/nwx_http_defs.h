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
* Description:  Handling of general url loading
*
*/


#ifndef NW_HTTP_DEFS_H
#define NW_HTTP_DEFS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Content Type Strings */
extern const unsigned char HTTP_all_all_string[];
extern const unsigned char HTTP_text_all_string[];
extern const unsigned char HTTP_text_html_string[];
extern const unsigned char HTTP_text_plain_string[];
extern const unsigned char HTTP_text_x_hdml_string[];
extern const unsigned char HTTP_text_x_ttml_string[];
extern const unsigned char HTTP_text_x_vCalendar_string[];
extern const unsigned char HTTP_text_x_vCard_string[];
extern const unsigned char HTTP_text_vnd_wap_wml_string[];
extern const unsigned char HTTP_text_vnd_wap_wmlscript_string[];
extern const unsigned char HTTP_application_x_javascript_string[];
extern const unsigned char HTTP_text_javascript_string[];
extern const unsigned char HTTP_text_ecmascript_string[];

extern const unsigned char HTTP_text_vnd_wap_wta_event_string[];
extern const unsigned char HTTP_multipart_all_string[];
extern const unsigned char HTTP_multipart_mixed_string[];
extern const unsigned char HTTP_multipart_form_data_string[];
extern const unsigned char HTTP_multipart_byteranges_string[];
extern const unsigned char HTTP_multipart_alternative_string[];
extern const unsigned char HTTP_application_all_string[];
extern const unsigned char HTTP_application_java_vm_string[];
extern const unsigned char HTTP_application_x_www_form_urlencoded_string[];
extern const unsigned char HTTP_application_x_hdmlc_string[];
extern const unsigned char HTTP_application_vnd_wap_wml_plus_xml_string[];
extern const unsigned char HTTP_application_vnd_wap_wmlc_string[];
extern const unsigned char HTTP_application_vnd_wap_wmlscriptc_string[];
extern const unsigned char HTTP_application_vnd_wap_wta_eventc_string[];
extern const unsigned char HTTP_application_vnd_wap_uaprof_string[];
extern const unsigned char HTTP_application_vnd_wap_wtls_ca_certificate_string[];
extern const unsigned char HTTP_application_vnd_wap_wtls_user_certificate_string[];
extern const unsigned char HTTP_application_x_x509_ca_cert_string[];
extern const unsigned char HTTP_application_x_x509_user_cert_string[];
extern const unsigned char HTTP_image_string[];
extern const unsigned char HTTP_image_all_string[];
extern const unsigned char HTTP_image_bmp_string[];
extern const unsigned char HTTP_image_gif_string[];
extern const unsigned char HTTP_image_jpeg_string[];
extern const unsigned char HTTP_image_tiff_string[];
extern const unsigned char HTTP_image_png_string[];
extern const unsigned char HTTP_image_vnd_wap_wbmp_string[];
extern const unsigned char HTTP_image_vnd_nokia_ota_bitmap_string[];
extern const unsigned char HTTP_image_x_ota_bitmap_string[];
extern const unsigned char HTTP_application_vnd_wap_multipart_all_string[];
extern const unsigned char HTTP_application_vnd_wap_multipart_mixed_string[];
extern const unsigned char HTTP_application_vnd_wap_multipart_form_data_string[];
extern const unsigned char HTTP_application_vnd_wap_multipart_byteranges_string[];
extern const unsigned char HTTP_application_vnd_wap_multipart_alternative_string[];
extern const unsigned char HTTP_application_xml_string[];
extern const unsigned char HTTP_text_xml_string[];
extern const unsigned char HTTP_application_vnd_wap_wbxml_string[];
extern const unsigned char HTTP_application_x_x968_cross_cert_string[];
extern const unsigned char HTTP_application_x_x968_ca_cert_string[];
extern const unsigned char HTTP_application_x_x968_user_cert_string[];
extern const unsigned char HTTP_text_vnd_wap_si_string[];
extern const unsigned char HTTP_application_vnd_wap_sic_string[];
extern const unsigned char HTTP_text_vnd_wap_sl_string[];
extern const unsigned char HTTP_application_vnd_wap_slc_string[];
extern const unsigned char HTTP_text_vnd_wap_co_string[];
extern const unsigned char HTTP_application_vnd_wap_coc_string[];
extern const unsigned char HTTP_application_vnd_wap_multipart_related_string[];
extern const unsigned char HTTP_application_vnd_wap_sia_string[];
extern const unsigned char HTTP_text_vnd_wap_connectivity_xml_string[];
extern const unsigned char HTTP_application_vnd_wap_connectivity_wbxml_string[];
extern const unsigned char HTTP_application_pkcs7_mime_string[];
extern const unsigned char HTTP_application_vnd_wap_hashed_certificate_string[];
extern const unsigned char HTTP_application_vnd_wap_signed_certificate_string[];
extern const unsigned char HTTP_application_vnd_wap_cert_response_string[];
extern const unsigned char HTTP_application_xhtml_xml_string[];
extern const unsigned char HTTP_application_wml_xml_string[];
extern const unsigned char HTTP_text_css_string[];
extern const unsigned char HTTP_application_vnd_wap_mms_message_string[];
extern const unsigned char HTTP_application_vnd_wap_rollover_certificate_string[];
extern const unsigned char HTTP_text_x_co_desc_string[];
extern const unsigned char HTTP_application_vnd_wap_xhtml_xml_string[];

extern const unsigned char HTTP_application_vnd_wap_syncml_wbxml_string[];
extern const unsigned char HTTP_application_vnd_nokia_ringing_tone_string[];
extern const unsigned char HTTP_application_x_nokiagamedata_string[];

/* Nokia defined content types string for ota push */
extern const unsigned char HTTP_application_wap_prov_browser_settings_string[];
extern const unsigned char HTTP_application_wap_prov_browser_bookmarks_string[];

extern const unsigned char HTTP_application_xhtml_xml_saved_string[];
extern const unsigned char HTTP_application_wmlc_saved_string[];
extern const unsigned char HTTP_application_saved_string[];

#ifdef __TEST_TESTHARNESS_ENABLED
extern const unsigned char HTTP_application_x_nokia_test_harness_string[];
#endif


/* End Content Type */

// Charset strings
extern const char NW_Utf8_CharsetStr[];
extern const char NW_UsAscii_CharsetStr[];
extern const char NW_Iso10646Ucs2_CharsetStr[];
extern const char NW_Ucs2_CharsetStr[];
extern const char NW_Big5_CharsetStr[];
extern const char NW_Gb2312_CharsetStr[];
extern const char NW_Windows1255_CharsetStr[];
extern const char NW_Windows1256_CharsetStr[];
extern const char NW_Iso88591_CharsetStr[];
extern const char NW_Iso88592_CharsetStr[];
extern const char NW_Iso88593_CharsetStr[];
extern const char NW_Iso88594_CharsetStr[];
extern const char NW_Iso88595_CharsetStr[];
extern const char NW_Iso88596_CharsetStr[];
extern const char NW_Iso88597_CharsetStr[];
extern const char NW_Iso88598_CharsetStr[];
extern const char NW_Iso88598i_CharsetStr[];
extern const char NW_Iso88599_CharsetStr[];
extern const char NW_Iso885915_CharsetStr[];
extern const char NW_Windows1250_CharsetStr[];
extern const char NW_Windows1251_CharsetStr[];
extern const char NW_Windows1252_CharsetStr[];
extern const char NW_Windows1253_CharsetStr[];
extern const char NW_Windows1254_CharsetStr[];
extern const char NW_Windows1257_CharsetStr[];
extern const char NW_Tis_620_CharsetStr[];
extern const char NW_Unicode_CharsetStr[];
extern const char NW_ShiftJis_CharsetStr[];
extern const char NW_ShiftJis_CharsetStr2[];
extern const char NW_X_SJis_CharsetStr[];
extern const char NW_JisX0201_1997_CharsetStr[];
extern const char NW_JisX0208_1997_CharsetStr[];
extern const char NW_EucJp_CharsetStr[];
extern const char NW_X_EucJp_CharsetStr[];
extern const char NW_Iso2022Jp_CharsetStr[];
extern const char NW_Windows874_CharsetStr[];
extern const char NW_Koi8r_CharsetStr[];
extern const char NW_Koi8u_CharsetStr[];


/* Character set */
#define HTTP_gb2312                 0x07E9
#define HTTP_big5                   0x07EA
#define HTTP_iso_10646_ucs_2        0x03E8
#define HTTP_iso_8859_1             0x04
#define HTTP_iso_8859_2             0x05
#define HTTP_iso_8859_3             0x06
#define HTTP_iso_8859_4             0x07
#define HTTP_iso_8859_5             0x08
#define HTTP_iso_8859_6             0x09
#define HTTP_iso_8859_7             0x0A
#define HTTP_iso_8859_8             0x0B
#define HTTP_iso_8859_9             0x0C
#define HTTP_shift_JIS              0x11
#define HTTP_us_ascii               0x03
#define HTTP_utf_8                  0x6A
#define HTTP_utf_16                 1015
#define HTTP_utf_7                  1012
#define HTTP_utf_16be               1013
#define HTTP_utf_16le               1014
#define HTTP_windows_1255           0x08CF
#define HTTP_windows_1256           0x08D0
#define HTTP_windows_1250           0x08CA
#define HTTP_windows_1251           0x08CB
#define HTTP_windows_1253           0x08CD
#define HTTP_windows_1254           0x08CE
#define HTTP_windows_1257           0x08D1
#define HTTP_tis_620                0x08D3
#define HTTP_jis_x0201_1997         0x0F
#define HTTP_jis_x0208_1997         0x3F
#define HTTP_euc_jp                 0x12
#define HTTP_iso_2022_jp            0x27
#define HTTP_windows_874            0x07E0
#define HTTP_Koi8_r                 0x0824
#define HTTP_Koi8_u                 0x0828

                 
#define HTTP_undefined              0xFD  /*  Nokia specific thingie */
#define HTTP_unknown                0xFE  /*  Nokia specific thingie */

/* HTTP Headers names */
#define HTTP_ANY                    0x00
#define HTTP_Accept                 0x80  
#define HTTP_AcceptCharset          0x81
#define HTTP_AcceptEncoding         0x82
#define HTTP_AcceptLanguage         0x83
#define HTTP_AcceptRanges           0x84
#define HTTP_Age                    0x85
#define HTTP_Allow                  0x86
#define HTTP_Authorization          0x87
#define HTTP_CacheControl           0x88
#define HTTP_Connection             0x89
#define HTTP_ContentBase            0x8A
#define HTTP_ContentEncoding        0x8B
#define HTTP_ContentLanguage        0x8C
#define HTTP_ContentLength          0x8D
#define HTTP_ContentLocation        0x8E
#define HTTP_ContentMD5             0x8F
#define HTTP_ContentRange           0x90
#define HTTP_ContentType            0x91
#define HTTP_Date                   0x92
#define HTTP_Etag                   0x93
#define HTTP_Expires                0x94
#define HTTP_From                   0x95
#define HTTP_Host                   0x96
#define HTTP_IfModifiedSince        0x97
#define HTTP_IfMatch                0x98
#define HTTP_IfNoneMatch            0x99
#define HTTP_IfRange                0x9A
#define HTTP_IfUnmodifiedSince      0x9B
#define HTTP_Location               0x9C
#define HTTP_LastModified           0x9D
#define HTTP_MaxForwards            0x9E
#define HTTP_Pragma                 0x9F
#define HTTP_ProxyAuthenticate      0xA0
#define HTTP_ProxyAuthorization     0xA1
#define HTTP_Public                 0xA2
#define HTTP_Range                  0xA3
#define HTTP_Referer                0xA4
#define HTTP_RetryAfter             0xA5
#define HTTP_Server                 0xA6
#define HTTP_TransferEncoding       0xA7
#define HTTP_Upgrade                0xA8
#define HTTP_UserAgent              0xA9
#define HTTP_Vary                   0xAA
#define HTTP_Via                    0xAB
#define HTTP_Warning                0xAC
#define HTTP_WWWAuthenticate        0xAD
#define HTTP_Content_Disposition    0xAE
#define HTTP_ApplicationId          0xAF
#define HTTP_ContentUri             0xB0
#define HTTP_InitiatorUri           0xB1
#define HTTP_AcceptApplication      0xB2
#define HTTP_BearerIndication       0xB3
#define HTTP_PushFlag               0xB4
#define HTTP_UAProf                 0xB5
#define HTTP_UAProfDiff             0xB6
#define HTTP_UAProfileWarning       0xB7
#define HTTP_XWapTod                0xBF

/* The x-wap.tod header is also defined here as a full HTTP string because we
   need to use it while our header encoding version stays at 1.2 */
#define STR_HTTP_XWAP_TOD           "x-wap.tod"

#define HTTP_SetCookie              0xC1
#define HTTP_Cookie                 0xC2
/* For codes 0xB8(0x38) through 0xC3(0x44) see WSP spec. */
#define HTTP_XwapSecurity           0xC4

#define HTTP_SPECIAL_textext_header 0xFF01
#define HTTP_SPECIAL_inttext_header 0xFF02

/* Cache-Control */
#define HTTP_CC_No_cache            0x00
#define HTTP_CC_No_store            0x01
#define HTTP_CC_Max_age             0x02
#define HTTP_CC_Max_stale           0x03
#define HTTP_CC_Min_fresh           0x04
#define HTTP_CC_Only_if_cached      0x05
#define HTTP_CC_Public              0x06
#define HTTP_CC_Private             0x07
#define HTTP_CC_No_transform        0x08
#define HTTP_CC_Must_revalidate     0x09
#define HTTP_CC_Proxy_revalidate    0x0A
#define HTTP_CC_Cache_extension     0x0F00  

/* Parameter names */
#define HTTP_Q                      0x80
#define HTTP_Charset                0x81
#define HTTP_Level                  0x82
#define HTTP_Type                   0x83
#define HTTP_Uaprof                 0x84
#define HTTP_Name                   0x85
#define HTTP_Filename               0x86
#define HTTP_Differences            0x87
#define HTTP_Padding                0x88

/* Language names */
#define HTTP_Afar                   0x01
#define HTTP_Abkhazian              0x02
#define HTTP_Afrikaans              0x03
#define HTTP_Amharic                0x04
#define HTTP_Arabic                 0x05
#define HTTP_Assamese               0x06
#define HTTP_Aymara                 0x07
#define HTTP_Azerbaijani            0x08
#define HTTP_Bashkir                0x09
#define HTTP_Byelorussian           0x0A
#define HTTP_Bulgarian              0x0B
#define HTTP_Bihari                 0x0C
#define HTTP_Bislama                0x0D
#define HTTP_Bengali                0x0E
#define HTTP_Tibetan                0x0F
#define HTTP_Breton                 0x10
#define HTTP_Catalan                0x11
#define HTTP_Corsican               0x12
#define HTTP_Czech                  0x13
#define HTTP_Welsh                  0x14
#define HTTP_Danish                 0x15
#define HTTP_German                 0x16
#define HTTP_Bhutani                0x17
#define HTTP_Greek                  0x18
#define HTTP_English                0x19
#define HTTP_Esperanto              0x1A
#define HTTP_Spanish                0x1B
#define HTTP_Estonian               0x1C
#define HTTP_Basque                 0x1D
#define HTTP_Persian                0x1E
#define HTTP_Finnish                0x1F
#define HTTP_Fiji                   0x20
#define HTTP_Faeroese               0x82
#define HTTP_French                 0x22
#define HTTP_Frisian                0x83
#define HTTP_Irish                  0x24
#define HTTP_Scots_Gaelic           0x25
#define HTTP_Galician               0x26
#define HTTP_Guarani                0x27
#define HTTP_Gujarati               0x28
#define HTTP_Hausa                  0x29
#define HTTP_Hebrew                 0x2A
#define HTTP_Hindi                  0x2B
#define HTTP_Croatian               0x2C
#define HTTP_Hungarian              0x2D
#define HTTP_Armenian               0x2E
#define HTTP_Interlingua            0x84
#define HTTP_Indonesian             0x30
#define HTTP_Interlingue            0x86
#define HTTP_Inupiak                0x87
#define HTTP_Icelandic              0x33
#define HTTP_Italian                0x34
#define HTTP_Inuktitut              0x89
#define HTTP_Japanese               0x36
#define HTTP_Javanese               0x37
#define HTTP_Georgian               0x38
#define HTTP_Kazakh                 0x39
#define HTTP_Greenlandic            0x8A
#define HTTP_Cambodian              0x3B
#define HTTP_Kannada                0x3C
#define HTTP_Korean                 0x3D
#define HTTP_Kashmiri               0x3E
#define HTTP_Kurdish                0x3F
#define HTTP_Kirghiz                0x40
#define HTTP_Latin                  0x8B
#define HTTP_Lingala                0x42
#define HTTP_Laothian               0x43
#define HTTP_Lithuanian             0x44
#define HTTP_Lettish                0x45
#define HTTP_Malagasy               0x46
#define HTTP_Maori                  0x47
#define HTTP_Macedonian             0x48
#define HTTP_Malayalam              0x49
#define HTTP_Mongolian              0x4A
#define HTTP_Moldavian              0x4B
#define HTTP_Marathi                0x4C
#define HTTP_Malay                  0x4D
#define HTTP_Maltese                0x4E
#define HTTP_Burmese                0x4F
#define HTTP_Nauru                  0x81
#define HTTP_Nepali                 0x51
#define HTTP_Dutch                  0x52
#define HTTP_Norwegian              0x53
#define HTTP_Occitan                0x54
#define HTTP_Oromo                  0x55
#define HTTP_Oriya                  0x56
#define HTTP_Punjabi                0x57
#define HTTP_Polish                 0x58
#define HTTP_Pashto                 0x59
#define HTTP_Portuguese             0x5A
#define HTTP_Quechua                0x5B
#define HTTP_Rhaeto_Romance         0x8C
#define HTTP_Kirundi                0x5D
#define HTTP_Romanian               0x5E
#define HTTP_Russian                0x5F
#define HTTP_Kinyarwanda            0x60
#define HTTP_Sanskrit               0x61
#define HTTP_Sindhi                 0x62
#define HTTP_Sangho                 0x63
#define HTTP_Serbo_Croatian         0x64
#define HTTP_Sinhalese              0x65
#define HTTP_Slovak                 0x66
#define HTTP_Slovenian              0x67
#define HTTP_Samoan                 0x68
#define HTTP_Shona                  0x69
#define HTTP_Somali                 0x6A
#define HTTP_Albanian               0x6B
#define HTTP_Serbian                0x6C
#define HTTP_Siswati                0x6D
#define HTTP_Sesotho                0x6E
#define HTTP_Sundanese              0x6F
#define HTTP_Swedish                0x70
#define HTTP_Swahili                0x71
#define HTTP_Tamil                  0x72
#define HTTP_Telugu                 0x73
#define HTTP_Tajik                  0x74
#define HTTP_Thai                   0x75
#define HTTP_Tigrinya               0x76
#define HTTP_Turkmen                0x77
#define HTTP_Tagalog                0x78
#define HTTP_Setswana               0x79
#define HTTP_Tonga                  0x7A
#define HTTP_Turkish                0x7B
#define HTTP_Tsonga                 0x7C
#define HTTP_Tatar                  0x7D
#define HTTP_Twi                    0x7E
#define HTTP_Uighur                 0x7F
#define HTTP_Ukrainian              0x50
#define HTTP_Urdu                   0x21
#define HTTP_Uzbek                  0x23
#define HTTP_Vietnamese             0x2F
#define HTTP_Volapuk                0x85
#define HTTP_Wolof                  0x31
#define HTTP_Xhosa                  0x32
#define HTTP_Yiddish                0x88
#define HTTP_Yoruba                 0x35
#define HTTP_Zhuang                 0x3A
#define HTTP_Chinese                0x41
#define HTTP_Zulu                   0x5C
  
/* Content encoding names */
#define  HTTP_None                  0xff
#define  HTTP_Gzip                  0x00
#define  HTTP_Compress              0x01
#define  HTTP_Deflate               0x02

/* x-wap-security parameters */
#define  HTTP_CloseSubordinate      0x80

// Encoding
// IMPORTANT: Sync with \browser\settingsinc\settingscontainer.h
// The order of these enums can NOT be changed 
enum TEncoding
    {
    EGb2312,
    EISO8859_1,
    EBig5,
    EUTF8,
    EISO8859_2,
    EISO8859_4,
    EISO8859_5,
    EISO8859_7,
    EISO8859_9,
    EISO8859_8i,
    EISO8859_8,
    EISO8859_6,
    EWindows1256,
    EWindows1255,
    EWindows1250,
    EWindows1251,
    EWindows1253,
    EWindows1254,
    EWindows1257,
    EAutomatic,
    EISO10646_UCS_2,
    ETis620,
    EShiftJis,
    EJisX0201_1997,
    EJisX0208_1997,
    EEucJp,
    EIso2022Jp,
    EWindows874,
    EKoi8_r,
    EKoi8_u,
    EDummyLast
    };

#define NW_URL_METHOD_GET             0x40
#define NW_URL_METHOD_OPTIONS         0x41
#define NW_URL_METHOD_HEAD            0x42
#define NW_URL_METHOD_DELETE          0x43
#define NW_URL_METHOD_TRACE           0x44
#define NW_URL_METHOD_POST            0x60
#define NW_URL_METHOD_PUT             0x61
#define NW_URL_METHOD_DATA            0x62

#define  SwitchingProtocols   101
#define  ContentStale         102  /* Should be 110 */

/* Success */
#define  Success              200
#define  Created              201
#define  Accepted             202
#define  NonAuthInfo          203
#define  NoContent            204
#define  ResetContent         205
#define  PartialContent       206

/* Redirects */
#define  MultipleChoices      300
#define  MovedPerm            301
#define  MovedTemp            302
#define  SeeOther             303
#define  NotModified          304
#define  UseProxy             305

/* User agent erros */
#define  BadRequest           400
#define  Unauthorized         401
#define  PaymentRequired      402
#define  Forbidden            403
#define  NotFound             404
#define  MethodNotAllowed     405
#define  NotAcceptable        406
#define  ProxyAuth            407
#define  RequestTimeOut       408
#define  Conflict             409
#define  Gone                 410
#define  LengthRequired       411
#define  PreconditionFailed   412
#define  EntityTooLarge       413
#define  URLTooLarge          414
#define  UnsupportedMedia     415

/* Server errors. */
#define  ServerError          500
#define  NotImplemented       501
#define  BadGateway           502
#define  ServiceUnavailable   503
#define  GatewayTimeout       504
#define  HTTPVerNotSupported  505


/* HTTP Default Headers */
#define NW_DEFAULTHEADERS_NOTANY  0x00
#define NW_DEFAULTHEADERS_CONTENT 0x01
#define NW_DEFAULTHEADERS_CHARSET 0x02
#define NW_DEFAULTHEADERS_LANG    0x04
#define NW_DEFAULTHEADERS_ALL     0x07

/* Other names */
#define HTTP_Chunked                0x00
#define NW_DEFAULT_CONTENTTYPE      HTTP_application_vnd_wap_wmlc
#define NW_DEFAULT_CHARSET          HTTP_iso_8859_1

#define SAVED_DECK_VERSION "SavedDeck Ver 2.0"

/* These types are for optimized accept headers */
#define NW_UrlRequest_Type_Any              1  /* Any type               */
#define NW_UrlRequest_Type_Image            2  /* Load an image type      */
#define NW_UrlRequest_Type_Css              3  /* Load a stylesheet type */
#define NW_UrlRequest_Type_JavaScript       4  /* JavaScript type        */
#define NW_UrlRequest_Type_Plugin           5  /* Load plugin resource   */
#define NW_UrlRequest_Type_None             6  /* No content types       */

/* Byte order: big-endian or little-endian for 2-byte charset or single byte */
#define NW_BYTEORDER_SINGLE_BYTE    0
#define NW_BYTEORDER_BIG_ENDIAN     1
#define NW_BYTEORDER_LITTLE_ENDIAN  2
// The native byte order is little endian.
#define NW_BYTEORDER_NATIVE NW_BYTEORDER_LITTLE_ENDIAN

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NW_HTTP_DEFS_H */
