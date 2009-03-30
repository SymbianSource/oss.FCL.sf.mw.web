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

#include "config.h"
#include "OOMHandler.h"
/*
 * Copyright (C) 2007 Nokia, Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer. 
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution. 
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <config.h>
#include "BrCtl.h"
#include "WebView.h"
#include "WebFrame.h"
#include "Cache.h"
#include "OOMHandler.h"
#include "StaticObjectsContainer.h"

namespace WebCore {

OOMHandler::OOMHandler()
{
    MemoryManager::AddCollector(this);
    MemoryManager::AddStopper(this);
}

OOMHandler::~OOMHandler()
{
    MemoryManager::RemoveCollector(this);
    MemoryManager::AddStopper(this);
}

TUint OOMHandler::Collect(TUint aRequired)
{
    // clear all cached images.
    cache()->clearImages();
    return 0;
}

void OOMHandler::Restore()
{
    // tot fixme: do nothing so far.
}

void OOMHandler::Stop()
{
    // the first step is to stop all the active loadings
    const Vector<CBrCtl*>& ctrls = StaticObjectsContainer::instance()->activeBrowserControls();
    for (int i=0; i<ctrls.size(); ++i) {
        ctrls[i]->webView()->mainFrame()->stopLoading();
    }
}

}
