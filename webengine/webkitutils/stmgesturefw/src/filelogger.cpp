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
* Description:  Gesture helper implementation
*
*/
#include "filelogger.h"

#include <bautils.h>
#include <pathinfo.h>

_LIT(KLogFolder, "logs\\");
_LIT(KLogFileExt, ".log");
_LIT8(KLineEnd, "\r\n");

const TInt KTimeRecordSize = 25;

_LIT8(KTimeFormat,"%04d-%02d-%02d %02d:%02d:%02d.%03d ");
_LIT8(KLogStart, "--== New %S log ==--\r\n");

CFileLogger::CFileLogger()
{
}

TBool CFileLogger::Construct()
{
    TInt err = iFs.Connect();
    if (!err)
    {
        TFileName logFileName;
        err = !GetLogFileNameWithoutExt(logFileName);
        if(!err)
        {
            logFileName.Append(KLogFileExt);
            err = iFile.Create(iFs, logFileName, EFileShareAny | EFileWrite);
            if(!err)
            {
                TInt pos(0);
                iFile.Seek(ESeekEnd, pos);
                TBuf8<KMaxFullName> appName;
                appName.Copy(BaflUtils::ExtractAppNameFromFullName(RThread().FullName()));
                iLogBuffer.AppendFormat(KLogStart, &appName);
                err = iFile.Write(iLogBuffer);
            }
        }
    }
    return (!err);
}

CFileLogger* CFileLogger::Logger()
{
    CFileLogger* logger = (CFileLogger*)Dll::Tls();
    if (!logger)
    {
        logger = new CFileLogger;
        if (logger)
        {
            if (logger->Construct())
            {
                Dll::SetTls(logger);
            }
            else
            {
                delete logger;
                logger = NULL;
            }
        }
    }
    return logger;
}

void CFileLogger::Close()
{
    delete (CFileLogger*)Dll::Tls();
    Dll::FreeTls();
}

CFileLogger::~CFileLogger()
{
    iFile.Close();
    iFs.Close();
}

TBool CFileLogger::GetLogFileNameWithoutExt(TDes& aFileName)
{
    TFileName path = PathInfo::MemoryCardRootPath() ;
    path.Append(PathInfo::OthersPath()) ;   // Put the log in the "Others" directory
    aFileName.Append(path);
    aFileName.Append(KLogFolder);
    TBool res = BaflUtils::FolderExists(iFs, aFileName);
    if (!res)
    {
        aFileName.Zero();
        aFileName.Append(path) ; // put it then directly to Others folder
        res = ETrue ;
    }
    TPtrC fileName(BaflUtils::ExtractAppNameFromFullName(RThread().FullName()));
    aFileName.Append(fileName);
    TTime time;
    time.HomeTime();
    TDateTime dateTime;
    dateTime = time.DateTime();
    // add timestamp to the name (then one has to clear them manually)
    aFileName.AppendNum(dateTime.Month()+1) ;
    aFileName.AppendNum(dateTime.Day()+1) ;
    aFileName.AppendNum(dateTime.Hour()) ;
    aFileName.AppendNum(dateTime.Minute()) ;
    aFileName.AppendNum(dateTime.Second()) ;
    return res;
}

void CFileLogger::Write(const TDesC8& aText)
{
    CFileLogger* logger = Logger();
    if(logger)
    {
        logger->iLogBuffer.Copy(aText);
        logger->DoWrite();
    }
}

void CFileLogger::Write(const TDesC16& aText)
{
    CFileLogger* logger = Logger();
    if(logger)
    {
        logger->iLogBuffer.Copy(aText);
        logger->DoWrite();
    }
}

void CFileLogger::Write(TRefByValue<const TDesC8> aFmt,...)
{
    CFileLogger* logger = Logger();
    if (logger)
    {
        VA_LIST list;
        VA_START(list, aFmt);
        logger->iLogBuffer.FormatList(aFmt, list);
        logger->DoWrite();
        VA_END(list);
    }
}

void CFileLogger::DoWrite()
{
    if(iFile.SubSessionHandle())
    {
        TTime time;
        time.HomeTime();
        TDateTime dateTime;
        dateTime = time.DateTime();
        TBuf8<KTimeRecordSize> timeRecord;
        timeRecord.Format(KTimeFormat, dateTime.Year(), dateTime.Month()+1, dateTime.Day()+1, 
                          dateTime.Hour(), dateTime.Minute(), dateTime.Second(), 
                          dateTime.MicroSecond()/1000);
        iLogBuffer.Insert(0, timeRecord);
        iLogBuffer.Append(KLineEnd);
        iFile.Write(iLogBuffer);
    }
}
void CFileLogger::DoFlush()
{
    if(iFile.SubSessionHandle())
    {
        iFile.Flush();
    }

}
void CFileLogger::Flush()
{
    CFileLogger* logger = Logger();
    if (logger)
    {
        logger->DoFlush() ;
    }
}
