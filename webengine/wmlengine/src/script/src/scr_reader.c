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
    $Workfile: scr_reader.c $

    Purpose:

       This file implements functions needed to read the entire ScriptServer 
       bytecode.
 */

#include "scr_reader.h"
#include "scr_context.h"
#include "scr_core.h"
#include "scr_api.h"
#include "nwx_url_utils.h"
#include "nw_wae_reader.h"
#include "nw_nvpair.h"
 
#include "nwx_logger.h"
#include "nwx_mem.h"
#include "nwx_http_defs.h"
#include "BrsrStatusCodes.h"

/* prototypes */
NW_Bool read_header_info(NW_Reader_t *r);
NW_Bool read_constpool(NW_Reader_t *r);
NW_Bool read_pragmapool(NW_Reader_t *r, const NW_Ucs2 *referer, const NW_Ucs2 *url);
NW_Bool read_funcpool(NW_Reader_t *r);


#define AC_DOMAIN       0
#define AC_PATH         1
#define UA_PROP         2
#define UA_PROP_SCHEME  3

#define SCRIPT_VERSION 0x01

function_t *read_function(NW_Reader_t *r)
{
  function_t *f = 0;
  NW_Int32  mbyte;
  NW_Int32 temp;

  f = (function_t *)NW_Mem_Malloc(sizeof(function_t));
  if (!f)
  {
    set_error_code(SCR_ERROR_OUT_OF_MEMORY);
    return 0;
  }

  if (!NW_Reader_ReadNextByte(r, &f->nbr_of_args) ||
      !NW_Reader_ReadNextByte(r, &f->nbr_of_local_vars))
  {
    set_error_code(SCR_ERROR_VERIFICATION_FAILED);
    NW_Mem_Free(f);
    return 0;
  }

  temp = f->nbr_of_args + f->nbr_of_local_vars;
  if (temp >= 256)
  {
    set_error_code(SCR_ERROR_STACK_UNDERFLOW);
    NW_Mem_Free(f);
    return 0;
  }

  NW_Reader_ReadMbyteInt(r, &mbyte);
  f->code_size = NW_UINT16_CAST(mbyte);

  f->code = (NW_Byte *)NW_Mem_Malloc(f->code_size * sizeof(NW_Byte));
  if (!f->code)
  {
    set_error_code(SCR_ERROR_OUT_OF_MEMORY);
    return 0;
  }

  if ( (r->readerPos + f->code_size) > r->readerSize)
  {
    set_error_code(SCR_ERROR_VERIFICATION_FAILED);
    NW_Mem_Free(f->code);
    NW_Mem_Free(f);
    return NULL;
  }
  /* changed return value type of NW_Reader_ReadBytes, added return value handling code.
     Prevent accesses beyond compiled code size. (2/16/2000, jac). */
  if (!NW_Reader_ReadBytes(r, f->code, f->code_size))
  {
    set_error_code(SCR_ERROR_VERIFICATION_FAILED);
    NW_Mem_Free( f->code );			/* NEW: 2/18/2000, jac */
	NW_Mem_Free( f );				/* changed: 2/18/2000, jac */
    return 0;
  }

  return f;
}

NW_Bool read_script(NW_Reader_t *r, const NW_Ucs2 *referer, const NW_Ucs2 *url)
{
  context_t *ctx = ScriptAPI_getCurrentContext();
  clear_url_context(ctx);
  return NW_BOOL_CAST(read_header_info(r) &&
                       read_constpool(r) &&
                       read_pragmapool(r, referer, url) &&
                       read_funcpool(r));
}

/* code size and version are not used for decoding the script */
NW_Bool read_header_info(NW_Reader_t *r)
{
  NW_Byte version;
  NW_Int32  code_size;

  NW_ASSERT(r);
  if (!NW_Reader_ReadNextByte(r, &version) || version != SCRIPT_VERSION)
  {
    NW_LOG1(NW_LOG_LEVEL1, "WMLScript Header Validation Error.  Pos = %d\n", NW_Reader_GetPos(r) ); 
    set_error_code(SCR_ERROR_VERIFICATION_FAILED);
    return NW_FALSE;
  }

  NW_Reader_ReadMbyteInt(r, &code_size);

  if (code_size != NW_INT32_CAST(r->readerSize - r->readerPos))
  {
    NW_LOG1(NW_LOG_LEVEL1, "WMLScript Header Validation Error.  Pos = %d\n", NW_Reader_GetPos(r) );
    set_error_code(SCR_ERROR_VERIFICATION_FAILED);
    return NW_FALSE;
  }

  return NW_TRUE;
}


NW_Bool read_constpool(NW_Reader_t *r)
{
  NW_Byte     constType;
  constpool_t *cp;
  NW_Int32    i = 0;
  NW_Int32    cp_size;
  NW_Int32    charset;
  NW_Uint16   cset;

  NW_ASSERT(r);

  NW_Reader_ReadMbyteInt(r, &cp_size);

  NW_Reader_ReadMbyteInt(r, &charset);
  cset = NW_UINT16_CAST(charset);

  /* for the time being we rely on the content because of file simulation */
  if ( ! ( ( cset == HTTP_iso_10646_ucs_2 ) ||
           ( cset == HTTP_iso_8859_1 )      ||
           ( cset == HTTP_utf_8 )           ||
           ( cset == HTTP_us_ascii )) || (cset < (NW_Uint32)charset) )
  {
    /* Not an accepted character set.  Reject. */
    return NW_FALSE;
  }
  r->readerEncoding = cset;

  if (cp_size == 0)
    return NW_TRUE;

  cp = create_cp(cp_size, charset);
  if (!cp)
  {
    NW_LOG1(NW_LOG_LEVEL1, "WMLScript Constant Pool Validation Error.  Pos = %d\n", NW_Reader_GetPos(r) );
    return NW_FALSE;
  }

  for(i = 0; i < cp_size; i++)
  {
    if (!NW_Reader_ReadNextByte(r, &constType))
    {
      NW_LOG1(NW_LOG_LEVEL1, "WMLScript Constant Pool Validation Error.  Pos = %d\n", NW_Reader_GetPos(r)); 
      set_error_code(SCR_ERROR_VERIFICATION_FAILED);
      return NW_FALSE;
    }

    switch (constType)
    {

     case CP_TYPE_8_BIT_SIGNED_INT:
      {
        NW_Byte ival;

        if (!NW_Reader_ReadNextByte(r, &ival))
        {
          NW_LOG1(NW_LOG_LEVEL1, "WMLScript Constant Pool Validation Error.  Pos = %d\n", NW_Reader_GetPos(r) );
          set_error_code(SCR_ERROR_VERIFICATION_FAILED);
          return NW_FALSE;
        }

        append_cp_int(i, (NW_Int8) ival);
        break;
      }
     case CP_TYPE_16_BIT_SIGNED_INT:
      {
        NW_Int16 ival = -1;

        if(!(NW_Reader_ReadInt16(r, &ival)))
        {
          NW_LOG1(NW_LOG_LEVEL1, "WMLScript Constant Pool Validation Error.  Pos = %d\n", NW_Reader_GetPos(r) );
          set_error_code(SCR_ERROR_VERIFICATION_FAILED);
          return NW_FALSE;
        }
        append_cp_int(i, ival);
        break;
      }
     case CP_TYPE_32_BIT_SIGNED_INT:
      {
        NW_Int32  ival = -1;

        if(!(NW_Reader_ReadInt(r, &ival)))
        {
          NW_LOG1(NW_LOG_LEVEL1, "WMLScript Constant Pool Validation Error.  Pos = %d\n", NW_Reader_GetPos(r) );
          set_error_code(SCR_ERROR_VERIFICATION_FAILED);
          return NW_FALSE;
        }
        append_cp_int(i, ival);
        break;
      }

     case CP_TYPE_32_BIT_FLOAT:
      {
        NW_Float32 f;

        f = NW_Reader_ReadFloat(r);
        append_cp_float(i, f);
        break;
      }

     case CP_TYPE_UTF8_STRING:
      {
        NW_Int32  bytes;
        NW_Int32  nchars;
        str_t strval = 0;
        NW_Ucs2 *str;

        NW_Reader_ReadMbyteInt(r, &bytes);
        /* TODO: assert that mbyte is not bigger than short */

        if ( bytes == 0 )
        {
          /* Null string. Don't read any bytes. */
          nchars = 0;
          str = NULL;
        }
        else
        {
          nchars = NW_Reader_GetStrCharCount(r, bytes);
          if ( nchars <= 0 || (nchars + r->readerPos) >= r-> readerSize )    /* CHANGED: 2/18/2000, jac */
          {
            NW_LOG1(NW_LOG_LEVEL1, "WMLScript Constant Pool Validation Error.  Pos = %d\n", NW_Reader_GetPos(r) ); /* prevents improper mem accesses */
            return NW_FALSE;  /* Char count should be > 0.  Return failure. */
          }

          str = (NW_Ucs2 *) NW_Mem_Malloc(nchars * sizeof(NW_Ucs2));
          if (!str)
          {
            NW_LOG1(NW_LOG_LEVEL1, "WMLScript Constant Pool Validation Error.  Pos = %d\n", NW_Reader_GetPos(r));
            set_error_code(SCR_ERROR_OUT_OF_MEMORY);
            NW_Mem_Free(str);
            return NW_FALSE;
          }

          if (!NW_Reader_ReadChars(r, str, bytes))
          {
            NW_LOG1(NW_LOG_LEVEL1, "WMLScript Constant Pool Validation Error.  Pos = %d\n", NW_Reader_GetPos(r));
            NW_Mem_Free(str);
            return NW_FALSE;
          }
        }

        strval = new_str(nchars, str);
        append_cp_string(i, strval);
        NW_Mem_Free(str);
        free_str(strval);
        break;
      }

     case CP_TYPE_EMPTY_STRING:
      {
        str_t strval = 0;

        strval = new_str(0, 0);
        append_cp_string(i, strval);
        free_str(strval);
        break;
      }

     case CP_TYPE_EXT_STRING:
      {
        if ( (charset == HTTP_utf_8)      ||
             (charset == HTTP_us_ascii)   ||
             (charset == HTTP_iso_8859_1) ||
             (charset == HTTP_iso_10646_ucs_2) )
        {
          NW_Int32  mbyte;
          NW_Int32  nchars;
          NW_Int16 bytes = -1;
          str_t strval = 0;
          NW_Ucs2* buf;

          NW_Reader_ReadMbyteInt(r, &mbyte);
          bytes = (NW_Int16)mbyte;

          if ((bytes != mbyte) || ( bytes == 0 ))
          {
            /* Null string. Don't read any bytes. */
            nchars = 0;
            buf = NULL;
          }
          else
          {
            nchars = NW_Reader_GetStrCharCount(r, bytes);
            if ( nchars <= 0 || (nchars + r-> readerPos) >= r-> readerSize)				/* CHANGED: 2/18/2000, jac */
            {
              NW_LOG1(NW_LOG_LEVEL1, "WMLScript Constant Pool Validation Error.  Pos = %d\n", NW_Reader_GetPos(r)); 
              return NW_FALSE;  /* Char count should be > 0.  Return failure. */
            }
            buf = (NW_Ucs2 *)NW_Mem_Malloc(nchars * sizeof(NW_Ucs2));
            if (!buf)
            {
              NW_LOG1(NW_LOG_LEVEL1, "WMLScript Constant Pool Validation Error.  Pos = %d\n", NW_Reader_GetPos(r) );
              set_error_code(SCR_ERROR_OUT_OF_MEMORY);
              NW_Mem_Free(buf);
              return NW_FALSE;
            }
			
            if (!NW_Reader_ReadChars(r, (NW_Ucs2*) buf, bytes))
            {
              NW_LOG1(NW_LOG_LEVEL1, "WMLScript Constant Pool Validation Error.  Pos = %d\n", NW_Reader_GetPos(r) );
              NW_Mem_Free(buf);
              return NW_FALSE;
            }
          }

          strval = new_str(nchars , buf);
          NW_Mem_Free(buf);
          append_cp_string(i, strval);
          free_str(strval);
        }
        else
        {
          NW_LOG1(NW_LOG_LEVEL1, "WMLScript Constant Pool Validation Error.  Pos = %d\n", NW_Reader_GetPos(r) );
          set_error_code(SCR_ERROR_VERIFICATION_FAILED);
          return NW_FALSE;
        }
        break;
        
      }

     default:
      NW_LOG1(NW_LOG_LEVEL1, "WMLScript Constant Pool Validation Error.  Pos = %d\n", NW_Reader_GetPos(r));
      set_error_code(SCR_ERROR_VERIFICATION_FAILED);
      return NW_FALSE;
    }/*end of switch*/
    
  } /*end of for loop*/
  return NW_TRUE;
}

NW_Bool check_access(NW_Int32  path_index, NW_Int32  domain_index, const NW_Ucs2 *referer, const NW_Ucs2 *url)
{
  NW_Ucs2 *path = 0;
  NW_Ucs2 *domain = 0;
  NW_Bool res = NW_FALSE;
  NW_Ucs2 default_path[2] = { '/', '\0' };
  TBrowserStatusCode status;

  if ((path_index == -1) && (domain_index == -1)) return NW_TRUE;

  if (path_index != -1)
  {
    val_t v = uninitialize_val();

    if (!valid_const_index(path_index))
    {
      set_error_code(SCR_ERROR_VERIFICATION_FAILED);
      goto clean;
    }

    v = get_const(path_index);
    path = val2ucs2(v);
    if (!path) goto clean;
    
  }
  else path = default_path;

  if (domain_index != -1)
  {
    val_t v = uninitialize_val();

    if (!valid_const_index(domain_index))
    {
      set_error_code(SCR_ERROR_VERIFICATION_FAILED);
      goto clean;
    }

    v = get_const(domain_index);
    domain = val2ucs2(v);
    if (!domain) goto clean;
  }

  else
  {
    /* if no domain was specified we use the host of the url */
    status = NW_Url_GetHost(url, &domain);
    if (status == KBrsrOutOfMemory)
    {
      set_error_code(SCR_ERROR_OUT_OF_MEMORY);
      res = NW_FALSE;
      goto clean;
    }
    if (!domain)
    {
      NW_ASSERT(NW_FALSE);
      res = NW_FALSE;
      goto clean;
    }
  }

  status = NW_Url_AccessOK(domain, path, referer, NULL);
  if (status == KBrsrOutOfMemory)
  {
    set_error_code(SCR_ERROR_OUT_OF_MEMORY);
    res = NW_FALSE;
  }
  else if (status == KBrsrFailure)
    res = NW_FALSE;
  else
    res = NW_TRUE;

clean:
  if (path && (path != default_path)) NW_Mem_Free(path);
  if (domain) NW_Mem_Free(domain);

  return res;
}

NW_Bool read_pragmapool(NW_Reader_t *r, const NW_Ucs2 *referer, const NW_Ucs2 *url)
{
  NW_Int32  i, index1, index2, index3, numOfPragmas;
  NW_Int32  domain_index = -1, path_index = -1;
  NW_Bool accessDisable = NW_TRUE;
  NW_Byte type;

  NW_Reader_ReadMbyteInt(r, &numOfPragmas);
  for(i = 0; i < numOfPragmas; i++)
  {
    if (!NW_Reader_ReadNextByte(r, &type))
    {
      NW_LOG1(NW_LOG_LEVEL1, "WMLScript Pragma Pool Validation Error.  Pos = %d\n", NW_Reader_GetPos(r) );
      set_error_code(SCR_ERROR_VERIFICATION_FAILED);
      return NW_FALSE;
    }

    switch (type) {

     case AC_DOMAIN:
      NW_Reader_ReadMbyteInt(r, &domain_index);
      if (!is_const_string(domain_index))
      {
        NW_LOG1(NW_LOG_LEVEL1, "WMLScript Pragma Pool Validation Error.  Pos = %d\n", NW_Reader_GetPos(r) );
        set_error_code(SCR_ERROR_VERIFICATION_FAILED);
        return NW_FALSE;
      }
      accessDisable = NW_FALSE;
      /* set domain in context */
      break;
     case AC_PATH:
      NW_Reader_ReadMbyteInt(r, &path_index);
      if (!is_const_string(path_index))
      {
        NW_LOG1(NW_LOG_LEVEL1, "WMLScript Pragma Pool Validation Error.  Pos = %d\n", NW_Reader_GetPos(r) );
        set_error_code(SCR_ERROR_VERIFICATION_FAILED);
        return NW_FALSE;
      }
      accessDisable = NW_FALSE;
      /* set path in context */
      break;
     case UA_PROP:
      NW_Reader_ReadMbyteInt(r, &index1);
      NW_Reader_ReadMbyteInt(r, &index2);
      if (!is_const_string(index1) || !is_const_string(index2))
      {
        NW_LOG1(NW_LOG_LEVEL1, "WMLScript Pragma Pool Validation Error.  Pos = %d\n", NW_Reader_GetPos(r) );
        set_error_code(SCR_ERROR_VERIFICATION_FAILED);
        return NW_FALSE;
      }
      /* TODO handle UA */
      break;
     case UA_PROP_SCHEME:
      NW_Reader_ReadMbyteInt(r, &index1);
      NW_Reader_ReadMbyteInt(r, &index2);
      NW_Reader_ReadMbyteInt(r, &index3);

      if (!is_const_string(index1) || !is_const_string(index2) || !is_const_string(index3))
      {
        NW_LOG1(NW_LOG_LEVEL1, "WMLScript Pragma Pool Validation Error.  Pos = %d\n", NW_Reader_GetPos(r) );
        set_error_code(SCR_ERROR_VERIFICATION_FAILED);
        return NW_FALSE;
      }
      /* TODO handle UA */
      break;
     default:
      NW_LOG1(NW_LOG_LEVEL1, "WMLScript Pragma Pool Validation Error.  Pos = %d\n", NW_Reader_GetPos(r) );
      set_error_code(SCR_ERROR_VERIFICATION_FAILED);
      return NW_FALSE;
    }
  }
  if (!accessDisable && ! check_access(path_index, domain_index, referer, url))
  {
    NW_LOG1(NW_LOG_LEVEL1, "WMLScript Pragma Pool Access Validation Error.  Pos = %d\n", NW_Reader_GetPos(r) );
    set_error_code(SCR_ERROR_ACCESS_VIOLATION);
    return NW_FALSE;
  }
  else return NW_TRUE;
}

NW_Ucs2 *read_fname_str(NW_Reader_t *r)
{
  NW_Byte  len;
  NW_Ucs2  *s = NULL;
  NW_Ucs2  wc;
  NW_Int32 i;

  if (!NW_Reader_ReadNextByte(r, &len))
  {
    set_error_code(SCR_ERROR_VERIFICATION_FAILED);
    return NULL;
  }
  if (((NW_Uint32) len + r->readerPos) >= r-> readerSize)		     /* NEW: 2/18/2000, jac */
  {
	set_error_code( SCR_ERROR_VERIFICATION_FAILED );
	return NULL;
  }

  s = (NW_Ucs2 *)NW_Mem_Malloc((len + 1) * sizeof(NW_Ucs2));
  if (!s)
  {
    set_error_code(SCR_ERROR_OUT_OF_MEMORY);
    return NULL;												 /* was: 0; 2/18/2000, jac */
  }

  NW_Mem_memset(s, 0, (len + 1) * sizeof(NW_Ucs2));

  for(i = 0; i < len; i++)
  {
    if(!(NW_Reader_ReadChar(r, &wc)))
    {
      NW_Mem_Free(s);
      set_error_code( SCR_ERROR_VERIFICATION_FAILED );
      return NULL;
    }
    s[i] = wc;
  }

  if (!NW_CheckName(s))
  {
    NW_Mem_Free(s);
    set_error_code( SCR_ERROR_VERIFICATION_FAILED );
    return NULL;
  }

  return s;
}


NW_Bool is_identifier(str_t s)
{
  NW_Ucs2 c;
  NW_Int32  i, len;

  c = scr_charat(s, 0);

  if (!str_is_alpha(c)) return NW_FALSE;

  len = str_len(s);
  for(i = 1; i < len; i++)
    if (!str_is_alpha(c) ||!str_is_digit(c)) return NW_FALSE;

  if (str_is_reserved(s)) return NW_FALSE;

  return NW_TRUE;
}

NW_Bool read_fname_table(NW_Reader_t *r, NW_Int32  no_of_fcns)
{
  NW_Int32  i;
  NW_Byte fn_index;
  NW_Ucs2 *str = 0;
  NW_Byte no_of_names;

  if (!NW_Reader_ReadNextByte(r, &no_of_names))
  {
    set_error_code(SCR_ERROR_VERIFICATION_FAILED);
    return NW_FALSE;
  }

  if (no_of_names > 0) {
    if (!create_fname_table(no_of_names))
    {
      set_error_code(SCR_ERROR_VERIFICATION_FAILED);
      return NW_FALSE;
    }

    for(i =0; i < no_of_names; i++)
    {
      if (!NW_Reader_ReadNextByte(r, &fn_index) || (fn_index >= no_of_fcns))
      {
        set_error_code(SCR_ERROR_VERIFICATION_FAILED);
        return NW_FALSE;
      }
      str = read_fname_str(r);
      if (!str)
      {
        set_error_code(SCR_ERROR_VERIFICATION_FAILED);
        return NW_FALSE;
      }
      add_fname(fn_index, str);
      
    }
  }
  return NW_TRUE;
}

NW_Bool read_funcpool(NW_Reader_t *r)
{
  NW_Int32  i;
  function_t *f;
  NW_Byte no_of_fcns;

  if (!NW_Reader_ReadNextByte(r, &no_of_fcns) || (no_of_fcns == 0))
  {
    NW_LOG1(NW_LOG_LEVEL1, "WMLScript Function Pool Validation Error.  Pos = %d\n", NW_Reader_GetPos(r) );
    set_error_code(SCR_ERROR_VERIFICATION_FAILED);
    return NW_FALSE;
  }

  if (!create_func_pool(no_of_fcns))
  {
    NW_LOG1(NW_LOG_LEVEL1, "WMLScript Function Pool Validation Error.  Pos = %d\n", NW_Reader_GetPos(r) );
    set_error_code(SCR_ERROR_VERIFICATION_FAILED);
    return NW_FALSE;
  }

  if (!read_fname_table(r, no_of_fcns)) {
    NW_LOG1(NW_LOG_LEVEL1, "WMLScript Function Pool Validation Error.  Pos = %d\n", NW_Reader_GetPos(r) );
    return NW_FALSE;
  }

  for(i =0; i < no_of_fcns; i++) {
	f = read_function(r);
    if (f == NULL) {
		NW_LOG1(NW_LOG_LEVEL1, "WMLScript Function Pool Validation Error.  Pos = %d\n", NW_Reader_GetPos(r) );
      return NW_FALSE;
    }
    add_func_fp(f);
  }
  return NW_TRUE;
}


