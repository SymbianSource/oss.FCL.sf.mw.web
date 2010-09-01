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


#include "nw_hed_historyentryi.h"

#include "NW_Text_Abstract.h"
#include "nw_adt_resizablevector.h"
#include "nw_text_ucs2.h"
#include "nwx_string.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   static functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_HED_HistoryEntry_NewContentData (NW_HED_HistoryEntry_t* thisObj)
{
  TBrowserStatusCode status = KBrsrOutOfMemory;

  if (thisObj->contentData == NULL) {
    NW_THROWIF_NULL (thisObj->contentData = NW_Object_Shared_New (NULL));
    NW_THROWIF_ERROR (status = NW_Object_Shared_Reserve (thisObj->contentData));
  }

  return KBrsrSuccess;

NW_CATCH_ERROR
  NW_Object_Delete (thisObj->contentData);
  thisObj->contentData = NULL;

  return status;
}

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_HED_HistoryEntry_Class_t NW_HED_HistoryEntry_Class = {
  { /* NW_Object_Core          */
    /* super                   */ &NW_Object_Dynamic_Class,
    /* queryInterface          */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base          */
    /* interfaceList           */ NULL
  },
  { /* NW_Object_Dynamic       */
    /* instanceSize            */ sizeof (NW_HED_HistoryEntry_t),
    /* construct               */ _NW_HED_HistoryEntry_Construct,
    /* destruct                */ _NW_HED_HistoryEntry_Destruct
  },
  { /* NW_HED_HistoryEntry     */
    /* unused                  */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_HistoryEntry_Construct (NW_Object_Dynamic_t* dynamicObject,
                                va_list* argp)
{
  NW_HED_HistoryEntry_t* thisObj;
  
  /* parameter assertions */
  NW_ASSERT (dynamicObject != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject, &NW_HED_HistoryEntry_Class));
  NW_ASSERT (argp != NULL);

  /* for convenience */
  thisObj = NW_HED_HistoryEntryOf (dynamicObject);

  /* set the member variables */
  thisObj->urlRequest = (NW_HED_UrlRequest_t*) va_arg (*argp, const NW_HED_UrlRequest_t*);
  NW_ASSERT (thisObj->urlRequest != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj->urlRequest, &NW_HED_UrlRequest_Class));

  thisObj->urlResponse = (NW_HED_UrlResponse_t*) va_arg (*argp, const NW_HED_UrlResponse_t*);
  NW_ASSERT (thisObj->urlResponse != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj->urlResponse, &NW_HED_UrlResponse_Class));

  thisObj->contentData = NULL;

  thisObj->pageTitle = NULL;
  thisObj->activeBoxId = 0;
  thisObj->docPosition.x = 0;
  thisObj->docPosition.y = 0;
  /* 
  ** create the NW_Object_Shared that allows us to treat history-entry as a 
  ** reference counted obj.  The weird thing about this is that thisObj points
  ** to thisObj->sharingObject which in turn points to thisObj.  This is on 
  ** purpose...  When thisObj->sharingObject is released for the last time it
  ** WILL delete "thisObj".
  */
  thisObj->sharingObject = NW_Object_Shared_New (NW_Object_DynamicOf (thisObj));

  if(thisObj->sharingObject == NULL) return KBrsrOutOfMemory;
  
  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
void
_NW_HED_HistoryEntry_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_HED_HistoryEntry_t* thisObj;

  /* parameter assertions */
  NW_ASSERT (dynamicObject != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject, &NW_HED_HistoryEntry_Class));

  /* for convenience */
  thisObj = NW_HED_HistoryEntryOf (dynamicObject);

  /* deallocate the member variables */
  NW_Object_Delete (thisObj->urlRequest);
  NW_Object_Delete (thisObj->urlResponse);

  NW_Mem_Free(thisObj->pageTitle);

  if (thisObj->contentData != NULL) {
    (void) NW_Object_Shared_Release (thisObj->contentData);
  }
}

/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_HED_HistoryEntry_AdoptContentData (NW_HED_HistoryEntry_t* thisObj,
                                     NW_Object_Dynamic_t* contentData)
{
  TBrowserStatusCode status;

  /* parameter assertions */
  NW_ASSERT (thisObj != NULL);

  /* if need be create the shared object */
  if (thisObj->contentData == NULL) {
    NW_THROWIF_ERROR (status = NW_HED_HistoryEntry_NewContentData (thisObj));
  }

  /* adopt the contentData */
  NW_THROWIF_ERROR (status = NW_Object_Shared_AdoptObject (thisObj->contentData, contentData));

  return KBrsrSuccess;

NW_CATCH_ERROR
  return status;
}

/* ------------------------------------------------------------------------- */
NW_Object_Dynamic_t*
NW_HED_HistoryEntry_OrphanContentData (NW_HED_HistoryEntry_t* thisObj)
{
  NW_Object_Dynamic_t* contentData = NULL;

  /* parameter assertions */
  NW_ASSERT (thisObj != NULL);

  if (thisObj->contentData != NULL) {
    (void) NW_Object_Shared_OrphanObject (thisObj->contentData, &contentData);
  }

  return contentData;
}

/* ------------------------------------------------------------------------- */
NW_Object_Shared_t*
NW_HED_HistoryEntry_GetContentDataHolder (NW_HED_HistoryEntry_t* thisObj)
{
  /* parameter assertions */
  NW_ASSERT (thisObj != NULL);

  /* if the contentData object hasn't been created, create it */
  if (thisObj->contentData == NULL) {
    NW_HED_HistoryEntry_NewContentData (thisObj);
  }

  return thisObj->contentData;
}

/* ------------------------------------------------------------------------- */
NW_HED_HistoryEntry_t*
NW_HED_HistoryEntry_Clone (const NW_HED_HistoryEntry_t* thisObj)
{
  NW_HED_UrlRequest_t* urlReq = NULL;
  NW_HED_UrlResponse_t* urlResp = NULL;
  NW_HED_HistoryEntry_t* newEntry = NULL;

  NW_THROWIF_NULL (urlReq = NW_HED_UrlRequest_Clone (thisObj->urlRequest));
  NW_THROWIF_NULL (urlResp = NW_HED_UrlResponse_Clone (thisObj->urlResponse));
  NW_THROWIF_NULL (newEntry = NW_HED_HistoryEntry_New (urlReq, urlResp));

  /* 
  ** note that contentData is not copied, this is on purpose.  The cloned
  ** entry must point to the same shared-object.
  */
  
  if (thisObj->contentData != NULL) {
    newEntry->contentData = thisObj->contentData;
    (void) NW_Object_Shared_Reserve (newEntry->contentData);
  }

  return newEntry;

NW_CATCH_ERROR
  NW_Object_Delete (urlReq);
  NW_Object_Delete (urlResp);

  return NULL;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_HED_HistoryEntry_Reserve (NW_HED_HistoryEntry_t* thisObj)
{
  return NW_Object_Shared_Reserve (thisObj->sharingObject);
}

/* ------------------------------------------------------------------------- */
NW_Bool
NW_HED_HistoryEntry_Release (NW_HED_HistoryEntry_t* thisObj)
{
  return NW_Object_Shared_Release (thisObj->sharingObject);
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_HED_HistoryEntry_SetPageTitle(NW_HED_HistoryEntry_t* entry, NW_Ucs2* pageTitle)
{ 
  if(entry->pageTitle != NULL)
      {
      NW_Mem_Free(entry->pageTitle);
      entry->pageTitle = NULL;
      }
  entry->pageTitle = NW_Str_Newcpy(pageTitle);
  if(entry->pageTitle == NULL) return KBrsrOutOfMemory;
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
void
NW_HED_HistoryEntry_SetDocPosition(NW_HED_HistoryEntry_t* entry, 
                                   NW_GDI_Point2D_t* position)
{
  entry->docPosition.x = position->x;
  entry->docPosition.y = position->y;
}

/* ------------------------------------------------------------------------- */
void
NW_HED_HistoryEntry_GetDocPosition(NW_HED_HistoryEntry_t* entry, 
                                   NW_GDI_Point2D_t* position)
{
  position->x = entry->docPosition.x;
  position->y = entry->docPosition.y;
}

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_HED_HistoryEntry_t*
NW_HED_HistoryEntry_New (NW_HED_UrlRequest_t* urlRequest, NW_HED_UrlResponse_t* urlResponse)
{
  NW_ASSERT (urlRequest);

  return (NW_HED_HistoryEntry_t*)
    NW_Object_New (&NW_HED_HistoryEntry_Class, urlRequest, urlResponse);
}
