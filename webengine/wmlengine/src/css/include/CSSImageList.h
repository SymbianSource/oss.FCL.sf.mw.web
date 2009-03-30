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
* Description:  Hold different CSS images
*
*/


#ifndef CCSSImageList_H
#define CCSSImageList_H

//  INCLUDES
#include "BrsrStatusCodes.h"
#include "nw_text_ucs2.h"
#include "nw_image_abstractimage.h"
#include "nw_image_virtualimage.h"
#include "nw_image_epoc32simpleimage.h"
#include <nwx_http_header.h>
#include "nw_css_processor.h"
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES
class CVirtualNodeImageEntry;

typedef CArrayPtrFlat<CVirtualNodeImageEntry>	 CVirtualNodeImageList;

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  This class contains information about virtual node-image pairs associated
*  with an image list entry
*  @lib css.lib
*  @since 2.1
*/
class CVirtualNodeImageEntry : public CBase
  {
  public:

   /**
    * Two-phased constructor.
    * @return CVirtualNodeImageEntry*
	  */
    static CVirtualNodeImageEntry* NewL();

    /**
    * Two-phased constructor.
    * @return CVirtualNodeImageEntry*
	  */
    static CVirtualNodeImageEntry* NewLC();

    /**
    * Destructor.
    */
    virtual ~CVirtualNodeImageEntry();

    /**
    * This method add a new virtual image 
    * @since 2.1
    * @param aVirtualImage: virtual image
    * @return void
    */
    void AddVirtualImage( NW_Image_Virtual_t* aVirtualImage ) { iVirtualImage = aVirtualImage; }

    /**
    * This method add a new virtual node 
    * @since 2.1
    * @param aVirtualNode: virtual node associated with a virtual image
    * @return void
    */
    void AddVirtualNode( NW_DOM_ElementNode_t* aVirtualNode ) { iVirtualNode = aVirtualNode; }

    /**
    * This method sets the ownership of the virtual image
    * @since 2.1
    * @return void
    */
    void TakeVirtualImageOwnership( void ) { iImageOwnership = ETrue; }

    /**
    * This method removes the ownership of the virtual image
    * @since 2.1
    * @param aUrl: url of the image
    * @return void
    */
    void RemoveVirtualImageOwnership( void ) { iImageOwnership = EFalse; }

    /**
    * This method returns the virtual image
    * @since 2.1
    * @return NW_Image_Virtual_t* virtual image
    */
    NW_Image_Virtual_t* VirtualImage( void ) { return iVirtualImage; }

    /**
    * This method returns with virtual node
    * @since 2.1
    * @return NW_DOM_ElementNode_t* virtual node
    */
    NW_DOM_ElementNode_t* VirtulaNode( void ) { return iVirtualNode; }

    /**
    * This method returns the value of the ownership
    * @since 2.1
    * @return TBool: ETrue if the image is owned by the entry
    */
    TBool VirtualImageOwnership( void ) { return iImageOwnership; }

  private: // New functions

    /**
    * C++ default constructor.
    */
    CVirtualNodeImageEntry();

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();

  private:
    // associated virtual image. 
    // "ownership" member indicates
    // the imageOwnership of the image
    NW_Image_Virtual_t* iVirtualImage;
    // associated virtual DOM node. 
    // not owned
    NW_DOM_ElementNode_t* iVirtualNode;
    TBool iImageOwnership;
  };


/**
*  This class contains information about an image entry in the list
*  @lib css.lib
*  @since 2.1
*/
class CCSSImageListEntry : public CBase
{
public:

   /**
    * Two-phased constructor.
    * @return CCSSImageListEntry*
	  */
    static CCSSImageListEntry* NewL( NW_Text_t* aUrl, TBool aListItem );

    /**
    * Two-phased constructor.
    * @return CCSSImageListEntry*
	  */
    static CCSSImageListEntry* NewLC( NW_Text_t* aUrl, TBool aListItem );

    /**
    * Destructor.
    */
    virtual ~CCSSImageListEntry();

  public: // set-get functions
    
    /**
    * This method sets the image and its content type
    * @since 2.1
    * @param aImage: image object
    * @param aContentType: content type string
    * @return void
    */
    void SetAbstractImage( NW_Image_AbstractImage_t* aImage, NW_Http_ContentTypeString_t aContentType );

    /**
    * This method returns the listItem member variable
    * @since 2.1
    * @return TBool EFalse if the image is background image
    */
    TBool ListItem( ) { return iListItem; }

    /**
    * This method returns the "main" image which is the base of every
    * virtual images in the list
    * @since 2.1
    * @return NW_Image_AbstractImage_t* main image
    */
    NW_Image_AbstractImage_t* AbstractImage( ) { return iImage; }

    /**
    * This method returns the URL of the image. It is also the ID of the list.
    * @since 2.1
    * @return NW_Text_t* url
    */
    NW_Text_t* ImageUrl( ) { return iUrl; }

    /**
    * This method returns the content type of the image
    * @since 2.1
    * @return NW_Http_ContentTypeString_t content type
    */
    NW_Http_ContentTypeString_t ImageContentType( ) { return iContentTypeString; }
    
    /**
    * This method returns the virtual node-image list associated with this entry
    * @since 2.1
    * @return CVirtualNodeImageList* virtual node-image pair list
    */
    CVirtualNodeImageList* VirtualNodeImageList() { return iVirtualNodeImageList; }

    /**
    * This method appends a new virtual-node pair to the list
    * @since 2.1
    * @param aVirtualImageNodeEntry: virtual image-node pair entry
    * @return void
    */
    void AddVirtualImageNodeEntryL( CVirtualNodeImageEntry* aVirtualImageNodeEntry ) 
              { iVirtualNodeImageList->AppendL( aVirtualImageNodeEntry ); }

  private: // New functions

    /**
    * C++ default constructor.
    */
    CCSSImageListEntry( NW_Text_t* aUrl, TBool aListItem );

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();

  private: // members    
    
    // url of Image. owned
  NW_Text_t* iUrl;
    // content type of image. owned
    NW_Http_ContentTypeString_t iContentTypeString;
    // either background or list image
    TBool iListItem;
    // "main" image. owned
  NW_Image_AbstractImage_t* iImage;
    // list of virtual node-image pairs
    CVirtualNodeImageList* iVirtualNodeImageList;
};
// CLASS DECLARATION

/**
*  This class has different CSS images that can be used by the document
*  @lib css.lib
*  @since 2.1
*/
class CCSSImageList : public CBase
{
  public:  // Constructors and destructor
        
   /**
    * Two-phased constructor.
    * @return CCSSHandler*
	  */
    static CCSSImageList* NewL( NW_CSS_Processor_t& aProcessor );

    /**
    * Destructor.
    */
    virtual ~CCSSImageList();

  public: // New functions
        
    /**
    * This method creates a new image entry for a given URL
    * @since 2.1
    * @param aUrl: url of the image
    * @param aImageNode: associated DOM node
    * @param aListItem: image can be either list item or background image
    * @return void
    */
    void CreateImageEntryL( NW_Text_t* aUrl, NW_DOM_ElementNode_t* aImageNode, TBool aListItem );

    /**
    * This method add a new virtual node for a given URL
    * @since 2.1
    * @param aUrl: url of the image
    * @param aVirtualImageNode: virtual node
    * @return void
    */
    NW_Image_AbstractImage_t* AddVirtualNodeToImageEntryL( NW_Text_t* aUrl, NW_DOM_ElementNode_t* aVirtualImageNode );

    /**
    * This method sets the image for a given URL
    * @since 2.1
    * @param aUrl: url of the image
    * @param aData: image data
    * @param aLength: image length
    * @param aContentTypeString: content type of image
    * @return image object 
    */
    NW_Image_AbstractImage_t* SetImageL(NW_Text_t* aUrl, NW_Byte* aData, NW_Uint32 aLength, 
              NW_Http_ContentTypeString_t aContentTypeString );

    /**
    * This method returns the image corresponding to a given URL
    * @since 2.1
    * @param aUrl: url of the image
    * @param aFound: denotes if entry exists
    * @return corresponding image
    */
    NW_Image_AbstractImage_t* GetImageL( NW_Text_t* aUrl, TBool& aFound );

    /**
    * This method returns the image list entry corresponding to a given virtual image
    * @since 2.1
    * @param aVirtualImage: image
    * @param aImageNode: corresponding node 
    * @return CCSSImageListEntry* corresponding image list entry
    */
    CCSSImageListEntry* EntryByVirtualImage( NW_Image_AbstractImage_t* aVirtualImage, 
                                                            NW_DOM_ElementNode_t** aImageNode );
    /**
    * This method removes the virtual image ownership.
    * @since 2.1
    * @param aImageEnrty: image enrty that holds the virtual image
    * @param aVirtualImage: image to be removed
    * @return void
    */
    void RemoveVirtualImageOwnership( CCSSImageListEntry& aImageEnrty, NW_Image_AbstractImage_t* aVirtualImage );

    /**
    * This method removes the virtual image.
    * @since 2.1
    * @param aImageEnrty: image enrty that holds the virtual image
    * @param aVirtualImage: image to be removed
    * @return void
    */
    void VirtualImageDestroyed( CCSSImageListEntry& aImageEnrty, NW_Image_AbstractImage_t* aVirtualImage );

    /**
    * This method Write CSS image components to a file in the form of multipart segments.
    * Each segment consists of:
    *                  header length (including content type length)
    *                  image data length
    *                  content type (including character encoding)
    *                  headers
    *                  image
    * @since 2.1
    * @param aFh: file name
    * @return KBrsrSuccess, KBrsrOutOfMemory, KBrsrFailure, or KBrsrSavedPageFailed
    */
    TBrowserStatusCode WriteMultipartSegments(NW_Osu_File_t aFh);

    /**
    * This method returns the size of the imagelist
    * @since 2.1
    * @return size of the imageList
    */
    TInt GetNumImages();

    /**
    * This method issues load request for urls which don't have image
    * @since 2.1
    * @return KBrsrSuccess or KBrsrOutOfMemory
    */
    TBrowserStatusCode ShowImages();

  private: // New functions

    /**
    * C++ default constructor.
    */
    CCSSImageList( NW_CSS_Processor_t& aProcessor );

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();

    /**
    * This method gets CSS image for a given URL
    * @since 2.1
    * @param aUrl: url of the image to be fetched
    * @param aRetEntry: image
    * @return KBrsrSuccess, KBrsrOutOfMemory, KBrsrNotFound
    */
    TBool QueryEntryL (NW_Text_t* aUrl, CCSSImageListEntry*& aRetEntry);

    /**
    * This method changes the ownership status of a virtual image.
    * @since 2.1
    * @param aImageEnrty: image enrty that holds the virtual image
    * @param aVirtualImage: image to be removed
    * @param aDestroyed: ETrue if the image was destroyed
    * @return void
    */
    void VirtualImageOwnershipChanged( CCSSImageListEntry& aImageEnrty, NW_Image_AbstractImage_t* aVirtualImage, 
                                       TBool aDestroyed );
  
  
    typedef CArrayPtrFlat<CCSSImageListEntry> CImageListArray;

  private:    // Data

    // vector containing the images. owned
    CImageListArray* iImageList;
    // CSS processor. not owned
    NW_CSS_Processor_t* iProcessor;
};

#endif /* CCSSImageList_H */
