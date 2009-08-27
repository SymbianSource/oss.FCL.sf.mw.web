/*
* Copyright (c) 1998-2004 Nokia Corporation and/or its subsidiary(-ies).
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
**-------------------------------------------------------------------------
**  Component Generic Include
**-------------------------------------------------------------------------
*/
#include <apgcli.h>
#include <apmstd.h>
#include <f32file.h>

#include "MVCShell.h"
#include "nw_object_exceptions.h"
#include "nwx_mem.h"
#include "nwx_string.h"
#include "nwx_url_utils.h"
#include "nw_nvpair.h"
#include "nw_wae_reader.h"
#include "nw_string_string.h"
#include "nwx_http_defs.h"
#include "nwx_settings.h"
#include "nwx_statuscodeconvert.h"

/*
**-------------------------------------------------------------------------
**  Module Specific Includes
**-------------------------------------------------------------------------
*/
#include "nw_loadreq.h"
#include "urlloader_urlloaderint.h"
#include "BrsrStatusCodes.h"
#include "nwx_http_defs.h"

/*
**-------------------------------------------------------------------------
**  Constants
**-------------------------------------------------------------------------
*/

//TODO STREAMING: Need to track down an API for getting these.  Initial inquiry
//didn't find one and found other apps are hard-coding these as well.  These,
//however, will not be needed when we switch to the streaming implementation.
static const NW_Ucs2 KCDrivePrefix[] = {'c', ':', '\\', '\0'};
static const NW_Ucs2 KEDrivePrefix[] = {'e', ':', '\\', '\0'};

// The boundary string generated will be done using random numbers and
// converting them to ASCII hex strings. The length should be fairly long so as
// to reduce the probability of the boundary matching any data being sent. As
// of this writing, Microsoft I.E. uses a length of 41, of which 14 chars are
// random ASCII hex chars and the rest are all dashes. In our case, all 41
// chars will be random providing a very low probability of a match. RFC2046
// states that the max length is 70.
static const int KBoundaryLen = 41;

static const NW_Uint8 KDefaultContentType[] =
{'a', 'p', 'p', 'l', 'i', 'c', 'a', 't', 'i', 'o', 'n', '/', 'o', 'c', 't', 'e', 't', '-', 's', 't', 'r', 'e', 'a', 'm'};

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

#define NW_URLENCODED_TYPE_STR (const char*)"application/x-www-form-urlencoded"
#define NW_URLENCODED_CHARSET_UTF8_TYPE_STR (const char*)"application/x-www-form-urlencoded; charset=utf-8"
#define NW_MULTIPART_TYPE_STR  (const char*)"multipart/form-data"

// The HTTP Post header is created elsewhere.  It inserts enough initial CRLFs
// already in preparation for the body; thus, the first boundary string needs
// one fewer CRLF. Inserting one too many CRLF will cause some sites to not
// work on multipart post (e.g., www.gifworks.com). Therefore, there are two
// boundary start strings, one with CRLF and one without. Note, the CRLF does
// belong in the front of and as part of the boundary string.  See RFC 1521:
// "...initial CRLF is considered to be attached to encpsulation boundary
// rather than part of the preceding part."
#define NW_BOUNDARY_FIRST_START_STR (const char*)"--"
#define NW_BOUNDARY_START_STR (const char*)"\r\n--"
#define NW_BOUNDARY_END_STR (const char*)"--\r\n"
#define NW_CONTENTTYPE_TEXTPLAIN_CHARSET_STR (const char*)"\r\nContent-Type: text/plain; charset="
#define NW_CONTENTTYPE_STR (const char*)"\r\nContent-Type: "
#define NW_CONTENTDISPOSITION_STR (const char*)"\r\nContent-Disposition: form-data; name="
#define NW_FILENAME_STR (const char*)" filename="
#define NW_HEADEREND_STR (const char*)"\r\n\r\n"


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
**  Internal Prototypes
**-------------------------------------------------------------------------
*/
TBrowserStatusCode Transcode( NW_Byte *p, const NW_Ucs2 *s, 
    NW_Http_CharSet_t http_charset, NW_Uint32 *byteLen );
NW_Uint32 TranscodedLength( const NW_Ucs2 *s,
    NW_Http_CharSet_t http_charset);
NW_Byte* TranscodeTo10646_ucs2( const NW_Ucs2 *aSrc, NW_Byte *aDst );

static TBrowserStatusCode getEscapedPostdataL(NW_NVPair_t *postfields,
                                      NW_Ucs2 ** escapedPostdata,
                                      NW_HED_AppServices_t* appServices,
                                      void * appCtx);
/*
**-------------------------------------------------------------------------
**  Internal Functions
**-------------------------------------------------------------------------
*/

//-------------------------------------------------------------------------
// getConvertedStringL():
//    aString:          A NW_Ucs2* that needs url-escaping and/or foreign charset conversion.
//    aConvertedString: Gets filled in with a (NW_Ucs2*) pointing to the result of those operations.
//                      *aConvertedString is NULL-terminated.
//    aConvertedLen:    Set to the length of aConvertedString, not including the NULL-terminator
//
static TBrowserStatusCode getConvertedStringL(const NW_Ucs2* aString,
                                               NW_HED_AppServices_t* appServices,
                                               void * appCtx,
                                               NW_Ucs2** convertedString,
                                               NW_Uint32* convertedLen)
{
NW_Ucs2*  escapedString = NULL;
NW_Uint32 escapedLen = 0;
NW_Uint32 originalStringLen = NW_Str_Strlen(aString);
NW_Uint32 copyPos = 0;
NW_Uint32 convertPos = 0;
CBufFlat* buffer = CBufFlat::NewL(20);  // arbitrary granularity.

*convertedString = NULL;
*convertedLen = 0;

NW_TRY( status )
    {
    escapedLen = (3 * originalStringLen + 1);
    escapedString = NW_Str_New(escapedLen);
    NW_THROW_OOM_ON_NULL(escapedString, status);
    
    (void)NW_Mem_memset(escapedString, 0, escapedLen * sizeof(NW_Ucs2));
    
    // protect the URL from thinking field-data is a delimiter character.
    NW_Url_EscapeString(aString, escapedString);
    
    /* convert the value to foreign charset if necessary */
    if (appServices && appCtx)
        {
        NW_String_t text;
        NW_Uint32 origEscapedLen = NW_Str_Strlen(escapedString);
        
        NW_Uint32 i = 0;
        while (i < origEscapedLen)
            {
            // Each interation copies 1 ascii sub-string and/or 
            // converts 1 non-ascii sub string.
            
            // Copy ascii chars
            copyPos = i;
            while (i < origEscapedLen && escapedString[i] <= 127)
                {
                i++;
                }

            if (i != copyPos)
                {
                buffer->InsertL(buffer->Size(), escapedString + copyPos, (i - copyPos) * sizeof(NW_Ucs2));
                }
            
            convertPos = i;
            while (i < origEscapedLen && escapedString[i] > 127)
                {
                // gather non-ascii chars for conversion.
                i++;
                }
            
            if (i != convertPos)
                {
                // Need to convert foreign char string.
                NW_Uint32 foreignLen = i - convertPos ;
                NW_Ucs2*  foreignString = NW_Str_New( foreignLen);
                NW_Ucs2*  transmitString = NULL;

                NW_Mem_memcpy( foreignString, escapedString + convertPos, foreignLen * sizeof(NW_Ucs2));
                foreignString[ foreignLen ] = 0;
                
                // convert the ucs2 into byte* from charset conversion process*/
                status = StatusCodeConvert(NW_String_ucs2CharToString (&text, foreignString, HTTP_iso_10646_ucs_2));
                NW_Mem_Free(foreignString);
                NW_THROW_ON_ERROR(status);
                
                // convert to the foreign charset */
                status = appServices->characterConversionAPI.convertToForeignCharSet(appCtx, &text);
                NW_THROW_ON_ERROR(status);

                // TODO: Apply Utf8 encoding here if appropriate.

                // Apply %HH encoding to the foreign charset string.
                status = NW_Url_HHEscape((const char*)text.storage, &transmitString);
                NW_String_deleteStorage(&text);
                NW_THROW_ON_ERROR(status);

                buffer->InsertL(buffer->Size(), transmitString, NW_Str_Strsize(transmitString) - sizeof(NW_Ucs2));
                NW_Mem_Free(transmitString);
                }
            }
        
        buffer->Compress();
        *convertedLen = buffer->Size();
        *convertedString = NW_Str_New( *convertedLen );
        NW_THROW_OOM_ON_NULL( *convertedString, status );
        
        NW_Mem_memcpy( *convertedString, buffer->Ptr(0).Ptr(), *convertedLen );        
        }
    else 
        {
        NW_Uint32 charCount = NW_Str_Strlen(escapedString);
        *convertedLen =  charCount * sizeof(NW_Ucs2);
        *convertedString = NW_Str_New(charCount);
        NW_THROW_OOM_ON_NULL( *convertedString, status );        
        NW_Str_Strcpy( *convertedString, escapedString);
        }    
    }

NW_CATCH( status )
    {
    }

NW_FINALLY
    {
    buffer->Reset();
    delete buffer;
    NW_Str_Delete(escapedString);
    return status;
    }

NW_END_TRY
}

//TODO CHARSET: These character-handling functions should be in a common
//character translation module.  Currently, this type of functionality is
//spread throughout the code rather than centralized where it can be more
//easily maintained and controlled.

// -----------------------------------------------------------------------------
// GetMaxTranscodedByteLength
// Gets the max number of bytes required to hold the transcoded version of the
// specified source string. This is not precise. It is an upper bounds. The
// actual transcoded string length may end up being less than this max length.
// @param aSrc Pointer to source string, for which the max transcoded length is
//  determined. The source string must be HTTP_iso_10646_ucs_2.
// @param aHttpCharSet Char set to use for transcoding.
// @return Max number of bytes in the transcoded string.
// -----------------------------------------------------------------------------
//
NW_Uint32 GetMaxTranscodedByteLength( const NW_Ucs2 *aSrc,
    NW_Http_CharSet_t aHttpCharSet )
    {
    NW_Uint32 len = 0;

    // Return 0 length for NULL pointer or empty string.
    if ( ( aSrc == NULL ) || ( aSrc[0] == 0 ) )
        {
        return len;
        }

    // Get the length (in characters) of the UCS2 source string.
    len = NW_Str_Strlen( aSrc );

    // For performance reasons, the exact byte length is returned for a few
    // simple and quick cases. For all the rest, an upper bounds is calculated
    // as being 3 byte-sized chars per every UCS2 char.
    switch ( aHttpCharSet )
        {
        case HTTP_us_ascii:         // 1 byte per char; length already set
        case HTTP_iso_8859_1:
            break;
        case HTTP_iso_10646_ucs_2:  // 2 bytes per char
            len *= sizeof(NW_Ucs2);
            break;
        default:                    // upper bounds is 3 byte-sized chars per char
            len *= 3;
            break;
        }
    return len;
    }

// -----------------------------------------------------------------------------
// TranscodeTo10646_ucs2
// Transcodes from a UCS2 string to a UCS2 byte stream for HTTP posts, which
// results in changing the two-byte chars from little endian to big endian.
// @param aSrc Pointer to source string, which is to be transcoded.
// @param aDst Pointer to destination for the transcoded results.
// @return Pointer to the next available byte within the destination buffer.
// -----------------------------------------------------------------------------
//
NW_Byte* TranscodeTo10646_ucs2( const NW_Ucs2* aSrc, NW_Byte* aDst )
{
    while ( *aSrc )
        {
        aDst += NW_Reader_WriteUcs2CharBuff( *aSrc, aDst);
        aSrc++;
        }
    return aDst;
}

// -----------------------------------------------------------------------------
// Transcode
// Translates string from NW_Ucs2 to specified charset (does not append a
// null-terminator).
// @param aDst Pointer to destination into which transcoded string is returned.
//  The size of the destination buffer should be large enough to contain the
//  transcoded string and as such the buffer needs to be allocated large enough
//  when it is created.
// @param aSrc Pointer to source string, which is to be transcoded into the
//  destination. The source string must be HTTP_iso_10646_ucs_2.
// @param aHttpCharSet Pointer to charset name (e.g. "utf-8").
// @param aBufLen Length of destination buffer (amount of space available for
//  transcoded string).
// @param aByteLen Pointer at which the byte length of the transcoded string is
//  returned.
// @return  KBrsrSuccess: if success.
//          KBrsrOutOfMemory: if out of memory.
//          KBrsrFailure: any other failure.
// -----------------------------------------------------------------------------
//
TBrowserStatusCode Transcode( NW_Byte *aDst, const NW_Ucs2 *aSrc, 
    NW_Http_CharSet_t aHttpCharSet, NW_Uint32 aBufLen, NW_Uint32* aByteLen )
    {
    NW_String_t text;
    NW_Uint32 srcByteLen;

    // Parameter assertion block.
    NW_ASSERT( aDst );
    NW_ASSERT( aByteLen );

    text.storage = NULL;

    // Initialize returned length to zero for now, in case an error occurs.
    *aByteLen = 0;

    NW_TRY( status )
        {

        // If there is no source, just return.
        if ( aSrc == NULL )
            {
            NW_THROW( KBrsrSuccess );
            }

        srcByteLen = NW_Str_Strlen( aSrc ) * sizeof( NW_Ucs2 );

        // Special case UCS2 for performance reasons - if both source and
        // destination are UCS2 then no intermediate buffer needed.
        if ( aHttpCharSet == HTTP_iso_10646_ucs_2 )
            {
            // Verify that there is sufficient space for the text.
            if ( srcByteLen > aBufLen )
                {
                NW_THROW( KBrsrFailure );
                }
            aDst = TranscodeTo10646_ucs2( aSrc, aDst );
            // Return the length of transcoded text in destination.
            *aByteLen = srcByteLen;
            NW_THROW( KBrsrSuccess );
            }

        // Allocate an intermediate buffer. All other char sets go through the 
        // transcoding methods which unfortunately require an intermediate
        // buffer.
        text.storage = (NW_Byte*)NW_Mem_Malloc( srcByteLen );
        NW_THROW_OOM_ON_NULL( text.storage, status );

        // Copy the source into the intermediate buffer.
        NW_Mem_memcpy( text.storage, aSrc, srcByteLen );
        text.length = srcByteLen;

        // Tanscode. Note, this assumes transcoding to original character set.
//TODO CHARSET: Make the API more flexible to take the charset as a parameter.
        status = CShell::ConvertToForeignCharSet( &text );
        _NW_THROW_ON_ERROR( status );
        // Verify that there is sufficient space for the text.
        if ( text.length > aBufLen )
            {
            NW_THROW( KBrsrFailure );
        }
        // Copy the transcoded text into the destination buffer. Note, the
        // transcoded length always includes 2 null bytes.
//TODO CHARSET: Fix ConvertToForeignCharSet and any code that uses it to not
//use 2 extra null bytes. Not as easy as it sounds. That will lead into a
//tangled web.
	    NW_Mem_memcpy( aDst, text.storage, text.length - 2 );
        // Return the length of transcoded text in destination.
        *aByteLen = text.length - 2;
        }
    // status = KBrsrSuccess; // set by NW_CATCH
    NW_CATCH( status )
        {
        }
    NW_FINALLY
        {
        NW_String_deleteStorage( &text );
        return status;
        }
    NW_END_TRY
    }

/************************************************************************
 
  Function: GetHttpCharsetAsAsciiText

  Description:  Returns the string of the character set.

  Parameters:   httpCharset - in: HTTP charset.
    
  Return Value: Pointer to the charset string.
                  
**************************************************************************/
static const char* GetHttpCharsetAsAsciiText(NW_Http_CharSet_t httpCharset)
{
    switch (httpCharset)
    {
        case HTTP_utf_8:
            return NW_Utf8_CharsetStr;
        case HTTP_us_ascii:
            return NW_UsAscii_CharsetStr;
        case HTTP_iso_10646_ucs_2:
            return NW_Iso10646Ucs2_CharsetStr;
        case HTTP_windows_1255:
            return NW_Windows1255_CharsetStr;
        case HTTP_windows_1256:
            return NW_Windows1256_CharsetStr;
        case HTTP_windows_1250:
            return NW_Windows1250_CharsetStr;
        case HTTP_windows_1251:
            return NW_Windows1251_CharsetStr;
        case HTTP_windows_1253:
            return NW_Windows1253_CharsetStr;
        case HTTP_windows_1254:
            return NW_Windows1254_CharsetStr;
        case HTTP_windows_1257:
            return NW_Windows1257_CharsetStr;
        case HTTP_big5:
            return NW_Big5_CharsetStr;
        case HTTP_gb2312:
            return NW_Gb2312_CharsetStr;
        case HTTP_iso_8859_1:
            return NW_Iso88591_CharsetStr;
        case HTTP_iso_8859_2:
            return NW_Iso88592_CharsetStr;
        case HTTP_iso_8859_3:
            return NW_Iso88593_CharsetStr;
        case HTTP_iso_8859_4:
            return NW_Iso88594_CharsetStr;
        case HTTP_iso_8859_5:
            return NW_Iso88595_CharsetStr;
        case HTTP_iso_8859_6:
            return NW_Iso88596_CharsetStr;
        case HTTP_iso_8859_7:
            return NW_Iso88597_CharsetStr;
        case HTTP_iso_8859_8:
            return NW_Iso88598_CharsetStr;
        case HTTP_iso_8859_9:
            return NW_Iso88599_CharsetStr;
        case HTTP_tis_620:  // not flagged because we will not get here unless tis_620 is enabled
            return NW_Tis_620_CharsetStr;
        case HTTP_shift_JIS:
            return NW_ShiftJis_CharsetStr;
        case HTTP_jis_x0201_1997:
            return NW_JisX0201_1997_CharsetStr;
        case HTTP_jis_x0208_1997:
            return NW_JisX0208_1997_CharsetStr;
        case HTTP_euc_jp:
            return NW_EucJp_CharsetStr;
        case HTTP_iso_2022_jp:
            return NW_Iso2022Jp_CharsetStr;
        case HTTP_windows_874:
            return NW_Windows874_CharsetStr;
        case HTTP_Koi8_r:
            return NW_Koi8r_CharsetStr;
        case HTTP_Koi8_u:
            return NW_Koi8u_CharsetStr;
    }
    NW_ASSERT(NW_FALSE);
    return NULL; /* Make compiler happy */
}

// -----------------------------------------------------------------------------
// getPostDataCharset
// Gets the charset to be used for posting data. This is currently the encoding
// of the original document.
// @return Charset to be used or HTTP_iso_8859_1 if no known value.
//  HTTP_iso_8859_1 was chosen as the "default" since it is a very common
//  charset.
// -----------------------------------------------------------------------------
//
static NW_Http_CharSet_t getPostdataCharset()
    {
    NW_Http_CharSet_t charset;

    charset = (NW_Http_CharSet_t)NW_Settings_GetOriginalEncoding();
    return ((charset == HTTP_unknown) || (charset == 0))
        ? (NW_Http_CharSet_t)HTTP_iso_8859_1 : charset;
    }



/************************************************************************
 
  Function: getEscapedPostdata 

  Description:  Give a single url-encoded string containing all
                the postfield data, in the form of

                "name1=value1&name2=value2&...&namen=valuen"

  Parameters:   *postfields        - pointer to the postfield data.
                **escaped_postdata - pointer to the encoded string.
    
  Return Value: KBrsrSuccess
                KBrsrOutOfMemory
                  
**************************************************************************/
//??? RFD: This section needs to be corrected along with the rest of the URL-encoding fixes underway.
static TBrowserStatusCode getEscapedPostdata(NW_NVPair_t *postfields,
                                      NW_Ucs2 ** escapedPostdata,
                                      NW_HED_AppServices_t* appServices,
                                      void * appCtx)
{
  TBrowserStatusCode status = KBrsrFailure;

  TInt err;
  TRAP(err, status = getEscapedPostdataL(postfields, escapedPostdata, appServices, appCtx));

  return status;
}
                                     
static TBrowserStatusCode getEscapedPostdataL(NW_NVPair_t *postfields,
                                      NW_Ucs2 ** escapedPostdata,
                                      NW_HED_AppServices_t* appServices,
                                      void * appCtx)
{

  NW_Ucs2   *name;
  NW_Ucs2   *value;
  NW_Ucs2*  convertedName = NULL;
  NW_Uint32 convertedNameLen = 0;
  NW_Ucs2*  convertedValue = NULL;
  NW_Uint32 convertedValueLen = 0;
  NW_Uint32 parameterLen = 0;
  
  const char* ie  = "ie";
  const char* euc = "EUC-KR"; 
  NW_TRY( status )
      {


		NW_ASSERT(escapedPostdata != NULL);

		*escapedPostdata = NULL;

		if ( NW_NVPair_IsEmpty(postfields) )
  	        {
            // No postdata fields.  Return an empty string for
		    // escaped_postdata.
  	        *escapedPostdata = NW_Str_NewcpyConst( "" );
		    return KBrsrSuccess;
		    }

        // set status to KBrsrSuccess because if the were only ie=EUC-KR as 
        // postdata we need to return KBrsrSuccess 
        status = KBrsrSuccess;
		CBufFlat* buffer = CBufFlat::NewL(20);  // arbitrary granularity.
		NW_Bool   needSep = NW_FALSE;

		(void) NW_NVPair_ResetIter( postfields );
		while ( KBrsrSuccess == (status = NW_NVPair_GetNext( postfields, &name, &value ))) 
          {
          if ( 0 == NW_Str_StrnicmpConst( name, ie, NW_Str_Strlen( name ) ) &&
               0 == NW_Str_StrnicmpConst( value, euc, NW_Str_Strlen( value ) ) )
              {
              continue;
              } 

          if ( needSep ) 
              {
			  // append "&" delimiter, as Ucs2
              buffer->InsertL(buffer->Size(), WAE_URL_POSTFIELD_SEP, 2);
              parameterLen++;
              }

          // Each name and value pair needs to be escaped and/or converted to foreign charset.

          status = getConvertedStringL(name, appServices, appCtx, &convertedName, &convertedNameLen);
          _NW_THROW_ON_ERROR( status );
          buffer->InsertL(buffer->Size(), convertedName, convertedNameLen);
          NW_Mem_Free(convertedName);
          convertedName = NULL;

          buffer->InsertL(buffer->Size(), "=", 2); // seperator is UCS2
          
          status = getConvertedStringL(value, appServices, appCtx, &convertedValue, &convertedValueLen);
          _NW_THROW_ON_ERROR( status );
          buffer->InsertL(buffer->Size(), convertedValue, convertedValueLen);
          NW_Mem_Free(convertedValue);
          convertedValue = NULL;

          needSep = NW_TRUE;
          }

      parameterLen = buffer->Size();
      *escapedPostdata = (NW_Ucs2*) NW_Mem_Malloc(parameterLen + 2);
      NW_Mem_memcpy( *escapedPostdata, buffer->Ptr(0).Ptr(), parameterLen);
      (void)NW_Mem_memset( ((NW_Byte*)(*escapedPostdata)) + parameterLen, 0, 2 );
      buffer->Reset();
      delete buffer;
            
      }

  NW_CATCH( status )
      {
      NW_Mem_Free(convertedName);
      NW_Mem_Free(convertedValue);

      }

  NW_FINALLY
      {
      return status;      
      }

  NW_END_TRY
}


// -----------------------------------------------------------------------------
// CreateBoundary
// Creates the boundary string that separates each part in the multipart post.
// The boundary string generated is fairly long and has some degree of
// randomness (by using a random number generator). This is to reduce the
// chance that the string generated could actually match any of the data being
// posted. Although highly unlikely, if it does match then the post would fail.
// If the user attempts to re-transmit, a new string will be generated and the
// post is highly likely to succeed.
// @param aBoundary Ptr to address into which the allocated boundary string
//      is stored.  The caller takes ownership of the allocated memory.
// @return KBrsrSuccess or KBrsrOutOfMemory.
// -----------------------------------------------------------------------------
//
static TBrowserStatusCode CreateBoundary( NW_Uint8** aBoundary )
    {
    // Allocate memory for the boundary string.
    NW_Uint8* boundary = (NW_Uint8*)NW_Mem_Malloc( KBoundaryLen + 1 );
    *aBoundary = boundary;
    if ( !boundary )
        {
        return KBrsrOutOfMemory;
        }
    // Fill the boundary string with random numbers, converting the numbers to
    // hex chars and inserting each char into the string.
    NW_Uint8 n;
    int random = 0;
    for ( int i = 0; i < KBoundaryLen; i++ )
        {
        if ( !random )
            {
            random = rand();
            }
        // Extract lower nibble.
        n = (NW_Uint8)(random & 0x0F);
        // Convert to ASCII hex.
        n = (NW_Uint8)((n < 10) ? (n + '0') : ((n - 10) + 'A'));
        // Shift right by a nibble.
        random = (random >> 4) & 0x0FFFFFFF;
        // Insert char.
        *boundary++ = n;
        }
    // Null terminate.
    *boundary = 0;

    return KBrsrSuccess;
    }

// -----------------------------------------------------------------------------
// GetMimeType
// Gets the mime-type string for a given file (E.g., "image/gif", etc.).
// @param aFileName Ptr to name of file for which to get the mime-type.
// @param aMimeTypeString Ptr to desination into which the newly allocated
//      mime-type string is stored.  The caller takes ownership of the allocated
//      memory.
// @return  KBrsrSuccess: if success.
//          KBrsrOutOfMemory: if out of memory.
//          KBrsrFailure: any other failure.
// -----------------------------------------------------------------------------
//
static TBrowserStatusCode GetMimeType(
    const NW_Ucs2* aFileName, TUint8** aMimeTypeString )
    {
    TInt err;
    TUid uid;
    TDataType dataType;
    RApaLsSession apaSession;
    TBool apaSessionConnected = EFalse;

    // Parameter assertion block.
    NW_ASSERT( aFileName != NULL );
    NW_ASSERT( aMimeTypeString != NULL );

    *aMimeTypeString = NULL;

    NW_TRY( status )
        {
        err = apaSession.Connect();
        if ( err )
            {
            NW_THROW( (err == KErrNoMemory) ? KBrsrOutOfMemory : KBrsrFailure );
            }
        apaSessionConnected = ETrue;
            
        // Ask the application architecture to find the file type.
        TPtrC fileNamePtr( (const TUint16*)aFileName,
            (TInt)NW_Str_Strlen( aFileName ) );

        err = apaSession.AppForDocument( fileNamePtr, uid, dataType );
        if ( err )
            {
            NW_THROW( (err == KErrNoMemory) ? KBrsrOutOfMemory : KBrsrFailure );
            }

        TPtrC8 dataTypePtr = dataType.Des8();
        TInt len = dataTypePtr.Length();

        // If no mime-type yet, then use the default.
        if ( !len )
            {
            len = sizeof( KDefaultContentType );
            dataTypePtr.Set( KDefaultContentType, len );
            }

        // Allocate a buffer and return the mime-type in it.
        len++; // +1 for NULL terminator
        *aMimeTypeString = new TUint8 [len]; 
        if ( *aMimeTypeString == NULL )
            {
            NW_THROW( KBrsrOutOfMemory );
            }
        TPtr8 mimeTypeStringPtr( *aMimeTypeString, len );
        mimeTypeStringPtr.Copy( dataTypePtr );
        mimeTypeStringPtr.ZeroTerminate();
        }
    // status = KBrsrSuccess; // set by NW_CATCH
    NW_CATCH( status )
        {
        }
    NW_FINALLY
        {
        if ( apaSessionConnected )
            {
            apaSession.Close();
            }
        return status;
        }
    NW_END_TRY
    }

// -----------------------------------------------------------------------------
// GetMimeTypeSize
// Gets the number of characters in the mime-type string.
// @param aFileName Ptr to name of file for which to get the mime-type size.
// @param aSize Ptr to destination into which to return the size.
// @return  KBrsrSuccess: if success.
//          KBrsrOutOfMemory: if out of memory.
//          KBrsrFailure: any other failure.
// -----------------------------------------------------------------------------
//
static TBrowserStatusCode GetMimeTypeSize(
    const NW_Ucs2* aFileName, NW_Uint32* aSize )
    {
    TBrowserStatusCode status;
    TUint8* mimeType;

    // Parameter assertion block.
    NW_ASSERT( aFileName != NULL );
    NW_ASSERT( aSize != NULL );

    *aSize = 0; // default
    status = GetMimeType( aFileName, &mimeType );
    if ( status == KBrsrSuccess )
        {
        *aSize = NW_Asc_strlen( (const char*)mimeType );
        delete mimeType;
        }
    return status;
    }

// -----------------------------------------------------------------------------
// GetMultipartEntrySize
// Gets the upper bounds on the size of the multipart entry.  The size returned
//  may not be the exact size but it will be >= exact size.
//
//  Each multipart entry is in one of two formats. One is for name/values and 
//  the other is for files:
//
//  name/value:
//    <cr><lf>--boundary<cr><lf>
//    Content-Type: text/plain; charset=charsetString<cr><lf>
//    Content-Disposition: form-data; name="name"<cr><lf>
//    Content-Length: contentLength<cr><lf><cr><lf>
//    {value here without the curly braces}
//
//  files:
//    <cr><lf>--boundary<cr><lf>
//    Content-Disposition: form-data; name="name"; filename="d:\dir\filename.ext"<cr><lf><cr><lf>
//    {contents of file without the curly braces}    
//
// @param aName
// @param aValue
// @param aHttpCharSet
// @param aBoundary
// @param aFirst
// @param aSize
// @return
// -----------------------------------------------------------------------------
//
static TBrowserStatusCode GetMultipartEntrySize(
    const NW_Ucs2* aName, 
    const NW_Ucs2* aValue, 
    NW_Http_CharSet_t aHttpCharSet,
    NW_Uint8* aBoundary,
    TBool aFirst,
    NW_Uint32* aSize )
    {
    TBrowserStatusCode ret = KBrsrSuccess;
    NW_Uint32 size = 0;

    // Parameter assertion block.
    NW_ASSERT( aName != NULL );
    NW_ASSERT( aValue != NULL );
    NW_ASSERT( aBoundary != NULL );
    NW_ASSERT( aSize != NULL );

    // Note, content length is not included in mime part header.  Some sites
    // do not work properly if included.  Microsoft I.E. does not include one.

    // Get size of the boundary, common to all types of mime parts.
    size = ( aFirst ) ? NW_Asc_strlen( NW_BOUNDARY_FIRST_START_STR ) :
        NW_Asc_strlen( NW_BOUNDARY_START_STR );
    size += NW_Asc_strlen( (const char*)aBoundary );

    // Add in the rest of the size; depends on type of mime part.
    if ( NW_LoadReq_IsUploadFile( aValue ) )
        {
        TInt err = KErrNone;
        RFs rfs;
        TBool rfsConnected = EFalse;
        RFile file;
        TBool fileOpened = EFalse;
        TPtrC fileNamePtr( (TUint16*)aValue );
        TInt fileSize;

        NW_TRY( status )
            {
//TODO STREAMING: This is very in-efficient to connect every time and then to
//reconnect again later to read the file. This will be addressed in next phase
//with streaming implementation.
            err = rfs.Connect();
            if ( err )
                {
                NW_THROW_STATUS( status, ((err == KErrNoMemory) ? KBrsrOutOfMemory : KBrsrFailure ) );
                }
            rfsConnected = ETrue;

            err = file.Open( rfs, fileNamePtr, EFileRead | EFileShareExclusive | EFileStream );
            if ( err )
                {
//TODO STREAMING: Also look for missing file error returned. This too will
//change with streaming implementation.
                NW_THROW_STATUS( status, ((err == KErrNoMemory) ? KBrsrOutOfMemory : KBrsrFailure ) );
                }
            fileOpened = ETrue;

            err = file.Size( fileSize );
            if ( err )
                {
//TODO STREAMING: What other errors should be checked? This too will
//change with streaming implementation.
                NW_THROW_STATUS( status, ((err == KErrNoMemory) ? KBrsrOutOfMemory : KBrsrFailure ) );
                }

            NW_Uint32 mimeTypeSize = 0;
            status = GetMimeTypeSize( aValue, &mimeTypeSize );
            _NW_THROW_ON_ERROR( status );

            NW_Uint32 mimeContentTypeSize = (mimeTypeSize)
                ? mimeTypeSize + NW_Asc_strlen( NW_CONTENTTYPE_STR ) : 0;

            size += NW_Asc_strlen( NW_CONTENTDISPOSITION_STR )
                + NW_Str_Strlen( aName ) + 3 //+3 for quotes around name and a semicolon at end
                + NW_Asc_strlen( NW_FILENAME_STR )
                + NW_Str_Strlen( aValue ) + 2 //+2 for quotes around filename
                + mimeContentTypeSize
                + NW_Asc_strlen( NW_HEADEREND_STR )
                + fileSize;
            }
        // status = KBrsrSuccess; // set by NW_CATCH
        NW_CATCH( status )
            {
            }
        NW_FINALLY
            {
            if ( fileOpened )
                {
                file.Close();
                }
            if ( rfsConnected )
                {
                rfs.Close();
                }
            ret = status;
            }
        NW_END_TRY
        }
    else
        {
        NW_Uint32 transcodedValueLength = GetMaxTranscodedByteLength( aValue, aHttpCharSet );
        size += NW_Asc_strlen( NW_CONTENTTYPE_TEXTPLAIN_CHARSET_STR )
            + NW_Asc_strlen( GetHttpCharsetAsAsciiText( aHttpCharSet ) )
            + NW_Asc_strlen( NW_CONTENTDISPOSITION_STR )
            + NW_Str_Strlen( aName ) + 2/*quotes around name*/
            + NW_Asc_strlen( NW_HEADEREND_STR )
            + transcodedValueLength;
        }

    *aSize = (ret == KBrsrSuccess) ? size : 0;
    return ret;
    }

// -----------------------------------------------------------------------------
// GetMultipartSize
// Gets the upper bounds on the size of the multipart data.  The size returned
//  may not be the exact size but it will be >= exact size.
// @param aPostFields
// @param aHttpCharSet
// @param aBoundary
// @param aSize
// @return
// -----------------------------------------------------------------------------
//
static TBrowserStatusCode GetMultipartSize(
    NW_NVPair_t *aPostFields,
    NW_Http_CharSet_t aHttpCharSet,
    NW_Uint8* aBoundary,
    NW_Uint32* aSize )
    {
    NW_Ucs2 *name;
    NW_Ucs2 *value;
    TBrowserStatusCode status;
    NW_Uint32 size;
    NW_Uint32 partSize;
    TBool first = ETrue;

    // Parameter assertion block.
    NW_ASSERT( aPostFields != NULL );
    NW_ASSERT( aBoundary != NULL );
    NW_ASSERT( aSize != NULL );

    size = 0;
    NW_NVPair_ResetIter( aPostFields );
    while ( (status = NW_NVPair_GetNext( aPostFields, &name, &value )) == KBrsrSuccess )
        {
        status = GetMultipartEntrySize( name, value, aHttpCharSet, aBoundary,
            first, &partSize );
        if ( status != KBrsrSuccess )
            {
            return status;
            }
        size += partSize;
        first = EFalse;
        }

    // Add the size of the ending boundary marker, plus one for null termination
    size += NW_Asc_strlen(NW_BOUNDARY_START_STR) +
    NW_Asc_strlen((const char*)aBoundary) +
    NW_Asc_strlen(NW_BOUNDARY_END_STR) + 1;

    *aSize = size;
    return KBrsrSuccess;
    }

static TBrowserStatusCode StorePart( NW_Uint8* aDataBuffer, NW_Uint32 len,
    NW_Uint8* aBoundary, NW_Ucs2* aName, NW_Ucs2* aValue,
    NW_Http_CharSet_t aHttpCharSet, TBool aFirst,
    NW_Uint8** retPtr )
    {
    TBrowserStatusCode ret = KBrsrSuccess;
    char* curr = (char*)aDataBuffer;
    NW_Ucs2* ptr;
    NW_Uint32 neededLen;
    NW_Uint32 byteLen = 0;

    // Store the boundary, common to all types of mime parts.
    neededLen = NW_Asc_strlen(
        ((aFirst) ? NW_BOUNDARY_FIRST_START_STR : NW_BOUNDARY_START_STR) )
        + NW_Asc_strlen( (char*)aBoundary );
    if ( neededLen >= len )
        {
        return KBrsrFailure;
        }
    NW_Asc_strcpy( curr, 
        ((aFirst) ? NW_BOUNDARY_FIRST_START_STR : NW_BOUNDARY_START_STR) );
    NW_Asc_strcat( curr, (char*)aBoundary );
    curr += neededLen;
    len -= neededLen;

    // Store the rest of the part; depends on type of mime part.
    if ( NW_LoadReq_IsUploadFile( aValue ) )
        {
        TInt err = KErrNone;
        RFs rfs;
        TBool rfsConnected = EFalse;
        RFile file;
        TBool fileOpened = EFalse;
        TPtrC fileNamePtr( (TUint16*)aValue );
        TInt fileSize;
        TUint8* mimeType = NULL;

        NW_TRY( status )
            {
            // Append the content disposition.
            neededLen = NW_Asc_strlen( NW_CONTENTDISPOSITION_STR );
            if ( neededLen >= len )
                {
                return KBrsrFailure;
                }
            NW_Asc_strcpy( curr, NW_CONTENTDISPOSITION_STR );
            curr += neededLen;

            // Append the variable name, within quotes.
            neededLen = NW_Str_Strlen( aName ) + 3; // +3 for two quotes and semicolon
            if ( neededLen >= len )
                {
                return KBrsrFailure;
                }
            *curr++ = '\"';
            ptr = aName;
            while (*ptr != 0)
                {
                *curr++ = (char)*ptr;
                ptr++;
                }
            *curr++ = '\"';
            *curr++ = ';';  // end it with a semicolon
            len -= neededLen;

            // Append the file name tag.
            neededLen = NW_Asc_strlen( NW_FILENAME_STR );
            if ( neededLen >= len )
                {
                return KBrsrFailure;
                }
            NW_Asc_strcpy( curr, NW_FILENAME_STR );
            curr += neededLen;
            len -= neededLen;

            // Append the actual file name, within quotes.
            neededLen = NW_Str_Strlen( aValue ) + 2; // +2 for quotes
            if ( neededLen >= len )
                {
                return KBrsrFailure;
                }
            *curr++ = '\"';
            ptr = aValue;
            while (*ptr != 0)
            {
                *curr++ = (char)*ptr;
                ptr++;
            }
            *curr++ = '\"';
            len -= neededLen;

            // Append the ContentType if the mime-type is known.
            status = GetMimeType( aValue, &mimeType );
            _NW_THROW_ON_ERROR( status );

            if ( mimeType )
                {
                neededLen = NW_Asc_strlen( NW_CONTENTTYPE_STR )
                    + NW_Asc_strlen( (const char*)mimeType );
                if ( neededLen >= len )
                    {
                    return KBrsrFailure;
                    }
                NW_Asc_strcpy( curr, NW_CONTENTTYPE_STR );
                NW_Asc_strcat( curr, (const char*)mimeType );
                curr += neededLen;
                len -= neededLen;
                }

            // Append the header end.
            neededLen = NW_Asc_strlen( NW_HEADEREND_STR );
            if ( neededLen >= len )
                {
                return KBrsrFailure;
                }
            NW_Asc_strcpy( curr, NW_HEADEREND_STR );
            curr += neededLen;
            len -= neededLen;

            // Append the contents of the file.
            err = rfs.Connect();
            if ( err )
                {
                NW_THROW_STATUS( status, ((err == KErrNoMemory) ? KBrsrOutOfMemory : KBrsrFailure ) );
                }
            rfsConnected = ETrue;

            err = file.Open( rfs, fileNamePtr, EFileRead | EFileShareExclusive );
            if ( err )
                {
//TODO STREAMING: Also look for missing file error returned. This too will
//change with streaming implementation.
                NW_THROW_STATUS( status, ((err == KErrNoMemory) ? KBrsrOutOfMemory : KBrsrFailure ) );
                }
            fileOpened = ETrue;

            err = file.Size( fileSize );
            if ( err )
                {
//STREAMING: What other errors should be checked? This too will
//change with streaming implementation.
                NW_THROW_STATUS( status, ((err == KErrNoMemory) ? KBrsrOutOfMemory : KBrsrFailure ) );
                }

            neededLen = (NW_Uint32)fileSize;
            if ( neededLen >= len )
                {
                return KBrsrFailure;
                }

            TPtr8 tmpPtr( (TUint8*)(curr), 0, fileSize );
            err = file.Read( tmpPtr );
            if ( err )
                {
                NW_THROW_STATUS( status, ((err == KErrNoMemory) ? KBrsrOutOfMemory : KBrsrFailure ) );
                }

            curr += neededLen;
            len -= neededLen;
            }
        // status = KBrsrSuccess; // set by NW_CATCH
        NW_CATCH( status )
            {
            }
        NW_FINALLY
            {
            delete mimeType;
            if ( fileOpened )
                {
                file.Close();
                }
            if ( rfsConnected )
                {
                rfs.Close();
                }
            }
        ret = status;
        NW_END_TRY
        }
    else
        {
        char* charSetAsText = (char*)GetHttpCharsetAsAsciiText( aHttpCharSet );

        // Verify the length will not exceed remaining buffer length (not
        // including transcoded data yet).
        neededLen = NW_Asc_strlen( NW_CONTENTTYPE_TEXTPLAIN_CHARSET_STR )
            + NW_Asc_strlen( charSetAsText )
            + NW_Asc_strlen( NW_CONTENTDISPOSITION_STR )
            + NW_Str_Strlen( aName ) + 2 // +2 for quotes
            + NW_Asc_strlen( NW_HEADEREND_STR );
        if ( neededLen >= len )
            {
            return KBrsrFailure;
            }

        // Append content type and disposition text.
        NW_Asc_strcpy( curr, NW_CONTENTTYPE_TEXTPLAIN_CHARSET_STR );
        NW_Asc_strcat( curr, charSetAsText );
        NW_Asc_strcat( curr, NW_CONTENTDISPOSITION_STR );
        curr = curr + NW_Asc_strlen( curr );
        // Append variable name to end of content disposition, within quotes.
        *curr++ = '\"';
        ptr = aName;
        while (*ptr != 0)
        {
            *curr++ = (char)*ptr;
            ptr++;
        }
        *curr++ = '\"';
        // Append the header end.
        NW_Asc_strcpy( curr, NW_HEADEREND_STR );
        curr = curr + NW_Asc_strlen( curr );

        // Adjust available buffer length.
        len -= neededLen;

        // Append the data.
        ret = Transcode( (NW_Uint8*)curr, aValue, aHttpCharSet, len, &byteLen );
        if ( ret != KBrsrSuccess )
            {
            if ( ret != KBrsrOutOfMemory )
                {
                ret = KBrsrFailure;
                }
            }
        else
            {
            curr += byteLen;
            }
        }

    *retPtr = (NW_Uint8*)curr;
    return ret;
    }

/*****************************************************************

  Name: BuildMultipartBody

  Description:
    Allocate and create a multipart/form-data encoded body for the 
    postfields.

  Parameters:
    aBuffer      - out: multipart/form-data encoded postfield body
    aPostFields  - in: postfields to encode.
    aCharset     - in: character set to encode the postfield values.
    aBoundary    - out: boundary value that separates each part; caller
                    must deallocate regardless of status returned!

  Return Value:
     KBrsrSuccess
     KBrsrOutOfMemory
     KBrsrFailure

******************************************************************/
static TBrowserStatusCode BuildMultipartBody(
    NW_Buffer_t** aBuffer,
    NW_NVPair_t* aPostFields,
    const NW_Http_CharSet_t aCharset,
    NW_Uint8** aBoundary)
    {
    NW_Uint32 len;
    NW_Byte* p;
    NW_Byte* newP;
    NW_Ucs2* name;
    NW_Ucs2* value;
    TBrowserStatusCode status;

    NW_ASSERT( aBuffer != NULL );
    NW_ASSERT( aBoundary != NULL );

    *aBoundary = NULL;

    status = CreateBoundary( aBoundary );
    if ( status != KBrsrSuccess )
        {
        return status;
        }

    // Calculate the length of the multipart data. This will be used to
    // allocate a buffer to contain the actual data. Note, the length
    // is an upper bounds and not necessarily the exact length.
    status = GetMultipartSize( aPostFields, aCharset, *aBoundary, &len );
    if ( status != KBrsrSuccess )
        {
        return status;
        }
    NW_ASSERT( len != 0 );

    // Allocate the buffer for the multipart data.
    *aBuffer = NW_Buffer_New( len );
    if ( *aBuffer == NULL)
        {
        return KBrsrOutOfMemory;
        }

    // Insert the multipart content into the buffer. The length was
    // determined above; however, remaining length is tracked to make sure
    // nothing has changed (like the length of the files) since the time it
    // was determined.  Tracking the length also provides a sanity check.
    p = (*aBuffer)->data;
    TBool first = ETrue;
    (void)NW_NVPair_ResetIter( aPostFields );
    while ( (status = NW_NVPair_GetNext( aPostFields, &name, &value )) == KBrsrSuccess )
        {
        status = StorePart( p, len, *aBoundary, name, value, aCharset, first, &newP );
        if ( status != KBrsrSuccess )
            {
            return status;
            }
        len -= (newP - p);
        p = newP;
        first = EFalse;
        }

    // Make sure there is enough space for ending boundary marker and a null.
    NW_Uint32 endingLen = NW_Asc_strlen( NW_BOUNDARY_START_STR )
        + NW_Asc_strlen( (char*)*aBoundary )
        + NW_Asc_strlen( NW_BOUNDARY_END_STR );
    // Add one to make sure there is enough space for the null terminator.
    if ( (endingLen + 1) > len )
        {
        return KBrsrFailure;
        }

    // Add the ending boundary marker and final null terminator.
    NW_Asc_strcpy( (char*)p, NW_BOUNDARY_START_STR );
    NW_Asc_strcat( (char*)p, (char*)*aBoundary );
    NW_Asc_strcat( (char*)p, NW_BOUNDARY_END_STR );

    p += endingLen;

    // Set length in the destination buffer to actual amount of data stored.
    (*aBuffer)->length = p - (*aBuffer)->data;

    return KBrsrSuccess;
    }

/*
**-------------------------------------------------------------------------
**  External Public (Exported) Functions
**-------------------------------------------------------------------------
*/

TBrowserStatusCode NW_LoadReq_Create ( const NW_Ucs2 *url,
                                const NW_Bool postMethodFlag, 
                                const NW_Ucs2 *referer, 
                                const NW_Ucs2 *acceptCharset,
                                NW_NVPair_t *postfields,
                                const NW_Ucs2 *enctype,
                                const NW_Http_CharSet_t docCharset,
                                NW_Http_Header_t **header,
                                NW_Ucs2** resultUrl,
                                NW_Uint8* method,
                                NW_Buffer_t** body,
                                NW_HED_AppServices_t* appServices,
                                void * appCtx)
{
  TBrowserStatusCode status = KBrsrSuccess;
  char*                 content = NULL;
  NW_Http_CharSet_t     httpCharset;
  NW_Ucs2*              escapedPostdata = NULL;
  NW_Ucs2*              convertedPostdata = NULL;
  NW_Uint8*             boundary = NULL;

  NW_REQUIRED_PARAM(acceptCharset);
  NW_REQUIRED_PARAM(docCharset);

  NW_ASSERT(url != NULL);

  /* Start with empty return values. */
  *resultUrl = NULL;
  *method = 0;
  *body = NULL;

  /* validate enctype */
  if ( enctype != NULL ) {
    if ( postMethodFlag ) {
      if ( NW_Str_StricmpConst(enctype, NW_URLENCODED_TYPE_STR)
        && NW_Str_StricmpConst(enctype, NW_MULTIPART_TYPE_STR) )
        return KBrsrWmlbrowserBadContent;
    } else {
      if ( NW_Str_StricmpConst(enctype, NW_URLENCODED_TYPE_STR) )
        return KBrsrWmlbrowserBadContent;
    }
  }

  /* Get charset to use for the postdata. */
  httpCharset = getPostdataCharset();
  if (httpCharset == HTTP_unknown) {
    return KBrsrUnsupportedFormCharset;
  }

  if ( postMethodFlag ) {
    *resultUrl = NW_Str_Newcpy(url);
    if (*resultUrl == NULL) {
      goto OutOfMemory;
    }

    if ( NW_LoadReq_IsMultipart( enctype ) ) {

      /* post method and multipart/form-data body */
      status = BuildMultipartBody(body, postfields, httpCharset, &boundary);
      if (status != KBrsrSuccess) {
        goto OutOfMemory;
      }
      if (*header == NULL) {
        *header = UrlLoader_HeadersNew(NULL, 
            (const unsigned char *)NW_MULTIPART_TYPE_STR, 
            referer, boundary, (*body)->length, NULL);
      }

      if (*header == NULL) {
        goto OutOfMemory;
      }

    } else {    
//??? RFD: This section needs to be corrected along with the rest of the URL-encoding fixes underway.
      /* post method and url-encoded body */
      status = getEscapedPostdata(postfields, &escapedPostdata, appServices, appCtx);
      if (status != KBrsrSuccess) 
        goto OutOfMemory;
      /* first process the post data to corrected formatted and escaped */
      status = NW_Url_ConvertUnicode( escapedPostdata, &convertedPostdata );
      if (status != KBrsrSuccess) 
        goto OutOfMemory;

      /* convert the post data from NW_Ucs2 to ascii */
      content = NW_Str_CvtToAscii(convertedPostdata);
      if (content == NULL) {
        goto OutOfMemory;
      }
  
      /* Create a buffer and insert the content */
      *body = NW_Buffer_New(0);
      if (*body == NULL) {
        goto OutOfMemory;
      }
      NW_Buffer_SetData(*body, content);

      /* Remove the null-terminator. */
      (*body)->length -= 1;
      if (*header == NULL) {
            *header = UrlLoader_HeadersNew(NULL, (const unsigned char *)NW_URLENCODED_TYPE_STR, 
				referer, boundary, (*body)->length, NULL);
      }

      if (*header == NULL) {
        goto OutOfMemory;
      }
    }
  } else {   
    /* GET method */
    /* Don't add Content-Type to GET methods - there is no content */
    /* Create header because the caller expects a header to be created here */
    if (*header == NULL) {
      *header = UrlLoader_HeadersNew(NULL, NULL, referer, NULL, 0, NULL);
    }

    if (*header == NULL) {
      goto OutOfMemory;
    }
    status = NW_LoadReq_BuildQuery(url, postfields, resultUrl, appServices, appCtx);
    if (status != KBrsrSuccess) {
      goto OutOfMemory;
    }
  }

  /* no longer add accept-charsets as headers */
  /* Do the load */
  if ( postMethodFlag ) {
    *method = NW_UINT8_CAST(NW_URL_METHOD_POST);
  } else {
    *method = NW_UINT8_CAST(NW_URL_METHOD_GET);
  }

  /* cleanup and return normally */
  NW_Str_Delete(escapedPostdata);
  NW_Str_Delete(convertedPostdata);
  NW_Mem_Free(boundary);
  return status;

  /* 
   * clean up and return from KBrsrOutOfMemory errors 
   * Note: NW_Http_Header_Free(), NW_Mem_Free(), NW_Buffer_Free(), 
   * and NW_Str_Delete() all handle NULL pointers 
   */
OutOfMemory:

  NW_Mem_Free(boundary);
  NW_Str_Delete(escapedPostdata);
  NW_Str_Delete(convertedPostdata);
  NW_Mem_Free(content);
  UrlLoader_HeadersFree(*header);
  *header = NULL;
  NW_Str_Delete(*resultUrl);
  *resultUrl = NULL;
  NW_Buffer_Free(*body);
  *body = NULL;

  return KBrsrOutOfMemory;
}

TBrowserStatusCode NW_LoadReq_BuildQuery(const NW_Ucs2 *url,
                                  NW_NVPair_t *postfields, 
                                  NW_Ucs2 **resultUrl,
                                  NW_HED_AppServices_t * appServices,
                                  void * appCtx)
{
  NW_Uint32   len;
  NW_Ucs2     *reqUrl = NULL;
  NW_Ucs2     *frag    = NULL;
  NW_Ucs2     *base    = NULL;
  NW_Ucs2     *pQuery  = NULL;
  NW_Ucs2     *escapedPostdata = NULL;
  TBrowserStatusCode status;
  TBrowserStatusCode getQueryStatus;

  NW_ASSERT(url != NULL);
  NW_ASSERT(resultUrl != NULL);

  len = NW_Str_Strlen(url) + 1;
  status = getEscapedPostdata(postfields, &escapedPostdata, appServices, appCtx);
  if (status != KBrsrSuccess)
    return status;
  
  if ((status = NW_Url_GetBase(url, &base)) != KBrsrSuccess)
  {
    NW_Str_Delete(escapedPostdata);
    return status;
  }
  /* At this point, getQueryStatus cannot be KBrsrMalformedUrl.
     If it was the case it would be detected earlier by NW_Url_GetBase()
  */ 
  getQueryStatus = NW_Url_GetQuery(url, &pQuery);

  /* We should free the pQuery - nobody uses it */
  NW_Str_Delete(pQuery);
  pQuery = NULL;

  status = NW_Url_GetFragId(url, &frag, NW_FALSE);
  if (status != KBrsrSuccess && status != KBrsrFailure)
    /* FAILURE means url had no fragment - not a fatal error */
  {
    NW_Str_Delete(escapedPostdata);
    return status;
  }

  len += NW_Str_Strlen(escapedPostdata) + 1;
  if (frag) 
    len += NW_Str_Strlen(frag);
  
  reqUrl = NW_Str_New(len);
  if (reqUrl == NULL)
  {
    NW_Str_Delete(escapedPostdata);
   return KBrsrOutOfMemory;
  }

  NW_Str_Strcpy(reqUrl, base);

  /* Prevent a separator to be added to the end of string.
     Prevent the WAE_URL_QUERY_SEP to be applied when Url
     has the query field: http://www.foo.com/mywml.wml?pf=ONE
  */
  if ( ! NW_NVPair_IsEmpty(postfields) )
  {
    if (getQueryStatus == KBrsrSuccess) {
      NW_Str_StrcatConst(reqUrl, WAE_URL_POSTFIELD_SEP);
    } else {
      NW_Str_StrcatConst(reqUrl, WAE_URL_QUERY_SEP);
    }
  }

  NW_Str_Strcat(reqUrl, escapedPostdata);

  if (frag)
  {
    NW_Str_StrcatConst(reqUrl, WAE_URL_FRAG_SEP);
    NW_Str_Strcat(reqUrl, frag);
    NW_Str_Delete(frag);
  }

  NW_Str_Delete(base);
  NW_Str_Delete(escapedPostdata);

  *resultUrl = reqUrl;
  return KBrsrSuccess;
}

// -----------------------------------------------------------------------------
// NW_LoadReq::IsMultipart
// Checks the encoding type to see if it is for a multipart request.
// (Other items are commented in header.)
// -----------------------------------------------------------------------------
//
NW_Bool NW_LoadReq_IsMultipart( const NW_Ucs2 *aEncodingType )
    {
    NW_Bool ret = NW_FALSE;
    if ( aEncodingType )
        {
        if ( NW_Str_StricmpConst( aEncodingType, NW_MULTIPART_TYPE_STR ) == 0 )
            {
            ret = NW_TRUE;
            }
        }
    return ret;
    }

//TODO STREAMING: Modify the way in which filenames are handled. The current
//implementation is a quick, short-term solution.  This is going to change
//with streaming implementation.
// -----------------------------------------------------------------------------
// NW_LoadReq::IsUploadFile
// Determines whether or not the current part of the multipart post is for a
// file to be uploaded or not.
// (Other items are commented in header.)
// -----------------------------------------------------------------------------
//
NW_Bool NW_LoadReq_IsUploadFile( const NW_Ucs2 *aValue )
    {
    // Checks file name to see if it starts with "C:\" or E:\".
    if ( aValue )
        {
        if ( (NW_Str_Strnicmp( aValue, KCDrivePrefix, NW_Str_Strlen( KCDrivePrefix ) ) == 0)
            || (NW_Str_Strnicmp( aValue, KEDrivePrefix, NW_Str_Strlen( KEDrivePrefix ) ) == 0) )
            {
            return NW_TRUE;
            }
        }
    return NW_FALSE;
    }
