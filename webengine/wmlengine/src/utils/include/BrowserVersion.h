/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Browser version definition
*
*/


#ifndef BROWSERVERSION_H
#define BROWSERVERSION_H

#define MobileBrowserName (_L("MobileBrowser"))

#if defined (__S60_50__)
    #define MobileBrowserVersion (_L("5.0"))
#elif defined (__S60_32__)
    #define MobileBrowserVersion (_L("3.2"))
#elif defined (__SERIES60_31__)
    #define MobileBrowserVersion (_L("3.1"))   
#elif defined (__SERIES60_30__)
    #define MobileBrowserVersion (_L("3.0"))
#elif defined(__SERIES60_28__)
    #define MobileBrowserVersion (_L("2.8"))
#elif defined (__SERIES60_27__)
    #define MobileBrowserVersion (_L("2.7"))
#elif defined (__SERIES60_26__)
    #define MobileBrowserVersion (_L("2.6"))
#else
    #define MobileBrowserVersion (_L("2.x"))
#endif

#define MobileBrowserBuild (_L("0516"))


#endif // BROWSERVERSION_H
//end of file
