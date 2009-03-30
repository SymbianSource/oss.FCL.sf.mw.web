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


/*
    $Workfile: wml_ops.h $

    Purpose:

        Maps the WML bytecodes from the WML Specification to a unique namespace.
        This also makes the browser code more readable.
*/

#ifndef WML_OPS_H
#define WML_OPS_H

/* #defines for Global tokens. */


#define GT_SWITCH_PAGE          0x0
#define GT_END                  0x1
#define GT_ENTITY               0x2
#define GT_STR_I                0x3
#define GT_LITERAL              0x4
#define GT_PI                   0x43
#define GT_LITERAL_C            0x44
#define GT_STR_T                0x83
#define GT_LITERAL_A            0x84
#define GT_EXT_0                0xC0
#define GT_EXT_1                0xC1
#define GT_EXT_2                0xC2
#define GT_OPAQUE               0xC3
#define GT_LITERAL_AC           0xC4

/* #defines for overridden Global tokens. */

#define GT_VAR_ESC_I            0x40
#define GT_VAR_UNESC_I          0x41
#define GT_VAR_DIRECT_I         0x42
#define GT_VAR_ESC_T            0x80
#define GT_VAR_UNESC_T          0x81
#define GT_VAR_DIRECT_T         0x82

/* #defines for Tag tokens. */

#define TAG_A                   0x1C
#define TAG_ANCHOR              0x22
#define TAG_ACCESS              0x23
#define TAG_B                   0x24
#define TAG_BIG                 0x25
#define TAG_BR                  0x26
#define TAG_CARD                0x27
#define TAG_DO                  0x28
#define TAG_EM                  0x29
#define TAG_FIELDSET            0x2A
#define TAG_GO                  0x2B
#define TAG_HEAD                0x2C
#define TAG_I                   0x2D
#define TAG_IMG                 0x2E
#define TAG_INPUT               0x2F
#define TAG_META                0x30
#define TAG_NOOP                0x31
#define TAG_P                   0x20
#define TAG_POSTFIELD           0x21
#define TAG_PRE                 0x1B
#define TAG_PREV                0x32
#define TAG_ONEVENT             0x33
#define TAG_OPTGROUP            0x34
#define TAG_OPTION              0x35
#define TAG_REFRESH             0x36
#define TAG_SELECT              0x37
#define TAG_SMALL               0x38
#define TAG_STRONG              0x39
#define TAG_TABLE               0x1F
#define TAG_TD                  0x1D
#define TAG_TEMPLATE            0x3B
#define TAG_TIMER               0x3C
#define TAG_TR                  0x1E
#define TAG_U                   0x3D
#define TAG_SETVAR              0x3E
#define TAG_WML                 0x3F

/* #defines for AttrName tokens. */

#define AS_ACCEPT_CHARSET       0x5
#define AS_ACCESSKEY            0x5E    /* 3/30/00, JCashook */
#define AS_ALIGN                0x52
#define AS_ALIGN_BOTTOM         0x6
#define AS_ALIGN_CENTER         0x7
#define AS_ALIGN_LEFT           0x8
#define AS_ALIGN_MIDDLE         0x9
#define AS_ALIGN_RIGHT          0xA
#define AS_ALIGN_TOP            0xB
#define AS_ALT                  0xC
#define AS_CLASS                0x54
#define AS_COLUMNS              0x53
#define AS_CONTENT              0xD
#define AS_CONTENT_MIME_CHARSET 0x5C
#define AS_DOMAIN               0xF
#define AS_EMPTYOK_FALSE        0x10
#define AS_EMPTYOK_TRUE         0x11
#define AS_ENCTYPE              0x5F
#define AS_ENCTYPE_APPLICATION  0x60
#define AS_ENCTYPE_MULTIPART    0x61
#define AS_FORMAT               0x12
#define AS_FORUA_FALSE          0x56
#define AS_FORUA_TRUE           0x57
#define AS_HEIGHT               0x13
#define AS_HREF                 0x4A
#define AS_HREF_HTTP            0x4B
#define AS_HREF_HTTPS           0x4C
#define AS_HSPACE               0x14
#define AS_HTTP_EQUIV           0x5A
#define AS_HTTP_EQUIV_CONT_TYPE 0x5B
#define AS_HTTP_EQUIV_EXPIRES   0x5D
#define AS_ID                   0x55
#define AS_IVALUE               0x15
#define AS_INAME                0x16
#define AS_LABEL                0x18
#define AS_LOCALSRC             0x19
#define AS_MAXLENGTH            0x1A
#define AS_METHOD_GET           0x1B
#define AS_METHOD_POST          0x1C
#define AS_MODE_NOWRAP          0x1D
#define AS_MODE_WRAP            0x1E
#define AS_MULTIPLE_FALSE       0x1F
#define AS_MULTIPLE_TRUE        0x20
#define AS_NAME                 0x21
#define AS_NEWCONTEXT_FALSE     0x22
#define AS_NEWCONTEXT_TRUE      0x23
#define AS_ONENTERBACKWARD      0x25
#define AS_ONENTERFORWARD       0x26
#define AS_ONPICK               0x24
#define AS_ONTIMER              0x27
#define AS_OPTIONAL_FALSE       0x28
#define AS_OPTIONAL_TRUE        0x29
#define AS_PATH                 0x2A
#define AS_SCHEME               0x2E
#define AS_SENDREFERER_FALSE    0x2F
#define AS_SENDREFERER_TRUE     0x30
#define AS_SIZE                 0x31
#define AS_SRC                  0x32
#define AS_SRC_HTTP             0x58
#define AS_SRC_HTTPS            0x59
#define AS_ORDERED_TRUE         0x33
#define AS_ORDERED_FALSE        0x34
#define AS_TABINDEX             0x35
#define AS_TITLE                0x36
#define AS_TYPE                 0x37
#define AS_TYPE_ACCEPT          0x38
#define AS_TYPE_DELETE          0x39
#define AS_TYPE_HELP            0x3A
#define AS_TYPE_PASSWORD        0x3B
#define AS_TYPE_ONPICK          0x3C
#define AS_TYPE_ONENTERBACKWARD 0x3D
#define AS_TYPE_ONENTERFORWARD  0x3E
#define AS_TYPE_ONTIMER         0x3F
#define AS_TYPE_OPTIONS         0x45
#define AS_TYPE_PREV            0x46
#define AS_TYPE_RESET           0x47
#define AS_TYPE_TEXT            0x48
#define AS_TYPE_VND             0x49
#define AS_VALUE                0x4D
#define AS_VSPACE               0x4E
#define AS_WIDTH                0x4F
#define AS_XML_LANG             0x50
#define AS_XML_SPACE_PRESERVE   0x62
#define AS_XML_SPACE_DEFAULT    0x63
#define AS_CACHE_NOCACHE        0x64

/* #defines for AttrValue tokens. */
/* Following AV definitions are not being
   used, so it has been commented out.
   Note: its not deleted, for feature use.
#define AV_COM                  0x85
#define AV_EDU                  0x86
#define AV_NET                  0x87
#define AV_ORG                  0x88
#define AV_ACCEPT               0x89
#define AV_BOTTOM               0x8A
#define AV_CLEAR                0x8B
#define AV_DELETE               0x8C
#define AV_HELP                 0x8D
#define AV_HTTP                 0x8E
#define AV_HTTP_WWW             0x8F
#define AV_HTTPS                0x90
#define AV_HTTPS_WWW            0x91
#define AV_LIST                 0x92
#define AV_MIDDLE               0x93
#define AV_NOWRAP               0x94
#define AV_ONENTERBACKWARD      0x96
#define AV_ONENTERFORWARD       0x97
#define AV_ONPICK               0x95
#define AV_ONTIMER              0x98
#define AV_OPTIONS              0x99
#define AV_PASSWORD             0x9A
#define AV_RESET                0x9B
#define AV_TEXT                 0x9D
#define AV_TOP                  0x9E
#define AV_UNKNOWN              0x9F
#define AV_WRAP                 0xA0
#define AV_WWW                  0xA1
*/

#endif  /* WML_OPS_H */
