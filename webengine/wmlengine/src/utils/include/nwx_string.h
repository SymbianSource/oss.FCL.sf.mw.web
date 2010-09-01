/*
* Copyright (c) 1999 - 2001 Nokia Corporation and/or its subsidiary(-ies).
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


/***************************************************************************
**   File: nwx_string_h
**   Subsystem Name:String
**   Purpose:   Provides string manipulation routines in a platform independent
**              manner.
**************************************************************************/

#ifndef NWX_STRING_H
#define NWX_STRING_H

#ifdef __cplusplus
extern "C" {
#endif

/*
** Includes
*/

#include <stdarg.h>
#include "nwx_defs.h"
#include "nwx_memseg.h"
#include "BrsrStatusCodes.h"

#define NW_DEFAULT_FLOAT_PREC 6

/*  These constants can used to implement the NW_Str_Iswhatever()
**  functions if there is a conflict between the WAE character set
**  definitions and the C runtime IsWhatever() functions
*/

#define WAE_ASCII_NULL                0x00

#define WAE_ASCII_UPPER_A             0x41
#define WAE_ASCII_UPPER_F             0x46
#define WAE_ASCII_UPPER_Z             0x5A

#define WAE_ASCII_LOWER_A             0x61
#define WAE_ASCII_LOWER_F             0x66
#define WAE_ASCII_LOWER_Z             0x7A

#define WAE_ASCII_0                   0x30
#define WAE_ASCII_9                   0x39

#define WAE_ASCII_SPACE               0x20
#define WAE_ASCII_HT                  0x09
#define WAE_ASCII_LF                  0x0a
#define WAE_ASCII_VT                  0x0b
#define WAE_ASCII_FF                  0x0c
#define WAE_ASCII_CR                  0x0d
#define WAE_ASCII_ZSPACE              0x200b

#define WAE_ASCII_HYPHEN              0x2d
#define WAE_ASCII_SOFT_HYPHEN         0xad

#define WAE_ASCII_ANY_CHAR_BEGIN      0x0
#define WAE_ASCII_ANY_CHAR_END        0x7F

#define WAE_ASCII_DOLLAR              0x24
#define WAE_ASCII_PLUS                0x2B
#define WAE_ASCII_MINUS               0x2D
#define WAE_ASCII_POINT               0x2E
#define WAE_ASCII_UNDERSCORE          0x5F
#define WAE_ASCII_LEFT_BRACE          0x28
#define WAE_ASCII_RIGHT_BRACE         0x29
#define WAE_ASCII_COLON               0x3a

/*
#define WAE_ASCII_PUNC_RANGE_1_BEGIN  0x20
#define WAE_ASCII_PUNC_RANGE_1_END    0x2F
#define WAE_ASCII_PUNC_RANGE_2_BEGIN  0x3A
#define WAE_ASCII_PUNC_RANGE_2_END    0x40
#define WAE_ASCII_PUNC_RANGE_3_BEGIN  0x5B
#define WAE_ASCII_PUNC_RANGE_3_END    0x60
#define WAE_ASCII_PUNC_RANGE_4_BEGIN  0x7B
#define WAE_ASCII_PUNC_RANGE_4_END    0x7F

#define WAE_ASCII_COMMA               0x2C
*/

/*
**----------------------------------------------------------------
** Type Declarations
**----------------------------------------------------------------
*/

typedef enum NW_Str_Location_e{
  NW_Str_Start,
  NW_Str_End,
  NW_Str_Both
}NW_Str_Location_t;
/* Deprecated NW_Str_Begining because it misspelled beginning: */
#define NW_Str_Begining NW_Str_Start

/*
** Global Function Declarations
*/

/* Test if character is alphanumeric */
NW_Int32 NW_Str_Isalnum(const NW_Ucs2 ch);

/* Test if character is alphabetic */
NW_Int32 NW_Str_Isalpha(const NW_Ucs2 ch);

/* Test if character is decimal digit */  
NW_Int32 NW_Str_Isdigit(const NW_Ucs2 ch);

/* Test if character is hexadecimal digit */  
NW_Int32 NW_Str_Isxdigit(const NW_Ucs2 ch);

/* Test if character is white space */
NW_Int32 NW_Str_Isspace(const NW_Ucs2 ch);

/* Test if character is a newline */
NW_Int32 NW_Str_Isnewline(const NW_Ucs2 ch);

/* Test if line can break at character */
NW_Int32 NW_Str_Isbreak(const NW_Ucs2 ch);

/* Test if character is punctuation */
NW_Int32 NW_Str_Ispunct(const NW_Ucs2 ch);

/* Test if character is ascii (0x00 to 0x7F) */
NW_Int32 NW_Str_Isascii(const NW_Ucs2 ch);

/* Test if character is control char (0x00 to 0x1F) */
NW_Int32 NW_Str_Iscntrl(const NW_Ucs2 ch);

/* Test if character is uppercase */
NW_Int32 NW_Str_Isupper(const NW_Ucs2 ch);

/* Test if character is lowercase */
NW_Int32 NW_Str_Islower(const NW_Ucs2 ch);

/* Test if the phone number */
NW_Bool NW_Str_StrIsNumber(NW_Ucs2 *string, NW_Uint32 count);

/* Test, if a valid length */
NW_Bool NW_Str_StrIsValidLength(NW_Ucs2 *string);

/* Create a new string */
NW_Ucs2 *NW_Str_New(const NW_Uint32 len);

/* Create a new copy of a string */
NW_Ucs2 *NW_Str_Newcpy(const NW_Ucs2 *string);

/* Create a new copy of two concatinated strings */
NW_Ucs2 *NW_Str_Newcat(const NW_Ucs2 *string1, 
                       const NW_Ucs2 *string2);

/* Create a new string */
NW_Ucs2 *NW_Str_SegNew(const NW_Uint32 len, 
                       const NW_Mem_Segment_Id_t sid);

/* Create a new copy of a string */
NW_Ucs2 *NW_Str_SegNewcpy(const NW_Ucs2 *string, 
                          const NW_Mem_Segment_Id_t sid);

/* Create a new copy of two concatinated strings */
NW_Ucs2 *NW_Str_SegNewcat(const NW_Ucs2 *string1, 
                          const NW_Ucs2 *string2,
                          const NW_Mem_Segment_Id_t sid);

/* Delete a string */
void NW_Str_Delete(NW_Ucs2 *string);

/* Return number of characters in string (excluding null termination) */
NW_Uint32 NW_Str_Strlen(const NW_Ucs2 *string);

/* Return number of bytes in string including terminator */
NW_Uint32 NW_Str_Strsize(const NW_Ucs2 *string);

/* Copy null-terminated string */
NW_Ucs2 *NW_Str_Strcpy(NW_Ucs2 *destination,
                         const NW_Ucs2 *source);

/* Concatinate null-terminated strings */
NW_Ucs2 *NW_Str_Strcat(NW_Ucs2 *destination,
                       const NW_Ucs2 *source);

/* Copy first "count" characters of null-terminated string */
NW_Ucs2 *NW_Str_Strncpy(NW_Ucs2 *destination,
                        const NW_Ucs2 *source,
                        const NW_Uint32 count);

/* Find first occurrence of character in string */
NW_Ucs2 *NW_Str_Strchr(const NW_Ucs2 *string, 
                       const NW_Ucs2 wc);

/* Find last occurrence of character in string */
NW_Ucs2 *NW_Str_Strrchr(const NW_Ucs2 *string, 
                        const NW_Ucs2 wc);

/* Find first occurrence of a character set in string */
NW_Uint32 NW_Str_Strcspn(const NW_Ucs2 *string,
                         const NW_Ucs2 *search);

/* Find substring within a string */
NW_Ucs2 *NW_Str_Strstr(const NW_Ucs2 *string,
                       const NW_Ucs2 *search);

/* Find a substring within a string ended by terminator */
NW_Ucs2* NW_Str_Strstrtil(const NW_Ucs2 *string, 
                                char sub_terminator);
/* Compare two strings */
NW_Int32 NW_Str_Strcmp(const NW_Ucs2 *string1,
                       const NW_Ucs2 *string2);

/* Compare two strings, case-insenstive */
NW_Int32 NW_Str_Stricmp(const NW_Ucs2 *string1,
                        const NW_Ucs2 *string2);

/* Compare first "count" characters from two strings */
NW_Int32 NW_Str_Strncmp(const NW_Ucs2 *string1,
                        const NW_Ucs2 *string2,
                        const NW_Uint32 count );

/* Compare first "count" characters from two strings, case-insenstive */
NW_Int32 NW_Str_Strnicmp(const NW_Ucs2 *string1,
                         const NW_Ucs2 *string2,
                         const NW_Uint32 count );

/* Calculate a 32-bit hash code for a string */
NW_Uint32 NW_Str_Hash(const NW_Ucs2 *string);

/* Convert a string to an integer */
NW_Int32  NW_Str_Atoi(const NW_Ucs2 *string);

/* Convert to string using specified base */
NW_Ucs2 *NW_Str_ItoBase(const NW_Int32  i, NW_Ucs2* string, const NW_Int32 base);

/* Convert an integer to a string */
NW_Ucs2 *NW_Str_Itoa(const NW_Int32  i, NW_Ucs2* string);

/* Convert to string using specified base */
NW_Ucs2 *NW_Str_UltoBase(const NW_Int32  i, NW_Ucs2* string, const NW_Int32 base);

/* Convert an NW_Uint32  integer to a string */
NW_Ucs2 *NW_Str_Ultoa(const NW_Uint32 i, NW_Ucs2* string);

/* Convert a string to an integer */
NW_Int32 NW_Str_Strtol(const NW_Ucs2 *ptr, NW_Ucs2 **endPtr, NW_Int32  base);

/* Convert a string to an unsigned integer */
NW_Int32 NW_Str_Strtoul(const NW_Ucs2 *ptr, NW_Ucs2 **endPtr, NW_Int32  base);

/* Convert a string to an integer and check for overflow */
TBrowserStatusCode NW_Str_CvtToInt32(const NW_Ucs2 *string, NW_Ucs2 **endptr, 
                              NW_Int32  base, NW_Int32 *ret_long);

/* Convert a string to a float point value and check for overflow */
TBrowserStatusCode NW_Str_CvtToFloat32(const NW_Ucs2 *string, NW_Ucs2 **endptr, 
                                NW_Float32 *ret_double);

/* Increment string pointer by "count" characters */
NW_Ucs2 *NW_Str_Inc(const NW_Ucs2 *string, const NW_Int32 count);

/* Convert ucs2 string to new ascii string */
char *NW_Str_CvtToAscii(const NW_Ucs2 *string);

/* Convert null-terminated ascii string to new ucs2 string */
NW_Ucs2 *NW_Str_CvtFromAscii(const char *string);

/* Copy a ucs2 string to an already allocated ascii string. */
void NW_Str_CpyToAscii(char* dest, const NW_Ucs2* src);
/* Copy a ascii string to an already allocated ucs2 string. */
void NW_Str_CpyFromAscii(NW_Ucs2* dest, const char *src);

/* Convert character to lower case */
NW_Ucs2 NW_Str_ToLower(const NW_Ucs2 ch);

/* Convert character to upper case */
NW_Ucs2 NW_Str_ToUpper(const NW_Ucs2 ch);

/* Convert string to lower case for ASCII value only */
void NW_Str_Strlwr(NW_Ucs2 *string);

/* Convert string to upper case for ASCII value only */
void NW_Str_Strupr(NW_Ucs2 *string);

/* Convert Ucs2 string to Upper case */
TBrowserStatusCode NW_Str_Ucs2StrUpr(NW_Ucs2 **ppStr);

/* Convert Ucs2 string to Lower case */
TBrowserStatusCode NW_Str_Ucs2StrLwr(NW_Ucs2 **ppStr);

/* Capitilize the first character in each word */
TBrowserStatusCode NW_Str_Ucs2StrTitle(NW_Ucs2 **ppStr);

/* Return new substring of original string */
NW_Ucs2 *NW_Str_Substr(const NW_Ucs2 *string, 
                       const NW_Uint32 start, const NW_Uint32 len);

/* Return new substring of original string */
NW_Ucs2 *NW_Str_SegSubstr(const NW_Ucs2 *string, 
                          const NW_Uint32 start, const NW_Uint32 len,
                          const NW_Mem_Segment_Id_t sid);

/* Format to a string */
NW_Int32 NW_Str_Sprintf(NW_Ucs2 *buf, const NW_Ucs2 *format, ...);
NW_Int32 NW_Str_Vsnprintf(NW_Ucs2 *buf, const NW_Int32 count, const NW_Ucs2 *format, va_list *argptr );

/* Create a new copy of a string from an Ascii string constant */
NW_Ucs2 *NW_Str_NewcpyConst(const char* const string);

/* Copy null-terminated Ascii string constant  */
NW_Ucs2 *NW_Str_StrcpyConst(NW_Ucs2 *destination,
                            const char* const source);

/* Concatinate null-terminated Ascii string constant */
NW_Ucs2 *NW_Str_StrcatConst(NW_Ucs2 *destination,
                            const char* const source);

/* Compare a string to an Strii string constant */
NW_Int32 NW_Str_StrcmpConst(const NW_Ucs2 *string1,
                            const char* const string2);

/* Compare a string to an Strii string constant, case-insenstive */
NW_Int32 NW_Str_StricmpConst(const NW_Ucs2 *string1,
                             const char* const string2);

/* Compare first "count" characters of a string to an Strii string constant */
NW_Int32 NW_Str_StrncmpConst(const NW_Ucs2 *string1,
                             const char* const string2,
                             const NW_Uint32 count);

/* Compare first "count" characters of a string to an Strii string constant , case-insenstive */
NW_Int32 NW_Str_StrnicmpConst(const NW_Ucs2 *string1,
                              const char* const string2,
                              const NW_Uint32 count);

/* Compress whitespace, per WML Ver 1.2, section 11.8.1 */
void NW_Str_CompressWhitespace(NW_Ucs2 *s);

/* Replace all the occurance of char1 with char2 */
NW_Int32 NW_Str_Strcharreplace(NW_Ucs2* const string, const NW_Ucs2 char1, const NW_Ucs2 char2);

/* Creates (allocates memory) a new string with all the whitespaces removed from the specified location */
NW_Ucs2* NW_Str_Trim(const NW_Ucs2* string, NW_Str_Location_t where);

/* Skips over leading whitespaces and/or truncates trailing whitespaces depending on specified location */
NW_Ucs2* NW_Str_TrimInPlace(NW_Ucs2* string, NW_Str_Location_t where);

/* Create a Ascii string */
char *NW_Asc_New(const NW_Uint32 len);

/* Create a Ascii string */
void NW_Asc_Delete(char *s1);

NW_Uint32 NW_Asc_strlen(const char *s);

char *NW_Asc_strcpy(char *s1, const char *s2);

char *NW_Asc_strcat(char *s1, const char *s2);

NW_Int32 NW_Asc_Strchr(const char* str, char ch);

NW_Int32 NW_Asc_strcmp(const char *string1, const char *string2);

NW_Int32 NW_Asc_stricmp(const char *string1, const char *string2);

NW_Int32 NW_Asc_strnicmp(const char *string1, const char *string2,
                         const NW_Uint32 count);

char *NW_Asc_Itoa(const NW_Int32  i, char* string);

NW_Ucs2* NW_Asc_toUcs2(char *string);

// compare string as byte *, case insensitive, "count" indicate the first number of bytes to compare.
NW_Int32 NW_Byte_Strnicmp(const NW_Byte *string1,
                         const NW_Byte *string2,
                         const NW_Uint32 count );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /* NWX_STRING_H */
