/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef C_WRTCONTENTHARVESTER_H
#define C_WRTCONTENTHARVESTER_H 

// INCLUDES
#include <apgcli.h> // RApaLsSession 
#include <contentharvesterplugin.h>
#include "widgetappdefs.rh"
#include "wrtharvesterregistryaccess.h"


// FORWARD DECLARATIONS
class CWrtHarvesterPSNotifier;
class CWrtHarvesterPublisherObserver;
class MLiwInterface;
class CLiwGenericParamList;

// CONSTANTS

// CLASS DECLARATION
/**
 * 
 *
 *  @lib ??????.dll
 *  @since S60 S60 v3.1
 */

class CWrtHarvester: public CContentHarvesterPlugin
	{
	public: // Constructors and destructor
		/**
		 * Two-phased constructor.
		 */
		static CWrtHarvester* NewL( MLiwInterface* aCPSInterface );
		
		/**
		 * Destructor.
		 */
		virtual ~CWrtHarvester();

        // from base class 

		/**
    	* 
    	*/
		void UpdateL();
		
		/**
    	* 
    	*/
		void TryLaunchNextWidgetL();

        /**
		* @param aContentId
		* @param aTrigger
		*/
		void HandlePublisherNotificationL( const TDesC& aContentId, const TDesC8& aTrigger );
		
	private:
        
    	/**
    	* Perform the second phase construction 
    	*/		
		void ConstructL();
        
        /** 
		* Default constructor.
		* @param aCPSInterface
		*/
		CWrtHarvester( MLiwInterface* aCPSInterface );

		/**
    	* 
    	*/
		void UpdatePublishersL();
		
		/**
		* 
    	* @param aPublisherId
    	* @param aContentId
    	* @param aContentType
    	* @param aResultType
    	* @return TInt
    	*/
		TInt RegisterPublisherL( const TDesC& aPublisherId, const TDesC& aContentId,
		        const TDesC& aContentType, const TDesC8& aResultType );
		
		/**
    	* 
    	* @param aPublisherId
    	* @param aContentId
    	* @param aContentType
    	*/
		void RemovePublisherL( const TDesC& aPublisherId, const TDesC& aContentId,
		        const TDesC& aContentType );
		
		/**
		* Starts WRT widget
    	* @param aUid Uid of the widget
    	*/
		void StartWidgetL( TUid aUid );
		
		/**
    	* 
    	* @param aPublisherId
    	* @param aContentId
    	* @param aContentType
    	*/
		void RequestForNotificationL( const TDesC& aPublisherId, const TDesC& aContentId,
		        const TDesC& aContentType );
		
		/**
    	* 
    	* @param aInParamList
    	* @return TInt
    	*/
		TInt ExtractItemId( const CLiwGenericParamList& aInParamList );
		
		
		/**
		 * 
		 */
		void LaunchWidgetL( TWidgetOperations aOperation, TUid aUid );


	private: // data
       	
       	/**
        * Instance of CPS interface used for update with CPS.
        */
        MLiwInterface* iCPSInterface;
        
       	/**
       	 * Publish & Subscribe listener
       	 * own
       	 */
        CWrtHarvesterPSNotifier* iWidgetUIListener;

        /**
         * Publish & Subscribe listener
         * own
         */
        CWrtHarvesterPSNotifier* iWidgetRegListener;
        
        /**
         * Publish & Subscribe listener
         * own
         */
        CWrtHarvesterPSNotifier* iWidgetMMCListener;
        
		/**
    	* 
    	*/
        RWrtArray< CWrtHarvesterPublisherObserver > iObservers;

		/**
    	* 
    	*/
        WrtHarvesterRegistryAccess iRegistryAccess;
        
		/**
    	* 
    	*/
        RArray<TUid> iWidgetUids;
        
        /**
         * 
         */
        RApaLsSession iApaSession;
    };

#endif // C_WRTCONTENTHARVESTER_H 
