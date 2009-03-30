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

#ifndef NW_HTMLP_DICT_H
#define NW_HTMLP_DICT_H

#include "nwx_defs.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** ----------------------------------------------------------------------- **
    @typedef:     NW_HTMLP_ElementTableIndex

    @synopsis:    Index.

    @scope:       public
    @type:        NW_Uint32

    @description: The lower sixteen bits are used to store the index of the 
                  dictionary elements(0xefff)  and upper 16 bits are zero in this case. 
                  The UPPER 16 bits(from 0xffff0000) are for DTD elements index 
                  and lower are zero in this.
 ** ----------------------------------------------------------------------- **/
typedef NW_Uint32 NW_HTMLP_ElementTableIndex_t;


/** ----------------------------------------------------------------------- **
    @enum:        NW_HTMLP_ElementContentType

    @synopsis:    Element description to limit element content.

    @scope:       public

    @names:
       [ANY]
                  Element may contain document content mixed with markup.

       [EMPTY]
                  Element has no content of any kind.

       [PCDATA]
                  Element may contain document content but no markup.

       [PLAINTEXT]
                  Element contains the rest of the file as document text.

    @description: Element description to limit element content.
 ** ----------------------------------------------------------------------- **/
typedef enum NW_HTMLP_ElementContentType_e {
  ANY, EMPTY, PCDATA, PLAINTEXT
} NW_HTMLP_ElementContentType_t;


/** ----------------------------------------------------------------------- **
    @struct:      NW_HTMLP_ElementDescription

    @synopsis:    Element description.

    @scope:       public
    @variables:
       const NW_Uint8* const tag
                  Pointer to element name.

       const NW_HTMLP_ElementTableIndex_t* const closes
                  Closes indicator.

       const NW_HTMLP_ElementTableIndex_t* const blocks
                  Blocks indicator.

       NW_HTMLP_ElementContentType_t contentType
                  Content type.
	   NW_BOOL splyHandling
	              If some special handling of the tags are required.

    @description: Element description.
 ** ----------------------------------------------------------------------- **/
typedef const struct NW_HTMLP_ElementDescription_s {
  const NW_Uint8* tag; 
  const NW_HTMLP_ElementTableIndex_t* closes;
  const NW_HTMLP_ElementTableIndex_t* blocks;
  NW_HTMLP_ElementContentType_t contentType;
  NW_Bool splHandling;
}NW_HTMLP_ElementDescriptionConst_t;
  
extern const NW_HTMLP_ElementTableIndex_t NW_HTMLP_ElementTableCount;
extern NW_HTMLP_ElementDescriptionConst_t NW_HTMLP_ElementDescriptionTable[];

/* 
 * Access routines to get pointers to NW_HTMLP_ElementDescriptionTable &
 * NW_HTMLP_ElementTableCount from outside the XmlParserDLL
 */
NW_HTMLP_ElementDescriptionConst_t* NW_HTMLP_Get_ElementDescriptionTable();
NW_HTMLP_ElementTableIndex_t NW_HTMLP_Get_ElementTableCount();

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

/* NW_HTMLP_DICT_H */
#endif
