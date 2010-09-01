/*
* Copyright (c) 1999-2000 Nokia Corporation and/or its subsidiary(-ies).
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

/* This is a dummy file for core code.  These values are 
   based on NT limitation.  These should be change according to
   to porting platform, if the platform does not have a float.h file
*/
#ifndef _FLOAT_H_
#define _FLOAT_H_ 

#define FLT_MAX         3.402823466e+38F  /* max value from NT */
#define FLT_MIN         1.175494351e-38F  /* min positive value from NT */
#define FLT_DIG         6
#define FLT_MAX_10_EXP  38
#define FLT_MIN_10_EXP -37

#endif /*_FLOAT_H_*/
