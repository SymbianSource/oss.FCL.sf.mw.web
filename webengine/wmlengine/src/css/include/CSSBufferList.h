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
* Description:  Hold different CSS stylesheets that are applied to document
*
*/


#ifndef CCSSBufferList_H
#define CCSSBufferList_H

//  INCLUDES
#include "CSSParser.h"
#include "nwx_osu_file.h"
#include "BrsrStatusCodes.h"

#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  buffer list entry represents a style sheet
*  @lib css.lib
*  @since 2.1
*/
class CCSSBufferEntry : public CBase
{
public:
  // constructor
	CCSSBufferEntry(TText8* aData, TUint32 aLength, TUint32 aEncoding, 
                     TBool aInternalStyle) 
    {  
    iData = aData;
    iLength = aLength;
    iEncoding = aEncoding;
    iInternalStyle = aInternalStyle;
    iPriority = 0;
    iUrl = NULL;
    }
  // destructor
	virtual ~CCSSBufferEntry()
	  {
    if ((!iInternalStyle) && (iData != NULL))
      {
      NW_Mem_Free(iData);
      }
    NW_Mem_Free(iUrl);
    }
  static CCSSBufferEntry* NewL(TText8* aData, TUint32 aLength, TUint32 aEncoding, 
                     TBool aInternalStyle, const TText16* aUrl);
  // buffer data
  TText8* iData;
  // buffer data length
  TUint32 iLength;
  // buffer data encoding
  TUint32 iEncoding;
  // indicates if it is external Stylesheet
  TBool iInternalStyle;
  // stylesheet priority
  TInt iPriority;
  // url of stylesheet
  TText16* iUrl;
};

// CLASS DECLARATION

/**
*  This class Hold different CSS stylesheets that are applied to document
*  @lib css.lib
*  @since 2.1
*/
class CCSSBufferList : public CBase
{
  public:  // Constructors and destructor
        
   /**
    * Two-phased constructor.
    * @return CCSSHandler*
	  */
    static CCSSBufferList* NewL();

    /**
    * Destructor.
    */
    virtual ~CCSSBufferList();

  public: // New functions
        
    /**
    * This method adds a CSS buffer to the list
    * @since 2.1
    * @param aBuffer: CSS data
    * @param aLength: length of CSS data
    * @param aEncoding: encoding of CSS data
    * @param aInternalStyle: if it is associated with <style> or <link> 
    * @param aUrl: url of the buffer
    * @return void
    */
    void AddL(TText8* aBuffer, TUint32 aLength, TUint32 aEncoding, 
              TBool aInternalStyle,  TText16* aUrl);

    /**
    * This method initializes the parser with the data associated with indexed buffer
    * @since 2.1
    * @param aParser: parser
    * @param aIndex: index of CSS data
    * @return void
    */
    void InitializeParser(TCSSParser* aParser, TInt aIndex);

    /**
    * This method is Invoked when a load respose comes back for a CSS
    * @since 2.1
    * @param aData: CSS data
    * @param aLength: length of CSS data
    * @param aEncoding: encoding of CSS data
    * @param aIndex: index of CSS data to be updated
    * @return void
    */
    void UpdateEntry(TText8* aData, TUint32 aLength, TUint32 aEncoding, TInt aIndex);

    /**
    * This method is Invoked after adding a buffer for an @import request
    * @since 2.1
    * @param aRefIndex: index of CSS data to be updated
    * @return void
    */
    void UpdatePriority(TInt aRefIndex);

    /**
    * This method is Invoked after adding a buffer for an @import request
    * @since 2.1
    * @param aIndex: index of CSS data 
    * @return priority of the CSS in aIndex
    */
    TInt GetPriority(TInt aIndex);

    /**
    * This method returns encoding of the CSS in aIndex
    * @since 2.1
    * @param aIndex: index of CSS data 
    * @return encoding of the CSS in aIndex
    */
    TUint32 GetEncoding(TInt aIndex);

    /**
    * This method Returns number of CSS objects
    * @since 2.1
    * @return number of CSS stylesheets
    */
    TInt GetSize();

    /**
    * This method is Returns number of externally loaded CSS objects
    * @since 2.1
    * @return number of externally loaded CSS stylesheets
    */
    TInt GetExternalSize();

    /**
    * This method Write CSS components to a file in the form of multipart segments.
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
    * Return the number of entries in the buffer
    */
    TInt Count() {return iBufferList->Count();}

    /**
    *
    */
    NW_Ucs2* EntryUrl(TInt aIndex)
        {
        if (aIndex >= 0 && aIndex < Count())
            {
            CCSSBufferEntry* entry = iBufferList->At(aIndex);
            return entry->iUrl;
            }
        return NULL;
        }
  private: // New functions

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();

  typedef CArrayPtrFlat<CCSSBufferEntry>	CBufferListArray;

  private:    // Data

    // vector containing the buffer list
    CBufferListArray* iBufferList;
};

#endif /* CCSSBufferList_H */
