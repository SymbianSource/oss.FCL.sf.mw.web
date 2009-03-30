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
    $Workfile: scr_browser.c $

    Purpose:

        This file implements the ScriptServer functionality for the Crypto standard library.
*/


#include "scr_conv.h"
#include "scr_api.h"
#include "nwx_mem.h"
#include "scr_srv.h"
#include "scr_crypto.h"
#include "nwx_string.h"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
    Name    :   size_crypto_lib
    Input   :   void  
    Output  :   NW_Byte
    Purpose :   This function returns the number of functions in crypto library
* * * * * * * * * * * * * * * * * * * * * * * * * * * * */

NW_Byte size_crypto_lib(void)
{
  return NO_OF_CRYPTO_FUNCS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
    Name    :   Crypto_signText
    Input   :   void  
    Output  :   bool
    Purpose :   This function enables the user to digitally sign a text
* * * * * * * * * * * * * * * * * * * * * * * * * * * * */

NW_Bool Crypto_signText(void)
{
/*  val_t v1 = uninitialize_val();*/    /* keyId */
/*  NW_Int32 v2 = 0;                 */    /* keyIdType */
/*  NW_Int32 v3 = 0;                 */    /* option */
  val_t v4 = uninitialize_val();    /* stringToSign */
  NW_Int32 option = 0;
  NW_Int32 keyIdType = 0;
  NW_Int32 keyIdLen = 0;
  NW_Ucs2 *stringToSign = 0;
  NW_Ucs2 tempCh;
  NW_Bool retVal = NW_FALSE; 
  NW_Bool success = NW_FALSE;
  str_t keyIdStr = 0;
  NW_Byte *keyString = 0;
  NW_Int32 i = 0;

  /* Pop all parameters. */ 
  v4 = pop_estack();                /* keyId */

  /* Validate KeyId.  */
  if (ScriptVarType(v4) == VAL_TYPE_INVALID) {
    pop_estack_and_free();                /* keyIdType */
    pop_estack_and_free();                /* option */
    pop_estack_and_free();                /* stringToSign */
    free_val(v4);
    push_estack(new_invalid_val());
    return NW_FALSE;
  }
  /* Validate all other parameters. */
  success = GetIntVal(&keyIdType); /* check if keyIdType < 0 */
  if ((!success) || (keyIdType < 0)) {
    pop_estack_and_free();                /* option */
    pop_estack_and_free();                /* stringToSign */
    push_estack(new_invalid_val());
    free_val(v4);
    return NW_FALSE;
  }
  success = GetIntVal(&option); /* Check if option < 0 */
  if ((!success) || (option < 0)) {
    pop_estack_and_free();                /* stringToSign */
    free_val(v4);
    push_estack(new_invalid_val());
    return NW_FALSE;
  }
  /* Validate stringToSign contains information. Also check if the string in empty */
  stringToSign = GetUcs2Val();
  if ((!stringToSign) || (*stringToSign == 0)) {
    free_val(v4);
    push_estack(new_invalid_val());
    return NW_FALSE;
  }

  /* Now for the KeyId.  sets error code if allocation fails */
  if (!val2str(v4, &keyIdStr)) {
    NW_Str_Delete(stringToSign);
    free_str(keyIdStr);
    free_val(v4);
    push_estack(new_invalid_val());
    return NW_FALSE;
  }
  /* If length != 0 or if length is not a multiple of 20 -> invalid */
  keyIdLen = str_len(keyIdStr);
  /* Check for keyIdLen --> added from Wave on 04/18/01 */
  if((keyIdLen != 0) && (keyIdLen %20 != 0))
  {
    NW_Str_Delete(stringToSign);
    keyIdLen = 0;
    keyIdStr = NULL;
    stringToSign = NULL;
  }
  if(keyIdLen == 0)
  {
    keyString = NULL;
  }
  else
  {
    keyString = (NW_Byte *)NW_Mem_Malloc(keyIdLen);
    if (keyString == NULL) {
      NW_Str_Delete(stringToSign);
      set_error_code(SCR_ERROR_OUT_OF_MEMORY);
      free_str(keyIdStr);
      free_val(v4);
      push_estack(new_invalid_val());
      return NW_FALSE;
    }

    for (i = 0;i < keyIdLen; i++) {
      tempCh = scr_charat(keyIdStr, i);
      /* Check for the keyId parameter to be greater than 0xFF */
      if(tempCh > 0xFF) {
        free_str(keyIdStr);
        NW_Str_Delete(stringToSign);
        NW_Mem_Free(keyString);
        free_val(v4);
        push_estack(new_invalid_val());
        return NW_FALSE;
      }
      else {
        keyString[i] = (NW_Byte)tempCh;
      }
    }
  }
  /*
  ** val2str will allocate space for NULL even if strlen is 0, so we always
  ** need free keyIdStr
  */
  free_val(v4);
  free_str(keyIdStr);
  
 /* Calling the respective ScriptAPI */
  retVal = ScriptAPI_signText(stringToSign, option, keyIdType, keyString, keyIdLen); 
  return retVal; 
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
    Name    :   populate_crypto_lib
    Input   :   lib_function_table_t *
    Output  :   void
    Purpose :   This function populates the crypto library
* * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*  functions in Crypto library */
static const lib_function_t crypto_lib_table[NO_OF_CRYPTO_FUNCS] =
{ /*  arg_size,   func_ptr,                 is_async  */
  {   4,          Crypto_signText,          NW_TRUE  }
};

void populate_crypto_lib(lib_function_table_t *table)
{
  
  NW_Int32 index = ScriptAPI_get_lib_index(CRYPTO_LIB_ID);
  (*table)[index] = crypto_lib_table;

  return;
}



/*****************************************************************

  Name: NW_ScrCrypto_SignTextParamsNew()

  Description:  Init the SignTextParams_t struct

  Parameters:    

  Return Value: SignTextParams_t*

******************************************************************/
SignTextParams_t* 
NW_ScrCrypto_SignTextParamsNew(SignState_t state, NW_Ucs2 *stringToSign, NW_Int32 option, 
                               NW_Int32 keyIdType, NW_Byte *keyIdByte, NW_Int32 keyIdLen, NW_Uint8 certNum,
                               void **certificatesList, void *selectedCertRef, void *selectedKeyRef)
{
#ifndef PROFILE_NO_WTLS
  SignTextParams_t *params = (SignTextParams_t*) NW_Mem_Malloc(sizeof(SignTextParams_t));
  if (params != NULL)
  {
    params->state = state;
    params->stringToSign = stringToSign;
    params->option = option;
    params->keyIdType = keyIdType;
    params->keyIdByte = keyIdByte;
    params->keyIdLen = keyIdLen;
    params->certNum = certNum;
    params->certificatesList = certificatesList;
    params->selectedCertRef = selectedCertRef;
    params->selectedKeyRef = selectedKeyRef;
    params->signedContent = NULL;
  }
#else
  SignTextParams_t *params = NULL;
  NW_REQUIRED_PARAM(selectedKeyRef);
  NW_REQUIRED_PARAM(selectedCertRef);
  NW_REQUIRED_PARAM(certificatesList);
  NW_REQUIRED_PARAM(certNum);
  NW_REQUIRED_PARAM(keyIdLen);
  NW_REQUIRED_PARAM(keyIdByte);
  NW_REQUIRED_PARAM(keyIdType);
  NW_REQUIRED_PARAM(option);
  NW_REQUIRED_PARAM(stringToSign);
  NW_REQUIRED_PARAM(state);

#endif /*PROFILE_NO_WTLS*/
  return params;
}

/*****************************************************************

  Name: NW_ScrCrypto_SignTextParamsFree()

  Description:  Free the SignTextParams_t struct

  Parameters:   params - the object to free 

  Return Value: none

******************************************************************/
void NW_ScrCrypto_SignTextParamsFree(SignTextParams_t *params)
{
//#pragma message("TODO:: scr_crypto.c, enable signtext")
NW_REQUIRED_PARAM(params);
#if 0
#ifndef PROFILE_NO_WTLS
  NW_Uint8 i;
  if (params != NULL)
  {
    NW_Mem_Free(params->stringToSign);
    params->stringToSign=NULL;
    NW_Mem_Free(params->keyIdByte);
    params->keyIdByte=NULL;
    NW_Wim_free_RefList(params->certificatesList);
    NW_Wim_free_Ref(params->selectedCertRef);
    NW_Wim_free_Ref(params->selectedKeyRef);
    if (params->signedContent != NULL)
    {
      NW_Mem_Free(params->signedContent->signature);
      if (params->signedContent->signerInfos!=NULL)
      {
        for (i=0;i<params->signedContent->signerInfoCount;i++)
        {
          NW_Mem_Free(params->signedContent->signerInfos[i]);
        }
      }
      NW_Mem_Free(params->signedContent->signerInfos);
      if (params->signedContent->contentInfo != NULL)
      {
        NW_Mem_Free(params->signedContent->contentInfo->content);
      }
      NW_Mem_Free(params->signedContent->contentInfo);
      NW_Mem_Free(params->signedContent->authenticatedAttribute);
      NW_Mem_Free(params->signedContent);
    }
    NW_Mem_Free(params);
  }

#else
  NW_REQUIRED_PARAM(params);

#endif /*PROFILE_NO_WTLS*/
#endif
}
