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


#ifndef NW_WML_DEFS_H
#define NW_WML_DEFS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nwx_defs.h"

typedef NW_Uint16 NW_Wml_ElType_e;
typedef NW_Uint16 NW_Wml_Attr_t;

/* Private WML definitions */
typedef enum {
  ED_CARD_NOT_IN_DECK,
  ED_ACCESS,
  ED_BAD_CONTENT,
  ED_OUT_OF_MEMORY
} NW_Wml_Error_Message_e;

/* Events */
#define CLICK   (0x01)

/* public attribute types */

typedef enum {
  INVALID_TASK,
  GO_TASK,
  PREV_TASK,
  REFRESH_TASK,
  RESTORE_TASK,
  NOOP_TASK
} NW_Wml_Task_e;

typedef enum {
  ONENTERFORWARD,
  ONENTERBACKWARD,
  ONCLICK,
  ONTIMER
} NW_Wml_Intrinsic_e;

typedef enum {
  SINGLE_SELECT,
  MULTIPLE_SELECT
} NW_Wml_Select_Type_e;


#define WAE_ASC_EMPTY_STR               (const char*)""
#define WAE_ASC_TRUE_STR                (const char*)"true"
#define WAE_ASC_FALSE_STR               (const char*)"false"
#define WAE_ASC_0_STR                   (const char*)"0"
#define WAE_ASC_1_STR                   (const char*)"1"
#define WAE_ASC_SEMICOLON_STR           (const char*)";"
#define WAE_ASC_HTTP_SLASH_SLASH_STR    (const char*)"http://"
#define WAE_ASC_HTTPS_SLASH_SLASH_STR   (const char*)"https://"
#define WAE_ASC_BOTTOM_STR              (const char*)"bottom"
#define WAE_ASC_CENTER_STR              (const char*)"center"
#define WAE_ASC_LEFT_STR                (const char*)"left"
#define WAE_ASC_MIDDLE_STR              (const char*)"middle"
#define WAE_ASC_HTTPS_STR               (const char*)"https"
#define WAE_ASC_RESET_STR               (const char*)"reset"
#define WAE_ASC_RIGHT_STR               (const char*)"right"
#define WAE_ASC_TOP_STR                 (const char*)"top"
#define WAE_ASC_APP_CHAR_STR            (const char*)"application/vnd.wap.wmlc;charset="
#define WAE_ASC_APP_X_WWW_CHAR_STR      (const char*)"application/x-www-form-urlencoded"
#define WAE_ASC_MULTIPART_STR           (const char*)"multipart/form-data"
#define WAE_ASC_HTTP_STR                (const char*)"http"
#define WAE_ASC_CONTENT_TYPE_STR        (const char*)"Content-Type"
#define WAE_ASC_EXPIRES_STR             (const char*)"Expires"
#define WAE_ASC_GET_STR                 (const char*)"get"
#define WAE_ASC_POST_STR                (const char*)"post"
#define WAE_ASC_NOCACHE_STR             (const char*)"no-cache"
#define WAE_ASC_WRAP_STR                (const char*)"wrap"
#define WAE_ASC_NOWRAP_STR              (const char*)"nowrap"
#define WAE_ASC_ACCEPT_STR              (const char*)"accept"
#define WAE_ASC_DELETE_STR              (const char*)"delete"
#define WAE_ASC_HELP_STR                (const char*)"help"
#define WAE_ASC_PASSWORD_STR            (const char*)"password"
#define WAE_ASC_ONPICK_STR              (const char*)"onpick"
#define WAE_ASC_ONENTERBACKWARD_STR     (const char*)"onenterbackward"
#define WAE_ASC_ONENTERFORWARD_STR      (const char*)"onenterforward"
#define WAE_ASC_ONTIMER_STR             (const char*)"ontimer"
#define WAE_ASC_OPTIONS_STR             (const char*)"options"
#define WAE_ASC_PREV_STR                (const char*)"prev"
#define WAE_ASC_TEXT_STR                (const char*)"text"
#define WAE_ASC_VND_STR                 (const char*)"vnd."
#define WAE_ASC_PRESERVE_STR            (const char*)"preserve"
#define WAE_ASC_DEFAULT_STR             (const char*)"default"
#define WAE_ASC_NOSAVE_STR              (const char*)"no_save"
#define WAE_ASC_NOKIA_STR               (const char*)"nokia"
#define WAE_ASC_UNKNOWN_STR             (const char*)"unknown"

#define WAE_ASC_STRLEN(str) (NW_Asc_strlen((str)))

#define MALLOC_FAILED { NW_Mem_Segment_Free(MEM_FUNK); return status; }

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /* NW_WML_DEFS_H */
