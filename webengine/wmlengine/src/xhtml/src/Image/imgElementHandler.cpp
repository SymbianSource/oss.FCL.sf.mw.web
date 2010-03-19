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
* Description:  Element handler for images
*
*/


// INCLUDE FILES
#include "nw_image_imgelementhandleri.h"
#include "nw_image_virtualimage.h"
#include "nw_image_epoc32simpleimage.h"
#include "nw_image_areaelementhandler.h"
#include "nw_text_ascii.h"
#include "nwx_settings.h"
#include "HEDDocumentListener.h"
#include "nw_gdi_utils.h"
#include "nwx_http_defs.h"
#include "nw_hed_documentroot.h"
#include "nw_imagech_epoc32contenthandler.h"
#include "nw_xhtml_xhtmlcontenthandler.h"
#include "nw_xhtml_xhtml_1_0_tokens.h"
#include "nw_hed_hedeventhandler.h"
#include "nw_evt_activateevent.h"
#include "nw_evt_focusevent.h"
#include "nw_lmgr_rootbox.h"
#include "nw_lmgr_animatedimagebox.h"
#include "nw_lmgr_imagemapbox.h"
#include "nw_lmgr_areabox.h"
#include "MapElementList.h"
#include "ImageMapPopup.h"
#include "BrsrStatusCodes.h"
#include "nw_hypertext_aelementhandler.h"
#include "nwx_statuscodeconvert.h"
#include "nw_lmgr_verticaltablecellbox.h"

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Function: NW_ImageLoadNeeded
// Description: According to Vertical Layout Algorithm, we do not want to load
//   an image under following 2 conditions:
//   1) Image height/width is less than 4px and it is not inside <a> or <input>
//   2) Image width is less than 26px and height is greater than 19 px
//      and it is not inside <a> or <input>
// Returns: NW_Bool: NW_FALSE if image is too small to be loaded
//                 : NW_TRUE if image can be loaded
// -----------------------------------------------------------------------------
//
static
NW_Bool NW_ImageLoadNeeded(NW_XHTML_ContentHandler_t* contentHandler,
                           NW_DOM_ElementNode_t* elementNode)
    {
    TBrowserStatusCode status;
    NW_Ucs2* width = NULL;
    NW_Ucs2* height = NULL;
    NW_Int32 widthVal = 0;
    NW_Int32 htVal = 0;
    NW_Bool imageLoadNeeded = NW_TRUE;
    NW_DOM_ElementNode_t* node = NULL;
    NW_Bool widthValFound = NW_FALSE;
    
    /* find the 'width' attribute and get its string value */
    status = NW_XHTML_ElementHandler_GetAttribute (contentHandler, elementNode,
        NW_XHTML_AttributeToken_width, &width);
    if ((status == KBrsrSuccess) && (width != NULL))
        {
        NW_Ucs2* endPtr = NULL;
        NW_Float32 value;
        
        status = NW_Str_CvtToFloat32(width, &endPtr, &value);
        if (status == KBrsrSuccess)
            {
            widthVal = (NW_Int32)value;
            }
        widthValFound = NW_TRUE;
        NW_Mem_Free(width);
        
        if (widthVal < NW_VerticalLayout_MinImageWidth1)
            {
            imageLoadNeeded = NW_FALSE; /* image is too small to be loaded */
            }
        }
    
    if (imageLoadNeeded)
        {
        /* find the 'height' attribute and get its string value */
        status = NW_XHTML_ElementHandler_GetAttribute (contentHandler, elementNode,
            NW_XHTML_AttributeToken_height, &height);
        if ((status == KBrsrSuccess) && (height != NULL))
            {
            NW_Ucs2* endPtr = NULL;
            NW_Float32 value;
            
            status = NW_Str_CvtToFloat32(height, &endPtr, &value);
            if (status == KBrsrSuccess)
                {
                htVal = (NW_Int32)value;
                }
            NW_Mem_Free(height);
            
            if (htVal < NW_VerticalLayout_MinImageHeight1)
                {
                imageLoadNeeded = NW_FALSE; /* image is too small to be loaded */
                }
            }
        }
    
    
    if (imageLoadNeeded && widthValFound &&
        (widthVal < NW_VerticalLayout_MinImageWidth2) &&
        (htVal > NW_VerticalLayout_MinImageHeight2))
        {
        imageLoadNeeded = NW_FALSE; /* image is too small to be loaded */
        }
    
        /* if image is too small we need to check if it is within an <a> because in that
    case we  need to load it */
    if (!imageLoadNeeded)
        {
        NW_Text_t* usemapVal = NULL;
        
        NW_XHTML_GetDOMAttribute (contentHandler, elementNode,
            NW_XHTML_AttributeToken_usemap, &usemapVal);
        if (usemapVal)
            {
            imageLoadNeeded = NW_TRUE;
            }
        else
            {
            /* find the enclosing <a> element */
            node = NW_DOM_Node_getParentNode(elementNode);
            while (node)
                {
                NW_Uint16 tagToken = NW_HED_DomHelper_GetElementToken (node);
                
                /* if we find an enclosing <a> we break;*/
                if (tagToken == NW_XHTML_ElementToken_a)
                    {
                    imageLoadNeeded = NW_TRUE;
                    break;
                    }
                    /* if we find an enclosing block-level element we break and do not load image
                    Assumption is that an <a> tag will not have block level elements in it
                */
                if (NW_XHTML_ElementHandler_IsBlockLevelTag(tagToken))
                    break;
                node = NW_DOM_Node_getParentNode(node);
                }
            }
        }
    
    return imageLoadNeeded;
}



// -----------------------------------------------------------------------------
// Function: NW_XHTML_imgElementHandler_InsertIntoImageMapLookupTable
// Description:
// -----------------------------------------------------------------------------
//
static
TBrowserStatusCode
NW_XHTML_imgElementHandler_InsertIntoImageMapLookupTable(NW_XHTML_ContentHandler_t* contentHandler,
                                                         NW_Uint16* usemapValue, NW_LMgr_ImageMapBox_t* imageMapBox )
  {
  NW_TRY( status )
    {
    CMapElementList* mapElementList = NULL;
    // retrieve the map list from the content handler, if it does not exist, create one
    mapElementList = static_cast<CMapElementList*>(NW_XHTML_ContentHandler_GetImageMapLookupTable(contentHandler));
    if( !mapElementList )
      {
      // create a new maplist to hold the map elements. deleted in the contenthandler destructor
      TRAP( status, mapElementList = CMapElementList::NewL() );
      NW_THROW_ON( status, KBrsrOutOfMemory );

      // add the maplist to the content handler
      NW_XHTML_ContentHandler_SetImageMapLookupTable( contentHandler, mapElementList );
      }
    // add image map box to the list
    status = mapElementList->SetImageMapBox( usemapValue, imageMapBox );
    _NW_THROW_ON_ERROR( status );
    }
  NW_CATCH( status )
    {
    }
  NW_FINALLY
    {
    return status;
    }
  NW_END_TRY
  }


// -----------------------------------------------------------------------------
// Function: NW_XHTML_imgElementHandler_CreateImageMapBox
// Description:
// -----------------------------------------------------------------------------
//
static
TBrowserStatusCode
NW_XHTML_imgElementHandler_CreateImageMapBox( const NW_XHTML_ElementHandler_t* elementHandler,
                                              NW_XHTML_ContentHandler_t* xhtmlContentHandler,
                                              NW_DOM_ElementNode_t* elementNode,
                                              NW_Image_Virtual_t* virtualImage,
                                              NW_Text_t* altText,
                                              NW_Text_t* url,
                                              NW_Bool brokenImage,
                                              NW_Text_t* usemapVal,
                                              NW_LMgr_ImageMapBox_t** newImageMapBox )
  {
  NW_LMgr_ImageMapBox_t* imageMapBox = NULL;
  NW_Uint16* storage = NULL;
  NW_Text_Length_t length;
  NW_Uint8 freeNeeded = NW_FALSE;
  NW_HED_EventHandler_t* eventHandler;

  NW_TRY( status )
    {
    eventHandler = NW_HED_EventHandler_New( NW_HED_DocumentNodeOf( xhtmlContentHandler ), elementNode );
    NW_THROW_OOM_ON_NULL( eventHandler, status );

    imageMapBox = NW_LMgr_ImageMapBox_New( 5,
                                           NW_Image_AbstractImageOf( virtualImage ),
                                           altText,
                                           brokenImage,
                                           NW_LMgr_EventHandlerOf( eventHandler ),
                                           url);
    NW_THROW_OOM_ON_NULL( imageMapBox, status );
    // image map box takes eventHandler's ownership
    eventHandler = NULL;
    // convert the mapName to Unicode and get the storage
    NW_Uint16* tempStorage = NULL;
    storage = (NW_Uint16*)NW_Text_GetUCS2Buffer( usemapVal, 0, &length, &freeNeeded );
    NW_THROW_OOM_ON_NULL( storage, status );

    // check to see if we need to strip off a leading '#' character from the map string
    tempStorage = storage;
    NW_Ucs4 character = NW_Text_GetAt( usemapVal, 0 );

    if( character == '#' )
      {
      tempStorage = NW_Str_Inc( storage, 1 );
      }
    // if we are expecting more data then just insert the map into the lookup table,
    // otherwise create the area list too
    if( !xhtmlContentHandler->lastChunk )
      {
      // insert image map box to the imagMapList so that we can create area list on this
      // image map box when the entire body has been loaded.
      status = NW_XHTML_imgElementHandler_InsertIntoImageMapLookupTable(
        xhtmlContentHandler, tempStorage, imageMapBox );
      _NW_THROW_ON_ERROR( status );
      }
    else
      {
      // call CreateAreaList to loop through area and anchor tags in order to create
      // Area boxes
      CMapElementList* mapElementList;
      NW_DOM_ElementNode_t* mapDomNode;

      // fetch the map element node out of the maplist
      mapElementList = (CMapElementList*)
        NW_XHTML_ContentHandler_GetImageMapLookupTable( xhtmlContentHandler );
      NW_THROW_ON_NULL( mapElementList, status, KBrsrFailure );

      // look up the map name in the list and retrieve the associated element node
      mapDomNode = mapElementList->DomNode( tempStorage );
      NW_THROW_ON_NULL( mapDomNode, status, KBrsrFailure );

      status = NW_XHTML_imgElementHandler_CreateAreaList( elementHandler, xhtmlContentHandler,
        mapDomNode, NW_LMgr_BoxOf( imageMapBox ) );
      _NW_THROW_ON_ERROR( status );
      }
    }
  NW_CATCH( status )
    {
    // ignore image map so that we create a regular image
    NW_Object_Delete( eventHandler );
    }
  NW_FINALLY
    {
    if( freeNeeded )
      {
      NW_Mem_Free( (NW_Ucs2*)storage );
      }
    *newImageMapBox = imageMapBox;
    return status;
    }
  NW_END_TRY
  }


// -----------------------------------------------------------------------------
// Function: NW_XHTML_imgElementHandler_CreateAreaList
// Description: Loops through the area and/or anchor tags associated with a map
//   and creates a list of NW_LMgr_AreaBox_t objects. This list is owned by the
//   parent NW_LMgr_ImageMapBox_t class
// Returns: TBrowserStatusCode
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
NW_XHTML_imgElementHandler_CreateAreaList(const NW_XHTML_ElementHandler_t* elementHandler,
                                          NW_XHTML_ContentHandler_t* contentHandler,
                                          NW_DOM_ElementNode_t* elementNode,
                                          NW_LMgr_Box_t* box )
  {
  NW_ADT_DynamicVector_t* areaList;
  NW_LMgr_Box_t* areaBox = NULL;
  void** temp = NULL;
  NW_Text_t* hrefVal = NULL;
  NW_Text_t* shapeVal = NULL;
  NW_Text_t* coordsVal = NULL;
  NW_Text_t* altText = NULL;
  NW_Ucs2* storage = NULL;
  NW_Uint8 freeNeeded = 0;
  NW_HED_EventHandler_t* eventHandler = NULL;
  NW_Text_Length_t charCount = 0;
  NW_LMgr_ImageMapBox_t* imageMapBox = NW_LMgr_ImageMapBoxOf( box );
  NW_Bool isFirstAreaBox = NW_TRUE;
  NW_Bool isJavaScript = NW_FALSE;

  NW_TRY (status)
    {
    // retrieve the area list from the image map box, if it does not exist, create one
    areaList = (NW_ADT_DynamicVector_t*)NW_LMgr_ImageMapBox_GetAreaBoxList(imageMapBox);
    if (!areaList)
      {
      areaList = (NW_ADT_DynamicVector_t*)
        NW_ADT_ResizableVector_New (sizeof(NW_LMgr_AreaBox_t), 10, 5 );
      NW_THROW_OOM_ON_NULL (areaList, status);

      // add the maplist to the content handler
      NW_LMgr_ImageMapBox_SetAreaBoxList(imageMapBox, areaList);
      }

    // loop through the children of the map element and add them to the list
    for (elementNode = NW_DOM_Node_getFirstChild (elementNode);
         elementNode != NULL;
         elementNode = NW_DOM_Node_getNextSibling (elementNode))
      {
      // verify that the elementNode is an area tag
      if (NW_DOM_ElementNode_getTagToken(elementNode) == NW_XHTML_ElementToken_area)
        {
        // find the 'href' attribute and get its string value
        NW_XHTML_GetDOMAttribute (contentHandler,
                                  elementNode,
                                  NW_XHTML_AttributeToken_href, &hrefVal);


        if (!hrefVal)
          {
          NW_XHTML_aElementHandler_getBaseTagUrl (contentHandler, &hrefVal);
          }

        if (hrefVal)
          {
          NW_Url_Schema_t scheme = NW_SCHEMA_INVALID;

          storage = (NW_Ucs2*)
            NW_Text_GetUCS2Buffer (hrefVal, NW_Text_Flags_Aligned, &charCount,
                                   &freeNeeded);

          NW_THROW_OOM_ON_NULL (storage, status);

          status = NW_Url_GetScheme(storage, &scheme);

          if(NW_Str_Strncmp(storage, (const NW_Ucs2*) NW_STR("javascript:"), 11) == 0)
          {
            isJavaScript = NW_TRUE;
          }

          // we don't want to display bad links, but other schemes are okay.
          if (status != KBrsrMalformedUrl || isJavaScript)
            {
            // find the 'coords' attribute and get its string value
            NW_XHTML_GetDOMAttribute (contentHandler,
                                      elementNode,
                                      NW_XHTML_AttributeToken_coords, &coordsVal);


            // find the 'shape' attribute and get its string value
            NW_XHTML_GetDOMAttribute (contentHandler,
                                      elementNode,
                                      NW_XHTML_AttributeToken_shape, &shapeVal);

            // find the 'alt' attribute and get its string value
            NW_XHTML_GetDOMAttribute (contentHandler,
                                      elementNode,
                                      NW_XHTML_AttributeToken_alt, &altText);

            // if we don't have alternate text, use the href string instead
            if ((altText == NULL) || (altText->characterCount == 0))
              {
              altText = NW_Text_Copy(hrefVal, NW_TRUE);
              }

            eventHandler =
              NW_HED_EventHandler_New (NW_HED_DocumentNodeOf (contentHandler), elementNode);

            areaBox = (NW_LMgr_Box_t*)NW_LMgr_AreaBox_New(6,
                                                          NW_LMgr_EventHandlerOf (eventHandler),
                                                          imageMapBox,
                                                          shapeVal,
                                                          coordsVal,
                                                          altText,
                                                          hrefVal);

            NW_THROW_OOM_ON_NULL (areaBox, status);

            if (NW_LMgr_AreaBox_IsInvalidAreaBox(areaBox))
              {
              NW_Object_Delete (areaBox);
              }
            else
              {
              if (isFirstAreaBox)
              {
                temp = NW_ADT_DynamicVector_InsertAt (areaList, &areaBox, NW_ADT_Vector_AtEnd);

                isFirstAreaBox = NW_FALSE;
              }
              else
              {
                NW_ADT_Vector_Metric_t elementSize = NW_ADT_Vector_GetSize(areaList);
                NW_Int32 low = 0;
                NW_Int32 high = (NW_Int32)elementSize - 1;
                NW_Int32 mid = 0;
                NW_GDI_Metric_t xMid = 0, yMid = 0;

                // In order to speed the insertion into the array, it is best to use a form of
                // Binary Search in order to not have to process the array sequencially. The correct
                // location of the item is found when low is no longer <= high.
                while (low <= high)
                {
                  // Find the mid point of the array
                  mid = (NW_Int32)((low + high) / 2);

                  // Retreive the area box at the mid point
                  NW_LMgr_Box_t* tempAreaBox = *(NW_LMgr_Box_t**)NW_ADT_Vector_ElementAt (areaList, (NW_ADT_Vector_Metric_t)mid);

                  if (tempAreaBox != NULL)
                  {
                    // Retrieve the x,y coordinates of the mid point area box
                    NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( areaBox );
                    NW_GDI_Rectangle_t tempBoxBounds = NW_LMgr_Box_GetFormatBounds( tempAreaBox );
                    xMid = tempBoxBounds.point.x;
                    yMid = tempBoxBounds.point.y;

                    if (boxBounds.point.y < yMid)
                    {
                      high = mid - 1;
                    }
                    else if (boxBounds.point.y == yMid)
                    {
                      if (boxBounds.point.x < xMid)
                      {
                        high = mid - 1;
                      }
                      else
                      {
                        low = mid + 1;
                      }
                    }
                    else
                    {
                      low = mid + 1;
                    }
                  }
                  else
                  {
                    break;
                  }
                }

                temp = NW_ADT_DynamicVector_InsertAt (areaList, &areaBox, (NW_ADT_Vector_Metric_t)low);

              }
              NW_THROW_OOM_ON_NULL (temp, status);

              // apply common attributes and styles
              status = NW_XHTML_ElementHandler_ApplyStyles (elementHandler, contentHandler,
                elementNode, &areaBox, NULL);

              _NW_THROW_ON_ERROR( status );
              }
            }
          }
        }
      }
    }
  NW_CATCH (status)
    {
    NW_Object_Delete(shapeVal);
    NW_Object_Delete(coordsVal);
    NW_Object_Delete(altText);
    NW_Object_Delete(hrefVal);
    }
  NW_FINALLY
    {
    if (freeNeeded)
      {
      NW_Str_Delete(storage);
      }
    return status;
    }
  NW_END_TRY
  }

// -----------------------------------------------------------------------------
// Function: NW_XHTML_imgElementHandler_DisplayPopUp
// Description: Displays the pop up responsible for listing the image map's
//   area boxes so that the user can make a selection in small screen mode
// Returns: TInt: index into the list of area's being displayed
//              : -1 if there was no selection made
// -----------------------------------------------------------------------------
//
TInt
NW_XHTML_imgElementHandler_DisplayPopUp(CFbsBitmap* imageMapBitmap,
                                        CArrayFixFlat<TRect>* areaCoordList,
                                        CArrayPtrFlat<HBufC>* areaAltTextList)
{
  CArrayPtrFlat<CFbsBitmap>* areaBitmapList;
  CFbsBitmap* areaBitmap;
  CFbsBitmapDevice* bitmapDevice;
  CFbsBitGc* bitmapContext;
  TInt areaCount;
  TInt selectedItem( -1 );

  if( areaCoordList )
    {
    areaCount = areaCoordList->Count();
    areaBitmapList = new( ELeave )CArrayPtrFlat<CFbsBitmap>( 5 );
    CleanupStack::PushL( areaBitmapList );

    for ( TInt i = 0; i < areaCount; i++ )
    {
      // takes each area and create the corresponding bitmap
      areaBitmap = new( ELeave )CFbsBitmap();
      CleanupStack::PushL( areaBitmap );

      User::LeaveIfError( areaBitmap->Create( areaCoordList->At( i ).Size(), EColor4K ) );
      // create bitmap device
      bitmapDevice = CFbsBitmapDevice::NewL( areaBitmap );
      CleanupStack::PushL( bitmapDevice );

      // create graphics context for bitmap device
      User::LeaveIfError( bitmapDevice->CreateContext( bitmapContext ) );
      CleanupStack::PushL( bitmapContext );

      // copy area
      bitmapContext->BitBlt( TPoint( 0, 0 ), imageMapBitmap, areaCoordList->At( i ) );

      CleanupStack::PopAndDestroy( 2 ); // bitmapContext, bitmapDevice

      areaBitmapList->AppendL( areaBitmap );
      // list takes ownership
      CleanupStack::Pop(); // areaBitmap
    }

    selectedItem = CImageMapPopup::CreateAndRunL( *areaBitmapList, *areaAltTextList );
    // CreateAndRunL takes areaBitmaplist ownership
    CleanupStack::Pop(); // areaBitmapList
    }
  return selectedItem;
}

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

// -----------------------------------------------------------------------------
const
NW_XHTML_imgElementHandler_Class_t NW_XHTML_imgElementHandler_Class = {
  { /* NW_Object_Core              */
    /* super                       */ &NW_XHTML_ElementHandler_Class,
    /* queryInterface              */ _NW_Object_Core_QueryInterface
  },
  { /* NW_XHTML_ElementHandler     */
    /* initialize                  */ _NW_XHTML_imgElementHandler_Initialize,
    /* createBoxTree               */ _NW_XHTML_imgElementHandler_CreateBoxTree,
    /* processEvent                */ _NW_XHTML_imgElementHandler_ProcessEvent
  },
  { /* NW_XHTML_imgElementHandler */
    /* unused                      */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   singleton definition
 * ------------------------------------------------------------------------- */

// -----------------------------------------------------------------------------
const NW_XHTML_imgElementHandler_t NW_XHTML_imgElementHandler = {
  { { &NW_XHTML_imgElementHandler_Class } }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

// -----------------------------------------------------------------------------
TBrowserStatusCode
_NW_XHTML_imgElementHandler_Initialize (const NW_XHTML_ElementHandler_t* elementHandler,
                                        NW_XHTML_ContentHandler_t* contentHandler,
                                        NW_DOM_ElementNode_t* elementNode)
{
  NW_Text_t* url = NULL;
  TBrowserStatusCode status;

  NW_REQUIRED_PARAM(elementHandler);

  /* NW_Settings_GetImageEnabled() called to determine whether
  autoloading of images is turned on. If not, then just display
  the initial "x" and alt text, and don't issue any load requests.
  */
  if (!NW_Settings_GetImagesEnabled())
  {
    return KBrsrSuccess;
  }

  /* In case vertical layout is enabled we need to check if there is any
     need to load the image */
  NW_HED_DocumentRoot_t* documentRoot =
    (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode( contentHandler );
  NW_ASSERT( documentRoot != NULL );

  NW_LMgr_RootBox_t* rootBox = NW_HED_DocumentRoot_GetRootBox( documentRoot );
  NW_ASSERT( rootBox != NULL );

  if( NW_LMgr_RootBox_GetSmallScreenOn( rootBox ) && !contentHandler->switchBackToNormalLayout)
  {
    if (!NW_ImageLoadNeeded(contentHandler, elementNode))
      return KBrsrSuccess;
  }

  /* find the 'src' attribute and get its string value */
  status = NW_XHTML_GetDOMAttribute (contentHandler, elementNode,
                                  NW_XHTML_AttributeToken_src, &url);
  if (status != KBrsrSuccess)
  {
    return status;
  }

  /* load the image */
  status = NW_XHTML_ContentHandler_StartLoad (contentHandler,
      url, NW_HED_UrlRequest_Reason_DocLoadChild, elementNode, NW_UrlRequest_Type_Image);

  /* cleanup after ourselves */
  NW_Object_Delete (url);

  if (status != KBrsrOutOfMemory)
  {
    status = KBrsrSuccess;
  }

  return status;
}

// -----------------------------------------------------------------------------
TBrowserStatusCode
_NW_XHTML_imgElementHandler_CreateBoxTree (const NW_XHTML_ElementHandler_t* elementHandler,
                                           NW_XHTML_ContentHandler_t* xhtmlContentHandler,
                                           NW_DOM_ElementNode_t* elementNode,
                                           NW_LMgr_ContainerBox_t* parentBox )
  {
  NW_ImageCH_Epoc32ContentHandler_t* imageContentHandler = NULL;
  NW_LMgr_Box_t* box = NULL;
  NW_Text_t* altText = NULL;
  NW_LMgr_RootBox_t* rootBox = NULL;
  NW_Image_Virtual_t* virtualImage = NULL;
  NW_Text_t* usemapVal = NULL;
  NW_Text_t* url = NULL;
  NW_Bool brokenImage = NW_FALSE;
  NW_Image_Epoc32Simple_t* simpleImage = NULL;
  NW_LMgr_ContainerBox_t* tempParentBox = NULL;

  NW_TRY (status)
    {
    NW_HED_DocumentRoot_t* documentRoot;
    NW_HED_CompositeNode_t* compositeNode;

    documentRoot = (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode( xhtmlContentHandler );
    NW_ASSERT( documentRoot != NULL );

    rootBox = NW_HED_DocumentRoot_GetRootBox( documentRoot );

    // get the associated content handler if any
    compositeNode = static_cast<NW_HED_CompositeNode_t*>
      ( NW_Object_QueryAggregate( xhtmlContentHandler, &NW_HED_CompositeNode_Class ) );
    NW_ASSERT (compositeNode != NULL);

    imageContentHandler = (NW_ImageCH_Epoc32ContentHandler_t*)
      NW_HED_CompositeNode_LookupChild( compositeNode, elementNode );
    // imageContenHandler might be NULL -as the content has not arrived yet. do not throw OOM error

    // retrieve the altText attribute
    NW_XHTML_GetDOMAttribute( xhtmlContentHandler, elementNode, NW_XHTML_AttributeToken_alt, &altText );

    // create the image alias from the image content handler
    if( imageContentHandler && imageContentHandler->image )
      {
      // this case arises when this Image Content Handler contains the Virtual Image
      if( NW_Object_IsInstanceOf( imageContentHandler->image, &NW_Image_Virtual_Class ) == NW_TRUE )
        {
        // get the abstract image from the virtual image
        virtualImage = NW_Image_Virtual_New( NW_Image_VirtualOf( imageContentHandler->image )->image );
        }
      else
        {
        virtualImage = NW_Image_Virtual_New( imageContentHandler->image );
        }
      NW_THROW_OOM_ON_NULL( virtualImage, status );

      brokenImage = NW_FALSE;
      }
    else
    // if there is no content handler or content handler image associated with our node
    // we create a canned virtual image
      {
      NW_Image_AbstractImage_t* cannedImage;

      NW_ASSERT( rootBox->cannedImages != NULL );

      cannedImage = (NW_Image_AbstractImage_t*)
          NW_Image_CannedImages_GetImage( rootBox->cannedImages, NW_Image_Missing );
      NW_THROW_OOM_ON_NULL( cannedImage, status );

      virtualImage = NW_Image_Virtual_New( cannedImage );
      NW_THROW_OOM_ON_NULL( virtualImage, status );

      brokenImage = NW_TRUE;
      }

    // retrieve the name of the map. this will be used as the key in our map list
    NW_XHTML_GetDOMAttribute( xhtmlContentHandler,
      elementNode, NW_XHTML_AttributeToken_usemap, &usemapVal );

    // if this image is not using a image map, create an AnimatedImageBox, otherwise create
    // a ImageMapBox
    if( usemapVal )
      {
      NW_LMgr_ImageMapBox_t* imageMapBox;

      // find the 'src' attribute and get its string value
      status = NW_XHTML_GetDOMAttribute (xhtmlContentHandler, elementNode,
                                         NW_XHTML_AttributeToken_src, &url);

      NW_THROW_ON( status, KBrsrOutOfMemory );

      status = NW_XHTML_imgElementHandler_CreateImageMapBox( elementHandler, xhtmlContentHandler, elementNode,
        virtualImage, altText, url, brokenImage, usemapVal, &imageMapBox );

      // catch only OOM
      NW_THROW_ON( status, KBrsrOutOfMemory );

      // image container takes both the image and the alttext ownership
      box = (NW_LMgr_Box_t*)imageMapBox;
      virtualImage = NULL;
      altText = NULL;
      url = NULL;
      // we need to update the box tree in the imagecontenthandler since imagemapbox
      // is now associated with the imagemapbox
      if (imageContentHandler)
        {
        NW_HED_ContentHandlerOf( imageContentHandler )->boxTree = box;
        }
      }

    // create regular image box if we are unable to create an ImageMapBox
    if( !box  )
      {
      if( !imageContentHandler )
        {
        box = (NW_LMgr_Box_t*)NW_LMgr_AnimatedImageBox_New( 3, NW_Image_AbstractImageOf( virtualImage ),
          altText, brokenImage );
        NW_THROW_OOM_ON_NULL( box, status );
        // image container takes both the image and the alttext ownership
        virtualImage = NULL;
        altText = NULL;
        }
      else
        {
        // otherwise let the content handler build the ImageBox
        status = NW_HED_DocumentNode_GetBoxTree( imageContentHandler, &box );
        _NW_THROW_ON_ERROR( status );
        }
      }
    // add the box to the tree can return success or out of memory
    status = NW_LMgr_ContainerBox_AddChild( parentBox, box );
    _NW_THROW_ON_ERROR( status );

    // apply common attributes and styles can return success or out of memory
    status = NW_XHTML_ElementHandler_ApplyStyles( elementHandler, xhtmlContentHandler,
      elementNode, &box, NULL );
    NW_THROW_ON( status, KBrsrOutOfMemory );
	
    // In case of 'display:none' or an error Apply Styles removes the image box. 
    if (box == NULL)
    {
      NW_THROW_SUCCESS (status);
    }

    /* If the image is aligned a float has been inserted into its
     * property list. This is a problem if this image is inside container 
	 * which is inside a vertical table cell. The table algorithms don't handle
     * floats. This is the best point to remove the property if it has
     * been inserted. Walk up the box tree until we reach a VerticalTableCellBox,
     * or NULL.*/
    tempParentBox = parentBox;

    while(tempParentBox)
        {
        if (NW_Object_IsClass (tempParentBox, &NW_LMgr_VerticalTableCellBox_Class))
            {
            NW_LMgr_Box_RemoveProperty(box, NW_CSS_Prop_float);
            break;
            }
        tempParentBox = NW_LMgr_Box_GetParent(tempParentBox);
        }

    /* Store the "width" and "height" values of the <img>. This may be
     * required in the image decoding functionality.
     *
     * In the case of creating the box tree the first time the virtual
     * images will not have an image associated with them. The fact that
     * the image is non-null can be used to infer that it is a simple
     * image.
     *
     * In the case of RESUME we are rebuilding the box tree and all the
     * images are present, even the virtual ones. You must check the
     * class of the image.
     */

    if(imageContentHandler && imageContentHandler->image &&
       NW_Object_IsInstanceOf(imageContentHandler->image, &NW_Image_Epoc32Simple_Class ))
      {
        simpleImage = (NW_Image_Epoc32Simple_t*)imageContentHandler->image;
      }

      if(simpleImage)
      {
       NW_Uint16 nodeToken = 0;
       NW_Uint16 attrToken = 0;
       NW_DOM_AttributeListIterator_t listIter;
       NW_DOM_AttributeHandle_t attrHandle;
       NW_String_t attrValueStr;
       NW_Uint32 attrEncoding;
       NW_Ucs2 *ucs2AttrVal = NULL;

       nodeToken = NW_DOM_Node_getNodeToken(elementNode);

       if(nodeToken == NW_XHTML_ElementToken_img)
       {
        if (NW_DOM_ElementNode_hasAttributes(elementNode) )
        {
         /* Go through all the attributes and check for height and width */

         status = StatusCodeConvert(NW_DOM_ElementNode_getAttributeListIterator(elementNode, &listIter) );

         _NW_THROW_ON_ERROR( status );

         if(status == KBrsrSuccess)
               {
                while (NW_DOM_AttributeListIterator_getNextAttribute(&listIter, &attrHandle)
                                   == NW_STAT_WBXML_ITERATE_MORE)
                {
                 attrToken = NW_DOM_AttributeHandle_getToken(&attrHandle);
                 if( (attrToken == NW_XHTML_AttributeToken_width) ||
                     (attrToken == NW_XHTML_AttributeToken_height) )
                 {
                  attrEncoding = NW_DOM_AttributeHandle_getEncoding(&attrHandle);
                  status = StatusCodeConvert(NW_DOM_AttributeHandle_getValue(&attrHandle, &attrValueStr) );
                  _NW_THROW_ON_ERROR( status );
                  status = StatusCodeConvert( NW_String_stringToUCS2Char(&attrValueStr, attrEncoding, &ucs2AttrVal) );
                  _NW_THROW_ON_ERROR( status );
                  if(attrToken == NW_XHTML_AttributeToken_width)
                  {
                  simpleImage->srcWidth = NW_Str_Atoi(ucs2AttrVal);
                  }
                  else if(attrToken == NW_XHTML_AttributeToken_height)
                  {
                   simpleImage->srcHeight =  NW_Str_Atoi(ucs2AttrVal);
                  }

                  if(ucs2AttrVal != NULL)
                    {
                     NW_Mem_Free(ucs2AttrVal);
                    }
                 }
                }/*end while*/

               }/*end if(status == KBrsrSuccess)*/
        } /*end if (NW_DOM_ElementNode_hasAttributes(elementNode) )*/
       }/*end if(nodeToken == NW_XHTML_ElementToken_img)*/


      }/*end if(simpleImage) */
    }
  NW_CATCH (status)
    {
    NW_Object_Delete( box );
    }
  NW_FINALLY
    {
    NW_Object_Delete( usemapVal );
    NW_Object_Delete( altText );
    NW_Object_Delete( virtualImage );
    NW_Object_Delete( url );

    return status;
    }
  NW_END_TRY
  }

// -----------------------------------------------------------------------------
NW_Uint8
_NW_XHTML_imgElementHandler_ProcessEvent (const NW_XHTML_ElementHandler_t* elementHandler,
                                          NW_XHTML_ContentHandler_t* contentHandler,
                                          NW_DOM_ElementNode_t* elementNode,
                                          NW_LMgr_Box_t* box,
                                          NW_Evt_Event_t* event)
  {
  NW_REQUIRED_PARAM(elementHandler);
  NW_REQUIRED_PARAM(contentHandler);
  NW_REQUIRED_PARAM(elementNode);

  NW_LMgr_RootBox_t* rootBox = NW_LMgr_Box_GetRootBox(box);
  NW_ASSERT(rootBox);

  NW_HED_AppServices_t* appServices = rootBox->appServices;
  NW_ASSERT(appServices);

  // we are really only interested in the following events
  if ((NW_Object_Core_GetClass (event) != &NW_Evt_ActivateEvent_Class) &&
      (NW_Object_Core_GetClass (event) != &NW_Evt_FocusEvent_Class))
    {
    return NW_LMgr_EventNotAbsorbed;
    }

  if(NW_Object_Core_GetClass (event) == &NW_Evt_FocusEvent_Class)
    {
    return NW_LMgr_EventAbsorbed;
    }

  // if the event is an activate event and we are in small screen mode,
  // call the code to display the image in the image map view
  if (NW_Object_IsInstanceOf (event, &NW_Evt_ActivateEvent_Class) &&
      NW_LMgr_RootBox_GetSmallScreenOn( NW_LMgr_Box_GetRootBox( box ) ) )
    {
    appServices->browserAppApi.SwitchToImgMapView();
    }
  // we absorbed the event!
  return NW_LMgr_EventAbsorbed;
  }
