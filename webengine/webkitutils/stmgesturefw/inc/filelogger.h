/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  API for FileLogger.dll
*
*/
// This file defines the API for FileLogger.dll
#ifndef __FILELOGGER_H__
#define __FILELOGGER_H__

#include <f32file.h>
const TInt KMaxLogEntrySize = 400;

#ifdef GESTURE_LOGGING
#if defined(__WINS__) && !defined(__SERIES60_30__)
#define RDEBUG(args...) RDebug::Printf(args)
#define RDEBUG8(args) RDebug::RawPrint(args)
#define RDEBUG16(args...) RDebug::Print(args...)
#else
#define RDEBUG(args...)
#define RDEBUG8(args)
#define RDEBUG16(args...)
#endif

#define LOGFLUSH            CFileLogger::Flush()
#define LOGCLOSE            CFileLogger::Close()
#define LOGARG(txt,parm...) {_LIT8(KTxt, txt); CFileLogger::Write(KTxt, parm); RDEBUG(txt, parm);}
#define LOGTXT(txt) {_LIT8(KTxt, txt); CFileLogger::Write((const TDesC8&)KTxt); RDEBUG8(KTxt);}
#define LOGDES8(des)        {CFileLogger::Write(des); RDEBUG8(des);}
#define LOGDES16(des)       {CFileLogger::Write(des); RDEBUG16(des);}
#define LOGERR(txt,err)     if (!err) {} else {LOGARG(txt, err);}
#define LOGCALL(exp) {LOGARG("Calling \"%s\"", #exp); exp; LOGARG("Call to \"%s\" passed.", #exp);}
#define LOGENTER            LOGARG("%s start", __PRETTY_FUNCTION__)
#define LOGEXIT             LOGARG("%s end", __PRETTY_FUNCTION__)
#define LOGMEM(ptr)         LOGARG("%s [0x%x]", #ptr, (TUint)ptr)

NONSHARABLE_CLASS( CFileLogger ): public CBase
{
public:
    static void Write(const TDesC8& aText);
    static void Write(const TDesC16& aText);
    static void Write(TRefByValue<const TDesC8> aFmt,...);
    static void Close();
    static void Flush();

private:
    CFileLogger();
    ~CFileLogger();
    TBool Construct();
    static CFileLogger* Logger();
    void DoWrite();
    TBool GetLogFileNameWithoutExt(TDes& aFileName);
    void DoFlush() ;

private:
    RFs                     iFs;
    RFile                   iFile;
    TBuf8<KMaxLogEntrySize> iLogBuffer;
};

#else 
#define LOGFLUSH     
#define LOGCLOSE     
#define LOGARG(txt,parm...) 
#define LOGTXT(txt) 
#define LOGDES8(des)
#define LOGDES16(des)       
#define LOGERR(txt,err)     
#define LOGCALL(exp) 
#define LOGENTER     
#define LOGEXIT      
#define LOGMEM(ptr)  
#endif 




#endif  // __FILELOGGER_H__
