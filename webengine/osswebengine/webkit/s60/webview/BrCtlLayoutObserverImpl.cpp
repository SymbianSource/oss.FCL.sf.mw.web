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
* Description:  Handle scrollbar and tab events
*
*/


//  INCLUDES

#include <../bidi.h>
#include "BrCtlLayoutObserverImpl.h"
#include "WebView.h"
#include "WebFrame.h"
#include "WebFrameView.h"
#include "BrCtl.h"

#include "Page.h"
#include "FocusController.h"
#include "Frame.h"
#include <wtf/RefPtr.h>

using namespace WebCore;

// -----------------------------------------------------------------------------
// CBrCtlLayoutObserver::CBrCtlLayoutObserver
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CBrCtlLayoutObserver::CBrCtlLayoutObserver( WebView* aWebView)
                  : m_webView(aWebView)
    {

    }

// -----------------------------------------------------------------------------
// CBrCtlLayoutObserver::NotifyLayoutChange
// Inform the layout of the page: right to left or left to right. 
// Useful when the application draws the scrollbar itself.
// -----------------------------------------------------------------------------
//                    
void CBrCtlLayoutObserver::NotifyLayoutChange( TBrCtlLayout aNewLayout ) 
    {
    if(!m_webView)
    return;

    TSize contentSize = m_webView->mainFrame()->frameView()->contentSize();

    switch(aNewLayout)
      {
      case EOriginTopRight:
           m_webView->mainFrame()->frameView()->scrollTo(TPoint(contentSize.iWidth,0));
           break;

     case EOriginTopLeft:
          {
          TPoint ptInit(0,0);
          TPoint ptFromHistory = m_webView->brCtl()->historyHandler()->historyController()->currentEntryPosition();
          TPoint ptCurr = m_webView->mainFrame()->frameView()->contentPos();

          if ( ptCurr != ptFromHistory ) {
              if ( ptInit == ptCurr ) {
                  m_webView->mainFrame()->frameView()->scrollTo(ptFromHistory);
                  }
             else{
                  m_webView->mainFrame()->frameView()->scrollTo(TPoint(0,0));
                 }
            }
          }
          break;

      default:
          break;
      }
    }
