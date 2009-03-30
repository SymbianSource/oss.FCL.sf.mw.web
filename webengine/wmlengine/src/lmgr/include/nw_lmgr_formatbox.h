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


#ifndef NW_FORMATBOX_H
#define NW_FORMATBOX_H

#include "nw_lmgr_containerbox.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */


/* 
 * Format boxes manage formatting. These are boxes within which other
 * boxes can be laid out using some kind of formatting algorithm: a
 * flow, a table, etc. Format box is an abstract class from which
 * various classes of formatter, such as flows and tables, are
 * sub-classed. Specific format box classes may only know how to
 * format boxes of specific types.  
 */

typedef struct NW_LMgr_FormatBox_s NW_LMgr_FormatBox_t;

/* This is the fundamental formatting mechanism. Inserts a box into
 * the format, placing and sizing it in relation to other boxes and
 * the format's borders.  The fundamental design assumption of the
 * formatting model is that boxes can be added sequentially, one at a
 * time to the format, and that the sequence determines, in part, how
 * boxes are laid out. This is a pure virtual method that must be
 * overridden by format box subclasses.  
 */

typedef
TBrowserStatusCode
(*NW_LMgr_FormatBox_Format_t)(NW_LMgr_FormatBox_t *format, 
                              NW_LMgr_Box_t *box,
                              NW_LMgr_FormatContext_t *context);

/* When a container box is added to a format, the format invokes this
 * method to recursively format the the children of the container. If
 * the added container is a format box, this method is invoked with
 * the container as both the first and second arguments. However,
 * since not all containers are format boxes, the format may instead
 * invoke this method on itself, passing the container as the second
 * argument only.  This results in the container children being
 * formatted within the parent format.  This is useful for elements
 * like XHTML <a> which do not create a new formatting context for
 * their children.  
 */

typedef
TBrowserStatusCode
(*NW_LMgr_FormatBox_FormatChildren_t)(NW_LMgr_FormatBox_t* format, 
                                      NW_LMgr_ContainerBox_t *whose,
                                      NW_LMgr_FormatContext_t* context);

/* Since properties may affect the formatting context (for example, by
 * setting flow margins), format boxes provide a method to apply such
 * properties.  
 */

typedef
TBrowserStatusCode
(*NW_LMgr_FormatBox_ApplyFormatProps_t)(NW_LMgr_FormatBox_t* format, 
                                        NW_LMgr_FormatContext_t* context);

/* Class declarations -------------------------------------------------------*/
  
typedef struct NW_LMgr_FormatBox_ClassPart_s {
  NW_LMgr_FormatBox_ApplyFormatProps_t applyFormatProps;
} NW_LMgr_FormatBox_ClassPart_t;
  
typedef struct NW_LMgr_FormatBox_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_Box_ClassPart_t NW_LMgr_Box;
  NW_LMgr_ContainerBox_ClassPart_t NW_LMgr_ContainerBox;
  NW_LMgr_FormatBox_ClassPart_t NW_LMgr_FormatBox;
} NW_LMgr_FormatBox_Class_t;

/* ------------------------------------------------------------------------- */
struct NW_LMgr_FormatBox_s {
  NW_LMgr_ContainerBox_t super;
};
  
/* ------------------------------------------------------------------------- */

#define NW_LMgr_FormatBox_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_LMgr_FormatBox))

#define NW_LMgr_FormatBoxOf(_object) \
  (NW_Object_Cast (_object, NW_LMgr_FormatBox))

  /* ------------------------------------------------------------------------- */
#define NW_LMgr_FormatBox_Format(_object, _box, _context) \
  (NW_LMgr_FormatBox_GetClassPart (_object).format ((_object), (_box), (_context)))
  
/* ------------------------------------------------------------------------- */

#define NW_LMgr_FormatBox_FormatChildren(_object, _whose, _context) \
  (NW_LMgr_FormatBox_GetClassPart (_object).formatChildren ((_object), (_whose), (_context)))

#define NW_LMgr_FormatBox_ApplyFormatProps(_object, _context) \
  (NW_LMgr_FormatBox_GetClassPart (_object).applyFormatProps ((_object), (_context)))

/* ---------------------------------------------------------------------------*/

NW_LMGR_EXPORT const NW_LMgr_FormatBox_Class_t NW_LMgr_FormatBox_Class;

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif 







































