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
* Description:  Object for containing map element handler specific methods.
*
*/


#include "nw_image_mapelementhandleri.h"

#include "nw_xhtml_xhtmlcontenthandler.h"
#include "nw_lmgr_imagemapbox.h"
#include "nw_lmgr_areabox.h"
#include "nw_basicforms_xhtmlformliaison.h"
#include "MapElementList.h"

static
TBrowserStatusCode
NW_XHTML_mapElementHandler_InsertIntoImageMapLookupTable(NW_XHTML_ContentHandler_t* contentHandler,
                                                         NW_DOM_ElementNode_t* elementNode)
  {
  CMapElementList* mapElementList = NULL;
  NW_Text_t* nameVal = NULL;
  NW_Text_Length_t length;
  NW_Uint8 freeNeeded;
  
  NW_TRY (status) 
    {
    // retrieve the map list from the content handler, if it does not exist, create one
    mapElementList = static_cast<CMapElementList*>(NW_XHTML_ContentHandler_GetImageMapLookupTable(contentHandler));
    if (mapElementList == NULL)
      {
      // create a new maplist to hold the map elements. deleted in the contenthandler destructor
      TRAP(status, mapElementList = CMapElementList::NewL());
      NW_THROW_ON (status, KBrsrOutOfMemory);
    
      // add the maplist to the content handler
      NW_XHTML_ContentHandler_SetImageMapLookupTable(contentHandler, mapElementList);
      }

    // retrieve the name of the map. this will be used as the key in our map list
    if (NW_DOM_ElementNode_getTagToken(elementNode) == NW_XHTML_ElementToken_map)
      {
      NW_XHTML_GetDOMAttribute (contentHandler, 
                                elementNode, 
                                NW_XHTML_AttributeToken_name, &nameVal);

      // if the name is empty, try to see if the id was specified
      if (nameVal == NULL)
        { 
        NW_XHTML_GetDOMAttribute (contentHandler, 
                                  elementNode, 
                                  NW_XHTML_AttributeToken_id, &nameVal);
        
        }
      } 

    if (nameVal != NULL)
      {
      // convert the mapName to Unicode and get the storage
      NW_Uint16* storage = (NW_Uint16*)NW_Text_GetUCS2Buffer (nameVal, 0, &length, &freeNeeded);
      NW_THROW_OOM_ON_NULL (storage, status);

      // add image map dom node to the list
      status = mapElementList->SetDomNode( storage, elementNode );
      _NW_THROW_ON_ERROR( status );
      }
    } 
  NW_CATCH (status) 
    {
    } 
  NW_FINALLY 
    {
    NW_Object_Delete( nameVal );
    return status;  
    } 
  NW_END_TRY
  }

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_XHTML_mapElementHandler_Class_t NW_XHTML_mapElementHandler_Class = {
  { /* NW_Object_Core              */
    /* super                       */ &NW_XHTML_ElementHandler_Class,
    /* queryInterface              */ _NW_Object_Core_QueryInterface
  },
  { /* NW_XHTML_ElementHandler     */
    /* initialize                  */ _NW_XHTML_mapElementHandler_Initialize,
    /* createBoxTree               */ _NW_XHTML_mapElementHandler_CreateBoxTree,
    /* processEvent                */ _NW_XHTML_ElementHandler_ProcessEvent
  },
  { /* NW_XHTML_mapElementHandler */
    /* unused                      */ 0
  }
};

/* ------------------------------------------------------------------------- *
   singleton definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_XHTML_mapElementHandler_t NW_XHTML_mapElementHandler = {
  { { &NW_XHTML_mapElementHandler_Class } }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_mapElementHandler_Initialize (const NW_XHTML_ElementHandler_t* elementHandler,
                                        NW_XHTML_ContentHandler_t* contentHandler,
                                        NW_DOM_ElementNode_t* elementNode)
{
  NW_TRY (status) 
  {
  // parameter assertions 
  NW_ASSERT (NW_Object_IsInstanceOf (contentHandler, &NW_XHTML_ContentHandler_Class));
  NW_ASSERT (elementNode != NULL);

    // special handling if map is the last part of the chunk: 
    // leave map to be processed in the next chunk
    // in case this is the last chunk, we need to process map in this chunk
    if (!NW_XHTML_ElementHandler_NextSiblingExists (contentHandler, elementNode, NW_TRUE))
    {
      NW_THROW_SUCCESS (status);
    }

  // invoke our superclass method 
  status = _NW_XHTML_ElementHandler_Initialize(elementHandler, contentHandler, elementNode);
  
  if (status != KBrsrSuccess) 
  {
    NW_THROW (status);
  }
  
  // insert the map name and dom node into the maplist
  status = NW_XHTML_mapElementHandler_InsertIntoImageMapLookupTable(contentHandler, elementNode);
  }
  NW_CATCH (status) 
  {
  }
  NW_FINALLY 
  {
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_mapElementHandler_CreateBoxTree (const NW_XHTML_ElementHandler_t* elementHandler,
                                           NW_XHTML_ContentHandler_t* contentHandler,
                                           NW_DOM_ElementNode_t* elementNode,
                                           NW_LMgr_ContainerBox_t* parentBox)
{
  NW_LMgr_Box_t *box = NULL;

  NW_TRY (status ) 
  {
    // special handling if a is the last node of the chunck: 
    // leave it to be processed in the next chunk
    if (!NW_XHTML_ElementHandler_NextSiblingExists (contentHandler, elementNode, NW_FALSE))
    {
      NW_THROW_SUCCESS (status);
    }

    box = (NW_LMgr_Box_t*)NW_LMgr_ContainerBox_New(0);
    NW_THROW_OOM_ON_NULL (box, status);

    // Add the box to the tree 
    status = NW_LMgr_ContainerBox_AddChild (parentBox, box);
    _NW_THROW_ON_ERROR (status);

    // Apply common attributes and styles 
    status = NW_XHTML_ElementHandler_ApplyStyles (elementHandler, contentHandler, 
                                         elementNode, &box, NULL);
    NW_THROW_ON (status, KBrsrOutOfMemory);

    // Apply Styles will take care of replacing container box if display was block, list-item or marquee.
    // In case of 'display:none' or an error it removes the container box. That's why we check box
    if (box == NULL)
    {
      NW_THROW_SUCCESS (status);
    }

    parentBox = NW_LMgr_ContainerBoxOf(box);

    /* invoke our superclass for completion */
    status = _NW_XHTML_ElementHandler_CreateBoxTree(elementHandler, contentHandler, elementNode, parentBox);
    NW_THROW_ON (status, KBrsrOutOfMemory);
   }
   NW_CATCH (status) 
   {
     if(box) 
     {
        NW_Object_Delete(box);
     }
   }
   NW_FINALLY {
     return status;
   } NW_END_TRY
}
