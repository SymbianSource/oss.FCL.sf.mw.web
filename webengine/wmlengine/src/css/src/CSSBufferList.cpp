/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Hold different CSS stylesheets that are applied to document
*
*/



// INCLUDE FILES
#include <e32def.h>  // First to avoid NULL redefine warning (no #ifndef NULL).
#include "CSSBufferList.h"
#include <nwx_assert.h>
#include "nwx_multipart_generator.h"
#include "nwx_string.h"
#include "nwx_http_defs.h"
#include "nwx_settings.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CCSSBufferEntry::NewL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
CCSSBufferEntry* CCSSBufferEntry::NewL(TText8* aData, TUint32 aLength, TUint32 aEncoding, 
                 TBool aInternalStyle, const TText16* aUrl)
    {
    CCSSBufferEntry* self = new(ELeave)CCSSBufferEntry(aData, aLength, aEncoding, aInternalStyle);
    CleanupStack::PushL(self);
    if (aUrl != NULL)
        {
        self->iUrl = NW_Str_Newcpy(aUrl);
        if (self->iUrl == NULL)
          {
          User::Leave(KErrNoMemory);
          }
        }
    CleanupStack::Pop(); // self
    return self;
    }

// -----------------------------------------------------------------------------
// CCSSBufferList::CCSSBufferList
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCSSBufferList::ConstructL()
  {
  // initialize the bufferList 
  iBufferList = new (ELeave) CBufferListArray(2);
  }

// -----------------------------------------------------------------------------
// CCSSBufferList::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCSSBufferList* CCSSBufferList::NewL()
  {
  CCSSBufferList* self = new( ELeave ) CCSSBufferList();
  
  CleanupStack::PushL( self );
  self->ConstructL();
  CleanupStack::Pop();

  return self;
  }

// Destructor
CCSSBufferList::~CCSSBufferList()
{
  if(iBufferList)
    {
    iBufferList->ResetAndDestroy();
    }
  delete iBufferList;
}


// -----------------------------------------------------------------------------
// CCSSBufferList::AddL
// adds CSS stylesheet to list
// -----------------------------------------------------------------------------
//
void 
CCSSBufferList::AddL(TText8* aBuffer, TUint32 aLength, TUint32 aEncoding, 
                     TBool aInternalStyle,  TText16* aUrl)
{
  // entry to be inserted
  CCSSBufferEntry* tempEntry = CCSSBufferEntry::NewL(aBuffer, aLength, aEncoding, aInternalStyle, aUrl);
  CleanupStack::PushL( tempEntry );
  TInt size = iBufferList->Count();
  tempEntry->iPriority = size;

  // create a new entry and initialize it */
  iBufferList->AppendL(tempEntry);

  CleanupStack::Pop(); // tempEntry 
}

// -----------------------------------------------------------------------------
// CCSSBufferList::InitializeParser
// adds CSS stylesheet to list
// -----------------------------------------------------------------------------
//
void 
CCSSBufferList::InitializeParser(TCSSParser* aParser, TInt aIndex)
{
  CCSSBufferEntry* entry = iBufferList->At(aIndex);
  NW_ASSERT(entry->iData);
  aParser->Init(entry->iData, entry->iLength, entry->iEncoding);
}

// -----------------------------------------------------------------------------
// CCSSBufferList::UpdateEntry
// This method is Invoked when a load respose comes back for a CSS
// -----------------------------------------------------------------------------
//
void 
CCSSBufferList::UpdateEntry(TText8* aData, TUint32 aLength, TUint32 aEncoding, TInt aIndex)
{
  CCSSBufferEntry* entry = iBufferList->At(aIndex);
  entry->iData = aData;
  entry->iLength = aLength;
  entry->iEncoding = aEncoding;
}

// -----------------------------------------------------------------------------
// CCSSBufferList::GetEncoding
// returns encoding of the CSS in aIndex
// -----------------------------------------------------------------------------
//
TUint32
CCSSBufferList::GetEncoding(TInt aIndex)
{
  CCSSBufferEntry* entry = iBufferList->At(aIndex);
  return entry->iEncoding;
}

// -----------------------------------------------------------------------------
// CCSSBufferList::GetSize
// Returns number of CSS objects
// -----------------------------------------------------------------------------
//
TInt CCSSBufferList::GetSize()
{
  return iBufferList->Count();
}

// -----------------------------------------------------------------------------
// CCSSBufferList::GetExternalSize
// Returns number of externally loaded CSS objects
// -----------------------------------------------------------------------------
//
TInt CCSSBufferList::GetExternalSize()
{
  TInt externalSize = 0;
  TInt size = GetSize();

  /* for convenience */
  for (TInt index = 0; index < size; index++)
  {
    CCSSBufferEntry* entry = iBufferList->At(index);

    /* If it was loaded externally, the url will be set */
    if (entry->iUrl != NULL)
    {
      externalSize++;
    }
  }
  return externalSize;
}

// -----------------------------------------------------------------------------
// CCSSBufferList::WriteMultipartSegments
// Write CSS components to a file in the form of multipart segments.
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
CCSSBufferList::WriteMultipartSegments(NW_Osu_File_t aFh)
{
  TUint originalEncoding = NW_Settings_GetOriginalEncoding();

  TBrowserStatusCode retStatus = KBrsrSuccess;

  TInt size = GetSize();
  for (TInt index = 0; index < size; index++)
  {
    TText8* segment = NULL;
    TUint32 segmentLen = 0;

    CCSSBufferEntry* entry = iBufferList->At(index);
    
    /* If it was loaded externally, the url will be set */
    if (entry->iUrl != NULL)
    {

    char* url = NW_Str_CvtToAscii(entry->iUrl);
    if (url == NULL)
    {
       return KBrsrOutOfMemory;
    }
    else
        {
      retStatus = NW_CreateMultipartSegment
                  (
                    entry->iEncoding,
					originalEncoding,  //this parameter is used by saved deck, it doesn't matter here.
                    (NW_Http_ContentTypeString_t)HTTP_text_css_string,
                    url,
                    entry->iData,
                    entry->iLength,
                    &segment,
                    &segmentLen
                  );
      NW_Mem_Free(url);
      url = NULL;
        }
      if (retStatus != KBrsrSuccess)
        return retStatus;

      retStatus = NW_Osu_WriteFile(aFh, segment, segmentLen);
      NW_Mem_Free(segment);
      if (retStatus != KBrsrSuccess)
        return retStatus;
    }
  }
  return retStatus;
}

// -----------------------------------------------------------------------------
// CCSSBufferList::UpdatePriority
// This method is Invoked after adding a buffer for an @import request
// -----------------------------------------------------------------------------
//
void
CCSSBufferList::UpdatePriority(TInt aRefIndex)
{
  CCSSBufferEntry* entry1 = iBufferList->At(aRefIndex);
  TInt refPriority = entry1->iPriority;

  TInt size = GetSize();
  CCSSBufferEntry* entry2 = iBufferList->At(size-1);
  entry2->iPriority = refPriority;
  iBufferList->At(size-1) = entry2;
  
  for (TInt index = 0; index < (size-1); index++)
  {
    CCSSBufferEntry* entry = iBufferList->At(index);
    if (entry->iPriority >= refPriority)
    {
      entry->iPriority++;
      iBufferList->At(index) = entry;
    }
  }
}

// -----------------------------------------------------------------------------
// CCSSBufferList::GetPriority
// Returns priority of stylesheet
// -----------------------------------------------------------------------------
//
TInt
CCSSBufferList::GetPriority(TInt aIndex)
{
  CCSSBufferEntry* entry = iBufferList->At(aIndex);
  return entry->iPriority;
}
