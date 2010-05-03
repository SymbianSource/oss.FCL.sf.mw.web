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


#include "nw_lmgr_animatedimageaggregatei.h"
#include "nw_lmgr_animatedimagebox.h"
#include "nw_lmgr_rootbox.h"
#include "GDIDevicecontext.h"
#include "BrsrStatusCodes.h"
#include "nw_image_epoc32simpleimagei.h"
#include "nw_image_virtualimagei.h"
#include "Epoc32ImageDecoder.h"

/* ------------------------------------------------------------------------- *
   forward method declarations
 * ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_LMgr_AnimatedImageAggregate_TimerCallback (void *timerData);

/* ------------------------------------------------------------------------- *
   private methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_LMgr_AnimatedImageAggregate_CreateTimer (NW_LMgr_AnimatedImageAggregate_t* thisObj,
                                            NW_Uint32 delay) 
{
  NW_TRY (status) {
    if (thisObj->timer == NULL) {

    if (delay < 200)
    {
        delay = 200; // hack: set a floor of 200 milliseconds for the timer
                     // set the floor higher if you need to.
    }
      thisObj->timer =
        NW_System_Timer_New (NW_LMgr_AnimatedImageAggregate_TimerCallback, 
                             thisObj, delay, NW_TRUE);
      NW_THROW_ON_NULL (thisObj->timer, status, KBrsrOutOfMemory);
    }
  }

  NW_CATCH (status) { /* empty */ }

  NW_FINALLY {
    return status;
  } NW_END_TRY
}


/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_LMgr_AnimatedImageAggregate_AnimateImage (NW_LMgr_AnimatedImageAggregate_t* thisObj) 
  {
  NW_LMgr_Box_t* imageBox;
  NW_Image_AbstractImage_t* image;
  NW_Int16 loopCount;

  NW_TRY(status) 
    {
    /* obtain the aggregator */
    imageBox =  (NW_LMgr_Box_t*) NW_Object_Aggregate_GetAggregator(thisObj);

    /* obtain the image*/
    image = NW_LMgr_AnimatedImageAggregate_GetImage (thisObj);

    NW_GDI_Rectangle_t imageBoxRect;

    /*destroy the timer so we can change the delay*/
    NW_LMgr_AnimatedImageAggregate_DestroyTimer(thisObj);

    // check if the imageBox is visible to make sure that we do not 
    // animate images which are out of the view.
    if( NW_LMgr_Box_GetVisibleBounds( imageBox, &imageBoxRect ) == NW_TRUE )
      {
      NW_LMgr_RootBox_t* rootBox;
      NW_GDI_Rectangle_t viewRect;
      CGDIDeviceContext* deviceContext;
      // we need the rootbox to get the device context which then gives us the
      // current view coordinates
      rootBox = NW_LMgr_Box_GetRootBox( imageBox );
      NW_THROW_ON_NULL( rootBox, status, KBrsrFailure );

      deviceContext = NW_LMgr_RootBox_GetDeviceContext( rootBox );
      NW_THROW_ON_NULL (deviceContext, status, KBrsrFailure);

      viewRect = deviceContext->ClipRect ();
      /*go to the next frame*/
      status = NW_Image_AbstractImage_IncrementImage(image);

      // check if the image box is in the view
      if( !NW_GDI_Rectangle_Cross( &imageBoxRect, &viewRect, NULL ) )
        {
        // off view
        NW_THROW_SUCCESS( status );
        }
      }
  
    if (status == KBrsrSuccess)
      {   
      loopCount = NW_Image_AbstractImage_GetLoopCount(image);

      if((loopCount == -1) || (loopCount > 0)) 
        {
        /*display the frame*/
        status = NW_LMgr_Box_Refresh(imageBox);
        _NW_THROW_ON_ERROR(status);
  
        }
      else
        {
        /*looping is completed*/
        NW_LMgr_AnimatedImageAggregate_DestroyTimer(thisObj);
		NW_Image_Epoc32Simple_t* simpleImage = NW_Image_Epoc32SimpleOf (NW_Image_VirtualOf (image)->image);
		CEpoc32ImageDecoder* decoder = STATIC_CAST(CEpoc32ImageDecoder*, simpleImage->decoder);
		decoder->ResetImage();
        }
      }
    else
      {
      /* The previous frame did not finish decoding, give it more time */
      status = NW_LMgr_AnimatedImageAggregate_CreateTimer(thisObj, 5);
      _NW_THROW_ON_ERROR(status);
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
   private static methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_LMgr_AnimatedImageAggregate_TimerCallback (void *timerData)
{
  NW_LMgr_AnimatedImageAggregate_t *thisObj;
  NW_LMgr_AnimatedImageBox_t* box;
  NW_LMgr_RootBox_t* rootBox;
  NW_LMgr_Box_t* theBox;

  // cast timerData to a aggregate */
  thisObj = NW_LMgr_AnimatedImageAggregateOf (timerData);
  
  TTime timeNow;
  timeNow.UniversalTime();
  TInt64 now = timeNow.Int64();
  thisObj->nowHigh = I64HIGH(now);
  thisObj->nowLow = I64LOW(now);
  thisObj->nowSet = NW_TRUE;
  
  theBox = (NW_LMgr_Box_t*) NW_Object_Aggregate_GetAggregator (thisObj);
  if (NW_Object_IsInstanceOf (theBox, &NW_LMgr_AnimatedImageBox_Class))
    {
    // obtain the aggregator */
    box = (NW_LMgr_AnimatedImageBox_t*) NW_Object_Aggregate_GetAggregator (thisObj);

    // if the animation is suspended we do not draw
    if (box->iIsSuspended)
      {
      return KBrsrSuccess;
      }
    }

  // if the box tree is not visible, we do not draw */
  rootBox = NW_LMgr_Box_GetRootBox (NW_LMgr_BoxOf(theBox));
  if (rootBox == NULL || !NW_LMgr_RootBox_HasFocus (rootBox)) 
    {
    return KBrsrSuccess;
    }

  (void) NW_LMgr_AnimatedImageAggregate_AnimateImage (thisObj);
	return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_LMgr_AnimatedImageAggregate_Class_t NW_LMgr_AnimatedImageAggregate_Class = {
  { /* NW_Object_Core                 */
    /* super                          */ &NW_Object_Aggregate_Class,
    /* querySecondary                 */ _NW_Object_Core_QuerySecondary
  },
  { /* NW_Object_Secondary            */
    /* offset                         */ 0
  },
  { /* NW_Object_Aggregate            */
    /* secondaryList                  */ NULL,
    /* construct                      */ NULL,
    /* destruct                       */ _NW_LMgr_AnimatedImageAggregate_Destruct
  },
  { /* NW_LMgr_AnimatedImageAggregate */
    /* animate                        */ _NW_LMgr_AnimatedImageAggregate_Animate,
    /* getImage                       */ NULL,
    /* startTimer                     */ _NW_LMgr_AnimatedImageAggregate_StartTimer
  }
  
};


/* ------------------------------------------------------------------------- *
   NW_Object_Aggregate method(s)
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
void
_NW_LMgr_AnimatedImageAggregate_Destruct (NW_Object_Aggregate_t* aggregate)
{
  NW_LMgr_AnimatedImageAggregate_t* thisObj;

  /* for convenience */
  thisObj = NW_LMgr_AnimatedImageAggregateOf (aggregate);

  /* destroy the timer */
  NW_LMgr_AnimatedImageAggregate_DestroyTimer (thisObj);
}

/* ------------------------------------------------------------------------- *
   NW_LMgr_AnimatedImageAggregate method(s)
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_LMgr_AnimatedImageAggregate_Animate (NW_LMgr_AnimatedImageAggregate_t* /*thisObj*/) 
  {  
  return KBrsrSuccess;
  }

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_LMgr_AnimatedImageAggregate_StartTimer (NW_LMgr_AnimatedImageAggregate_t* thisObj) 
  {  
  NW_Image_AbstractImage_t* image;
  NW_Uint32 delay;

  // obtain the image
  image = NW_LMgr_AnimatedImageAggregate_GetImage( thisObj );

  NW_Image_Epoc32Simple_t* simpleImage = NW_Image_Epoc32SimpleOf (NW_Image_VirtualOf (image)->image);
  CEpoc32ImageDecoder* decoder = STATIC_CAST(CEpoc32ImageDecoder*, simpleImage->decoder);
  
  if (!NW_Image_AbstractImage_IsAnimated(image) || (decoder && decoder->Finished()))
    {
    // not an animated image
    return KBrsrSuccess; 
    }
  
  
  // get the delay
  delay = NW_Image_AbstractImage_GetDelay(image);
  NW_Uint32 subDelay;
  
// Subtract the time that passed since the timet callbacl was called
  NW_LMgr_AnimatedImageAggregate_t* AnimatedImageAggregate = (NW_LMgr_AnimatedImageAggregate_t *)
              NW_Object_QuerySecondary (thisObj, &NW_LMgr_AnimatedImageAggregate_Class);    
  if (AnimatedImageAggregate->nowSet == NW_TRUE)
    {
    TTime timeNow;
    timeNow.UniversalTime();
    TInt64 now = timeNow.Int64();
    TInt64 callbackTime = MAKE_TINT64(thisObj->nowHigh, thisObj->nowLow);
    TInt64 theDelay = delay - (now - callbackTime)/1000;
    if (theDelay < 5)
    	{
  		theDelay = 5;
    	}
    subDelay = (NW_Uint32) theDelay;
    }//if nowSet
  else
    {
    subDelay = delay;    	
    }

  AnimatedImageAggregate->nowSet = NW_FALSE;

  // start the timer   
  NW_LMgr_AnimatedImageAggregate_CreateTimer (thisObj, subDelay);    

  // animation started
  thisObj->started = NW_TRUE;
  
  return KBrsrSuccess;
  }

/* ------------------------------------------------------------------------- */
void
NW_LMgr_AnimatedImageAggregate_DestroyTimer (NW_LMgr_AnimatedImageAggregate_t* thisObj) 
{
  NW_Object_Delete (thisObj->timer);
  thisObj->timer = NULL;
}
