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


#ifndef NW_LMGR_BOX_H
#define NW_LMGR_BOX_H

#include "nw_lmgr_formatcontext.h"
#include "nw_object_dynamic.h"
#include "nw_lmgr_propertylist.h"
#include "nw_gdi_types.h"
#include "BrsrTypes.h"
#include "nw_adt_vector.h"
#include "NW_LMgr_EXPORT.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_LMgr_Box_Class_s NW_LMgr_Box_Class_t;

typedef struct NW_LMgr_RootBox_s NW_LMgr_RootBox_t;

/* ------------------------------------------------------------------------- *
   public type definitions
 * ------------------------------------------------------------------------- */
typedef struct NW_LMgr_FrameInfo_s {
  NW_Int32 left;
  NW_Int32 right;
  NW_Int32 top;
  NW_Int32 bottom;
} NW_LMgr_FrameInfo_t;

enum TLMgrFrameSide
  {
  ELMgrFrameLeft    = 0x01,
  ELMgrFrameRight   = 0x02,
  ELMgrFrameTop     = 0x04,  
  ELMgrFrameBottom  = 0x08,
  ELMgrFrameAll     =  ELMgrFrameLeft | ELMgrFrameRight | ELMgrFrameTop | ELMgrFrameBottom
  };

#define NW_LMgr_FrameInfo_Auto (NW_Int32)0x10000
#define NW_LMgr_FrameInfo_AutoMargin (NW_Int32)0x00000
#define NW_LMgr_Box_MinBorderWidth 1
#define NW_LMgr_Box_MediumBorderWidth 2
#define NW_LMgr_Box_MaxBorderWidth 3


/* ------------------------------------------------------------------------- *
   virtual methods type definitions
 * ------------------------------------------------------------------------- */
typedef
TBrowserStatusCode
(*NW_LMgr_Box_Split_t)(NW_LMgr_Box_t* box,
                       NW_GDI_Metric_t space,
                       NW_LMgr_Box_t* *splitBox,
                       NW_Uint8 flags);
typedef
TBrowserStatusCode
(*NW_LMgr_Box_Resize_t)(NW_LMgr_Box_t* box, NW_LMgr_FormatContext_t* context);

typedef
TBrowserStatusCode
(*NW_LMgr_Box_PostResize_t)(NW_LMgr_Box_t* box);


typedef
TBrowserStatusCode
(*NW_LMgr_Box_GetMinimumContentSize_t)(NW_LMgr_Box_t* box,
                                       NW_GDI_Dimension2D_t *size);

typedef
NW_Bool
(*NW_LMgr_Box_HasFixedContentSize_t)(NW_LMgr_Box_t *box);


typedef
TBrowserStatusCode
(*NW_LMgr_Box_Constrain_t)(NW_LMgr_Box_t* box,
                           NW_GDI_Metric_t constraint);

typedef
TBrowserStatusCode
(*NW_LMgr_Box_Draw_t) (NW_LMgr_Box_t* box,
                       CGDIDeviceContext* deviceContext,
                       NW_Uint8 hasFocus);

typedef
TBrowserStatusCode
(*NW_LMgr_Box_Render_t) (NW_LMgr_Box_t* box,
                         CGDIDeviceContext* deviceContext,
                         NW_GDI_Rectangle_t* clipRect,
                         NW_LMgr_Box_t *currentBox,
                         NW_Uint8 flags,
                         NW_Bool* hasFocus,
                         NW_Bool* skipChildren,
                         NW_LMgr_RootBox_t* rootBox );

typedef
NW_GDI_Metric_t
(*NW_LMgr_Box_GetBaseline_t)(NW_LMgr_Box_t* box);

typedef
TBrowserStatusCode
(*NW_LMgr_Box_Shift_t)(NW_LMgr_Box_t* box, NW_GDI_Point2D_t *delta);

typedef
NW_LMgr_Box_t*
(*NW_LMgr_Box_Clone_t)(NW_LMgr_Box_t* box);

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_LMgr_Box_ClassPart_s {
  NW_LMgr_Box_Split_t split;
  NW_LMgr_Box_Resize_t resize;
  NW_LMgr_Box_PostResize_t postResize;
  NW_LMgr_Box_GetMinimumContentSize_t getMinimumContentSize;
  NW_LMgr_Box_HasFixedContentSize_t hasFixedContentSize;
  NW_LMgr_Box_Constrain_t constrain;
  NW_LMgr_Box_Draw_t draw;
  NW_LMgr_Box_Render_t render;
  NW_LMgr_Box_GetBaseline_t getBaseline;
  NW_LMgr_Box_Shift_t shift;
  NW_LMgr_Box_Clone_t clone;
} NW_LMgr_Box_ClassPart_t;
  
struct NW_LMgr_Box_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_Box_ClassPart_t NW_LMgr_Box;
};


/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_LMgr_Box_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_LMgr_Box))
  
#define NW_LMgr_BoxOf(object) \
  (NW_Object_Cast (object, NW_LMgr_Box))
  
/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT const NW_LMgr_Box_Class_t NW_LMgr_Box_Class;

/* ------------------------------------------------------------------------- */
/* Public macros 
 */

#define NW_LMGR_FLOOR_DIV2(x) ((NW_GDI_Metric_t) (x >> 1))
#define NW_LMGR_CEIL_DIV2(x)  ((NW_GDI_Metric_t) ((x + x % 2) >> 1))

/* ------------------------------------------------------------------------- *
   TODO: replace these with platform-specific values
 * ------------------------------------------------------------------------- */

#define NW_LMGR_BOX_MIN_WIDTH (NW_GDI_Metric_t)10
#define NW_LMGR_BOX_MIN_HEIGHT (NW_GDI_Metric_t)15

/* Flags */

typedef enum NW_LMgr_Box_SplitFlags_e {
  NW_LMgr_Box_SplitFlags_AtNewLine   = 0x01,
  NW_LMgr_Box_SplitFlags_NoPendingWS = 0x02,
  NW_LMgr_Box_SplitFlags_Nowrap = 0x04
} NW_LMgr_Box_SplitFlags_t;

enum {
  NW_LMgr_Box_Flags_DrawFloats = 0x01
};

/* ------------------------------------------------------------------------- *
   public prototypes
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT
NW_LMgr_RootBox_t*
_NW_LMgr_Box_GetRootBox (NW_LMgr_Box_t* box);

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_LMgr_Box_Split(_object, _space, _split, _flags) \
  (NW_LMgr_Box_GetClassPart (_object).split ((_object), (_space), (_split), (_flags)))

#define NW_LMgr_Box_Resize(_object, _context) \
  (NW_LMgr_Box_GetClassPart (_object).resize ((_object),(_context)))

#define NW_LMgr_Box_PostResize(_object) \
  (NW_LMgr_Box_GetClassPart (_object).postResize (_object))


#define NW_LMgr_Box_GetMinimumContentSize(_object, _size) \
  (NW_LMgr_Box_GetClassPart (_object).getMinimumContentSize ((_object), (_size)))

#define NW_LMgr_Box_HasFixedContentSize(_object) \
  (NW_LMgr_Box_GetClassPart (_object).hasFixedContentSize (_object))

#define NW_LMgr_Box_Constrain(_object, _constraint) \
  (NW_LMgr_Box_GetClassPart (_object).constrain ((_object), (_constraint)))

#define NW_LMgr_Box_GetBaseline(_object) \
  (NW_LMgr_Box_GetClassPart (_object).getBaseline (_object))

#define NW_LMgr_Box_Shift(_object, _shift) \
  (NW_LMgr_Box_GetClassPart (_object).shift ((_object), (_shift)))

#define NW_LMgr_Box_Clone(_object) \
  (NW_LMgr_Box_GetClassPart (_object).clone ((_object)))

#define NW_LMgr_Box_Draw(_object, _deviceContext, _hasFocus) \
  (NW_LMgr_Box_GetClassPart(_object). \
     draw ((_object), (_deviceContext), (_hasFocus)))

#define NW_LMgr_Box_Render(_object, _deviceContext, _clipRect, _currentBox, _flags, _hasFocus, _skipChildren, _rootBox ) \
  (NW_LMgr_Box_GetClassPart(_object). \
     render ((_object), (_deviceContext), (_clipRect), (_currentBox), (_flags), (_hasFocus), (_skipChildren), (_rootBox) ))

#define NW_LMgr_Box_GetParent(_object) \
  (NW_OBJECT_CONSTCAST(NW_LMgr_ContainerBox_t*) NW_LMgr_BoxOf (_object)->parent)

#define NW_LMgr_Box_PropListGet(_box) \
  (NW_OBJECT_CONSTCAST(NW_LMgr_PropertyList_t*) (_box)->propList)

#define NW_LMgr_Box_SetPropList(_object, _propList) \
  (((_object)->propList = (_propList)), KBrsrSuccess)

#define NW_LMgr_Box_SetParent(_box, _parent) \
  ((_box)->parent = (_parent))

#define NW_LMgr_Box_GetRootBox(_box) \
  (_NW_LMgr_Box_GetRootBox (NW_LMgr_BoxOf (_box)))

NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_Box_GetRawProperty(NW_LMgr_Box_t *box,
                           NW_LMgr_PropertyName_t name,
                           NW_LMgr_Property_t* property);
NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_Box_GetPropertyFromList(NW_LMgr_Box_t* box,
                                NW_LMgr_PropertyName_t name,
                                NW_LMgr_Property_t* property);

NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_Box_GetProperty (NW_LMgr_Box_t* box, 
                         NW_LMgr_PropertyName_t name, 
                         NW_LMgr_Property_t* property);

NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_Box_GetPropertyValue (NW_LMgr_Box_t* box, 
                              NW_LMgr_PropertyName_t name,
                              NW_LMgr_PropertyValueType_t type,
                              NW_LMgr_PropertyValue_t* value);
NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_Box_RemoveProperty (NW_LMgr_Box_t* box,
                            NW_LMgr_PropertyName_t name);

NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_Box_RemovePropertyWithoutDelete (NW_LMgr_Box_t* box,
                            NW_LMgr_PropertyName_t name);

NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_Box_SetProperty (NW_LMgr_Box_t* box,
                         NW_LMgr_PropertyName_t name,
                         NW_LMgr_Property_t* property);

NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_Box_GetMinimumSize(NW_LMgr_Box_t* box,
                           NW_GDI_Dimension2D_t *size);

NW_LMGR_EXPORT
NW_Bool
NW_LMgr_Box_HasFixedWidth(NW_LMgr_Box_t* box);

NW_LMGR_EXPORT
NW_Bool
NW_LMgr_Box_HasFixedHeight(NW_LMgr_Box_t* box);

NW_LMGR_EXPORT
void
NW_LMgr_Box_GetMargins(NW_LMgr_Box_t *box, 
                       NW_LMgr_FrameInfo_t *margins,
                       NW_LMgr_FrameInfo_t *isAuto,
                       NW_Int8 aFrameSide );

NW_LMGR_EXPORT
void
NW_LMgr_Box_GetPadding(NW_LMgr_Box_t *box, 
                       NW_LMgr_FrameInfo_t *padding, 
                       NW_Int8 aFrameSide );

NW_LMGR_EXPORT 
NW_Int32
NW_LMgr_Box_GetModelProp(NW_LMgr_Box_t *box, NW_LMgr_PropertyName_t propName); 

NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_Box_GetSizeProperties(NW_LMgr_Box_t *box,
                              NW_GDI_Dimension2D_t *size);

NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_Box_ApplyFontProps (NW_LMgr_Box_t* box);

NW_LMGR_EXPORT
CGDIFont*
NW_LMgr_Box_GetFont (NW_LMgr_Box_t* box);

NW_LMGR_EXPORT
NW_GDI_Metric_t
NW_LMgr_Box_GetEm (NW_LMgr_Box_t* box);

NW_LMGR_EXPORT
NW_GDI_Metric_t
NW_LMgr_Box_GetEx (NW_LMgr_Box_t* box);

NW_LMGR_EXPORT
void
NW_LMgr_Box_GetBorderWidth(NW_LMgr_Box_t *box,
                           NW_LMgr_FrameInfo_t *borderWidth,
                           NW_Int8 aFrameSide );

NW_LMGR_EXPORT
void
NW_LMgr_Box_GetBorderColor(NW_LMgr_Box_t *box,
                           NW_LMgr_FrameInfo_t *borderColor);

NW_LMGR_EXPORT
void
NW_LMgr_Box_GetBorderStyle(NW_LMgr_Box_t *box,
                           NW_LMgr_FrameInfo_t *borderStyle,
                           NW_Int8 aFrameSide );

NW_LMGR_EXPORT
NW_LMgr_Box_t*
NW_LMgr_Box_FindBoxOfClass (NW_LMgr_Box_t* box,
                            const NW_Object_Class_t* theClass,
                            NW_Bool derived);

NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_Box_Detach (NW_LMgr_Box_t* box);

NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_Box_Refresh (NW_LMgr_Box_t* box);

NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_Box_DrawBorder (NW_GDI_Rectangle_t bounds, 
                        CGDIDeviceContext* deviceContext, 
                        NW_LMgr_FrameInfo_t* borderWidth, 
                        NW_LMgr_FrameInfo_t* borderStyle, 
                        NW_LMgr_FrameInfo_t* borderColor);

NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_Box_DrawBackground (NW_LMgr_Box_t* box, 
                            NW_LMgr_ContainerBox_t* parent,
                            CGDIDeviceContext* deviceContext);

NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_Box_GetInnerRectangle(NW_LMgr_Box_t* box, 
                              NW_GDI_Rectangle_t* innerRectangle);

NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_Box_InvalidateBoxTree(NW_LMgr_Box_t* box, NW_LMgr_Box_t* boxWithFocus);

NW_Bool
NW_LMgr_Box_GetVisibleBounds(const NW_LMgr_Box_t* box,
                             NW_GDI_Rectangle_t* bounds);
NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_Box_Predraw (NW_LMgr_Box_t* box,
                      CGDIDeviceContext* deviceContext,
                      NW_Uint8 hasFocus);

NW_LMGR_EXPORT
void
NW_LMgr_Box_SetHasFocus(NW_LMgr_Box_t* box, NW_Bool hasFocus);

NW_LMGR_EXPORT
NW_Bool
NW_LMgr_Box_HasFocus(NW_LMgr_Box_t* box);

NW_LMGR_EXPORT
NW_Bool
NW_LMgr_Box_Update_RenderCache (NW_LMgr_RootBox_t* rootBox, 
                                     NW_LMgr_Box_t* box,
                                     NW_GDI_Rectangle_t* realClip,
                                     void ***ptr);



/**
* NW_LMgr_Box_GetFormatBounds
* @description This method returns the format bounds of the box. Note that this bounds could
* differ from display bounds. Format bounds are mainly used by the format code so functions like
* resize and GetMinimumHeight should use format bounds while Render and Draw should use
* display bounds instead. Display bounds get updated during the format. When a box gets fully formatted, 
* the format bounds get transfered to the display bounds by calling NW_LMgr_Box_UpdateDisplayBounds.
*
* @param NW_LMgr_Box_t* aBox: pointer to the box you want to retrieve the bounds
*
* @return NW_GDI_Rectangle_t: format bounds
*/
NW_LMGR_EXPORT
NW_GDI_Rectangle_t 
NW_LMgr_Box_GetFormatBounds( NW_LMgr_Box_t* aBox );

/**
* NW_LMgr_Box_SetFormatBounds
* @description This method sets the format bounds on the box. Note that this bounds could 
* differ from display bounds. see NW_LMgr_Box_GetFormatBounds comment above for more info
* @param NW_LMgr_Box_t* aBox: pointer to the box you want to set the bounds
* @param NW_GDI_Rectangle_t aFormatBounds: format bounds*
* @return void
*/
NW_LMGR_EXPORT
void NW_LMgr_Box_SetFormatBounds( NW_LMgr_Box_t* aBox, NW_GDI_Rectangle_t aFormatBounds );

/**
* NW_LMgr_Box_GetDisplayBounds
* @description This method returns the display bounds on the box. Note that this bounds could 
* differ from format bounds. see NW_LMgr_Box_GetFormatBounds comment above for more info
* @param NW_LMgr_Box_t* aBox: pointer to the box you want to retrieve the bounds
*
* @return NW_GDI_Rectangle_t : display bounds
*/
NW_LMGR_EXPORT
NW_GDI_Rectangle_t 
NW_LMgr_Box_GetDisplayBounds( NW_LMgr_Box_t* aBox );

/**
* NW_LMgr_Box_UpdateDisplayBounds
* @description This method sets the display bounds on the box. Note that this bounds could 
* differ from format bounds. see NW_LMgr_Box_GetFormatBounds comment above for more info
* @param NW_LMgr_Box_t* aBox: pointer to the box you want to set the bounds
* @param NW_GDI_Rectangle_t aDisplayBounds: display bounds
*
* @return void
*/
NW_LMGR_EXPORT
void 
NW_LMgr_Box_UpdateDisplayBounds( NW_LMgr_Box_t* aBox, NW_GDI_Rectangle_t aDisplayBounds );

const NW_Int8*
NW_LMgr_Box_GetBoxType (NW_LMgr_Box_t *box);

/**
* NW_LMgr_Box_UpdateContainerBodyBox
* @description This method updates hight for displaybounds and formatbounds
*  of bodybox container to take into account body bottommargin. 
* it is called only once after page load complete
* @param NW_LMgr_RootBox_t* aRootBox: pointer to the rootBox
*
* @return void
*/
void NW_LMgr_Box_UpdateContainerBodyBox( NW_LMgr_RootBox_t* aRootBox );

const NW_Int8*
NW_LMgr_Box_GetBoxType (NW_LMgr_Box_t *box);

#ifdef _DEBUG

NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_Box_DumpBoxTree(NW_LMgr_Box_t* box);

#endif /* _DEBUG */

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_Box_Initialize (NW_LMgr_Box_t* box,
                        NW_ADT_Vector_Metric_t numProperties);

NW_LMGR_EXPORT
NW_LMgr_Box_t*
NW_LMgr_Box_New (NW_ADT_Vector_Metric_t numProperties);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_LMGR_BOX_H */



