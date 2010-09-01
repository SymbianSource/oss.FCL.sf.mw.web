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
    $Workfile: wml_list.c $

    Purpose:

        Definition of template functions for creating, appending, accessing and 
        deleting generic singley linked lists.
*/

#include "wml_list.h"

#include "nwx_mem.h"
#include "nwx_memseg.h"
#include "BrsrStatusCodes.h"

/*
 * RETURN NW_Wml_List_t * or 0 if malloc fails
 */
NW_Wml_List_t *NW_WmlList_NewEmptyList(NW_Mem_Segment_Id_t memSegment)
{
  NW_Wml_List_t *l = (NW_Wml_List_t *) NW_Mem_Segment_Malloc(sizeof(NW_Wml_List_t), memSegment);
  
  if (l == 0)
    return 0;

  l->first = l->last = l->cursor = 0;
  l->memSegment = memSegment;

  return l;
}

/*
 * RETURN KBrsrSuccess
 *       KBrsrOutOfMemory
 */
TBrowserStatusCode NW_WmlList_Append(NW_Wml_List_t *l, void *el)
{
  NW_Wml_Node_t *n = (NW_Wml_Node_t *)NW_Mem_Segment_Malloc(sizeof(NW_Wml_Node_t), l->memSegment);

  if (n == 0)
    return KBrsrOutOfMemory;
  
  NW_ASSERT(l != NULL);

  n->data = el;
  n->next = 0;

  if (l->last)
  {
    l->last->next = n;
    l->last = n;
  }

  /* append to empty list */
  else
  {
    l->first = l->last = l->cursor = n;
  }

  return KBrsrSuccess;
}


void *NW_WmlList_First(NW_Wml_List_t *l)
{

  l->cursor = l->first;

  if (l->cursor && l->cursor->data)
    return l->cursor->data;
  else
    return 0;
}

void *NW_WmlList_Next(NW_Wml_List_t *l)
{
  if (NW_WmlList_IsLast(l))
    return 0;
  
  l->cursor = l->cursor->next;
  return l->cursor->data;
}

NW_Bool NW_WmlList_IsLast(NW_Wml_List_t *l)
{
  return NW_BOOL_CAST(l->cursor->next == 0);
}

NW_Int32  NW_WmlList_Length(NW_Wml_List_t *l)
{
  NW_Wml_Node_t *n = 0;
  NW_Int32  length = 0;
  
  if (l == 0) 
    return 0;
  n = l->first;

  while(n != 0)
  {
    length++;
    n = n->next;
  }
  return length;
}

#if(0)  
/* These functions are not being used as of 7/31/2001 */
void NW_WmlList_Destroy(NW_Wml_List_t *l)
{
  NW_Wml_Node_t *n, *rest;

  n = rest = l->first;
  while(rest != 0)
  {
    rest = n->next;
    NW_Mem_Free(n); /* should be updated */
  }

  NW_Mem_Free(l);
}

/*
 * RETURN void * or 0 if malloc fails
 */

void *NW_WmlList_ToArray(NW_Wml_List_t *l, NW_Int32  el_size)
{
  NW_Byte *a;
  NW_Int32  len, i;
  
  if (l == 0) return 0;

  len = NW_WmlList_Length(l);
  
  a = (NW_Byte *)NW_Mem_Segment_Malloc(el_size * len, l->memSegment);
  if (a == 0) 
    return 0;

  NW_Mem_memcpy(a, NW_WmlList_First(l), el_size);
  
  for(i = 1; i < len; i++)
      NW_Mem_memcpy(a + i * el_size, NW_WmlList_Next(l), el_size);

  return a;
}
#endif
