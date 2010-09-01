/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*     Utility for converting cxml parser status codes to browser
*     status codes
*     
*
*/

#include "nwx_statuscodeconvert.h"

TBrowserStatusCode StatusCodeConvert( NW_Status_t cxml_status )
  {
  TBrowserStatusCode status;
  
	switch(cxml_status)
	  {
    case NW_STAT_SUCCESS:
      {
      status = KBrsrSuccess;
      break;
      }
    case NW_STAT_NOT_FOUND:
      {
      status = KBrsrNotFound;
      break;
      }
    case NW_STAT_OUT_OF_MEMORY:
      {
      status = KBrsrOutOfMemory;
      break;
      }
    case NW_STAT_DOM_NODE_TYPE_ERR:
      {
	    status = KBrsrDomNodeTypeErr;
	    break;
      }
    case NW_STAT_DOM_NO_VALUE_PREFIX:
      {
      status = KBrsrDomNoValuePrefix;
      break;
      }
    case NW_STAT_WBXML_ERROR_BYTECODE:
      {
      status = KBrsrWbxmlErrorBytecode;
      break;
      }
    case NW_STAT_WBXML_ERROR_CHARSET_UNSUPPORTED:
      {
      status = KBrsrWbxmlErrorCharsetUnsupported;
      break;
      }
    default:
      {
      status = KBrsrFailure;
      break;
      }
    }
	  
	return status;  
  }
