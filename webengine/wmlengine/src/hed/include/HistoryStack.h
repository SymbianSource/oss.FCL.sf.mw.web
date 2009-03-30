/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Stack of HistoryEntry objects containing info about previously 
*       viewed pages.  Allows for the navigation (forward and back) of 
*       those pages and for the maintenance (add/remove) of the pages
*
*/


#ifndef HISTORYSTACK_H
#define HISTORYSTACK_H

// INCLUDES
#include <e32base.h>
//#include "historyentry.h"
#include "HistoryInterface.h"
#include "HistoryController.h"

// CONSTANTS

/*
   static const TUint KHistoryStackSize = 20;

// MACROS

// DATA TYPES

enum THistoryStackDirection
{

    EHistoryStackDirectionPrevious,
    EHistoryStackDirectionCurrent,
    EHistoryStackDirectionNext
};


enum THistoryStackLocation
{

    EHistoryStackLocationAtBeginning,
    EHistoryStackLocationInMiddle,
    EHistoryStackLocationAtEnd
};
*/

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CHistoryMemCollector;
class CUrlRequestInfo;

// CLASS DECLARATION

/**
* CHistoryStack
* 
*  @lib ?library
*  @since ?Series60_version
*/
class CHistoryStack : public CBase
    {

    public:

        /**
        * Two-phased constructor.
        * @return CHistoryStack *
        */
        static CHistoryStack* NewL(TInt aHistoryStackStackSize);

        /**
        * Two-phased constructor.
        * @return CHistoryStack *
        */
        static CHistoryStack* NewLC(TInt aHistoryStackStackSize);

        /**
        * Sets the new current page in the history relative to
        * the current page.
        * Leave if index out of bounds.
        */
        void SetCurrentL (THistoryStackDirection aDirection);

        /**
        * Sets the new current page in the history to
        * the index.
        * Leave if index out of bounds.
        */
        void SetCurrentByIndexL (TInt aIndex);

        /**
        * Return the entry in the direction relative to the current page
        * @return CHistoryEntry const*
        */
//        CHistoryEntry* Entry (THistoryStackDirection aDirection) ;


        /**
        * Delete entire history
        */
        void DeleteAll ();

        /**
        * Returns the number of objects in the history list
        * @return TInt
        */
//        TInt Length () const {return iHistoryList->Count();}

        /**
        * Retreive an entry by it's position (index) in the list
        * @return CHistoryEntry const*
        */
//        CHistoryEntry* EntryByIndex (TInt aHistoryIndex);

        /**
        * Determine position in stack
        * @return THistoryStackLocation 
        */
        THistoryStackLocation HistoryStackLocation () const;

		// return current index
		TInt CurrentIndex () const { return iCurrent; }
		
		// sets the current index
		void SetCurrentIndex(TInt aHistoryIndex) {iCurrent = aHistoryIndex;}

        /**
		* Insert
		* Add Url to History List
		* @since 3.x
        * @return void
		*/
		void Insert( const TDesC& aUrl, const CUrlRequestInfo* aRequestInfo);

        /**
		* Insert
		* Add Url to History List
		* @since 3.x
        * @return void
		*/
//        void InsertL( const TDesC& aUrl, const TDesC& aRequestUrl, TBool aIsPost, TDesC8& aFormContentType, CPostDataParts* aPostData );

        /**
        * Implements virtual TBool ContainsItemForURL(const TDesC& aUrl) = 0;)
        * @return TBool
        * Return TRUE if URL request is in the history list
        */
        TBool containsItemForURL (const TDesC& aUrl);
        
        /**
        * Delete the entry(s) from the list
        */
        void DeleteEntry (TInt aIndex, TInt aCount=1);
        
        /**
        * Returns the stack size
        */
        TInt Size(){ return iHistoryStackStackSize; }
        
        /**
        * Retreive the index of the entry relative to the current page
        * @return TInt
        */
        TInt Index (THistoryStackDirection aDirection);

	public:
	
	    void ClearThumbnails();

		TBool IsOOMCollecting();

        /**
        * Destructor
        */
        ~CHistoryStack();

    private:


        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(TInt aHistoryStackStackSize);

        /**
        * Insert entry into stack at the current position and 
        * delete all entries above the current entry.
        * Leave if out of memory.
        */
//        void PushEntryL (CHistoryEntry* aHistoryEntry);

        /**
        * Insert entry either at the beginning of the list
        * or at the end of the list
        * Leave if out of memory.
        */
//        void InsertEntryL (CHistoryEntry* aHistoryEntry, THistoryStackLocation aLocation);

		/**
		*  Default constructor
		*/
		CHistoryStack();

        /*  Private Members  */

    private :

		/**
		* List of pointers to History Entries
		* Pointers are owned by this class
		*/
//        CArrayPtrFlat<CHistoryEntry>* iHistoryList;

        TInt iCurrent;

		TInt iHistoryStackStackSize;

        CHistoryMemCollector* iMemCollector;

    };
#endif  // HISTORYSTACK_H

// End of File
