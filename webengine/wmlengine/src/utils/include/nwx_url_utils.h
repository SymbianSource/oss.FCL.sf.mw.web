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
    $Workfile: nwx_url_utils.h $

    Purpose:

       This file is used by the wae_urlparse.h.  It defines the prototypes 
       for all the functions needed for parsing a URL (both relative and absolute)

*/
        
#ifndef NWX_URL_UTILS_H
#define NWX_URL_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif


/*
** Includes
*/

#include "nwx_defs.h"
#include "BrsrStatusCodes.h"



#define  WAE_URL_ESCAPE_CHAR  '%'
#define  WAE_URL_FRAG_CHAR    '#'

/* reserved characters -- as per rfc2396 */
#define  WAE_URL_PARAM_CHAR   ';'
#define  WAE_URL_PATH_CHAR    '/'
#define  WAE_URL_QUERY_CHAR   '?'
#define  WAE_URL_COLON_CHAR   ':'
#define  WAE_URL_AT_CHAR      '@'
#define  WAE_URL_AMP_CHAR     '&'
#define  WAE_URL_EQUAL_CHAR   '='
#define  WAE_URL_PLUS_CHAR    '+'
#define  WAE_URL_DOLLAR_CHAR  '$'
#define  WAE_URL_COMMA_CHAR   ','

/* reserved IPv6 characters -- as per rfc 2732 */
#define WAE_URL_LEFT_SQUARE_BRACKET_CHAR     '['
#define WAE_URL_RIGHT_SQUARE_BRACKET_CHAR    ']'

/* mark characters -- as per definition in rfc2396 */
#define  WAE_URL_DASH_CHAR    '-'
#define  WAE_URL_UNDER_CHAR   '_'
#define  WAE_URL_DOT_CHAR     '.'
#define  WAE_URL_BANG_CHAR    '!'
#define  WAE_URL_TILDE_CHAR   '~'
#define  WAE_URL_STAR_CHAR    '*'
#define  WAE_URL_APOST_CHAR   '\''
#define  WAE_URL_PAREN_CHAR   '('
#define  WAE_URL_THESIS_CHAR  ')'


/* String that separates a scheme from a url */
#define WAE_URL_NETLOC_SEP       (const char*)"//"
#define WAE_URL_PATH_SEP         (const char*)"/"
#define WAE_URL_QUERY_SEP        (const char*)"?"
#define WAE_URL_FRAG_SEP         (const char*)"#"
#define WAE_URL_COLON_SEP        (const char*)":"

#define WAE_URL_POSTFIELD_SEP    (const char*)"&"

#define WAE_URL_CURRENT_DIR      (const char*)"./"
#define WAE_URL_PARENT_DIR       (const char*)"../"

/* Schemata strings */
#define WAE_URL_HTTP_SCHEME      (const char*)"http"
#define WAE_URL_HTTPS_SCHEME     (const char*)"https"
#define WAE_URL_FTP_SCHEME       (const char*)"ftp"
#define WAE_URL_GOPHER_SCHEME    (const char*)"gopher"
#define WAE_URL_MAILTO_SCHEME    (const char*)"mailto"
#define WAE_URL_NEWS_SCHEME      (const char*)"news"
#define WAE_URL_NNTP_SCHEME      (const char*)"nntp"
#define WAE_URL_TELNET_SCHEME    (const char*)"telnet"
#define WAE_URL_WAIS_SCHEME      (const char*)"wais"
#define WAE_URL_FILE_SCHEME      (const char*)"file"
#define WAE_URL_PROSPERO_SCHEME  (const char*)"prospero"
#define WAE_URL_WTAI_SCHEME      (const char*)"wtai"
#define WAE_URL_TEL_SCHEME       (const char*)"tel"

#define WAE_URL_HTTP_PORT_STR    (const char*)"80"
#define WAE_URL_HTTP_PORT        80

#define WAE_URL_CALENDAR_EXT     (const char*)"vcs"
#define WAE_URL_JPG_EXT          (const char*)"jpg"

/*
**  Global Types
*/
typedef enum {
  NW_SCHEMA_INVALID = 0,
  NW_SCHEMA_UNKNOWN, 
  NW_SCHEMA_HTTPS,
  NW_SCHEMA_FTP,
  NW_SCHEMA_GOPHER,
  NW_SCHEMA_MAILTO,
  NW_SCHEMA_NEWS,
  NW_SCHEMA_NNTP,
  NW_SCHEMA_TELNET,
  NW_SCHEMA_WAIS,
  NW_SCHEMA_FILE,
  NW_SCHEMA_HTTP,
  NW_SCHEMA_PROSPERO,
  NW_SCHEMA_WTAI,
  NW_SCHEMA_TEL
} NW_Url_Schema_t; 


typedef struct
{
  const NW_Ucs2 *schemeStart;
  const NW_Ucs2 *schemeEnd;
  
  const NW_Ucs2 *opaqueStart;
  const NW_Ucs2 *opaqueEnd;
  
  const NW_Ucs2 *authorityStart;
  const NW_Ucs2 *authorityEnd;
  
  const NW_Ucs2 *userInfoStart;
  const NW_Ucs2 *userInfoEnd;
  
  const NW_Ucs2 *hostStart;
  const NW_Ucs2 *hostEnd;
  
  const NW_Ucs2 *portStart;
  const NW_Ucs2 *portEnd;
  
  const NW_Ucs2 *pathStart;
  const NW_Ucs2 *pathEnd;
  
  const NW_Ucs2 *queryStart;
  const NW_Ucs2 *queryEnd;
  
  const NW_Ucs2 *fragStart;
  const NW_Ucs2 *fragEnd;
  
  NW_Bool     isIPAddress;
  
} NW_Rfc2396_Parts_t;


TBrowserStatusCode NW_Url_GetScheme( const NW_Ucs2 *url_buff, NW_Url_Schema_t *ret_scheme);

TBrowserStatusCode NW_Url_GetSchemeStr( const NW_Ucs2 *url_buff, NW_Ucs2 **ret_string );
/* Function to get a scheme specific part */
TBrowserStatusCode NW_Url_GetSchemeSpecificPart(const NW_Ucs2 *url_buff, NW_Ucs2 **ret_string);
TBrowserStatusCode NW_Url_GetParameters( const NW_Ucs2 *url_buff, NW_Ucs2 **ret_string );
TBrowserStatusCode NW_Url_GetBase( const NW_Ucs2 *url_buff, NW_Ucs2 **ret_string );
NW_Bool NW_Url_IsValid(const NW_Ucs2 *pUrl);
NW_Bool NW_Url_GetRfc2396Parts( const NW_Ucs2 *pUri, NW_Rfc2396_Parts_t *parts );

TBrowserStatusCode NW_Url_GetPort( const NW_Ucs2 *url_buff, NW_Bool *is_valid, NW_Ucs2 **ret_string );
TBrowserStatusCode NW_Url_GetFragId(const NW_Ucs2 *url_buff, NW_Ucs2 ** ret_string, NW_Bool incFragChar);
TBrowserStatusCode NW_Url_GetNetwork(const NW_Ucs2 *url_buff, NW_Ucs2 **ret_string);
TBrowserStatusCode NW_Url_GetHost(const NW_Ucs2 *url_buff, NW_Ucs2 **ret_string);
TBrowserStatusCode NW_Url_GetQuery(const NW_Ucs2 *url_buff, NW_Ucs2 **ret_string);
TBrowserStatusCode NW_Url_GetPath(const NW_Ucs2 *url_buff, NW_Ucs2 **ret_string);
TBrowserStatusCode NW_Url_GetPathNoParam(const NW_Ucs2 *url_buff, NW_Ucs2 **ret_string);

/* Function to create a new url based on applying a relative url to a base url. */
TBrowserStatusCode NW_Url_RelToAbs( const NW_Ucs2 *base, const NW_Ucs2 *relative, NW_Ucs2 **ret_string);

/* Function to create a new relative url based on applying a absolute url to a base url. */
TBrowserStatusCode NW_Url_AbsToRel(const NW_Ucs2 *base, const NW_Ucs2 *url, NW_Ucs2 **ret_string);
/*  This function will encode a string into x-www-form-urlencoded format.*/
NW_Bool NW_Url_EscapeString( const NW_Ucs2 *src, NW_Ucs2 *dest );

NW_Uint32   NW_Url_EscapeStringLen( const NW_Ucs2 *src );

/*  This function will un-encode a string from x-www-form-urlencoded format.*/
NW_Bool NW_Url_UnEscapeString( const NW_Ucs2 *src, NW_Ucs2 *dest );

/* This function creates a url in which 16-bit chars are converted to UTF-8, then URI chars */
TBrowserStatusCode NW_Url_ConvertUnicode( const NW_Ucs2* source, NW_Ucs2** dest );

NW_Uint32  NW_Url_UnEscapeStringLen( const NW_Ucs2 *src );

TBrowserStatusCode NW_Url_AccessOK(const NW_Ucs2 *domain, const NW_Ucs2 *path, const NW_Ucs2 *url, NW_Int32 *pNumLabelDiff );

TBrowserStatusCode NW_Url_IsRelative(const NW_Ucs2 *url, NW_Bool * isRelative);
TBrowserStatusCode NW_Url_GetResolvedPath(const NW_Ucs2 *pBasePath,
                                   const NW_Ucs2 *pRelPath,
                                   NW_Ucs2 **path);

TBrowserStatusCode NW_Url_AddPostfields(const NW_Ucs2 *url, const NW_Ucs2 *postfields, NW_Ucs2 **fullUrl);

TBrowserStatusCode NW_Url_BuildUrl(const NW_Ucs2 *src,
                            const NW_Url_Schema_t scheme,
                            NW_Ucs2 **url);


NW_Ucs2 *NW_Url_Fragment(NW_Ucs2 *url);

TBrowserStatusCode NW_Url_BuildAbsUrl(const NW_Ucs2 *url, 
                               const NW_Ucs2 *card_name,
                               NW_Ucs2 **ret_string);


/* Function escapes the unwise and space character from the url */
NW_Bool NW_Url_EscapeUnwise( const NW_Ucs2 *src, NW_Ucs2 *dest);

/* Function gives the length of url for which unwise and space characters are to be escaped */
NW_Uint32   NW_Url_EscapeUnwiseLen( const NW_Ucs2 *src );

/* Function escapes unwise and space characters in file url. */
NW_Ucs2* NW_Url_ConvertFileToEscapeURL(const NW_Ucs2 *src);

/* Function unescapes the file url */
NW_Ucs2* NW_Url_ConvertFileFromEscapeURL(const NW_Ucs2 *src);

/* Function returns the scheme type. It does not check if the url is valid or not */
TBrowserStatusCode NW_Url_GetSchemeNoValidation(const NW_Ucs2* url, NW_Url_Schema_t* ret_scheme);

TBrowserStatusCode NW_Url_IsBrowserScheme(const NW_Ucs2 *pUri, NW_Bool *isBrowserScheme);

/* Function remove characters from the url */
void NW_Url_RemoveChars (const NW_Ucs2 *src, NW_Ucs2 *dest, const NW_Ucs2 *chs);

/* Function gives the length of url for which characters are to be removed */
NW_Uint32 NW_Url_RemoveCharsLen( const NW_Ucs2 *src, const NW_Ucs2 *chs );

/* Function returns a string in which every byte from source is %HH escaped. */ 
TBrowserStatusCode NW_Url_HHEscape( const char* aSource, NW_Ucs2** aDest );

/* Function returns the Url file extension */
NW_Ucs2* NW_Url_GetUrlFileExtension(const NW_Ucs2 *src);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NWX_URL_UTILS_H */
