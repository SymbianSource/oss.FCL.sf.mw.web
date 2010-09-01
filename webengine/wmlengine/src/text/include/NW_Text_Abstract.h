/*
* Copyright (c) 2000 - 2001 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef NW_Text_Abstract_h
#define NW_Text_Abstract_h

#include "NW_Text_EXPORT.h"
#include "nw_object_dynamic.h"
#include "nw_text_types.h"
#include "nw_text_iterator.h"
#include <nw_string_char.h>
#include <nw_string_string.h>
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Text_Abstract_Class_s NW_Text_Abstract_Class_t;
typedef struct NW_Text_Abstract_s NW_Text_Abstract_t;

//struct NW_String_String_s;
//typedef struct NW_String_String_s NW_String_t;

// struct NW_String_t;
/* ------------------------------------------------------------------------- *
   public constants
 * ------------------------------------------------------------------------- */
enum {
  NW_Text_Flags_TakeOwnership      = 0x01,
  NW_Text_Flags_Aligned            = 0x02,
  NW_Text_Flags_Copy               = 0x04,
  NW_Text_Flags_CompressWhitespace = 0x08,
  NW_Text_Flags_NullTerminated     = 0x10,
  NW_Text_Flags_RetainLineFeeds    = 0x20
};

/* ------------------------------------------------------------------------- *
   method prototypes
 * ------------------------------------------------------------------------- */
typedef
TBrowserStatusCode
(*NW_Text_Abstract_CreateIterator_t) (NW_Text_Abstract_t* abstractText,
                                      NW_Text_Direction_t direction,
                                      NW_Text_Iterator_t** iterator);

typedef
NW_Uint32
(*NW_Text_Abstract_GetStorageSize_t) (const NW_Text_Abstract_t* abstractText);

typedef
NW_Ucs4
(*NW_Text_Abstract_GetCharacterAt_t) (const NW_Text_Abstract_t* abstractText,
                                      NW_Text_Length_t index);

typedef
NW_Ucs2*
(*NW_Text_Abstract_GetUCS2Buffer_t) (const NW_Text_Abstract_t* abstractText,
                                     NW_Uint16 flags,
                                     NW_Text_Length_t* charCount,
                                     NW_Bool* freeNeeded);

typedef
NW_Text_Length_t
(*NW_Text_Abstract_GetSpaceAfter_t) (const NW_Text_Abstract_t* abstractText,
                                   NW_Text_Length_t index);

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_Text_Abstract_ClassPart_s {
  NW_Text_Abstract_CreateIterator_t createIterator;
  NW_Text_Abstract_GetStorageSize_t getStorageSize;
  NW_Text_Abstract_GetCharacterAt_t getCharacterAt;
  NW_Text_Abstract_GetUCS2Buffer_t getUCS2Buffer;
  NW_Text_Abstract_GetSpaceAfter_t getSpaceAfter;
} NW_Text_Abstract_ClassPart_t;

struct NW_Text_Abstract_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_Text_Abstract_ClassPart_t NW_Text_Abstract;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_Text_Abstract_s {
  NW_Object_Dynamic_t super;

  /* member variables */
  void* storage;
  NW_Text_Length_t characterCount;
  NW_Uint16 flags;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Text_Abstract_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_Text_Abstract))

#define NW_Text_AbstractOf(_object) \
  (NW_Object_Cast (_object, NW_Text_Abstract))

#define NW_TextOf(_object) \
  (NW_Text_AbstractOf (_object))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_TEXT_EXPORT const NW_Text_Abstract_Class_t NW_Text_Abstract_Class;

/* ------------------------------------------------------------------------- *
   public method prototypes
 * ------------------------------------------------------------------------- */
NW_TEXT_EXPORT
TBrowserStatusCode
_NW_Text_Abstract_SetStorage (NW_Text_Abstract_t* thisObj,
                              void* storage,
                              NW_Text_Length_t characterCount,
                              NW_Uint16 flags);

NW_TEXT_EXPORT
TBrowserStatusCode
_NW_Text_Abstract_SetCharacterCount (NW_Text_Abstract_t* thisObj,
                                     NW_Text_Length_t characterCount);

NW_TEXT_EXPORT
NW_Text_Abstract_t*
_NW_Text_Abstract_Copy (const NW_Text_Abstract_t* abstractText,
                        NW_Bool flags);

NW_TEXT_EXPORT
NW_Text_t*
_NW_Text_Abstract_Split (NW_Text_Abstract_t* abstractText,
                         NW_Text_Length_t index);


NW_Text_t*
_NW_Text_Abstract_SplitOnSpace (NW_Text_Abstract_t* abstractText,
                                NW_Text_Length_t index,
                                NW_Text_Length_t *actualIndex);

NW_TEXT_EXPORT
NW_Int32
_NW_Text_Abstract_PartialCompare (const NW_Text_Abstract_t* abstractText,
                                  const NW_Text_Abstract_t* rhsText,
                                  NW_Text_Length_t numCharacters);

NW_TEXT_EXPORT
NW_Int32
_NW_Text_Abstract_Compare (const NW_Text_Abstract_t* abstractText,
                           const NW_Text_Abstract_t* rhsText);

NW_TEXT_EXPORT
NW_Int32
_NW_Text_Abstract_PartialASCIICompare (const NW_Text_Abstract_t* abstractText,
                                       const NW_Uint8* rhsString,
                                       NW_Text_Length_t numCharacters);

NW_TEXT_EXPORT
NW_Int32
_NW_Text_Abstract_ASCIICompare (const NW_Text_Abstract_t* abstractText,
                                const NW_Uint8* rhsString);

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_Text_SetStorage(_object, _storage, _characterCount, _flags) \
  (_NW_Text_Abstract_SetStorage \
     (NW_Text_AbstractOf (_object), (_storage), (_characterCount), (_flags)))

#define NW_Text_GetCharCount(_object) \
  (NW_OBJECT_CONSTCAST(NW_Text_Length_t) NW_Text_AbstractOf (_object)->characterCount)

#define NW_Text_SetCharCount(_object, _characterCount) \
  ((void) (NW_Text_AbstractOf (_object)->characterCount = (_characterCount)))

#define NW_Text_CreateIterator(_object, _direction, _iterator) \
  (NW_Object_Invoke (_object, NW_Text_Abstract, createIterator) ( \
     NW_Text_AbstractOf (_object), (_direction), (_iterator)))

#define NW_Text_GetAt(_object, _index) \
  (NW_Object_Invoke (_object, NW_Text_Abstract, getCharacterAt) ( \
     NW_Text_AbstractOf (_object), (_index)))

#define NW_Text_GetUCS2Buffer(_object, _flags, _characterCount, _freeNeeded) \
  (NW_Object_Invoke (_object, NW_Text_Abstract, getUCS2Buffer) ( \
     NW_Text_AbstractOf (_object), (_flags), (_characterCount), (_freeNeeded)))

#define NW_Text_GetSpaceAfter(_object, _index) \
  (NW_Object_Invoke (_object, NW_Text_Abstract, getSpaceAfter) ( \
     NW_Text_AbstractOf (_object), (_index)))

#define NW_Text_Copy(_object, _deepCopy) \
  (_NW_Text_Abstract_Copy ( \
     NW_Text_AbstractOf (_object), (_deepCopy)))

#define NW_Text_Split(_object, _index) \
  (_NW_Text_Abstract_Split (NW_Text_AbstractOf (_object), (_index)))

#define NW_Text_SplitOnSpace(_object, _index, _actualIndex) \
  (_NW_Text_Abstract_SplitOnSpace (NW_Text_AbstractOf (_object), (_index), (_actualIndex)))

#define NW_Text_PartialCompare(_object, _rhsText, _numCharacters) \
  (_NW_Text_Abstract_PartialCompare ( \
     NW_Text_AbstractOf (_object), NW_Text_AbstractOf (_rhsText), (_numCharacters)))

#define NW_Text_Compare(_object, _rhsText) \
  (_NW_Text_Abstract_Compare ( \
     NW_Text_AbstractOf (_object), NW_Text_AbstractOf (_rhsText)))

#define NW_Text_PartialASCIICompare(_object, _rhsString, _numCharacters) \
  (_NW_Text_Abstract_PartialASCIICompare ( \
     NW_Text_AbstractOf (_object), (_rhsString), (_numCharacters)))

#define NW_Text_ASCIICompare(_object, _rhsString) \
  (_NW_Text_Abstract_ASCIICompare ( \
     NW_Text_AbstractOf (_object), (_rhsString)))

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
NW_TEXT_EXPORT
NW_Text_t*
NW_Text_New (NW_Uint32 encoding,
             void* storage,
             NW_Text_Length_t charCount,
             NW_Uint16 flags);

NW_TEXT_EXPORT
NW_Text_t*
NW_Text_NewFromStringAndDeleteString (NW_String_t* stringStr, 
                                      NW_Uint32 encoding);


NW_TEXT_EXPORT
NW_Text_t*
NW_Text_Concatenate(NW_Text_t* val1, NW_Text_t* val2);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_Text_Abstract_h */



