/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class represents the Widget Preferences object
*
*/

// INCLUDE FILES
#include <e32std.h>
#include <S32FILE.H>
#include <f32file.h>
#include <uri16.h>
#include <e32hashtab.h>
#include "Preferences.h"


// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
const TInt KMaxIntLength = 10;
const TInt KMaxKeyValueSize = 4096; //4 k

_LIT( KKeyFormat, "%d.%S" );
_LIT( KPrefsFile,"prefs.dat" );
_LIT( KPrefsTmpFile,"prefs.dat.tmp" );
_LIT( KPrefsBakFile, "prefs.dat.bak" );


// ----------------------------------------------------------------------------
// WidgetPreferences::WidgetPreferences
// C++ Constructor
//
//
// ----------------------------------------------------------------------------
WidgetPreferences::WidgetPreferences() :m_widgetid(0),
                                        m_widgetbundleid(0),
                                        m_basepath(0),
                                        m_filepath(0)
{
    m_preferences = new RPtrHashMap<TDesC,PrefElement>();    
}

// ----------------------------------------------------------------------------
// WidgetPreferences::~WidgetPreferences
// Destructor
//
//
// ----------------------------------------------------------------------------
WidgetPreferences::~WidgetPreferences()
{
    //TRAP_IGNORE( saveL() );
    if (m_preferences) {
        m_preferences->ResetAndDestroy();
        m_preferences->Close();
        delete m_preferences;        
    }
    delete m_basepath;
    delete m_widgetbundleid;        
    delete m_filepath; 
}

// ----------------------------------------------------------------------------
// WidgetPreferences::setBasePathL
//
//
// ----------------------------------------------------------------------------
void WidgetPreferences::setBasePathL(const TDesC& aValue)
{
    m_basepath = aValue.AllocL();    
}

// ----------------------------------------------------------------------------
// WidgetPreferences::setWidgetId
//
//
// ----------------------------------------------------------------------------
void WidgetPreferences::setWidgetId(TInt aValue)
{
    m_widgetid = aValue;
}

// ----------------------------------------------------------------------------
// WidgetPreferences::setBundleIdL
//
//
// ----------------------------------------------------------------------------
void WidgetPreferences::setWidgetBundleId(const TDesC& aValue)
{
    m_widgetbundleid = aValue.AllocL();
}

// ----------------------------------------------------------------------------
// WidgetPreferences::setBundleIdL
//
//
// ----------------------------------------------------------------------------
TDesC& WidgetPreferences::getWidgetBundleId()
{
   return *m_widgetbundleid;
}

// ----------------------------------------------------------------------------
// WidgetPreferences::PreferenceL
// Get preference for a key
//
//
// ----------------------------------------------------------------------------
TInt WidgetPreferences::preferenceL( const TDesC& akey, HBufC*& avalue)
{

    TInt rSuccess = KErrNotFound;
    TInt size = 0;
    
    if ( !m_basepath || (m_basepath->Length() <= 0) )
        return rSuccess;

 
    if ( akey.Length() <= KMaxKeyValueSize ) {
    
        HBufC* k = HBufC::NewLC( akey.Length() + KMaxIntLength + 1 );
        k->Des().Format( KKeyFormat, m_widgetid, &akey );
        
        PrefElement* pref = m_preferences->Find( *k );
                    
        if ( !pref ) {
            CleanupStack::PopAndDestroy( k );
            return rSuccess;
        }
            
        size = pref->valueSize();            

        if ( size > KMaxKeyValueSize ) {
            // return contents from temp file whose name is stored 
            // in the m_value member of m_preferences
            RFs fs;

            if ( fs.Connect() == KErrNone ) {
                CleanupClosePushL( fs );
                HBufC* filePath = HBufC::NewLC( pref->value().Length() );
                TPtr fName( filePath->Des() );
                fName.Append( pref->value() );

                RFileReadStream readStream;

                if ( readStream.Open( fs, *filePath, EFileRead ) == KErrNone ) {
                    CleanupClosePushL( readStream );
                    TInt len( readStream.ReadInt32L() );
                    if ( len > 0 ) {                                       
                        HBufC* v = HBufC::NewLC( len );
                        TPtr ptrvalue = v->Des();
                        readStream.ReadL( ptrvalue, len );    
                        avalue = v; // ownership xfered
                        CleanupStack::Pop( v );
                        rSuccess = KErrNone;
                    }     
                    CleanupStack::PopAndDestroy();//readStream
                }   

                CleanupStack::PopAndDestroy( 2 ); //filePath,fs
            }
        }        
        else if ( size >= 0 ) {
            avalue = pref->value().AllocL();
            rSuccess = KErrNone;
        }

        CleanupStack::PopAndDestroy( k );
    }

    return rSuccess;

}

// ----------------------------------------------------------------------------
// WidgetPreferences::SetPreferenceL
// set Preference for a key
//
//
// ----------------------------------------------------------------------------
void WidgetPreferences::setPreferenceL( const TDesC& akey, const TDesC& avalue)
{
    if ( !m_basepath || (m_basepath->Length() <= 0) )
        return;

    if ( akey.Length() <= KMaxKeyValueSize ) {
        
        HBufC* k = HBufC::NewLC( akey.Length() + KMaxIntLength + 1 );
        k->Des().Format( KKeyFormat, m_widgetid, &akey );

        // if hash has the key and its value
        // delete the old value later when the new value was successfully updated
        PrefElement* prefExisting = NULL;
        prefExisting = m_preferences->Find( *k );                            

        if ( avalue.Length() <= KMaxKeyValueSize ) {
            PrefElement* pref = new (ELeave) PrefElement;   
            CleanupStack::PushL( pref );   
            pref->setValueL( avalue );
            pref->setValueSize( avalue.Length() );
            m_preferences->InsertL( k, pref );
            CleanupStack::Pop(); //pref   
        }
        else {
            // create a temp file and save the value in temp file. 
            // m_value member of PrefElement contains the temp file name.
            RFs fs;
            RFile file;
            if ( fs.Connect() == KErrNone ) {
                CleanupClosePushL( fs );

                // create and write to file
                TFileName tempFileName;
                file.Temp( fs, *m_basepath, tempFileName, EFileWrite|EFileShareExclusive );
                CleanupClosePushL( file );
                HBufC* filePath = HBufC::NewLC( tempFileName.Length() );
                TPtr fName( filePath->Des() );
                fName.Append( tempFileName );
                RFileWriteStream writeStream( file );
                CleanupClosePushL( writeStream );        
                TRAPD( err, 
                    writeStream.WriteInt32L( avalue.Length() );
                    writeStream.WriteL( avalue );             
                    writeStream.CommitL(); );
                // If an error occured while writing to the file, delete the temp file
                // This should be the case when disk is full
                if ( err != KErrNone )
                    {
                    CleanupStack::PopAndDestroy( ); //writeStream
                    file.Close();
                    fs.Delete( *filePath );
                    User::Leave( err );
                    }

                // create new preference element
                PrefElement* pref = new ( ELeave ) PrefElement;
                CleanupStack::PushL( pref );  
                pref->setValueSize( avalue.Length() );
                pref->setValueL( *filePath );               
                // update new preference element
                m_preferences->InsertL( k, pref );

                CleanupStack::Pop( pref );
                CleanupStack::PopAndDestroy( ); //writeStream
                CleanupStack::PopAndDestroy( 3 ); //filePath,file,fs
            }           
        }        

        // now the new value is updated, it's safe to delete the old value
        if ( prefExisting ) {
            prefExisting->setCleanFileFlag( ETrue );
            delete prefExisting;
        }

        CleanupStack::Pop();   // k

        // Save update to persistent storage
        TRAPD(err, saveL());
        if(err!= KErrNone)
            {
            deleteAllPrefFiles();
            }
    }

}

// ----------------------------------------------------------------------------
// WidgetPreferences::removePreferenceL
// remove Preference for a key
//
//
// ----------------------------------------------------------------------------
void WidgetPreferences::removePreferenceL( const TDesC& akey, const TDesC& avalue)
{
    if ( !m_basepath || (m_basepath->Length() <= 0) )
        return;

    // double check value is NULL
    if ( avalue != KNullDesC ) {
        return;
        }

    if ( akey.Length() <= KMaxKeyValueSize ) {
        
        HBufC* k = HBufC::NewLC( akey.Length() + KMaxIntLength + 1 );
        k->Des().Format( KKeyFormat, m_widgetid, &akey );

        // if hash has the key and its value
        // delete the old value later when the new value was successfully updated
        PrefElement* prefExisting = NULL;
        prefExisting = m_preferences->Find( *k );                            

        m_preferences->Remove(k);

        // now the new value is updated, it's safe to delete the old value
        if ( prefExisting ) {
            prefExisting->setCleanFileFlag( ETrue );
            delete prefExisting;
        }

        CleanupStack::PopAndDestroy();   // k

        // Save update to persistent storage
	TRAPD(err, saveL());
    if(err!= KErrNone) {
        deleteAllPrefFiles();
        }
    }
}

// ----------------------------------------------------------------------------
// WidgetPreferences::SaveL
// SAve preferences to persistent storage
//
//
// ----------------------------------------------------------------------------
void WidgetPreferences::saveL()
{
    if ( !m_basepath || (m_basepath->Length() <= 0) )
        return;

    RFs fs;

        
    // Connect to file server
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL( fs );
        
    // Form filenames
    HBufC* tmpFilePath = HBufC::NewLC( m_basepath->Length() + KPrefsTmpFile().Length() );
    HBufC* bakFilePath = HBufC::NewLC( m_basepath->Length() + KPrefsBakFile().Length() );
    HBufC* filePath = HBufC::NewLC( m_basepath->Length() + KPrefsFile().Length() );

    TPtr fName( tmpFilePath->Des() );
    fName.Append( *m_basepath );
    fName.Append( KPrefsTmpFile );
    
    fName.Set( bakFilePath->Des() );
    fName.Append( *m_basepath );
    fName.Append( KPrefsBakFile );
    
    fName.Set( filePath->Des() );
    fName.Append( *m_basepath );
    fName.Append( KPrefsFile );

	  RFileWriteStream writeStream;
    writeStream.PushL();
    
	// Try to create the temp file
	if(writeStream.Replace(fs, *tmpFilePath, EFileWrite|EFileShareExclusive) != KErrNone) {
		// Make sure the private path exists
		fs.CreatePrivatePath( EDriveC );
		
		// Try again to create the file
		User::LeaveIfError(writeStream.Create(fs, *tmpFilePath, EFileWrite|EFileShareExclusive));
	}

    // Try to externalize the data to the stream
    TRAPD(err, externalizeL(writeStream));
    
    // Close the stream
    CleanupStack::PopAndDestroy(); // writeStream
    	     
    // If no error at this point then we're guaranteed to have a valid file
    if(err!=KErrNone)
    	{
    	// Delete the temp file
    	fs.Delete(*tmpFilePath);
    	
    	// And leave with the error code
    	User::Leave(err);
    	}
    	
    // Backup any old valid file just in case
    fs.Delete(*bakFilePath);
    fs.Rename(*filePath, *bakFilePath);
    
    // Rename the temp file to be the actual settings file
    err = fs.Rename(*tmpFilePath, *filePath);
    if(err!=KErrNone)
    	{
    	// If we had a backup settings file, try to return it
    	fs.Rename(*bakFilePath, *filePath);
    	User::Leave(err);
    	}

	// Cleanup
	CleanupStack::PopAndDestroy(4); // filePath, bakFilePath, tmpFilePath, fs

}

// ----------------------------------------------------------------------------
// WidgetPreferences::externalizeL
//
//
// ----------------------------------------------------------------------------

void WidgetPreferences::externalizeL(RWriteStream& aStream) const
{
    aStream.WriteInt32L( m_preferences->Count() );
	
    TPtrHashMapIter<TDesC,PrefElement> it( *m_preferences );
    const TDesC* key;
    const PrefElement* pref;

    while ( ( key = it.NextKey() ) != 0 ) {
        pref = it.CurrentValue();
        aStream.WriteInt32L( key->Length() );
        aStream.WriteL( *key );
        aStream.WriteInt32L( pref->value().Length() );
        aStream.WriteL( pref->value() );
        aStream.WriteInt32L( pref->valueSize() );
    }
    aStream.CommitL();
    
}

// ----------------------------------------------------------------------------
// WidgetPreferences::internalizeL
//
//
// ----------------------------------------------------------------------------
void WidgetPreferences::internalizeL(RReadStream& aStream)
{
    TInt count( aStream.ReadInt32L() );
    for( TInt i = 0; i < count; i++ ) {
        
        TInt len = aStream.ReadInt32L();

        if ( len > 0 ) {
            HBufC* key = HBufC::NewLC( len );
            TPtr ptrkey = key->Des();
            aStream.ReadL( ptrkey, len );
            len = aStream.ReadInt32L();

            if ( len <= KMaxKeyValueSize ) {
                HBufC* value = HBufC::NewLC( len );
                TPtr ptrvalue = value->Des();
                aStream.ReadL( ptrvalue, len );
                PrefElement* pref = new ( ELeave ) PrefElement;
                CleanupStack::PushL( pref );
                pref->setValueL( ptrvalue );
                TInt size = aStream.ReadInt32L();
                pref->setValueSize( size );
                m_preferences->InsertL( key, pref );
                CleanupStack::Pop(); //pref
                CleanupStack::PopAndDestroy(); //value
                CleanupStack::Pop(); //key
            }
            else {
                CleanupStack::PopAndDestroy( key );
             }

         }

        else {
            break;
        }
     }

 }

// ----------------------------------------------------------------------------
// WidgetPreferences::deleteAllPrefFiles
//
//
// ----------------------------------------------------------------------------
void WidgetPreferences::deleteAllPrefFiles(){

   if ( !m_basepath || (m_basepath->Length() <= 0) ) 
         return;


   RFs fs;
   // Deleting bkUp and main prefs file.
   User::LeaveIfError(fs.Connect());
   CleanupClosePushL( fs );
   
   HBufC* bkFilePath = HBufC::NewLC( m_basepath->Length() + KPrefsBakFile().Length() );

   TPtr fName( bkFilePath->Des() );
   fName.Append( *m_basepath );
   fName.Append( KPrefsBakFile );
 
   fs.Delete( *bkFilePath  );
         
   CleanupStack::PopAndDestroy(); 

   HBufC* prefFilePath = HBufC::NewLC( m_basepath->Length() + KPrefsFile().Length() );

   TPtr fNamePr( prefFilePath->Des() );
   fNamePr.Append( *m_basepath );
   fNamePr.Append( KPrefsFile);
 
   fs.Delete( *prefFilePath  );
         
   CleanupStack::PopAndDestroy(2);

}

// ----------------------------------------------------------------------------
// WidgetPreferences::LoadL
// Load preferences from persistent storage
//
//
// ----------------------------------------------------------------------------
void WidgetPreferences::loadL()
{
   if ( !m_basepath || (m_basepath->Length() <= 0) ) 
         return;
    
    // Try to connect to file server
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL( fs );
    
    // Form settings file name
    HBufC* filePath = HBufC::NewLC( m_basepath->Length() + KPrefsFile().Length() );
    TPtr fName( filePath->Des() );
    fName.Append( *m_basepath );
    fName.Append( KPrefsFile );

    // Open stream
    RFileReadStream readStream;
    readStream.PushL();
    TInt err = readStream.Open( fs, *filePath, EFileRead );
    if(err!=KErrNone) {
        User::Leave(err);
    }
    
    // Read the data from the stream
    internalizeL(readStream);

    CleanupStack::PopAndDestroy(3); // readStream, filePath, fs
}
    
// ----------------------------------------------------------------------------
// PrefElement::PrefElement
// C++ constructor
//
//
// ----------------------------------------------------------------------------    
PrefElement::PrefElement() : m_value (0), m_valuesize (0), m_cleanFileFlag (EFalse)
{
}

    
// ----------------------------------------------------------------------------
// PrefElement::~PrefElement
// Destructor
//
//
// ----------------------------------------------------------------------------
PrefElement::~PrefElement()
{
    // When we update the existing key with newer value:
    // if the value > 4k, which means a file was created to store the value;
    // we need to delete the old file.
    // If called from ~Preferences(), we shouldn't do this -- cleanFileFlag is false
    if ( m_cleanFileFlag && m_valuesize > KMaxKeyValueSize ) {
        HBufC* filePathExisting = HBufC::NewLC( value().Length() );
        TPtr fName( filePathExisting->Des() );
        fName.Append( value() );

        RFs fs;
        if ( fs.Connect() == KErrNone ) {
            fs.Delete(*filePathExisting);
            fs.Close();
        }              

        CleanupStack::PopAndDestroy(); //filePathExisting
    }

    delete m_value;
}


// ----------------------------------------------------------------------------
// PrefElement::setValueL
// set value for a preference
//
//
// ----------------------------------------------------------------------------
void PrefElement::setValueL( const TDesC& value )
{
    if ( m_value ) {
        delete m_value;
    }

    m_value  = value.AllocL();
}



