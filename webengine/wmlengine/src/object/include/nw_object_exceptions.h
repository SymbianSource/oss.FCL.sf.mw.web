/*
* Copyright (c) 2000 - 2001 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef _NW_Object_exceptions_h_
#define _NW_Object_exceptions_h_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define NW_TRY(_variable) \
  { TBrowserStatusCode _variable = KBrsrSuccess; if (_variable) {}

#define NW_CATCH(_variable)\
  { (_variable) = KBrsrSuccess; goto _NW_FINALLY; } _NW_CATCH:

#define NW_FINALLY \
  _NW_FINALLY:

#define NW_END_TRY \
  }
  
#define NW_FINISHED \
  { goto _NW_FINALLY; }

#define NW_THROW(_status) \
  { goto _NW_CATCH; }

#define NW_THROW_STATUS(_variable, _status) \
  { (_variable) = (TBrowserStatusCode) (_status); goto _NW_CATCH; }

#define _NW_THROW_ON_ERROR(_variable) \
  { if ((_variable) != (TBrowserStatusCode) KBrsrSuccess) { goto _NW_CATCH; } }

#define NW_THROW_SUCCESS(_variable) \
  { (_variable) = (TBrowserStatusCode) KBrsrSuccess; goto _NW_FINALLY; }

#define NW_THROW_UNEXPECTED(_variable) \
  { (_variable) = (TBrowserStatusCode) KBrsrUnexpectedError; goto _NW_CATCH; }

#define NW_THROW_ON_ERROR(_variable) \
  { if ((_variable) != (TBrowserStatusCode) KBrsrSuccess) { \
      if ((_variable) != (TBrowserStatusCode) KBrsrOutOfMemory) { \
        (_variable) = (TBrowserStatusCode) KBrsrUnexpectedError; \
      } goto _NW_CATCH; } }

#define NW_THROW_ON(_variable, _status) \
  { if ((_variable) == (TBrowserStatusCode) (_status)) { goto _NW_CATCH; } }

#define NW_THROW_ON_NULL(_pointer, _variable, _status) \
  { if (((void*)_pointer) == (void*) NULL) { \
      (_variable) = (TBrowserStatusCode) (_status); goto _NW_CATCH; } }

#define NW_THROW_OOM_ON_NULL(_pointer, _variable) \
  NW_THROW_ON_NULL ((_pointer), (_variable), KBrsrOutOfMemory)

#define NW_THROW_ON_TRAP_ERROR(_err, _status) \
  { if ((_err) != (TInt) KErrNone) { \
      if ((_err) == (TInt) KErrNoMemory) { \
        (_status) = (TBrowserStatusCode) KBrsrOutOfMemory; \
      } \
      else { \
        (_status) = (TBrowserStatusCode) KBrsrUnexpectedError; \
      } goto _NW_CATCH; } }

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_Object_exceptions_h_ */
