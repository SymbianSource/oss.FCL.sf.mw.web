/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef NW_ORDEREDDITHER_H
#define NW_ORDEREDDITHER_H


#include "nw_imagech_gifdecoder.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define MAX_COLORS  255

extern
TBrowserStatusCode ConvertIntensity(Gif_data_t* Gif_data ,  NW_Uint32 histogram[]);

extern
TBrowserStatusCode ConvertToGreyScale(Gif_data_t* Gif_data);

extern
void DitherPixel(Gif_data_t* Gif_data, NW_Uint16* temp_code, NW_Uint32 pixel);

extern
TBrowserStatusCode NW_ImageCH_GIF_histogram_singlebit_image(NW_Byte ** buf,Gif_data_t* Gif_data,NW_Uint16* offset);


#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_GIFDECODER_H */
