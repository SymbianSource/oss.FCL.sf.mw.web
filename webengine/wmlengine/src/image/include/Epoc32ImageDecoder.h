/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies).
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


/* CEpoc32ImageDecoder class
 *
 * Every non-canned image has an image decoder associated with it. This is opened
 * when the image is created in order to get the size. This is an asynchronous
 * operation; "MiuoOpenComplete" is called when the opening is complete.
 *
 * The first time an image is displayed, "Decode" is called, which starts the
 * asynchrounous decoding process. When this completes, the image object is
 * updated with the decoded image, which then causes the new image to be
 * displayed.
 *
 * A decode is also done every time the frame number of an animated image is
 * changed.
 */

#ifndef EPOC32IMAGEDECODER_H
#define EPOC32IMAGEDECODER_H

// INCLUDES

#include <e32base.h>
#include <imageconversion.h>

#include "nw_image_epoc32simpleimage.h"
   
// FORWARD DECLARATIONS
// #define INCREMENTAL_IMAGE_ON

class CFbsBitmap;

// CLASS DECLARATION

class CEpoc32ImageDecoder : public CActive
    {
    public:
        static CEpoc32ImageDecoder* NewL(NW_Image_Epoc32Simple_t* aImage);
        void ConstructL();
        void RunL();
        void DoCancel() {iImageConverter->Cancel();}

        virtual ~CEpoc32ImageDecoder();

        /*!
         @function Decode
         @discussion Opens data into fbsBitmap. 
         */
        void Open();

        /*!
         @function Decode
         @discussion Converts data into fbsBitmap. 
         */
        TInt Decode(); 

        NW_Bool IncrementImage(NW_Bool& startingFrameSequenceOver);

        /*!
         @function PartialNextChunkL
         @discussion new chunk of raw data
         */
        void PartialNextChunkL( NW_Buffer_t* aNextChunk );

        /*!
         @function ImageDataComplete
         @discussion end of raw data
         */
        void ImageDataComplete();


        /*!
         @function ResetImage
         @discussion display the last frame of an animated image
         */
		void ResetImage();

        /*!
         @function Finished
         @discussion Finished animating image. 
         */
		TBool Finished();


        inline void SetImageHasNotBeenOpened (NW_Bool aImageHasNotBeenOpened) { iImageHasNotBeenOpened = aImageHasNotBeenOpened; }
    
        enum TDecoderState   
        {  
        ID_IDLE = 0,  
        ID_INITIALIZING,  
        ID_DECODING,  
        // state added to check if the decoding is already complete  
        ID_DECODE_COMPLETE  
        };     
        /*  
        @function getDecoderState  
        @discussion Return the current state of the decoder   
        */  
        
        inline TDecoderState getDecoderState() { return iState; }  

    private:

        void DoDecodeL(); 

        CEpoc32ImageDecoder(NW_Image_Epoc32Simple_t* aImage);

        /*!
        @function OpenComplete

        @discussion Called when the image file has been opened
        */
        void OpenComplete();

        /*!
        @function ConvertComplete

        @discussion Called when conversion process performed on the bitmap is complete
        */
        void ConvertComplete();

        /*!
        @function CreateComplete

        @discussion Called when the file in which the bitmap is to be saved has been created 
        */
        void CreateComplete( TInt aError );

        TDisplayMode GetDisplayMode( TFrameInfo& aFrameInfo );

        void CheckAnimation();
        void DeleteImageConverterBitmapMask();
        TInt16 GetLoopCount();

        /*
         * imageSizeCheck: This compares the original image size with
         * that specified in the <img src = ".." width="" height="">. Then
         * creates bit map accordingly. The value is return true if the 
         * <img> tag attribute values need to be used.
         */

         NW_Bool ImageSizeCheck(TSize iFrameInfoOverallSize, TSize* sizeSet);
         TInt ImgAttrInfo();
		 TInt CheckForDRMImage();

    private:
        enum TDecoderState iState; // Current decoder operation 

        CBufferedImageDecoder* iImageConverter;  // Connection to media server
        
        NW_Image_Epoc32Simple_t* iImage;// OOC image
        TPtrC8 iImageDataPtr;           // Raw unconverted image data
        TFrameInfo iFrameInfo;          // Info about image from media server
        NW_Bool iIsAnimated;            // NW_TRUE if more than one frame in image
        TUint iCurrentFrame;            // Number of current frame displayed
        CFbsBitmap* iBitmap;            // Decoded bitmap
        CFbsBitmap* iMask;              // Decoded bitmap mask
        NW_Bool iImageHasNotBeenOpened; // NW_TRUE the first time an image is opened
        TBool  iMoreData;               // iMoreData
        TBool  iFirstFrameComplete;     // first frame complete
        TBool  iRawDataComplete;        // we've got all the raw data
        RFs iRfs;                       // A dummy parameter required by the Symbian API
		TUint8 *iDRMOutBuf;             //Outputput buffer for the DRM decryption.
        TBool iFinished;				// Finished animating image.
    };

#endif
