/*
* Copyright (c) 2006, 2008 Nokia Corporation and/or its subsidiary(-ies).
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
*     Lauches/inform widget stub ui to display widget
*
*
*/


// INCLUDE FILES

#include "widgetappdefs.rh"
#include <APACMDLN.H>
#include <s32mem.h>
#include <e32std.h>
#include <e32base.h>
#include <f32file.h>
#include <apgcli.h>
#include <W32STD.H>
#include <APGTASK.H>

// CONSTANTS

// LOCAL FUNCTION DEFINITIONS
// RDesWriteStream
/** 
* Parse command line in order to launch the widget.
*
* The function checks the command line arguments and parses 
* relevant information from it. Parameters are all out-params.
* @param aUid UID of the widget.
* @param aOperation Operation to perform.
*/
static void ParseCmdLineL( TUid& aUid, TUint32& aOperation );

/** 
* Launch or bring foreground the asked widget.
*
* Launch widget.
* @param aUid UID of the widget.
* @param aOperation Operation to perform.
*/
static void LaunchWidgetL( const TUid& aUid, TUint32 aOperation );

/** 
* Launch new widget.
*
* Launch new widget.
* @param aUid UID of the widget.
* @param aOperation Operation to perform.
*/
static void LaunchWidgetUIL( 
    const TUid& aUid, 
    const TDesC8& aMessage, 
    TUint32 aOperation );

//===========================================================================
// Start the stuff.
//===========================================================================
void StartTheApplicationL()
    {
    TUid uid( TUid::Null() );
    TUint32 operation( 0 );
    
    ParseCmdLineL( uid, operation );
    LaunchWidgetL( uid, operation );
    }
    
//===========================================================================
// Parse command line.
// The function checks the command line arguments to launch the widget.
// 
// Too bad that we can't have version number information in the beginning 
// of the command line. Now we have to try/catch the miniview size.

// The format is originally:
// <UID.iUid> <url lenght> <url>
// 
// For miniview, we append miniview size as in 
// <TUint32>, meaning
//      - 0 - launch to full screen
//      - 1 - launch to mini view
//      - 2 - deactivate    
//
//===========================================================================
void ParseCmdLineL( TUid& aUid, TUint32& aOperation )
    {
    CApaCommandLine* commandLine( NULL );
    
    // KApaMaxCommandLine says this can be only 0x100, but we can't break the 
    // data format.
    TUint32 mainHTMLLen( 0 ); 
    HBufC* mainHTML( NULL );
    TPtr mainHTMLPtr( NULL, 0, 0 );
    TPtrC8 data( NULL, 0 );
    
    User::LeaveIfError( CApaCommandLine::GetCommandLineFromProcessEnvironment( commandLine) );
    
    data.Set( commandLine->OpaqueData() );
    RDesReadStream readStream( data );
    
    // Read widget uid from opaque data
    aUid.iUid = readStream.ReadUint32L();
    
    // Read widget main html path from opaque data
    mainHTMLLen = readStream.ReadUint32L();
    
    if ( !mainHTMLLen )
        {
        User::Leave( KErrArgument );
        }
            
    mainHTML = HBufC::NewLC(mainHTMLLen);

    mainHTMLPtr.Set( mainHTML->Des() );
    readStream.ReadL(mainHTMLPtr, mainHTMLLen);
    
    // Dump the URL, we don't need it anymore.    
    CleanupStack::PopAndDestroy( mainHTML );

    // Try to get the size.
    TRAP_IGNORE( aOperation = readStream.ReadInt32L() );
    }
    
//===========================================================================
// Launch widget.
//===========================================================================
//
void LaunchWidgetL( const TUid& aUid, TUint32 aOperation )
    {
    __UHEAP_MARK;
    
    TUid widgetAppUid( TUid::Uid( KWidgetAppUid ) );
    
    RWsSession wsSession;
    TApaTaskList taskList( wsSession );
    HBufC8* message( HBufC8::NewLC( KWidgetUiMaxMessageLength ) );
    TPtr8 des( message->Des() );
    RDesWriteStream stream( des );
    
    CleanupClosePushL( stream );
    
    // Make the message to be sent.
    stream.WriteUint32L( 1 );
    stream.WriteUint32L( aUid.iUid );
    stream.WriteInt32L( aOperation );
        
    CleanupStack::PopAndDestroy( &stream );
    
    // Create Window server session
    User::LeaveIfError( wsSession.Connect() );
    CleanupClosePushL( wsSession );

    // Get the task list
    // Try to find out if stub ui is already running
    TApaTask task = taskList.FindApp( widgetAppUid );

    if ( task.Exists() )
        {
        // TODO make something here, or not...
        widgetAppUid = TUid::Uid( 1 );
        if ( aOperation == LaunchFullscreen ||
             aOperation == WidgetSelect )
            {
            task.BringToForeground();
            }
        task.SendMessage( widgetAppUid, des );
        }
    else
        {
        // TODO CONST
        if ( aOperation == LaunchFullscreen ||
             aOperation == LaunchMiniview ||
             aOperation == WidgetSelect ) //WidgetUI has died -> re-launch
            {
            LaunchWidgetUIL( widgetAppUid, *message, aOperation );
            }
        }
        
    CleanupStack::PopAndDestroy( 2, message );
    
    __UHEAP_MARKEND;
    }

//===========================================================================
// Launch Widget UI.
//===========================================================================
void LaunchWidgetUIL( 
    const TUid& aUid, 
    const TDesC8& aMessage, 
    TUint32 aOperation )
    {
    HBufC* document( NULL );
    CApaCommandLine* line( CApaCommandLine::NewLC() );
    TApaAppInfo info;
    RApaLsSession session;
    
    User::LeaveIfError( session.Connect() );
    CleanupClosePushL( session );
    
    User::LeaveIfError( session.GetAppInfo( info, aUid ) );
        
    document = HBufC::NewMaxLC( TReal( TReal( aMessage.Length() )  / 2.0 ) + 0.5 );

    Mem::Copy( 
        reinterpret_cast< TUint8* >( const_cast< TUint16* >( document->Ptr() ) ),
        aMessage.Ptr(),
        KWidgetUiMaxMessageLength );
        
    line->SetDocumentNameL( *document );
    line->SetExecutableNameL( info.iFullName );
        
    // TODO make const definitions.
    if ( aOperation == 1 )
        {
        line->SetCommandL( EApaCommandBackground );
        }
        
    session.StartApp( *line );

    CleanupStack::PopAndDestroy( 3, line );
    }

/*
* E32Main: called by operating system to start the program
*/
TInt E32Main(void)
    {
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
    if (cleanupStack == NULL)
        {
        return -1;
        }
    TRAP_IGNORE(StartTheApplicationL());

    delete cleanupStack;
    return KErrNone;
    }





