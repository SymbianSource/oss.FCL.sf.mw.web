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


#include "BrsrStatusCodes.h"
#include "nw_lmgr_activecontainerboxi.h"
#include "nw_lmgr_cssproperties.h"
#include "nw_lmgr_propertylist.h"
#include "nw_lmgr_simplepropertylist.h"
#include "nw_lmgr_slavepropertylist.h"
#include "nw_sound.h"
#include "nw_sound_contenthandler.h"

/* ------------------------------------------------------------------------- *
   global data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
static
const
NW_LMgr_IEventListener_Class_t _NW_LMgr_ActiveContainerBox_IEventListener_Class = {
  { /* NW_Object_Core         */
    /* super                  */ &NW_LMgr_IEventListener_Class,
    /* queryInterface         */ _NW_Object_Interface_QueryInterface
  },
  { /* NW_Object_Interface    */
    /* offset                 */ offsetof (NW_LMgr_ActiveContainerBox_t, NW_LMgr_IEventListener)
  },
  { /* NW_LMgr_IEventListener */
    /* processEvent           */ _NW_LMgr_ActiveContainerBox_IEventListener_ProcessEvent
  }
};

/* ------------------------------------------------------------------------- */
static
const NW_Object_Class_t* const _NW_LMgr_ActiveContainerBox_InterfaceList[] = {
  &_NW_LMgr_ActiveContainerBox_IEventListener_Class,
  NULL
};

/* ------------------------------------------------------------------------- */
const
NW_LMgr_ActiveContainerBox_Class_t  NW_LMgr_ActiveContainerBox_Class = {
  { /* NW_Object_Core             */
    /* super                      */ &NW_LMgr_ContainerBox_Class,
    /* queryInterface             */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base             */
    /* interfaceList              */ _NW_LMgr_ActiveContainerBox_InterfaceList
  },
  { /* NW_Object_Dynamic          */
    /* instanceSize               */ sizeof (NW_LMgr_ActiveContainerBox_t),
    /* construct                  */ _NW_LMgr_ActiveContainerBox_Construct,
    /* destruct                   */ _NW_LMgr_ActiveContainerBox_Destruct
  },
  { /* NW_LMgr_Box                */
    /* split                      */ _NW_LMgr_Box_Split,
    /* resize                     */ _NW_LMgr_ContainerBox_Resize,
    /* postResize                 */ _NW_LMgr_Box_PostResize,
    /* getMinimumContentSize      */ _NW_LMgr_Box_GetMinimumContentSize,
    /* hasFixedContentSize        */ _NW_LMgr_Box_HasFixedContentSize,
    /* constrain                  */ _NW_LMgr_Box_Constrain,
    /* draw                       */ _NW_LMgr_Box_Draw,
    /* render                     */ _NW_LMgr_ContainerBox_Render,
    /* getBaseline                */ _NW_LMgr_ContainerBox_GetBaseline,
    /* shift                      */ _NW_LMgr_ContainerBox_Shift,
    /* clone                      */ _NW_LMgr_ActiveContainerBox_Clone
  },
  { /* NW_LMgr_ContainerBox       */
    /* unused                     */ NW_Object_Unused
  },
  { /* NW_LMgr_ActiveContainerBox */
    /* unused                     */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_LMgr_ActiveContainerBox_Construct (NW_Object_Dynamic_t* dynamicObject,
                                       va_list* argp)
{
  NW_LMgr_ActiveContainerBox_t* thisObj;
  TBrowserStatusCode status;

  /* for convenience */
  thisObj = NW_LMgr_ActiveContainerBoxOf (dynamicObject);

  /* invoke our superclass constructor */
  status = _NW_LMgr_ContainerBox_Construct (dynamicObject, argp);
  if (status != KBrsrSuccess) {
    return status;
  }

  /* initialize the member variables */
  thisObj->eventHandler = va_arg (*argp, NW_LMgr_EventHandler_t*);
  thisObj->actionType = (NW_LMgr_ActiveContainerBox_ActionType_t) va_arg (*argp, NW_Uint32 );
  thisObj->hrefAttr = NULL;
  thisObj->iName = NULL;
  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
void
_NW_LMgr_ActiveContainerBox_Destruct (NW_Object_Dynamic_t* dynamicObject)
    {
    NW_LMgr_ActiveContainerBox_t* thisObj;
    NW_LMgr_PropertyList_t* propertyList;
    NW_LMgr_Property_t prop;

    /* for convenience */
    thisObj = NW_LMgr_ActiveContainerBoxOf (dynamicObject);

    propertyList = NW_LMgr_Box_PropListGet(NW_LMgr_BoxOf(thisObj));

    if (NW_Object_IsInstanceOf(propertyList, &NW_LMgr_SimplePropertyList_Class))
        {
        // Get the sound content handler if there's one
        // Only for mater box
        TBrowserStatusCode status = NW_LMgr_Box_GetRawProperty( NW_LMgr_BoxOf(thisObj), 
                            NW_CSS_Prop_contentHandlerId, &prop );
        if ( status == KBrsrSuccess )
            {
            NW_Sound_ContentHandler_t* aSoundCH = (NW_Sound_ContentHandler_t*)prop.value.object;
            // Because several active container box might share one SoundCH
            // To avoid multiple deleting of the SoundCH:
            // Before the ref count is greater than 1, just remove the property
            // Only when the ref count is decreased to 1, SoundCH property will be deleted
            if( aSoundCH->iBoxRefCnt > 1 )
                {
                NW_LMgr_Box_RemovePropertyWithoutDelete( NW_LMgr_BoxOf(thisObj), 
                                                         NW_CSS_Prop_contentHandlerId );
                }
            // Reduce ref count, which could go down to 0
            aSoundCH->iBoxRefCnt--;
            }

        /* if we are not a clone, destroy the EventHandler */
        NW_Object_Delete (thisObj->eventHandler);
        }

    NW_Object_Delete(thisObj->hrefAttr);
    NW_Object_Delete(thisObj->iName);
    }

/* ------------------------------------------------------------------------- *
 * Clones of active containers inherit the property list
 * and the event handler.  The children are not copied into the new box.
 */
NW_LMgr_Box_t*
_NW_LMgr_ActiveContainerBox_Clone (NW_LMgr_Box_t* box)
{
  NW_LMgr_ActiveContainerBox_t* thisObj;
  NW_LMgr_ActiveContainerBox_t* newActiveContainerBox = NULL;
  NW_LMgr_Box_t* newLMgrBox = NULL;

  /* parameter assertion block */
  NW_ASSERT(NW_Object_IsInstanceOf (box, &NW_LMgr_ActiveContainerBox_Class));

  /* for convenience */
  thisObj = NW_LMgr_ActiveContainerBoxOf (box);

  /* Create the new box */
  newActiveContainerBox =
    NW_LMgr_ActiveContainerBox_New (0, thisObj->eventHandler, thisObj->actionType);
  if (newActiveContainerBox == NULL) {
    return NULL;
  }
  NW_LMgr_ActiveContainerBox_SetName( newActiveContainerBox, thisObj->iName, NW_FALSE );

  // Get the LMgr box OOC object for the new box.
  newLMgrBox = NW_LMgr_BoxOf(newActiveContainerBox);

  /* Copy the property list, if one exists */
  if (box->propList != NULL) {
    newLMgrBox->propList = NW_LMgr_PropertyList_Clone (box->propList);
  }
  else{
    /* instantiate a new SlavePropertyList object */
    newLMgrBox->propList = (NW_LMgr_PropertyList_t*)
              NW_LMgr_SlavePropertyList_New (NULL);
  }
  if (newLMgrBox->propList == NULL) {
    NW_Object_Delete (newActiveContainerBox);
    return NULL;
  }

  return newLMgrBox;
}

/* ------------------------------------------------------------------------- *
   implemented interface methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_Uint8
_NW_LMgr_ActiveContainerBox_IEventListener_ProcessEvent (NW_LMgr_IEventListener_t* eventListener,
                                                         NW_Evt_Event_t* event)
{
  NW_LMgr_ActiveContainerBox_t* thisObj;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (eventListener,
                                     &NW_LMgr_IEventListener_Class));
  NW_ASSERT (NW_Object_IsInstanceOf (event, &NW_Evt_Event_Class));

  /* obtain the implementer */
  thisObj = (NW_LMgr_ActiveContainerBox_t *) NW_Object_Interface_GetImplementer (eventListener);
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_LMgr_ActiveContainerBox_Class));

  /* notify the event handler (if there is one) */
  if (thisObj->eventHandler != NULL) {
    return NW_LMgr_EventHandler_ProcessEvent (thisObj->eventHandler,
                                              NW_LMgr_BoxOf (thisObj), event);
  }

  /* if we get this far, then the event was not absorbed */
  return 0;
}

/* ------------------------------------------------------------------------- *
   convenience methods
 * ------------------------------------------------------------------------- */

// -----------------------------------------------------------------------------
// NW_LMgr_ActiveContainerBox::New
// Creates a new file selection box.
// (Other items are commented in header.)
// -----------------------------------------------------------------------------
//
NW_LMgr_ActiveContainerBox_t*
NW_LMgr_ActiveContainerBox_New (NW_ADT_Vector_Metric_t numProperties,
                             NW_LMgr_EventHandler_t* eventHandler,
                             NW_LMgr_ActiveContainerBox_ActionType_t aActionType)
{
  return (NW_LMgr_ActiveContainerBox_t*)
    NW_Object_New (&NW_LMgr_ActiveContainerBox_Class, numProperties, eventHandler, (NW_Uint32) aActionType);
}

// -----------------------------------------------------------------------------
// NW_LMgr_ActiveContainerBox::SetName
// Sets the name of this active container box. The name is used by ECMA script.
// (Other items are commented in header.)
// -----------------------------------------------------------------------------
//
void
NW_LMgr_ActiveContainerBox_SetName( NW_LMgr_ActiveContainerBox_t* aThisObj,
                                   NW_Text_t* aName, NW_Bool aTakeOwnershipOfNameObject )
    {
    // If not taking ownership of name object then make a deep copy. One way or
    // another, this object will own the name object it has.
    if (aName && !aTakeOwnershipOfNameObject)
        {
        // If out of memory occurs, aName will end up being NULL.
        aName = NW_Text_Copy(aName, NW_TRUE);
        }
    aThisObj->iName = aName;
    }

// -----------------------------------------------------------------------------
// Gets the soundstart state from this active container box, if present.
// Soundstart is a proprietary customer feature of anchor tags. The soundstart
// state is stored as CSS properties of this box. Note, the soundstart src URL
// is NOT stored and is returned as NULL in the soundstartState structure.
// (Other items are commented in header.)
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
NW_LMgr_ActiveContainerBox_GetSoundstartState( NW_LMgr_ActiveContainerBox_t* aThisObj,
    NW_Sound_ContentHandler_t** aSoundCH, TSoundstartState* aSoundstartState )
    {
    TBrowserStatusCode status;
    NW_LMgr_Box_t* lmgrBox;
    NW_LMgr_PropertyValue_t value;

    // Parameter assertion block.
    NW_ASSERT( aThisObj );
    NW_ASSERT( aSoundstartState );

    lmgrBox = NW_LMgr_BoxOf( aThisObj );

    // Get the content handler that has the sound content.
    *aSoundCH = NULL;
    status = NW_LMgr_Box_GetPropertyValue( lmgrBox, NW_CSS_Prop_contentHandlerId,
        NW_CSS_ValueType_Object, &value );
    if ( status != KBrsrSuccess )
        {
        goto cleanup;
        }
    *aSoundCH = (NW_Sound_ContentHandler_t*)value.object;

    // Get the soundstart action.
    status = NW_LMgr_Box_GetPropertyValue( lmgrBox, NW_CSS_Prop_soundstartAction,
        NW_CSS_ValueType_Integer, &value );
    if ( status != KBrsrSuccess )
        {
        goto cleanup;
        }
    aSoundstartState->action = (TSoundstartAction)value.integer;

    // Get the soundstart loop.
    status = NW_LMgr_Box_GetPropertyValue( lmgrBox, NW_CSS_Prop_soundstartLoop,
        NW_CSS_ValueType_Integer, &value );
    if ( status != KBrsrSuccess )
        {
        goto cleanup;
        }
    aSoundstartState->loop = (NW_Int16)value.integer;

    // Get the soundstart volume.
    status = NW_LMgr_Box_GetPropertyValue( lmgrBox, NW_CSS_Prop_soundstartVolume,
        NW_CSS_ValueType_Integer, &value );
    if ( status != KBrsrSuccess )
        {
        goto cleanup;
        }
    aSoundstartState->volume = (TSoundstartVolume)value.integer;

cleanup:
    return status;
    }

// -----------------------------------------------------------------------------
// NW_LMgr_ActiveContainerBox::SetSoundstartState
// Sets the soundstart state of this active container box. Soundstart is a
// proprietary customer feature of anchor tags. The soundstart state is stored
// as CSS properties of this box. Note, the soundstart src URL is NOT set.
// (Other items are commented in header.)
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
NW_LMgr_ActiveContainerBox_SetSoundstartState( NW_LMgr_ActiveContainerBox_t* aThisObj,
    NW_DOM_ElementNode_t* aElementNode, TSoundstartState* aSoundstartState )
    {
    TBrowserStatusCode status = KBrsrSuccess;
    NW_LMgr_Box_t* lmgrBox;
    NW_LMgr_Property_t cssProp;

    // Parameter assertion block.
    NW_ASSERT( aThisObj );
    NW_ASSERT( aSoundstartState );

    lmgrBox = NW_LMgr_BoxOf( aThisObj );
    NW_ASSERT( lmgrBox );

    // Add the element node address (its ID) as a CSS property. This will allow
    // the sound content handler to find the corresponding box. The sound
    // content handler will look for this box when the sound has loaded and it
    // will then link the box back to the sound content.
    cssProp.type = NW_CSS_ValueType_Object;
    cssProp.value.object = aElementNode;
    status = NW_LMgr_Box_SetProperty( lmgrBox, NW_CSS_Prop_elementId, &cssProp );
    if ( status != KBrsrSuccess )
        {
        goto cleanup;
        }

    // Set the soundstart action as a CSS property.
    cssProp.type = NW_CSS_ValueType_Integer;
    cssProp.value.integer = (NW_Int32)aSoundstartState->action;
    status = NW_LMgr_Box_SetProperty( lmgrBox, NW_CSS_Prop_soundstartAction, &cssProp );
    if ( status != KBrsrSuccess )
        {
        goto cleanup;
        }

    // Set the soundstart loop as a CSS property.
    cssProp.type = NW_CSS_ValueType_Integer;
    cssProp.value.integer = (NW_Int32)aSoundstartState->loop;
    status = NW_LMgr_Box_SetProperty( lmgrBox, NW_CSS_Prop_soundstartLoop, &cssProp );
    if ( status != KBrsrSuccess )
        {
        goto cleanup;
        }

    // Set the soundstart volume as a CSS property.
    cssProp.type = NW_CSS_ValueType_Integer;
    cssProp.value.integer = (NW_Int32)aSoundstartState->volume;
    status = NW_LMgr_Box_SetProperty( lmgrBox, NW_CSS_Prop_soundstartVolume, &cssProp );
    if ( status != KBrsrSuccess )
        {
        goto cleanup;
        }

cleanup:
    return status;
    }
