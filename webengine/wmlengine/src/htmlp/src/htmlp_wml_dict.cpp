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
#include "nw_htmlp_wml_dict.h"



/* must keep alphabetized by tag name */
NW_HTMLP_ElementDescriptionConst_t NW_HTMLP_WML_ElementDescriptionTable[] = {
  {NW_HTMLP_tag_A,          NULL,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_ACCESS,     NULL,    NULL,       EMPTY, NW_FALSE},
  {NW_HTMLP_tag_ANCHOR,     NULL,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_B,          NULL,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_BIG,        NULL,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_BR,         NULL,    NULL,       EMPTY, NW_FALSE},
  {NW_HTMLP_tag_CARD,       NULL,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_DO,         NULL,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_EM,         NULL,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_FIELDSET,   NULL,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_GO,         NULL,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_HEAD,       NULL,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_I,          NULL,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_IMG,        NULL,    NULL,       EMPTY, NW_FALSE},
  {NW_HTMLP_tag_INPUT,      NULL,    NULL,       EMPTY, NW_FALSE},
  {NW_HTMLP_tag_META,       NULL,    NULL,       EMPTY, NW_TRUE},
  {NW_HTMLP_tag_NOOP,       NULL,    NULL,       EMPTY, NW_FALSE},
  {NW_HTMLP_tag_ONEVENT,    NULL,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_OPTGROUP,   NULL,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_OPTION,     NULL,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_P,          NULL,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_POSTFIELD,  NULL,    NULL,       EMPTY, NW_FALSE},
  {NW_HTMLP_tag_PRE,        NULL,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_PREV,       NULL,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_REFRESH,    NULL,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_SELECT,     NULL,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_SETVAR,     NULL,    NULL,       EMPTY, NW_FALSE},
  {NW_HTMLP_tag_SMALL,      NULL,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_STRONG,     NULL,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_TABLE,      NULL,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_TD,         NULL,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_TEMPLATE,   NULL,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_TIMER,      NULL,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_TR,         NULL,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_U,          NULL,    NULL,       ANY, NW_FALSE},
  {NW_HTMLP_tag_WML,        NULL,    NULL,       ANY, NW_FALSE}
};

const NW_HTMLP_ElementTableIndex_t NW_HTMLP_WML_ElementTableCount
= (sizeof(NW_HTMLP_WML_ElementDescriptionTable)
   / sizeof(NW_HTMLP_ElementDescriptionConst_t));

/* 
 * Access routines to get pointers to NW_HTMLP_WML_ElementDescriptionTable &
 * NW_HTMLP_WML_ElementTableCount from outside the XmlParserDLL
 */
NW_HTMLP_ElementDescriptionConst_t* NW_HTMLP_Get_WML_ElementDescriptionTable()
{ 
  return (NW_HTMLP_WML_ElementDescriptionTable);
}

NW_HTMLP_ElementTableIndex_t NW_HTMLP_Get_WML_ElementTableCount()
{
  return (NW_HTMLP_WML_ElementTableCount);
}

#else

/* Not sure why this func is here but I made it static. Shaun Keller */
static
void FeaRmeNoHTMLParser_htmlp_dict(){
   int i = 0;
   i+=1;
}
#endif /* FEA_RME_NOHTMLPARSER */
