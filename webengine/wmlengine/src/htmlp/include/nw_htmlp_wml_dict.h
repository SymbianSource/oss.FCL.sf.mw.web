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


/** ----------------------------------------------------------------------- **
    @package:     NW_HTMLP

    @synopsis:    default

    @description: default

 ** ----------------------------------------------------------------------- **/

#ifndef NW_HTMLP_WML_DICT_H
#define NW_HTMLP_WML_DICT_H

#include "nw_htmlp_dict.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
NW_HTMLP_ElementDescriptionConst_t* NW_HTMLP_Get_WML_ElementDescriptionTable();

NW_HTMLP_ElementTableIndex_t NW_HTMLP_Get_WML_ElementTableCount();

static const NW_Uint8 NW_HTMLP_tag_A[] = {1, 'a'};
#define HTMLP_WML_TAG_INDEX_A 0

static const NW_Uint8 NW_HTMLP_tag_ACCESS[] = {6, 'a','c','c','e','s','s'};
#define HTMLP_WML_TAG_INDEX_ACCESS (HTMLP_WML_TAG_INDEX_A + 1)

static const NW_Uint8 NW_HTMLP_tag_ANCHOR[] = {6, 'a','n','c','h','o','r'};
#define HTMLP_WML_TAG_INDEX_ANCHOR (HTMLP_WML_TAG_INDEX_ACCESS + 1)

static const NW_Uint8 NW_HTMLP_tag_B[] = {1, 'b'};
#define HTMLP_WML_TAG_INDEX_B (HTMLP_WML_TAG_INDEX_ANCHOR + 1)

static const NW_Uint8 NW_HTMLP_tag_BIG[] = {3, 'b','i','g'};
#define HTMLP_WML_TAG_INDEX_BIG (HTMLP_WML_TAG_INDEX_B + 1)

static const NW_Uint8 NW_HTMLP_tag_BR[] = {2, 'b','r'};
#define HTMLP_WML_TAG_INDEX_BR (HTMLP_WML_TAG_INDEX_BIG + 1)

static const NW_Uint8 NW_HTMLP_tag_CARD[] = {4, 'c','a','r','d'};
#define HTMLP_WML_TAG_INDEX_CARD (HTMLP_WML_TAG_INDEX_BR + 1)

static const NW_Uint8 NW_HTMLP_tag_DO[] = {2, 'd','o'};
#define HTMLP_WML_TAG_INDEX_DO (HTMLP_WML_TAG_INDEX_CARD + 1)

static const NW_Uint8 NW_HTMLP_tag_EM[] = {2, 'e','m'};
#define HTMLP_WML_TAG_INDEX_EM (HTMLP_WML_TAG_INDEX_DO + 1)

static const NW_Uint8 NW_HTMLP_tag_FIELDSET[] 
= {8, 'f','i','e','l','d','s','e','t'};
#define HTMLP_WML_TAG_INDEX_FIELDSET (HTMLP_WML_TAG_INDEX_EM + 1)

static const NW_Uint8 NW_HTMLP_tag_GO[] = {2, 'g','o'};
#define HTMLP_WML_TAG_INDEX_GO (HTMLP_WML_TAG_INDEX_FIELDSET + 1)

static const NW_Uint8 NW_HTMLP_tag_HEAD[] =   {4, 'h','e','a','d'};
#define HTMLP_WML_TAG_INDEX_HEAD (HTMLP_WML_TAG_INDEX_GO + 1)

static const NW_Uint8 NW_HTMLP_tag_I[] = {1, 'i'};
#define HTMLP_WML_TAG_INDEX_I (HTMLP_WML_TAG_INDEX_HEAD + 1)

static const NW_Uint8 NW_HTMLP_tag_IMG[] = {3, 'i','m','g'};
#define HTMLP_WML_TAG_INDEX_IMG (HTMLP_WML_TAG_INDEX_I + 1)

static const NW_Uint8 NW_HTMLP_tag_INPUT[] = {5, 'i','n','p','u','t'};
#define HTMLP_WML_TAG_INDEX_INPUT (HTMLP_WML_TAG_INDEX_IMG + 1)

static const NW_Uint8 NW_HTMLP_tag_META[] = {4, 'm','e','t','a'};
#define HTMLP_WML_TAG_INDEX_META (HTMLP_WML_TAG_INDEX_INPUT + 1)

static const NW_Uint8 NW_HTMLP_tag_NOOP[] = {4, 'n','o','o','p'};
#define HTMLP_WML_TAG_INDEX_NOOP (HTMLP_WML_TAG_INDEX_META + 1)

static const NW_Uint8 NW_HTMLP_tag_ONEVENT[] 
= {7, 'o','n','e','v','e','n','t'};
#define HTMLP_WML_TAG_INDEX_ONEVENT (HTMLP_WML_TAG_INDEX_NOOP + 1)

static const NW_Uint8 NW_HTMLP_tag_OPTGROUP[] 
= {8, 'o','p','t','g','r','o', 'u', 'p'};
#define HTMLP_WML_TAG_INDEX_OPTGROUP (HTMLP_WML_TAG_INDEX_ONEVENT + 1)

static const NW_Uint8 NW_HTMLP_tag_OPTION[] = {6, 'o','p','t','i','o','n'};
#define HTMLP_WML_TAG_INDEX_OPTION (HTMLP_WML_TAG_INDEX_OPTGROUP + 1)

static const NW_Uint8 NW_HTMLP_tag_P[] =   {1, 'p'};
#define HTMLP_WML_TAG_INDEX_P (HTMLP_WML_TAG_INDEX_OPTION + 1)

static const NW_Uint8 NW_HTMLP_tag_POSTFIELD[] 
= {9, 'p','o','s','t','f','i','e','l','d'};
#define HTMLP_WML_TAG_INDEX_POSTFIELD (HTMLP_WML_TAG_INDEX_P + 1)

static const NW_Uint8 NW_HTMLP_tag_PRE[] = {3, 'p','r','e'};
#define HTMLP_WML_TAG_INDEX_PRE (HTMLP_WML_TAG_INDEX_POSTFIELD + 1)

static const NW_Uint8 NW_HTMLP_tag_PREV[] = {4, 'p','r','e','v'};
#define HTMLP_WML_TAG_INDEX_PREV (HTMLP_WML_TAG_INDEX_PRE + 1)

static const NW_Uint8 NW_HTMLP_tag_REFRESH[] 
= {7, 'r','e','f','r','e','s','h'};
#define HTMLP_WML_TAG_INDEX_REFRESH (HTMLP_WML_TAG_INDEX_PREV + 1)

static const NW_Uint8 NW_HTMLP_tag_SELECT[] = {6, 's','e','l','e','c','t'};
#define HTMLP_WML_TAG_INDEX_SELECT (HTMLP_WML_TAG_INDEX_REFRESH + 1)

static const NW_Uint8 NW_HTMLP_tag_SETVAR[] = {6, 's','e','t','v','a','r'};
#define HTMLP_WML_TAG_INDEX_SETVAR (HTMLP_WML_TAG_INDEX_SELECT + 1)

static const NW_Uint8 NW_HTMLP_tag_SMALL[] = {5, 's','m','a','l','l'};
#define HTMLP_WML_TAG_INDEX_SMALL (HTMLP_WML_TAG_INDEX_SETVAR + 1)

static const NW_Uint8 NW_HTMLP_tag_STRONG[] = {6, 's','t','r','o','n','g'};
#define HTMLP_WML_TAG_INDEX_STRONG (HTMLP_WML_TAG_INDEX_SMALL + 1)

static const NW_Uint8 NW_HTMLP_tag_TABLE[] =  {5, 't','a','b','l','e'};
#define HTMLP_WML_TAG_INDEX_TABLE (HTMLP_WML_TAG_INDEX_STRONG + 1)

static const NW_Uint8 NW_HTMLP_tag_TD[] =  {2, 't','d'};
#define HTMLP_WML_TAG_INDEX_TD (HTMLP_WML_TAG_INDEX_TABLE + 1)

static const NW_Uint8 NW_HTMLP_tag_TEMPLATE[] 
= {8, 't','e','m','p','l','a','t','e'};
#define HTMLP_WML_TAG_INDEX_TEMPLATE (HTMLP_WML_TAG_INDEX_TD + 1)

static const NW_Uint8 NW_HTMLP_tag_TIMER[] =  {5, 't','i','m','e','r'};
#define HTMLP_WML_TAG_INDEX_TIMER (HTMLP_WML_TAG_INDEX_TEMPLATE + 1)

static const NW_Uint8 NW_HTMLP_tag_TR[] =  {2, 't','r'};
#define HTMLP_WML_TAG_INDEX_TR (HTMLP_WML_TAG_INDEX_TIMER + 1)

static const NW_Uint8 NW_HTMLP_tag_U[] =  {1, 'u'};
#define HTMLP_WML_TAG_INDEX_U (HTMLP_WML_TAG_INDEX_TR + 1)

static const NW_Uint8 NW_HTMLP_tag_WML[] = {3 ,'w','m','l'};
#define HTMLP_WML_TAG_INDEX_WML (HTMLP_WML_TAG_INDEX_U + 1)


#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

/* NW_HTMLP_WML_DICT_H */
#endif
