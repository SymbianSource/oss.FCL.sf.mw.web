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


#include "nw_lmgr_animatedimageboxi.h"

#include "nw_lmgr_rootbox.h"
#include "nw_lmgr_animatedimageaggregatei.h"
#include "BrsrStatusCodes.h"
#include "LMgrMarkerImage.h"

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_LMgr_AnimatedImageBox_Class_t  NW_LMgr_AnimatedImageBox_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_LMgr_ImgContainerBox_Class,
    /* queryInterface            */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base            */
    /* interfaceList             */ _NW_LMgr_AnimatedImageBox_SecondaryList
  },
  { /* NW_Object_Dynamic         */
    /* instanceSize              */ sizeof (NW_LMgr_AnimatedImageBox_t),
    /* construct                 */ _NW_LMgr_AnimatedImageBox_Construct,
    /* destruct                  */ NULL
  },
  { /* NW_LMgr_Box               */
    /* split                     */ _NW_LMgr_ImgContainerBox_Split,
    /* resize                    */ _NW_LMgr_Box_Resize,
  /* postResize                */ _NW_LMgr_Box_PostResize,
    /* getMinimumContentSize     */ _NW_LMgr_ImgContainerBox_GetMinimumContentSize,
    /* hasFixedContentSize       */ _NW_LMgr_MediaBox_HasFixedContentSize,
    /* constrain                 */ _NW_LMgr_Box_Constrain,
    /* draw                      */ _NW_LMgr_AnimatedImageBox_Draw,
    /* render                    */ _NW_LMgr_Box_Render,
    /* getBaseline               */ _NW_LMgr_Box_GetBaseline,
    /* shift                     */ _NW_LMgr_Box_Shift,
    /* clone                     */ _NW_LMgr_Box_Clone
  },
  { /* NW_LMgr_MediaBox          */
    /* unused                    */ NW_Object_Unused
  },
  { /* NW_LMgr_ImgContainerBox   */
    /* unused                    */ NW_Object_Unused
  },
  { /* NW_LMgr_AnimatedImageBox  */
    /* unused                    */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- */
const NW_Object_Class_t* const _NW_LMgr_AnimatedImageBox_SecondaryList[] = {  
  &NW_LMgr_AnimatedImageBox_AnimatedImageAggregate_Class,
  NULL
};

/* ------------------------------------------------------------------------- */
const NW_LMgr_AnimatedImageAggregate_Class_t NW_LMgr_AnimatedImageBox_AnimatedImageAggregate_Class = {
  { /* NW_Object_Core                 */
    /* super                          */ &NW_LMgr_AnimatedImageAggregate_Class,
    /* querySecondary                 */ _NW_Object_Core_QuerySecondary
  },
  { /* NW_Object_Secondary            */
    /* offset                         */ offsetof (NW_LMgr_AnimatedImageBox_t,
                                                   NW_LMgr_AnimatedImageAggregate)
  },
  { /* NW_Object_Aggregate            */
    /* secondaryList                  */ NULL,
    /* construct                      */ NULL,
    /* destruct                       */ NULL
  },
  { /* NW_LMgr_AnimatedImageAggregate */
    /* animate                        */ _NW_LMgr_AnimatedImageAggregate_Animate,
    /* getImage                       */ _NW_LMgr_AnimatedImageBox_AnimatedImageAggregate_GetImage, 
    /* startTimer                     */ _NW_LMgr_AnimatedImageAggregate_StartTimer
  }
  
};

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_LMgr_AnimatedImageBox_Construct(NW_Object_Dynamic_t* dynamicObject,
                                    va_list* argp)
  {
  NW_TRY (status) 
    {
    NW_LMgr_AnimatedImageBox_t* animatedImageBox;

    NW_ASSERT(dynamicObject != NULL);

    // invoke our superclass constructor
    status = _NW_LMgr_ImgContainerBox_Construct (dynamicObject, argp);
    NW_THROW_ON_ERROR(status);

    // for convenience
    animatedImageBox = NW_LMgr_AnimatedImageBoxOf( dynamicObject );
   
    // initialize the member variables
    animatedImageBox->iIsSuspended = NW_FALSE; 
    NW_LMgr_AnimatedImageAggregate_t* AnimatedImageAggregate = (NW_LMgr_AnimatedImageAggregate_t *)
              NW_Object_QuerySecondary (animatedImageBox, &NW_LMgr_AnimatedImageAggregate_Class);    
    AnimatedImageAggregate->nowSet = NW_FALSE;
    } 
  NW_CATCH (status) 
    {
    } 
  NW_FINALLY 
    {
    return status;
    } 
  NW_END_TRY
  }

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------*/
TBrowserStatusCode
_NW_LMgr_AnimatedImageBox_Draw (NW_LMgr_Box_t* box,
                                CGDIDeviceContext* deviceContext,
                                NW_Uint8 hasFocus)
  {  
  NW_LMgr_AnimatedImageBox_t* thisObj;
  NW_LMgr_AnimatedImageAggregate_t* AnimatedImageAggregate;

  NW_TRY(status) 
    {
    // for convenience
    thisObj = NW_LMgr_AnimatedImageBoxOf (box);

    if (!thisObj->iIsSuspended)
      {
      // invoke our superclass draw method 
      status = NW_LMgr_ImgContainerBox_Class.NW_LMgr_Box.draw (box, deviceContext,
                                                        hasFocus);
      NW_THROW_ON_ERROR(status);
    
      // support for animations. Note the aggregate takes over all reponsibilites
      // associated with animations. the box need not be concerned about timers etc 

      // get the aggregate object 
      AnimatedImageAggregate = (NW_LMgr_AnimatedImageAggregate_t *)
              NW_Object_QuerySecondary (thisObj, &NW_LMgr_AnimatedImageAggregate_Class);    
	
      if(NW_Object_IsInstanceOf(box, &LMgrMarkerImage_Class))
      {
      	status = NW_LMgr_AnimatedImageAggregate_StartTimer (AnimatedImageAggregate); 
      } 


      NW_THROW_ON_ERROR(status);
      }
    }
  NW_CATCH(status) 
    {
    }
  NW_FINALLY 
    { 
    return status;
    }
  NW_END_TRY
  }

/* ------------------------------------------------------------------------- *
   implemented interface/aggregate methods
 * ------------------------------------------------------------------------- */

NW_Image_AbstractImage_t*
_NW_LMgr_AnimatedImageBox_AnimatedImageAggregate_GetImage (NW_LMgr_AnimatedImageAggregate_t* animatedImageAggregate)
  {
  NW_LMgr_AnimatedImageBox_t* thisObj;
  NW_LMgr_ImgContainerBox_t* imageBox;

  /* obtain the aggregator */
  thisObj =  (NW_LMgr_AnimatedImageBox_t *) 
        NW_Object_Aggregate_GetAggregator (animatedImageAggregate);
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_LMgr_AnimatedImageBox_Class));

  imageBox = NW_LMgr_ImgContainerBoxOf( thisObj );
  return imageBox->virtualImage;
  }


/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
void
NW_LMgr_AnimatedImageBox_Suspend (NW_LMgr_AnimatedImageBox_t* aAnimatedBox)
  {
  NW_LMgr_AnimatedImageAggregate_t* animatedImageAggregate;
  NW_Image_AbstractImage_t* image;

  NW_ASSERT(aAnimatedBox);
  
  animatedImageAggregate = (NW_LMgr_AnimatedImageAggregate_t *)
              NW_Object_QuerySecondary (aAnimatedBox, &NW_LMgr_AnimatedImageAggregate_Class);

  image = NW_LMgr_AnimatedImageAggregate_GetImage (animatedImageAggregate);

  if (NW_Image_AbstractImage_IsAnimated(image)) 
    {
    aAnimatedBox->iIsSuspended = NW_TRUE;

    NW_LMgr_AnimatedImageAggregate_DestroyTimer (animatedImageAggregate);
    }
  }

/* ------------------------------------------------------------------------- */
void
NW_LMgr_AnimatedImageBox_Resume (NW_LMgr_AnimatedImageBox_t* aAnimatedBox)
  {
  NW_LMgr_AnimatedImageAggregate_t* animatedImageAggregate;
  NW_Image_AbstractImage_t* image;
  NW_Uint32 delay;

  NW_ASSERT(aAnimatedBox);
  
  animatedImageAggregate = (NW_LMgr_AnimatedImageAggregate_t *)
            NW_Object_QuerySecondary (aAnimatedBox, &NW_LMgr_AnimatedImageAggregate_Class);

  image = NW_LMgr_AnimatedImageAggregate_GetImage (animatedImageAggregate);

  if (NW_Image_AbstractImage_IsAnimated(image)) 
    {
    aAnimatedBox->iIsSuspended = NW_FALSE;

    delay = NW_Image_AbstractImage_GetDelay(image);

    NW_LMgr_AnimatedImageAggregate_CreateTimer (animatedImageAggregate, delay);
    }
  }

/* ------------------------------------------------------------------------- */
NW_LMgr_AnimatedImageBox_t*
NW_LMgr_AnimatedImageBox_New (NW_ADT_Vector_Metric_t numProperties,
                              NW_Image_AbstractImage_t* virtualImage, 
                             NW_Text_t* altText, NW_Bool brokenImage )
  {
  return (NW_LMgr_AnimatedImageBox_t*)
    NW_Object_New (&NW_LMgr_AnimatedImageBox_Class, numProperties, virtualImage, altText, brokenImage );
  }
