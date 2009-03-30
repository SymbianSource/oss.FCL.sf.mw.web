/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Status codes used by Browser Engine
*
*/

#ifndef BRSR_STATUS_CODES_H
#define BRSR_STATUS_CODES_H

#ifdef __cplusplus
extern "C" {
#endif

typedef signed int TBrowserStatusCode;

/*
** Includes
*/

#define BRSR_STAT_IS_SUCCESS(a) ((a) == KBrsrSuccess)
#define BRSR_STAT_IS_FAILURE(a) ((a) != KBrsrSuccess)

/* Used by NW_HED_DocumentNode_HandleError to specify the error "class" */
#define BRSR_STAT_CLASS_NONE 0
#define BRSR_STAT_CLASS_GENERAL 1
#define BRSR_STAT_CLASS_HTTP 2


#define	KBrsrSuccess          	                0
#define	KBrsrFailure          	                -26000
#define	KBrsrFileNotFound     	                -26003
#define	KBrsrFileWriteError   	                -26008
#define	KBrsrFileDiskFullError	                -26009
#define	KBrsrOutOfMemory      	                -26011
#define	KBrsrUnexpectedError                    -26012
#define	KBrsrNotImplemented   	                -26013
#define	KBrsrWaitTimeout      	                -26014
#define	KBrsrBufferTooSmall   	                -26015
#define	KBrsrBadInputParam    	                -26016
#define	KBrsrMissingInputParam	                -26017
#define	KBrsrMalformedUrl                       -26018
#define	KBrsrNotFound         	                -26021
#define	KBrsrBufferEnd	                        -26022
#define	KBrsrIterateMore                        -26023
#define	KBrsrIterateDone                        -26024
#define	KBrsrUnknownScheme	                    -26026
#define	KBrsrBadRedirect                        -26027
#define	KBrsrTooManyRedirects                   -26028
#define	KBrsrHttpStatus	                        -26035
#define	KBrsrMissingAuthHeader	                -26037
#define	KBrsrCancelled        	                -26038
#define	KBrsrWpsaNoActiveTransactions           -26067
#define	KBrsrNoContentLength                    -26068
#define	KBrsrTransactionCompete	                -26069
#define	KBrsrWimiInvalidPin	                    -26072
#define	KBrsrWimiPinBlocked	                    -26073
#define	KBrsrWimiNoCard	                        -26075
#define	KBrsrWimiNoInit	                        -26084
#define	KBrsrWimiBadCert                        -26085
#define	KBrsrWimiWimErr	                        -26089
#define	KBrsrWimiSessionNotSet	                -26090
#define	KBrsrWimiCertNotFound                   -26091
#define	KBrsrWimiKeyNotFound                    -26092
#define	KBrsrWimiDuplicateCert	                -26093
#define	KBrsrWtlsHandshakeFail	                -26102
#define	KBrsrWtlsUnknowCertAuth	                -26110
#define	KBrsrWtlsUcNotYetValid	                -26111
#define	KBrsrWtlsUcExpired	                    -26112
#define	KBrsrWtlsUnsupCert	                    -26114
#define	KBrsrWtlsDecodeFail	                    -26116
#define	KBrsrScriptCardNotInDeck                -26131
#define	KBrsrScriptNoAccess	                    -26132
#define	KBrsrScriptBadContent                   -26133
#define	KBrsrScriptErrorUserExit                -26134
#define	KBrsrScriptErrorUserAbort               -26135
#define	KBrsrScriptErrorStackUnderflow	        -26136
#define	KBrsrScriptErrorStackOverflow           -26137
#define	KBrsrScriptFatalLibFuncError            -26138
#define	KBrsrScriptInvalidFuncArgs	            -26139
#define	KBrsrScriptVerificationFailed           -26140
#define	KBrsrScriptExternalFunctionNotFound	    -26141
#define	KBrsrScriptUnableToLoadCompilationUnit	-26142
#define	KBrsrScriptAccessViolation	            -26143
#define	KBrsrWmlbrowserCardNotInDeck            -26144
#define	KBrsrWmlbrowserNoAccess	                -26145
#define	KBrsrWmlbrowserBadContent               -26146
#define	KBrsrWmlbrowserBadContentType           -26148
#define	KBrsrWmlbrowserInputNonconformingToMask	-26149
#define	KBrsrWmlbrowserOnPickEx	                -26150
#define	KBrsrXhtmlBadContent                    -26151
#define	KBrsrCryptoUserCancel                   -26152
#define	KBrsrCryptoNoCert                       -26153
#define	KBrsrUnsupportedFormCharset	            -26156
#define	KBrsrScrproxyScrAbort                   -26163
#define	KBrsrScrproxyScrBusy                    -26164
#define	KBrsrScrproxyScrNotResponding           -26165
#define	KBrsrWaeNavigationCancelled	            -26167
#define	KBrsrConnCancelled	                    -26172
#define	KBrsrConnFailed	                        -26173
#define	KBrsrLmgrConstraintTooSmall	            -26189
#define	KBrsrLmgrNotConstrained	                -26190
#define	KBrsrLmgrFormatOverflow	                -26191
#define	KBrsrLmgrSplitOk                        -26192
#define	KBrsrLmgrNoSplit                        -26194
#define	KBrsrLmgrNotClear                       -26196
#define	KBrsrDomNoStringReturned                -26199
#define	KBrsrDomNodeTypeErr	                    -26200
#define	KBrsrDomNoValuePrefix                   -26201
#define KBrsrWbxmlErrorBytecode                 -26202
#define	KBrsrWbxmlErrorCharsetUnsupported       -26203
#define	KBrsrHedNoCurrentHistEntry	            -26211
#define	KBrsrHedNoPreviousHistEntry	            -26212
#define	KBrsrHedNoNextHistEntry	                -26213
#define	KBrsrHedContentDispatched               -26215
#define	KBrsrTcpHostUnreachable	                -26223
#define	KBrsrSavedPageFailed                    -26224
#define	KBrsrRestartParsing                     -26225
#define KBrsrMvcConverterNotFound               -26226
#define KBrsrContinue							-26227
#define KBrsrLastErrorCode		                -26999

// HTTP status codes
#define KBrsrHTTP100                            -25100
#define KBrsrHTTP101                            -25101
#define KBrsrHTTP102                            -25102
#define KBrsrHTTP200                            -25200
#define KBrsrHTTP201                            -25201
#define KBrsrHTTP202                            -25202
#define KBrsrHTTP203                            -25203
#define KBrsrHTTP204                            -25204
#define KBrsrHTTP205                            -25205
#define KBrsrHTTP206                            -25206
#define KBrsrHTTP300                            -25300
#define KBrsrHTTP301                            -25301
#define KBrsrHTTP302                            -25302
#define KBrsrHTTP303                            -25303
#define KBrsrHTTP304                            -25304
#define KBrsrHTTP305                            -25305
#define KBrsrHTTP400                            -25400
#define KBrsrHTTP401                            -25401
#define KBrsrHTTP402                            -25402
#define KBrsrHTTP403                            -25403
#define KBrsrHTTP404                            -25404
#define KBrsrHTTP405                            -25405
#define KBrsrHTTP406                            -25406
#define KBrsrHTTP407                            -25407
#define KBrsrHTTP408                            -25408
#define KBrsrHTTP409                            -25409
#define KBrsrHTTP410                            -25410
#define KBrsrHTTP411                            -25411
#define KBrsrHTTP412                            -25412
#define KBrsrHTTP413                            -25413
#define KBrsrHTTP414                            -25414
#define KBrsrHTTP415                            -25415
#define KBrsrHTTP500                            -25500
#define KBrsrHTTP501                            -25501
#define KBrsrHTTP502                            -25502
#define KBrsrHTTP503                            -25503
#define KBrsrHTTP504                            -25504
#define KBrsrHTTP505                            -25505


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /* BRSR_STATUS_CODES_H */
