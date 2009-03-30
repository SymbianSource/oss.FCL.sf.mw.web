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


/*****************************************************************
**    File name:  nwx_string.c
**    Part of: string (NW)
**    Description:   Provides interfaces to (NW) string API .
******************************************************************/

/*
**-------------------------------------------------------------------------
**  Include Files
**-------------------------------------------------------------------------
*/
#include "nwx_string.h"
#include "nwx_mem.h"
#include "nwx_memseg.h"
#include "nwx_float.h"
#include "nwx_math.h"
#include "BrsrStatusCodes.h"

/*lint -save -e794 Conceivable use of null pointer*/

/*
**-------------------------------------------------------------------------
**  Constants
**-------------------------------------------------------------------------
*/


/*
**-------------------------------------------------------------------------
**  Internal Types
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  Macros
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  Internal Prototypes
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  File Scoped Static Variables
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  Global Variable Definitions
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  Internal Functions
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  External Public (Exported) Functions
**-------------------------------------------------------------------------
*/

/*****************************************************************
**  Name:  NW_Asc_New
**  Description:  Create a Ascii string
**  Parameters:   NW_Uint32 len
**  Return Value: new Ascii String or Null of no memory available
******************************************************************/
char *NW_Asc_New(const NW_Uint32 len)
{
  char *tmp;

  tmp = (char*) NW_Mem_Malloc((len + 1) * sizeof(char));
  if (tmp != NULL) {
    tmp[0] = WAE_ASCII_NULL;
  }

  return tmp;
}

/*****************************************************************
**  Name:  NW_Asc_Delete
**  Description:  Delete a Ascii string
**  Parameters:   char *
**  Return Value: void
******************************************************************/
void NW_Asc_Delete(char *string)
{
  if (string != NULL) {
    NW_Mem_Free(string);
  }
  return;
}

/*****************************************************************
**  Name:  NW_Asc_ToUpper
**  Description:  Converts a char to upper case
**  Parameters:   ch - the character to convert
**  Return Value: the converted character
******************************************************************/
NW_Ucs2 NW_Asc_ToUpper(const char ch)
{
  if((ch >= WAE_ASCII_LOWER_A) && (ch <= WAE_ASCII_LOWER_Z)) {
    /*lint -e{571} Suspicious cast*/
    return (NW_Ucs2)(ch - WAE_ASCII_LOWER_A + WAE_ASCII_UPPER_A);
  } else {
    return ch;
  }
}

NW_Uint32 NW_Asc_strlen(const char *s)
{
  NW_Uint32 i = 0;
  while (*s != '\0')
  {
    s++;
    i++;
  }
  return i;
}

char *NW_Asc_strcpy(char *s1, const char *s2)
{
  while (*s2 != '\0') {
    *s1 = *s2;
    s1++;
    s2++;
  }
  *s1 = *s2;
  return s1;
}

char *NW_Asc_strcat(char *s1, const char *s2)
{
  char *myDest = s1;
  while (*myDest != '\0') {
   myDest++;
  }
  NW_Asc_strcpy(myDest, s2);
  return s1;
}

NW_Int32 NW_Asc_Strchr(const char* str, char ch)
{
    NW_Int32 found = -1;
    NW_Uint32 count = 0;
    NW_ASSERT(str != NULL);
    while((*str != NULL) && (found == -1))
    {
        if(*str == ch)
        {
            found = count;
        }
        else
        {
            str++;
            count++;
        }
    }

    if(*str == ch)
    {
        found = count;
    }
    return found;
}

NW_Int32 NW_Asc_strcmp(const char *string1, const char *string2)
{
  NW_ASSERT(string1 != NULL);
  NW_ASSERT(string2 != NULL);

  while ((*string1 == *string2) &&
         (*string1 != '\0') && (*string2 != '\0'))
  {
    string1++;
    string2++;
  }
  return (*string1 - *string2);
}

NW_Int32 NW_Asc_stricmp(const char *string1, const char *string2)
{
  NW_ASSERT(string1 != NULL);
  NW_ASSERT(string2 != NULL);

  while ((NW_Asc_ToUpper(*string1) == NW_Asc_ToUpper(*string2)) &&
         (*string1 != WAE_ASCII_NULL) && (*string2 != WAE_ASCII_NULL))
  {
    string1++;
    string2++;
  }
  return (NW_Asc_ToUpper(*string1) - NW_Asc_ToUpper(*string2));
}

NW_Int32 NW_Asc_strnicmp(const char *string1, const char *string2,
                         const NW_Uint32 count)
{
  NW_Uint32 i = 1;
  
  NW_ASSERT(string1 != NULL);
  NW_ASSERT(string2 != NULL);

  while ((NW_Asc_ToUpper(*string1) == NW_Asc_ToUpper(*string2)) &&
         (*string1 != WAE_ASCII_NULL) && (*string2 != WAE_ASCII_NULL) &&
         (i < count))
  {
    string1++;
    string2++;
    i++;
  }
  if (count == 0) {
    return 0;
  } else {
    return (NW_Asc_ToUpper(*string1) - NW_Asc_ToUpper(*string2));
  }
}

char * NW_Asc_Itoa(const NW_Int32  i, char* s)
{
  NW_Int32 nDigits;
  NW_Int32 posVal = i;
  char *string = s;

  NW_ASSERT(string != NULL);

  if (i < 0) {
    *string = '-';
    string++;
    posVal = -i;
  }
  
  nDigits = posVal;
  do {
    nDigits /= 10;
    string++;
  } while (nDigits > 0);
  
  nDigits = posVal;
  *string = '\0';
  string--;
  do {
    *string = (char)((nDigits % 10) + '0');
    nDigits = nDigits / 10;
    string--;
  } while (nDigits > 0);
  return s;
}


NW_Ucs2* NW_Asc_toUcs2(char *string)
{
  NW_Int32 i, size;
  NW_Ucs2* res = NULL;

  NW_ASSERT(string);

  if (string != NULL)  {
    size = NW_Asc_strlen(string) + 1;
    res = (NW_Ucs2*) NW_Mem_Malloc(size * sizeof(NW_Ucs2));
    if (res != NULL) {
      NW_Mem_memset(res,0,(size*sizeof(NW_Ucs2)));
      for (i=0; i < size; i++) {
        res[i] = (char)(string[i]);
      }
    }
  }
  return res;
}

/*****************************************************************
**  Name:  NW_Str_Isalnum
**  Description:  Checks if the character is a particular 
**                representation of an alphanumeric character
**  Parameters:   ch  - character to test
**  Return Value: non-zero if c is an alphanumeric character, else 0
******************************************************************/
NW_Int32 NW_Str_Isalnum(const NW_Ucs2 ch)
{
  return ( NW_Str_Isalpha(ch) || NW_Str_Isdigit(ch) );
}

/*****************************************************************
**  Name:  NW_Str_Isalpha
**  Description:  Checks if the character is a particular 
**                representation of an alphabetic character
**  Parameters:   ch  - character to test
**  Return Value: non-zero if c is an alphabetic character, else 0
******************************************************************/
NW_Int32 NW_Str_Isalpha(const NW_Ucs2 ch)
{
  return ( ( (ch >= WAE_ASCII_LOWER_A) && (ch <= WAE_ASCII_LOWER_Z) ) ||
           ( (ch >= WAE_ASCII_UPPER_A) && (ch <= WAE_ASCII_UPPER_Z) ) );
}

/*****************************************************************
**  Name:  NW_Str_Isdigit
**  Description:  Checks if the character is a particular 
**                representation of a decimal-digit character.
**  Parameters:   ch  - character to test
**  Return Value: non-zero if c is a decimal-digit character, else 0
******************************************************************/
NW_Int32 NW_Str_Isdigit(const NW_Ucs2 ch)
{
  return ( (ch >= WAE_ASCII_0) && (ch <= WAE_ASCII_9) );
}

/*****************************************************************
**  Name:  NW_Str_Isxdigit
**  Description:  Checks if the character is a particular 
**                representation of a hexadecimal-digit character.
**  Parameters:   ch  - character to test
**  Return Value: non-zero if c is a hexadecimal-digit character, else 0
******************************************************************/
NW_Int32 NW_Str_Isxdigit(const NW_Ucs2 ch)
{
  return (NW_Str_Isdigit(ch) ||
          ( (ch >= WAE_ASCII_UPPER_A) && (ch <= WAE_ASCII_UPPER_F) ) ||
          ( (ch >= WAE_ASCII_LOWER_A) && (ch <= WAE_ASCII_LOWER_F) ) );
}

/*****************************************************************
**  Name:  NW_Str_Isspace
**  Description:  Checks if the character is a particular 
**                representation of a space character
**  Parameters:   ch  - character to test
**  Return Value: non-zero if c is a white-space character, else 0
******************************************************************/
NW_Int32 NW_Str_Isspace(const NW_Ucs2 ch)
{
  return ((ch == WAE_ASCII_SPACE) || (ch == WAE_ASCII_CR) ||
          (ch == WAE_ASCII_LF) || (ch == WAE_ASCII_FF) ||
          (ch == WAE_ASCII_HT) || (ch == WAE_ASCII_VT) ||
          (ch == WAE_ASCII_ZSPACE));
}

/*****************************************************************
**  Name:  NW_Str_Isnewline
**  Description:  Checks if the character is a newline
**  Parameters:   ch  - character to test
**  Return Value: non-zero if c is a linefeed character, else 0
******************************************************************/
NW_Int32 NW_Str_Isnewline(const NW_Ucs2 ch)
{
  return ((ch == WAE_ASCII_LF) || (ch == WAE_ASCII_VT) || (ch == WAE_ASCII_FF));
}

/*****************************************************************
**  Name:  NW_Str_Isbreak
**  Description:  Checks if the line can be broken at the
**                given character.  This will only be the case if
**                the character is white-space or a hyphen
**  Parameters:   ch  - character to test
**  Return Value: non-zero if c is a white-space character, else 0
******************************************************************/
NW_Int32 NW_Str_Isbreak(const NW_Ucs2 ch)
{
  return ((NW_Str_Isspace(ch)) || (ch == WAE_ASCII_HYPHEN) ||
          (ch == WAE_ASCII_SOFT_HYPHEN));
}

/*****************************************************************
**  Name:  NW_Str_Ispunct
**  Description:  Checks if the character is a particular 
**                representation of a punctuation character
**  Parameters:   ch  - character to test
**  Return Value: non-zero if c is a punctuation character, else 0
******************************************************************/
NW_Int32 NW_Str_Ispunct(const NW_Ucs2 ch)
{
  return ((ch > WAE_ASCII_SPACE) && !NW_Str_Isalnum(ch));
}

/*****************************************************************
**  Name:  NW_Str_Isascii
**  Description:  Checks if the character is a particular 
**                representation of a ascii (0x00 to 0x7F) character
**  Parameters:   ch  - character to test
**  Return Value: non-zero if c is a ascii character, else 0
******************************************************************/
NW_Int32 NW_Str_Isascii(const NW_Ucs2 ch)
{  
  /* unsigned character always greater than 0 */
  return (/*(ch >= WAE_ASCII_ANY_CHAR_BEGIN) && */(ch <= WAE_ASCII_ANY_CHAR_END));
}

/*****************************************************************
**  Name:  NW_Str_Iscntrl
**  Description:  Checks if the character is a particular 
**                representation of a control char (0x00 to 0x1F) character
**  Parameters:   ch  - character to test
**  Return Value: non-zero if c is a control char character, else 0
******************************************************************/
NW_Int32 NW_Str_Iscntrl(const NW_Ucs2 ch)
{
  return ((ch < WAE_ASCII_SPACE) && !NW_Str_Isspace(ch));
}

/*****************************************************************
**  Name:  NW_Str_Isupper
**  Description:  Checks if the character is a particular 
**                representation of an alphabetic character
**  Parameters:   ch  - character to test
**  Return Value: non-zero if c is an alphabetic character, else 0
******************************************************************/
NW_Int32 NW_Str_Isupper(const NW_Ucs2 ch)
{
  return ((ch >= WAE_ASCII_UPPER_A) && (ch <= WAE_ASCII_UPPER_Z));
}

/*****************************************************************
**  Name:  NW_Str_Islower
**  Description:  Checks if the character is a particular 
**                representation of an alphabetic character
**  Parameters:   ch  - character to test
**  Return Value: non-zero if c is an alphabetic character, else 0
******************************************************************/
NW_Int32 NW_Str_Islower(const NW_Ucs2 ch)
{
  return ((ch >= WAE_ASCII_LOWER_A) && (ch <= WAE_ASCII_LOWER_Z));
}

/*****************************************************************
**  Name:  NW_Str_New
**  Description:  Create a new string
**  Parameters:   len - length of string (excluding the terminal NULL)
**  Return Value: pointer to the new string or NULL
******************************************************************/
NW_Ucs2 *NW_Str_New(const NW_Uint32 len)
{
  NW_Ucs2 *tmp;

  /* Yes, sometimes we really *want* to allocate empty strings */

  tmp = (NW_Ucs2*) NW_Mem_Malloc((len + 1) * sizeof(NW_Ucs2));
  if (tmp != NULL) {
    tmp[0] = WAE_ASCII_NULL;
  }

  return tmp;
}

/*****************************************************************
**  Name:  NW_Str_Newcpy
**  Description:  Create a new copy of string
**  Parameters:   *string - pointer to the old string
**  Return Value: pointer to the new string or NULL
******************************************************************/
NW_Ucs2 *NW_Str_Newcpy(const NW_Ucs2 *string)
{
  NW_Uint32 len;
  NW_Ucs2 *tmp;

  NW_ASSERT(string != NULL);

  len = NW_Str_Strlen(string);
  tmp = (NW_Ucs2*) NW_Mem_Malloc((len + 1) * sizeof(NW_Ucs2));
  if (tmp != NULL) {
    NW_Str_Strcpy(tmp, string);
  }

  return tmp;
}

/*****************************************************************
**  Name:  NW_Str_Newcat
**  Description:  Create a new copy of two concatinated strings
**  Parameters:   *string1 - pointer to the 1st string
**                *string2 - pointer to the 2nd string
**  Return Value: pointer to the new string or NULL
******************************************************************/
NW_Ucs2 *NW_Str_Newcat(const NW_Ucs2 *string1, 
                                const NW_Ucs2 *string2)
{
  NW_Uint32 len1, len2;
  NW_Ucs2 *tmp;

  NW_ASSERT(string1 != NULL);
  NW_ASSERT(string2 != NULL);

  len1 = NW_Str_Strlen(string1);
  len2 = NW_Str_Strlen(string2);
  tmp = (NW_Ucs2*) NW_Mem_Malloc((len1 + len2 + 1) * sizeof(NW_Ucs2));
  if (tmp != NULL) {
    NW_Str_Strcpy(tmp, string1);
    NW_Str_Strcpy((tmp + len1), string2);
  }

  return tmp;
}

/*****************************************************************
**  Name:  NW_Str_SegNew
**  Description:  Create a new string
**  Parameters:   len - length of string (excluding the terminal NULL)
**  Return Value: pointer to the new string or NULL
******************************************************************/
NW_Ucs2 *NW_Str_SegNew(const NW_Uint32 len, 
                         const NW_Mem_Segment_Id_t sid)
{
  NW_Ucs2 *tmp;

  /* Yes, sometimes we really *want* to allocate empty strings */
  tmp = (NW_Ucs2 *)NW_Mem_Segment_Malloc((len + 1) * sizeof(NW_Ucs2), sid);
  if (tmp != NULL) {
    tmp[0] = WAE_ASCII_NULL;
  }

  return tmp;
}

/*****************************************************************
**  Name:  NW_Str_SegNewcpy
**  Description:  Create a new copy of string
**  Parameters:   *string - pointer to the old string
**  Return Value: pointer to the new string or NULL
******************************************************************/
NW_Ucs2 *NW_Str_SegNewcpy(const NW_Ucs2 *string, 
                            const NW_Mem_Segment_Id_t sid)
{
  NW_Uint32 len;
  NW_Ucs2 *tmp;

  NW_ASSERT(string != NULL);

  len = NW_Str_Strlen(string);
  tmp = (NW_Ucs2*) NW_Mem_Segment_Malloc((len + 1) * sizeof(NW_Ucs2), sid);
  if (tmp != NULL) {
    NW_Str_Strcpy(tmp, string);
  }

  return tmp;
}

/*****************************************************************
**  Name:  NW_Str_SegNewcat
**  Description:  Create a new copy of two concatinated strings
**  Parameters:   *string1 - pointer to the 1st string
**                *string2 - pointer to the 2nd string
**  Return Value: pointer to the new string or NULL
******************************************************************/
NW_Ucs2 *NW_Str_SegNewcat(const NW_Ucs2 *string1, 
                            const NW_Ucs2 *string2,
                            const NW_Mem_Segment_Id_t sid)
{
  NW_Uint32 len1, len2;
  NW_Ucs2 *tmp;

  NW_ASSERT(string1 != NULL);
  NW_ASSERT(string2 != NULL);

  len1 = NW_Str_Strlen(string1);
  len2 = NW_Str_Strlen(string2);
  tmp = (NW_Ucs2*) NW_Mem_Segment_Malloc((len1 + len2 + 1) * sizeof(NW_Ucs2), sid);
  if (tmp != NULL) {
    NW_Str_Strcpy(tmp, string1);
    NW_Str_Strcpy((tmp + len1), string2);
  }

  return tmp;
}

/*****************************************************************
**  Name:  NW_Str_Delete
**  Description:  Delete a string
**  Parameters:   *string - pointer to the string
**  Return Value: void
******************************************************************/
void NW_Str_Delete(NW_Ucs2 *string)
{
  if (string != NULL) {
    NW_Mem_Free(string);
  }
  return;
}

/*****************************************************************
**  Name:  NW_Str_Strlen
**  Description:  Get the length of a string
**  Parameters:   *string - Null-terminated string
**  Return Value: Returns the number of characters in string, 
**                excluding the terminal NULL.
******************************************************************/
NW_Uint32 NW_Str_Strlen(const NW_Ucs2 *string)
{
  NW_Uint32 len = 0;
  NW_ASSERT(string != NULL);

  while (*string != WAE_ASCII_NULL) {
    string++;
    len++;
  }
  return len;
}

/*****************************************************************
**  Name:  NW_Str_Strsize
**  Description:  Get the size of the string
**  Parameters:   *string - Null-terminated string
**  Return Value: number of bytes in string including terminator
******************************************************************/
NW_Uint32 NW_Str_Strsize(const NW_Ucs2 *string)
{
  NW_ASSERT(string != NULL);

  /*lint -e{666} Expression with side effects passed to repeated parameter in macro*/
  return NW_UINT32_CAST((NW_Str_Strlen(string) + 1) * sizeof(NW_Ucs2));
}

/*****************************************************************
**  Name:  NW_Str_Strcpy
**  Description:  Copy a string
**  Parameters:   *destination - Destination string
**                *source - Source string
**  Return Value: pointer to the destination string 
******************************************************************/
NW_Ucs2 *NW_Str_Strcpy(NW_Ucs2 *destination, 
                         const NW_Ucs2 *source)
{
  NW_Ucs2 *walker = destination;
  NW_ASSERT(destination != NULL);
  NW_ASSERT(source != NULL);

  while (*source != WAE_ASCII_NULL) {
    *walker++ = *source++;
  }
  *walker = *source;
  return destination;
}

/*****************************************************************
**  Name:  NW_Str_Strcat
**  Description:  Concatinate source string to destination string
**  Parameters:   *destination - Destination string
**                *source - Source string
**  Return Value: pointer to destination string 
******************************************************************/
NW_Ucs2 *NW_Str_Strcat(NW_Ucs2 *destination, 
                         const NW_Ucs2 *source)
{
  NW_ASSERT(destination != NULL);
  NW_ASSERT(source != NULL);

  while (*destination != WAE_ASCII_NULL) {
    destination++;
  }
  return NW_Str_Strcpy(destination, source);
}

/*****************************************************************
**  Name:  NW_Str_Strncpy
**  Description:  Copy characters of one string to another
**  Parameters:   *destination - Destination string
**                *source - Source string
**                count -  Maximum number of characters to be copied
**  Return Value: pointer to destination string 
******************************************************************/
NW_Ucs2 *NW_Str_Strncpy(NW_Ucs2 *destination,
                          const NW_Ucs2 *source,
                          const NW_Uint32 count )
{
  NW_Uint32 nCopied = 0;
  
  NW_ASSERT(destination != NULL);
  NW_ASSERT(source != NULL);

  while ( (nCopied < count) && (*source != WAE_ASCII_NULL) ) {
    *destination = *source;
    destination++;
    source++;
    nCopied++;
  }
  /*
  ** ANSI 7.11.2.4 says nulls are appended till count characters in all have
  ** been copied
  */
  while (nCopied < count) {
    *destination = WAE_ASCII_NULL;
    destination++;
    nCopied++;
  }
  return destination;
}

/*****************************************************************
**  Name:  NW_Str_Strchr
**  Description:  Find first occurrence of a character in a string
**  Parameters:   *string - Null-terminated source string
**                ch -  Character to be located
**  Return Value: pointer to the occurrence or NULL if not found
******************************************************************/
NW_Ucs2 *NW_Str_Strchr(const NW_Ucs2 *string, 
                         const NW_Ucs2 ch)
{
  const NW_Ucs2 *found = NULL;
  NW_ASSERT(string != NULL);

  while ((*string != WAE_ASCII_NULL) && (found == NULL)) {
    if (*string == ch) {
      found = string;
    } else {
      string++;
    }
  }
  /* handle ch == 0 */
  if (*string == ch) {
      found = string;
  }
  return (NW_Ucs2 *)found;
}

/*****************************************************************
**  Name:  NW_Str_Strrchr
**  Description:  Find the last occurrence of a character in a string
**  Parameters:   *string - Null-terminated source string
**                ch -  Character to be located
**  Return Value: pointer to the occurrence or NULL if not found
******************************************************************/
NW_Ucs2 *NW_Str_Strrchr(const NW_Ucs2 *string,
                          const NW_Ucs2 ch)
{
  const NW_Ucs2 *found = NULL;
  const NW_Ucs2 *walker = NULL;
  NW_ASSERT(string != NULL);

  walker = string + NW_Str_Strlen(string);
  while ( (walker >= string) && (found == NULL) ) {
    if (*walker == ch) {
      found = walker;
    } else {
      walker--;
    }
  }
  return (NW_Ucs2 *)found;
}

/*****************************************************************
**  Name:  NW_Str_Strcspn
**  Description:  Find the first occurrence of a character set in a string
**  Parameters:   *string - Null-terminated source string
**                *search -  Character set to be located
**  Return Value: index of the character found
******************************************************************/
NW_Uint32 NW_Str_Strcspn(const NW_Ucs2 *string,
                      const NW_Ucs2 *search)
{
  NW_Uint32 count = 0;
  
  NW_ASSERT(string != NULL);
  NW_ASSERT(search != NULL);

  while ((*string != WAE_ASCII_NULL) &&
         NW_Str_Strchr(search, *string))
  {
    count++;
    string++;
  }
  return count;
}

/*****************************************************************
**  Name:  NW_Str_Strstr
**  Description:  Find a substring within a string
**  Parameters:   *string - Null-terminated source string
**                *search -  Null-terminated string to search for
**  Return Value: pointer to start of substring or NULL if not found
******************************************************************/
NW_Ucs2 *NW_Str_Strstr(const NW_Ucs2 *string,
                         const NW_Ucs2 *search)
{
  NW_Uint32 searchLen = NW_Str_Strlen(search);
  
  NW_ASSERT(string != NULL);
  NW_ASSERT(search != NULL);

  if (*search == WAE_ASCII_NULL) {
    return (NW_Ucs2 *)string;
  }
  while ((*string != WAE_ASCII_NULL) &&
         NW_Str_Strncmp(string, search, searchLen))
  {
    string++;
  }
  if (*string != WAE_ASCII_NULL) {
    return (NW_Ucs2 *)string;
  }
  return NULL;
}

/*****************************************************************
**  Name:  NW_Str_Strstrtil
**  Description:  Find a substring within a string ended by terminator
**  Parameters:   *string - Null-terminated source string
**                *sub_terminator
**  Return Value: substring
******************************************************************/
NW_Ucs2* NW_Str_Strstrtil(const NW_Ucs2 *string, char sub_terminator){

  const NW_Ucs2 *pStart = NULL;
  const NW_Ucs2 *pEnd   = NULL;
  
  NW_ASSERT(string != NULL);

  pStart = string;
 
  pEnd = NW_Str_Strchr(pStart, sub_terminator);

  if (pEnd != NULL) {
    return NW_Str_Substr(pStart, 0, (pEnd - pStart));
  }
  else {
    return NULL;
  }
}

/*****************************************************************
**  Name:  NW_Str_Strcmp
**  Description:  Compare two strings
**  Parameters:   *string1 - Null-terminated string to compare
**                *string2 - Null-terminated string to compare
**  Return Value: <0 if string1 < string2, 0 if equal, >0 if string1 > string2
******************************************************************/
NW_Int32 NW_Str_Strcmp(const NW_Ucs2 *string1, 
                    const NW_Ucs2 *string2)
{
  NW_ASSERT(string1 != NULL);
  NW_ASSERT(string2 != NULL);

  while ((*string1 == *string2) &&
         (*string1 != WAE_ASCII_NULL) && (*string2 != WAE_ASCII_NULL))
  {
    string1++;
    string2++;
  }
  return (*string1 - *string2);
}

/*****************************************************************
**  Name:  NW_Str_Stricmp
**  Description:  Compare two strings, case-insensitve
**  Parameters:   *string1 - Null-terminated string to compare
**                *string2 - Null-terminated string to compare
**  Return Value: <0 if string1 < string2, 0 if equal, >0 if string1 > string2
******************************************************************/
NW_Int32 NW_Str_Stricmp(const NW_Ucs2 *string1, 
                     const NW_Ucs2 *string2)
{
  NW_ASSERT(string1 != NULL);
  NW_ASSERT(string2 != NULL);


  while ((NW_Str_ToUpper(*string1) == NW_Str_ToUpper(*string2)) &&
         (*string1 != WAE_ASCII_NULL) && (*string2 != WAE_ASCII_NULL))
  {
    string1++;
    string2++;
  }
  return (NW_Str_ToUpper(*string1) - NW_Str_ToUpper(*string2));
}

/*****************************************************************
**  Name:  NW_Str_Strncmp
**  Description:  Compare first "count" characters of two strings
**  Parameters:   *string1 - Null-terminated string to compare
**                *string2 - Null-terminated string to compare
**                count - Maximum number of characters to compare
**  Return Value: <0 if str1 < str2, 0 if str1 == str2, or >0 if str1 > str2
******************************************************************/
NW_Int32 NW_Str_Strncmp(const NW_Ucs2 *string1, 
                     const NW_Ucs2 *string2,
                     const NW_Uint32 count)
{
  NW_Uint32 i = 1;
  
  NW_ASSERT(string1 != NULL);
  NW_ASSERT(string2 != NULL);

  while ((*string1 == *string2) &&
         (*string1 != WAE_ASCII_NULL) && (*string2 != WAE_ASCII_NULL) &&
         (i < count))
  {
    string1++;
    string2++;
    i++;
  }
  if (count == 0) {
    return 0;
  } else {
    return (*string1 - *string2);
  }
}

/*****************************************************************
**  Name:  NW_Str_Strnicmp
**  Description:  Compare first "count" characters of two strings, case-insensitive
**  Parameters:   *string1 - Null-terminated string to compare
**                *string2 - Null-terminated string to compare
**                count - Maximum number of characters to compare
**  Return Value: <0 if str1 < str2, 0 if str1 == str2, or >0 if str1 > str2
******************************************************************/
NW_Int32 NW_Str_Strnicmp(const NW_Ucs2 *string1, 
                      const NW_Ucs2 *string2,
                      const NW_Uint32 count)
{
  NW_Uint32 i = 1;
  
  NW_ASSERT(string1 != NULL);
  NW_ASSERT(string2 != NULL);

  while ((NW_Str_ToUpper(*string1) == NW_Str_ToUpper(*string2)) &&
         (*string1 != WAE_ASCII_NULL) && (*string2 != WAE_ASCII_NULL) &&
         (i < count))
  {
    string1++;
    string2++;
    i++;
  }
  if (count == 0) {
    return 0;
  } else {
    return (NW_Str_ToUpper(*string1) - NW_Str_ToUpper(*string2));
  }
}

/*****************************************************************
**  Name:  NW_Str_Hash
**  Description:  Calculate a 32-bit hash code for a string 
**  Parameters:   *string - Null-terminated string to hash
**  Return Value: integer value
******************************************************************/
NW_Uint32 NW_Str_Hash(const NW_Ucs2 *string)
{
  /* Psuedo-random numbers from Excel */
  static const NW_Uint32 hashtable[16] = {
    0x14FDDB16, 0x441E357C, 0x71036490, 0xC5428810, 
    0xBF776505, 0x7D781343, 0x9D949D3A, 0x846D619D, 
    0x8E53570C, 0x494C0336, 0x1F30DB1B, 0xCB5F2D5F, 
    0x38692DEB, 0x51202F39, 0x75CD2E06, 0x7678235A
  };

  static const NW_Uint32 highbit[2] = {0, 0x80000000};

  NW_Uint16 i;
  NW_Uint32 ch;
  NW_Uint32 hashcode = 0;

  NW_ASSERT(string != NULL);

  do {
    ch = *string;

    /* XOR pseudo-random number and then rotate bits */
    for (i = 0; i < 4; i++) {
      hashcode = hashcode ^ hashtable[ch & 0x0F];
      hashcode = (hashcode >> 1) | highbit[hashcode & 0x01];
      ch = ch >> 4;
    }

  } while ((*string++) != 0);

  return hashcode;
}

/*****************************************************************
**  Name:  NW_Str_Atoi
**  Description:  Converts a string to an integer 
**  Parameters:   *string - Null-terminated string to convert
**  Return Value: integer value
******************************************************************/
NW_Int32  NW_Str_Atoi(const NW_Ucs2 *string)
{
  NW_Ucs2 *endPtr;
  NW_ASSERT(string != NULL);

  return NW_Str_Strtol(string, &endPtr, 10);
}

NW_Ucs2 *NW_Str_ItoBase(const NW_Int32 i, NW_Ucs2* s, const NW_Int32 base)
{
  NW_Int32 nDigits;
  NW_Int32 posVal = i;
  NW_Ucs2 *string = s;
  NW_ASSERT(string != NULL);

  if(i == 0)
  {
    string++;
    *string = WAE_ASCII_NULL;
    string--;
    *string = (NW_Ucs2)WAE_ASCII_0;
    return s;
  }

  if (i < 0) {
    *string = '-';
    string++;
    posVal = -i;
  }
  nDigits = posVal;
  while (nDigits > 0) {
    nDigits /= base;
    string++;
  }
  nDigits = posVal;
  *string = WAE_ASCII_NULL;
  string--;
  while (nDigits > 0) {
    NW_Int32 numeral = nDigits % base;
    if (numeral < 10) {
      *string = (NW_Ucs2)(numeral + WAE_ASCII_0);
    } else {
      *string = (NW_Ucs2)(numeral - 10 + WAE_ASCII_LOWER_A);
    }
    nDigits = nDigits / base;
    string--;
  }
  return s;
}

/*****************************************************************
**  Name:  NW_Str_Itoa
**  Description:  Convert a decimal integer to a string
**  Parameters:   i - Integer to be converted
**                *string - pointer to string
**  Return Value: pointer to the converted string
******************************************************************/
NW_Ucs2 *NW_Str_Itoa(const NW_Int32  i, NW_Ucs2* string)
{
  return NW_Str_ItoBase(i, string, 10);
}

NW_Ucs2 *NW_Str_UltoBase(const NW_Int32 i, NW_Ucs2* s, const NW_Int32 base)
{
  NW_Int32 nDigits;
  NW_Ucs2 *string = s;

  NW_ASSERT(string != NULL);

  nDigits = i;
  do {
    nDigits /= base;
    string++;
  } while (nDigits != 0);

  nDigits = i;
  *string = WAE_ASCII_NULL;
  string--;
  do {
    NW_Int32 numeral = nDigits % base;
    if (numeral < 10) {
      *string = (NW_Ucs2)(numeral + WAE_ASCII_0);
    } else {
      *string = (NW_Ucs2)(numeral - 10 + WAE_ASCII_LOWER_A);
    }
    nDigits /= base;
    string--;
  } while (nDigits != 0);
  return s;
}

/*****************************************************************
**  Name:  NW_Str_Ultoa
**  Description:  Convert an NW_Uint32  integer to an NW string
**  Parameters:   i - Integer to be converted
**                *string - pointer to string
**  Return Value: pointer to the converted string
******************************************************************/
NW_Ucs2 *NW_Str_Ultoa(const NW_Uint32 i, NW_Ucs2* string)
{
  return NW_Str_UltoBase((NW_Int32)i, string, 10);
}


/*****************************************************************
**  Name:  NW_Str_Strtol
**  Description:  Converts a string to an integer 
**  Parameters:   *ptr - Null-terminated string to convert
**                **endPtr - returned pointer to end of converted string
**                base - radix for conversion
**  Return Value: integer value
******************************************************************/
NW_Int32 NW_Str_Strtol(const NW_Ucs2 *ptr, NW_Ucs2 **endPtr, NW_Int32  base) 
{
  NW_Int32 result;
  NW_Int32 prevResult;
  NW_Int32 currDigit;
  NW_Int32 sign = 1;
  
  const NW_Ucs2 *myEnd;
  NW_ASSERT(ptr != NULL);
  NW_ASSERT(endPtr != NULL);

  while ((*ptr != WAE_ASCII_NULL) && NW_Str_Isspace(*ptr)) {
    ptr++;
  }

  if (*ptr == WAE_ASCII_PLUS) {
    ptr++;
  }
  if (*ptr == WAE_ASCII_MINUS) {
    ptr++;
    sign = -1;
  }
  myEnd = ptr;
  result = 0;
  currDigit = 0;
  
  do {
    prevResult = result;
    result = result*base + currDigit;
    if (result < prevResult) {
      *endPtr = NULL;
      return 0;
    }
    if ((*myEnd >= WAE_ASCII_0) && (*myEnd <= WAE_ASCII_9)) {
      currDigit = (*myEnd - WAE_ASCII_0);
      myEnd++;
    } else if ((base > 10) && NW_Str_Isalpha(*myEnd)) {
      currDigit = NW_Str_ToUpper(*myEnd) - WAE_ASCII_UPPER_A + 10;
      myEnd++;
    } else {
      currDigit = base;
    }
  } while ( currDigit < base );
  *endPtr = (NW_Ucs2 *)myEnd;
  return sign*result;
}

/*****************************************************************
**  Name:  NW_Str_Strtoul
**  Description:  Converts a string to an unsigned integer 
**  Parameters:   *ptr - Null-terminated string to convert
**                **endPtr - returned pointer to end of converted string
**                base - radix for conversion
**  Return Value: integer value
******************************************************************/
NW_Int32  NW_Str_Strtoul(const NW_Ucs2 *ptr, NW_Ucs2 **endPtr, NW_Int32  base) 
{
  NW_Int32 result;
  NW_Int32 prevResult;
  NW_Int32 currDigit;

  const NW_Ucs2 *myEnd;
  
  NW_ASSERT(ptr != NULL);
  NW_ASSERT(endPtr != NULL);

  while ((*ptr != WAE_ASCII_NULL) && NW_Str_Isspace(*ptr)) {
    ptr++;
  }

  if (*ptr == WAE_ASCII_PLUS) {
    ptr++;
  }
  myEnd = ptr;
  result = 0;
  currDigit = 0;
  
  do {
    prevResult = result;
    result = result*base + currDigit;
    if (result < prevResult) {
      *endPtr = NULL;
      return 0;
    }
    if ((*myEnd >= WAE_ASCII_0) && (*myEnd <= WAE_ASCII_9)) {
      currDigit = (*myEnd - WAE_ASCII_0);
      myEnd++;
    } else if ((base > 10) && NW_Str_Isalpha(*myEnd)) {
      currDigit = NW_Str_ToUpper(*myEnd) - WAE_ASCII_UPPER_A + 10;
      myEnd++;
    } else {
      currDigit = base;
    }
  } while ( currDigit < base );
  *endPtr = (NW_Ucs2 *)myEnd;
  return result;
}

/*****************************************************************
**  Name:  NW_Str_CvtToInt32
**  Description:  Converts a string to an integer value
**                and checks for overflow
**  Parameters:   *ptr - Null-terminated string to convert
**                **endPtr - returned pointer to end of converted string
**                base - radix for conversion
**                *value - integer value returned
**  Return Value: KBrsrSuccess or KBrsrBadInputParam
******************************************************************/
TBrowserStatusCode NW_Str_CvtToInt32(const NW_Ucs2 *string, NW_Ucs2 **endptr, 
                             NW_Int32  base, NW_Int32 *value)
{
  NW_ASSERT(string != NULL);
  NW_ASSERT(endptr != NULL);

  *endptr = NULL;
  *value  = NW_Str_Strtol(string, endptr, base);

  if (*endptr == NULL) {
    return KBrsrBadInputParam;
  } else {
    return KBrsrSuccess;
  }
}

/*****************************************************************
**  Name:  NW_Str_CvtToInt32
**  Description:  Converts a string to a floating-point value
**                and checks for overflow
**  Parameters:   *ptr - Null-terminated string to convert
**                **endPtr - returned pointer to end of converted string
**                *ret_double - float value returned
**  Return Value: KBrsrSuccess or KBrsrBadInputParam
******************************************************************/
TBrowserStatusCode NW_Str_CvtToFloat32(const NW_Ucs2 *string, NW_Ucs2 **end, 
                                NW_Float32 *ret_double)

{
  const NW_Ucs2* ptr;
  const NW_Ucs2* start = string;
  NW_Ucs2 c;
  NW_Float64 intPart = 0.0;
  NW_Float64 fracPart = 0.0;
  NW_Float32 expPart = 1.0;
  NW_Int32   sign = 1;
  NW_Int32   expVal = 0;
  NW_Float64 res64;
  
  NW_ASSERT(string);
  NW_ASSERT(end);
  NW_ASSERT(ret_double);

  /* skipping white spaces */
  while ((*start != 0) && (*start <= 32)) {
    start++;
  }

  ptr = start;
  if (*ptr == 0) {
    return KBrsrBadInputParam;               /* reach the end of the string */
  }

  c = *ptr;
  if ((c != '+') && (c != '-') && (c != '.') && !NW_Str_Isdigit(c)) {
    return KBrsrBadInputParam;            
  }

  if (c != '.') {
    /* read NW_Int32  part */
    if (c == '-') {
      sign = -1;
    } else if (NW_Str_Isdigit(c)) {
      intPart = (NW_Float32)(c - '0');
    }
    ptr++;
    while ((*ptr != 0) && NW_Str_Isdigit(*ptr)) {
      if (is_finite_float64(intPart)) {
        intPart = (intPart * 10) + (*ptr - '0');
      }
      ptr++;
    }
    if (is_finite_float64(intPart)) {
      intPart *= sign;
    } else {
      return KBrsrBadInputParam;            
    }

  }

  if ((*ptr != 0) && (*ptr == '.')) {
    /* read fraction part */
    ptr++;
    expPart = 0.1f;
    while ((*ptr != 0) && NW_Str_Isdigit(*ptr)) {
      /*lint -e{747} Significant prototype coercion*/
      if (! is_zero_float64(expPart)) {
        fracPart = fracPart + expPart*(*ptr - '0');
        expPart *= 0.1f;
      }
      ptr++;
    }
    fracPart *= sign;
  }

  if ((*ptr != 0) && ((*ptr == 'e') || (*ptr == 'E'))) {
    /* if we have xxx[e|E] we must have exponent of some kind */
    ptr++;
    if (*ptr == 0) {
      return KBrsrBadInputParam;               
    }
    c = *ptr;
    /* exp must be e[+|-]nn */
    if ((c != '+') && (c != '-') && !NW_Str_Isdigit(c)) {
      return KBrsrBadInputParam;               
    }

    ptr++;
    sign = 1;
    /* if exp is e[+|-] or e[+|-]xx where xx is not a number, error */
    if ((c == '+') || (c == '-')) {
      if ((*ptr == 0) || !NW_Str_Isdigit(*ptr)) {
        return KBrsrBadInputParam;               
      } else if (c == '-') {
        sign = -1;
      }
    } else {
      expVal = c - '0';
    }

    /* Get exponent digits. */
    while ((*ptr != 0) && NW_Str_Isdigit(*ptr)) {
      /* if we can't hold exponent we've got over/underflow */
      if ((expVal > NW_FLOAT32_MAX_EXP) && (sign > 0)) {
        return KBrsrBadInputParam;  /* overflow */
      } else {
        /* if exponent is neg, spec says underflow -> 0, so */
        /* we need to eat all chars in exp, but want expVal to */
        /* remain a big, negative, number */
        if (expVal >= 0) {
          if (expVal < 2 * NW_FLOAT32_MAX_EXP) {
            expVal = (expVal * 10) + (*ptr - '0');
          }
        }
      }
      ptr++;
    }
    if (expVal > 0) {
      expVal *= sign;
    }
  }
  /* End exponent part. */
  *end = (NW_Ucs2*)ptr;

  /* explicit check for just sign char, i.e. '+' or '-' */
  if ( (start == *end) ||
       ((start + 1 == *end) && !NW_Str_Isdigit(*start)) )
  {
    return KBrsrBadInputParam;               
  }

  /* The calculations will be done with doubles, so allow double
   * the exponents for the time being.
   */
  if (expVal < NW_FLOAT32_MIN_EXP * 2) {
    *ret_double = 0.0f;
    return KBrsrSuccess;
  } else if (expVal > NW_FLOAT32_MAX_EXP * 2) {
    return KBrsrBadInputParam;               
  } else {
    res64 = (intPart + fracPart)*NW_Math_powint64(10.0f, expVal);
    if (is_zero_float64(res64)) {
      *ret_double = 0.0f;
      return KBrsrSuccess;
    } else if (is_finite_float64(res64)) {
      *ret_double = (NW_Float32)res64;
      return KBrsrSuccess;
    } else {
      return KBrsrBadInputParam;               
    }
  }
}

/*****************************************************************
**  Name:  NW_Str_Inc
**  Description:  Increments the string pointer by count
**  Parameters:   *string - pointer to null-terminated string
**                count  - Amount to increment
**  Return Value: pointer to the new location or NULL
******************************************************************/
NW_Ucs2 *NW_Str_Inc(const NW_Ucs2 *string, const NW_Int32 count)
{
  NW_Int32 len;

  NW_ASSERT(string != NULL);

  len = NW_Str_Strlen(string);
  if(count > len)
    return NULL;
  
  return (NW_Ucs2 *)(string + count);
}

/*****************************************************************
**  Name:  NW_Str_CvtToAscii
**  Description:  Convert ucs2 string to new null-terminated ascii string
**  Parameters:   *string - Null-terminated ucs2 string
**  Return Value: pointer to the new ascii string or NULL
******************************************************************/
char *NW_Str_CvtToAscii(const NW_Ucs2 *string)
{
  NW_Int32 i, size;
  char* res = NULL;

  NW_ASSERT(string);

  size = NW_Str_Strlen(string) + 1;
  res = (char*) NW_Mem_Malloc((NW_Uint32)size * sizeof(char));
  /* this is a really naive conversion to ascii */
  if (res != NULL) {
    for (i=0; i < size; i++) {
      res[i] = (char)(string[i] & 0xFF);
    }
  }
  return res;
}

/*****************************************************************
**  Name:  NW_Str_CvtFromAscii
**  Description:  Convert null-terminated ascii string to new ucs2 string
**  Parameters:   *string - Null-terminated ascii string
**  Return Value: pointer to new ucs2 string or NULL
******************************************************************/
NW_Ucs2 *NW_Str_CvtFromAscii(const char *string)
{
  NW_Uint32 i, size;
  NW_Ucs2* res = NULL;

  NW_ASSERT(string);

  size = NW_Asc_strlen(string) + 1;
  res = (NW_Ucs2*) NW_Mem_Malloc(size * sizeof(NW_Ucs2));
  /* this is a really naive conversion from ascii */
  if (res != NULL) {
    for (i=0; i < size; i++) {
      res[i] = string[i];
    }
  }
  return res;
}

/*****************************************************************
**  Name:  NW_Str_ToLower
**  Description:  Converts a NW_Ucs2 to lower case
**  Parameters:   ch - the character to convert
**  Return Value: the converted character
******************************************************************/
NW_Ucs2 NW_Str_ToLower(const NW_Ucs2 ch)
{
  if((ch >= WAE_ASCII_UPPER_A) && (ch <= WAE_ASCII_UPPER_Z)) {
    return (NW_Ucs2)(ch - WAE_ASCII_UPPER_A + WAE_ASCII_LOWER_A);
  } else {
    return ch;
  }
}

/*****************************************************************
**  Name:  NW_Str_ToUpper
**  Description:  Converts a NW_Ucs2 to upper case
**  Parameters:   ch - the character to convert
**  Return Value: the converted character
******************************************************************/
NW_Ucs2 NW_Str_ToUpper(const NW_Ucs2 ch)
{
  if((ch >= WAE_ASCII_LOWER_A) && (ch <= WAE_ASCII_LOWER_Z)) {
    return (NW_Ucs2)(ch - WAE_ASCII_LOWER_A + WAE_ASCII_UPPER_A);
  } else {
    return ch;
  }
}

/*****************************************************************
**  Name:  NW_Str_Strlwr
**  Description:  Converts a string lower case
**  Parameters:   *string - the string to convert
**  Return Value: void
******************************************************************/
void NW_Str_Strlwr(NW_Ucs2 *string)
{
  NW_ASSERT(string);
  while (*string != WAE_ASCII_NULL) {
    *string = NW_Str_ToLower(*string);
    string++;
  }
  return;
}

/*****************************************************************
**  Name:  NW_Str_Strupr
**  Description:  Converts a string upper case
**  Parameters:   *string - the string to convert
**  Return Value: void
******************************************************************/
void NW_Str_Strupr(NW_Ucs2 *string)
{
  NW_ASSERT(string);
  while (*string != WAE_ASCII_NULL) {
    *string = NW_Str_ToUpper(*string);
    string++;
  }
  return;
}

/*****************************************************************
**  Name:  NW_Str_Substr
**  Description:  Returns a new substring
**  Parameters:   *string - the source string
**                start - index of the start of the substring
**                len - length of the substring
**  Return Value: pointer to the new substring or NULL
******************************************************************/
NW_Ucs2 *NW_Str_Substr(const NW_Ucs2 *string, 
                         const NW_Uint32 start, const NW_Uint32 len)
{
  NW_Ucs2 *tmp = NULL;

  NW_ASSERT(string);

  if (NW_Str_Strlen(string) < (start + len)) {
    return NULL;
  }
  
  /* Yes, sometimes we really *want* to allocate empty strings */
  tmp = (NW_Ucs2*) NW_Mem_Malloc((len + 1) * sizeof(NW_Ucs2));
  if (tmp != NULL) {
    NW_Str_Strncpy(tmp, (string + start), len);
    tmp[len] = WAE_ASCII_NULL;
  }
  return tmp;
}

/*****************************************************************
**  Name:  NW_Str_SegSubstr
**  Description:  Returns a new substring
**  Parameters:   *string - the source string
**                start - index of the start of the substring
**                len - length of the substring
**  Return Value: pointer to the new substring or NULL
******************************************************************/
NW_Ucs2 *NW_Str_SegSubstr(const NW_Ucs2 *string, 
                            const NW_Uint32 start, const NW_Uint32 len,
                            const NW_Mem_Segment_Id_t sid)
{
  NW_Ucs2 *tmp = NULL;

  NW_ASSERT(string);
  NW_ASSERT(NW_Str_Strlen(string) >= (start + len));

  /* Yes, sometimes we really *want* to allocate empty strings */
  tmp = (NW_Ucs2*) NW_Mem_Segment_Malloc((len + 1) * sizeof(NW_Ucs2), sid);
  if (tmp != NULL) {
    NW_Str_Strncpy(tmp, (string + start), len);
    tmp[len] = WAE_ASCII_NULL;
  }
  return tmp;
}

/*****************************************************************
**  Name:  NW_Str_NewcpyConst
**  Description:  Create a new copy of a string
**  Parameters:   *string - pointer to the old string
**  Return Value: pointer to the new string or NULL
******************************************************************/
NW_Ucs2 *NW_Str_NewcpyConst(const char* const string)
{
  NW_Uint32 i = 0;
  NW_Uint32 len;
  NW_Ucs2 *tmp;

  NW_ASSERT(string != NULL);

  len = NW_Asc_strlen(string);
  tmp = (NW_Ucs2*) NW_Mem_Malloc((len + 1) * sizeof(NW_Ucs2));
  if (tmp != NULL) {
    for (i = 0; i <= len; i++) {
      tmp[i] = string[i];
    }
  }

  return tmp;
}

/*****************************************************************
**  Name:  NW_Str_StrcpyConst
**  Description:  Copy a string
**  Parameters:   *destination - Destination string
**                *source - Source Ascii string constant
**  Return Value: pointer to the destination string 
******************************************************************/
NW_Ucs2 *NW_Str_StrcpyConst(NW_Ucs2 *destination, 
                              const char* const source)
{
  NW_Int32 i = 0;
  char ch;

  NW_ASSERT(destination != NULL);
  NW_ASSERT(source != NULL);

  do {
    ch = source[i];
    destination[i] = ch;
    i++;
  } while (ch != WAE_ASCII_NULL);

  return destination;
}

/*****************************************************************
**  Name:  NW_Str_StrcatConst
**  Description:  Concatinate source string to destination string
**  Parameters:   *destination - Destination string
**                *source - Source Ascii string constant
**  Return Value: pointer to destination string 
******************************************************************/
NW_Ucs2 *NW_Str_StrcatConst(NW_Ucs2 *destination, 
                              const char* const source)
{
  NW_Uint32 i = 0;
  NW_Uint32 j = 0;
  char ch;

  NW_ASSERT(destination != NULL);
  NW_ASSERT(source != NULL);

  while (destination[i] != WAE_ASCII_NULL) {
    i++;
  }

  do {
    ch = source[j];
    destination[i] = ch;
    i++;
    j++;
  } while (ch != WAE_ASCII_NULL);

  return destination;
}

/*****************************************************************
**  Name:  NW_Str_StrcmpConst
**  Description:  Compare a string to an Ascii string constant
**  Parameters:   *string1 - Null-terminated string to compare
**                *string2 - Null-terminated string to compare
**  Return Value: <0 if string1 < string2, 0 if equal, >0 if string1 > string2
******************************************************************/
NW_Int32 NW_Str_StrcmpConst(const NW_Ucs2 *string1, 
                         const char* const string2)
{
  NW_Int32 i = 0;
  NW_Int32 ch1;
  NW_Int32 ch2;

  NW_ASSERT(string1 != NULL);
  NW_ASSERT(string2 != NULL);

  do {
    ch1 = string1[i];
    ch2 = string2[i];
    i++;
  } while (((ch1 - ch2) == 0) && (ch1 != WAE_ASCII_NULL));

  return (ch1 - ch2);
}

/*****************************************************************
**  Name:  NW_Str_StricmpConst
**  Description:  Compare a string to an Ascii string constant, case-insensitve
**  Parameters:   *string1 - Null-terminated string to compare
**                *string2 - Null-terminated string to compare
**  Return Value: <0 if string1 < string2, 0 if equal, >0 if string1 > string2
******************************************************************/
NW_Int32 NW_Str_StricmpConst(const NW_Ucs2 *string1,
                          const char* const string2)
{
  NW_Int32 i = 0;
  NW_Int32 ch1;
  NW_Int32 ch2;

  NW_ASSERT(string1 != NULL);
  NW_ASSERT(string2 != NULL);

  do {
    ch1 = NW_Str_ToUpper(string1[i]);
    ch2 = NW_Str_ToUpper(string2[i]);
    i++;
  } while (((ch1 - ch2) == 0) && (ch1 != WAE_ASCII_NULL));

  return (ch1 - ch2);
}

/*****************************************************************
**  Name:  NW_Str_StrncmpConst
**  Description:  Compare first "count" characters of a string to 
**                an Ascii string constant
**  Parameters:   *string1 - Null-terminated string to compare
**                *string2 - Null-terminated string to compare
**                count - Maximum number of characters to compare
**  Return Value: <0 if str1 < str2, 0 if str1 == str2, or >0 if str1 > str2
******************************************************************/
NW_Int32 NW_Str_StrncmpConst(const NW_Ucs2 *string1,
                          const char* const string2,
                          const NW_Uint32 count)
{
  NW_Uint32 i = 0;
  NW_Int32 ch1;
  NW_Int32 ch2;

  NW_ASSERT(string1 != NULL);
  NW_ASSERT(string2 != NULL);

  do {
    ch1 = string1[i];
    ch2 = string2[i];
    i++;
  } while (((ch1 - ch2) == 0) && (ch1 != WAE_ASCII_NULL) && (i < count));

  return (ch1 - ch2);
}

/*****************************************************************
**  Name:  NW_Str_StrnicmpConst
**  Description:  Compare first "count" characters of a string to 
**                an Ascii string constant, case-insensitive
**  Parameters:   *string1 - Null-terminated string to compare
**                *string2 - Null-terminated string to compare
**                count - Maximum number of characters to compare
**  Return Value: <0 if str1 < str2, 0 if str1 == str2, or >0 if str1 > str2
******************************************************************/
NW_Int32 NW_Str_StrnicmpConst(const NW_Ucs2 *string1,
                           const char* const string2,
                           const NW_Uint32 count)
{
  NW_Uint32 i = 0;
  NW_Uint16 ch1;
  NW_Uint16 ch2;

  NW_ASSERT(string1 != NULL);
  NW_ASSERT(string2 != NULL);

  do {
    ch1 = NW_Str_ToUpper(string1[i]);
    ch2 = NW_Str_ToUpper(string2[i]);
    i++;
  } while (((ch1 - ch2) == 0) && (ch1 != WAE_ASCII_NULL) && (i < count));

  return (ch1 - ch2);
}

/*******************************************************************
  Name:         NW_Str_CompressWhitespace

  Description:  Removes excess whitespace, per WML Ver 1.2,
                section 11.8.1.

  Parameters:   s -- pointer to string to be squeezed

  Algorithm:    Remove leading whitespace, and compress all other
                sequences of whitespace into a single blank characer.
                "   For  example,   this   " becomes
                "For example, this ".

  Returns:      void

  NOTE:         Some gateways do this for us -- this code ensures that
                Rainbow behaves properly for gateways which fail to
                do so.
********************************************************************/

void NW_Str_CompressWhitespace(NW_Ucs2 *s)
{
  NW_Bool compressing = NW_TRUE;
  NW_Ucs2 *p = s;

  NW_ASSERT(s);

  while (*p) {
    if (NW_Str_Isspace(*p)) {
      *p = ' ';         /* make sure all whitespace becomes a blank */
      if (compressing) {
        p++;
        continue;
      }
      compressing = NW_TRUE;
    } else {
      compressing = NW_FALSE;
    }
    *s++ = *p++;
  }
  *s = '\0';
}

/*******************************************************************
  Name:         NW_Str_Strcharreplace

  Description:  Replaces char1 by char2 in a string

  Parameters:   stirng -- pointer to string to perform operation on
                char1 - character to be replaced
                char2 - character to be replaced by

  Algorithm:    Replace char1 by char2 in a string
                For example replace all d with a in string add will 
                result in ddd

  Returns:      NW_Int32 - number of replacements

********************************************************************/

NW_Int32 NW_Str_Strcharreplace(NW_Ucs2* const string, const NW_Ucs2 char1, const NW_Ucs2 char2)
{
  NW_Int32  count = 0;
  NW_Int32 i = 0;

  NW_ASSERT(string);
  
  while(string[i])
  {
    if(string[i] == char1) {
      string[i] = char2;
      count++;
    }
    i++;
  }

  return count;
}

/*******************************************************************
  Name:         NW_Str_Trim

  Description:  Removes all the whitespaces from the start or/and
                end of the string

  Parameters:   string -- pointer to string to perform operation on
                where - specifies from whether start/end or both locs
                

  Returns:      NW_Ucs2* - returns a new string;

********************************************************************/
NW_Ucs2* NW_Str_Trim(const NW_Ucs2* string, NW_Str_Location_t where)
{
  NW_Uint32 start = 0;
  NW_Uint32 end = 0;
  NW_Uint32 len;  
  const NW_Ucs2* ptr;

  if(string == NULL)
    return NULL;

  len = NW_Str_Strlen(string);

  ptr = string;

  while ((*ptr != WAE_ASCII_NULL) && NW_Str_Isspace(*ptr)) {
    ptr++;
    start++;
  }

  if(where == NW_Str_Start)
    return NW_Str_Substr(string, start, (len > start) ? (len - start) : 0);

  ptr = string + len - 1;
  end = len;

  while ((*ptr != WAE_ASCII_NULL) && NW_Str_Isspace(*ptr)) {
    ptr--;
    end--;
  }

  if(where == NW_Str_End)
    return NW_Str_Substr(string, 0, end);

  if(where == NW_Str_Both)
    return NW_Str_Substr(string, start, (end > start) ? (end - start) : 0);

  return NULL;
}

/*******************************************************************
  Name:         NW_Str_TrimInPlace

  Description:  'Removes' all the whitespaces from the start or/and
                end of the string.  This is done in-place within the
                existing memory.  Leading whitespace is not actually
                removed but the returned pointer will be such that it
                has been skipped.  Trailing whitespace is removed by
                moving the null terminator of the string.

                CAUTION: The returned pointer will be somewhere in
                the middle of the original string.  Care must be taken
                when dealing with an allocated buffer such that the
                original buffer is deallocated only after all pointers
                within the buffer are no longer used.

  Parameters:   string -- pointer to string to perform operation on
                where - specifies from whether start/end or both locs
                

  Returns:      NW_Ucs2* - returns a pointer to the trimmed string.

********************************************************************/
NW_Ucs2* NW_Str_TrimInPlace(NW_Ucs2* string, NW_Str_Location_t where)
{
  NW_Ucs2* ptrStart;
  NW_Ucs2* ptrEnd;

  if(string == NULL) {
    return NULL;
  }

  if((where != NW_Str_Start) && (where != NW_Str_End) && (where != NW_Str_Both)) {
    return NULL;
  }

  ptrStart = string;
  if((where == NW_Str_Start) || (where == NW_Str_Both)) {
    while ((*ptrStart != WAE_ASCII_NULL) && NW_Str_Isspace(*ptrStart)) {
      ptrStart++;
    }
  }

  if((where == NW_Str_End) || (where == NW_Str_Both)) {
    ptrEnd = ptrStart + NW_Str_Strlen(ptrStart) - 1;
    while ((ptrEnd >= ptrStart) && NW_Str_Isspace(*ptrEnd)) {
      *ptrEnd-- = WAE_ASCII_NULL;
    }
  }

  return ptrStart;
}

/*******************************************************************
  Name:         NW_Str_StrIsNumber

  Description:  This function is to check the given string contains
                only digits, spaces, 'w' char. These charecters are
                used to represent phone numbers.

  Parameters:   stirng -- pointer to string to perform operation on
                count - number of charecters to check.

  Returns:      NW_TRUE if all the charectors confirms to charecter
                set to represent phone number. NW_FALSE other wise.

********************************************************************/
NW_Bool NW_Str_StrIsNumber(NW_Ucs2 *string, NW_Uint32 count)
{
  NW_Uint32 i = 0;

  if(count == 0)
    return NW_FALSE;

  while((i < count) && *string)
  {
    if(NW_Str_Isdigit(*string) ||
       NW_Str_Isbreak(*string) ||
       (*string == 'w') ||
       (*string == 'p') ||
       (*string == '*') ||
       (*string == '#') ||
       (*string == '+') ||
			 (*string == '.') ||
			 (*string == ',') ||
			 (*string == ')') ||
			 (*string == '('))
    {
      string++;
      i++;
    }
    else
    {
      return NW_FALSE;
    }
  }

  return NW_TRUE;
}

/*******************************************************************
  Name:         NW_Str_StrIsValidLength

  Description:  This function is to check the given string contains
                first letter optional +/-, and only digits,

  Parameters:   stirng -- pointer to string to perform operation on

  Returns:      NW_TRUE -- if all the charectors confirms to charecter
	              in a valid number NW_FALSE other wise.

********************************************************************/
NW_Bool NW_Str_StrIsValidLength(NW_Ucs2 *string)
{
	NW_Bool retValue = NW_FALSE;
  NW_Uint32 i = NW_Str_Strlen(string);

  if(i == 0)
    return retValue;

  if (*string == WAE_ASCII_PLUS) {
    string++;
		i--;
  }

  while(i && *string)
  {
		retValue = NW_TRUE;
    if(NW_Str_Isdigit(*string))
    {
      string++;
      i--;
    }
    else
    {
      retValue = NW_FALSE;
			break;
    }
  }

  return retValue;
}

// compare string as byte *, case insensitive, 
//"count" indicate the first number of bytes to compare.

NW_Int32 NW_Byte_Strnicmp(const NW_Byte *string1, const NW_Byte *string2,
                         const NW_Uint32 count)
{
  NW_Uint32 i = 1;
  
  NW_ASSERT(string1 != NULL);
  NW_ASSERT(string2 != NULL);

  if (string1 == string2)
    return 0;

  while ((NW_Asc_ToUpper(*string1) == NW_Asc_ToUpper(*string2)) &&
         (i < count))
  {
    string1++;
    string2++;
    i++;
  }
  if (count == 0) {
    return 0;
  } else {
    return (NW_Asc_ToUpper(*string1) - NW_Asc_ToUpper(*string2));
  }
}
/*lint -restore*/
