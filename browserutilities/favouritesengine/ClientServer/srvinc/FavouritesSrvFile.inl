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
* Description: 
*      Inline method definitions of class CFavouritesSrvFile.   
*      
*
*/


#ifndef FAVOURITES_SRV_FILE_INL
#define FAVOURITES_SRV_FILE_INL

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CFavouritesSrvFile::~CFavouritesSrvFile
// ---------------------------------------------------------
//
CFavouritesSrvFile::~CFavouritesSrvFile()
    {
    iFile.Close();
    iFs.Close();
    }

// ---------------------------------------------------------
// CFavouritesSrvFile::CFavouritesSrvFile
// ---------------------------------------------------------
//
CFavouritesSrvFile::CFavouritesSrvFile()
    {
    }

#endif

// End of File
