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

/* Created by araman on 05/31/00 for Crypto Library. */


#ifndef SCR_CRYPTO_H
#define SCR_CRYPTO_H

#include "scr_core.h"

typedef enum {
  SignatureAlgorithmRsaSha1 = 0x01
}SignatureAlgorithm_t;

typedef enum {
  SignatureIdKeyHash = 1
}SIgnatureIdType_t;
/* This is defined by the spec */
typedef enum {
  SignatureCertificateTypeWtls = 2,
  SignatureCertificateTypeX509 = 3,
  SignatureCertificateTypeX968 = 4,
  SignatureCertificateTypeURL  = 5
}SignatureCertificateType_t;
        
typedef enum {
  SignatureContentTypeText = 1,
  SignatureContentTypeData = 2
}SignatureContentType_t;

typedef enum {
  SignatureAuthenticatedAttributeGmtUtcTime = 1,
  SignatureAuthenticatedAttributeSignerNonce = 2
}  SignatureAuthenticatedAttribute_t;

typedef enum {
  SignatureIncludeContent = 1,
  SignatureIncludeKeyHash = 2,
  SignatureIncludeCertificate = 4
} SignatureInclude_t;

typedef enum {
  SignatureKeyIdentifierNone = 0,
  SignatureKeyIdentifierUserKeyHash = 1,
  SignatureKeyIdentifierTrustedKeyHash = 2
} SignatureKeyIdentifier_t;

typedef enum {
  SignStateInit=0,
  SignStateGetSelectedCert,
  SignStateGetPIN,
  SignStateGetSignature,
  SignStateBuildSignature,
  SignStateAddCertificateContent,
  SignStateFinish,
  SignStateError
} SignState_t;

typedef struct {
  NW_Byte algorithm;
  NW_Uint16 signLen;
  NW_Byte *sign;
} Signature;

typedef struct {
  NW_Uint8 signerInfoType;
  NW_Uint16 signerInfoLen;
  NW_Byte* signerInfo;    
} SignerInfo;

typedef struct {
  NW_Uint8 contentType;
  NW_Uint16 contentEncoding;
  NW_Bool contentPresent;
  NW_Uint16 contentLen;
  NW_Byte* content;    
} ContentInfo;

typedef struct {
  NW_Uint8 attributeType;
  NW_Uint8 gmtUtcTime[12];
} AuthenticatedAttribute;

typedef struct {
  NW_Uint8 version;
  NW_Uint8 signerInfoCount;
  Signature *signature;
  SignerInfo **signerInfos;
  ContentInfo *contentInfo;
  AuthenticatedAttribute *authenticatedAttribute;
} SignedContent;


typedef struct {
  SignState_t state;
  NW_Ucs2 *stringToSign;
  NW_Int32 option;
  NW_Int32 keyIdType;
  NW_Byte *keyIdByte;
  NW_Int32 keyIdLen;
  NW_Uint8 certNum;
  void **certificatesList;
  void *selectedCertRef;
  void *selectedKeyRef;
  SignedContent *signedContent;
}SignTextParams_t;
  
void populate_crypto_lib(lib_function_table_t *table);
NW_Byte size_crypto_lib(void);

/* Init the SignTextParams_t struct */
SignTextParams_t* 
NW_ScrCrypto_SignTextParamsNew(SignState_t state, NW_Ucs2 *stringToSign, NW_Int32 option, 
                               NW_Int32 keyIdType, NW_Byte *keyIdByte, NW_Int32 keyIdLen, NW_Uint8 certNum,
                               void **certificatesList, void *selectedCertRef, void *selectedKeyRef);
/* Free the SignTextParams_t struct */
void NW_ScrCrypto_SignTextParamsFree(SignTextParams_t *params);

#endif /*SCR_CRYPTO_H*/
