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
* Description:  Hold different map element objects
*
*/


#ifndef MAPELEMENTLIST_H
#define MAPELEMENTLIST_H

//  INCLUDES
#include "nw_xhtml_elementhandler.h"
#include "BrsrStatusCodes.h"
#include "nw_lmgr_imagemapbox.h"

#include <e32base.h>
#include "nwx_string.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FORWARD DECLARATIONS
typedef CArrayPtrFlat<NW_LMgr_ImageMapBox_t>	 CImageMapBoxList;

// CLASS DECLARATION

/**
*  This class contains information about a map element entry in the list
*  @lib xhtml.lib
*  @since 2.1
*/
class CMapElementEntry : public CBase
  {  
  public:

   /**
    * Two-phased constructor.
    * @return CMapElementEntry*
	  */
    static CMapElementEntry* NewL( TUint16* aMapName );

    /**
    * Two-phased constructor.
    * @return CMapElementEntry*
	  */
    static CMapElementEntry* NewLC( TUint16* aMapName );
    
    /**
    * Destructor.
    */
    virtual ~CMapElementEntry();
    
    /**
    * This method sets the dom node for a given map name
    * @since 2.1
    * @param aDomNode: pointer to the dom node
    * @return 
    */
    void SetDomNode( NW_DOM_ElementNode_t* aDomNode );
    
    /**
    * This method adds an image box for a given map name
    * @since 2.1
    * @param aImageMapBox: pointer to the image box
    * @return TBrowserStatusCode
    */
    TBrowserStatusCode AddImageMapBox( NW_LMgr_ImageMapBox_t* aImageMapBox );

    /**
    * This method returns the dom node
    * @since 2.1
    * @return a pointer to a NW_DOM_ElementNode_t
    */
    NW_DOM_ElementNode_t* DomNode() { return iDomNode; }

    /**
    * This method returns the image map list
    * @since 2.1
    * @return a pointer to a CImageMapBoxList
    */
    CImageMapBoxList* ImageMapBoxList() { return iImageMapBoxList; }

       
    TUint16*              iMapName;                 // entry Id

  private: // new functions

    /**
    * Constructor.
    */
    CMapElementEntry(); 

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL( TUint16* aMapName );
  
  private: // member variables

    NW_DOM_ElementNode_t* iDomNode;                 // corresponding DOM node
    CImageMapBoxList*     iImageMapBoxList;         // image box list sharing the same map name
  };

// CLASS DECLARATION

/**
*  This class represents a maping of map names to dom nodes
*  @lib xhtml.lib
*  @since 2.1
*/
class CMapElementList : public CBase
  {
  public:  // Constructors and destructor
        
   /**
    * Two-phased constructor.
    * @return CMapElementList*
    */
    static CMapElementList* NewL();

    /**
    * Destructor.
    */
    virtual ~CMapElementList();

  public: // New functions
        
    /**
    * This method sets the dom node for a given map name
    * @since 2.1
    * @param aMapName: name of the map
    * @param aDomNode: pointer to the dom node
    * @return TBrowserStatusCode
    */
    TBrowserStatusCode SetDomNode( TUint16* aMapName, NW_DOM_ElementNode_t* aDomNode );

    /**
    * This method sets an image map box for a given map name
    * @since 2.1
    * @param aMapName: name of the map
    * @param aImageMapBox: pointer to the image map box
    * @return TBrowserStatusCode
    */
    TBrowserStatusCode SetImageMapBox( TUint16* aMapName, NW_LMgr_ImageMapBox_t* aImageMapBox );

    /**
    * This method returns the number of entries in the mapelement list
    * @since 2.1
    * @param aIndex: index
    * @return a pointer to a CMapElementEntry
    */
    TInt MapElementCount() { return iMapElementList->Count(); }

    /**
    * This method returns an map element enrty by index
    * @since 2.1
    * @param aIndex: index
    * @return a pointer to a CMapElementEntry
    */
    CMapElementEntry* MapElementEntry( TInt aIndex ) { return iMapElementList->At( aIndex ); }

    /**
    * This method returns the dom node for a given map name
    * @since 2.1
    * @param aMapName: name of the map
    * @return a pointer to a NW_DOM_ElementNode_t object or NULL
    */
    NW_DOM_ElementNode_t* DomNode( TUint16* aMapName );

    /**
    * This method returns the image map list for a given map name
    * @since 2.1
    * @param aMapName: name of the map
    * @return a pointer to a CImageMapBoxList object or NULL
    */
    CImageMapBoxList* ImageMapBoxList( TUint16* aMapName );
    
  private: // New functions

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();

    /**
    * This method sets either a dom node or an image map box for a given map name
    * @since 2.1
    * @param aMapName: name of the map
    * @param aDomNode: pointer to the dom node
    * @param aImageMapBox: pointer to the image map box
    * @return TBrowserStatusCode
    */
    TBrowserStatusCode Set( TUint16* aMapName, NW_DOM_ElementNode_t* aDomNode, NW_LMgr_ImageMapBox_t* aImageMapBox );

    /**
    * This method gets the entry for a given map name
    * @since 2.1
    * @param aMapName: name of the map
    * @return a pointer to the entry
    */
    CMapElementEntry* Get( TUint16* aMapName );

  
    typedef CArrayPtrFlat<CMapElementEntry> CMapElementListArray;

  private:    // Data

    // vector containing the map elements
    CMapElementListArray* iMapElementList;
  };

#endif /* MAPELEMENTLIST_H */
