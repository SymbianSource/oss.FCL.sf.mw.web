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
* Description:  Applies CSS property 
*
*/



// INCLUDE FILES
#include <e32def.h>  // First to avoid NULL redefine warning (no #ifndef NULL).

#include <e32base.h>
#include "nwx_assert.h"
#include "CSSDeclListHandle.h"
#include "nw_lmgr_simplepropertylist.h"
#include "nw_text_ucs2.h"
#include "nw_lmgr_rootbox.h"
#include "CSSPropVal.h"
#include "nwx_math.h"
#include "nwx_string.h"
#include "CSSReader.h" 
#include "BrsrStatusCodes.h"
 
// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

#define TCSSDeclListHandleMaxVals 6

enum TCSSDeclListHandleBoxModelSides
{
  SHORTHAND,
  TOP,
  BOTTOM,
  LEFT,
  RIGHT
};

static const NW_LMgr_PropertyName_t borderProps[] = 
{
  NW_CSS_Prop_border,
  NW_CSS_Prop_borderTop,
  NW_CSS_Prop_borderBottom,
  NW_CSS_Prop_borderLeft,
  NW_CSS_Prop_borderRight,
};

static const NW_LMgr_PropertyName_t borderWidthProps[] = 
{
  NW_CSS_Prop_borderWidth,
  NW_CSS_Prop_topBorderWidth,
  NW_CSS_Prop_bottomBorderWidth,
  NW_CSS_Prop_leftBorderWidth,
  NW_CSS_Prop_rightBorderWidth,
};

static const NW_LMgr_PropertyName_t borderStyleProps[] = 
{
  NW_CSS_Prop_borderStyle,
  NW_CSS_Prop_topBorderStyle,
  NW_CSS_Prop_bottomBorderStyle,
  NW_CSS_Prop_leftBorderStyle,
  NW_CSS_Prop_rightBorderStyle,
};

static const NW_LMgr_PropertyName_t borderColorProps[] = 
{
  NW_CSS_Prop_borderColor,
  NW_CSS_Prop_topBorderColor,
  NW_CSS_Prop_bottomBorderColor,
  NW_CSS_Prop_leftBorderColor,
  NW_CSS_Prop_rightBorderColor,
};

static const NW_LMgr_PropertyName_t paddingProps[] = 
{
  NW_CSS_Prop_padding,
  NW_CSS_Prop_topPadding,
  NW_CSS_Prop_bottomPadding,
  NW_CSS_Prop_leftPadding,
  NW_CSS_Prop_rightPadding,
};

static const NW_LMgr_PropertyName_t marginProps[] = 
{
  NW_CSS_Prop_margin,
  NW_CSS_Prop_topMargin,
  NW_CSS_Prop_bottomMargin,
  NW_CSS_Prop_leftMargin,
  NW_CSS_Prop_rightMargin
};

static const NW_LMgr_PropertyName_t* const boxModelProps[] = 
{
  borderProps,
  borderWidthProps,
  borderStyleProps,
  borderColorProps,
  paddingProps,
  marginProps
};

// MODULE DATA STRUCTURES

// FORWARD DECLARATIONS

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

TBool TCSSDeclListHandle::ConvertValueL(NW_LMgr_PropertyName_t aPropName, 
                           TCSSPropertyVal *aInValue,
                           NW_LMgr_Property_t *aOutProp,
                           TCSSParser* aParser,
                           NW_CSS_Processor_t* aProcessor,
                           NW_LMgr_Box_t *aBox,
                           TInt aBufferIndex)
{
  NW_LMgr_PropertyValueToken_t token;
  TBool status = EFalse;
  TInt err;

  /* Find where the value is located */
  TUint32 encoding = aParser->GetEncoding();
  TText16* valStorage = aInValue->val.GetUnicodeL(encoding);

  if (valStorage != NULL)
  {
    switch (aInValue->type)
    {
    case URI:
      /* URIs are only used to load images */ 
      TRAP(err, status =  TCSSPropVal::SetImageValL(aPropName, aOutProp, valStorage, aProcessor, aBox, aBufferIndex ));
      if (err != KErrNone)
      {
        NW_Mem_Free(valStorage);
        User::Leave(err);
      }
      break;

    case RGB:
    case HASH:
      TRAP(err, status = TCSSPropVal::SetColorL(aParser, aOutProp, aInValue, valStorage));
      if (err != KErrNone)
      {
        NW_Mem_Free(valStorage);
        User::Leave(err);
      }
      break;

    case IDENTIFIER: 
      {
        TBool tokenFound = TCSSPropertyTable::GetPropertyValToken(valStorage, &token);
        if (tokenFound)
        {
          status = TCSSPropVal::SetTokenVal(aPropName, aOutProp, token, aBox);
          break;
        }
      }

      /* FALL THROUGH! */
      /*lint -fallthrough*/
      /* If the identifier is not recognized as a token, we interpret is
         as a string */
         
    case STRING:
      aOutProp->value.object =
        NW_Text_UCS2_New (valStorage, (TUint16)NW_Str_Strlen (valStorage),
                          NW_Text_Flags_TakeOwnership);
      aOutProp->type = NW_CSS_ValueType_Text;
      if (aOutProp->value.object == NULL)
      {
        NW_Mem_Free(valStorage);
        User::Leave(KErrNoMemory);
      }
      status = ETrue;
      break;

    default:
      {
        /* In all other cases, we assume that we are dealing with a numeric value */
        TText16* endPtr = NULL;
        NW_Float32 v;

        /* TODO valStorage could be a real number not just an integer */
        TBrowserStatusCode cnvt = NW_Str_CvtToFloat32(valStorage, &endPtr, &v);
        if (cnvt == KBrsrSuccess)
        {
          status = TCSSPropVal::SetNumberPropVal(aPropName, aOutProp, aInValue->type, v);
        }
        break;
      }
    }

    if ((aOutProp->type != NW_CSS_ValueType_Text) || (!status)) 
    {
      NW_Mem_Free(valStorage);
    }
  }

  return status;
}

/* ------------------------------------------------------------------------- */
void 
TCSSDeclListHandle::ApplyCommonPropertyL(NW_LMgr_PropertyName_t aPropName, 
				 NW_LMgr_Property_t *aProperty,
				 TBool aImportant,
				 NW_LMgr_PropertyList_t* aAppliedList,
				 NW_LMgr_Box_t* aBox,
         TBool aDefaultStyle)
{
  NW_LMgr_Property_t prop;
  TUint32 typeMask;

  /* Get the property entry */
  const TCSSPropertyTablePropEntry* propEntry = TCSSPropertyTable::GetPropEntryByToken(aPropName);

  /* 
   Since rules are arranged by decreasing specificity, if a property is 
   already present, it can be replaced only if the other property is
   marked important.  However, if the property has been applied within the
   current declaration list, it SHOULD be overwritten.
  */
  TBrowserStatusCode status = NW_LMgr_Box_GetRawProperty(aBox, aPropName, &prop);
  if ((status == KBrsrSuccess) && (!aImportant)) 
  {
    /* This here for DefaultStyleSheet */
    if (aAppliedList == NULL)
    { 
	    return;
    }

    status = NW_LMgr_PropertyList_Get(aAppliedList, aPropName, &prop);
    if (status != KBrsrSuccess) 
    {//same property is not present in the current declaration list
      // but it was there on the box - so don't set it again
	    return;
    }
  }

  /* If the property value is of an allowed type, set the property */
  typeMask = 1 << aProperty->type;
  
  if ( ((typeMask & propEntry->type) != 0) ||
	      ((aProperty->type == NW_CSS_ValueType_Token) &&
	      (aProperty->value.integer == NW_CSS_PropValue_inherit)))
  {
    if (aDefaultStyle)
    {
      aProperty->type = (NW_LMgr_PropertyValueType_t)
                        (aProperty->type | NW_CSS_ValueType_DefaultStyle);
    }
	  status = NW_LMgr_Box_SetProperty(aBox, aPropName, aProperty);
	  if (status == KBrsrOutOfMemory)
    {
      User::Leave(KErrNoMemory);
    }

	  /* Now zero the object, so it does not get deleted by us */
	  if ((aProperty->type == NW_CSS_ValueType_Text) ||
	      (aProperty->type == NW_CSS_ValueType_Image)) 
    {
	    aProperty->value.object = NULL;
	  }

    /* Add the property to the appliedList list */
    if (aAppliedList != NULL) 
    {
      prop = *aProperty;
      prop.type =	(NW_LMgr_PropertyValueType_t)(prop.type | NW_CSS_ValueType_Copy);
      if (aDefaultStyle)
      {
        prop.type = (NW_LMgr_PropertyValueType_t)
                    (prop.type | NW_CSS_ValueType_DefaultStyle);
      }
      status = NW_LMgr_PropertyList_Set(aAppliedList, aPropName, &prop);
	    if (status == KBrsrOutOfMemory)
      {
        User::Leave(KErrNoMemory);
      }
    }
  }
  else 
  {
    /* Here we cut people some slack:  if the property expects a length,
	  and the value was specified without a unit, we assume it is a
	  pixel value. */
    if ( (aProperty->type == NW_CSS_ValueType_Number) &&
	       (propEntry->type & (1 << NW_CSS_ValueType_Px)) ) 
    {
	    aProperty->value.integer = (TInt32)aProperty->value.decimal;
	    aProperty->type = NW_CSS_ValueType_Px;

      if (aDefaultStyle)
      {
        aProperty->type = (NW_LMgr_PropertyValueType_t)
                        (aProperty->type | NW_CSS_ValueType_DefaultStyle);
      }

	    status = NW_LMgr_Box_SetProperty(aBox, aPropName, aProperty);
	    if (status == KBrsrOutOfMemory)
      {
        User::Leave(KErrNoMemory);
      }

	    /* Add the property to the appliedList */
	    if (aAppliedList != NULL) 
      {
	      prop = *aProperty;
	      prop.type = (NW_LMgr_PropertyValueType_t)(prop.type | NW_CSS_ValueType_Copy);

        if (aDefaultStyle)
        {
          prop.type = (NW_LMgr_PropertyValueType_t)(prop.type | NW_CSS_ValueType_DefaultStyle);
        }
	      status = NW_LMgr_PropertyList_Set(aAppliedList, aPropName, &prop);
	      if (status == KBrsrOutOfMemory)
        {
          User::Leave(KErrNoMemory);
        }
      }//if (aAppliedList != NULL) 
    }
  }//else
}

/* ------------------------------------------------------------------------- */
void 
TCSSDeclListHandle::ApplyBoxModelPropertyL(NW_LMgr_PropertyName_t aPropName, 
                           const NW_LMgr_PropertyName_t *aProps,
                           NW_LMgr_Property_t *aPropertyVals,
                           TUint8 aNumVals,
                           TBool aImportant,
                           NW_LMgr_PropertyList_t* aAppliedList,
                           NW_LMgr_Box_t* aBox,
                           TBool aDefaultStyle)
{
  TBool mustExpand = NW_FALSE;
  NW_LMgr_Property_t topValue, bottomValue, leftValue, rightValue;
  TCSSDeclListHandleBoxModelSides side;
  NW_LMgr_Property_t p, pCopy;
  TUint8 index;
  TBool isLocal = NW_FALSE;
  TBrowserStatusCode status;

  if ((aPropName == aProps[SHORTHAND]) && (aNumVals == 1)) 
  {
    side = TOP;
    while (side <= RIGHT)
    {
      status = NW_LMgr_Box_GetPropertyFromList(aBox, aProps[side], &p);
      if (status == KBrsrSuccess)
      {
        mustExpand = NW_TRUE;
        break;
      }
      side = (TCSSDeclListHandleBoxModelSides)(side + 1);
    }

    if (!mustExpand) 
    {
      TCSSDeclListHandle::ApplyCommonPropertyL(aProps[SHORTHAND], 
                          &(aPropertyVals[0]), aImportant, aAppliedList, aBox, aDefaultStyle);
      return;
    }
  }

  /* First check whether the shorthand property is already set */
  status = NW_LMgr_Box_GetPropertyFromList(aBox, aProps[SHORTHAND], &p);
  if (status == KBrsrSuccess) 
  {
    if (aAppliedList != NULL) 
    {
      status = NW_LMgr_PropertyList_Get(aAppliedList, aProps[SHORTHAND], &p);
      if (status == KBrsrSuccess) 
      {
        isLocal = NW_TRUE;
      }
    }
    if (aImportant || isLocal) 
    {
      (void)NW_LMgr_Box_RemoveProperty(aBox, aProps[SHORTHAND]);

      if (isLocal)
      {
        (void)NW_LMgr_PropertyList_Remove(aAppliedList, aProps[SHORTHAND]);
      }
      pCopy = p;
      pCopy.type = (NW_LMgr_PropertyValueType_t)(p.type | NW_CSS_ValueType_Copy);
      side = TOP;
      while (side <= RIGHT)
      {
        status = NW_LMgr_Box_SetProperty(aBox, aProps[side], &p);
	      if (status == KBrsrOutOfMemory)
        {
          User::Leave(KErrNoMemory);
        }
        if (isLocal)
        {
          status = NW_LMgr_PropertyList_Set(aAppliedList, aProps[side], &pCopy);
	        if (status == KBrsrOutOfMemory)
          {
            User::Leave(KErrNoMemory);
          }
        }
        side = (TCSSDeclListHandleBoxModelSides)(side + 1);
      }
    }// if (important || isLocal) 
    else 
    {
      /* Delete all the values */
      for (index = 0; index < aNumVals; index++) 
      {
        if (aPropertyVals[index].type == NW_CSS_ValueType_Text)
        {
          NW_Object_Delete(aPropertyVals[index].value.object);
        }
      }
      return;
    }
  }

  /* Now set the appropriate values */
  if (aPropName == aProps[SHORTHAND]) 
  {
    switch (aNumVals) 
    {
    case 1:
      topValue = aPropertyVals[0];
      bottomValue = aPropertyVals[0];
      leftValue = aPropertyVals[0];
      rightValue = aPropertyVals[0];
      break;

    case 2:
      topValue = aPropertyVals[0];
      bottomValue = aPropertyVals[0];
      leftValue = aPropertyVals[1];
      rightValue = aPropertyVals[1];
      break;

    case 3:
      topValue = aPropertyVals[0];
      bottomValue = aPropertyVals[2];
      leftValue = aPropertyVals[1];
      rightValue = aPropertyVals[1];
      break;

    default:
      topValue = aPropertyVals[0];
      bottomValue = aPropertyVals[2];
      leftValue = aPropertyVals[3];
      rightValue = aPropertyVals[1];
      break;
    }

    /* Finally, apply the properties */
     
    TCSSDeclListHandle::ApplyCommonPropertyL(aProps[TOP], &topValue, aImportant, aAppliedList, aBox, aDefaultStyle);

    TCSSDeclListHandle::ApplyCommonPropertyL(aProps[BOTTOM], &bottomValue, aImportant, aAppliedList, aBox, aDefaultStyle);

    TCSSDeclListHandle::ApplyCommonPropertyL(aProps[LEFT], &leftValue, aImportant, aAppliedList, aBox, aDefaultStyle);

    TCSSDeclListHandle::ApplyCommonPropertyL(aProps[RIGHT], &rightValue, aImportant, aAppliedList, aBox, aDefaultStyle);
  }
  else 
  {
    TCSSDeclListHandle::ApplyCommonPropertyL(aPropName, &(aPropertyVals[0]), aImportant, aAppliedList, aBox, aDefaultStyle);
  }
}

/* ------------------------------------------------------------------------- */
void 
TCSSDeclListHandle::ApplyBorderPropertyL(NW_LMgr_PropertyName_t aPropName, 
                                       NW_LMgr_Property_t *aPropertyVals,
                                       TUint8 aNumVals,
                                       TBool aImportant,
                                       NW_LMgr_PropertyList_t* aAppliedList,
                                       NW_LMgr_Box_t* aBox,
                                       TBool aDefaultStyle)
{
  //lint --e{661} Warning -- Possible access of out-of-bounds pointer, borderColorProps[boxSide]

  TUint8 boxSide;
  /* find box model property type */
  for (boxSide = SHORTHAND; boxSide <= RIGHT; boxSide ++) 
  {
    if (borderProps[boxSide] == aPropName) 
    {
      break;
    }
  }

  /* Go through the values and apply them */
  for (TUint8 index = 0; index < aNumVals && index < 3; index ++) 
  {
    
    switch (aPropertyVals[index].type) 
    {
    case NW_CSS_ValueType_Color:
      TCSSDeclListHandle::ApplyBoxModelPropertyL(borderColorProps[boxSide],
                                                borderColorProps, 
                                                &(aPropertyVals[index]), 1, 
                                                aImportant, aAppliedList, aBox, aDefaultStyle);
      break;

    case NW_CSS_ValueType_Token:
      if (aPropertyVals[index].value.token == NW_CSS_PropValue_inherit)
      {
        TCSSDeclListHandle::ApplyBoxModelPropertyL(borderWidthProps[boxSide], 
                                                  borderWidthProps,
                                                  &(aPropertyVals[index]), 1, 
                                                  aImportant, aAppliedList, aBox, aDefaultStyle);

        TCSSDeclListHandle::ApplyBoxModelPropertyL(borderStyleProps[boxSide], 
                                                  borderStyleProps,
                                                  &(aPropertyVals[index]), 1, 
                                                  aImportant, aAppliedList, aBox, aDefaultStyle);

        TCSSDeclListHandle::ApplyBoxModelPropertyL(borderColorProps[boxSide], 
                                                   borderColorProps,
                                                   &(aPropertyVals[index]), 1, 
                                                   aImportant, aAppliedList, aBox, aDefaultStyle);
      }
      else 
      {
        TCSSDeclListHandle::ApplyBoxModelPropertyL(borderStyleProps[boxSide],
                                                   borderStyleProps,
                                                   &(aPropertyVals[index]), 1, 
                                                   aImportant, aAppliedList, aBox, aDefaultStyle);
      }
      break;

    default:
      TCSSDeclListHandle::ApplyBoxModelPropertyL(borderWidthProps[boxSide],
                                                 borderWidthProps,
                                                 &(aPropertyVals[index]), 1, 
                                                 aImportant, aAppliedList, aBox, aDefaultStyle);
      break;
    }
  }
}

/* ------------------------------------------------------------------------- */
void TCSSDeclListHandle::ApplyFontPropertyL(NW_LMgr_Property_t *aProps,
                                           TUint8 aNumVals,
                                           TBool aImportant,
                                           NW_LMgr_PropertyList_t* aAppliedList,
                                           NW_LMgr_Box_t* aBox,
                                           TBool aDefaultStyle)
{
  NW_LMgr_PropertyName_t propName;

  /* Go through the values and apply them */
  for (TUint8 index = 0; index < aNumVals && index < 3; index ++)
  {
    switch (aProps[index].type) 
    {

    case NW_CSS_ValueType_Text:
      propName = NW_CSS_Prop_fontFamily;
      break;

    case NW_CSS_ValueType_Integer:
      propName = NW_CSS_Prop_fontWeight;
      break;

    case NW_CSS_ValueType_Token:

      switch (aProps[index].value.integer) 
      {
      
      case NW_CSS_PropValue_normal:
      case NW_CSS_PropValue_smallcaps:  
        propName = NW_CSS_Prop_fontVariant;
        break;

      case NW_CSS_PropValue_inherit:

        TCSSDeclListHandle::ApplyCommonPropertyL(NW_CSS_Prop_fontFamily, &(aProps[index]), 
                                                 aImportant, aAppliedList, aBox, aDefaultStyle);

        TCSSDeclListHandle::ApplyCommonPropertyL(NW_CSS_Prop_fontSize, &(aProps[index]), 
                                                 aImportant, aAppliedList, aBox, aDefaultStyle);

        TCSSDeclListHandle::ApplyCommonPropertyL(NW_CSS_Prop_fontWeight, &(aProps[index]), 
                                                 aImportant, aAppliedList, aBox, aDefaultStyle);

        TCSSDeclListHandle::ApplyCommonPropertyL(NW_CSS_Prop_fontVariant, &(aProps[index]), 
                                                 aImportant, aAppliedList, aBox, aDefaultStyle);

        TCSSDeclListHandle::ApplyCommonPropertyL(NW_CSS_Prop_fontStyle, &(aProps[index]), 
                                                 aImportant, aAppliedList, aBox, aDefaultStyle);
        
        /* 
        ** TODO: This break should be here, but if it is simply added then propName will not
        **       be set for the call to ApplyCommonPropertyL, below.  Falling through is
        **       benign because propName ends up with a value of "NW_CSS_Prop_fontStyle"
        **       which was just called above.
        */
        /* break; */
        /*lint -fallthrough*/

      default:
        propName = NW_CSS_Prop_fontStyle;
        break;
      }
      break;

    default:
      /* If an illegal value type sneaks in, it will be removed by ApplyProperty */
      propName = NW_CSS_Prop_fontSize;
      break;
    }

    TCSSDeclListHandle::ApplyCommonPropertyL(propName, &(aProps[index]), 
                                             aImportant, aAppliedList, aBox, aDefaultStyle);
  }
}

/* ------------------------------------------------------------------------- */
void TCSSDeclListHandle::ApplyListStylePropertyL(NW_LMgr_Property_t *aProps,
                       TUint8 aNumVals,
                       TBool aImportant,
                       NW_LMgr_PropertyList_t* aAppliedList,
                       NW_LMgr_Box_t* aBox,
                       TBool aDefaultStyle)
{
  /* Go through the values and apply them */
  for (TUint8 index = 0; index < aNumVals && index < 3; index ++)
  {
    NW_LMgr_PropertyName_t propName;
  
    switch (aProps[index].type) 
    {

    case NW_CSS_ValueType_Image:
      propName = NW_CSS_Prop_listStyleImage;
      break;

    case NW_CSS_ValueType_Token:

      switch (aProps[index].value.integer) 
      {      
      case NW_CSS_PropValue_inside:
      case NW_CSS_PropValue_outside:  
        propName = NW_CSS_Prop_listStylePosition;
        break;

      default:
        propName = NW_CSS_Prop_listStyleType;
        break;
      }
      break;

    default:
      /* If an illegal value type sneaks in, it will be removed by ApplyProperty */
      propName = NW_CSS_Prop_listStyleType;
      break;
    }

    TCSSDeclListHandle::ApplyCommonPropertyL(propName, &(aProps[index]), 
                                             aImportant, aAppliedList, aBox, aDefaultStyle);
  }
}

/* ------------------------------------------------------------------------- *
 * Function:      NW_CSS_DeclarationListHandle_ApplyBackgroundProperty
 *
 * Description:   Handles background and backgroud-position properites,
 *                analyze the compound property values for them.
 *                Example:
 *                background: white repeat-x url("foo.gif");
 *                background-position: center top
 *                To define position of the image we need only 2 values/tokens
 *                Algorithm can handle when accidentally somebody will write more than 2.
 *                Examples 
 *                "0px, 0px" or " 0% 0%" - top left coner ; 
 *                "left, top " or "top left" - top left coner ;
 *                "0px left" or "left 0" top left coner ;
 *                "0 center"  means X=0 Y= 1/2 height of the box  etc.
 *
 * Param@         thisObj -- DeclarationListHandle object
 * Param@         propName -- Property Name eg. NW_CSS_Prop_Background
 * Param@         props -- list of properties
 * Param@         numVals -- number of properties in the list
 * Param@         important -- is important or not
 * Param@         appliedList -- the applied property list
 * Param@         box -- the target box to apply properties
 * Param@         defaultStyle -- whether this is default style
 */


void 
TCSSDeclListHandle::ApplyBackgroundPropertyL(NW_LMgr_PropertyName_t aPropName, 
                                            NW_LMgr_Property_t *aProps,
                                            TUint8 aNumVals,
                                            TBool aImportant,
                                            NW_LMgr_PropertyList_t* aAppliedList,
                                            NW_LMgr_Box_t* aBox,
                                            TBool aDefaultStyle)
{
  TBool attachXDefined = EFalse;
  TBool attachYDefined = EFalse;
  TBool centerDefined = EFalse;
  NW_LMgr_Property_t centerProp;
  TUint8 origPropNumVals = aNumVals;
  const TUint8 KMaxNumCoord = 2;           //To define position of the image we need only 2 values/tokens which define X and Y coordinates.
  TUint8 indexCenter = 0xFF;// if token "center" is found, indexCenter is its index in the array 
                            // of tokens, values  (indexCenter initialized to 0xFF to prevent compiler warning.
  TUint8 numTokens = 0;
	if (aPropName == NW_CSS_Prop_backgroundPosition) 
	{
		// most common case 2 values at the beginning

		if(((aNumVals>=2) && NW_CSS_ValueType_Token != aProps[0].type) && (NW_CSS_ValueType_Token != aProps[1].type))
		{

			aPropName = NW_CSS_Prop_backgroundPosition_x;
			TCSSDeclListHandle::ApplyCommonPropertyL(aPropName, &(aProps[0]), aImportant, 
                                                 aAppliedList, aBox, aDefaultStyle);
			attachXDefined = NW_TRUE;

			aPropName = NW_CSS_Prop_backgroundPosition_y;
			TCSSDeclListHandle::ApplyCommonPropertyL(aPropName, &(aProps[1]), aImportant, 
                                                 aAppliedList, aBox, aDefaultStyle);
			attachYDefined = NW_TRUE;
		}
		else // tokens possibly present or we have only one entry (aNumVals = 1)
		{
		    while (aNumVals--) // Check for tokens first, because only when we have information 
             // about tokens we know how to handle values.
		    {
		        if(NW_CSS_ValueType_Token == aProps[aNumVals].type)
		        {
			        if (aProps[aNumVals].value.token == NW_CSS_PropValue_top ||
			            aProps[aNumVals].value.token == NW_CSS_PropValue_bottom)
			        {
				        if(!attachYDefined)
				        {
				            aPropName = NW_CSS_Prop_backgroundPosition_y;
			            	TCSSDeclListHandle::ApplyCommonPropertyL(aPropName, &(aProps[aNumVals]), aImportant, 
                                               aAppliedList, aBox, aDefaultStyle);
				            attachYDefined = NW_TRUE;
				            numTokens++;
				        }
			        }
			        else if (aProps[aNumVals].value.token == NW_CSS_PropValue_left ||
                        aProps[aNumVals].value.token == NW_CSS_PropValue_right)
			        {
			            if(!attachXDefined)
				        {
				            aPropName = NW_CSS_Prop_backgroundPosition_x;
				            TCSSDeclListHandle::ApplyCommonPropertyL(aPropName, &(aProps[aNumVals]), aImportant, 
                                                 aAppliedList, aBox, aDefaultStyle);
				            attachXDefined = NW_TRUE;
				            numTokens++;
				        }
			        }
			        else if (aProps[aNumVals].value.token == NW_CSS_PropValue_center) 
			        {
			        centerDefined = NW_TRUE;
			        numTokens++;
			        indexCenter = aNumVals;
			        }
		        }
		}// end while
		

		// If we have encountered any tokens, we already applied properties 
		
		if((numTokens < KMaxNumCoord) && (origPropNumVals > numTokens))
		{ 
            // Value has to be accepted as  X, if we have not encountered "left" or 'right"
            //  otherwise it will be accepted as Y 
            // If we have found "top" or "bottom" value has to  be accepted as X 
            // Later missing X or Y will be defaulted to Center.
            // In case " center value" Center will define X, value will define Y
            // Examples for values  2px, 30%, 45 (default unit is px) etc..
		
		    for (TUint8 i=0; i< origPropNumVals; i++)
		    {
			    if(NW_CSS_ValueType_Token != aProps[i].type)
			    {
				    if(!attachXDefined && !(centerDefined  && (indexCenter <i)))
				    {
				    	aPropName = NW_CSS_Prop_backgroundPosition_x;
				    	TCSSDeclListHandle::ApplyCommonPropertyL(aPropName, &(aProps[i]), aImportant, 
                                                 aAppliedList, aBox, aDefaultStyle);
				    	attachXDefined = NW_TRUE;
				 
				    }
				    else
				    {
					    if(!attachYDefined)
					    {
					        aPropName = NW_CSS_Prop_backgroundPosition_y;
					        TCSSDeclListHandle::ApplyCommonPropertyL(aPropName, &(aProps[i]), aImportant, 
                                                 aAppliedList, aBox, aDefaultStyle);
					         attachYDefined = NW_TRUE;
					    }
				    }
			    }
		    }//end for

	    }//endif
    }//endif combination of tokens / values

  }
  else if (aPropName == NW_CSS_Prop_background)
  {
    while (aNumVals--)
    {
      switch(aProps[aNumVals].value.token)
      {
      case NW_CSS_PropValue_top:
      case NW_CSS_PropValue_bottom:
        aPropName = NW_CSS_Prop_backgroundPosition_y;
        attachYDefined = NW_TRUE;
        break;

      case NW_CSS_PropValue_left:
      case NW_CSS_PropValue_right:
        aPropName = NW_CSS_Prop_backgroundPosition_x;
        attachXDefined = NW_TRUE;
        break;

      case NW_CSS_PropValue_center:
        centerDefined = NW_TRUE;
        break;

      case NW_CSS_PropValue_scroll:
      case NW_CSS_PropValue_background_fixed:
        aPropName = NW_CSS_Prop_backgroundAttachment;
        break;

      case NW_CSS_PropValue_background_repeat:
      case NW_CSS_PropValue_background_repeat_x:
      case NW_CSS_PropValue_background_repeat_y:
      case NW_CSS_PropValue_background_norepeat:
        aPropName = NW_CSS_Prop_backgroundRepeat;
        break;

      default:
        switch (aProps[aNumVals].type)
        {
        case NW_CSS_ValueType_Color:
          aPropName = NW_CSS_Prop_backgroundColor;
          break;

        case NW_CSS_ValueType_Image:
          aPropName = NW_CSS_Prop_backgroundImage;
          break;
          
        default:
          break;
        }
        break;
      }
      if ((aProps[aNumVals].value.token != NW_CSS_PropValue_center) || !centerDefined)
      {
        TCSSDeclListHandle::ApplyCommonPropertyL(aPropName, &(aProps[aNumVals]), aImportant, 
                                                 aAppliedList, aBox, aDefaultStyle);
      }
    }
  }
  if (centerDefined)
  {
    centerProp.type = NW_CSS_ValueType_Token;
    centerProp.value.token = NW_CSS_PropValue_center;
    if (!attachXDefined)
    {
      aPropName = NW_CSS_Prop_backgroundPosition_x;
      TCSSDeclListHandle::ApplyCommonPropertyL(aPropName, &centerProp, aImportant, 
                                               aAppliedList, aBox, aDefaultStyle);
    }
    if (!attachYDefined)
    {
      aPropName = NW_CSS_Prop_backgroundPosition_y;
      TCSSDeclListHandle::ApplyCommonPropertyL(aPropName, &centerProp, aImportant, 
                                               aAppliedList, aBox, aDefaultStyle);
    }
  }
}
 

  

/* ------------------------------------------------------------------------- 
            public methods
 ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
void 
TCSSDeclListHandle::ApplyPropertyL(NW_LMgr_PropertyName_t aPropName, 
                                   NW_LMgr_Property_t *aProps,
                                   TUint8 aNumVals,
                                   TBool aImportant,
                                   NW_LMgr_PropertyList_t* aAppliedList,
                                   NW_LMgr_Box_t* aBox,
                                   TBool aDefaultStyle)
{
  NW_ASSERT(aNumVals <= TCSSDeclListHandleMaxVals);

  /* The box model properties (border, padding and margin) need special attention.
   * As an optimization, we keep the shorthand property around as long as we can.
   */
  switch (aPropName) 
  {

  case NW_CSS_Prop_border:
  case NW_CSS_Prop_borderTop:
  case NW_CSS_Prop_borderBottom:
  case NW_CSS_Prop_borderLeft:
  case NW_CSS_Prop_borderRight:

    TCSSDeclListHandle::ApplyBorderPropertyL(aPropName,aProps, aNumVals,
                             aImportant, aAppliedList, aBox, aDefaultStyle);
    break;
    
  case NW_CSS_Prop_borderWidth:
  case NW_CSS_Prop_topBorderWidth:
  case NW_CSS_Prop_bottomBorderWidth:
  case NW_CSS_Prop_leftBorderWidth:
  case NW_CSS_Prop_rightBorderWidth:

    TCSSDeclListHandle::ApplyBoxModelPropertyL(aPropName, boxModelProps[1], aProps,  
                            aNumVals, aImportant, aAppliedList, aBox, aDefaultStyle);

    break;

  case NW_CSS_Prop_borderStyle:
  case NW_CSS_Prop_topBorderStyle:
  case NW_CSS_Prop_bottomBorderStyle:
  case NW_CSS_Prop_leftBorderStyle:
  case NW_CSS_Prop_rightBorderStyle:

    TCSSDeclListHandle::ApplyBoxModelPropertyL(aPropName, boxModelProps[2], aProps,  
                        aNumVals, aImportant, aAppliedList, aBox, aDefaultStyle);
    break;

  case NW_CSS_Prop_borderColor:
  case NW_CSS_Prop_topBorderColor:
  case NW_CSS_Prop_bottomBorderColor:
  case NW_CSS_Prop_leftBorderColor:
  case NW_CSS_Prop_rightBorderColor:

    TCSSDeclListHandle::ApplyBoxModelPropertyL(aPropName, boxModelProps[3], aProps,  
                              aNumVals, aImportant, aAppliedList, aBox, aDefaultStyle);
    break;

  case NW_CSS_Prop_padding:
  case NW_CSS_Prop_topPadding:
  case NW_CSS_Prop_bottomPadding:
  case NW_CSS_Prop_leftPadding:
  case NW_CSS_Prop_rightPadding:

    TCSSDeclListHandle::ApplyBoxModelPropertyL(aPropName, boxModelProps[4], aProps,  
                            aNumVals, aImportant, aAppliedList, aBox, aDefaultStyle);
    break;

  case NW_CSS_Prop_margin:
  case NW_CSS_Prop_topMargin:
  case NW_CSS_Prop_bottomMargin:
  case NW_CSS_Prop_leftMargin:
  case NW_CSS_Prop_rightMargin:

    TCSSDeclListHandle::ApplyBoxModelPropertyL(aPropName, boxModelProps[5], aProps,  
                            aNumVals, aImportant, aAppliedList, aBox, aDefaultStyle);
    break;

  case NW_CSS_Prop_font:

    TCSSDeclListHandle::ApplyFontPropertyL(aProps, aNumVals, aImportant, 
                                       aAppliedList, aBox, aDefaultStyle);
    break;

  case NW_CSS_Prop_listStyle:

    TCSSDeclListHandle::ApplyListStylePropertyL(aProps, aNumVals, aImportant, 
                                       aAppliedList, aBox, aDefaultStyle);
    break;

  case NW_CSS_Prop_background:
  case NW_CSS_Prop_backgroundPosition:
    TCSSDeclListHandle::ApplyBackgroundPropertyL(aPropName, aProps, aNumVals, aImportant, 
                                         aAppliedList, aBox, aDefaultStyle);
    break;

  default:
    if ((aPropName == NW_CSS_Prop_textDecoration) && (aNumVals == 2))
    {
      if (((aProps[0].value.token == NW_CSS_PropValue_underline) && 
          (aProps[1].value.token == NW_CSS_PropValue_blink)) ||
          ((aProps[0].value.token == NW_CSS_PropValue_blink) && 
          (aProps[1].value.token == NW_CSS_PropValue_underline)) )
      {
        aProps[0].value.token = NW_CSS_PropValue_blinkUnderline;
      }
    }

    TCSSDeclListHandle::ApplyCommonPropertyL(aPropName, &(aProps[0]), aImportant, 
                                             aAppliedList, aBox, aDefaultStyle);
    break;
  }
}

void DeletePropList(NW_LMgr_PropertyList_t* appliedList)
{
  if (appliedList != NULL)
  {
    (void) NW_LMgr_PropertyList_Clear(appliedList);
    NW_Object_Delete(appliedList);
  }
}

void DeleteOutProps(NW_LMgr_Property_t* outProps, TInt numVals)
{
  for (TInt index = 0; index < numVals; index++) 
  {
    NW_LMgr_Property_t *property = &(outProps[index]);
    if (property->type == NW_CSS_ValueType_Text ||
        property->type == NW_CSS_ValueType_Image) 
    {
      NW_Object_Delete(property->value.object);
    }
  }
}

/*------------------------------------------------------------------------*/ 
void 
TCSSDeclListHandle::ApplyStylesL(NW_LMgr_Box_t* aBox,
                                TCSSParser* aParser,
                                NW_CSS_Processor_t* aProcessor,
                                TInt aBufferIndex)
{
  TCSSReaderUnit propertyName;
  TBool important;
  TCSSPropertyVal propertyVals[TCSSDeclListHandleMaxVals];
  NW_LMgr_PropertyList_t *appliedList;
  TUint8 numVals;
  NW_LMgr_Property_t outProps[TCSSDeclListHandleMaxVals];

  /* Create a vector for the appliedList properties */
  appliedList = (NW_LMgr_PropertyList_t*) NW_LMgr_SimplePropertyList_New (2, 2);
  if (appliedList == NULL)
  {
    User::Leave(KErrNoMemory);
  }

  while (aParser->DeclarationListIterate(&propertyName, propertyVals, &numVals, 
          TCSSDeclListHandleMaxVals, &important) == KBrsrIterateMore)
  {
    TText16* ucs2Str = NULL;

    if (propertyName.iLength == 0)
      continue;

    TRAPD(ret, ucs2Str = propertyName.GetUnicodeL(aParser->GetEncoding()));
    if (ret == KErrNoMemory)
    {
      DeletePropList(appliedList);
      User::Leave(KErrNoMemory);
    }
    if (ucs2Str == NULL)
      continue;
      
    // Get corresponding property entry
    const TCSSPropertyTablePropEntry* propEntry = TCSSPropertyTable::GetPropertyEntry(ucs2Str);

    NW_Mem_Free(ucs2Str);

    if (propEntry == NULL)
      continue;

    /* Convert the raw CSS values */
    for (TInt index = 0; index < numVals; index++) 
    {
      TBool converted = EFalse;
      TRAPD(ret1, converted = TCSSDeclListHandle::ConvertValueL(propEntry->tokenName,  
                             &(propertyVals[index]),&(outProps[index]), aParser, aProcessor, aBox, aBufferIndex));
      if (!converted) 
      {
        DeleteOutProps(outProps, index);
        DeletePropList(appliedList);
        if (ret1 == KErrNoMemory)
        {
          User::Leave(KErrNoMemory);
        }
        return;
      }
    }

    /* Apply the property */
    TRAPD(ret2, TCSSDeclListHandle::ApplyPropertyL(propEntry->tokenName, outProps, numVals, 
                                                  important, appliedList, aBox, EFalse));
    
    /* First we delete, then we check status.  This avoids leakage in case of failure. */
    DeleteOutProps(outProps, numVals);
    if (ret2 == KErrNoMemory)
    {
      DeletePropList(appliedList);
      User::Leave(KErrNoMemory);
    }
  }//while
  DeletePropList(appliedList);
}
