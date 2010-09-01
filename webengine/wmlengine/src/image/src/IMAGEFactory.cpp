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


#include "NW_Image_ImageFactoryI.h"
#include "nwx_string.h"

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_Image_ImageFactory_Class_t NW_Image_ImageFactory_Class = {
  { /* NW_Object_Core        */
    /* super                 */ &NW_Object_Core_Class,
    /* queryInterface        */ _NW_Object_Core_QueryInterface
  },
  { /* NW_Image_ImageFactory */
    /* mimeTable             */ NULL,
    /* numEntries            */ 0,
    /* createImage           */ _NW_Image_ImageFactory_CreateImage
  }
};

/* ------------------------------------------------------------------------- *
   convenience function
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_Image_AbstractImage_t*
_NW_Image_ImageFactory_CreateImage (const NW_Image_ImageFactory_t* imageFactory,
                                    NW_Http_ContentTypeString_t contentTypeString,
                                    NW_Buffer_t* imageData)
{
  const NW_Image_ImageFactory_MimeTableEntry_t* mimeTable;
  NW_Uint16 numEntries;
  NW_Uint16 index;

  /* lookup the contentType and create an appropriate image object */
  mimeTable = NW_Image_ImageFactory_GetMimeTable (imageFactory);
  numEntries = NW_Image_ImageFactory_GetNumEntries (imageFactory);
  for (index = 0; index < numEntries; index++) {
    if (0 == NW_Asc_stricmp((char*)mimeTable[index].contentTypeString, (char*)contentTypeString)) {
      return (NW_Image_AbstractImage_t*) NW_Object_New (mimeTable[index].objClass, imageData);
    }
  }

  /* no image format registered for the supplied mimeType */
  return NULL;
}

/* ------------------------------------------------------------------------- */
const NW_Image_ImageFactory_t NW_Image_ImageFactory = {
  { &NW_Image_ImageFactory_Class }
};
