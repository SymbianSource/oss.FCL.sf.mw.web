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


 /*
    $Workfile: scr_conv.c $

    Purpose: handle parameter parsing in urls

    $History: scr_conv.c $
  */

#include "scr_conv.h"
#include "scr_core.h"
#include "scr_valexpr.h"

#include "nwx_mem.h"
#include "nwx_string.h"
#include "nwx_url_utils.h"
#include "nwx_math.h"
#include "nwx_float.h"

#include "nw_wae_reader.h"
#include "BrsrStatusCodes.h"

#define NO_PRECISION  -1
#define NO_WIDTH      -1

/*
**-------------------------------------------------------------------------
**  Internal Prototypes
**-------------------------------------------------------------------------
*/
static NW_Int32 Snprintf(NW_Ucs2 *buf, const NW_Int32 buf_len, const NW_Ucs2 *format, ... );

/*
**-------------------------------------------------------------------------
**  Internal Functions
**-------------------------------------------------------------------------
*/
/* Implementation of Snprintf exists to avoid NW_Str_Vsnprintf get duplicated */
static NW_Int32 Snprintf(NW_Ucs2 *buf, const NW_Int32 buf_len, const NW_Ucs2 *format, ... )
{
  va_list args;
  NW_Int32  status;

  va_start (args, format );
  /* buf_len - 1, for null termination */
  status = NW_Str_Vsnprintf ( buf, buf_len - 1, format, &args );
  va_end (args);

  return status;
}

NW_Ucs2 *str2ucs2(str_t str)
{
  NW_Ucs2 *res = 0;
  NW_Int32 len;

  len = str_len(str);
  res = NW_Str_New(len);
  if (!res)
  {
    set_error_code(SCR_ERROR_OUT_OF_MEMORY);
    return 0;
  }

  if (len > 0) NW_Mem_memcpy(res, &str[1], len*sizeof(NW_Ucs2));
  res[len] = 0;

  return res;
}



char *str2ascii(str_t s)
{
  NW_Int32  len, i;
  NW_Uint16 wc;
  char *res;

  len = str_len(s);

  res = (char*)NW_Mem_Malloc(len + 1);
  if (!res)
  {
    set_error_code(SCR_ERROR_OUT_OF_MEMORY);
    return 0;
  }

  for(i = 0; i < len; i++)
  {
    wc = s[1 + i];
    if (wc > 127)
    {
      NW_Mem_Free(res);
      return 0;
    }
    res[i] = (char)wc;
  }
  res[len] = 0;
  return res;
}


NW_Bool str2int(str_t s, NW_Int32  *res, NW_Bool *consumeAll)
{
  NW_Int32  start = 0, len = str_len(s), end, i;
  str_t substr;
  NW_Ucs2 *wstr;
  NW_Ucs2 *p = NULL; /*TODO:  JW fix in WAVE*/
  NW_Ucs2 c;
  TBrowserStatusCode status;

  NW_ASSERT(s);
  NW_ASSERT(res);
  NW_ASSERT(consumeAll);

  *consumeAll = NW_FALSE;

  /* skipping white spaces */
  while ( (start < len) && (scr_charat(s, start) <= 32) ) {
    start++;
  }

  i = start;
  if (i == len) {
    return NW_FALSE; /* reach the end of the string */
  }
  c = scr_charat(s, i);
  if ((c != '+') && (c != '-') && !NW_Str_Isdigit(c)) {
    return NW_FALSE;
  }
  i++;

  if ( ((c == '+') || (c == '-')) &&
       ((i >= len) || !NW_Str_Isdigit(scr_charat(s, i))) )
  {
    return NW_FALSE;
  }
  
  while ((i < len) && NW_Str_Isdigit(scr_charat(s, i))) {
    i++;
  }
  end = i;

  if (start == end) {
    return NW_FALSE;
  }

  while ((i < len) && (scr_charat(s, i) <= 32) ) {
    i++;
  }

  if (i== len) {
    *consumeAll = NW_TRUE;
  }

  /* extract substring and convert to long */
  substr = scr_substr(s, start, end - start);
  if (substr == NULL)
    return NW_FALSE;

  wstr = str2ucs2(substr);
  free_str(substr);
  if (wstr == NULL)
    return NW_FALSE;

  status = NW_Str_CvtToInt32(wstr, &p, 10, res);
  NW_Mem_Free(wstr);
  if (status != KBrsrSuccess) {
    return NW_FALSE;
  }

  if (p != NULL) {
    return NW_TRUE;
  } else {
    return NW_FALSE;
  }
}


/*
<prefix><NW_Int32 >.<secondInt>e<exponent><postfix>
*/
/* needs more work to meet the specs */
NW_Bool str2float(str_t s, NW_Float32 * res, NW_Bool * consumeAll)
{
  NW_Int32 start = 0, len = str_len(s), end, i;
  NW_Ucs2 c;
  NW_Float64 intPart = 0.0;
  NW_Float64 fracPart = 0.0;
  NW_Float32 expPart = 1.0;
  NW_Int32   sign = 1;
  NW_Int32   expVal = 0;
  NW_Float64 res64;
  
  NW_ASSERT(s);
  NW_ASSERT(res);
  NW_ASSERT(consumeAll);
  *consumeAll = NW_FALSE;
  /* skipping white spaces */
  while ((start < len) && (scr_charat(s, start) <= 32)) {
    start++;
  }

  i = start;
  if (i == len) {
    return NW_FALSE;               /* reach the end of the string */
  }

  c = scr_charat(s, i);
  if ((c != '+') && (c != '-') && (c != '.') && !NW_Str_Isdigit(c)) {
    return NW_FALSE;
  }

  if (c != '.') {
    /* read NW_Int32  part */
    if (c == '-') {
      sign = -1;
    } else if (NW_Str_Isdigit(c)) {
      intPart = (NW_Float32)(c - '0');
    }
    i++;
    while ((i < len) && NW_Str_Isdigit(scr_charat(s, i))) {
      if (is_finite_float64(intPart)) {
        intPart = (intPart * 10) + (scr_charat(s, i) - '0');
      }
      i++;
    }
    if (is_finite_float64(intPart)) {
      intPart *= sign;
    } else {
      return NW_FALSE;
    }

  }

  if ((i < len) && (scr_charat(s, i) == '.')) {
    /* read fraction part */
    i++;
    expPart = 0.1f;
    while ((i < len) && NW_Str_Isdigit(scr_charat(s, i))) {
      /*lint -e{747} Significant prototype coercion*/
      if (! is_zero_float64(expPart)) {
        fracPart = fracPart + expPart*(scr_charat(s, i) - '0');
        expPart *= 0.1f;
      }
      i++;
    }
    fracPart *= sign;
  }

  if ((i < len) && ((scr_charat(s, i) == 'e') || (scr_charat(s, i) == 'E'))) {
    /* if we have xxx[e|E] we must have exponent of some kind */
    if (++i >= len) {
      return NW_FALSE;
    }
    c = scr_charat(s, i);
    /* exp must be e[+|-]nn */
    if ((c != '+') && (c != '-') && !NW_Str_Isdigit(c)) {
      return NW_FALSE;
    }

    i++;
    sign = 1;
    /* if exp is e[+|-] or e[+|-]xx where xx is not a number, error */
    if ((c == '+') || (c == '-')) {
      if ((i >= len) || !NW_Str_Isdigit(scr_charat(s, i))) {
        return NW_FALSE;
      } else if (c == '-') {
          sign = -1;
      }
    } else {
      expVal = c - '0';
    }

    /* Get exponent digits. */
    while ((i < len) && NW_Str_Isdigit(scr_charat(s, i))) {
      /* if we can't hold exponent we've got over/underflow */
      if ((expVal > NW_FLOAT32_MAX_EXP) && (sign > 0)) {
        return NW_FALSE; /* overflow */
      } else {
        /* if exponent is neg, spec says underflow -> 0, so */
        /* we need to eat all chars in exp, but want expVal to */
        /* remain a big, negative, number */
        if (expVal >= 0) {
          if (expVal < 2 * NW_FLOAT32_MAX_EXP) {
            expVal = (expVal * 10) + (scr_charat(s, i) - '0');
          }
        }
      }
      i++;
    }
    if (expVal > 0) {
      expVal *= sign;
    }
  }
  /* End exponent part. */
  end = i;

  /* explicit check for just sign char, i.e. '+' or '-' */
  if ( (start == end) ||
       ((start + 1 == end) && !NW_Str_Isdigit(scr_charat(s, start))) )
  {
    return NW_FALSE;
  }

  while ((i < len) && (scr_charat(s, i) <= 32)) {
    i++;
  }

  if (i == len) {
    *consumeAll = NW_TRUE;
  }

  /* The calculations will be done with doubles, so allow double
   * the exponents for the time being.
   */
  if (expVal < NW_FLOAT32_MIN_EXP * 2) {
    *res = 0.0f;
    return NW_TRUE;
  } else if (expVal > NW_FLOAT32_MAX_EXP * 2) {
    return NW_FALSE;
  } else {
    res64 = (intPart + fracPart)*NW_Math_powint64(10.0f, expVal);
    if (is_zero_float64(res64)) {
      *res = 0.0f;
      return NW_TRUE;
    } else if (is_finite_float64(res64)) {
      *res = (NW_Float32)res64;
      return NW_TRUE;
    } else {
      return NW_FALSE;
    }
  }
}


str_t int2str(NW_Int32 i)
{
  /* Note: largest formatted NW_Int32 is 11 chars
   * (sign plus 10 digits)
   */
  NW_Ucs2 buf[MAX_CHAR_GROUP_LEN];
  NW_Ucs2 fmt[8];
  NW_Uint32 len;

  NW_Str_StrcpyConst(fmt, "%d");
  len = Snprintf(buf, MAX_CHAR_GROUP_LEN, fmt, i);

  NW_ASSERT(len < MAX_CHAR_GROUP_LEN);
	/* To fix TI compiler warning */
	(void) len;

  return ucs22str(buf);
}


str_t float2str(NW_Float32 f)
{
  /* Note: largest formatted NW_Float32 is 13 chars
   * (sign plus 8 digits [24-bit mantissa] plus decimal point
   * plus 'E' plus exponent sign plus 2 digits) */
  NW_Ucs2 buf[MAX_CHAR_GROUP_LEN];
  NW_Ucs2 fmt[8];
  NW_Uint32 len;
  NW_Float64 d = f; /* we use double to match %g c.f. NW_Str_Sprintf */

  NW_Str_StrcpyConst(fmt, "%g");
  len = Snprintf(buf, MAX_CHAR_GROUP_LEN, fmt, d);

  NW_ASSERT(len < MAX_CHAR_GROUP_LEN);
	/* To fix TI compiler warning */
	(void) len;

  return ucs22str(buf);
}


str_t bool2str(NW_Bool b)
{
  NW_Ucs2 bool_str[8];

  if (b)
    NW_Str_StrcpyConst(bool_str, "true");
  else
    NW_Str_StrcpyConst(bool_str, "false");

  return ucs22str(bool_str);
}

/*
** VSWAP wmlscriptlib/string/format/f/10 wants a single precision float to
** end up with an arbitrary number of sig digs.  If value is initially
** specified as string, and it CAN be converted into a float, then just
** strip leading & trailing spaces, apply width and precision including of
** course rounding at the proper position. More appropriate, colorful, and
** unfriendly commentary on this decision would seem unprofessional,
** particularly in a source-code product.  AWalsh 23-Feb-01.
*/
                
NW_Bool ProcessFloatStr(NW_Ucs2 *tmp_buff, NW_Int32 buffLength,
                        NW_Int32 precision, val_t val)
{
  NW_Ucs2   *pEntire;
  NW_Ucs2   *pStart;
  NW_Ucs2   *pTemp;
  NW_Ucs2   *pDecimal = NULL;
  NW_Bool   hasSign = NW_FALSE;
  NW_Int32  numberLen;
  NW_Int32  fracLen;
  NW_Int32  intLen;
  NW_Int32  roundSpot;

  if (!IS_STR(val)) {
    return NW_FALSE;
  }

  /*
  ** subtract 1 from buffLength because we might need to add a character
  ** during rounding.
  */
  buffLength--;

  pEntire = pStart = str2ucs2(val.value.s);

  /* strip leading white space */
  while ( *pStart && NW_Str_Isspace(*pStart) ) {
    pStart++;
  }
  if ( !(*pStart) ) {
    NW_Str_Delete(pEntire);
    return NW_FALSE;
  }
  pTemp = pStart;
  
  /* check to make sure format is [+/-]ddd.ddd */
  if ( (*pTemp == WAE_URL_PLUS_CHAR) || (*pTemp == WAE_URL_DASH_CHAR) ) {
    pStart++;
    pTemp++;
    hasSign = NW_TRUE;
  }
  while ( *pTemp &&
          (NW_Str_Isdigit(*pTemp) || (*pTemp == WAE_URL_DOT_CHAR))  )
  {
    if (*pTemp == WAE_URL_DOT_CHAR) {
      /* two dots is not valid float representation */
      if (pDecimal != NULL) {
        NW_Str_Delete(pEntire);
        return NW_FALSE;
      }
      pDecimal = pTemp;
    }
    pTemp++;
  }
  /*
  ** if we're not at the end, and we've found something other than
  ** whitespace, then we don't have [+/-]ddd.ddd so we can't use our hack.
  ** Also, if the string we're trying to fudge is too long for the buffer,
  ** don't use the hack.
  */
  numberLen = NW_Str_Strlen(pStart) - NW_Str_Strlen(pTemp);
  if ( (*pTemp && !NW_Str_Isspace(*pTemp))
       || ( numberLen >= buffLength ) )
  {
    NW_Str_Delete(pEntire);
    return NW_FALSE;
  }
  /*
  ** now we've got the right format, starting at pStart, ending at pTemp,
  ** possibly containing pDecimal.  Examine precision to decide whether we need
  ** to add zeros, round and/or truncate.
  */
  if (precision == NO_PRECISION) {
    precision = NW_DEFAULT_FLOAT_PREC;
  }
  if (pDecimal != NULL) {
    /* don't count the '.' in fracLen or intLen */
    fracLen = NW_Str_Strlen(pDecimal) - NW_Str_Strlen(pTemp) - 1;
    intLen = numberLen - fracLen - 1;
  } else {
    fracLen = 0;
    intLen = numberLen;
  }
  /*
  ** if we need to pad, and it pushes us over the limit of buffLength then
  ** bail out on our hack.
  */
  if (numberLen - fracLen + precision >= buffLength) {
    NW_Str_Delete(pEntire);
    return NW_FALSE;
  }
  /*
  ** do rounding if necessary - note AWalsh is too lazy to apply the
  ** even/odd rule, so 5 rounds up.  Also note AWalsh is too lazy to worry
  ** about non-ascii character sets and assumes integer values for chars
  ** '0' + 1 = '1', '1' + 1 = '2' etc.
  */
  roundSpot = precision;
  if ( (precision < fracLen) && (pDecimal[roundSpot+1] > '4') ) {
    NW_ASSERT(pDecimal != NULL);
    while (pDecimal[roundSpot] == '9') {
      pDecimal[roundSpot] = '0';
      roundSpot--;
    }
    if (roundSpot == 0) {
      roundSpot = intLen;
      while ( (pStart[roundSpot] == '9') && (roundSpot >= 0) ){
        pStart[roundSpot] = '0';
        roundSpot--;
      }
      if (roundSpot >= 0) {
        pStart[roundSpot]++;
      }
    } else {
      pDecimal[roundSpot]++;
    }
    fracLen = precision;
  }
  /*
  ** now we have enough to create the result.  NOTE: if roundSpot < 0 then
  ** we had all 9's and we need to stick an extra 1 in front of all the 0's
  ** we created while trying to round.
  */
  pTemp = tmp_buff;
  /* copy sign char if we have one */
  if (hasSign) {
    *pTemp = *(pStart-1);
    pTemp++;
  }
  /* if we need to add 1 at the beginning, do so */
  if (roundSpot < 0) {
    *pTemp = '1';
    pTemp++;
  }
  pTemp = NW_Str_Strncpy(pTemp, pStart, intLen);
  if (precision > 0) {
    *pTemp = WAE_URL_DOT_CHAR;
    pTemp++;
    if (fracLen != 0) {
      pTemp = NW_Str_Strncpy(pTemp, pDecimal + 1, fracLen);
    }
    while (fracLen < precision) {
      *pTemp = '0';
      fracLen++;
      pTemp++;
    }
  }
  *pTemp = '\0';
  NW_Str_Delete(pEntire);
  return NW_TRUE;
}

/* This function is to read and check the input format specifier of String.format.
**The specifier form is "%[width] [.precision] type".both width and precision must be
**nonnegative decimal integers.If any of them read is negative, return "invalid".And a 
**special case,the input is "%f" or "%.f", it will keep width = -1 && precision = -1 
**(it means both of them are not specified) and return NW_TRUE.
*/
NW_Bool parse_format(NW_Ucs2 * str, NW_Int32 * width, NW_Int32 * precision,
                   NW_Int32 * type, NW_Int32 * len)
{
  NW_Ucs2 *p = str;
  
  /* read width */
  NW_ASSERT(width);
  if (!p) {
    return NW_FALSE;
  }
  /*if the input is "%-2.0d", return invalid */
  if (NW_Str_Isdigit(*p)) {
    *width = NW_Str_Strtol(p, &p, 10);
    if( *width < 0) {
      return NW_FALSE;
    }
  }
  /* read precision */
  NW_ASSERT(precision);
  if (!p) {
     return NW_FALSE;
  }
  if (*p == '.') {
    ++p;
  }
  if(NW_Str_Isdigit(*p)) {
    *precision = NW_Str_Strtol(p, &p, 10);
     /*if the input is "%.-2d", return invalid */
    if( *precision < 0) {
      return NW_FALSE;
    }
  }
  if (!p) {
    return NW_FALSE;
  }
  NW_ASSERT(type);
  if ((*p == 'd') || (*p == 's') || (*p == 'f')) {
    *type = *p;
  }
  else {
    return NW_FALSE;
  }

  /* 9.16 format section in the WMLScript Spec.
   * When the type is a string:  the spec says:
   *   When the width is larger than the precision,
   * the width should be ignored.  
   *  There is a CR pending on this.  This is the wrong thing
   * to do...  
   *  
   */
  /* Actually, we've been back and forth on this several times.
     The latest consensus is that the spec is probably OK, but
     needs to be clarified.  The basic idea is that precision 
     is doesn't make much sense with strings (%s).  Therefore,
     if precision is specified and width > precision is
     width ignored(or precision is used as the width).  In the
     more normal case only width would be specified and that is
     used as the width.
     jwild 05/17/2000. */
  /*
  ** AWalsh (22-Feb-01): in order to properly ignore width, we must set the
  ** value to 0.  The exact behavior is tested in VSWAP June2000 conformance
  ** wmlscriptlib/string/format/s/6
  */
  if (( *type == 's' ) && (*precision != NO_PRECISION )) {
    if (*width > *precision) {
      *width = 0;
    }
  }
  NW_ASSERT(len);
  *len = p - str + 1;
  return NW_TRUE;
}


NW_Bool set_char_buff(NW_Ucs2 * buff, NW_Int32 index, NW_Ucs2 c)
{
  NW_ASSERT(buff);
  if (index > MAX_STR_LEN) {
    return NW_FALSE;
  }
  buff[index] = c;
  return NW_TRUE;
}


str_t scr_format(NW_Ucs2 * format, val_t val, NW_Bool * retval)
{
  NW_Uint32 j = 0;
  NW_Ucs2 *buff = NULL;
  str_t retString = NULL;
  NW_Bool useFormat = NW_FALSE;
  NW_Ucs2 *p = format;
  NW_Uint16 checkNBytes;

  NW_ASSERT(retval);
  *retval = NW_TRUE;

  /* Create the buffer on the heap to avoid call stack overflow */
  buff = NW_Str_New(MAX_STR_LEN);
  if (buff == NULL) {
    set_error_code(SCR_ERROR_OUT_OF_MEMORY);
    *retval = NW_FALSE;
    return NULL;
  }

  while (*p) {
    if (*p == '%') {
      if (*(++p) == '%') {
        NW_ASSERT(j < MAX_STR_LEN);
        if (!set_char_buff(buff, j++, '%')) {
          *retval = NW_FALSE;
          break;
        }
        p++;
      } else {
        
        NW_Int32 width = NO_PRECISION;
        NW_Int32 precision = NO_WIDTH;
        NW_Int32 type = 0;
        NW_Int32 nbytes = 0;
        NW_Int32 i = 0;
        
        *retval = parse_format(p, &width, &precision, &type, &nbytes);
        if(!(*retval)){
          break;
        }
        /* If width is more than allowed then set width to the max allowed */
        if (width >= MAX_CHAR_GROUP_LEN) {
          width = MAX_CHAR_GROUP_LEN - 1; /* Last byte is to hold null char */
        }

        if (!useFormat) {
          if (type == 's') {
            NW_Int32 formatted_len;
            NW_Int32 len;
            NW_Int32 preceeding_spaces = 0;
            str_t s = 0;
            if (!val2str(val, &s)) {
              *retval = NW_FALSE;
              break;
            }
            len = str_len(s);
            formatted_len = len;

            if (precision >= 0)	/*if precision is specified*/
            {
              if ( (precision >= width) && (width >= len) )
              {
                preceeding_spaces = width - len;
              }
              else
              {
                if (precision < len)
                  formatted_len = precision;
              }
            }
            else if (width >= len)
            {
              preceeding_spaces = width - len;
            }

            for (i = 0; (i<preceeding_spaces) && *retval; i++) {
              NW_ASSERT(j < MAX_STR_LEN);
              if (!set_char_buff(buff, j++, ' ')) {
                *retval = NW_FALSE;
                break;
              }
            }

            for (i = 0; (i < formatted_len) && *retval; i++) {
              NW_ASSERT(j < MAX_STR_LEN);
              if (!set_char_buff(buff, j++, scr_charat(s, i))) {
                *retval = NW_FALSE;
                break;
              }
            }
            free_str(s);
          } else {
            NW_Ucs2 tmp_buff[MAX_CHAR_GROUP_LEN];
            NW_Ucs2 *fexpr = NULL;
            NW_Ucs2 *pPrecision;
            NW_Int32 i2, length;

            tmp_buff[0] = '\0';

            checkNBytes = NW_UINT16_CAST(nbytes + 1);
            i2 = checkNBytes;
            if (i2 != (nbytes + 1)) {
              set_error_code(SCR_ERROR_INVALID_FUNCTION_ARGUMENTS);
              *retval = NW_FALSE;
              break;
            }

            /*
            ** make room for [possible] extra 0 for default precision if a
            ** dot, but not an explicit precision.
            */
            fexpr = NW_Str_New(nbytes + 2);
            if (!fexpr) {
              set_error_code(SCR_ERROR_OUT_OF_MEMORY);
              *retval = NW_FALSE;
              break;
            }

            NW_Str_Strncpy(fexpr, p-1, (nbytes + 1));
            *(fexpr + nbytes + 1) = 0;

            /*
            ** for epoc32 - don't send a dot in the format specifier without
            ** a precision
            */

            pPrecision = NW_Str_Strchr(fexpr, WAE_URL_DOT_CHAR);
            if ( (pPrecision != NULL) && !NW_Str_Isdigit(*(pPrecision+1)) )
            {
              NW_Ucs2 *pTail = NW_Str_Newcpy(pPrecision + 1);
              /* spec says dot without precision same as precision of 0 */
              *(pPrecision + 1) = '0';
              NW_Str_Strcpy((pPrecision + 2), pTail);
              NW_Str_Delete(pTail);
            }
            if (type == 'd') {
              NW_Int32 ival;
              if (IS_FLOAT(val))
              {
                NW_Float32 f;
                if (val2float(val, &f)) {
                  free_val(val);
                  val = new_int_val(NW_Math_truncate(f));
                } 
                else {
                  free_val(val);
                  val = new_invalid_val();
                }
              }
              if (val2int(val, &ival)) {
                if (Snprintf(tmp_buff, MAX_CHAR_GROUP_LEN, fexpr, ival) >= MAX_CHAR_GROUP_LEN) {
                  /* we just trampled memory */
                  NW_ASSERT(NW_FALSE);
                  *retval = NW_FALSE;
                }
              } else
                *retval = NW_FALSE;
            } else if (type == 'f') {
              NW_Float32 fval;

              if (val2float(val, &fval)) {
                /* use NW_Float64 - sprintf expects double as arg for %g & %f */
                NW_Float64 gVal = fval;

                if (!ProcessFloatStr(tmp_buff, MAX_CHAR_GROUP_LEN, precision, val))
                {
                  if (Snprintf(tmp_buff, MAX_CHAR_GROUP_LEN, fexpr, gVal) >= MAX_CHAR_GROUP_LEN) {
                    /* we just trampled memory */
                    NW_ASSERT(NW_FALSE);
                    *retval = NW_FALSE;
                  }
                }
                  
              } else {
                *retval = NW_FALSE;
              }
            } else {
              *retval = NW_FALSE;
            }

            length = NW_Str_Strlen(tmp_buff);

            for (i = 0; (i < width - length) && *retval; i++) {
              NW_ASSERT(j < MAX_STR_LEN);
              if (!set_char_buff(buff, j++, ' ')) {
                *retval = NW_FALSE;
                break;
              }
            }

            if (*retval) {
              for (i = 0; i < length; i++) {
                NW_ASSERT(j < MAX_STR_LEN);
                if (!set_char_buff(buff, j++, tmp_buff[i])) {
                  *retval = NW_FALSE;
                  break;
                }
              }
            }
            NW_Str_Delete(fexpr);
          }
          useFormat = NW_TRUE;
        }
        p += nbytes;
      }
    } else {
      if(j < MAX_STR_LEN)
      {
        if (!set_char_buff(buff, j++, *(p++)))
        {
          *retval = NW_FALSE;
          break;
        }
      }
      else
      {
        *retval = NW_FALSE;
        NW_Str_Delete(buff);
        return NULL;
      }
    }
  }

  if (*retval == NW_TRUE) {
    NW_ASSERT(j < MAX_STR_LEN);
    buff[j] = '\0';
    retString = ucs22str(buff);
  } else {
    retString = NULL;
  }

  NW_Str_Delete(buff);
  return retString;
}


str_t ucs2ch2str(NW_Ucs2 ch)
{
  str_t res = 0;

  res = new_str(1, NULL);
  if (!res) {
    set_error_code(SCR_ERROR_OUT_OF_MEMORY);
    return 0;
  }

  res[1] = ch;

  return res;
}
