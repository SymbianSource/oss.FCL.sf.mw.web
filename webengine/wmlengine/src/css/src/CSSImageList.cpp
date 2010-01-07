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
* Description:  Hold different CSS images that are applied to document
*
*/

// INCLUDE FILES
#include <e32def.h>  // First to avoid NULL redefine warning (no #ifndef NULL).
#include "BrsrStatusCodes.h"
#include "CSSImageList.h"
#include "nw_text_ucs2.h"
#include "nw_image_virtualimage.h"
#include "epoc32imagedecoder.h"
#include "nwx_multipart_generator.h"
#include "nwx_string.h"
#include "nwx_http_defs.h"
#include "nw_css_processori.h"
#include "nwx_settings.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
const TInt KCSSImageListArrayGranularity = 2;

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CVirtualNodeImageEntry::CVirtualNodeImageEntry
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVirtualNodeImageEntry::CVirtualNodeImageEntry( )
  {
  }

// -----------------------------------------------------------------------------
// CVirtualNodeImageEntry::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVirtualNodeImageEntry::ConstructL( )
  {
  }

// -----------------------------------------------------------------------------
// CVirtualNodeImageEntry::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVirtualNodeImageEntry* CVirtualNodeImageEntry::NewL()
  {
  CVirtualNodeImageEntry* self = new( ELeave )CVirtualNodeImageEntry();

  CleanupStack::PushL( self );
  self->ConstructL();
  CleanupStack::Pop();

  return self;
  }

// -----------------------------------------------------------------------------
// CVirtualNodeImageEntry::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVirtualNodeImageEntry* CVirtualNodeImageEntry::NewLC()
  {
  CVirtualNodeImageEntry* self = CVirtualNodeImageEntry::NewL();
  CleanupStack::PushL( self );

  return self;
  }

// -----------------------------------------------------------------------------
// CVirtualNodeImageEntry::~CVirtualNodeImageEntry
// destructor.
// -----------------------------------------------------------------------------
//
CVirtualNodeImageEntry::~CVirtualNodeImageEntry()
  {
  if( iImageOwnership )
    {
    // delete the image
    NW_Object_Delete( iVirtualImage );
    }
  }

// -----------------------------------------------------------------------------
// CCSSImageListEntry::CCSSImageListEntry
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCSSImageListEntry::CCSSImageListEntry( NW_Text_t* aUrl, TBool aListItem )
  : iUrl( aUrl ), iListItem( aListItem )
  {
  }

// -----------------------------------------------------------------------------
// CCSSImageListEntry::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCSSImageListEntry::ConstructL( )
  {
  // initialize the virtual node-image pair list
  iVirtualNodeImageList = new (ELeave) CVirtualNodeImageList( KCSSImageListArrayGranularity );
  }

// -----------------------------------------------------------------------------
// CCSSImageListEntry::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCSSImageListEntry* CCSSImageListEntry::NewL( NW_Text_t* aUrl, TBool aListItem )
  {
  CCSSImageListEntry* self = new( ELeave ) CCSSImageListEntry( aUrl, aListItem );

  CleanupStack::PushL( self );
  self->ConstructL();
  CleanupStack::Pop();

  return self;
  }

// -----------------------------------------------------------------------------
// CCSSImageListEntry::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCSSImageListEntry* CCSSImageListEntry::NewLC( NW_Text_t* aUrl, TBool aListItem )
  {
  CCSSImageListEntry* self = CCSSImageListEntry::NewL( aUrl, aListItem );
  CleanupStack::PushL( self );

  return self;
  }

// -----------------------------------------------------------------------------
// CCSSImageListEntry::~CCSSImageListEntry
// Destructor.
// -----------------------------------------------------------------------------
CCSSImageListEntry::~CCSSImageListEntry()
  {
  NW_Object_Delete( iUrl );

  NW_Mem_Free( iContentTypeString );
  // free virtual images
  if(iVirtualNodeImageList)
    {
    iVirtualNodeImageList->ResetAndDestroy();
    }
  delete iVirtualNodeImageList;
  iVirtualNodeImageList = NULL;

  // it's important to release virtual images first
  // as they might be observing this image
  // we are not observing the "main image"
  NW_Object_Delete( iImage );
  }

// -----------------------------------------------------------------------------
// CCSSImageListEntry::SetAbstractImage
// Sets the image and its content type
// -----------------------------------------------------------------------------
//
void CCSSImageListEntry::SetAbstractImage( NW_Image_AbstractImage_t* aImage, NW_Http_ContentTypeString_t aContentType )
  {
  iImage = aImage;
  iContentTypeString = aContentType;
  }

// -----------------------------------------------------------------------------
// CCSSImageList::QueryEntry
// Gets CSS image for a given URL
// -----------------------------------------------------------------------------
//
TBool CCSSImageList::QueryEntryL (NW_Text_t* aUrl, CCSSImageListEntry*& aRetEntry )
{
  NW_Text_Length_t length;
  NW_Uint8 freeNeeded;
  TBool found( EFalse );

  /* convert the url to Unicode and get the storage */
  NW_Ucs2* storage = (NW_Ucs2*)NW_Text_GetUCS2Buffer (aUrl, 0, &length, &freeNeeded);
  User::LeaveIfNull( storage );

  TInt size = iImageList->Count();
  for (TInt index = 0; index < size; index++)
  {
    CCSSImageListEntry* entry = iImageList->At( index );

    NW_Text_Length_t len = 0;
    NW_Text_t* imageUrl = entry->ImageUrl();

    const void* urlStorage = NW_Text_GetUCS2Buffer( imageUrl, 0, &len, NULL );

    if (!(NW_Mem_memcmp (urlStorage, storage, (length+1) *sizeof (NW_Ucs2))))
    {
      found = ETrue;
      aRetEntry = entry;
      break;
    }
  } /* end for */

  if (freeNeeded)
  {
    NW_Mem_Free ((NW_Ucs2*)storage);
  }
  return found;
}

// -----------------------------------------------------------------------------
// CCSSImageList::CCSSImageList
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCSSImageList::CCSSImageList( NW_CSS_Processor_t& aProcessor ) : iProcessor( &aProcessor )
  {
  }

// -----------------------------------------------------------------------------
// CCSSImageList::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCSSImageList::ConstructL()
  {
    // initialize the imageList
  iImageList = new( ELeave )CImageListArray( KCSSImageListArrayGranularity );
  }

// -----------------------------------------------------------------------------
// CCSSImageList::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCSSImageList* CCSSImageList::NewL( NW_CSS_Processor_t& aProcessor )
  {
  CCSSImageList* self = new( ELeave )CCSSImageList( aProcessor );

  CleanupStack::PushL( self );
  self->ConstructL();
  CleanupStack::Pop();

  return self;
  }

// -----------------------------------------------------------------------------
// CCSSImageList::~CCSSImageList
// Destructor.
// -----------------------------------------------------------------------------
//
CCSSImageList::~CCSSImageList()
{
  // first remove observers
  TInt size = 0;  
  if(iImageList)
    {
    size = iImageList->Count();
    }

  for( TInt index = 0; index < size; index++ )
    {
    CCSSImageListEntry* entry = iImageList->At( index );
    // check the virtual images list
    CVirtualNodeImageList* virtualImageList = entry->VirtualNodeImageList();
    TInt virtualImageCount = virtualImageList->Count();

    for( TInt i = 0; i < virtualImageCount; i++ )
      {
      NW_Image_Virtual_t* virtualImage;
      CVirtualNodeImageEntry* virtualImageEntry = virtualImageList->At( i );

      if( virtualImageEntry && ( virtualImage = virtualImageEntry->VirtualImage() ) != NULL )
        {
        // remove observer
        (void) NW_Image_AbstractImage_RemoveImageObserver( (NW_Image_AbstractImage_t*) virtualImage,
          &iProcessor->NW_Image_IImageObserver );
        }
      }
    }
  // destroy image list
  if(iImageList)
    {
    iImageList->ResetAndDestroy();
    }
  delete iImageList;
}

// -----------------------------------------------------------------------------
// CCSSImageList::CreateImageEntryL
// Creates a new entry to the image list where the URL is the ID
// -----------------------------------------------------------------------------
//
void CCSSImageList::CreateImageEntryL( NW_Text_t* aUrl, NW_DOM_ElementNode_t* aImageNode, TBool aListItem )
{
  // create and empty image entry
  CCSSImageListEntry* newCSSImageEntry = CCSSImageListEntry::NewLC( aUrl, aListItem );

  // Virtual image list includes both the node and
  // the image itself
  CVirtualNodeImageEntry* newVirtualNodeImageListEntry = CVirtualNodeImageEntry::NewLC();

  newVirtualNodeImageListEntry->AddVirtualNode( aImageNode );
  // no image yet
  newVirtualNodeImageListEntry->AddVirtualImage( NULL );
  // no ownership
  newVirtualNodeImageListEntry->RemoveVirtualImageOwnership();

  // take image list onwership
  newCSSImageEntry->AddVirtualImageNodeEntryL( newVirtualNodeImageListEntry );
  CleanupStack::Pop(); // newVirtualNodeImageListEntry

  // and append to the list. take ownership
  iImageList->AppendL( newCSSImageEntry );
  CleanupStack::Pop(); // newCSSImageEntry
  }

// -----------------------------------------------------------------------------
// CCSSImageList::AddVirtualNodeToImageEntryL
// adds a new virtual node for a given URL to the image entry list
// -----------------------------------------------------------------------------
//
NW_Image_AbstractImage_t*
CCSSImageList::AddVirtualNodeToImageEntryL( NW_Text_t* aUrl, NW_DOM_ElementNode_t* aVirtualImageNode )
  {
  NW_Image_AbstractImage_t* image = NULL;

  CCSSImageListEntry *entry = NULL;
  if( QueryEntryL( aUrl, entry ) )
    {
    CVirtualNodeImageEntry* newVirtualNodeImageEntry = CVirtualNodeImageEntry::NewLC();
    // add the virtual node first
    newVirtualNodeImageEntry->AddVirtualNode( aVirtualImageNode );
    NW_Image_Virtual_t* virtualImage = NULL;

    // if the (main)image has not been loaded yet,
    // then entry->iImage is NULL.
    if( entry->AbstractImage() )
      {
      virtualImage = NW_Image_Virtual_New( entry->AbstractImage() );
      User::LeaveIfNull( virtualImage );

      // take the image ownership
      newVirtualNodeImageEntry->TakeVirtualImageOwnership();

      image = NW_Image_AbstractImageOf( virtualImage );

      // and set processor as observer
      (void) NW_Image_AbstractImage_SetImageObserver( image,
        &iProcessor->NW_Image_IImageObserver );
      }
    else
      {
      // create the virtual image entry when the abstract comes in
      // for the time being, add NULL pointer to the list
      // no ownership
      newVirtualNodeImageEntry->RemoveVirtualImageOwnership();
      }
    newVirtualNodeImageEntry->AddVirtualImage( virtualImage );
    // add to the list. take ownership
    entry->AddVirtualImageNodeEntryL( newVirtualNodeImageEntry );
    CleanupStack::Pop(); // newVirtualNodeImageEntry
    }
  return image;
  }

// -----------------------------------------------------------------------------
// CCSSImageList::SetImageL
// sets the image for a given URL. It also checks the virtual node list.
// -----------------------------------------------------------------------------
NW_Image_AbstractImage_t*
CCSSImageList::SetImageL(NW_Text_t* aUrl, NW_Byte* aData, NW_Uint32 aLength,
                         NW_Http_ContentTypeString_t aContentTypeString)
    { 
    NW_Image_AbstractImage_t* image = NULL;
    CCSSImageListEntry *entry = NULL;
    
    if (QueryEntryL(aUrl, entry))
        {
        NW_GDI_Dimension3D_t size = {0, 0, 0};
        
        // Do we have an image content-type that Symbian won't recognize,
        // but the browser supports.
        TImageType imageType = GetRecognizedImageType((const char*)aContentTypeString);

        // make a copy of content type string as the caller
        // (_NW_CSS_Processor_ILoadRecipient_ProcessLoad) deletes it.
        TInt contentTypeLength = NW_Asc_strlen((char*)aContentTypeString);
        NW_Http_ContentTypeString_t contentTypeString =
                          (NW_Http_ContentTypeString_t)NW_Asc_New(contentTypeLength + 1);
        User::LeaveIfNull(contentTypeString);
        
        NW_Asc_strcpy((char*)contentTypeString, (char*)aContentTypeString);
        contentTypeString[contentTypeLength] = '\0';
        
        // create the abstract image
        image = (NW_Image_AbstractImage_t*)NW_Image_Epoc32Simple_New(size, NULL, NULL,
                                                                     NW_FALSE, NW_FALSE, imageType,
                                                                     aData, aLength,
                                                                     iProcessor->owner);
        User::LeaveIfNull(image);
       
        // set to the list
        entry->SetAbstractImage(image, contentTypeString);
        
        // check if there is a virtual image which is waiting for to be
        // updated. - create as many new virtual images out of entry->iImage as
        // virtual nodes we have
        // note: one virtual image must be created here for the main node
        // as the main box -which is not the virtual one- musn't own
        // simple image.
        // this "extra" virtual node is appended to the list at
        // CreateImageEntryL()
        CVirtualNodeImageList* virtualNodeImageList = entry->VirtualNodeImageList();
        TInt virtualImageCount = virtualNodeImageList->Count();
        for (TInt i = 0; i < virtualImageCount; i++)
            {
            CVirtualNodeImageEntry* virtualNodeImageEntry = virtualNodeImageList->At(i);
            if (!virtualNodeImageEntry->VirtualImage())
                {
                // create virtual image
                NW_Image_Virtual_t* virtualImage = NW_Image_Virtual_New(entry->AbstractImage());
                User::LeaveIfNull(virtualImage);
                // update the entry
                virtualNodeImageEntry->AddVirtualImage(virtualImage);
                virtualNodeImageEntry->TakeVirtualImageOwnership();
                
                // and set processor as observer
                (void)NW_Image_AbstractImage_SetImageObserver((NW_Image_AbstractImage_t*)virtualImage,
                                                              &iProcessor->NW_Image_IImageObserver);
                // and update the list
                virtualNodeImageList->At(i) = virtualNodeImageEntry;
                }
            }
#ifdef INCREMENTAL_IMAGE_ON
        NW_Image_Epoc32Simple_Initialize((NW_Image_Epoc32Simple_t*)image);
        // start opening the image
        ((CEpoc32ImageDecoder*)((NW_Image_Epoc32Simple_t*)image)->decoder)->Open();
#else 
        NW_Image_Epoc32Simple_Initialize((NW_Image_Epoc32Simple_t*)image);

#endif // INCREMENTAL_IMAGE_ON
        }   // end of if (QueryEntry...
    
    return image;
    }

// -----------------------------------------------------------------------------
// CCSSImageList::GetImageL
// gets the image for a given URL
// -----------------------------------------------------------------------------
//
NW_Image_AbstractImage_t*
CCSSImageList::GetImageL( NW_Text_t* aUrl, TBool& aFound )
{
  CCSSImageListEntry* entry = NULL;
  NW_Image_AbstractImage_t* image = NULL;

  aFound = QueryEntryL( aUrl, entry );

  if( aFound && entry != NULL )
    {
    image = entry->AbstractImage();
    }
  return image;
}

// -----------------------------------------------------------------------------
// CCSSImageList::EntryByVirtualImage
// gets both the image entry and the node for a given virtual image
// -----------------------------------------------------------------------------
//
CCSSImageListEntry* CCSSImageList::EntryByVirtualImage( NW_Image_AbstractImage_t* aVirtualImage,
                                                        NW_DOM_ElementNode_t** aImageNode )
  {
  TInt size = iImageList->Count();
  // lookup the virtual node-image list
  for( TInt index = 0; index < size; index++ )
    {
    CCSSImageListEntry* entry = iImageList->At( index );
    // check the virtual images list
    CVirtualNodeImageList* virtualNodeImageList = entry->VirtualNodeImageList();
    TInt virtualNodeImageCount = virtualNodeImageList->Count();
    for( TInt i = 0; i < virtualNodeImageCount; i++ )
      {
      NW_Image_Virtual_t* virtualImage;
      CVirtualNodeImageEntry* virtualNodeImageEntry = virtualNodeImageList->At( i );

      if( virtualNodeImageEntry && ( virtualImage = virtualNodeImageEntry->VirtualImage() ) != NULL )
        {
        if( NW_Image_AbstractImageOf( virtualImage ) == aVirtualImage )
          {
          // set image node. note that NULL can be passed if
          // the caller is not interested in the node value
          if( aImageNode != NULL )
            {
            *aImageNode = virtualNodeImageEntry->VirtulaNode();
            }
          return entry;
          }
        }
      }
    }
  return NULL;
  }

// -----------------------------------------------------------------------------
// CCSSImageList::RemoveVirtualImageOwnership
// remove virtual image ownership
// -----------------------------------------------------------------------------
//
void CCSSImageList::RemoveVirtualImageOwnership( CCSSImageListEntry& aImageEnrty, NW_Image_AbstractImage_t* aVirtualImage )
  {
  VirtualImageOwnershipChanged( aImageEnrty, aVirtualImage, EFalse );
  }

// -----------------------------------------------------------------------------
// CCSSImageList::VirtualImageDestroyed
// set virtual image to NULL
// -----------------------------------------------------------------------------
//
void CCSSImageList::VirtualImageDestroyed( CCSSImageListEntry& aImageEnrty, NW_Image_AbstractImage_t* aVirtualImage )
  {
  VirtualImageOwnershipChanged( aImageEnrty, aVirtualImage, ETrue );
  }

// -----------------------------------------------------------------------------
// CCSSImageList::VirtualImageOwnershipChanged
// it changes the ownership of the virtual image
// -----------------------------------------------------------------------------
//
void CCSSImageList::VirtualImageOwnershipChanged( CCSSImageListEntry& aImageEnrty, NW_Image_AbstractImage_t* aVirtualImage,
                                         TBool aDestroyed )
  {
  // go through the virtual list to find "aVirtualImage" and remove the ownership
  // by setting iImageOwnership to EFalse
  CVirtualNodeImageList* virtualNodeImageList = aImageEnrty.VirtualNodeImageList();
  if( virtualNodeImageList )
    {
    TInt virtualImageCount = virtualNodeImageList->Count();

    for( TInt i = 0; i < virtualImageCount; i++ )
      {
      NW_Image_Virtual_t* virtualImage;
      CVirtualNodeImageEntry* virtualNodeImageEntry = virtualNodeImageList->At( i );
      // check if entry has the ownership
      if( virtualNodeImageEntry && ( virtualImage = virtualNodeImageEntry->VirtualImage() ) != NULL )
  {
        if( NW_Image_AbstractImageOf( virtualImage ) == aVirtualImage )
    {
          // remove ownership
          virtualNodeImageEntry->RemoveVirtualImageOwnership();
          // set image to NULL
          if( aDestroyed )
      {
            virtualNodeImageEntry->AddVirtualImage( NULL );
            }
          virtualNodeImageList->At( i ) = virtualNodeImageEntry;
          return;
          }
      }
    }
  }
}


// -----------------------------------------------------------------------------
// CCSSImageList::WriteMultipartSegments
// Write CSS images to a file in the form of multipart segments.
// -----------------------------------------------------------------------------
//
TBrowserStatusCode CCSSImageList::WriteMultipartSegments(NW_Osu_File_t aFh)
{
  const NW_Byte* data = NULL;
  NW_Uint32 dataLen = 0;
  const NW_Ucs2* storage = NULL;
  NW_Text_Length_t length;
  NW_Uint8 freeNeeded;
  char* urlAscii = NULL;
  TBrowserStatusCode retStatus = KBrsrSuccess;
  NW_Image_Epoc32Simple_t* image = NULL;

  TUint originalEncoding = NW_Settings_GetOriginalEncoding();

  TInt size = iImageList->Count();
  for (TInt index = 0; index < size; index++)
  {
    NW_Byte* segment;
    NW_Uint32 segmentLen;

    CCSSImageListEntry* entry = iImageList->At(index);
    image = (NW_Image_Epoc32Simple_t*) entry->AbstractImage();

    if (image != NULL)
    {
      data = (const NW_Byte*)image->rawData;
      dataLen = image->rawDataLength;
    }

    /* Get the URL. If urlAscii is NULL, it will continue to create
     * segment - this is OK. */
    NW_Text_t* imageUrl = entry->ImageUrl();
    storage = NW_Text_GetUCS2Buffer (imageUrl, 0, &length, &freeNeeded);
    if (storage != NULL)
    {
      urlAscii = NW_Str_CvtToAscii(storage);
      if (freeNeeded)
        NW_Mem_Free ((NW_Ucs2*) storage);
    }

    /* The encoding type doesn't seem to matter here. */
    retStatus = NW_CreateMultipartSegment
                (
                  HTTP_iso_8859_1,
                  originalEncoding,
				  entry->ImageContentType(),
                  urlAscii,
                  data,
                  dataLen,
                  &segment,
                  &segmentLen
                );
    NW_Mem_Free(urlAscii);
    if (retStatus != KBrsrSuccess)
      return retStatus;

    retStatus = NW_Osu_WriteFile(aFh, segment, segmentLen);
    NW_Mem_Free(segment);
    if (retStatus != KBrsrSuccess)
      return retStatus;
  }
  return retStatus;
}

// -----------------------------------------------------------------------------
// CCSSImageList::GetNumImages
// returns the size of the imagelist
// -----------------------------------------------------------------------------
//
TInt CCSSImageList::GetNumImages()
{
  return iImageList->Count();
}

// -----------------------------------------------------------------------------
// CCSSImageList::ShowImagesL
// issues load request for urls which don't have image
// -----------------------------------------------------------------------------
//
TBrowserStatusCode CCSSImageList::ShowImages()
  {
  TBrowserStatusCode status = KBrsrSuccess;

  TInt numImages = GetNumImages() ;
  for (TInt index = 0; index < numImages; index++)
    {
    CCSSImageListEntry* entry = iImageList->At( index );

    if( entry->ImageUrl() != NULL && entry->AbstractImage() == NULL )
      {
      status = NW_CSS_Processor_ProcessLoadRequest( iProcessor, 
                                                    entry->ImageUrl(), 
                                                    NW_CSS_Processor_LoadImage,
                                                    iProcessor->isSavedDeck,
                                                    iProcessor->isHistLoad,
													iProcessor->isReLoad,
                                                    0);

      if( status == KBrsrOutOfMemory )
        {
        break;
        }
      }
    }
  return status;
  }

