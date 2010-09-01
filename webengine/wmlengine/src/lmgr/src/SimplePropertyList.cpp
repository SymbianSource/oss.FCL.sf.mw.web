/*
* Copyright (c) 2000 - 2004 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32base.h>
#include "BrsrStatusCodes.h"
#include "TKeyArrayPtr.h"
#include "nw_lmgr_simplepropertylisti.h"
#include "nw_lmgr_slavepropertylist.h"
#include "GDIFont.h"
#include "CSSPropertyTable.h"

/**
*   Wrapper class for wrapping NW_LMgr_PropertyList_Entry_t.
*   This wrapper class, being a real C++ class, can be used in Symbian
*   API container classes such as CArrayPtrFix.  This wrapper class was
*   implemented and utilized to allow the old style, non-C++ code to take
*   advantage of the efficiencies in the Symbian container classes.
*
*   @lib lmgr.lib
*   @since 2.1
*/
class CLmgrPropertyEntry : public CBase
    {
    public: // Constructors and destructor

    	  CLmgrPropertyEntry() { iEntry.value.object = NULL; }

		  virtual ~CLmgrPropertyEntry()
		  {
			  if (iEntry.value.object != NULL)
			  {
				  if( iEntry.type == NW_CSS_ValueType_Font )
                  {
					  // font is CBased now
					  delete (CGDIFont*)iEntry.value.object;
                  }
				  else if (((iEntry.type & NW_CSS_ValueType_Mask) >= NW_CSS_ValueType_Object) && !(iEntry.type & NW_CSS_ValueType_Copy))
				  {
					  NW_Object_Delete (iEntry.value.object);
					  iEntry.value.object = NULL;
                  }
			  }
		  }

    public: // Data

	      NW_LMgr_PropertyName_t iPropName;
	      NW_LMgr_PropertyList_Entry_t iEntry;

    };

typedef CArrayPtrFlat<CLmgrPropertyEntry>	CLmgrPropertyList;


static TBool NW_LMgr_SimplePropertyList_IsCachedEntry( NW_LMgr_PropertyName_t aPropertyName )
  {
  // check inherited properties
  if( aPropertyName >= NW_CSS_PropInherit )
    {
    // no xor?
    aPropertyName = (NW_LMgr_PropertyName_t)(aPropertyName - NW_CSS_PropInherit);
    }    
  return( aPropertyName <= NW_CSS_CachedProperty_Num );
  }
/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_LMgr_SimplePropertyList_Class_t NW_LMgr_SimplePropertyList_Class = {
  { /* NW_Object_Core             */
    /* super                      */ &NW_LMgr_PropertyList_Class,
    /* queryInterface             */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base             */
    /* interfaceList              */ NULL
  },
  { /* NW_Object_Dynamic          */
    /* instanceSize               */ sizeof (NW_LMgr_SimplePropertyList_t),
    /* construct                  */ _NW_LMgr_SimplePropertyList_Construct,
    /* destruct                   */ _NW_LMgr_SimplePropertyList_Destruct
  },
  { /* NW_LMgr_PropertyList       */
    /* get                        */ _NW_LMgr_SimplePropertyList_Get,
    /* set                        */ _NW_LMgr_SimplePropertyList_Set,
    /* remove                     */ _NW_LMgr_SimplePropertyList_Remove,
    /* clone                      */ _NW_LMgr_SimplePropertyList_Clone,
    /* clear                      */ _NW_LMgr_SimplePropertyList_Clear
  },
  { /* NW_LMgr_SimplePropertyList */
    /* unused                     */ NULL
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_LMgr_SimplePropertyList_Construct (NW_Object_Dynamic_t* dynamicObject,
                                       va_list* argp)
{
  NW_LMgr_SimplePropertyList_t* thisObj;

  //lint --e{550} Symbol 'capacity' and 'keySize' not accessed

  /* for convenience */
  thisObj = NW_LMgr_SimplePropertyListOf (dynamicObject);

  /* initialize the object */
  NW_Uint32 keySize = va_arg (*argp, NW_Uint32);
  NW_Uint16 capacity = (NW_Uint16)va_arg (*argp, NW_Uint32);
  NW_Uint16 increment = (NW_Uint16)va_arg (*argp, NW_Uint32);

  /* map holds pointers to the entry objects */
  thisObj->map = new CLmgrPropertyList(increment);
  if (thisObj->map == NULL)
  {
    return KBrsrOutOfMemory;
  }

  thisObj->iCachedProperty = (void**)NW_Mem_Malloc( sizeof( CLmgrPropertyEntry* ) * NW_CSS_CachedProperty_Num );
  if( !thisObj->iCachedProperty )
    {
    //
    delete (CLmgrPropertyList*)(thisObj->map);
    return KBrsrOutOfMemory;
    }
  // init
  CLmgrPropertyEntry** cachedList = (CLmgrPropertyEntry**)(thisObj->iCachedProperty);
  for( TInt i = 0; i < NW_CSS_CachedProperty_Num; i++ )
    {    
    cachedList[ i ] = NULL;
    }
  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
void
_NW_LMgr_SimplePropertyList_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_LMgr_SimplePropertyList_t* thisObj;
  CLmgrPropertyList* list;
 
  /* for convenience */
  thisObj = NW_LMgr_SimplePropertyListOf (dynamicObject);
  list = (CLmgrPropertyList*)(thisObj->map);

  if (list != NULL)
  {
    /* free the map entries */
    list->ResetAndDestroy();

    /* free the map itself */
    delete list;
    thisObj->map = NULL;
  }
  //
  CLmgrPropertyEntry** cachedList = (CLmgrPropertyEntry**)(thisObj->iCachedProperty);
  if ( cachedList != NULL)
  {	
	  for( TInt i = 0; i < NW_CSS_CachedProperty_Num; i++ )
		{
		delete cachedList[ i ];
		}
	  thisObj->iCachedProperty = NULL;
	  NW_Mem_Free( cachedList );
  }
  
  if (thisObj->backupPropList != NULL)
  {
    NW_Object_Delete(thisObj->backupPropList);
	thisObj->backupPropList = NULL;
  }
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode 
_NW_LMgr_SimplePropertyList_Get (const NW_LMgr_PropertyList_t* propertyList,
                                 NW_LMgr_PropertyName_t key,
                                 NW_LMgr_PropertyList_Entry_t* entry)
{  
  NW_LMgr_SimplePropertyList_t* thisObj;
  NW_LMgr_PropertyList_Entry_t prop;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (propertyList,
                                     &NW_LMgr_SimplePropertyList_Class));
  NW_ASSERT (entry != NULL);

  /* for convenience */
  thisObj = NW_LMgr_SimplePropertyListOf (propertyList);
  prop.value.object = NULL;

  if ((key == NW_CSS_Prop_elementId) && (thisObj->domNode != NULL))
  {
    entry->type = NW_CSS_ValueType_Object;
    entry->value.object = thisObj->domNode;
    return KBrsrSuccess;
  }

  if ((key == NW_CSS_Prop_leftBorderStyle) ||
      (key == NW_CSS_Prop_leftPadding) ||
      (key == NW_CSS_Prop_leftMargin))
  {
    NW_LMgr_Box_t *box;
    NW_LMgr_PropertyList_t* siblingPropList;

    box = NULL;
    (void) NW_LMgr_PropertyList_Get(thisObj, NW_CSS_Prop_sibling, &prop);
    if (prop.value.object)
    {
      if (NW_Object_IsInstanceOf(prop.value.object, &NW_LMgr_Box_Class))
      {
        box = (NW_LMgr_Box_t*)prop.value.object;
        siblingPropList = NW_LMgr_Box_PropListGet(box);

        if (NW_Object_IsInstanceOf(siblingPropList, &NW_LMgr_SlavePropertyList_Class))
        {
          /* implies that next box is a cloned box */
          switch(key)
          {
          /* Hide the borders where the split occurred */
          case NW_CSS_Prop_leftBorderStyle:
            entry->type = NW_CSS_ValueType_Token;
            entry->value.token = NW_CSS_PropValue_none;
            return KBrsrSuccess;

          /* Remove any padding or margins */
          case NW_CSS_Prop_leftPadding:
            entry->type = NW_ADT_ValueType_Integer;
            entry->value.token = 0;
            return KBrsrSuccess;

          case NW_CSS_Prop_leftMargin:
            entry->type = NW_ADT_ValueType_Integer;
            entry->value.token = 0;
            return KBrsrSuccess;

          default:
            break;
          }/* end switch */
        }/* end if */
      }/* end if */
    }/* if (prop.value.object) */
  } /*end if */

  return NW_LMgr_SimplePropertyList_MapLookup( thisObj, key, entry );
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_LMgr_SimplePropertyList_Set (NW_LMgr_PropertyList_t* propertyList,
                                 NW_LMgr_PropertyName_t key,
                                 const NW_LMgr_PropertyList_Entry_t* entry)
  {
  NW_LMgr_SimplePropertyList_t* thisObj;
  CLmgrPropertyList* list;
  
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (propertyList,
    &NW_LMgr_SimplePropertyList_Class));
  NW_ASSERT (entry != NULL);
  
  /* for convenience */
  thisObj = NW_LMgr_SimplePropertyListOf (propertyList);
  list = (CLmgrPropertyList*)(thisObj->map);
  CLmgrPropertyEntry** cachedList = (CLmgrPropertyEntry**)(thisObj->iCachedProperty);
  
  if ((key == NW_CSS_Prop_elementId) && (entry->type == NW_CSS_ValueType_Object))
    {
    thisObj->domNode = entry->value.object;
    return KBrsrSuccess;
    }
  
  /* if entry already exists, delete it */
  CLmgrPropertyEntry* newEntry = new CLmgrPropertyEntry();
  if(newEntry == NULL)
    {
    return KBrsrOutOfMemory;
    }
  newEntry->iPropName = key;

  TBool cachedEntry = EFalse;
  TNumKeyArrayPtr matchkey(_FOFF(CLmgrPropertyEntry, iPropName), ECmpTUint16);
  // check if this is a cahced property
  if( NW_LMgr_SimplePropertyList_IsCachedEntry( key ) )
    {
    // cached property
    cachedEntry = ETrue;
    // is there XOR operator?
    key = (NW_LMgr_PropertyName_t)( key > NW_CSS_PropInherit ? ( key - NW_CSS_PropInherit ) : key );
    // remove duplicate property
    delete cachedList[ key - 1 ];
    cachedList[ key - 1 ] = NULL;
    }
  else
    {
    TInt index;
    if (list->FindIsq(newEntry, matchkey, index) == 0)
      {
      CLmgrPropertyEntry* duplicateEntry;
      duplicateEntry = list->At(index);
      list->Delete(index);
      list->Compress();
      delete duplicateEntry;
      }
    }
  
  /* use byte-wise copy to get the unaligned client data into the new entry */
  NW_Mem_memcpy (&(newEntry->iEntry), entry, sizeof *entry);
  
  // set the entry in the map  
  if( cachedEntry )
    {
    //
    cachedList[ key - 1 ] = newEntry;
    }
  else
    {
    TRAPD(ret, list->InsertIsqL(newEntry, matchkey));
    if (ret == KErrNoMemory)
      {
      delete newEntry;
      return KBrsrOutOfMemory;
      }
    }
  
  return KBrsrSuccess;
  }

/* ------------------------------------------------------------------------- */
TBrowserStatusCode 
_NW_LMgr_SimplePropertyList_Remove (NW_LMgr_PropertyList_t* propertyList,
                                    NW_LMgr_PropertyName_t key )
{
  NW_LMgr_SimplePropertyList_t* thisObj;
  CLmgrPropertyList* list;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (propertyList,
                                     &NW_LMgr_SimplePropertyList_Class));

  /* for convenience */
  thisObj = NW_LMgr_SimplePropertyListOf (propertyList);
  list = (CLmgrPropertyList*)(thisObj->map);

  if( NW_LMgr_SimplePropertyList_IsCachedEntry( key ) )
    {
    CLmgrPropertyEntry** cachedList = (CLmgrPropertyEntry**)(thisObj->iCachedProperty);
    key = (NW_LMgr_PropertyName_t)( key > NW_CSS_PropInherit ? ( key - NW_CSS_PropInherit ) : key );
    // remove property
    delete cachedList[ key - 1 ];
    cachedList[ key - 1 ] = NULL;
    }
  else
    {
    /* remove the property */
    CLmgrPropertyEntry newEntry;
    CLmgrPropertyEntry* entryToDelete;
    newEntry.iPropName = key;
    TInt index;
    TNumKeyArrayPtr matchkey(_FOFF(CLmgrPropertyEntry, iPropName), ECmpTUint16);
    if (list->FindIsq(&newEntry, matchkey, index) == 0)
      {
      entryToDelete = list->At(index);
      list->Delete(index);
      list->Compress();
      delete entryToDelete;
      }
    }
    return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
NW_LMgr_PropertyList_t*
_NW_LMgr_SimplePropertyList_Clone (const NW_LMgr_PropertyList_t* propertyList)
{
  NW_LMgr_PropertyList_t* newPropertyList = NULL;

  NW_LMgr_SimplePropertyList_t* thisObj;
  NW_LMgr_PropertyList_Entry_t entry;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (propertyList,
                                     &NW_LMgr_SimplePropertyList_Class));
  /* for convenience */
  thisObj = NW_LMgr_SimplePropertyListOf (propertyList);

  entry.value.object = NULL;
  (void)NW_LMgr_PropertyList_Get(thisObj, NW_CSS_Prop_sibling, &entry);

  /* instantiate a new SimplePropertyList object */
  newPropertyList = (NW_LMgr_PropertyList_t*)
            NW_LMgr_SlavePropertyList_New ((NW_LMgr_Box_s *)entry.value.object);
  if (newPropertyList == NULL)
  {
    return NULL;
  }

  return newPropertyList;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode 
_NW_LMgr_SimplePropertyList_Clear (NW_LMgr_PropertyList_t* propertyList) 
{
  NW_LMgr_SimplePropertyList_t* thisObj;
  CLmgrPropertyList* list;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (propertyList,
                                     &NW_LMgr_SimplePropertyList_Class));
  /* for convenience */
  thisObj = NW_LMgr_SimplePropertyListOf (propertyList);
  list = (CLmgrPropertyList*)(thisObj->map);

  if (list != NULL)
    {
    list->ResetAndDestroy();
    }
  // clear cached properties
  CLmgrPropertyEntry** cachedList = (CLmgrPropertyEntry**)(thisObj->iCachedProperty);
  for( TInt i = 0; i < NW_CSS_CachedProperty_Num; i++ )
    {
    delete cachedList[ i ];
    cachedList[ i ] = NULL;
    }
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_LMgr_SimplePropertyList_Copy (const NW_LMgr_SimplePropertyList_t* thisObj,
                                  NW_LMgr_PropertyList_t* destination)
{
  TInt i;
  CLmgrPropertyList* list;
  TBrowserStatusCode status = KBrsrSuccess;

  /* for convenience */
  list = (CLmgrPropertyList*)(thisObj->map);

	for( i = 0; i < list->Count(); i++)
	  {
    CLmgrPropertyEntry* entry = list->At(i);
    NW_ASSERT(entry != NULL);

    if ((entry->iEntry.type & NW_CSS_ValueType_Mask) >= NW_CSS_ValueType_Object)
      {
      entry->iEntry.type |= NW_CSS_ValueType_Copy;
      }

    status = NW_LMgr_PropertyList_Set (destination, entry->iPropName, &(entry->iEntry));
    NW_ASSERT (status == KBrsrSuccess);
    }
  // copy cached properties
  CLmgrPropertyEntry** cachedList = (CLmgrPropertyEntry**)(thisObj->iCachedProperty);
  //
  for( i = 0; i < NW_CSS_CachedProperty_Num; i++ )
    {
    CLmgrPropertyEntry* entry = cachedList[ i ];

    if( entry )
      {
      if ((entry->iEntry.type & NW_CSS_ValueType_Mask) >= NW_CSS_ValueType_Object)
        {
        entry->iEntry.type |= NW_CSS_ValueType_Copy;
        }
      status = NW_LMgr_PropertyList_Set( destination, entry->iPropName, &(entry->iEntry) );
      NW_ASSERT( status == KBrsrSuccess );
      }
    }
  return status;
}

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TInt 
NW_LMgr_SimplePropertyList_Count (const NW_LMgr_SimplePropertyList_t* thisObj)
  {
  TInt count;
  CLmgrPropertyList* list;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj,
                                     &NW_LMgr_SimplePropertyList_Class));
  NW_ASSERT (thisObj->map != NULL);

  /* for convenience */
  list = (CLmgrPropertyList*)(thisObj->map);

  count = (list) ? list->Count() : 0;
  //
  CLmgrPropertyEntry** cachedList = (CLmgrPropertyEntry**)(thisObj->iCachedProperty);
  for( TInt i = 0; i < NW_CSS_CachedProperty_Num; i++ )
    {
    if( cachedList[ i ] )
      {
      count++;
      }
    }  
  return count;
  }

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_LMgr_SimplePropertyList_At (const NW_LMgr_SimplePropertyList_t* thisObj,
                               TInt index,
                               NW_LMgr_PropertyList_Entry_t* entry)
  {
  CLmgrPropertyList* list;
  TBrowserStatusCode status = KBrsrFailure;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj,
                                     &NW_LMgr_SimplePropertyList_Class));
  NW_ASSERT (entry != NULL);
  NW_ASSERT (thisObj->map != NULL);

  TInt cacheCount = 0;
  CLmgrPropertyEntry* listEntry;
  CLmgrPropertyEntry** cachedList = (CLmgrPropertyEntry**)(thisObj->iCachedProperty);
  //
  if( entry )
    {
    for( TInt i = 0; i < NW_CSS_CachedProperty_Num; i++ )
      {
      if( cachedList[ i ] )
        {      
        cacheCount++;
        }
      // 
      if( ( cacheCount - 1 ) == index )
        {
        //
        listEntry = cachedList[ i ];
        (void) NW_Mem_memcpy( entry, &(listEntry->iEntry), sizeof *entry);
        return KBrsrSuccess;
        }
      }  
  /* for convenience */
  list = (CLmgrPropertyList*)(thisObj->map);

  /* use byte-wise copy to get the unaligned data into the client's variable */
    if( index < ( list->Count() + cacheCount ) )
      {
      listEntry = list->At( index - cacheCount );
      NW_ASSERT(listEntry != NULL);
      (void) NW_Mem_memcpy (entry, &(listEntry->iEntry), sizeof *entry);
      status = KBrsrSuccess;
      }
    }
  return status;
  }

/* ------------------------------------------------------------------------- */
TBrowserStatusCode 
NW_LMgr_SimplePropertyList_GetNameAt (const NW_LMgr_SimplePropertyList_t* thisObj,
                                      TInt index,
                                      NW_LMgr_PropertyName_t* name)
{
  CLmgrPropertyList* list;
  TBrowserStatusCode status = KBrsrFailure;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj,
                                     &NW_LMgr_SimplePropertyList_Class));
  NW_ASSERT (name != NULL);
  NW_ASSERT (thisObj->map != NULL);

  TInt cacheCount = 0;
  CLmgrPropertyEntry* listEntry;
  CLmgrPropertyEntry** cachedList = (CLmgrPropertyEntry**)(thisObj->iCachedProperty);
  //
  if( name )
    {
    for( TInt i = 0; i < NW_CSS_CachedProperty_Num; i++ )
      {
      if( cachedList[ i ] )
        {      
        cacheCount++;
        }
      // 
      if( ( cacheCount - 1 ) == index )
        {
        //
        listEntry = cachedList[ i ];
        *name = listEntry->iPropName;
        return KBrsrSuccess;
        }
      }  
  /* for convenience */
  list = (CLmgrPropertyList*)(thisObj->map);

  /* use byte-wise copy to get the unaligned data into the client's variable */
    if( index < ( list->Count() + cacheCount ) )
    {
      listEntry = list->At( index - cacheCount );
      NW_ASSERT(listEntry != NULL);
      *name = listEntry->iPropName;
      status = KBrsrSuccess;
    }
  }
  return status;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode 
NW_LMgr_SimplePropertyList_CopyEntireList(NW_LMgr_SimplePropertyList_t* aOldPropList,
                                          NW_LMgr_SimplePropertyList_t** aNewPropList)
  {
  // this function does not support cached properties yet
  NW_LMgr_SimplePropertyList_t* newPropertyList = NULL;

  NW_TRY( status )
    {
    if( aOldPropList ) 
      {
      TInt i;
      newPropertyList = NW_LMgr_SimplePropertyList_New( 2, 2 );
      NW_THROW_OOM_ON_NULL( newPropertyList, status );

      // loop through and copy individual properties
      TInt count = NW_LMgr_SimplePropertyList_Count( aOldPropList );
      for( i = 0; i < count; i++)
	  {
		  NW_LMgr_PropertyName_t key;
		  NW_LMgr_PropertyList_Entry_t entry;
		  
		  status = NW_LMgr_SimplePropertyList_GetNameAt(aOldPropList, i, &key);
		  if (status == KBrsrSuccess)
		  {
			  // get the text property name 
			  (void)TCSSPropertyTable::GetPropEntryByToken(key);
			  
			  status = NW_LMgr_PropertyList_Get (aOldPropList, key, &entry);
			  
			  if ((entry.type & NW_CSS_ValueType_Mask) >= NW_CSS_ValueType_Object)
			  {
				  entry.type |= NW_CSS_ValueType_Copy;
			  }
			  
			  // Only print property if status == success
			  if (status == KBrsrSuccess)
			  {
				  status = NW_LMgr_PropertyList_Set (newPropertyList, key, &entry);
				  NW_THROW_ON_ERROR (status);
			  }
		  }
	  }
	}
    *aNewPropList = newPropertyList;
    }
  NW_CATCH (status)
    {
    if (newPropertyList)
      {
      NW_Object_Delete(newPropertyList);
      }
    }
  NW_FINALLY
    {
    return status;  
    }
  NW_END_TRY
  }

/* ------------------------------------------------------------------------- */
TBrowserStatusCode 
NW_LMgr_SimplePropertyList_MapLookup( NW_LMgr_SimplePropertyList_t* thisObj,
                                      NW_LMgr_PropertyName_t key, 
                                      NW_LMgr_PropertyList_Entry_t* entry )
  {
  /* fetch a pointer to the value from the map */
  CLmgrPropertyEntry tempEntry;
  CLmgrPropertyEntry* listEntry = NULL;
  //
  if( NW_LMgr_SimplePropertyList_IsCachedEntry( key ) )
    {
    CLmgrPropertyEntry** cachedList = (CLmgrPropertyEntry**)(thisObj->iCachedProperty);
    // cached property
    key = (NW_LMgr_PropertyName_t)( key > NW_CSS_PropInherit ? ( key - NW_CSS_PropInherit ) : key );

    listEntry = cachedList[ key - 1 ];
    if( !listEntry )
      {
      return KBrsrNotFound;
      }
    }
  else
    {
    CLmgrPropertyList* list;
    tempEntry.iPropName = key;
    TInt index;
    TNumKeyArrayPtr matchkey(_FOFF(CLmgrPropertyEntry, iPropName), ECmpTUint16);
    list = (CLmgrPropertyList*)(thisObj->map);

    if (list->FindIsq(&tempEntry, matchkey, index) != 0)
      {
      return KBrsrNotFound;
      }
    listEntry = list->At(index);
    }

  /* use byte-wise copy to get the unaligned data into the client's variable */
  NW_ASSERT(listEntry != NULL);
  (void) NW_Mem_memcpy (entry, &(listEntry->iEntry), sizeof *entry);
  tempEntry.iEntry.value.object = 0;

  /* successful completion */
  return KBrsrSuccess;
  }

/* ------------------------------------------------------------------------- */
TBrowserStatusCode 
NW_LMgr_SimplePropertyList_RemoveWithoutDelete (NW_LMgr_SimplePropertyList_t* thisObj,
                                                NW_LMgr_PropertyName_t key )
{
  CLmgrPropertyList* list;

  /* for convenience */
  list = (CLmgrPropertyList*)(thisObj->map);

  if( NW_LMgr_SimplePropertyList_IsCachedEntry( key ) )
    {
    CLmgrPropertyEntry** cachedList = (CLmgrPropertyEntry**)(thisObj->iCachedProperty);
    key = (NW_LMgr_PropertyName_t)( key > NW_CSS_PropInherit ? ( key - NW_CSS_PropInherit ) : key );
    // remove property
    delete cachedList[ key - 1 ];
    cachedList[ key - 1 ] = NULL;
    }
  else
    {
    /* remove the property */
    CLmgrPropertyEntry newEntry;
    newEntry.iPropName = key;
    TInt index;
    TNumKeyArrayPtr matchkey(_FOFF(CLmgrPropertyEntry, iPropName), ECmpTUint16);
    if (list->FindIsq(&newEntry, matchkey, index) == 0)
      {
      list->Delete(index);
      list->Compress();
      }
    }
    return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_LMgr_SimplePropertyList_t*
NW_LMgr_SimplePropertyList_New (NW_ADT_Vector_Metric_t capacity,
                                NW_ADT_Vector_Metric_t increment)
{
  return (NW_LMgr_SimplePropertyList_t*)
    NW_Object_New (&NW_LMgr_SimplePropertyList_Class,
                   sizeof (NW_LMgr_PropertyName_t), (NW_Uint32)capacity, (NW_Uint32)increment);
}
