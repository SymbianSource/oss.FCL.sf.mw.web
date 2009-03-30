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
* Description:  Box class associated with the handling object tags.
*
*/


// INCLUDE FILES
#include <e32std.h>
#include <e32def.h>

#include "LMgrObjectBoxOOCi.h"
#include "LMgrObjectBox.h"

// -----------------------------------------------------------------------------
#define GetImpl(_b) (NW_LMgr_ObjectBoxOf(_b)->iObjectBox)

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

const
NW_LMgr_ObjectBox_Class_t  NW_LMgr_ObjectBox_Class = 
{
    { /* NW_Object_Core            */
      /* super                     */ &NW_LMgr_MediaBox_Class,
      /* queryInterface            */ _NW_Object_Base_QueryInterface
    },
    { /* NW_Object_Base            */
      /* interfaceList             */ NULL
    },
    { /* NW_Object_Dynamic         */
      /* instanceSize              */ sizeof (NW_LMgr_ObjectBox_t),
      /* construct                 */ _NW_LMgr_ObjectBox_Construct,
      /* destruct                  */ _NW_LMgr_ObjectBox_Destruct
    },
    { /* NW_LMgr_Box               */
      /* split                     */ _NW_LMgr_ObjectBox_Split,
      /* resize                    */ _NW_LMgr_Box_Resize,
      /* postResize                */ _NW_LMgr_Box_PostResize,
      /* getMinimumContentSize     */ _NW_LMgr_ObjectBox_GetMinimumContentSize,
      /* hasFixedContentSize       */ _NW_LMgr_MediaBox_HasFixedContentSize,
      /* constrain                 */ _NW_LMgr_Box_Constrain,
      /* draw                      */ _NW_LMgr_ObjectBox_Draw,
      /* render                    */ _NW_LMgr_ObjectBox_Render,
      /* getBaseline               */ _NW_LMgr_Box_GetBaseline,
      /* shift                     */ _NW_LMgr_Box_Shift,
      /* clone                     */ _NW_LMgr_Box_Clone
    },
    { /* NW_LMgr_MediaBox          */
      /* unused                    */ 0
    },
    { /* NW_LMgr_ObjectBox         */
      /* unused                    */ NW_Object_Unused
    }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

// -----------------------------------------------------------------------------
// _NW_LMgr_ObjectBox_Construct
// Description:  The constructor.  
// Returns:      TBrowserStatusCode
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
_NW_LMgr_ObjectBox_Construct(NW_Object_Dynamic_t* dynamicObject, va_list* argp)
    {
    CLMgrObjectBox* objectBox = NULL;

    NW_TRY (status)
        {
        // Invoke our superclass constructor
        status = NW_LMgr_MediaBox_Class.NW_Object_Dynamic.construct(dynamicObject, argp);
        NW_THROW_ON_ERROR(status);

        // Create the implementation instance, iObjectBox
        TRAPD(err, objectBox = CLMgrObjectBox::NewL(NW_LMgr_BoxOf(dynamicObject)));
        NW_THROW_ON_TRAP_ERROR(err, status);

        NW_LMgr_ObjectBoxOf(dynamicObject)->iObjectBox = objectBox;
        }

    NW_CATCH (status) 
        {
        delete NW_LMgr_ObjectBoxOf(dynamicObject)->iObjectBox;
        NW_LMgr_ObjectBoxOf(dynamicObject)->iObjectBox = NULL;
        } 

    NW_FINALLY 
        {
        return status;
        } 
    NW_END_TRY
    }

// -----------------------------------------------------------------------------
// _NW_LMgr_ObjectBox_Destruct
// Description:  The destructor.  
// Returns:      void
// -----------------------------------------------------------------------------
//
void
_NW_LMgr_ObjectBox_Destruct(NW_Object_Dynamic_t* dynamicObject)
    {
    // Delete the implementation instance 
    delete NW_LMgr_ObjectBoxOf(dynamicObject)->iObjectBox;
    }

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_LMgr_ObjectBox_Split(NW_LMgr_Box_t* box, NW_GDI_Metric_t space,
        NW_LMgr_Box_t** splitBox, NW_Uint8 flags)
    {
    NW_REQUIRED_PARAM(box);

    // Pass it on to the implementation
    return GetImpl(box)->Split(space, splitBox, flags);
    }

// -----------------------------------------------------------------------------
TBrowserStatusCode
_NW_LMgr_ObjectBox_GetMinimumContentSize(NW_LMgr_Box_t* box, NW_GDI_Dimension2D_t* size)
    {
    NW_REQUIRED_PARAM(box);

    NW_ASSERT(size != NULL);

    // Pass it on to the implementation
    GetImpl(box)->GetMinimumContentSize(*size);

    return KBrsrSuccess;
    }

// -----------------------------------------------------------------------------
TBrowserStatusCode
_NW_LMgr_ObjectBox_Draw(NW_LMgr_Box_t* box,
                     CGDIDeviceContext* deviceContext,
                     NW_Uint8 hasFocus)
    {
    NW_REQUIRED_PARAM(box);

    // Pass it on to the implementation
    return GetImpl(box)->Draw(*deviceContext, hasFocus);
    }

// -----------------------------------------------------------------------------
TBrowserStatusCode
_NW_LMgr_ObjectBox_Render (NW_LMgr_Box_t* box, CGDIDeviceContext* deviceContext,
        NW_GDI_Rectangle_t* clipRect, NW_LMgr_Box_t* currentBox, NW_Uint8 flags, 
        NW_Bool* hasFocus, NW_Bool* skipChildren, NW_LMgr_RootBox_t* rootBox )
    {
    NW_REQUIRED_PARAM(box);

    NW_ASSERT(deviceContext != NULL);
    NW_ASSERT(rootBox != NULL);
    NW_ASSERT(clipRect != NULL);
    NW_ASSERT(hasFocus != NULL);
    NW_ASSERT(skipChildren != NULL);

    // Pass it on to the implementation
    return GetImpl(box)->Render(*deviceContext, *rootBox, *clipRect, currentBox, 
            flags, *hasFocus, *skipChildren);
    }
// -----------------------------------------------------------------------------
NW_Bool
NW_LMgr_ObjectBox_IsActive(NW_LMgr_Box_t* box)
    {
    NW_REQUIRED_PARAM(box);

    // Pass it on to the implementation
    return (NW_Bool) GetImpl(box)->IsActive();
    }

/* ------------------------------------------------------------------------- *
   public final methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
  convenience functions
 * ------------------------------------------------------------------------- */

// -----------------------------------------------------------------------------
NW_LMgr_ObjectBox_t*
NW_LMgr_ObjectBox_New (NW_ADT_Vector_Metric_t numProperties)
    {
    return (NW_LMgr_ObjectBox_t*) NW_Object_New(&NW_LMgr_ObjectBox_Class, 
            numProperties);
    }

