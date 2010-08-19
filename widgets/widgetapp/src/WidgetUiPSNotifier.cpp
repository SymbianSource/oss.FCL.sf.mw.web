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
 
#include "WidgetUiPSNotifier.h"
#include "WidgetUiWindowManager.h"
#include <widgetregistryclient.h>
#include "WidgetUiWindow.h"
#include "WidgetAppDefs.rh"
#include <e32debug.h>
CWidgetUiPSNotifier::CWidgetUiPSNotifier(CWidgetUiWindowManager& aWindowManager, TPropertyKeyValues aKey) :iWindowManager(aWindowManager), iKey(aKey),  CActive( EPriorityNormal )
    {
    }
CWidgetUiPSNotifier::~CWidgetUiPSNotifier()
    {
    Cancel();
    iProperty.Close();
    }

CWidgetUiPSNotifier* CWidgetUiPSNotifier::NewL(CWidgetUiWindowManager& aWindowManager, TPropertyKeyValues aKey )
    { 
    CWidgetUiPSNotifier* self = new (ELeave) CWidgetUiPSNotifier(aWindowManager, aKey);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );    
    return self;
    }

void CWidgetUiPSNotifier::ConstructL()
    { 
    CActiveScheduler::Add(this);    
    TInt r(KErrNone);
    
    if ( iKey != EWidgetRegAltered)
        {
        r = RProperty::Define( KPropertyWidgetUI, iKey, RProperty::EInt );
        if ( r != KErrAlreadyExists && r != KErrNone )
            {
            User::LeaveIfError( r );
            }
        User::LeaveIfError( iProperty.Attach( KPropertyWidgetUI,iKey));         
        }
      // Attach the key
    
    if ( iKey == EWidgetRegAltered )
        User::LeaveIfError( iProperty.Attach( KMyPropertyCat,iKey));         
    // initial subscription
    iProperty.Subscribe( iStatus );
    SetActive();
    }


// ---------------------------------------------------------------------------
// CWrtHarvesterPSNotifier::DoCancel
// ---------------------------------------------------------------------------
//
void CWidgetUiPSNotifier::DoCancel()
    {
    iProperty.Cancel();
    }

// ---------------------------------------------------------------------------
// CWidgetUiPSNotifier::RunL()
// ---------------------------------------------------------------------------
//
void CWidgetUiPSNotifier::RunL()
    {
    
    // resubscribe before processing new value to prevent missing updates
    iProperty.Subscribe( iStatus );
    SetActive();
    
    if (iKey == EWidgetRegAltered)
        {
        UpdateWindowList();
        }
    else if( iKey == ESapiPrompt || iKey == ESapiPromptCleared)
        {
        UpdateSapiPromptState();
        }

    }
// ---------------------------------------------------------------------------
// CWrtHarvesterPSNotifier::UpdateSapiPromptState
// ---------------------------------------------------------------------------
//
void CWidgetUiPSNotifier::UpdateSapiPromptState()
    {
    TInt value( 0 );
    iProperty.Get( value  );

    if( value != 0)
        {
        RPointerArray<CWidgetUiWindow>      windowList;
        iWindowManager.WindowList( windowList );
        for(TInt i=windowList.Count()-1;i>=0;i--)
            {
            if( value == windowList[i]->Uid().iUid)
                {
                if(iKey == ESapiPrompt )
                    {
                	  if( !windowList[i]->getSapiPromptCleared() )
                	  	   windowList[i]->setNeedToIgnoreSapiClearNtfn(EIncrement);
                	  else if( windowList[i]->getNeedToIgnoreSapiNtfn() == 0)
                        windowList[i]->setSapiPromptCleared(EFalse);
                    else if(windowList[i]->getNeedToIgnoreSapiNtfn() > 0 )
                        	  windowList[i]->setNeedToIgnoreSapiNtfn(EDecrement);

                    }
                else if(iKey == ESapiPromptCleared )
                    {
                	  if( windowList[i]->getSapiPromptCleared() )
                	      windowList[i]->setNeedToIgnoreSapiNtfn(EIncrement);
                	  else if ( windowList[i]->getNeedToIgnoreSapiClearNtfn() == 0)
                        windowList[i]->setSapiPromptCleared(ETrue);
                    else if(windowList[i]->getNeedToIgnoreSapiClearNtfn() > 0)
                  	    windowList[i]->setNeedToIgnoreSapiClearNtfn(EDecrement);
                    }
                break;
                }
            }
        }	
    
    }
// ---------------------------------------------------------------------------
// CWrtHarvesterPSNotifier::UpdateWindowList
// ---------------------------------------------------------------------------
//
void CWidgetUiPSNotifier::UpdateWindowList()
    {
    RPointerArray<CWidgetUiWindow>      iWindowList;
    RWidgetRegistryClientSession        iClientSession;
    RPointerArray<CWidgetInfo>          iWidgetInfoArray;
        
    iWindowManager.WindowList( iWindowList );
    User::LeaveIfError( iClientSession.Connect());
    iClientSession.InstalledWidgetsL(iWidgetInfoArray); 
    TInt j=0;
    for(TInt i=iWindowList.Count()-1;i>=0;i--){
        for(j=0;j<iWidgetInfoArray.Count();j++) {
            if( iWindowList[i]->Uid().iUid == iWidgetInfoArray[j]->iUid.iUid )
                {
                break;
                }
            }
            //present.... so break, go to the next element in iWindowList array
            if (j == iWidgetInfoArray.Count())
                iWindowManager.HandleWidgetCommandL(iWindowList[i]->Uid(), Deactivate);
                                               
        }
    }
    
