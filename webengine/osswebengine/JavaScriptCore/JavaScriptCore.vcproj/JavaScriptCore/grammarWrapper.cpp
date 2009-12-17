/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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
// This file exists to work around a bug in Visual Studio 2005
// Occasionally on launch VS05 will fail to correctly find and
// identify generated files, thereby excluding them from the build
// list and defaulting the "build rule" to "custom build rule"
// instead of whatever may be appropriate for the file type.

#include "grammar.cpp"
