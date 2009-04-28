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

#ifndef __UNKNOWNCONTENTHANDLER_H__
#define __UNKNOWNCONTENTHANDLER_H__

#include <e32base.h>
#include <http/rhttptransaction.h>
#include "HttpConnection.h"
#include "ResourceResponse.h"

class UnknownContentHandler
{
public:
    /**
    */
    virtual ~UnknownContentHandler();

    static UnknownContentHandler* NewL(HttpConnection* connection, WebCore::ResourceResponse& response);

    static bool isUnknownContent(WebCore::ResourceResponse& response);
    
    void updateContentTypeL(TPtrC8& chunkPtr);

    WebCore::ResourceResponse* handOffResponse();

private:
    /**
    */
    UnknownContentHandler(HttpConnection* connection);
    void ConstructL(WebCore::ResourceResponse& response);

private: // data
    HttpConnection* m_connection;
    WebCore::ResourceResponse* m_response;

};


#endif // __UNKNOWNCONTENTHANDLER_H__
// end of file