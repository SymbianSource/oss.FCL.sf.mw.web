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
**    File name:  nwx_sprintf.c
**    Part of: string (NW)
**    Description:   Provides interfaces to (NW) string API.
******************************************************************/

/*
**-------------------------------------------------------------------------
**  Include Files
**-------------------------------------------------------------------------
*/

#include "nwx_string.h"
#include "nwx_math.h"
#include "nwx_url_utils.h"

#include <stdarg.h>
#include "float.h" /* FLT_DIG */

/*lint -save -e794, -e661 Conceivable use of null pointer, Possible access of out-of-bounds pointer*/

/*
**-------------------------------------------------------------------------
**  Constants
**-------------------------------------------------------------------------
*/

#define FLAG_LEFT_JUSTIFIED (1 << 0)
#define FLAG_USE_PLUS       (1 << 1)
#define FLAG_USE_SPACE      (1 << 2)
#define FLAG_ALTERNATE      (1 << 3)

#define FIELD_FLAGS  (1 << 0)
#define FIELD_WIDTH  (1 << 1)
#define FIELD_PREC   (1 << 2)
#define FIELD_TYPE   (1 << 3)
#define MAX_FLOAT_STRING 20
#define MAX_TEMP_BUF 50
/*
**-------------------------------------------------------------------------
**  Internal Types
**-------------------------------------------------------------------------
*/

typedef enum {
  TYPE_INVALID,
  TYPE_SHORT,
  TYPE_LONG_INT,
  TYPE_LONG_DOUBLE
} Type;

typedef enum {
  FORMAT_INVALID,
  FORMAT_SIGNED,
  FORMAT_OCTAL,
  FORMAT_UNSIGNED,
  FORMAT_HEX,
  FORMAT_BIGHEX,
  FORMAT_FLOAT,
  FORMAT_EXP,
  FORMAT_BIGEXP,
  FORMAT_CHOOSE,
  FORMAT_BIGCHOOSE,
  FORMAT_CHAR,
  FORMAT_STRING,
  FORMAT_POINTER,
  FORMAT_OUTPUT_LEN
} Format;

typedef enum {
  STATE_NORMAL,
  STATE_DIRECTIVE
} State;

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
** this function rounds an integer (val) to a specified number of
** significant digits (prec).  If the digit at the place to be rounded is 5
** or greater we round AWAY from 0 (e.g. -15 gets rounded to -20).  The
** resulting value is converted to a string and returned in digString. On
** input, digString must contain a string representation of the value to be
** rounded.  The rounded integer value is returned as the value of the
** function.  Rounding is accomplished by adding 5Ex to val, where x
** specifies the position of the digit where rounding will occur. The result
** is divided by 1E(x+1) (integer division) to eliminate the (x+1) low order
** digits, then multiplied by 1E(x+1) to refill the (x+1) low order digits
** with zeros. ex.  val = 12345678 prec = 5, x = 2, val += 500 -> 12346178,
** 12346178/1000 -> 12346, 12346*1000 -> 12346000 which is then converted to
** the string "12346000"
*/
static NW_Int32 roundInt(NW_Int32 val, NW_Ucs2 *digString, NW_Int32 prec)
{
  NW_Int32 roundFactor; /* 5Ex */
  NW_Int32 roundLoc;    /* roundLoc = x+1 */
  NW_Int32 roundMult;   /* 1E(x+1) */

  /*
  ** calculate x  - for the example above, x will be 2, roundLoc is 3.
  ** NOTE: digString must start out as the string representation of val!
  */
  roundLoc = NW_Str_Strlen(digString) - prec;
  /*
  ** if we've got a negative number then we need to account for the '-' in
  ** the string representation, so subtract one from roundLoc.  Our example
  ** is positive so roundLoc remains 3.
  */
  if (val < 0) {
    roundLoc--;
  }
  /* now - if we need to do some rounding */
  if (roundLoc > 0) {
    roundFactor = 5;
    roundMult = 10;
    /*
    ** in our example roundLoc is 3, so we execute the following loop twice,
    ** roundFactor ends up as 500, roundMult ends up as 1000.
    */
    while (roundLoc > 1) {
      roundFactor *= 10;
      roundMult *= 10;
      roundLoc--;
    }
    /* if we're rounding negative numbers sub rather than add */
    if (val > 0) {
      val += roundFactor;
    } else {
      val -= roundFactor;
    }
    /* in our example 12346178/1000 => 12346*/
    val /= roundMult;
    
    /* in our example 12346178*1000 => 12346000 */
    val *= roundMult;

    /* finally, convert the result back to a string */
    NW_Str_Itoa(val, digString);
  }
  /* return the rounded value */
  return val;
}

static NW_Int32 gFormat(NW_Float64 x, NW_Int32 prec, NW_Ucs2 *output,NW_Int32 outSize)
{
  NW_Int32 base;
  NW_Int32 nDigits = 0;
  NW_Int32 exp;
  NW_Int32 count = 0;
  NW_Ucs2 expString[MAX_FLOAT_STRING];
  NW_Ucs2 baseString[MAX_FLOAT_STRING];
  NW_Ucs2 *ptrOut;
  NW_Ucs2 *ptrIn;

  NW_ASSERT(output != NULL);

  NW_Math_BaseandExp64(x, &base, &exp);
  NW_Str_Itoa(base, baseString);
  
  ptrOut = output;
  ptrIn = baseString;
  
  nDigits = NW_Str_Strlen(baseString);
  
  if (prec > nDigits){
	  nDigits = prec;
    /* 8: '-','0','.','E','-','1','5','\0' as a ex. -0.3E-15 */
	  if ((nDigits + 8) > outSize) {
		  return 0;
  	}
  }
  /**copy '-' into start of output**/
  if (x < 0) {
    *ptrOut++ = *ptrIn++;
  }
  *ptrOut++ = '0';
  *ptrOut++ = WAE_URL_DOT_CHAR;
  base = roundInt(base, baseString, prec);
  
  if ((NW_Str_Strlen(baseString) - nDigits) == 1) {
    exp ++;
  }
  
  while ((count < prec) && (*ptrIn != 0)) {
    *ptrOut++ = *ptrIn++;
    count++;
  }
  while (count < prec) {
    *ptrOut++ = '0';
    count++;
  }
   
  NW_Str_Itoa(exp, expString);


  if (exp != 0) {
    *ptrOut++ = 'E';
    ptrIn = expString;
    while (*ptrIn != 0) {
      *ptrOut++ = *ptrIn++;
    }
  }
  *ptrOut = 0;
  return (ptrOut - output);
}

/*
**this function is converting a float to a string by the precision input,and return 
**the length of it.First convert it to a mantissa and exponent(exp),then do a 
**rounding as a suitable position. To get the output,the string is made by 5 steps: 
**1)if negative, add a '-' at the beginning. 2)if exp > 0, output those bits in 
**baseString;otherwise add a '0' before '.'. 3)'.'. 4)if exp < 0, add '0'(s) as the bit(s)
**of precision;otherwise output the precision part of baseString. 5)if baseString is 
**not as precision as needed,fill '0'(s) in tail.ex. x = -1.2345678,prec = 6,->exp = 1,
**'-12345678' -> '-1234570' -> '-1.234570' ( len = 9)
*/ 
static NW_Int32 fFormat(NW_Float64 x, NW_Int32 prec, NW_Ucs2 *output, NW_Int32 outSize)
{
  NW_Int32 count = 0;        /* length of output string */
  NW_Int32 nDigits = 0;      /* length of baseString */ 
  NW_Int32 base;             /* integer form */
  NW_Int32 exp;              /* exponent */
  NW_Ucs2 baseString[MAX_FLOAT_STRING];
  NW_Int32 j,precCount = 0;  /* if it is smaller than the precision asked */ 
  NW_Int32 expCount = 0;     /* how many '0' should set before and after '.' */
  NW_Ucs2 *ptrIn;       /* baseString */
  NW_Int32 roundSpot;        /* suitable position of rounding */
  
  NW_ASSERT(output != NULL);
  NW_ASSERT(prec < outSize);
  /* convert x to a interger and exp,in our example, exp = 1 */ 
  NW_Math_BaseandExp64(x, &base, &exp);
  /* convert base to a string */
  NW_Str_Itoa(base, baseString);
  
  /* suitable position of rounding,in our example,roundSpot = 7 */ 
  roundSpot = prec + exp;
  /* now,roundSpot = 6 */
  if (roundSpot > FLT_DIG){
    roundSpot = FLT_DIG;
  }
  /* in our example,base -> 1234570 */
  base = roundInt(base, baseString, roundSpot);
  
  /* string length,in our example nDigits = 7 */
  nDigits = NW_Str_Strlen(baseString);
  NW_ASSERT(nDigits < outSize);
	/* To fix TI compiler warning */
	(void) nDigits;
  /* 4: '-', '0', '.', '\0' */  
  if (((exp + prec + 4 ) > outSize) || ((prec + 4) > outSize)){
  	return 0; 
  }
  ptrIn = baseString;
	
  /* copy '-' into start of output */
  if (x < 0) {
    *output++ = *ptrIn++;
	  count++;
  }  
  NW_ASSERT(count < outSize);
  /* is there any '0' before and after '.' */
  /* if exp > 0,expCount = -1; if exp = 0,expCount = 0; if exp < 0;expCount = -exp */
  if ( exp < 0) {
	  expCount = -exp ;
  }
  else{ 
	  if (exp == 0){
		  expCount = 0;
	  }
    else{
      expCount = -1;
    }
  }
  NW_ASSERT((ptrIn-baseString) < MAX_FLOAT_STRING);
  /* in our example,output '1',j -> 1*/
  for(j=0;j < exp;j++){
    if (( j < exp ) && ( *ptrIn != '\0')){
      *output++ = *ptrIn++;
      }
  }
  for (;j< exp; j++){
    *output++ = '0';
  }
  /*in our example,count -> 2 */
  count += j;
  NW_ASSERT(count < outSize);
  /* exp > 0, in our example, count -> 3 */ 
  if ( expCount == -1 ){
    *output++ = WAE_URL_DOT_CHAR;
    count++;
  }
  else{
    /* exp < = 0 */
    *output++ = '0';
    *output++ = WAE_URL_DOT_CHAR;
 	  count += 2;
    /* output '0' when exp < 0 */
    while ( expCount > 0) {
      *output++ = '0';
      expCount--;
      precCount++;
      count++;
	  }
  }  
  NW_ASSERT(count < outSize);
  /*in our example,output '234570',count -> 9,precCount -> 6*/
  while (( precCount  < prec ) && (*ptrIn != '\0')){
    *output++ = *ptrIn++;
    count++;
	  precCount++;
  }
  /* fill '0'(s) at the end if not precision enough */
  while (( precCount  < prec ) && (prec > 0)){
    *output++ = '0';
    count++;
    precCount++;
  }
  NW_ASSERT(count < outSize);
  *output = 0;
  /* in our example count -> 9 */
  return  (count) ;
}

static const NW_Ucs2 *parse_flags ( const NW_Ucs2 *format, NW_Uint32 *fields, NW_Uint32 *retFlags )
{
  const NW_Ucs2 *traverse = format;
  NW_Bool done = NW_FALSE;
  NW_Ucs2 ch;

  NW_ASSERT ( traverse != NULL );
  NW_ASSERT ( fields != NULL );
  NW_ASSERT ( retFlags != NULL );

  while ((( ch = *traverse ) != '\0') && !done) {
    switch (ch) {
     case '-' :
      *retFlags |= FLAG_LEFT_JUSTIFIED;
      traverse++;
      break;
     case '+' :
      *retFlags |= FLAG_USE_PLUS;
      traverse++;
      break;
     case ' ' :
      *retFlags |= FLAG_USE_SPACE;
      traverse++;
      break;
     case '#' :
      *retFlags |= FLAG_ALTERNATE;
      traverse++;
      break;
     default:
      /* end of flags */
      done = NW_TRUE;
    }
  }

  if ( traverse != format )
    *fields |= FIELD_FLAGS;

  return traverse;
}

static const NW_Ucs2 *parse_width ( const NW_Ucs2 *format, NW_Uint32 *fields, NW_Uint32  *retWidth )
{
  NW_Uint32  width;
  NW_Ucs2 *endPtr = NULL;

  NW_ASSERT ( format != NULL );
  NW_ASSERT ( fields != NULL );
  NW_ASSERT ( retWidth != NULL );

  width = NW_Str_Strtoul ( format, &endPtr, 10 );
  NW_ASSERT ( endPtr != NULL );
  if ( format != endPtr ) {
    *fields |= FIELD_WIDTH;
    *retWidth = width;
  }

  return endPtr;
}

static const NW_Ucs2 *parse_prec ( const NW_Ucs2 *format, NW_Uint32 *fields, NW_Uint32  *retPrec )
{
  NW_Uint32  prec;

  NW_ASSERT ( format != NULL );
  NW_ASSERT ( fields != NULL );
  NW_ASSERT ( retPrec != NULL );

  if ( *format == '.' ) {
    NW_Ucs2 *endPtr = NULL;
    format++;
    prec = NW_Str_Strtoul ( format, &endPtr, 10 );
    *fields |= FIELD_PREC;
    if ( format != endPtr ) {
      *retPrec = prec;
    } else {
      *retPrec = 0;
    }
    return endPtr;
  } else {
    return format;
  }
}

static const NW_Ucs2 *parse_type ( const NW_Ucs2 *format, NW_Uint32 *fields, Type *retType )
{
  NW_ASSERT ( format != NULL );
  NW_ASSERT ( fields != NULL );
  NW_ASSERT ( retType != NULL );

  switch ( *format ) {
   case 'h':
    *retType = TYPE_SHORT;
    *fields |= FIELD_TYPE;
    format++;
    break;
   case 'l':
    *retType = TYPE_LONG_INT;
    *fields |= FIELD_TYPE;
    format++;
    break;
   case 'L':
    *retType = TYPE_LONG_DOUBLE;
    *fields |= FIELD_TYPE;
    format++;
    break;
   default:
    /* no type */
    ;
  }
  return format;
}

static const NW_Ucs2 *parse_format ( const NW_Ucs2 *format, Format *retFormat )
{
  NW_ASSERT ( format != NULL );
  NW_ASSERT ( retFormat != NULL );

  switch ( *format ) {
   case 'i':
   case 'd':
    *retFormat = FORMAT_SIGNED;
    format++;
    break;
   case 'o':
    *retFormat = FORMAT_OCTAL;
    format++;
    break;
   case 'u':
    *retFormat = FORMAT_UNSIGNED;
    format++;
    break;
   case 'x':
    *retFormat = FORMAT_HEX;
    format++;
    break;
   case 'X':
    *retFormat = FORMAT_BIGHEX;
    format++;
    break;
   case 'f':
    *retFormat = FORMAT_FLOAT;
    format++;
    break;
   case 'e':
    *retFormat = FORMAT_EXP;
    format++;
    break;
   case 'E':
    *retFormat = FORMAT_BIGEXP;
    format++;
    break;
   case 'g':
    *retFormat = FORMAT_CHOOSE;
    format++;
    break;
   case 'G':
    *retFormat = FORMAT_BIGCHOOSE;
    format++;
    break;
   case 'c':
    *retFormat = FORMAT_CHAR;
    format++;
    break;
   case 's':
    *retFormat = FORMAT_STRING;
    format++;
    break;
   case 'p':
    *retFormat = FORMAT_POINTER;
    format++;
    break;
   case 'n':
    *retFormat = FORMAT_OUTPUT_LEN;
    format++;
    break;
   default:
    /* bad format string */
    NW_ASSERT ( 0 );
    format = NULL;
  }
  return format;
}

static NW_Int32  execute_hex ( NW_Ucs2 *output, NW_Int32 count, NW_Int32 fields,
                            NW_Int32 width, NW_Int32  prec, Type type, va_list *argptr )
{
  NW_Uint32 val = 0;
  NW_Int32  numWritten = 0;

  NW_ASSERT ( output != NULL );
  NW_ASSERT ( count  >= 0 );
  NW_ASSERT ( fields >= 0 );
  NW_ASSERT ( width  >= 0 );
  NW_ASSERT ( prec   >= 0 );

  NW_REQUIRED_PARAM(type);

  val = va_arg ( *argptr, NW_Uint32 );

  if (! ( fields & FIELD_WIDTH )) {
    width = 1;
  }
  if (! ( fields & FIELD_PREC )) {
    prec = 1;
  }

  /* perhaps I should through in val == 0 and width == 0 */
  if (! (( prec == 0 ) && (val == 0))) {
    NW_Ucs2 temp_buf[40];
    NW_Int32 len;
    NW_Int32 zeroes = 0;
    NW_Int32 spaces = 0;
    NW_Int32 i;

    /* Platform-specific */
    NW_Str_UltoBase(val, temp_buf, 16);

    len = NW_Str_Strlen ( temp_buf );

    if ( len < prec ) {
      zeroes = prec - len;
    }
    if (( len + zeroes ) < width ) {
      spaces = width - ( len + zeroes );
    }

    for ( i = 0; i < spaces; i++ ) {
      if ( count > (NW_Uint16) numWritten ) {
        *output++ = ' ';
        numWritten++;
      }
    }

    for ( i = 0; i < zeroes; i++ ) {
      if ( count > (NW_Uint16) numWritten ) {
        *output++ = '0';
        numWritten++;
      }
    }

    if ( count > (NW_Uint16) numWritten ) {
      NW_Int32 toWrite = len;
      if ( len > (count - (NW_Uint16) numWritten )) {
        toWrite = count - numWritten;
      }
      NW_Str_Strncpy ( output, temp_buf, toWrite );
      numWritten = numWritten + toWrite;
    }
  }

  return numWritten;
}

static NW_Int32  execute_signed ( NW_Ucs2 *output, NW_Int32 count, NW_Int32 fields,
                               NW_Int32 width, NW_Int32  prec, Type type, va_list *argptr )
{
  long val = 0;
  NW_Int32  numWritten = 0;
  NW_Bool negative = NW_FALSE;

  NW_ASSERT ( output != NULL );
  NW_ASSERT ( count  >= 0 );
  NW_ASSERT ( fields >= 0 );
  NW_ASSERT ( width  >= 0 );
  NW_ASSERT ( prec   >= 0 );

  if ( fields & FIELD_TYPE ) {
    switch ( type ) {
     case TYPE_SHORT :
      val = (NW_Int16) va_arg ( *argptr, NW_Int32 );
      break;
     case TYPE_LONG_INT :
      val = va_arg ( *argptr, long );
      break;
     case TYPE_LONG_DOUBLE:
      NW_ASSERT ( 0 ); /* should not happen with a %d */
      numWritten = -1;
      break;
     default:
      NW_ASSERT ( 0 );
    }
  } else {
    val = va_arg ( *argptr, NW_Int32 );
  }

  if (! ( fields & FIELD_WIDTH )) {
    width = 1;
  }
  if (! ( fields & FIELD_PREC )) {
    prec = 1;
  }

  /* perhaps I should through in val == 0 and width == 0 */
  if ((numWritten != -1) && (! (( prec == 0 ) && (val == 0)))) {
    NW_Ucs2 temp_buf[40];
    NW_Int32 len;
    NW_Int32 zeroes = 0;
    NW_Int32 spaces = 0;
    NW_Int32 i;

    /* Platform-specific */
    NW_Str_Itoa(val, temp_buf);
    if (*temp_buf == '-') negative = NW_TRUE;

    len = NW_Str_Strlen ( temp_buf );
    if ( negative ) len--;

    if ( len < prec ) {
      zeroes = prec - len;
    }
    if (( len + zeroes ) < width ) {
      spaces = width - ( len + zeroes );
      if ( negative ) spaces--;
    }

    for ( i = 0; i < spaces; i++ ) {
      if ( count > numWritten ) {
        *output++ = ' ';
        numWritten++;
      }
    }

    if ( negative ) {
      if ( count > numWritten ) {
        *output++ = '-';
        numWritten++;
      }
    }

    for ( i = 0; i < zeroes; i++ ) {
      if ( count > numWritten ) {
        *output++ = '0';
        numWritten++;
      }
    }

    if ( count > numWritten ) {
      NW_Int32 toWrite = len;
      if ( len > (count - numWritten )) {
        toWrite = count - numWritten;
      }
      if ( negative ) {
        NW_Str_Strncpy ( output, temp_buf + 1, toWrite);
      } else {
        NW_Str_Strncpy ( output, temp_buf, toWrite );
      }
      numWritten = numWritten + toWrite;
    }
  }

  return numWritten;
}
/*
**The function handle the output of float by precision asked.When the precison 
**input is zero,set it to '6' as a default value.If fmt is FORMAT_FLOAT,output a 
**float string as input. If is FORMAT_CHOOSE, output a string being written in 
**"0.2E8" format if the input val is not in 0.0001 <= |val| < 1000000. And in 
**the second situation, we skip useless zero(s) at the tail. 
*/
static NW_Int32  execute_choose (NW_Ucs2  *buf,  NW_Int32 count, NW_Int32 fields,
                                 NW_Int32 width, NW_Int32 prec,  Format   fmt,
                                 va_list  *argptr)
{
  NW_Float64 val;
  NW_Ucs2  temp_buf[MAX_TEMP_BUF];
  NW_Int32 i, j;
  NW_Int32 len        = 0;
  NW_Int32 spaces     = 0;
  NW_Int32 numWritten = 0;
  NW_Ucs2  *output    = buf;
  NW_Bool  localPrec  = NW_FALSE;  /* Is the precision set by input, or by local function */
  NW_Bool  decDot     = NW_FALSE;  /* Is there a decimal pot in the string */
  NW_Bool  eFormat    = NW_FALSE;  /* Is there a "E" in the string */

  NW_ASSERT ( buf != NULL );
  NW_ASSERT ( output != NULL );
  
  val = va_arg ( *argptr, NW_Float64 );
	/* 
   * if no precision asked, set it as a default value, localPrec -> NW_TRUE
   * if the precision is more then the buffer then set it to efault value.
   */
  if (((!(fields & FIELD_PREC)) && (prec == 0)) || (prec >= MAX_TEMP_BUF))
  {
    prec = NW_DEFAULT_FLOAT_PREC;
    localPrec = NW_TRUE;
  }
  if (! ( fields & FIELD_WIDTH)) {
    width = 1;
  }
  NW_ASSERT((fmt == FORMAT_FLOAT) || (fmt == FORMAT_CHOOSE));
  if (fmt == FORMAT_FLOAT){
    len = fFormat( val, prec, temp_buf, MAX_TEMP_BUF );/* just ouput float string */
  } 
  else {
    /* different formats as different float values input, for example, 20000 -> 20000.0,
    len = 8 by fFormat;20000000 -> 0.2E8 ,len = 5 by gFormat as prec = 1 */
    if ((val < 1000000.00 && val >= 0.0001) || (val > -1000000.00 && val <= -0.0001)){
      len = fFormat( val, prec, temp_buf, MAX_TEMP_BUF );
    }
    else {
      len = gFormat( val, prec, temp_buf, MAX_TEMP_BUF );
    }
    /* calculate trailing '0',if percision was set by local function; else output string
    as percision as asked*/
    if (localPrec) {
      i = 0;
      while ((temp_buf[len-i-1] == '0')&&((len-i-1) > 0)){
        i++;
      }
      for (j = 0 ; j <= (len - i - 1); j++){
        if (temp_buf[j] == '.'){
          decDot = NW_TRUE;
        }
        if (temp_buf[j] == 'E'){
          eFormat = NW_TRUE;
        }
      }
      /* if there isn't a decimal pot in the string, no need to skip '0'(s) at the tail */
      /* undo it if the val was processed by gFomat.for example "0.1E10" we shouldn't skip the tail "0"*/
      if ((decDot) && (!eFormat)){
        if (i == prec){
          len -= (i-1);
        }
        else {
          len -= i;
        }
      }
    }
  }
  /* if the last bit of the string is '.',skip it */
  if (temp_buf[len-1] == '.'){
    len -= 1;
  }
  /* is there any empty spaces need before the string */
  if ( width > len ){
    spaces = width - len;
  }
  for ( i = 0; (i < spaces) && (numWritten < count); i++ ) {
      *output++ = ' ';
      numWritten++;
  }
  i = 0;
  while((i < len) && (temp_buf[i] != '\0') && (numWritten < count)){
    *output++ = temp_buf[i];
    numWritten++;
		i++;
  }
  return numWritten;
}

static NW_Int32  execute_string ( NW_Ucs2 *output, NW_Int32 count, NW_Int32 fields,
                               NW_Int32 width, NW_Int32 prec, va_list *argptr )
{
  const NW_Ucs2 *val;
  NW_Int32 numWritten = 0;
  NW_Int32 len;
  NW_Int32 toWrite;
  NW_Int32 i;

  NW_ASSERT ( output != NULL );
  NW_ASSERT ( count  >= 0 );
  NW_ASSERT ( fields >= 0 );
  NW_ASSERT ( width  >= 0 );
  NW_ASSERT ( prec   >= 0 );

  if ( ! ( fields & FIELD_WIDTH )) {
    width = 1;
		/* To fix TI compiler warning */
		(void) width;
  }

  val = va_arg ( *argptr, const NW_Ucs2 *);
  if (val == NULL) {
    return 0;
  }
  len = NW_Str_Strlen ( val );

  /*
  ** WMLScript Std Lib 9.16 says for strings: "When the width is larger than
  ** precision, the width should be ignored." and in general: "If the number
  ** of characters in the output value is greater than the specified width
  ** or, if width is not given, all characters of the value are printed."
  ** Sooo, for strings, we always ignore width
  */
  toWrite = len;

  if ( fields & FIELD_PREC ) {
    if ( prec < toWrite ) toWrite = prec;
  }

  for ( i = 0; i < toWrite; i++) {
    if ( numWritten < count ) {
      *output++ = *val++;
      numWritten++;
    }
  }

  return numWritten;
}

static NW_Int32  execute_directive ( NW_Ucs2 *buf, NW_Int32 count, NW_Int32 fields,
                                  NW_Int32 width, NW_Int32 prec, Type type,
                                  Format fmt, va_list *argptr )
{
  NW_Int32 numWritten = 0;

  NW_ASSERT ( buf != NULL );
  NW_ASSERT ( count  >= 0 );
  NW_ASSERT ( fields >= 0 );
  NW_ASSERT ( width  >= 0 );
  NW_ASSERT ( prec   >= 0 );

  switch ( fmt ) {
   case FORMAT_SIGNED:
    numWritten = execute_signed ( buf, count, fields, width, prec, type, argptr );
    break;
   case FORMAT_FLOAT:
   case FORMAT_CHOOSE:
    numWritten = execute_choose ( buf, count, fields, width, prec, fmt, argptr );
    break;
   case FORMAT_STRING:
    numWritten = execute_string ( buf, count, fields, width, prec, argptr );
    break;
   case FORMAT_HEX:
   case FORMAT_BIGHEX:
    numWritten = execute_hex ( buf, count, fields, width, prec, type, argptr );
    break;
   default:
    NW_ASSERT (0);
    /* not implemented */
    numWritten = -1;
  }
  return numWritten;
}

/*
**-------------------------------------------------------------------------
**  External Public (Exported) Functions
**-------------------------------------------------------------------------
*/

NW_Int32 NW_Str_Vsnprintf (NW_Ucs2 *buf, const NW_Int32 count, const NW_Ucs2 *format, va_list *argptr)
{
  NW_Ucs2 ch;
  NW_Int32 numWritten = 0;
  NW_Ucs2 *output;
  const NW_Ucs2 *input;
  State state = STATE_NORMAL;

  NW_ASSERT ( buf != NULL );
  NW_ASSERT ( count >= 0 );
  NW_ASSERT ( format != NULL );

  input = format;

  output = buf;


  while ((( ch = *input ) != '\0') && ( numWritten >= 0 )) {
    switch (state) {
     case STATE_NORMAL:
      if ( ch == '%' ) {
        if (*(input + 1) == '%') {
          /* it's a %% */
          input++;
        } else {
          /* starting directive */
          state = STATE_DIRECTIVE;
        }
      }
      if ( state == STATE_NORMAL ) {
        /* regular character, just write it */
        NW_ASSERT(0 <= count);
        if ( numWritten < count ) {
          *output++ = ch;
          numWritten++;
        }
      }
      input++;
      break;
     case STATE_DIRECTIVE:
      {
        NW_Uint32 fields = 0;
        NW_Uint32 flags = 0;
        NW_Uint32  width = 0;
        NW_Uint32  prec = 0;
        Type type = TYPE_INVALID;
        Format fmt = FORMAT_INVALID;

        if (( input = parse_flags ( input, &fields, &flags )) != NULL ) {
          if (( input = parse_width ( input, &fields, &width )) != NULL ) {
            /* If width is more than allowed then set width to the max allowed */
            if (width > NW_UINT32_CAST(count))
            {
              width = NW_UINT32_CAST(count); /* Last byte is to hold null char */
            }
            if (( input = parse_prec ( input, &fields, &prec )) != NULL ) {
              if (( input = parse_type ( input, &fields, &type )) != NULL ) {
                if (( input = parse_format ( input, &fmt )) != NULL ) {
                  NW_Int32  retVal;
                  retVal = execute_directive ( output, count - numWritten,
                                               fields, width, prec, type,
                                               fmt, argptr );
                  if ( retVal >= 0 ) {
                    numWritten += retVal;
                    output += retVal;
                    state = STATE_NORMAL;
                  }
                } else {
                    numWritten = -1;
                  }
              } else {
                numWritten = -1;
              }
            } else {
              numWritten = -1;
            }
          } else {
            numWritten = -1;
          }
        }
      }
      break;
     default:
      NW_ASSERT ( 0 );
    }
  }

  NW_ASSERT(0 <= count);
  if (numWritten <= count) {
    *output = '\0';
  }
  return numWritten;
}

NW_Int32  NW_Str_Sprintf (NW_Ucs2 *buf, const NW_Ucs2 *format, ... )
{
  va_list args;
  NW_Int32  status;

  va_start (args, format );
  /* TODO: 9999999 should be some kind of MAX_INT */
  status = NW_Str_Vsnprintf ( buf, 9999999, format, &args );
  va_end (args);

  return status;
}

/*lint -restore*/
