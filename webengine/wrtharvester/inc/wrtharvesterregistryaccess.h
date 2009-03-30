/*
* Copyright (c) 2007, 2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef WRHARVESTERREGISTRYACCESS_H
#define WRHARVESTERREGISTRYACCESS_H 

// INCLUDES
#include <WidgetRegistryClient.h>
#include <badesca.h>

// FORWARD DECLARATIONS
class CWidgetInfo;

// CONSTANTS
template < class T > class RWrtArray : public RPointerArray< T >
    {
    public:
        inline RWrtArray() : RPointerArray< T >()
            {
            };
        inline void PushL()
            {
            TCleanupItem item( DoCleanup, this );
            CleanupStack::PushL( item );
            }
            
        inline void ResetAll()
            {
            for ( TInt i( this->Count() - 1 ); i >= 0; --i )
                {
                delete (*this)[ i ];
                }
            this->Close();
            }
        
    private:
        static void DoCleanup( TAny* aPtr )
            {
            __ASSERT_DEBUG( aPtr, User::Invariant() );
            RWrtArray< T >* ptr( reinterpret_cast< RWrtArray< T >* >( aPtr ) );
            for ( TInt i( ptr->Count() - 1 ); i >= 0; --i )
                {
                delete ( *ptr )[ i ];
                }
            ptr->Close();
            }
    };
    
// CLASS DECLARATION

/**
 *  Widget Register accessor.
 *
 *  Handles communication & widget bookkeeping of miniview-capable widgets.
 *
 *  @lib wrtharvester.dll
 *  @since S60 S60 v5.0
 */
class WrtHarvesterRegistryAccess
	{
	public:
        /**
        * Default constructor.
        */
        WrtHarvesterRegistryAccess();
        
        /**
        * Destructor.
        */
        ~WrtHarvesterRegistryAccess();
        
        /**
	    * Get widget bundle names for widgets supporting miniviews.
	    *
	    * NOTE: Ownership of pointers in the array is not transferred!
	    * Caller must not delete them.
	    * 
	    * @param aArray Array where the descriptor pointers are to be stored.
	    */
	    void WidgetBundleNamesL( RPointerArray< HBufC >& aArray );
	    
        /**
	    * Get widget uid.
	    *
	    * @param aBundleName Name of the widget
	    * @return TUid of the widget.
	    */
	    TUid WidgetUid( TPtrC aBundleName );
	    
	private:
        /**
        * Check if the widget support miniview.
        * 
        * @param aSession Widget registry session
        * @param aUid UID of widget.
        * @return Yes or no.
        */
        TBool SupportsMiniviewL( RWidgetRegistryClientSession& aSession, const TUid& aUid );
        
        /**
        * Returns the Bundle identifier for the given widget. Ownership transfered.
        * 
        * @param aSession Widget registry session
        * @param aUid UID of widget.
        * @return Identifier in a descriptor.
        */
        HBufC* ConstructWidgetNameL( RWidgetRegistryClientSession& aSession, CWidgetInfo& aInfo );
        
	private: // data
	    /**
	    *
	    */
	    RWrtArray< CWidgetInfo >     iWidgetInfoArray;
    };

#endif // WRHARVESTERREGISTRYACCESS_H 
