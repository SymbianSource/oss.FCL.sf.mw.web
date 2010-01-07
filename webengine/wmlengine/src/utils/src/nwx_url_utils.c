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
**    File name:  nwx_url_utils.c
**    Part of: Url Utils (NW)
**    Description:   This file implements parsing of the URL according
**      to the standards defined in RFC 2396.
******************************************************************/

/*
**-------------------------------------------------------------------------
**  Include Files
**-------------------------------------------------------------------------
*/
#include "nwx_url_utils.h"

#include "nwx_mem.h"
#include "nwx_string.h"
#include "nw_wae_reader.h"
#include "nwx_settings.h"
#include "nwx_http_defs.h"
#include "BrsrStatusCodes.h"

/*lint -save -e794, -e574 Conceivable use of null pointer, Signed-unsigned mix with relational*/

/*
**-------------------------------------------------------------------------
**  Constants
**-------------------------------------------------------------------------
*/
static const NW_Ucs2 HEXDIGITS[] = {'0', '1', '2', '3',
                                        '4', '5', '6', '7',
                                        '8', '9', 'A', 'B',
                                        'C', 'D', 'E', 'F' };

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
static void escape_one_char(const NW_Ucs2 *src, NW_Ucs2 **destPtr, NW_Bool escapeReservedDelimiters);

static NW_Bool escape_char( const NW_Ucs2 *src, NW_Ucs2 *dest, NW_Bool escapeReservedDelimiters);

static NW_Bool unescape_char( const NW_Ucs2 *src, NW_Ucs2 *dest);

static TBrowserStatusCode build_url( const NW_Ucs2 *scheme, const NW_Ucs2 *net_loc,
                              const NW_Ucs2 *path, const NW_Ucs2 *query,
                              const NW_Ucs2 *frag, NW_Ucs2 **ret_string);

static NW_Url_Schema_t returnschema(const NW_Ucs2 *scheme);

static NW_Bool is_legal_escaped_chars(const NW_Ucs2* s);

static NW_Bool is_legal_escaped_string(const NW_Ucs2* s);

static NW_Uint32 writeForeignCharBuff(const NW_Byte * c, NW_Byte *buff);


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
static NW_Bool NW_Url_IsSupportedEncoding(NW_Uint32 encoding)
{
    switch (encoding)
    {
        case HTTP_gb2312:
        case HTTP_big5:
        case HTTP_iso_8859_8:
        case HTTP_iso_8859_6:
        case HTTP_windows_1255:
        case HTTP_windows_1256:
        case HTTP_windows_1250:
        case HTTP_windows_1251:
        case HTTP_windows_1253:
        case HTTP_windows_1254:
        case HTTP_windows_1257:
        case HTTP_iso_8859_1:
        case HTTP_iso_8859_2:
        case HTTP_iso_8859_3:
        case HTTP_iso_8859_4:
        case HTTP_iso_8859_5:
        case HTTP_iso_8859_9:
        case HTTP_iso_8859_7:
        case HTTP_tis_620:  // not flagged because we will not get here unless tis_620 is enabled
        case HTTP_shift_JIS:
        case HTTP_jis_x0201_1997:
        case HTTP_jis_x0208_1997:
        case HTTP_euc_jp:
        case HTTP_iso_2022_jp:
        case HTTP_windows_874:
        case HTTP_Koi8_r:
        case HTTP_Koi8_u:
            return NW_TRUE;
        default:
            return NW_FALSE;
    }
}


static NW_Bool is_legal_escaped_chars(const NW_Ucs2* s)
{
  NW_Bool isLegal = NW_FALSE;

  NW_ASSERT(s);

  if ( s[0] == WAE_URL_ESCAPE_CHAR )
  {
    /* Found escape char.  Check for 2 following hex digits. */
    if ( NW_Str_Isxdigit(s[1]) && NW_Str_Isxdigit(s[2]) )
    {
      /* Legal escape char sequence. */
      isLegal = NW_TRUE;
    }
  }

  return isLegal;
}

static NW_Bool is_legal_escaped_string(const NW_Ucs2* s)
{
  NW_Bool isLegal = NW_TRUE;
  NW_Int32  i = 0;

  NW_ASSERT(s);

  while ( isLegal && ( s[i] != 0 ) )
  {
    if ( s[i] == WAE_URL_ESCAPE_CHAR )
    {
      /* Found escape char.  Check for
      2 following hex digits. */
      if ( is_legal_escaped_chars( &s[i] ) )
      {
        /* Legal escape char sequence.  Increment
        and continue checking. */
        i = i + 3;
      }
      else
      {
        /* Illegal escape sequence.  Fail. */
        isLegal = NW_FALSE;
      }
    }
    else
    {
      /*
      if (s[i] > 127) {
        isLegal = NW_FALSE;
      } else
      */
      i++;   /* Not escape.  Increment and continue checking. */
    }
  }
  return isLegal;
}
static NW_Bool is_uric_char(const NW_Ucs2 *pCh)
{
  if (NW_Str_Isalpha(*pCh)                         ||
      NW_Str_Isdigit(*pCh)                         ||
      is_legal_escaped_chars(pCh)                  ||  /* %xx */
      (*pCh == WAE_URL_PARAM_CHAR)                 ||  /* ; */
      (*pCh == WAE_URL_PATH_CHAR)                  ||  /* / */
      (*pCh == WAE_URL_QUERY_CHAR)                 ||  /* ? */
      (*pCh == WAE_URL_COLON_CHAR)                 ||  /* : */
      (*pCh == WAE_URL_AT_CHAR)                    ||  /* @ */
      (*pCh == WAE_URL_AMP_CHAR)                   ||  /* & */
      (*pCh == WAE_URL_EQUAL_CHAR)                 ||  /* = */
      (*pCh == WAE_URL_PLUS_CHAR)                  ||  /* + */
      (*pCh == WAE_URL_DOLLAR_CHAR)                ||  /* $ */
      (*pCh == WAE_URL_COMMA_CHAR)                 ||  /* , */
      (*pCh == WAE_URL_LEFT_SQUARE_BRACKET_CHAR)   ||  /* [ */
      (*pCh == WAE_URL_RIGHT_SQUARE_BRACKET_CHAR)  ||  /* ] */
      (*pCh == WAE_URL_DASH_CHAR)                  ||  /* - */
      (*pCh == WAE_URL_UNDER_CHAR)                 ||  /* _ */
      (*pCh == WAE_URL_DOT_CHAR)                   ||  /* . */
      (*pCh == WAE_URL_BANG_CHAR)                  ||  /* ! */
      (*pCh == WAE_URL_TILDE_CHAR)                 ||  /* ~ */
      (*pCh == WAE_URL_STAR_CHAR)                  ||  /* * */
      (*pCh == WAE_URL_APOST_CHAR)                 ||  /* ' */
      (*pCh == WAE_URL_PAREN_CHAR)                 ||  /* ( */
      (*pCh == WAE_URL_THESIS_CHAR) )                  /* ) */
    {
      return NW_TRUE;
    } else {
      return NW_FALSE;
    }
}
/*
**-------------------------------------------------------------------------
**  External Public (Exported) Functions
**-------------------------------------------------------------------------
*/
/* as per appendix A, rfc2396, URI-reference */
NW_Bool NW_Url_GetRfc2396Parts( const NW_Ucs2 *pUri, NW_Rfc2396_Parts_t *parts )
    {
    const NW_Ucs2 *pCh      = pUri;
    const NW_Ucs2 *pStart   = pUri;
    const NW_Ucs2 *pEnd     = NULL;
    const NW_Ucs2 *pAtChar  = NULL;
    NW_Bool isBrowserScheme;
    NW_Bool isIPv6Address = NW_FALSE;
    TBrowserStatusCode status;
    NW_Url_Schema_t schema;

    if ((pUri == NULL) || (NW_Str_Strlen(pUri) == 0))
        {
        return NW_FALSE;
        }

    /* If and only if the schema is NOT WTAI, then do simple check to make sure
       all chars are valid in the URI. WTAI does not use a valid URI since
       foreign chars are not encoded but sent as UCS2. */
    status = NW_Url_GetSchemeNoValidation(pUri, &schema);
	if ( (status != KBrsrOutOfMemory) && (schema != NW_SCHEMA_WTAI) )
        {
        while (*pCh != 0)
            {
            /* stop checking for valid uric chars at fragment */
            if (*pCh == WAE_URL_FRAG_CHAR)
                {
                pCh++;
                }
            if (!is_uric_char(pCh) )
                {
                return NW_FALSE;
                }
            /* stop checking the valid uric char for query string */
            if (*pCh == WAE_URL_QUERY_CHAR)
                {
                break;
                }
            pCh++;
            }   // end of while
        }

    parts->schemeStart    = NULL;
    parts->opaqueStart    = NULL;
    parts->authorityStart = NULL;
    parts->userInfoStart  = NULL;
    parts->hostStart      = NULL;
    parts->portStart      = NULL;
    parts->pathStart      = NULL;
    parts->queryStart     = NULL;
    parts->fragStart      = NULL;
    parts->isIPAddress    = NW_FALSE;

    NW_Url_IsBrowserScheme(pUri, &isBrowserScheme);

    pCh = pStart;
    parts->fragStart  = NW_Str_Strchr(pCh, WAE_URL_FRAG_CHAR);
    parts->queryStart = NW_Str_Strchr(pCh, WAE_URL_QUERY_CHAR);

    /* Browser schema can only have one frag, wtai can have multiple # chars */
    if (isBrowserScheme && (parts->fragStart != NULL) &&
        (NW_Str_Strchr(parts->fragStart+1, WAE_URL_FRAG_CHAR) != NULL))
        {
        return NW_FALSE;
        }
    else
        {
        pEnd = pUri + NW_Str_Strlen(pUri);
        if (parts->fragStart != NULL)
            {
            parts->fragEnd = pEnd;
            pEnd = parts->fragStart++;
            }
        }

    /* fragment can contain query char, but not the other way 'round */
    if ((parts->queryStart != NULL) && (parts->fragStart != NULL) &&
        (parts->queryStart >= parts->fragStart))
        {
        parts->queryStart = NULL;
        }
    else if (parts->queryStart != NULL)
        {
        parts->queryEnd = pEnd;
        pEnd = parts->queryStart++;
        }

    /* we've stripped off frag and query, so current pEnd will be end of */
    /* path if a path exists. */
    parts->pathEnd = pEnd;

    /* see if we have a scheme */
    parts->schemeStart = parts->schemeEnd = NW_Str_Strchr(pCh, WAE_URL_COLON_CHAR);
    if (parts->schemeStart != NULL)
        {
        if (parts->schemeStart > pEnd)
            {
            /* here the first colon shows up in a query or frag so it doesn't */
            /* represent the end of a scheme string */
            parts->schemeStart = NULL;
            }
        else /* now check for valid scheme string */
            {
            if (!NW_Str_Isalpha(*pCh))
                {
                parts->schemeStart = NULL;
                }
            else
                {
                while (pCh < parts->schemeStart)
                    {
                    if (!NW_Str_Isalpha(*pCh) && !NW_Str_Isdigit(*pCh) &&
                        (*pCh != WAE_URL_PLUS_CHAR) &&
                        (*pCh != WAE_URL_DASH_CHAR) &&
                        (*pCh != WAE_URL_DOT_CHAR))
                        {
                        parts->schemeStart = NULL;
                        }
                    pCh++;
                    }
                }
            }
        if (parts->schemeStart != NULL)
            {
            parts->schemeStart = pStart;
            pCh++; /* go one past colon character */
            pStart = pCh;
            }
        else
            {
            pCh = pStart;
            }
        }

    /* if we've got a scheme, handle possible opaque_part first */
    if ((parts->schemeStart != NULL) && (*pCh != WAE_URL_PATH_CHAR))
        {
        /* only restriction on opaque_part is >= 1 non-frag char, and doesn't */
        /* start with a slash */
        parts->queryStart = NULL;
        if (*pCh != 0)
            {
            parts->opaqueStart = pCh;
            parts->opaqueEnd = pEnd;
            }
        else
            {
            parts->opaqueStart = NULL;
            }

        // We can handle valid schemes with no opaque_part. i.e. "sms:", etc...
        return NW_TRUE;
        }
    else
        {
        /* we've got no scheme, or a scheme followed by net_path or abs_path.
        net_path, abs_path or rel_path. net_path & abs_path start with '/' */
        parts->pathStart = NULL;
        if (*pCh != WAE_URL_PATH_CHAR)
            {
            /* must have rel_path */
            NW_ASSERT(parts->schemeStart == NULL);
            parts->pathStart = NW_Str_Strchr(pCh, WAE_URL_PATH_CHAR);
            if ((parts->pathStart == NULL) || (parts->pathStart > pEnd))
                {
                parts->pathStart = pEnd;
                }

            while (pCh < parts->pathStart)
                {
                /* we know is_uric_char(*pCh) and it's not frag, query or path char */
                /* only other char not possible in rel_segment is colon */
                if (*pCh == WAE_URL_COLON_CHAR)
                    {
                    return NW_FALSE;
                    }
                pCh++;
                }   // end of while

            /* rel_segment must have at least one char */
            if (pCh > pStart)
                {
                parts->pathStart = pStart;
                parts->pathEnd = pEnd;
                }
            else
                {
                parts->pathStart = NULL;
                /* we've got a relativeURI but no path, and we've already taken */
                /* care of any query and fragment, so just return */
                return NW_TRUE;
                }
            }

        /* will be (net_path | abs_path) [ ? query ] */
        if ((*pCh != 0) && (*(pCh+1) == WAE_URL_PATH_CHAR) && (parts->pathStart == NULL))
            {
            /* handle net_path case */
            pCh += 2;    /* point at next char after "//" */
            parts->authorityStart = pCh;
            parts->pathStart = NW_Str_Strchr(pCh, WAE_URL_PATH_CHAR);
            if ((parts->pathStart == NULL) || (parts->pathStart > pEnd))
                {
                parts->pathStart = NULL;
                parts->authorityEnd = pEnd;
                }
            else
                {
                parts->authorityEnd = parts->pathStart;
                }

            /* check to see if this is an ipv6 address */
            if (*pCh == WAE_URL_LEFT_SQUARE_BRACKET_CHAR)
                {
                isIPv6Address = NW_TRUE;
                }

            /* net_path = "//" authority [abs_path] so look for "//" authority */
            /* authority = server | reg_name.  Server can be empty but reg_name */
            /* has at least one character, so we're really just looking to see */
            /* if this parses properly as [userinfo @] hostport */

            /* if we have an @ char, see if we have userinfo @ hostport */
            /* if userinfo is valid pCh will point one past the @ character */
            /* otherwise will not change.  userinfo can be null, or any valid */
            /* char other than / @ ? so we don't need to explicitly check -- */
            /* we've already found first occurrence of those 3 chars. */
            pAtChar = NW_Str_Strchr(pCh, WAE_URL_AT_CHAR);

            if ((pAtChar != NULL) && (pAtChar > parts->authorityEnd))
                {
                pAtChar = parts->userInfoStart = NULL;
                }
            else if (pAtChar != NULL)
                {
                parts->userInfoStart = pCh;
                parts->userInfoEnd = pAtChar;
                pCh = pAtChar + 1;
                }
            else
                {
                parts->userInfoStart = NULL;
                }

            /* now look for hostport -- if we have : check for valid port */
            if (isIPv6Address)
                {
                const NW_Ucs2 ipv6PortSepStr[] = {']', ':'};
                parts->portStart = NW_Str_Strstr(pCh, ipv6PortSepStr);
                if (parts->portStart != NULL)
                    {
                    /* portStart should begin at the : not the ], so must point
                    to the character after the one returned by NW_Str_Strstr */
                    parts->portStart += 1;
                    }

                }
            else
                {
                parts->portStart = NW_Str_Strchr(pCh, WAE_URL_COLON_CHAR);
                }

            /* port is *digit, which means it might be empty */
            if ((parts->portStart != NULL) && (parts->portStart < parts->authorityEnd))
                {
                pStart = parts->portStart;
                parts->portStart++;
                parts->portEnd = parts->portStart;
                while ( (parts->portEnd < parts->authorityEnd) &&
                    NW_Str_Isdigit(*parts->portEnd) )
                    {
                    parts->portEnd++;
                    }
                /* now if we ate all the digits, and we're not at a path, */
                /* query, frag, or end of uri then we can't make a valid */
                /* uri interpreting the current substring as */
                /* : port [abs_path] [? query] [# fragment] so indicate error */
                /* NOTE: since we have scheme:// remainder MUST conform to valid */
                /* hostname:port otherwise we have invalid URL */
                if (parts->portEnd != parts->authorityEnd)
                    {
                    return NW_FALSE;
                    }
                }
            else
                {
                parts->portStart = parts->authorityEnd;
                }

            /* portStart is overloaded -- NULL means we found ":" but not a valid */
            /* port #, portStart == authorityEnd means there is no ":" */
            if (parts->portStart != NULL)
                {
#define SEGARRAY_SIZE 4
                const NW_Ucs2 *segArray[SEGARRAY_SIZE];
                NW_Int32 nDots = 0;
                NW_Int32 nonDigits = 0;

                /* now look for hostname or IPv4address */
                parts->hostStart = pCh;
                if (parts->portStart == parts->authorityEnd)
                    {
                    parts->hostEnd = parts->portStart;
                    }
                else
                    {
                    parts->hostEnd = parts->portStart - 1;
                    NW_ASSERT(*parts->hostEnd == WAE_URL_COLON_CHAR);
                    }
                pCh = parts->portStart - 1;

                while (pCh >= parts->hostStart)
                    {
                    if (*pCh == WAE_URL_DOT_CHAR)
                        {
                        if (nDots < SEGARRAY_SIZE)
                            {
                            segArray[nDots] = pCh;
                            }
                        nDots++;
                        }
                    else if (!NW_Str_Isdigit(*pCh))
                        {
                        nonDigits++;
                        }
                    pCh--;
                    }   // end of while

                /* since portStart was overloaded, set to NULL if no valid port */
                if (parts->portStart == parts->authorityEnd)
                    {
                    parts->portStart = NULL;
                    }

                /* this may be IPv4address -- so make sure at least one */
                /* digit between each pair of dots and a digit at the */
                /* beginning and end. */
                if ((nDots == 3) && (nonDigits == 0))
                    {
                    /*lint --e{644} Variable may not have been initialized*/
                    if (((segArray[1] + 1) != segArray[0]) &&
                        ((segArray[2] + 1) != segArray[1]) &&
                        NW_Str_Isdigit(*parts->hostStart) &&
                        NW_Str_Isdigit(*(parts->hostEnd - 1)))
                        {
                        parts->isIPAddress = NW_TRUE;
                        return NW_TRUE;
                        }
                    }
                else
                    {
                    /* better have *(domainlabel ".") toplabel ["."] */
                    /* domain is less restrictive than top, so if we set domain */
                    /* false we will also set top false, so to simplify test at */
                    /* end, make sure all substrings in host are validDomain. */
                    NW_Bool validDomain = NW_TRUE;
                    NW_Bool validTop = NW_FALSE;

                    pCh = parts->hostStart;
                    while (pCh < parts->hostEnd) /* outer */
                        {
                        if (NW_Str_Isdigit(*pCh))
                            {
                            validDomain = NW_TRUE;
                            }
                        else if (NW_Str_Isalpha(*pCh))
                            {
                            validTop = NW_TRUE;
                            }

                        pCh++;
                        while ((pCh < parts->hostEnd) && (*pCh != WAE_URL_DOT_CHAR))
                            {
                            if (!NW_Str_Isalpha(*pCh) &&
                                !NW_Str_Isdigit(*pCh) )
                                {
                                /* if we're at the end of the host, or if the next */
                                /* char is a dot we need alphanum for valid label, but */
                                /* if we're in the interior, we can also accept dash */
                                if (((pCh == (parts->hostEnd - 1)) ||
                                    (*(pCh + 1) == WAE_URL_DOT_CHAR)) ||
                                    (*pCh != WAE_URL_DASH_CHAR))
                                    {
                                    validTop = NW_FALSE;
                                    validDomain = NW_FALSE;
                                    }
                                }
                            pCh++;
                            }   // end of inner while

                        /* we're at dot or hostEnd, bump by one */
                        pCh++;
                        }   // end of outer while

                    if (!validDomain || !validTop)
                        {
                        parts->hostStart = parts->userInfoStart = pAtChar = NULL;
                        }
                    }
                }   // end of if (parts->portStart != NULL)
            }
        else
            {
            /* we've got abs_path only, no auth */
            if (parts->pathStart == NULL)
                {
                parts->pathStart = pCh;
                }
            }   // end of if-else ((*pCh != 0) && (*(pCh+1)...
        }   // end of if ((parts->schemeStart != NULL) &&...

    return NW_TRUE;
}

/*
* RETURN: KBrsrSuccess
*         KBrsrFailure - string has NO fragment
*         KBrsrOutOfMemory
*         KBrsrMalformedUrl
*/
TBrowserStatusCode NW_Url_GetFragId(const NW_Ucs2 *url_buff, NW_Ucs2 **ret_string, NW_Bool incFragChar)
{
  const NW_Ucs2 *pStart = NULL;
  const NW_Ucs2 *pEnd   = NULL;
  NW_Rfc2396_Parts_t urlPieces;

  NW_ASSERT(url_buff);
  NW_ASSERT(ret_string);
  *ret_string = NULL;

  if (!NW_Url_GetRfc2396Parts(url_buff, &urlPieces))
  {
    return KBrsrMalformedUrl;
  }

  if (urlPieces.fragStart == NULL)
  {
    return KBrsrFailure;
  }

  pStart = urlPieces.fragStart;

  if (incFragChar == NW_TRUE) {
    pStart--;
  }

  pEnd = urlPieces.fragEnd;

  NW_ASSERT(pEnd >= pStart);
  *ret_string = NW_Str_Substr(pStart, 0, ((NW_Uint32)(pEnd - pStart)));
  if (*ret_string == NULL)
    return KBrsrOutOfMemory;

  return KBrsrSuccess;
}


/*
* returns predefined scheme
*
* RETURN: KBrsrSuccess
*          KBrsrFailure - bad url_buff
*                    - url_buff does not contain a scheme
*       KBrsrOutOfMemory
*
* MODIFIED: ret_scheme
*/
TBrowserStatusCode NW_Url_GetScheme(const NW_Ucs2 *url_buff,
                             NW_Url_Schema_t *ret_scheme)
{
  const NW_Ucs2 *pStart = NULL;
  const NW_Ucs2 *pEnd   = NULL;
  NW_Ucs2 *pBuff  = NULL;

  NW_Rfc2396_Parts_t urlPieces;

  NW_ASSERT(url_buff);
  NW_ASSERT(ret_scheme);

  *ret_scheme = NW_SCHEMA_INVALID;

  if (!NW_Url_GetRfc2396Parts(url_buff, &urlPieces))
  {
    return KBrsrMalformedUrl;
  }

  if (urlPieces.schemeStart == NULL)
  {
    return KBrsrFailure;
  }

  pStart = urlPieces.schemeStart;
  pEnd = urlPieces.schemeEnd;

  /* extract the scheme string */
  NW_ASSERT(pEnd >= pStart);
  pBuff = NW_Str_Substr(pStart, 0, ((NW_Uint32)(pEnd - pStart)));
  if (pBuff == NULL)
    return KBrsrOutOfMemory;

  /*
  ** RFC2396 sect 3.1 says "programs interpreting URI should treat upper
  ** case letters as equivalient to lower case in scheme names"
  */
  NW_Str_Strlwr(pBuff);

  *ret_scheme = returnschema(pBuff);

  NW_Str_Delete(pBuff);
  return KBrsrSuccess;
}



/*
* RETURN: KBrsrSuccess
*       KBrsrFailure - string contains NO network location
*       KBrsrOutOfMemory
*       KBrsrMalformedUrl
*/
TBrowserStatusCode NW_Url_GetNetwork(const NW_Ucs2 *url_buff, NW_Ucs2 **ret_string)
{
  const NW_Ucs2 *pStart = NULL;
  const NW_Ucs2 *pEnd   = NULL;

  NW_Rfc2396_Parts_t urlPieces;

  NW_ASSERT(url_buff);
  NW_ASSERT(ret_string);
  *ret_string = NULL;

  if (!NW_Url_GetRfc2396Parts(url_buff, &urlPieces))
  {
    return KBrsrMalformedUrl;
  }

  if (urlPieces.authorityStart == NULL)
  {
    return KBrsrFailure;
  }

  pStart = urlPieces.authorityStart;
  pEnd = urlPieces.authorityEnd;

  if (pStart != NULL) {
    NW_ASSERT(pEnd >= pStart);
    *ret_string = NW_Str_Substr(pStart, 0, ((NW_Uint32)(pEnd - pStart)));
    if (*ret_string == NULL) {
      return KBrsrOutOfMemory;
    }
  }
  return KBrsrSuccess;
}

/*
* RETURN: KBrsrSuccess
*       KBrsrFailure - string contains NO query
*       KBrsrOutOfMemory
*/
TBrowserStatusCode NW_Url_GetQuery(const NW_Ucs2 *url_buff, NW_Ucs2 **ret_string)
{
  const NW_Ucs2 *pStart = NULL;
  const NW_Ucs2 *pEnd   = NULL;

  NW_Rfc2396_Parts_t urlPieces;

  NW_ASSERT(url_buff);
  NW_ASSERT(ret_string);
  *ret_string = NULL;

  if (!NW_Url_GetRfc2396Parts(url_buff, &urlPieces))
  {
    return KBrsrMalformedUrl;
  }

  if (urlPieces.queryStart == NULL)
  {
    return KBrsrFailure;
  }

  pStart = urlPieces.queryStart;
  pEnd = urlPieces.queryEnd;
  NW_ASSERT(pEnd >= pStart);
  *ret_string = NW_Str_Substr(pStart, 0, ((NW_Uint32)(pEnd - pStart)));

  if (*ret_string == NULL) {
    return KBrsrOutOfMemory;
  }

  return KBrsrSuccess;
}

/*****************************************************************
  Name:         NW_Url_AddPostfields
  Description:  Adds postfileds string to a Url with out postfilds
                Note: the input url must not have postfields
  Parameters:   url (may have a fragment)
                postfields portion of a Url
                pointer to full url pointer location
  Algorithm:    Alocates memory for fullUrl that includes postfilds.
                Parses the input url for base and fragment.
                builds fullUrl by concat base+query separator+fragment
  Return Value: The returned fullUrl is not Null only in case
                if KBrsrSuccess success is returned
                Other return statuses:
                  KBrsrOutOfMemory,
                  KBrsrFailure
******************************************************************/
TBrowserStatusCode NW_Url_AddPostfields(const NW_Ucs2 *url, const NW_Ucs2 *postfields, NW_Ucs2 **fullUrl)
{
  NW_Ucs2     *pBase     = NULL;
  NW_Ucs2     *pFragment = NULL;
  TBrowserStatusCode status;

  *fullUrl = NW_Str_New(NW_Str_Strlen(url) + NW_Str_Strlen(postfields) +
                        NW_Asc_strlen(WAE_URL_QUERY_SEP));
  if (!fullUrl) {
    status = KBrsrOutOfMemory;
      NW_THROW_ERROR();
  }
  status = NW_Url_GetBase((const NW_Ucs2 *)url, &pBase);
  if (status != KBrsrSuccess) {
      NW_THROW_ERROR();
  }

  status = NW_Url_GetFragId((const NW_Ucs2 *)url, &pFragment, NW_TRUE);
  if (status != KBrsrSuccess && status != KBrsrFailure) {
      NW_THROW_ERROR();
  }

  (void)NW_Str_Strcpy(*fullUrl, (const NW_Ucs2 *)pBase);
  (void)NW_Str_StrcatConst(*fullUrl, WAE_URL_QUERY_SEP);
  (void)NW_Str_Strcat(*fullUrl, (const NW_Ucs2 *)postfields);
  if (pFragment) {
    (void)NW_Str_Strcat(*fullUrl, (const NW_Ucs2 *)pFragment);
  }
  status = KBrsrSuccess;

NW_CATCH_ERROR
  if (status != KBrsrSuccess) {
    NW_Str_Delete(*fullUrl);
    *fullUrl = NULL;
  }
  NW_Str_Delete(pBase);
  NW_Str_Delete(pFragment);
  return status;
}

/*
* RETURN: KBrsrSuccess
*          KBrsrFailure - url_buff has NO path
*          KBrsrOutOfMemory
*          KBrsrMalformedUrl
*/
TBrowserStatusCode NW_Url_GetHost(const NW_Ucs2 *url_buff, NW_Ucs2 **ret_string)
{
  const NW_Ucs2 *pStart = NULL;
  const NW_Ucs2 *pEnd   = NULL;

  NW_Rfc2396_Parts_t urlPieces;

  NW_ASSERT(url_buff);
  NW_ASSERT(ret_string);
  *ret_string = NULL;

  if (!NW_Url_GetRfc2396Parts(url_buff, &urlPieces))
  {
    return KBrsrMalformedUrl;
  }

  if (urlPieces.hostStart == NULL)
  {
    return KBrsrFailure;
  }

  pStart = urlPieces.hostStart;
  pEnd = urlPieces.hostEnd;

  if (pStart != NULL)
  {
    NW_ASSERT(pEnd >= pStart);
    *ret_string = NW_Str_Substr(pStart, 0, ((NW_Uint32)(pEnd - pStart)));
    if (*ret_string == NULL)
      return KBrsrOutOfMemory;
  }

  return KBrsrSuccess;
}

/*
* RETURN: KBrsrSuccess
*          KBrsrFailure - url_buff has NO path
*       KBrsrOutOfMemory
*/
TBrowserStatusCode NW_Url_GetPath(const NW_Ucs2 *url_buff, NW_Ucs2 **ret_string)
{
  const NW_Ucs2 *pStart = NULL;
  const NW_Ucs2 *pEnd   = NULL;

  NW_Rfc2396_Parts_t urlPieces;

  NW_ASSERT(url_buff);
  NW_ASSERT(ret_string);
  *ret_string = NULL;

  if (!NW_Url_GetRfc2396Parts(url_buff, &urlPieces))
  {
    return KBrsrMalformedUrl;
  }

  if (urlPieces.pathStart == NULL)
  {
    return KBrsrFailure;
  }

  pStart = urlPieces.pathStart;
  pEnd = urlPieces.pathEnd;

  if (pStart != NULL)
  {
    NW_ASSERT(pEnd >= pStart);
    *ret_string = NW_Str_Substr(pStart, 0, ((NW_Uint32)(pEnd - pStart)));
    if (*ret_string == NULL)
      return KBrsrOutOfMemory;
  }

  return KBrsrSuccess;
}


/*
* RETURN: KBrsrSuccess
*          KBrsrFailure - url_buff has NO path
*       KBrsrOutOfMemory
*/
TBrowserStatusCode NW_Url_GetPathNoParam(const NW_Ucs2 *url_buff, NW_Ucs2 **ret_string)
{
  const NW_Ucs2 *pStart = NULL;
  const NW_Ucs2 *pEnd   = NULL;
  NW_Ucs2       *pRet   = NULL;

  NW_Rfc2396_Parts_t urlPieces;

  NW_ASSERT(url_buff);
  NW_ASSERT(ret_string);
  *ret_string = NULL;

  if (!NW_Url_GetRfc2396Parts(url_buff, &urlPieces))
  {
    return KBrsrMalformedUrl;
  }

  if (urlPieces.pathStart == NULL)
  {
    return KBrsrFailure;
  }

  pStart = urlPieces.pathStart;
  pEnd = urlPieces.pathEnd;

  if (pStart != NULL)
  {
    NW_ASSERT(pEnd >= pStart);
    *ret_string = NW_Str_New((NW_Uint32)(pEnd - pStart));
    if (*ret_string == NULL)
      return KBrsrOutOfMemory;

    pRet = *ret_string;
    while (pStart < pEnd) {
      if (*pStart == WAE_URL_PARAM_CHAR) {
        while ((*pStart != WAE_URL_PATH_CHAR) && (pStart < pEnd))
        {
          pStart++;
        }
      }
      if (pStart < pEnd) {
        *pRet = *pStart;
        pRet++;
        pStart++;
      }
    }
    *pRet = 0;
  }

  return KBrsrSuccess;
}

/*
* RETURN: KBrsrSuccess
*       KBrsrOutOfMemory
*/
TBrowserStatusCode NW_Url_GetSchemeStr( const NW_Ucs2 *url_buff, NW_Ucs2 **ret_string)
{
  const NW_Ucs2 *pStart = NULL;
  const NW_Ucs2 *pEnd   = NULL;

  NW_Rfc2396_Parts_t urlPieces;

  NW_ASSERT(url_buff);
  NW_ASSERT(ret_string);
  *ret_string = NULL;

  if (!NW_Url_GetRfc2396Parts(url_buff, &urlPieces))
  {
    return KBrsrMalformedUrl;
  }

  pStart = urlPieces.schemeStart;
  pEnd   = urlPieces.schemeEnd;
  if (pStart == NULL)
  {
    return KBrsrSuccess;
  }

  NW_ASSERT(pEnd >= pStart);
  *ret_string = NW_Str_Substr(pStart, 0, ((NW_Uint32)(pEnd - pStart)));
  if (*ret_string == NULL) {
    return KBrsrOutOfMemory;
  }

  return KBrsrSuccess;
}

/*
* this function returns the base of a fully qualified URL (e.g.
* it rips off the card name).
*
* It assumes that the last portion of a URL is deck.
*
* RETURN: KBrsrSuccess
*       KBrsrOutOfMemory
*/
TBrowserStatusCode NW_Url_GetBase(const NW_Ucs2 *url_buff, NW_Ucs2 **ret_string)
{
  const NW_Ucs2 *pStart = NULL;
  const NW_Ucs2 *pEnd   = NULL;

  NW_Rfc2396_Parts_t urlPieces;

  NW_ASSERT(url_buff);
  NW_ASSERT(ret_string);
  *ret_string = NULL;

  if (!NW_Url_GetRfc2396Parts(url_buff, &urlPieces))
  {
    return KBrsrMalformedUrl;
  }

  /* fragStart points one past '#' if '#' exists */
  pEnd = urlPieces.fragStart;

  if (pEnd == NULL)
  {
    *ret_string = NW_Str_Newcpy(url_buff);
    if (*ret_string == NULL)
      return KBrsrOutOfMemory;

    return KBrsrSuccess;
  }
  else
  {
    pEnd--;
    NW_ASSERT(*pEnd == WAE_URL_FRAG_CHAR);
  }

  pStart = url_buff;

  NW_ASSERT(pEnd >= pStart);

  *ret_string = NW_Str_Substr(pStart, 0, ((NW_Uint32)(pEnd - pStart)));
  if (*ret_string == NULL)
    return KBrsrOutOfMemory;

  return KBrsrSuccess;
}

/*
* RETURN: KBrsrSuccess
*          KBrsrFailure - the string has NO port
*       KBrsrOutOfMemory
*/
TBrowserStatusCode NW_Url_GetPort(const NW_Ucs2 *url_buff, NW_Bool *is_valid,
                           NW_Ucs2 **ret_string)
{
  const NW_Ucs2   *pStart;
  const NW_Ucs2   *pEnd;

  NW_Rfc2396_Parts_t urlPieces;

  NW_ASSERT(url_buff);
  NW_ASSERT(is_valid);
  NW_ASSERT(ret_string);
  *ret_string = NULL;

  if (!NW_Url_GetRfc2396Parts(url_buff, &urlPieces))
  {
    return KBrsrMalformedUrl;
  }
  *is_valid = NW_TRUE;

  pStart = urlPieces.portStart;
  if (pStart != NULL)
  {
    pEnd = urlPieces.portEnd;

    /*malloc enough for the string and the null terminator.*/
    NW_ASSERT(pEnd >= pStart);
    *ret_string = NW_Str_Substr(pStart, 0, ((NW_Uint32)(pEnd - pStart)));
    if (*ret_string == NULL)
    {
      *is_valid = NW_FALSE;
      return KBrsrOutOfMemory;
    }
  }
  return KBrsrSuccess;
}

/*
* RETURN: KBrsrSuccess
*          KBrsrFailure - string has NO parameters
*       KBrsrOutOfMemory
*/
TBrowserStatusCode NW_Url_GetParameters(const NW_Ucs2 *url_buff, NW_Ucs2 **ret_string)
{
  const NW_Ucs2 *pCh = url_buff;
  const NW_Ucs2 *pStart = NULL;
  const NW_Ucs2 *pFirstParam = NULL;

  NW_Rfc2396_Parts_t urlPieces;

  NW_ASSERT(url_buff);
  NW_ASSERT(ret_string);
  *ret_string = NULL;

  if (!NW_Url_GetRfc2396Parts(url_buff, &urlPieces))
  {
    return KBrsrMalformedUrl;
  }

  pStart = urlPieces.pathStart;
  if (pStart == NULL)
  {
    return KBrsrSuccess;
  }
  pCh = urlPieces.pathEnd - 1;
  /* return all params from last segment in path */
  while ((pCh >= pStart) && (*pCh != WAE_URL_PATH_CHAR))
  {
    if (*pCh == WAE_URL_PARAM_CHAR)
    {
      pFirstParam = pCh;
    }
    pCh--;
  }

  if (pFirstParam != NULL)
  {
    pFirstParam++;
    pCh = urlPieces.pathEnd;

    NW_ASSERT(pCh >= pFirstParam);
    *ret_string = NW_Str_Substr(pFirstParam, 0, ((NW_Uint32)(pCh - pFirstParam)));
    if (*ret_string == NULL)
      return KBrsrOutOfMemory;

  }
  return KBrsrSuccess;
}

NW_Bool NW_Url_IsValid( const NW_Ucs2 *pUrl )
{
  NW_Rfc2396_Parts_t urlPieces;
  return NW_Url_GetRfc2396Parts(pUrl, &urlPieces);
}

/*****************************************************************
**  Name:  NW_Url_GetResovledPath
**  Description:  Parses a given path and extracts from it the filename
**  Parameters:   *pBasePath - pointer to the base path
**                **path - pointer to the path returned
**  Return Value: KBrsrOutOfMemory if could not allocate memory
**                KBrsrSuccess otherwise
**  Note:         pBasePath loooks like:  /browser/welcome.wml
******************************************************************/

TBrowserStatusCode NW_Url_GetResolvedPath(const NW_Ucs2 *pBasePath,
                                   const NW_Ucs2 *pRelPath,
                                   NW_Ucs2 **path)
{
  NW_Bool shortened;
  NW_Ucs2 *pStart = NULL;
  NW_Ucs2 *pDest = NULL;
  NW_Ucs2 *pSegment = NULL;
  NW_Ucs2 *pBackslashLocationStr = NULL;
  NW_Ucs2 *pTempBasePath = NULL;
  NW_Uint32    length = 0;
  const NW_Ucs2 escapedBackslashStr[] = {'%', '5', 'C'};
  const NW_Ucs2 slashStr[] = {'/'};

  /* check to see if the base path has any escaped '\' characters ('%5C')
     if any are found, replace with slash '/' character.  This is required
     for compatability with Internet Explorer */

  if (pBasePath != NULL)
    {
    pTempBasePath = NW_Str_New(NW_Str_Strlen(pBasePath));

    if (pTempBasePath == NULL)
      {
      return KBrsrOutOfMemory;
      }

    pTempBasePath = NW_Str_Strcpy(pTempBasePath, pBasePath);

    pBackslashLocationStr = NW_Str_Strstr(pTempBasePath, escapedBackslashStr);
    while(pBackslashLocationStr)
      {
      NW_Ucs2 *pTempStr;
      NW_Ucs2 *pTempStrStart;
      NW_Ucs2 *pTempStrEnd;
      NW_Uint32 basePathLength = NW_Str_Strlen(pTempBasePath);
      NW_Uint32 backslashLocation = pBackslashLocationStr - pTempBasePath;

      pTempStr = NW_Str_New(basePathLength);
      if(pTempStr == NULL)
        {
        NW_Str_Delete(pTempBasePath);
        return KBrsrOutOfMemory;
        }

      pTempStrStart = NW_Str_Substr(pTempBasePath, 0, backslashLocation);
      if(pTempStrStart == NULL)
        {
        NW_Str_Delete(pTempBasePath);
        NW_Str_Delete(pTempStr);
        return KBrsrOutOfMemory;
        }

      pTempStrEnd = NW_Str_Substr(pTempBasePath, backslashLocation + 3, basePathLength - backslashLocation - 3);
      if(pTempStrEnd == NULL)
        {
        NW_Str_Delete(pTempBasePath);
        NW_Str_Delete(pTempStr);
        NW_Str_Delete(pTempStrStart);
        return KBrsrOutOfMemory;
        }

      NW_Str_Strcat(pTempStr, pTempStrStart);
      NW_Str_Strcat(pTempStr, slashStr);
      NW_Str_Strcat(pTempStr, pTempStrEnd);

      pTempBasePath = NW_Str_Strcpy(pTempBasePath, pTempStr);

      NW_Str_Delete(pTempStrEnd);
      NW_Str_Delete(pTempStrStart);
      NW_Str_Delete(pTempStr);

      pBackslashLocationStr = NW_Str_Strstr(pTempBasePath, escapedBackslashStr);
      }

  }

  /* return NULL if error or not found */
  if (pBasePath != NULL) {
    length += NW_Str_Strlen(pTempBasePath);
  }
  if (pRelPath != NULL) {
    length += NW_Str_Strlen(pRelPath);
  }

  /* allocate max mem we'll ever need */
  *path = NW_Str_New(length + NW_Asc_strlen(WAE_URL_PATH_SEP));
  if (*path == NULL) {
    NW_Str_Delete(pTempBasePath);
    return KBrsrOutOfMemory;
  }

  /*set start of basepath*/
  pStart = *path;
  *pStart = 0;
  if (pBasePath != NULL) {
    NW_Str_Strcpy(pStart, pTempBasePath);
  }

  /* RFC2396 section 5 step 6 */

  /*
  ** "a) All but the last segment of the base URI's path component is copied
  ** to the buffer.  In other words, any characters after the last
  ** (right-most) slash charactger, if any, are excluded."  __NOTA BENE__ if
  ** the base path is empty we add a single / per the WML Script Standard
  ** Library Spec and proposed changes to RFC2396.
  */
  pDest = NW_Str_Strrchr(pStart, WAE_URL_PATH_CHAR);
  if (pDest != NULL && *pDest == WAE_URL_PATH_CHAR)
  {
    *(pDest + 1) = 0;
  }
  else if (pRelPath != NULL) /* slash doesn't exist, and we've got a rel path */
  {
    NW_Str_StrcpyConst(pStart, WAE_URL_PATH_SEP);
  }

  if (pRelPath != NULL) {
    /* b) the reference's path component is appended to the buffer */
    (void)NW_Str_Strcat(pStart, pRelPath);
  }

  /* deal with http://www.sees.com, <img src="../img/some.gif">
     pStart = /../img/some.gif; want to resolve it to be /img/some.gif
  */
  pDest = pStart;
  pDest = NW_Str_Strchr(pDest, WAE_URL_DOT_CHAR);
  if ( pDest != NULL &&
       pDest - 1 == pStart &&
       *(pDest - 1) == WAE_URL_PATH_CHAR &&
       *(pDest + 1) == WAE_URL_DOT_CHAR &&
	     *(pDest + 2) == WAE_URL_PATH_CHAR )
  {
    /* Move pDest + 2 to pStart.  Do not use NW_Asc_strcpy because
       the source and destination overlap.  Instead, use NW_Mem_memmove.
	*/
    NW_Mem_memmove(pStart, pDest + 2, ((NW_Str_Strlen(pDest + 2)) + 1) * sizeof(NW_Ucs2));
  }

  /*
  ** c) All occurrences of "./", where "." is a complete path segment, are
  **    removed from the buffer string.
  ** d) If the buffer string ends with "." as a complete path segment, that
  **    "." is removed.
  */
  shortened = NW_TRUE;
  pDest = pStart;
  while (shortened)
  {
    shortened = NW_FALSE;
    pDest = NW_Str_Strchr(pDest, WAE_URL_DOT_CHAR);
    if (pDest != NULL)
    {
      shortened = NW_TRUE;
      if (pDest == pStart || *(pDest - 1) == WAE_URL_PATH_CHAR)
      {
         /* got to end of string, so don't look for single dot again */
        if (*(pDest + 1) == 0)
        {
          *pDest = 0;
          shortened = NW_FALSE;
        }
        else if (*(pDest + 1) == WAE_URL_PATH_CHAR)
        {
          NW_Str_Strcpy(pDest, pDest + 2);
        }
      }
      pDest++;
    }
  }

  /*
  ** e) all occurrences of "<segment>/../", where <segment> is a complete
  **    path segment not equal to "..", are removed from the buffer string.
  **    Removal of these path segments is performed iteratively, removing
  **    the leftmost matching pattern on each iteration, until no matching
  **    pattern remains.
  ** f) If the buffer string ends with "<segment>/..", where <segment> is a
  **    complete path segment not equal to "..", that "<segment>/.." is removed.
  ** g) If the resulting buffer string still begins with one or more complete
  **    path segments of "..", then the reference is considered to be in error.
  **    Implementations may handle this error by reaining these components in
  **    the resolved path (i.e., treating them as part of the final URI), by
  **    removing them from the resolved parth (i.e., discarding relative levels
  **    above the root), or by avoiding traversal of the reference.
  */
  shortened = NW_TRUE;
  pDest = pStart;
  while (shortened)
  {
    shortened = NW_FALSE;
    pDest = NW_Str_Strchr(pDest, WAE_URL_DOT_CHAR);
    if (pDest != NULL)
    {
      shortened = NW_TRUE;
      if ( (pDest == pStart || *(pDest - 1) == WAE_URL_PATH_CHAR) &&
           *(pDest + 1) == WAE_URL_DOT_CHAR )
      {
        if ( (pDest == pStart || (pDest - 1) == pStart) &&
             (*(pDest + 2) == 0 || *(pDest + 2) == WAE_URL_PATH_CHAR) )
          {
          // there's extra ../ in the relative, we should remove them
          // case1: base url is /format/video/index.asp, relative url is ../../../formats/video/benefits.asp
          // case2: base url is /, relative url is ../formats/index.asp
          if ( *(pDest + 2) == WAE_URL_PATH_CHAR )
            {
            // Move pDest + 2 to pSegment or pStart.  Do not use NW_Asc_strcpy because
            // the source and destination overlap.  Instead, use NW_Mem_memmove.
             if (pSegment == pStart)
               {
               NW_Mem_memmove(pSegment, pDest + 2, ((NW_Str_Strlen(pDest + 2)) + 1) * sizeof(NW_Ucs2));
               pDest = pSegment;
               continue;
               }
             else if (*pBasePath == WAE_URL_PATH_CHAR)
               {
               NW_Mem_memmove(pStart, pDest + 2, ((NW_Str_Strlen(pDest + 2)) + 1) * sizeof(NW_Ucs2));
               pDest = pStart;
               continue;
               }
            }

		      NW_Str_Delete(pStart);
          *path = NULL;
          NW_Str_Delete(pTempBasePath);
          return KBrsrFailure;
          }

        /* see if we're at end of string */
        if (*(pDest + 2) == 0)
        {
          pSegment = pDest - 2;
          while (pSegment >= pStart && *pSegment != WAE_URL_PATH_CHAR)
          {
            pSegment--;
          }
          if (*pSegment == WAE_URL_PATH_CHAR)
          {
            *(pSegment + 1) = 0;
          }
          else
          {
            *pSegment = 0;
          }
          shortened = NW_FALSE;
        }
        else if (*(pDest + 2) == WAE_URL_PATH_CHAR)
        {
          pSegment = pDest - 2;
          while (pSegment >= pStart && *pSegment != WAE_URL_PATH_CHAR)
          {
            pSegment--;
          }
          /*
           * Move pDest + 2 to pSegment.  Do not use NW_Asc_strcpy because
           * the source and destination overlap.  Instead, use NW_Mem_memmove.
           */
          NW_Mem_memmove(pSegment, pDest + 2, ((NW_Str_Strlen(pDest + 2)) + 1) * sizeof(NW_Ucs2));
          pDest = pSegment;
        }
        else {
          pDest++;
        }
      } else {
        pDest++;
      }
    }
  }

  NW_Str_Delete(pTempBasePath);
  return KBrsrSuccess;
}

/*
 * RETURN KBrsrSuccess
 *        KBrsrFailure
 *        KBrsrOutOfMemory
 *        KBrsrMalformedUrl
 */
TBrowserStatusCode NW_Url_RelToAbs( const NW_Ucs2 *base, const NW_Ucs2 *relative,
                             NW_Ucs2 **ret_string)
{

  /*
   * Step1 -- Parse URL into its components
   */
  typedef struct
  {
    NW_Ucs2 *schemeStr;
    NW_Ucs2 *netLoc;
    NW_Ucs2 *port;
    NW_Ucs2 *path;
    NW_Ucs2 *query;
    NW_Ucs2 *fragId;
    //_Bool portValid;
  } parts_t;

  TBrowserStatusCode status = KBrsrSuccess;
  NW_Ucs2 *resolvedPath = NULL;

  NW_Rfc2396_Parts_t baseUrlPieces;
  NW_Rfc2396_Parts_t relUrlPieces;

  parts_t bp = {NULL, NULL, NULL, NULL, NULL, NULL};
  parts_t rp = {NULL, NULL, NULL, NULL, NULL, NULL};

  NW_ASSERT(base);
  NW_ASSERT(ret_string);
  *ret_string = NULL;


  if (!base || !NW_Url_GetRfc2396Parts(base, &baseUrlPieces))
    return KBrsrMalformedUrl;

  if (!relative || !NW_Url_GetRfc2396Parts(relative, &relUrlPieces))
    status = KBrsrMalformedUrl;

  /* get all the the url base parts */
  if (NW_Url_GetSchemeStr(base, &(bp.schemeStr)) == KBrsrOutOfMemory) {
    status = KBrsrOutOfMemory; NW_THROW_ERROR();
  }

  if (NW_Url_GetNetwork(base, &bp.netLoc) == KBrsrOutOfMemory) {
    status = KBrsrOutOfMemory;  NW_THROW_ERROR();
  }

  if (NW_Url_GetPath(base, &bp.path) == KBrsrOutOfMemory) {
    status = KBrsrOutOfMemory;  NW_THROW_ERROR();
  }

  if (NW_Url_GetQuery(base, &bp.query) == KBrsrOutOfMemory) {
    status = KBrsrOutOfMemory;  NW_THROW_ERROR();
  }

  if (NW_Url_GetFragId(base, &bp.fragId, NW_FALSE) == KBrsrOutOfMemory) {
    status = KBrsrOutOfMemory;  NW_THROW_ERROR();
  }

  /*get all url the parts from the relative*/
  if (NW_Url_GetSchemeStr(relative, &rp.schemeStr) == KBrsrOutOfMemory) {
     status = KBrsrOutOfMemory;  NW_THROW_ERROR();
  }

  if (NW_Url_GetNetwork(relative, &rp.netLoc) == KBrsrOutOfMemory) {
    status = KBrsrOutOfMemory;  NW_THROW_ERROR();
  }

  if (NW_Url_GetPath(relative, &rp.path) == KBrsrOutOfMemory) {
    status = KBrsrOutOfMemory;  NW_THROW_ERROR();
  }

  if (NW_Url_GetQuery(relative, &rp.query) == KBrsrOutOfMemory) {
    status = KBrsrOutOfMemory;  NW_THROW_ERROR();
  }

  if (NW_Url_GetFragId(relative, &rp.fragId, NW_FALSE) == KBrsrOutOfMemory) {
    status = KBrsrOutOfMemory;  NW_THROW_ERROR();
  }

  /*
   * Step 2
   */

  /* If the path component is empty and the scheme, authority, and query components are
   * undefined, then it is a reference to the current document and we are done.
   *
   * Otherwise, the reference URI's query and fragment components are defined as found
   * (or not found) within the URI reference and not inherited from the base URI.
   */

  if ( (rp.schemeStr == NULL) && (rp.netLoc == NULL) && (rp.path == NULL) &&
       (rp.query == NULL))
  {
    if  (rp.fragId == NULL) {
      status = build_url(bp.schemeStr, bp.netLoc, bp.path, bp.query, bp.fragId, ret_string);
    } else {
      /*
      ** example in rfc2396 appendix c.1 shows fragment only in rel part
      ** handled differently from any other relative uri - the last segment
      ** in the base is NOT stripped, and any existing fragment is simply
      ** replaced by the new fragment
      */
      status = build_url(bp.schemeStr, bp.netLoc, bp.path, bp.query, rp.fragId, ret_string);
    }
    NW_THROW_ERROR();
  }

   /*Step 3*/

  /* If the scheme component is defined, indicating that the reference starts with
   * a scheme name, then the reference is interpreted as an absolute URI
   * and we are done.
   *
   * Otherwise, the reference URI's scheme is inherited from the base URI's scheme
   * component.
   */

  if (rp.schemeStr != NULL) {
    *ret_string = NW_Str_Newcpy(relative);
    if (*ret_string == NULL) {
      status = KBrsrOutOfMemory;
    }
    NW_THROW_ERROR();
  }

  /*Step 4*/

  /* If the authority component is defined, then the reference is a network-path
   *
   * Otherwise, the reference URI's authority is inherited from the base URI's
   * authority component, which will also be undefined if the URI scheme does not
   * use an authority component.
   * Note:  referee is a network path
   */
  if (rp.netLoc != NULL) {
    status = build_url(bp.schemeStr, rp.netLoc, rp.path, rp.query, rp.fragId, ret_string);
    NW_THROW_ERROR();
  }

  /*Step5*/
  /* If the path component begins with a slash character, then the reference is an absolute
   * path
   * Note: the reference is an absolute path
   */

  if ((rp.path != NULL) && (rp.path[0] == WAE_URL_PATH_CHAR)) {
    status = build_url(bp.schemeStr, bp.netLoc, rp.path, rp.query, rp.fragId, ret_string);
    NW_THROW_ERROR();
  }

  /*
   * Otherwise resolving a relative path reference
   */
  status = NW_Url_GetResolvedPath(bp.path, rp.path, &resolvedPath);
  if (status != KBrsrSuccess) {
    NW_THROW_ERROR();
  }

  status = build_url(bp.schemeStr, bp.netLoc, resolvedPath, rp.query, rp.fragId,  ret_string);

NW_CATCH_ERROR
  NW_Str_Delete(bp.schemeStr);
  NW_Str_Delete(bp.netLoc);
  NW_Str_Delete(bp.port);
  NW_Str_Delete(bp.path);
  NW_Str_Delete(bp.query);
  NW_Str_Delete(bp.fragId);

  NW_Str_Delete(rp.schemeStr);
  NW_Str_Delete(rp.netLoc);
  NW_Str_Delete(rp.port);
  NW_Str_Delete(rp.path);
  NW_Str_Delete(rp.query);
  NW_Str_Delete(rp.fragId);

  NW_Str_Delete (resolvedPath);
  return status;

}

//??? RFD: Is this correct, regarding must be UTF8?
// Determines whether or not a UCS2 char needs to be escaped for encoding
// within a URL.
//
// RFC2396, "Uniform Resource Identifiers (URI): Generic Syntax", specifies
// the US-ASCII chars (0x00 - 0x7F) that can occur within a URI, specifies
// reserved chars, delimiters and how to escape-encode US-ASCII chars that
// are not allowed. It does not, however, specify how to handle NON US-ASCII
// chars (> 0x7F). For that, W3C provides guidance and sample code but, as
// of this writing, no formal specification. See the following:
//   "http://www.w3.org/International/O-URL-code.html"
//
// An excerpt from that W3C site:
//  "For worldwide interoperability, URIs have to be encoded uniformly. To map
//  the wide range of characters used worldwide into the 60 or so allowed
//  characters in a URI, a two-step process is used:
//      o. Convert the character string into a sequence of bytes using the
//          UTF-8 encoding.
//      o. Convert each byte that is not an ASCII letter or digit to %HH, where
//          HH is the hexadecimal value of the byte.
// The site also provides example code and other links.
//
// Because there isn't a definitive spec on how to handle chars > 0x7F, only
// the suggested W3C guidelines, there are different implementations out on
// the street that are incompatible. We cannot be all things to all people.
// There will be sites that cannot be supported without breaking some other
// site. Much care must be used when modifying URL encoding.
//
// STREET-SUPPORT: In real-world situations, it appears that more often than
// not, UTF-8 encoding is not used. Instead, chars >0x7F are escape-encoded
// directly without first using UTF-8. But it is highly probably that there are
// web severs that will not work this way and expect UTF-8.
//
// One heuristic mentioned on some web-sites is that if the original page
// specified a character set, then it is ok/recommended to use that and not
// convert to UTF-8. This means that the browser charset handling has to
// differentiate between when the charset is explicitely specified and when a
// default charset is used.
//??? RFD: This is not the case at the moment.
//
//??? RFD: For 2-byte chars, use little-endian or big-endian? There's no ambiguity if first converted to UTF-8 but there is when escape-encoding 2-byte chars.
//
// Notes:
// - UTF8 is defined in RFC2279, "UTF-8, a transformation format of ISO 10646".
// - RFC2396 has been updated by RFC2732, "Format for Literal IPv6 Addresses in
//  URL's".
//
// @param ch UCS2 char being checked.
// @param escapeReservedDelimiters Flag indicating whether or not reserved
//  and delimiter chars need to be escaped.
// @return NW_TRUE if char needs to be escaped; otherwise, NW_FALSE.
static NW_Bool shouldEscape( NW_Ucs2 ch, NW_Bool escapeReservedDelimiters )
{
    if ( ch > 0xFF ) // Hack: these chars are handled by NW_Url_ConvertUnicode...
        {
        return NW_FALSE;
        }

    if ( ch >= 0x80 )
        {
//??? RFD: When this was changed to TRUE, chars >7f and <ff were encoded and my bug seemed to be fixed.
        return NW_FALSE;
        }

    if ( (ch <= 0x1F) || (ch == 0x20) || (ch == 0x7F) )
        {
        return NW_TRUE; // US-ASCII control chars or space char
        }

    if ( escapeReservedDelimiters )
        {
        if ( (ch == ';') ||
             (ch == '/') ||
             (ch == '?') ||
             (ch == ':') ||
             (ch == '@') ||
             (ch == '&') ||
             (ch == '=') ||
             (ch == '+') ||
             (ch == '$') ||
             (ch == ',') ||
             // The following two chars are defined as "unwise" in RFC2396 but
             // were re-defined as delimiters for IPv6 in RFC2732.
             (ch == '[') ||
             (ch == ']') )
            {
            return NW_TRUE; // RFC2396 reserved chars
            }

        if ( (ch == '<') ||
             (ch == '>') ||
             (ch == '#') ||
             (ch == '%') ||
             (ch == '"') )
            {
            return NW_TRUE; // RFC2396 delimiter chars
            }
        }

    if ( (ch == '{')  ||
         (ch == '}')  ||
         (ch == '|')  ||
         (ch == '\\') ||
         (ch == '^')  ||
         (ch == '`') )
        {
        return NW_TRUE; // RFC2396 "unwise" chars
        }

    return NW_FALSE;
}

//??? RFD: This will only produce the correct results if the string has already been converted to UTF8.
NW_Uint32   NW_Url_EscapeStringLen( const NW_Ucs2 *src )
{
  NW_Uint32  len = 0,
  pos = 0,
  numToEsc = 0;

  if( src )
    len = NW_Str_Strlen(src);
  else
    len = 0;

  for( pos = 0; pos < len; pos++ )
  {
    if( shouldEscape( src[pos], NW_TRUE) )
    {
      numToEsc++;
    }
  }

  return (2 * numToEsc) + len;
}

/* Gets the byte-length needed to convert the UCS2 string to a URL encoded
   string. Only chars > 0x7F are URL encoded. If the original-encoding is
   supported, then each char encoded is encoded using 3 2-byte chars for a
   total of 6 bytes; otherwse, UTF8 encoding is used and then escaped, which
   for each char being encoded requires either 6 or 9 bytes, depending on the range:
     - range 0x0080 <= char <= 0x07FF: 2 3-byte chars for total of 6 bytes
     - range 0x0800 <= char <= 0xFFFF: 3 3 byte chars for total of 9 bytes */
static NW_Uint32 NW_Url_ConvertUnicodeLen(const NW_Ucs2 *str, NW_Uint32 encoding)
{
  NW_Uint32 len = 0;
//??? RFD: Why are we doing this with supported encoding? W3C guidelines say to use UTF8, then escape it.
  NW_Bool supportedEncoding = NW_Url_IsSupportedEncoding( encoding );

  while ( *str != 0 )
  {
    if ( *str <= 0x07f && *str != 0x20 )  // if char < 0x7f and not spc (space) char - no encode, otherwise check if to be 3-byte encoded
    {
      len += 1;     // no escaping
    }
    else if ( supportedEncoding || (*str <= 0x07ff) )
    {
      len += 6;
    }
    else
    {
      len += 9;
    }
    str++;
  }
  return len;
}

// Determines whether or not URL needs to be encoded for specified schema.
static NW_Bool isUrlEncodedForSchema( NW_Url_Schema_t schema )
    {
    NW_Bool ret = NW_FALSE;
    switch (schema)
        {
        case NW_SCHEMA_WTAI:
        case NW_SCHEMA_UNKNOWN:
            ret = NW_FALSE;
            break;

        case NW_SCHEMA_HTTPS:
        case NW_SCHEMA_FTP:
        case NW_SCHEMA_GOPHER:
        case NW_SCHEMA_MAILTO:
        case NW_SCHEMA_NEWS:
        case NW_SCHEMA_NNTP:
        case NW_SCHEMA_TELNET:
        case NW_SCHEMA_WAIS:
        case NW_SCHEMA_FILE:
        case NW_SCHEMA_HTTP:
        case NW_SCHEMA_PROSPERO:
        case NW_SCHEMA_TEL:
        default:
            ret = NW_TRUE;
        }
    return ret;
    }

// Converts an entire byte sequence to %HH format, storing the result as an NW_Ucs2*
// This is needed by charsets such as ShiftJis, which is multibyte and may have
// ascii values as one of the bytes.
TBrowserStatusCode NW_Url_HHEscape( const char* aSource, NW_Ucs2** aDest )
{
  NW_Uint32 escapedLen = 3 * NW_Asc_strlen(aSource);
  const char* currentByte = NULL;
  NW_Ucs2* dest = NULL;


  NW_ASSERT( aDest != NULL );
  NW_ASSERT( aSource != NULL);

  dest = NW_Str_New(escapedLen);
  *aDest = dest;
  if (dest == NULL)
      {
      return KBrsrOutOfMemory;
      }

  for (currentByte = aSource;
       *currentByte != NULL;
       currentByte++)
      {
      *dest = WAE_URL_ESCAPE_CHAR;
      dest++;
      *dest = HEXDIGITS[(*currentByte >> 4) & 15];
      dest++;
      *dest = HEXDIGITS[*currentByte & 15];
      dest++;
      }
  *dest = 0;
  return KBrsrSuccess;
}

//??? RFD: It is wrong to do chars <= 7F separately, especially because
// foreign charset encoding does not guarantee that encoded chars are > 0x7F;
// therefore, trying to URL encode after convertUnicode will cause double convertion.

/************************************************************************

  Function: NW_Url_ConvertUnicode

  Description:  Given a UCS-2 string in source,
                produces a string in which characters > 0x7F (8 and 16-bits)
                have been UTF-8 encoded (if not using foreign charset encoding)
                and url-escaped.
                This routine does not affect characters <= 0x7F in source.

  Return Value: KBrsrSuccess
                KBrsrOutOfMemory

**************************************************************************/
//??? RFD: What is the output of this method? Is it UCS2? Can't be UCS2!
// Or is it a byte stream or is it a byte stream that has every byte occupy two-bytes?
TBrowserStatusCode NW_Url_ConvertUnicode( const NW_Ucs2* source, NW_Ucs2** dest)
{
  const NW_Ucs2* ucs2Ptr = source;
  const NW_Byte* bytePtr = (NW_Byte *)source;
  NW_Byte  tmp[3];
  NW_Int32 cnt;
  NW_Ucs2* convertedPtr;
  NW_Int32 i;
  NW_Int32 convertedLen;
  NW_Ucs2 charToEncode;
  NW_Bool isUrlEncoded;
  NW_Uint32 encoding;
  NW_Url_Schema_t schema;
  TBrowserStatusCode status;

  NW_ASSERT( dest != NULL );
  *dest = NULL;

  if ( source == NULL)
    return KBrsrSuccess;

//??? RFD: Should determine here:
// 1. what encoding to use
// 2. whether or not to UTF-8 encode
// 3. whether or not to escape-encode
// 4. determine endianess if encoding is 2-bytes and not being UTF-8 encoded
//    (The endianness is important for 2-byte chars whether or not they are
//    escape-encoded.)
// And then pass this information to all the relevant sub-methods rather than
// all the special-case crap that is dispersed down through the layers here.
// It may make sense to even make these decisions at a higher level and pass
// that information into this method, if it gets called at all.
//
// The items mentioned differ based on the schema. For WTAI, for example,
// the encoding must be UCS2 regardless of what the original encoding was, it
// must not be UTF-8 encoded, it must not be escape-encoded, and it must be
// in little-endian.
//
// For HTTP, it is much more complex. See header comments added to shouldEscape.
//

//??? RFD: We don't seem to be converting to the original encoding when we are not using UTF-8.
// And we are not using UTF-8 most of the time now. This is wrong. If we are not using UTF-8
// then chars have to be transcoded into the original encoding. The original encoding shouls
// only be used if it was explicitely specified, not if some assumed default was chosen.

//TODO: Should have a getSchemaEncoding method. Some schemas use original
// encoding and others (internal, such as WTAI) use UCS2.
  encoding = NW_Settings_GetOriginalEncoding();

  convertedLen = 2 * ( NW_Url_ConvertUnicodeLen( source, encoding ) + 1 );
  *dest = (NW_Ucs2*) NW_Mem_Malloc( convertedLen );
  if (*dest == NULL) {
    return KBrsrOutOfMemory;
  }
  NW_Mem_memset(*dest, 0, convertedLen);

  convertedPtr = *dest;
//??? RFD: Checking SupportedEncoding is not quite the correct way to determine whether to UTF-8 encode.
  if (NW_Url_IsSupportedEncoding(encoding))
  {
    // Check to see whether or not the URL needs to use encoding. Some schemas
    // need URL encoding and some must not have it. WTAI, for example, must not
    // use encoding; chars are sent as straight UCS2.
    status = NW_Url_GetSchemeNoValidation(source, &schema);
    isUrlEncoded = (NW_Bool)(( status == KBrsrSuccess )
        ? isUrlEncodedForSchema(schema) : NW_TRUE);

    while ( *bytePtr != 0 || *(bytePtr +1)!=0)
    {
      NW_Mem_memset(tmp, 0, sizeof(tmp));
      cnt = writeForeignCharBuff(bytePtr, &tmp[0]);
      NW_ASSERT(cnt <= sizeof(tmp));
      if (tmp[0] >= 0x80 || cnt > 1) {
        for ( i = 0; i < cnt; i++ )
        {
          if (isUrlEncoded)
          {
            /* temporary variable needed because passing ((NW_Ucs2*) &tmp[i]) to
            escape_one_char() causes a word-boundary error when i is odd. */
            charToEncode = tmp[i];
            escape_one_char((const NW_Ucs2*) &charToEncode, &convertedPtr, NW_TRUE);
          }
          else
          {
            *convertedPtr++ = *((NW_Ucs2*) tmp);
            break;
          }
        }
      }
      else {
        //use "+" replace the space in the post string
//??? RFD: space chars are suppose to be escaped and why doesn't this apply below, the case where not supported encoding
        if (tmp[0] == 0x20) 
        {
          *convertedPtr++ = '%';  *convertedPtr++='2';   *convertedPtr++='0';  
        }
        else
        {
          *convertedPtr++ = (NW_Ucs2) tmp[0];
        }
      }
//??? RFD: Get rid of this bytePtr and use a UCS2 ptr.
      bytePtr += 2;
    }
  }
  else
  {
//??? RFD: Combine this with the above loop.
    while ( *ucs2Ptr != 0 )
    {
      NW_Mem_memset(tmp, 0, sizeof(tmp));
      cnt = NW_Reader_WriteUtf8CharBuff( *ucs2Ptr, &tmp[0]);
      NW_ASSERT(cnt <= sizeof(tmp));
      for ( i = 0; i < cnt; i++ )
      {
        if (tmp[i] >= 0x80 || cnt > 1) {
        /* temporary storage needed because passing ((NW_Ucs2*) &tmp[i]) to
          escape_one_char() causes a word-boundary error when i is odd.*/
          charToEncode = tmp[i];
          escape_one_char((const NW_Ucs2*) &charToEncode, &convertedPtr, NW_TRUE);
        }
        else {
          *convertedPtr = (NW_Ucs2) tmp[i];
          convertedPtr++;
        }
      }
      ucs2Ptr++;
    }
  }
  NW_ASSERT(convertedPtr - *dest <= convertedLen);

  return KBrsrSuccess;
}

//??? RFD: What the hell is "x-www-form-urlencoded format"?
/*  This function will encode a string into x-www-form-urlencoded format.*/

//??? RFD: it is backwards to have ConvertUnicode called after the string is escaped. The sequence
// should be 1. determine charset to use, 2. determine whether or not to UTF-8 encode, determine
// endianess if not using UTF-8, 3. UTF-8 encoded if using UTF-8, otherwise, convert to charset,
// 4. swap bytes if 2-byte charset and endianess is different from internal storage (little endian),
// 5. escape-encode if escape-encoding (WTAI schema doesn't escape-encode)
//
//??? RFD: the comment below on 0xFF doesn't match what is done in the code, which uses 0x7F. Which is it suppose to be?
/*  09-09-02 CEI: Modifed to ignore characters > 0xFF, which get handled by
 NW_Url_ConvertUnicode() just before the URL is sent.  This split is caused
 by issues involving WML variable resolution, which gives WML content
 developers control over URL escaping.
*/
//??? RFD: May need to cleanup all methods that call this.
NW_Bool NW_Url_EscapeString( const NW_Ucs2 *src, NW_Ucs2 *dest )
{
  NW_Int32   len = 0,
  posInSrc = 0,
  posInDest = 0;
  NW_Bool allASCII=NW_TRUE;

  NW_ASSERT(dest);

  if( src )
    len = NW_Str_Strlen(src);
  else
    len = 0;

  /* Walk through the source and escape any characters into destination. */
  posInDest = 0;
  for( posInSrc = 0; posInSrc < len; posInSrc++ )
  {
    if (src[posInSrc] > 127) {
      allASCII = NW_FALSE;
    }

    if (escape_char( &src[posInSrc], &dest[posInDest], NW_TRUE ) && (dest[posInDest] != 0x2B) ) {
      posInDest += 3;
    } else {
      posInDest++;
    }
  }
  dest[posInDest] = 0;
  return allASCII;
}

//??? RFD: This will only produce the correct results if the string has already been converted to UTF8.
static NW_Bool escape_char( const NW_Ucs2 *src, NW_Ucs2 *dest, NW_Bool escapeReservedDelimiters )
{

  NW_ASSERT(src);
  NW_ASSERT(dest);

  if( shouldEscape( *src, escapeReservedDelimiters ) )
  {
    escape_one_char(src, &dest, escapeReservedDelimiters);

    return NW_TRUE;
  }
  else
  {
    *dest = *src;

    return NW_FALSE;
  }
}

static void escape_one_char(const NW_Ucs2 *src, NW_Ucs2 **dest, NW_Bool escapeReservedDelimiters)
{
    NW_ASSERT(dest != NULL);
    NW_ASSERT(*dest != NULL);
    if (*src == 0x20 && escapeReservedDelimiters) {
      **dest = 0x2B; // change space to plus
       (*dest)++;
       return;
    }
    **dest = WAE_URL_ESCAPE_CHAR;
    (*dest)++;
    **dest = HEXDIGITS[(*src >> 4) & 15];
    (*dest)++;
    **dest = HEXDIGITS[*src & 15];
    (*dest)++;
}


NW_Uint32  NW_Url_UnEscapeStringLen( const NW_Ucs2 *src )
{
  NW_Uint32  len = 0,
  numToUnesc = 0,
  pos = 0;

  if( src ) {
    len = NW_Str_Strlen(src);
    /*
    ** better make sure the val we return here is a big enough buffer to
    ** hold result of NW_Url_UnEscapeString()
    */
    if ( !is_legal_escaped_string( src ) ) {
      return len;
    }
  }
  else
    len = 0;

  for( pos = 0; pos < len; pos++ )
  {
    if ( is_legal_escaped_chars( &src[pos] ) )
    {
      /* Only subtract from length if legal. */
      numToUnesc++;
    }
  }

  return len - (2 * numToUnesc);
}


/*  This function will un-encode a string from x-www-form-urlencoded format.*/
NW_Bool NW_Url_UnEscapeString( const NW_Ucs2 *src, NW_Ucs2 *dest )
{
  NW_Int32     len = 0,
  posInSrc = 0,
  posInDest = 0;
  NW_Bool retval = NW_TRUE;
  /*NW_Ucs2  *pDest = NULL;*/

  NW_ASSERT(src);
  NW_ASSERT(dest);

  if ( !is_legal_escaped_string( src ) )
  {
    /* String contains an illegal escape sequence.
    Put unchanged result in dest and return NW_FALSE. */
    NW_Str_Strcpy( dest, src );
    return NW_FALSE;
  }

  len = NW_Str_Strlen(src);

  /* Walk through the source and un-escape any characters into destination. */
  posInDest = 0;
  for( posInSrc = 0; posInSrc < len; posInSrc++ )
  {
    if( unescape_char( &src[posInSrc], &dest[posInDest]) )
    {
      posInSrc += 2;
    }

    if (dest[posInDest] > 0x7F)
      retval = NW_FALSE;
    posInDest++;
  }

  dest[posInDest] = 0;

  return retval;
}

static NW_Bool unescape_char( const NW_Ucs2 *src, NW_Ucs2 *dest )
{
  NW_Ucs2 buff[3];
  NW_Ucs2 *end;

  NW_ASSERT(src);
  NW_ASSERT(dest);

  if( *src == WAE_URL_ESCAPE_CHAR )
  {
    /*This may be a valid escape sequence.*/
    if( NW_Str_Isxdigit( *(src + 1) ) && NW_Str_Isxdigit( *(src + 2) ) )
    {
      /* Ok it is a valid escape sequence, convert it into a char.*/
      buff[0] = *(src + 1);
      buff[1] = *(src + 2);
      buff[2] = 0;
      *dest = (NW_Ucs2) NW_Str_Strtoul(buff, &end, 16);
      return NW_TRUE;
    }
    else
    {
      *dest = *src;
      return NW_FALSE;
    }
  }
  else
  {
    *dest = *src;
    return NW_FALSE;
  }

}

/*****************************************************************
** Name:        StripPortNum
** Description: Finds a valid port reference at the end of a string
**              and writes zero over its first character ':'.
**              Note: A valid port reference is always at the end
**              of a domain name. It consists of ':' and one or
**              more decimal digits. E.g. "foo.com:80"
**
**  Params:     Null-terminated domain potion of Url.
**
**  Return Value: <none>
******************************************************************/
static void StripPortNum(const NW_Ucs2 *domain)
{
  NW_Ucs2 *ptr;
  NW_Ucs2 *save_ptr;
  NW_Int32      i;   /* To count digits in the port number */

  ptr =
  save_ptr = NW_Str_Strrchr(domain, WAE_URL_COLON_CHAR);

  if (ptr)
  {
    for (i = 0, ++ptr; NW_Str_Isdigit(*ptr); ptr++)
      ++i;

    /* A valid port found when there is at least one digit after ':' and
       all remaining string characters (if any) are digits as well */
    if (i > 0 && *ptr == 0)
      *save_ptr = 0;
  }
  return;
}


/*

From 30-APR-98 WML Spec.

If a deck has a DOMAIN and/or PATH attribute, the referring deck's
URL must match the values of the attributes. Matching is done as
follows: the access domain is suffix-matched against the domain name
portion of the referring URL and the access path is prefix matched
against the path portion of the referring URL.

DOMAIN suffix matching is done using the entire element of each
sub-domain and must match each element exactly (eg, www.wapforum.org
shall match wapforum.org, but shall not match forum.org). PATH prefix
matching is done using entire path elements and must match each
element exactly (eg, /X/Y matches /X, but does not match /XZ).
The DOMAIN attribute defaults to the current deck’s domain. The PATH
attribute defaults to the value "/".

On 4-AUG-2000 the following was added:
A port number can be specified in the referring deck URL
and/or in the DOMAIN attribute value. (e.g. www.wapforum.org:80)
The WAP does not comment on such cases in respect to acces control.
The logic in the function below would strip the port number (if any)
before validating access rules.

Note. Some gateways may add the default port number (e.g. ":80") to
the URL saved in the HTTP request header. When the requested page is a
*.asp file with
<head> <access domain="<%=request. ServerVariables("HTTP_HOST")%>" ..
</head>
the domain attribute gets the port number if it was recorded in
the HTTP header.

07-DEC-2000
The pNumLabelDiff parameter added - pointer to the domain label difference
count. The count is 0 if there is an exact match of all complete domain
labels. If a referencing Url has more complete domain labels than the access
domain then the count is for those extra labels
If the input value for "pNumLabelDiff" is NULL then the function does
not perform the label difference calculation.
*
*
*
* RETURN: KBrsrSuccess - access is OK
*          KBrsrFailure - if domain is null or access is denied
*       KBrsrOutOfMemory
*/
TBrowserStatusCode NW_Url_AccessOK(const NW_Ucs2 *domain, const NW_Ucs2 *path,
                            const NW_Ucs2 *url, NW_Int32 *pNumLabelDiff)
{
  NW_Ucs2* pRefDomain = NULL;
  NW_Ucs2* pRefPath   = NULL;
  NW_Ucs2* fullPath   = NULL;
  NW_Ucs2* tmpDomain  = NULL;

  NW_Int32 accLen = 0;
  NW_Int32 refLen = 0;
  NW_Int32 deltaLen;
  NW_Int32    i;
  TBrowserStatusCode status;

  /* Find the host / path separation. */

  /* We require a domain name. */
  if ( !domain || !url )
  {
    status = KBrsrFailure;
    NW_THROW_ERROR();
  }

  status = NW_Url_GetNetwork(url, &pRefDomain);
  if (status != KBrsrSuccess) {
    NW_THROW_ERROR();
  }
  if ( !pRefDomain )
  {
    status = KBrsrFailure;
    NW_THROW_ERROR();
  }

  /* Create a copy of the domain string as
  we are going to modify it */
  tmpDomain = NW_Str_Newcpy(domain);
  if (tmpDomain == NULL)
  {
    status = KBrsrOutOfMemory;
    NW_THROW_ERROR();
  }

  /* We don't need a path; since the default is the root path as long as the
  domain name matches we are OK. */

  /* Now we have 2 domains that should be suffix matched and 2 paths
  that should be prefix matched. */

  /* Strip ":<port number>" (if any) by replacing ':' with 0 */
  StripPortNum(pRefDomain);
  StripPortNum(tmpDomain);

  /*Suffix match the domain names.*/
  refLen = NW_Str_Strlen( pRefDomain );
  accLen = NW_Str_Strlen( tmpDomain );

  if ( accLen > refLen )
  {
    /* Access domain name is longer.  Will never suffix match. */
    status = KBrsrFailure;
    NW_THROW_ERROR();
  }

  for( i = 1; i <= accLen; i++)
  {
    /* Loop over last character to first (0). */
    if ( NW_Str_ToLower(tmpDomain[accLen - i]) !=
         NW_Str_ToLower(pRefDomain[refLen - i]) )
    {
      status = KBrsrFailure;
      NW_THROW_ERROR();
    }
  }

  deltaLen = refLen - accLen;

  /* Check for this case:
  www.wapforum.org shall match wapforum.org, but shall not match forum.org
  */
  /* if pRefDomain is longer than domain, verify first or previous pRefDomain
  character is the path separator */
  if ( deltaLen > 0 )
  {
    if ( (pRefDomain[deltaLen]     != WAE_URL_DOT_CHAR) &&
         (pRefDomain[deltaLen - 1] != WAE_URL_DOT_CHAR) )
    {
      /* first or previous char are not separtor.  Does not match. */
      status = KBrsrFailure;
      NW_THROW_ERROR();
    }
  }

  /* Check for domain labels to support n.10.6 E2ESEC */
  if (pNumLabelDiff != NULL)
  {
    *pNumLabelDiff = 0;
    if (deltaLen > 0)
    {
      for (i = deltaLen; i >= 0; i--)
      {
        if (pRefDomain[i] == WAE_URL_DOT_CHAR) {
          (*pNumLabelDiff)++;
        }
      }
    }
  }

  if ( path )
  {
    status = NW_Url_GetPath(url, &pRefPath);
    if (status != KBrsrSuccess) {
      NW_THROW_ERROR();
    }
  }

  if( pRefPath != NULL )
  {
    NW_Ucs2 *tmp = NULL;

    if (path == NULL)
    {
      status = KBrsrFailure;
      NW_THROW_ERROR();
    }

    /* WMLScript 1.1 Section 6.7.2 and WML 1.1 Section 11.3.1
       says relative paths are converted to absolute. */
    status = NW_Url_RelToAbs(url, path, &tmp);
    if (status != KBrsrSuccess) {
      NW_THROW_ERROR();
    }
    status = NW_Url_GetPath(tmp, &fullPath);
    NW_Str_Delete(tmp);
    if (status != KBrsrSuccess) {
      NW_THROW_ERROR();
    }
    /* Prefix match the paths. */
    accLen = NW_Str_Strlen(fullPath);
    refLen = NW_Str_Strlen(pRefPath);

    if ( accLen > refLen )
    {
      /* Access path name is longer.  Will never prefix match. */
      status = KBrsrFailure;
      NW_THROW_ERROR();
    }

    for( i = 0;  i < accLen; i++)
    {
      if( NW_Str_ToLower(fullPath[i]) !=
          NW_Str_ToLower(pRefPath[i]) )
      {
        status = KBrsrFailure;
        NW_THROW_ERROR();
      }
    }

    /* if pRefPath is longer than path, verify last or next pRefPath
    character is the path separator */
    if ( accLen < refLen )
    {
      if ( ((accLen >= 1) && (pRefPath[accLen-1] != WAE_URL_PATH_CHAR)) &&
           (pRefPath[accLen] != WAE_URL_PATH_CHAR) )
      {
        /* last or next char are not separtor.  Does not match. */
        status = KBrsrFailure;
        NW_THROW_ERROR();
      }
    }
  } /*--end for "pRefPath != NULL" */

  status = KBrsrSuccess;

NW_CATCH_ERROR

  NW_Str_Delete( fullPath );
  NW_Str_Delete( pRefPath );
  NW_Str_Delete( tmpDomain );
  NW_Str_Delete( pRefDomain );
  return status;
}

/**
 ** check for whether the url pass is relative URL or absolute URL
 ** if it's not a valid URL, then return NW_STAT_FALLURE
 ** if it's a valid relative url, return KBrsrSuccess with *isRelative set to TRUE
 ** if it's a valid absolute url, return KBrsrSuccess with *isRelative set to FALSE
 **/
TBrowserStatusCode NW_Url_IsRelative(const NW_Ucs2 *url, NW_Bool *isRelative)
{
  NW_Rfc2396_Parts_t urlPieces;

  NW_ASSERT(isRelative);

  *isRelative = NW_TRUE;

  if (!NW_Url_GetRfc2396Parts(url, &urlPieces))
  {
    return KBrsrFailure;
  }

  if (urlPieces.schemeStart != NULL) {
    *isRelative = NW_FALSE;
  }
  /* else isRelative is true */

  return KBrsrSuccess;
}

/*
* RETURN: KBrsrSuccess
*       KBrsrOutOfMemory
*/
TBrowserStatusCode build_url( const NW_Ucs2 *scheme, const NW_Ucs2 *net_loc,
                       const NW_Ucs2 *path, const NW_Ucs2 *query,
                       const NW_Ucs2 *frag, NW_Ucs2 **ret_string)
{
  NW_Uint32  len = 0;

  if (scheme) {
    len += (NW_Str_Strlen(scheme) + 1);
  }
  if (net_loc) {
    len += (NW_Str_Strlen(net_loc) + 2);
  }
  if (path) {
    len += (NW_Str_Strlen(path));
  }
  if (query) {
    len += (NW_Str_Strlen(query) + 1);
  }
  if (frag) {
    len += (NW_Str_Strlen(frag) + 1);
  }

  *ret_string = NW_Str_New(len);
  if (*ret_string == NULL)
    return KBrsrOutOfMemory;

  (*ret_string)[0] = 0;

  if (scheme)
  {
    (void)NW_Str_Strcat(*ret_string, scheme);
    (void)NW_Str_StrcatConst(*ret_string, WAE_URL_COLON_SEP);
  }

  if (net_loc)
  {
    (void)NW_Str_StrcatConst(*ret_string, WAE_URL_NETLOC_SEP);
    (void)NW_Str_Strcat(*ret_string, net_loc);
  }

  if (path)
  {
    (void)NW_Str_Strcat(*ret_string, path);
  }

  if (query)
  {
    (void)NW_Str_StrcatConst(*ret_string, WAE_URL_QUERY_SEP);
    (void)NW_Str_Strcat(*ret_string, query);
  }

  if (frag)
  {
    (void)NW_Str_StrcatConst(*ret_string, WAE_URL_FRAG_SEP);
    (void)NW_Str_Strcat(*ret_string, frag);
  }

  return KBrsrSuccess;
}


static NW_Int32  num_of_segs(const NW_Ucs2 *path)
{
  NW_Int32  res = 0;

  while(*path)
  {
    if (*path++ == WAE_URL_PATH_CHAR) res++;
  }
  return res;
}

/*
* from a/b/c and a/d/f create ../f
*
* RETURN: KBrsrSuccess
*       KBrsrFailure - bad input
*       KBrsrOutOfMemory
*/
TBrowserStatusCode diff_paths(const NW_Ucs2 *base_path, const NW_Ucs2 *url_path,
                       NW_Ucs2 **ret_string)
{
  NW_Ucs2 *base_start = 0, *url_start = 0;
  NW_Ucs2 *base_dir = 0, *url_dir = 0, *url_file = 0, *base_file = 0;
  NW_Uint32 len;

  NW_ASSERT(url_path != NULL);
  NW_ASSERT(base_path != NULL);
  NW_ASSERT(ret_string != NULL);
  *ret_string = NULL;

  base_file = NW_Str_Strrchr(base_path, WAE_URL_PATH_CHAR);

  if (base_file)
  {
    NW_ASSERT(base_file - base_path + 1 >= 0);
    len = (NW_Uint32)(base_file - base_path + 1); /* include the / in the end */

    base_dir = NW_Str_Substr(base_path, 0, len);
    if (base_dir == NULL) {
      return KBrsrOutOfMemory;
    }
  }

  url_file = NW_Str_Strrchr(url_path, WAE_URL_PATH_CHAR);

  if (url_file)
  {
    len = url_file - url_path + 1; /* include the / in the end */
    url_dir = NW_Str_Substr(url_path, 0, len);
    if (url_dir == NULL) {
      /* Note: NW_Str_Delete() handles null pointers */
      NW_Str_Delete(base_dir);
      return KBrsrOutOfMemory;
    }
  }

  base_start  = base_dir;
  url_start   = url_dir;

  /* find common prefix */
  for(; *base_start; base_start++, url_start++)
  {
    if (*base_start != *url_start) break;
  }
  /* remove the leading '/' */
  url_file++;

  if (! *base_start && (*url_start == WAE_URL_PATH_CHAR))
  {
    /* same directory */
    *ret_string = NW_Str_Newcpy(url_file);
    if (*ret_string == NULL)
    {
      /* Note: NW_Str_Delete() handles null pointers */
      NW_Str_Delete(base_dir);
      NW_Str_Delete(url_dir);
      return KBrsrOutOfMemory;
    }
    if (NW_Str_Strlen(*ret_string) == 0) {
      NW_Str_Delete(*ret_string);
      *ret_string = NW_Str_NewcpyConst(WAE_URL_CURRENT_DIR);
      if (*ret_string == NULL) {
        NW_Str_Delete(base_dir);
        NW_Str_Delete(url_dir);
        return KBrsrOutOfMemory;
      }
    }
  }
  else
  {
    NW_Int32  segs;

    /* find where the last common path segment ends */
    for(;;)
    {
      if ((base_start > base_dir) == NW_FALSE) /* the first char is / */
      {
        /* Note: NW_Str_Delete() handles null pointers */
        NW_Str_Delete(base_dir);
        NW_Str_Delete(url_dir);
        return KBrsrFailure;
      }

      if (*(base_start - 1) == WAE_URL_PATH_CHAR) break;
      else base_start--;
    }

    /* calculate how many "../" are needed before adding the url_path */
    segs = (base_start? num_of_segs(base_start) : 0);

    /* make sure there's room for "./" if segs == 0 */
    len = segs * NW_Asc_strlen(WAE_URL_PARENT_DIR)
          + NW_Asc_strlen(WAE_URL_CURRENT_DIR)
          + NW_Str_Strlen(url_start)
          + NW_Str_Strlen(url_file);
    *ret_string = NW_Str_New(len);
    if (*ret_string == NULL)
    {
      /* Note: NW_Str_Delete() handles null pointers */
      NW_Str_Delete(base_dir);
      NW_Str_Delete(url_dir);
      return KBrsrOutOfMemory;
    }

    for (;segs > 0; segs--) {
      (void)NW_Str_StrcatConst(*ret_string, WAE_URL_PARENT_DIR);
    }

    (void)NW_Str_Strcat(*ret_string, url_start);
    (void)NW_Str_Strcat(*ret_string, url_file);

    if (NW_Str_Strlen(*ret_string) == 0) {
      (void)NW_Str_StrcatConst(*ret_string, WAE_URL_CURRENT_DIR);
    }
  }

  /* Note: NW_Str_Delete() handles null pointers */
  NW_Str_Delete(base_dir);
  NW_Str_Delete(url_dir);

  return KBrsrSuccess;
}

/*
* RETURN: KBrsrSuccess
*          KBrsrFailure - any failure
*              - base or url are null
*       KBrsrOutOfMemory
*/
TBrowserStatusCode NW_Url_AbsToRel(const NW_Ucs2 *base, const NW_Ucs2 *url,
                            NW_Ucs2 **ret_string)
{
  NW_Ucs2 *base_scheme = 0;
  NW_Ucs2 *url_scheme = 0;

  NW_Ucs2 *base_net_loc = 0;
  NW_Ucs2 *url_net_loc = 0;

  NW_Ucs2 *base_path = 0;
  NW_Ucs2 *url_path = 0;
  NW_Ucs2 *res_path = 0;

  NW_Ucs2 *url_query = 0;
  NW_Ucs2 *url_frag = 0;

  NW_Uint32  len;

  TBrowserStatusCode status = KBrsrSuccess;

  NW_ASSERT(ret_string);
  *ret_string = NULL;

  if (!base || !url) return KBrsrFailure;

  if ((status = NW_Url_GetSchemeStr(base, &base_scheme)) == KBrsrOutOfMemory)
    NW_THROW_ERROR();

  if ((status = NW_Url_GetSchemeStr(url, &url_scheme)) == KBrsrOutOfMemory)
    NW_THROW_ERROR();

  if ((status = NW_Url_GetNetwork(base, &base_net_loc)) == KBrsrOutOfMemory)
    NW_THROW_ERROR();

  if ((status = NW_Url_GetNetwork(url, &url_net_loc)) == KBrsrOutOfMemory)
    NW_THROW_ERROR();

  if ((status = NW_Url_GetQuery(url, &url_query)) == KBrsrOutOfMemory)
    NW_THROW_ERROR();

  if ((status = NW_Url_GetFragId(url, &url_frag, NW_TRUE)) == KBrsrOutOfMemory)
    NW_THROW_ERROR();

  /* assume both urls are absolute */

  if (!base_scheme || !url_scheme || !base_net_loc || !url_net_loc)
  {
    status = KBrsrFailure;
    NW_THROW_ERROR();
  }

  if (NW_Str_Strcmp(base_scheme, url_scheme) ||
      NW_Str_Strcmp(base_net_loc, url_net_loc))
  {
    *ret_string = NW_Str_Newcpy(url);
    if (*ret_string == NULL) {
      status = KBrsrOutOfMemory;
    }
    NW_THROW_ERROR();
  }
  else
  {
    status = NW_Url_GetPath(base, &base_path);
    if (status != KBrsrSuccess)
      NW_THROW_ERROR();

    status = NW_Url_GetPath(url, &url_path);
    if (status != KBrsrSuccess)
      NW_THROW_ERROR();

    /* all WAP urls have path */
    if (!base_path || !url_path)
    {
      status = KBrsrFailure;
      NW_THROW_ERROR();
    }

    status = diff_paths(base_path, url_path, &res_path);
    if (status != KBrsrSuccess)
      NW_THROW_ERROR();
  }

  len = 0;
  if (res_path) {
    len += NW_Str_Strlen(res_path);
  }
  if (url_query) {
    len += (NW_Str_Strlen(url_query) + NW_Asc_strlen(WAE_URL_QUERY_SEP));
  }
  if (url_frag) {
    len += NW_Str_Strlen(url_frag);
  }

  *ret_string = NW_Str_New(len);
  if (*ret_string == NULL)
  {
    status = KBrsrOutOfMemory;
    NW_THROW_ERROR();
  }

  if (res_path) {
    (void)NW_Str_Strcat(*ret_string, res_path);
  }
  if (url_query) {
    (void)NW_Str_StrcatConst(*ret_string, WAE_URL_QUERY_SEP);
    (void)NW_Str_Strcat(*ret_string, url_query);
  }
  if (url_frag) {
    (void)NW_Str_Strcat(*ret_string, url_frag);
  }

NW_CATCH_ERROR
  NW_Str_Delete(base_scheme);
  NW_Str_Delete(url_scheme);
  NW_Str_Delete(base_net_loc);
  NW_Str_Delete(url_net_loc);
  NW_Str_Delete(base_path);
  NW_Str_Delete(url_path);
  NW_Str_Delete(res_path);
  NW_Str_Delete(url_query);
  NW_Str_Delete(url_frag);

  return status;
}

TBrowserStatusCode NW_Url_BuildUrl(const NW_Ucs2 *src,
                            const NW_Url_Schema_t scheme,
                            NW_Ucs2 **url)
{

  TBrowserStatusCode status = KBrsrMalformedUrl;
  const char *schemeConst;
  NW_Ucs2 *scheme_str   = NULL;
  NW_Ucs2 *network_loc  = NULL;
  NW_Ucs2 *path         = NULL;
  NW_Ucs2 *query        = NULL;
  NW_Ucs2 *frag         = NULL;

  if(NW_Url_IsValid(src))
  {
    /* get the schemata strings */
    switch (scheme)
    {
    case NW_SCHEMA_HTTPS:
      schemeConst = WAE_URL_HTTPS_SCHEME;
      break;
    case NW_SCHEMA_FTP:
      schemeConst = WAE_URL_FTP_SCHEME;
      break;
    case NW_SCHEMA_GOPHER:
      schemeConst = WAE_URL_GOPHER_SCHEME;
      break;
    case NW_SCHEMA_MAILTO:
      schemeConst = WAE_URL_MAILTO_SCHEME;
      break;
    case NW_SCHEMA_NEWS:
      schemeConst = WAE_URL_NEWS_SCHEME;
      break;
    case NW_SCHEMA_NNTP:
      schemeConst = WAE_URL_NNTP_SCHEME;
      break;
    case NW_SCHEMA_TELNET:
      schemeConst = WAE_URL_TELNET_SCHEME;
      break;
    case NW_SCHEMA_WAIS:
      schemeConst = WAE_URL_WAIS_SCHEME;
      break;
    case NW_SCHEMA_FILE:
      schemeConst = WAE_URL_FILE_SCHEME;
      break;
    case NW_SCHEMA_HTTP:
      schemeConst = WAE_URL_HTTP_SCHEME;
      break;
    case NW_SCHEMA_PROSPERO:
      schemeConst = WAE_URL_PROSPERO_SCHEME;
      break;
    case NW_SCHEMA_TEL:
      schemeConst = WAE_URL_TEL_SCHEME;
      break;
    case NW_SCHEMA_UNKNOWN: /* fall through */
    default:
      return KBrsrBadInputParam;
    }

    scheme_str = NW_Str_NewcpyConst(schemeConst);
    if (scheme_str == NULL)
      NW_THROW_ERROR();

    if ((status = NW_Url_GetNetwork(src, &network_loc)) == KBrsrOutOfMemory)
      NW_THROW_ERROR();

    if (network_loc == NULL) {
      /* to add network seperator later */
      network_loc = NW_Str_NewcpyConst("");
    }

    if ((status = NW_Url_GetPath(src, &path)) == KBrsrOutOfMemory)
       NW_THROW_ERROR();

    if ((status = NW_Url_GetQuery(src, &query)) == KBrsrOutOfMemory)
       NW_THROW_ERROR();

    if ((status = NW_Url_GetFragId(src, &frag, NW_FALSE)) == KBrsrOutOfMemory)
       NW_THROW_ERROR();


    status = build_url(scheme_str, network_loc, path,
                      query,  frag, url);
    NW_THROW_ERROR();

  }

NW_CATCH_ERROR
  NW_Str_Delete(scheme_str);
  NW_Str_Delete(network_loc);
  NW_Str_Delete(path);
  NW_Str_Delete(query);
  NW_Str_Delete(frag);
  return status;
}



/*------------------------------------------------------------
 * NW_Url_Fragment
 *
 * return pointer to start of card name in url (the part
 * right after the '#' character), or NULL if no '#'
 * exists.  The card name may be the empty
 * string if no characters follow the '#' sign.
 *
 * NOTE: This assumes that you, the caller, have verified
 * the validity of the url (you can do this easily by
 * calling NW_Url_GetRfc2395Parts).
 *
 * NOTE: This does not allocate memory -- it only returns a
 * pointer to the fragment
 *----------------------------------------------------------*/

NW_Ucs2 *NW_Url_Fragment(NW_Ucs2 *url)
{
  NW_Bool result;
  NW_Rfc2396_Parts_t parts = {0};

  NW_ASSERT(url != NULL);
  result = NW_Url_GetRfc2396Parts(url, &parts);
  NW_ASSERT(result);
	/* To fix TI compiler warning */
	(void) result;

  return (NW_Ucs2*)(parts.fragStart);
}

/*
 * NW_Url_BuildAbsUrl -- construct a new URL. The resultant URL consists of
 * the base of the 'url' parameter, plus the 'card_name', if any.
 * The resultant url is placed in the ret_string.  It is the caller's
 * responsibility to delete the resulting string.
 *
 * The 'card_name' parameter may be NULL, in which case the return value
 * will be the base portion of the 'url' parameter.
 *
 * RETURNS
 *   KBrsrSuccess
 *   KBrsrMalformedUrl
 *   KBrsrOutOfMemory
 */
TBrowserStatusCode NW_Url_BuildAbsUrl(const NW_Ucs2 *url,
                               const NW_Ucs2 *card_name,
                               NW_Ucs2 **ret_string)
{
  NW_Ucs2     *base = NULL;
  TBrowserStatusCode   status;

  NW_ASSERT(url != NULL);
  NW_ASSERT(ret_string != NULL);

  *ret_string = NULL;

  if ((status = NW_Url_GetBase(url, &base)) == KBrsrSuccess)
  {
    NW_ASSERT(base != NULL);
    if (card_name != NULL)
    {
      NW_Ucs2 *fragId = NW_Str_New(NW_Str_Strlen(card_name) + 1 + 1);
      if (fragId == NULL) {
        NW_Str_Delete(base);
        return KBrsrOutOfMemory;
      }
      *fragId = WAE_URL_FRAG_CHAR;
      NW_Str_Strcpy(fragId+1,card_name);
      status = NW_Url_RelToAbs(base,fragId,ret_string);
      NW_Str_Delete(fragId);
      NW_Str_Delete(base);
    }
    else
      *ret_string = base;
  }
  return status;
}
/*
**-------------------------------------------------------------------------
** This function will return the scheme specific string in the URL. ie.
** scheme-specific-part. The caller has to free the ret_string.
** Absolute URI syntax as per the RFC 2396: <scheme>:<scheme-specific-part>
** RETURNS:     KBrsrMalformedUrl
**              KBrsrOutOfMemory
**              KBrsrSuccess
**-------------------------------------------------------------------------
*/
TBrowserStatusCode NW_Url_GetSchemeSpecificPart(const NW_Ucs2 *url_buff, NW_Ucs2 **ret_string)
{
  NW_Rfc2396_Parts_t  urlPieces;
  const NW_Ucs2     *pStart, *pEnd;

  NW_ASSERT(url_buff != NULL);

  /* Get the structure of URL-reference - RFC2396 */
  if (!NW_Url_GetRfc2396Parts(url_buff, &urlPieces)) {
    return KBrsrMalformedUrl;
  }

  pStart = urlPieces.schemeEnd;
  NW_ASSERT(pStart != NULL);
  pStart++;  /* scheme seperator */
  NW_ASSERT(pStart != NULL);

  pEnd = urlPieces.schemeStart + NW_Str_Strlen(url_buff);
  NW_ASSERT(pEnd != NULL);

  /* malloc enough for the string and the null terminator. */
  *ret_string = NW_Str_Substr(pStart, 0, ((NW_Uint32)(pEnd - pStart)));
  if (*ret_string == NULL)
  {
    return KBrsrOutOfMemory;
  }
  return KBrsrSuccess;
}

/*******************************************************************
  Name:         NW_Url_EscapeUnwiseLen

  Description:  Gives the length of string for escaping illegal chars

  Parameters:   src --  pointer to the string containing url

  Returns:      length of string
********************************************************************/
//??? RFD: This will only produce the correct results if the string has already been converted to UTF8.
NW_Uint32   NW_Url_EscapeUnwiseLen( const NW_Ucs2 *src )
{
  NW_Uint32  len = 0,
  pos = 0,
  numToEsc = 0;

  if( src )
    len = NW_Str_Strlen(src);
  else
    len = 0;

  for( pos = 0; pos < len; pos++ )
  {
    if( shouldEscape( src[pos], NW_FALSE) )
    {
      numToEsc++;
    }
  }

  return (2 * numToEsc) + len;
}

/*******************************************************************
  Name:         NW_Url_EscapeUnwise

  Description:  Escapes all the characters except legal chars

  Parameters:   src --  pointer to the string containing url
                dest -  pointer to the string where escaped
                        Url is to be stored

  Returns:      NW_Bool - true if successful
                        false if unsuccessful
********************************************************************/
//??? RFD: May need to cleanup all methods that call this.
NW_Bool NW_Url_EscapeUnwise( const NW_Ucs2 *src, NW_Ucs2 *dest)
{
  NW_Int32   len = 0,
  posInSrc = 0,
  posInDest = 0;
  NW_Bool allASCII=NW_TRUE;

  NW_ASSERT(dest);

  if( src )
    len = NW_Str_Strlen(src);
  else
    len = 0;

  /* Walk through the source and escape any characters into destination. */
  posInDest = 0;
  for( posInSrc = 0; posInSrc < len; posInSrc++ )
  {
//??? RFD: Why is this check made? Chars between 0x80 && 0xFF can and are escaped.
    if (src[posInSrc] > 127) {
      allASCII = NW_FALSE;
    }

    if (escape_char( &src[posInSrc], &dest[posInDest], NW_FALSE)) {
      posInDest += 3;
    } else {
      posInDest++;
    }
  }
  dest[posInDest] = 0;
//??? RFD: Description of return value above states TRUE if successful; else FALSE. Doesn't say anything about all ASCII.
  return allASCII;

}

/*******************************************************************
  Name:         NW_Url_ConvertFileToEscapeURL

  Description:  Converts an file URL to escaped Url
                Escapes all the charactes in Url except legal url
                characters

  Parameters:   src --  pointer to the string containing url

  Returns:      new string which contains URL in escaped form.
                Returns NULL if allocation fails.
********************************************************************/

NW_Ucs2* NW_Url_ConvertFileToEscapeURL(const NW_Ucs2 *src)
{
  NW_Ucs2 *escapedUrl = NULL;
	NW_Uint32 len = 0;

  len = NW_Url_EscapeUnwiseLen(src);
  escapedUrl = NW_Str_New(len);
  if (escapedUrl == NULL) {
    return NULL;
  }

  NW_Url_EscapeUnwise(src, escapedUrl);

  return escapedUrl;

}

/*******************************************************************
  Name:         NW_Url_ConvertFileFromEscapeURL

  Description:  Converts an Escaped URL to file Url

  Parameters:   src --  pointer to the string containing url

  Algorithm :   Unescapes all the escapes sequence

  Returns:      new string which contains URL in unescaped form.
********************************************************************/

NW_Ucs2* NW_Url_ConvertFileFromEscapeURL(const NW_Ucs2 *src)
{
  NW_Ucs2 *unescapedUrl = NULL;
	NW_Int32 len = 0;

  len = NW_Url_UnEscapeStringLen(src);

  NW_ASSERT(len >= 0);
  unescapedUrl = NW_Str_New((NW_Uint32)len);
  if (unescapedUrl == NULL) {
    return NULL;
  }

  NW_Url_UnEscapeString(src, unescapedUrl);

  return unescapedUrl;

}

/*******************************************************************
  Name:         NW_Url_GetSchemeNoValidation

  Description:  Get the URL Schema without checking the validity
                of the URL.
                eg. if url is "file://c:\hello dir\aa.wmlc" it does not
                check if url contans illegal chars or not.

  Parameters:   url -- pointer to string containg url
                ret_scheme - scheme returned

  Algorithm :   Checks for first occurance of  ":" in the string
                Compares the characters before ":" to valid url
                schemas and return the schema

  Returns:      TBrowserStatusCode - status
                KBrsrSuccess : if scheme found or unknown scheme
                KBrsrFailure : if no scheme (no ":" char in url)
                KBrsrOutOfMemory : if out of memory
********************************************************************/
TBrowserStatusCode NW_Url_GetSchemeNoValidation(const NW_Ucs2* url, NW_Url_Schema_t* ret_scheme)
    {
    NW_Ucs2 *scheme = NULL;
    NW_Ucs2 *seperator = NULL;

    NW_ASSERT(url);
    NW_ASSERT(ret_scheme);

    *ret_scheme = NW_SCHEMA_INVALID;

    seperator = NW_Str_Strchr(url, WAE_URL_COLON_CHAR);
    if (!seperator)
        {
        return KBrsrFailure;
        }

    NW_ASSERT(seperator >= url);
    scheme = NW_Str_Substr(url, 0, ((NW_Uint32)(seperator - url)));
    if (!scheme)
        {
        return KBrsrOutOfMemory;
        }

    /* RFC2396 sect 3.1 says "programs interpreting URI should
    treat upper case letters as equivalient to lower case in
    scheme names"  */
    NW_Str_Strlwr(scheme);
    *ret_scheme = returnschema(scheme);
    NW_Str_Delete(scheme);

    return KBrsrSuccess;
    }

/*
Below function compares the parameter scheme with standard schema type and returns
oe of the constants for the schema types.

eg . if value of scheme is "file" or "FILE" then it returns NW_SCHEMA_FILE.
     if value is not one of the valid types it returns NW_SCHEMA_UNKNOWN

*/

NW_Url_Schema_t returnschema(const NW_Ucs2 *scheme)
{
  NW_ASSERT(scheme);

  if (NW_Str_StrcmpConst(scheme, WAE_URL_HTTP_SCHEME) == 0) {
      return NW_SCHEMA_HTTP;
  } else if (NW_Str_StrcmpConst(scheme, WAE_URL_HTTPS_SCHEME) == 0) {
	    return NW_SCHEMA_HTTPS;
	} else if (NW_Str_StrcmpConst(scheme, WAE_URL_FTP_SCHEME) == 0) {
	    return NW_SCHEMA_FTP;
  } else if (NW_Str_StrcmpConst(scheme, WAE_URL_GOPHER_SCHEME) == 0) {
	    return NW_SCHEMA_GOPHER;
  } else if (NW_Str_StrcmpConst(scheme, WAE_URL_MAILTO_SCHEME) == 0) {
	  	return NW_SCHEMA_MAILTO;
  } else if (NW_Str_StrcmpConst(scheme, WAE_URL_NEWS_SCHEME) == 0) {
	    return NW_SCHEMA_NEWS;
  } else if (NW_Str_StrcmpConst(scheme, WAE_URL_NNTP_SCHEME) == 0) {
		  return NW_SCHEMA_NNTP;
  } else if (NW_Str_StrcmpConst(scheme, WAE_URL_TELNET_SCHEME) == 0) {
		  return NW_SCHEMA_TELNET;
  } else if (NW_Str_StrcmpConst(scheme, WAE_URL_WAIS_SCHEME) == 0) {
		  return NW_SCHEMA_WAIS;
  } else if (NW_Str_StrcmpConst(scheme, WAE_URL_FILE_SCHEME) == 0) {
		  return NW_SCHEMA_FILE;
  } else if (NW_Str_StrcmpConst(scheme, WAE_URL_WTAI_SCHEME) == 0) {
		  return NW_SCHEMA_WTAI;
  } else if (NW_Str_StrcmpConst(scheme, WAE_URL_PROSPERO_SCHEME) == 0) {
		  return NW_SCHEMA_PROSPERO;
  } else if (NW_Str_StrcmpConst(scheme, WAE_URL_TEL_SCHEME) == 0) {
		  return NW_SCHEMA_TEL;
  }else {
  	  return NW_SCHEMA_UNKNOWN;
  }
}

/* Copies a UCS2, potentially foreign char, into a buffer, returning # of bytes
   copied into the buffer. Input char is assumed to be in little endian. */
NW_Uint32 writeForeignCharBuff(const NW_Byte* c, NW_Byte *buff)
{
  NW_Uint32 cnt;

  NW_ASSERT(buff != NULL);

  buff[0] = *c++;
  cnt = (*c) ? 2 : 1;
  buff[1] = *c;
  return cnt;
}

/* Schema other than {http, https, file} are handled via SchemeDispatcher.
   They are treated as absolute URLs.
 */
TBrowserStatusCode
NW_Url_IsBrowserScheme(const NW_Ucs2 *pUri, NW_Bool *isBrowserScheme)
    {
    TBrowserStatusCode status;
    NW_Url_Schema_t schema;

    status = NW_Url_GetSchemeNoValidation(pUri, &schema);

    switch (schema)
        {

        case NW_SCHEMA_HTTPS:
        case NW_SCHEMA_HTTP:
        case NW_SCHEMA_FILE:
            /* These are handled by BrowserEngine */
            *isBrowserScheme = NW_TRUE;
            break;

        default:
            /* Not handled by BrowserEngine */
            *isBrowserScheme = NW_FALSE;
            break;

        }

    return status;
    }


/*******************************************************************
  Name:         NW_Url_RemoveCharsLen

  Description:  Url after removing the chars passed in whenever they appears

  Parameters:   src --  pointer to the string containing url
                chars -  chars to be removed

  Returns:      void
********************************************************************/
NW_Uint32 NW_Url_RemoveCharsLen( const NW_Ucs2 *src, const NW_Ucs2 *chs )
{
  NW_Int32  len = 0;
  NW_Int32  posInSrc = 0;
  NW_Int32  lenCh = 0;
  NW_Int32  posInCh = 0;
  NW_Bool found;

  NW_ASSERT(chs);

  if( src )
    len = NW_Str_Strlen(src);
  else
    len = 0;

  if( chs )
    lenCh = NW_Str_Strlen(chs);
  else
    lenCh = 0;

  /* Walk through the source and remove any characters from chs and build destination. */
  for( posInSrc = 0; posInSrc < len; posInSrc++ )
    {
    found = NW_FALSE;

    for( posInCh = 0; posInCh < lenCh; posInCh++ )
      {
      if (src[posInSrc] == chs[posInCh])
        {
        found = NW_TRUE;
        break;
        }
      }

    if (found)
      {
      len--;
      }
    }

  return len;
}


/*******************************************************************
  Name:         NW_Url_RemoveChars

  Description:  Creates a copy of a string but with specific chars removed.

  Parameters:   src -  pointer to the source string.
                dest - pointer to the destination into which the source.
                       string will be copied with specific chars removed.
                chs -  list of chars to be removed.

  Returns:      void
********************************************************************/
void NW_Url_RemoveChars( const NW_Ucs2 *src, NW_Ucs2 *dest, const NW_Ucs2 *chs )
{
  NW_Int32  len = 0;
  NW_Int32  posInSrc = 0;
  NW_Int32  posInDest = 0;
  NW_Int32  lenCh = 0;
  NW_Int32  posInCh = 0;
  NW_Bool found;

  NW_ASSERT(dest);
  NW_ASSERT(chs);

  if( src )
    len = NW_Str_Strlen(src);
  else
    len = 0;

  if( chs )
    lenCh = NW_Str_Strlen(chs);
  else
    lenCh = 0;

  /* Walk through the source and remove any characters from chs and build destination. */
  for( posInSrc = 0; posInSrc < len; posInSrc++ )
    {
    found = NW_FALSE;

    for( posInCh = 0; posInCh < lenCh; posInCh++ )
      {
      if (src[posInSrc] == chs[posInCh])
        {
        found = NW_TRUE;
        break;
        }
      }

    if (!found)
      {
      dest[posInDest] = src[posInSrc];
      posInDest++;
      }
    }

  dest[posInDest] = 0;
}

/*******************************************************************
  Name:         NW_Url_GetUrlFileExtension

  Description:  Returns the file extension from an url
  Parameters:   src -  pointer to the source string.

  Returns:      ext - pointer to the file extension.
********************************************************************/
NW_Ucs2* NW_Url_GetUrlFileExtension(const NW_Ucs2 *src)
{
    NW_Ucs2* fileName = NW_Str_Strrchr(src, WAE_ASCII_POINT);
    if (fileName == NULL)
      {
      return NULL;
      }
    return NW_Str_Substr(src, (NW_Str_Strlen(src) - NW_Str_Strlen(fileName) + 1), (NW_Str_Strlen(fileName) - 1));
}
/*lint -restore*/
