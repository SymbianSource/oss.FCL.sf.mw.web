/*
* Copyright (c) 2008, 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "WidgetUiNetworkListener.h"

CWidgetUiNetworkListener* CWidgetUiNetworkListener::NewL(CWidgetUiWindowManager& aWindowManager)
    {
    CWidgetUiNetworkListener* self = new (ELeave) CWidgetUiNetworkListener(aWindowManager);
    return self;
    }

CWidgetUiNetworkListener::CWidgetUiNetworkListener(CWidgetUiWindowManager& aWindowManager)
        : CActive(0), iWindowManager(aWindowManager)
    {
    CActiveScheduler::Add(this);
    StartListening();
    }

CWidgetUiNetworkListener::~CWidgetUiNetworkListener()
    {
    Cancel();
    Close();
    }

TInt CWidgetUiNetworkListener::StartListening()
    {
    TInt err = KErrNone;
    // If already listening do nothing
    if(!IsActive())
        {
        if((err = Connect()) == KErrNone)
            {
            // check if there is a current active connection
            if ( CheckActiveNetworkConnection() )
                {
                // notify widgets of an existing active network connection
                iWindowManager.NotifyConnecionChange( ETrue );
                }
            iConn.AllInterfaceNotification( iNote, iStatus );
            SetActive();
            }
        }
    return err;
    }

TBool CWidgetUiNetworkListener::CheckActiveNetworkConnection()
    {
    TUint iNumConnections, netConnections;
    TPckgBuf<TConnectionInfoV2> connectionInfo;
    
    iConn.EnumerateConnections(iNumConnections);
    netConnections  = iNumConnections;
    for(TUint i = 1; i <= iNumConnections; i++)
        {
        iConn.GetConnectionInfo(i, connectionInfo);
        // dont count bluetooth connection
        if(connectionInfo().ConnectionType() >= EConnectionBTPAN)
            {
            netConnections--;
            }
        }
    
    if (netConnections > 0)
        {
        return ETrue;
        }
    
    return EFalse;
    }

void CWidgetUiNetworkListener::RunL()
    {
    if ( iNote().iState == EInterfaceUp )
        { 
        // notify widgets of an active network connection
        iWindowManager.NotifyConnecionChange( ETrue );
        }
    else if ( iNote().iState == EInterfaceDown )
        {
        // check if there are no other active connections
        if ( !CheckActiveNetworkConnection() )
            {
            // notify widgets of a network connection going down
            iWindowManager.NotifyConnecionChange( EFalse );
            }
        }
         
    iConn.AllInterfaceNotification( iNote, iStatus );
    SetActive();
    }

void CWidgetUiNetworkListener::DoCancel()
    {
    iConn.CancelAllInterfaceNotification();
    Close();
    }

TInt CWidgetUiNetworkListener::Connect()
    {
    TInt err;
    
    if( ( err = iSocketServer.Connect() ) == KErrNone )
        {
        if( ( err = iConn.Open(iSocketServer, KAfInet) ) != KErrNone )
            {
            //failed, close server too
            iSocketServer.Close();
            }
        }
    
    return err;
    }

void CWidgetUiNetworkListener::Close()
    {
    iConn.Close();               // destructor does these
    iSocketServer.Close();
    }
