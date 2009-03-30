/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contains all CSS styles
*
*/



// INCLUDE FILES
#include <e32def.h>  // First to avoid NULL redefine warning (no #ifndef NULL).
#include <f32file.h>
#include "nw_css_defaultstylesheet.h"
#include <nwx_assert.h>
#include "nwx_settings.h"
#include "nwx_ctx.h"
#include "nwx_logger.h"
#include "CSSReader.h"
#include "CSSPropVal.h"
#include "CSSDeclListHandle.h"
#include "CSSVariableStyleSheet.h"
#include "nw_xhtml_xhtml_1_0_tokens.h"

#include <bldvariant.hrh>
#include <data_caging_path_literals.hrh>

// ================================= CONSTANTS ====================================

#define ASCII_LEFT_BRACE          '{'
#define ASCII_RIGHT_BRACE         '}'
#define ASCII_NULL                '\0'

static const NW_Ucs2 CSS_CURLY_BRACES[] = {ASCII_LEFT_BRACE, ASCII_RIGHT_BRACE, ASCII_NULL};

/* User Supplied Style Sheet
 * The user can provide a style sheet that will override the operator
 * provided one.
 * The user provided sheet is stored in the emulator at:
 *      K:\epoc32\wins\c\System\data
 * The operator provided sheet is stored in the emulator at:
 *      K:\epoc32\release\wins\udeb\z\system\data
 *
 * Note that since 3.0, these location will change as defined in
 * data_caging_path_literals.hrh
 */
void CCSSVariableStyleSheet::ConstructCSSFileName()
{
   // Literals for files ( drive, directory, filename(s) )
	_LIT(KZDrive,"z:");
	_LIT(KCDrive,"c:");

	// _LIT(KDC_APP_RESOURCE_DIR,"\\resource\\apps\\uiresourcefiles\\");	//	Application resource (.rsc)
	// _LIT(KDC_RESOURCE_FILES_DIR,"\\resource\\");				// 	Other resource files

	_LIT(KFileName,"oem.css");

	CSS_USER_SUPPLIED_FILE.Zero();
    CSS_USER_SUPPLIED_FILE += KCDrive;
    CSS_USER_SUPPLIED_FILE += KDC_RESOURCE_FILES_DIR;
    CSS_USER_SUPPLIED_FILE += KFileName;
    CSS_USER_SUPPLIED_FILE.ZeroTerminate();

	CSS_OPERATOR_SUPPLIED_FILE.Zero();
    CSS_OPERATOR_SUPPLIED_FILE += KZDrive;
    CSS_OPERATOR_SUPPLIED_FILE += KDC_RESOURCE_FILES_DIR;
    CSS_OPERATOR_SUPPLIED_FILE += KFileName;
    CSS_OPERATOR_SUPPLIED_FILE.ZeroTerminate();
}

#ifdef _DEBUG
static const NW_Ucs2 CSS_INVALID_TOKEN[] = {'*','E','R','R','O','R','*','\0'};
static const NW_Ucs2 CSS_VARIABLE[] = {'v','a','r','\0'};
static const NW_Ucs2 CSS_DEFAULT[] = {'d','e','f','\0'};
static const NW_Ucs2 CSS_HEX_FORMAT[] = {'0','x','%','6','.','6','X','\0'};
static const NW_Ucs2 CSS_FLOAT_FORMAT[] = {'%','1','0','f','\0'};

#define NW_XHTML_1_0_INDEX 1
#endif /* _DEBUG */

// ============================ CONSTRUCTOR/DESTRUCTOR ===============================

void CCSSVariableStyleSheet::operator=(const CCSSVariableStyleSheet& aObj){
NW_REQUIRED_PARAM( &aObj );
}

// -----------------------------------------------------------------------------
// Symbian two phase constructor.
// -----------------------------------------------------------------------------

void CCSSVariableStyleSheet::ConstructL()
{
  HBufC8* buffer = NULL;

  //Initially nothing is loaded.
  iStaticCSSLoaded = EFalse;
  iVariableCSSLoaded = EFalse;

  //Create the lists. Pick a course granularity, they will be compressed
  //after completion. Minimize churning as we add components.
  iPropList = new (ELeave) CCSSPropEntryList(10);
  iStringList = new (ELeave) CCSSStringEntryList(5);
  ConstructCSSFileName();

  buffer = ReadVariableCSSFile();
  AddDefaultStyleStrings();
  AddVariableStyles(buffer);
  AddDefaultStyleProperties();
  delete buffer;
#ifdef _DEBUG
  DumpVariableStyleSheet();
#endif /* _DEBUG */
}

CCSSVariableStyleSheet* CCSSVariableStyleSheet::NewL()
  {
  CCSSVariableStyleSheet* self = new( ELeave ) CCSSVariableStyleSheet();
  CleanupStack::PushL( self );
  self->ConstructL();
  CleanupStack::Pop();
  return self;
  }


// -----------------------------------------------------------------------------
// Destructor.
// Close and delete the lists. The destructors of each of the objects
// will be called implicitly as a result.
// -----------------------------------------------------------------------------

CCSSVariableStyleSheet::~CCSSVariableStyleSheet()
{
  TInt count;
  TInt index;

  //Walk through the RArray. For each entry, if it references a dynamic
  //entry (e.g. variable) delete the referenced object. Otherwise don't
  //do anything with the entry itself. Then close and delete the array.
  if (iPropList != NULL){
    count = iPropList->Count();
    for(index=0; index<count; index++){
      TCSSPropEntry& propEntry = iPropList->operator[](index);
      if (propEntry.variableEntry == TRUE){
        delete propEntry.entry->pattern;
        delete propEntry.entry;
      }
    }
    iPropList->Close();
    delete iPropList;
  }

  if (iStringList != NULL){
    count = iStringList->Count();
    for(index=0; index<count; index++){
      TCSSStringEntry& stringEntry = iStringList->operator[](index);
      if (stringEntry.variableEntry == TRUE){
        delete stringEntry.entry;
      }
    }
    iStringList->Close();
    delete iStringList;
  }
}


// ============================ PUBLIC METHODS ===============================

// -----------------------------------------------------------------------------
// CCSSVariableStyleSheet::CreateSingleton
//
// Creates the CCSSVariableStyleSheet singleton.
// -----------------------------------------------------------------------------

CCSSVariableStyleSheet* CCSSVariableStyleSheet::CreateSingleton()
{
  CCSSVariableStyleSheet* thisObj = CCSSVariableStyleSheet::GetSingleton();
  if (thisObj == NULL){
    TRAPD(ret, thisObj = CCSSVariableStyleSheet::NewL());
    if (ret != KErrNone)
      return NULL;
    TRAP(ret, NW_Ctx_Set(NW_CTX_VARIABLE_CSS, 0, (void*)thisObj));
    if (ret != KErrNone)
      return NULL;
  }
  return thisObj;
}


// -----------------------------------------------------------------------------
// CCSSVariableStyleSheet::DeleteSingleton
//
// Deletes the CCSSVariableStyleSheet singleton.
// -----------------------------------------------------------------------------

void CCSSVariableStyleSheet::DeleteSingleton()
{
  CCSSVariableStyleSheet* thisObj = CCSSVariableStyleSheet::GetSingleton();
  if (thisObj != NULL){
    delete thisObj;
    NW_Ctx_Set(NW_CTX_VARIABLE_CSS, 0, NULL);
  }
}


// -----------------------------------------------------------------------------
// CCSSVariableStyleSheet::GetSingleton
//
// Get the CCSSVariableStyleSheet singleton. If it doesn't already exist then returns null.
// -----------------------------------------------------------------------------

CCSSVariableStyleSheet* CCSSVariableStyleSheet::GetSingleton()
{
  CCSSVariableStyleSheet* thisObj = (CCSSVariableStyleSheet*) NW_Ctx_Get(NW_CTX_VARIABLE_CSS, 0);
  return thisObj;
}


// -----------------------------------------------------------------------------
// CCSSVariableStyleSheet::ApplyStyles
//
// Applies the combined default and variable styles.
// -----------------------------------------------------------------------------

TBrowserStatusCode
  CCSSVariableStyleSheet::ApplyStyles(NW_DOM_ElementNode_t* aElementNode,
                                     NW_LMgr_Box_t* aBox,
                                     NW_WBXML_Dictionary_t* aDictionary,
                                     const NW_Evt_Event_t* aEvent,
                                     NW_HED_DocumentNode_t* aDocNode,
                                     NW_HED_DomHelper_t* aDomHelper)
{
  const NW_CSS_DefaultStyleSheet_PropEntry_t* propertyVal;
  TInt index, numPropertyVals;
  NW_Uint16 tagToken;
  const TCSSPropertyTablePropEntry* propEntry;
  TBrowserStatusCode status = KBrsrSuccess;
  NW_LMgr_Property_t prop;
  CCSSNodeData nodeData;
  TCSSPropEntry startEntry;

  tagToken = NW_DOM_ElementNode_getTagToken(aElementNode);
  numPropertyVals = iPropList->Count();

  //The entries are in order based on a key comprised of the element token
  //and the element name. The old algorithm searched through the array
  //until it found the first element token that matched. This algorithm lets
  //the binary search algorithm of the array do the work of finding the first
  //element that matches, or at least where to start looking. The looking stops
  //when a key is found that is greater than the token OR the end of the array
  //is reached.

  //Create a dummy entry with a search key that starts with the first
  //entry (if it exists) for this token and a default name of 0. If no
  //entry exists we will start at the place it WOULD exist. The search
  //will fail quickly in that case.

  startEntry.key = (tagToken << 16) | 0;
  iPropList->FindInUnsignedKeyOrder(startEntry, index);

  for(; index < numPropertyVals; index++){
    NW_Byte *pattern;
    propertyVal = iPropList->operator[](index).entry;

    if (propertyVal->elementToken < tagToken)
      continue;

    if (propertyVal->elementToken > tagToken)
      return KBrsrSuccess;

    /* match pattern - if pattern matches apply style */
    pattern = propertyVal->pattern;
    if (pattern != NULL){
      NW_Uint32 numBytes;
      CCSSPatternMatcher patternMatcher(aDictionary, aDocNode, aDomHelper);
      TBool matched = EFalse;

      (void) NW_String_charBuffGetLength(pattern, HTTP_us_ascii, &numBytes);
      TCSSParser parser(pattern, numBytes, HTTP_us_ascii);

      TRAPD(ret, matched = patternMatcher.MatchPatternLazyL(aElementNode, &parser, pattern, aEvent, &nodeData));
      if (ret == KErrNoMemory)
        return KBrsrOutOfMemory;

      if (!matched)
        continue;
    }

    propEntry = TCSSPropertyTable::GetPropEntryByToken(propertyVal->name);
    if (propEntry == NULL)
      continue;

    status = InitProperty(propEntry, &prop, aBox, propertyVal->val, propertyVal->valType);
    if (status == KBrsrOutOfMemory)
      return status;
    if (status != KBrsrSuccess)
      continue;

    /*
     Rules are arranged by decreasing specificity. If a property is
     already present, it can be replaced only if the other property is
     marked important. All defaults are NOT important.
    */
	TInt ret;
    TRAP(ret, TCSSDeclListHandle::ApplyPropertyL(propEntry->tokenName, &prop, 1,
                                                  EFalse, NULL, aBox, ETrue));

    if ((prop.type == NW_CSS_ValueType_Text) ||(prop.type == NW_CSS_ValueType_Image)){
      NW_Object_Delete(prop.value.object);
    }
  }
  return status;
}


// ============================ PRIVATE METHODS ===============================


// -----------------------------------------------------------------------------
// CCSSVariableStyleSheet::ReadVariableCSSFile
//
// Loads the file into a buffer.
// -----------------------------------------------------------------------------

HBufC8*
CCSSVariableStyleSheet::ReadVariableCSSFile()
{
  HBufC8* buffer = NULL;

  if ((buffer = ReadFile(CSS_USER_SUPPLIED_FILE)) == NULL)
    {
      buffer = ReadFile(CSS_OPERATOR_SUPPLIED_FILE);
    }

  return buffer;
}


//Returns a HBufC8 with the contents of aFile
HBufC8*
CCSSVariableStyleSheet::ReadFile(const TDesC& aFile)
{
  RFs fs;
  RFile file;
  TInt size = 0;
  HBufC8* contents = NULL;

  if (fs.Connect() != KErrNone )
    return contents;

  if (file.Open(fs, aFile, EFileShareAny | EFileRead) != KErrNone )
    {
      fs.Close();
      return contents;
    }

  //The file exists, allocate enough space for the NULL termination later.
  file.Size(size);
  if ((size != 0) && ((contents = HBufC8::New(size + 4)) != NULL))
    {
      //We have opened the file and it has a non-zero size and we have allocated memory
      TPtr8 contentsPtr(contents->Des());
      if (file.Read(contentsPtr, size)!= KErrNone)
        {
          delete contents;
          contents = NULL;
        }
    }

  file.Close();
  fs.Close();
  return contents;
}


// -----------------------------------------------------------------------------
// CCSSVariableStyleSheet::AddVariableStyles
//
// Adds the variable styles to the default list.
// -----------------------------------------------------------------------------

TBool CCSSVariableStyleSheet::AddVariableStyles(HBufC8* aBuffer)
{
  iVariableCSSLoaded = EFalse;
  if (aBuffer != NULL)
    {
      iVariableCSSLoaded = ProcessBuffer(aBuffer->Ptr(), aBuffer->Length());
    }
  else
    {
      iVariableCSSLoaded = EFalse;
    }
  return iVariableCSSLoaded;
}


// -----------------------------------------------------------------------------
// CCSSVariableStyleSheet::ProcessBuffer
//
// Adds the variable styles to the default list.
// -----------------------------------------------------------------------------
TBool CCSSVariableStyleSheet::ProcessBuffer(const TUint8* aBuffer, TUint32 aLength)
{
  TInt index = 0;
  TInt size = 0;
  TUint32 charset = HTTP_us_ascii;
  CCSSRuleList* ruleList = NULL;
  CCSSRuleListElementEntry* entry = NULL;

  TCSSParser parser((TUint8*)aBuffer, aLength, charset) ;

  TRAPD(ret, ruleList = CCSSRuleList::NewL(NULL));
  if (ret != KErrNone)
    return EFalse;

  ruleList->SetDictionary((NW_WBXML_Dictionary_t *) &NW_XHTML_WBXMLDictionary);

  ruleList->ProcessBuffer((TText8*)aBuffer, aLength, charset, ETrue); // delete buffer with ruleList
  size = ruleList->GetSize();

  for (index = 0; index < size; index++)
    {
      entry = ruleList->GetElementRulesByIndex(index);
      if (entry != NULL)
        {
          UpdateList(&parser, entry->iRules, entry->iElementToken);
        }
    }
  delete ruleList;
  return ETrue;
}

// -----------------------------------------------------------------------------
// CCSSVariableStyleSheet::AddDefaultStyleStrings
//
// Adds the default style string to the default list.
// -----------------------------------------------------------------------------

void CCSSVariableStyleSheet::AddDefaultStyleStrings()
{
  NW_Uint32 numVals;
  NW_Ucs2** stringVals;
  const NW_CSS_DefaultStyleSheet_t* defaultCSS;
  NW_Uint32 index;
  TCSSStringEntry stringEntry;
  NW_Ucs2* stringVal;

  defaultCSS = &NW_CSS_DefaultStyleSheet;
  numVals = NW_CSS_DefaultStyleSheet_GetClassPart(defaultCSS).numStringVals;
  stringVals = (NW_Ucs2**)(NW_CSS_DefaultStyleSheet_GetClassPart(defaultCSS).stringVals);

  for(index = 0; index < numVals; index++)
    {
      // Decode all the data in this structure and add to the array.  No
      // duplicates are allowed. The variable stuff was already added so
      // this is a lower priority.
      stringEntry.variableEntry = EFalse;
      stringVal = stringVals[index];
      stringEntry.entry = stringVal;
      iStringList->Append(stringEntry);
    }

  iStaticCSSLoaded = ETrue;
}


// -----------------------------------------------------------------------------
// CCSSVariableStyleSheet::AddDefaultStyleProps
//
// Adds the default style properties to the default list.
// -----------------------------------------------------------------------------

void CCSSVariableStyleSheet::AddDefaultStyleProperties()
{
  NW_Uint32 numVals;
  NW_CSS_DefaultStyleSheet_PropEntry_s* propertyVals;
  const NW_CSS_DefaultStyleSheet_t* defaultCSS;
  NW_Uint32 index;
  TCSSPropEntry propEntry;

  defaultCSS = &NW_CSS_DefaultStyleSheet;
  numVals = NW_CSS_DefaultStyleSheet_GetClassPart(defaultCSS).numPropertyVals;
  propertyVals = (NW_CSS_DefaultStyleSheet_PropEntry_s*)(NW_CSS_DefaultStyleSheet_GetClassPart(defaultCSS).propertyVals);

  for(index = 0; index < numVals; index++)
    {
      // Decode all the data in this structure and add to the array.  No
      // duplicates are allowed. The variable stuff was already added so
      // this is a lower priority.

      propEntry.key = (propertyVals[index].elementToken <<16) | (propertyVals[index].name);
      propEntry.variableEntry = EFalse;
      propEntry.entry = &(propertyVals[index]);

      //Insert the property. If this was a duplicate we ignore it. Ignore return value
      InsertProperty(propEntry);
    }
  iStaticCSSLoaded = ETrue;
}


// -----------------------------------------------------------------------------
// CCSSVariableStyleSheet::InsertProperty
//
// Applies the styles to the property.
// -----------------------------------------------------------------------------
TInt CCSSVariableStyleSheet::InsertProperty(TCSSPropEntry aPropEntry)
{
  TCSSPropEntry compareEntry;
  TInt searchIndex;
  if (iPropList->FindInUnsignedKeyOrder(aPropEntry, searchIndex) == KErrNone){
    //A duplicate primary key was found. Check secondary key
    //(e.g. pattern) to see if this is really a duplicate. Only insert
    //if the pattern specified matches none of the existing ones.

    //Multiple entries with the same key exist. Walk back until we find
    //the first one. The binary search that returned this index doesn't
    //always return the first entry in the list of duplicate keys.
    while (searchIndex >= 0){
      compareEntry = iPropList->operator[](searchIndex);
      if (compareEntry.key != aPropEntry.key){
        searchIndex++;
        break;
      }
      if (searchIndex == 0){
        break;
      }
      searchIndex--;
    }

    //Search until primary keys no longer match.
    while (searchIndex < iPropList->Count()){
      compareEntry = iPropList->operator[](searchIndex);
      if (compareEntry.key != aPropEntry.key){
        //duplicate not found in primary key so no more duplicates.
        //Add to list.
        break;
      }

      //We have matching primary keys. Check secondary. Watch out for nulls.
      //This messy because NW_Asc_strcmp doesn't handle nulls.
      if ((compareEntry.entry->pattern == NULL)&&(aPropEntry.entry->pattern == NULL) ||
          (compareEntry.entry->pattern != NULL)&&(aPropEntry.entry->pattern != NULL) ){
        //Either both or neither null
        if ((compareEntry.entry->pattern == aPropEntry.entry->pattern) ||
            !NW_Asc_strcmp((char*)(compareEntry.entry->pattern), (char*)(aPropEntry.entry->pattern))){
          //Secondary match found. Either both pointers are null or the
          //strings are the same. This is really a duplicate.
          //Don't add to list.
          searchIndex = -1;
          break;
        }
      }
      //If you got here than primary keys matched but pattern didn't match.
      //Keep checking.
      searchIndex++;
    }
    //If you got here then you hit the end of the list while primary
    //keys matched but pattern didn't. Add it at the end which is
    //where the index should be pointing.
  }

  //Insert at the calculated index if a valid index. This is not a duplicate.
  if (searchIndex != -1){
    iPropList->Insert(aPropEntry, searchIndex);
  }
  return searchIndex;
}


// -----------------------------------------------------------------------------
// CCSSVariableStyleSheet::InitProperty
//
// Applies the styles to the property.
// -----------------------------------------------------------------------------

TBrowserStatusCode
CCSSVariableStyleSheet::InitProperty(const TCSSPropertyTablePropEntry* aPropEntry,
                                     NW_LMgr_Property_t* aProp,
                                     NW_LMgr_Box_t* aBox,
                                     NW_Float32 aVal,
                                     NW_Uint8 aValType)
{
  aProp->type = aValType;

 switch(aValType)
   {
   case NW_CSS_ValueType_Integer:
   case NW_CSS_ValueType_Px:
   case NW_CSS_ValueType_Color:
     aProp->value.integer = (NW_Int32)aVal;
     break;
   case NW_CSS_ValueType_Number:
   case NW_CSS_ValueType_In:
   case NW_CSS_ValueType_Cm:
   case NW_CSS_ValueType_Mm:
   case NW_CSS_ValueType_Pt:
   case NW_CSS_ValueType_Pc:
   case NW_CSS_ValueType_Em:
   case NW_CSS_ValueType_Ex:
   case NW_CSS_ValueType_Percentage:
     aProp->value.decimal = aVal;
     break;
   case NW_CSS_ValueType_Token:
     {
     TBool set = TCSSPropVal::SetTokenVal( aPropEntry->tokenName, aProp, (NW_Uint32) (NW_Int32) aVal, aBox);
     return set? KBrsrSuccess:KBrsrFailure;
     }
   case NW_CSS_ValueType_Text:
     {
     const NW_Ucs2* ucs2Str;
     NW_Text_Length_t length;

     if (aVal >= iStringList->Count())
       return KBrsrFailure;

     ucs2Str = iStringList->operator[]((NW_Int32)aVal).entry;
     length = (NW_Text_Length_t)(NW_Str_Strlen(ucs2Str));

     aProp->value.object = NW_Text_UCS2_New ((NW_Ucs2*) ucs2Str, length, 0);
     if (aProp->value.object == NULL)
       return KBrsrOutOfMemory;

     break;
     }
   default:
     return KBrsrFailure;
 }

 return KBrsrSuccess;
}


// -----------------------------------------------------------------------------
// CCSSVariableStyleSheet::UpdateList
//
//
// -----------------------------------------------------------------------------
TBrowserStatusCode
CCSSVariableStyleSheet::UpdateList(TCSSParser* aParser,
                                   CCSSRules* aRuleList,
                                   TUint16 aElementToken)
{
  TInt index;
  TInt size;
  TBrowserStatusCode status;

  size = aRuleList->Count();
  for (index = 0; index < size; index++)
    {
      CCSSSelector* entry;
      TUint32 specificity;
      TText16* patternStr;
      char* asciiPatternStr;
      TText8* selectorEndPosition;
      TCSSSimpleSelector simpleSelector;
      TCSSReaderUnit unit;
      TText8* declarationList;

      entry = aRuleList->At(index);
      aParser->SetPosition(entry->iSelector);
      specificity = 0;
      patternStr = NULL;
      asciiPatternStr = NULL;
      simpleSelector.specificity = 0;

      do{
        specificity += simpleSelector.specificity;
        status = aParser->SimpleSelectorListIterate(&simpleSelector);
      }while (status == KBrsrIterateMore);

      if (status != KBrsrIterateDone)
        continue;

      selectorEndPosition = aParser->GetPosition();
      if (specificity > 1)
        {
          unit.Init(entry->iSelector, selectorEndPosition - entry->iSelector, 0);
          patternStr = unit.GetUnicode(aParser->GetEncoding());

          if (patternStr != NULL)
            {
              //Add the curly braces back into the pattern. These are
              //required for parsing later on. Revove whitespace before adding curly
              //braces. If you decide to remove leading whitespace then save the
              //pointer so you can delete it later.
              NW_Str_TrimInPlace(patternStr, NW_Str_End);
              TText16* tempStr = NW_Str_Newcat(patternStr, CSS_CURLY_BRACES);
              if (tempStr != NULL)
                {
                  asciiPatternStr = NW_Str_CvtToAscii(tempStr);
                  delete tempStr;
                }
              delete patternStr;
            }
        }

      (aParser->GetReader())->GoToToken(LEFT_BRACE, &declarationList);
      NW_ASSERT(declarationList);
      UpdateEntry(aParser, aElementToken, asciiPatternStr);
    }
  return KBrsrSuccess;
}

// -----------------------------------------------------------------------------
// CCSSVariableStyleSheet::UpdateEntry
//
//
// -----------------------------------------------------------------------------
TBrowserStatusCode
CCSSVariableStyleSheet::UpdateEntry(TCSSParser* aParser,
                                   TUint16 aElementToken,
                                   char* aAsciiPatternStr)
{
  TBool important;
  TCSSReaderUnit propertyName;
  TCSSPropertyVal propertyVal;
  TUint8 numVals;
  TCSSPropEntry entryWrapper;
  TCSSStringEntry stringWrapper;
  NW_CSS_DefaultStyleSheet_PropEntry_s* entry;
  const TCSSPropertyTablePropEntry* propEntry;
  NW_LMgr_PropertyValue_t val;
  NW_LMgr_PropertyValueType_t valType;
  TText16* ucs2Str = NULL;
  TText16* valStorage = NULL;
  TUint32 encoding;

  while (aParser->DeclarationListIterate(&propertyName, &propertyVal, &numVals, 1, &important)
         == KBrsrIterateMore)
    {
      if (propertyName.iLength == 0)
        continue;

      encoding = aParser->GetEncoding();
      ucs2Str = propertyName.GetUnicode(encoding);
      if ((ucs2Str == NULL) || (propertyVal.type == -1))
        continue;

      propEntry = TCSSPropertyTable::GetPropertyEntry(ucs2Str);
      delete(ucs2Str);
      if (propEntry == NULL)
        continue;

      valStorage = propertyVal.val.GetUnicode(encoding);
      if (valStorage == NULL)
        continue;

      valType = SetPropertyVal(aParser, propEntry, &val, &propertyVal, valStorage);
      if (valType == 0)
        continue;

      /* create a new entry to and initialize it */
      entry = new NW_CSS_DefaultStyleSheet_PropEntry_s;
      entryWrapper.variableEntry = ETrue;
      entryWrapper.entry = entry;
      entryWrapper.key = (aElementToken <<16) | propEntry->tokenName;

      entry->elementToken = aElementToken;
      entry->pattern = (unsigned char *)aAsciiPatternStr;
      entry->name = propEntry->tokenName;
      entry->valType = valType;
      switch (valType)
        {
        case NW_CSS_ValueType_Percentage:
        case NW_CSS_ValueType_Em:
        case NW_CSS_ValueType_In:
        case NW_CSS_ValueType_Cm:
        case NW_CSS_ValueType_Mm:
        case NW_CSS_ValueType_Pt:
        case NW_CSS_ValueType_Pc:
          entry->val = (NW_Float32)val.decimal;
          break;
        case NW_CSS_ValueType_Token:
          entry->val = (NW_Float32)val.token;
          break;
        case NW_CSS_ValueType_Px:
        case NW_CSS_ValueType_Integer:
        case NW_CSS_ValueType_Ex:
        case NW_CSS_ValueType_Color:
          entry->val = (NW_Float32)val.integer;
          break;
        case NW_CSS_ValueType_Text:
          {
            stringWrapper.variableEntry = ETrue;
            stringWrapper.entry = valStorage;
            valStorage = NULL;
            iStringList->Append(stringWrapper);
            //Index of this entry is count - 1
            entry->val = (NW_Float32)(iStringList->Count() - 1);
            break;
          }
        default:
          entry->val = (NW_Float32)val.integer;
          break;
        }
      delete valStorage;

      if (InsertProperty(entryWrapper) == -1)
        {
          //This was a duplicate. It wasn't added to list. Free memory.
          delete entry->pattern;
          delete entry;
        }
    }


  return KBrsrSuccess;
}

// -----------------------------------------------------------------------------
// CCSSVariableStyleSheet::SetPropertyVal
//

NW_LMgr_PropertyValueType_t
CCSSVariableStyleSheet::SetPropertyVal(TCSSParser* aParser,
                                       const TCSSPropertyTablePropEntry* aPropEntry,
                                       NW_LMgr_PropertyValue_t* aVal,
                                       TCSSPropertyVal* aPropertyVal,
                                       TText16* aValStorage)
{
  NW_LMgr_PropertyValueToken_t token;
  TBool result = EFalse;
  NW_Float32 v;
  TText16* endPtr = NULL;


  if ((aPropertyVal->type == RGB) || (aPropertyVal->type == HASH)){
    //COLOR
     NW_LMgr_Property_t tmpVal;
     tmpVal.value.integer = 0; //remove warning
     tmpVal.type = 0;

     TRAPD( err, result = TCSSPropVal::SetColorL( aParser, &tmpVal, aPropertyVal, aValStorage));
     if ((err != KErrNone) || !result)
       return 0;

     aVal->integer = tmpVal.value.integer;
     return NW_CSS_ValueType_Color;
  }

  if (aPropertyVal->type == IDENTIFIER){
     //TOKEN
     if(aPropEntry->tokenName != NW_CSS_Prop_fontFamily) {
       if (TCSSPropertyTable::GetPropertyValToken(aValStorage, &token)){
         aVal->token = token;
         return NW_CSS_ValueType_Token;
       }
     }
  }

  if ((aPropertyVal->type == IDENTIFIER) || (aPropertyVal->type == STRING)){
    //TEXT
    /* If the identifier is not recognized as a token OR it is a real
       string interpret is as a string */
    return NW_CSS_ValueType_Text;
  }

  //NUMBER - If not one of the above it must be a number.

  if ( NW_Str_CvtToFloat32(aValStorage, &endPtr, &v) != KBrsrSuccess)
    return 0;

  switch (aPropertyVal->type)
    {
    case NUMBER:
      /* This code is the point that default units of measure are
         applied. This is a convenience. According to the spec. only the
         value zero can omit the units. However, defaults are usually
         inserted. */
      switch (aPropEntry->tokenName)
        {
          // PIXEL Defaults
		case NW_CSS_Prop_borderWidth:
        case NW_CSS_Prop_topBorderWidth:
        case NW_CSS_Prop_bottomBorderWidth:
        case NW_CSS_Prop_leftBorderWidth:
        case NW_CSS_Prop_rightBorderWidth:

        case NW_CSS_Prop_padding:
        case NW_CSS_Prop_topPadding:
        case NW_CSS_Prop_bottomPadding:
        case NW_CSS_Prop_leftPadding:
        case NW_CSS_Prop_rightPadding:

		case NW_CSS_Prop_margin:
		case NW_CSS_Prop_topMargin:
        case NW_CSS_Prop_bottomMargin:
        case NW_CSS_Prop_leftMargin:
        case NW_CSS_Prop_rightMargin:
          aVal->integer = (TInt32)v;
          return NW_CSS_ValueType_Px;

          //INTEGER defaults
        case NW_CSS_Prop_font:
        case NW_CSS_Prop_fontSize:
        case NW_CSS_Prop_fontWeight:
        case NW_CSS_Prop_marqueeSpeed:
        case NW_CSS_Prop_marqueeLoop:
        default:
          aVal->integer = (TInt32)v;
          return NW_CSS_ValueType_Integer;
        }

    case PERCENTAGE:
      aVal->decimal = v;
      return NW_CSS_ValueType_Percentage;

    case EMS:
      aVal->decimal = v;
      return NW_CSS_ValueType_Em;

    case EXS:
      aVal->decimal = v;
      return NW_CSS_ValueType_Ex;

    case PXS:
      aVal->integer =(TInt32)v;
      return NW_CSS_ValueType_Px;

    case INS:
      aVal->decimal = v;
      return NW_CSS_ValueType_In;

    case CMS:
      aVal->decimal = v;
      return NW_CSS_ValueType_Cm;

    case MMS:
      aVal->decimal = v;
      return NW_CSS_ValueType_Mm;

    case PCS:
      aVal->decimal = v;
      return NW_CSS_ValueType_Pc;

    case PTS:
      aVal->decimal = v;
      return NW_CSS_ValueType_Pt;

      //default:
      //return 0;
    }
  return 0;
}


// -----------------------------------------------------------------------------
// CCSSVariableStyleSheet::DumpVariableStyleSheet
//
// -----------------------------------------------------------------------------
#ifdef _DEBUG

TBrowserStatusCode
CCSSVariableStyleSheet::DumpVariableStyleSheet()
{
  TInt count;
  TInt index;

  NW_LOG0(NW_LOG_LEVEL3, "*** Printing Variable Style Sheet ****");
  //Walk through the RArray. For each entry, if it references a dynamic
  //entry (e.g. variable) delete the referenced object. Otherwise don't
  //do anything with the entry itself. Then close and delete the array.

  if (iPropList != NULL){
    NW_Ucs2 tmpString[40];
    NW_Ucs2* valString;
    NW_Ucs2* elementString;
    NW_Ucs2* patternString;
    NW_Int32 tmpInt;

    count = iPropList->Count();
    NW_LOG1(NW_LOG_LEVEL3, "CSS Property List -  %d entries", count);
    for(index=0; index<count; index++){
      TCSSPropEntry& propEntry = iPropList->operator[](index);

      const TCSSPropertyTablePropEntry* propTableEntry =
        TCSSPropertyTable::GetPropEntryByToken(propEntry.entry->name);

      switch(propEntry.entry->valType){
      case NW_CSS_ValueType_Color:
        tmpInt = (NW_Int32)(propEntry.entry->val);
        NW_Str_Sprintf(tmpString, CSS_HEX_FORMAT, tmpInt);
        valString = tmpString;
        break;
      case NW_CSS_ValueType_Token:
        {
          tmpInt = (NW_Int32)(propEntry.entry->val);
          const TCSSPropertyTablePropValEntry* valTableEntry =
            TCSSPropertyTable::GetPropValEntryByToken ((NW_LMgr_PropertyValueToken_t)tmpInt);
          valString = valTableEntry == NULL ? (NW_Ucs2*)CSS_INVALID_TOKEN : (NW_Ucs2*)valTableEntry->strVal;
        }
        break;
      case NW_CSS_ValueType_Text:
        if (propEntry.entry->val < iStringList->Count()){
          tmpInt = (NW_Int32)(propEntry.entry->val);
          TCSSStringEntry& stringEntry = iStringList->operator[](tmpInt);
          valString = stringEntry.entry;
        }else{
          valString = (NW_Ucs2*)CSS_INVALID_TOKEN;
        }
        break;

      case NW_CSS_ValueType_Number:
      case NW_CSS_ValueType_Percentage:
      case NW_CSS_ValueType_Em:
      case NW_CSS_ValueType_Ex:
      case NW_CSS_ValueType_In:
      case NW_CSS_ValueType_Cm:
      case NW_CSS_ValueType_Mm:
      case NW_CSS_ValueType_Pc:
      case NW_CSS_ValueType_Pt:
        NW_Str_Sprintf(tmpString, CSS_FLOAT_FORMAT, propEntry.entry->val);
        valString = tmpString;
        break;
      case NW_CSS_ValueType_Integer:
      case NW_CSS_ValueType_Px:
      default:
        tmpInt = (NW_Int32)(propEntry.entry->val);
        NW_Str_Itoa(tmpInt, tmpString);
        valString = tmpString;
      }

      // Pattern string contains the element name. It it exists show it. Otherwise show element name
      elementString = (NW_Ucs2*)NW_WBXML_Dictionary_getTagByFqToken((NW_XHTML_1_0_INDEX << 16) | propEntry.entry->elementToken);
      if (elementString == NULL) {
        elementString = (NW_Ucs2*)CSS_INVALID_TOKEN;
      }

      if (propEntry.entry->pattern == NULL){
		  patternString = NULL;
	  }else {
		  patternString = NW_Str_CvtFromAscii((const char*)(propEntry.entry->pattern));
		  NW_Str_Strcharreplace(patternString, ASCII_LEFT_BRACE, ASCII_NULL);
	  }

      NW_LOG7(NW_LOG_LEVEL3,
              "%3d %s  key: 0x%06X  %s{ %s: %s%s }",
              //Wrapper
              index,
              propEntry.variableEntry == TRUE ? (NW_Ucs2*)CSS_VARIABLE:(NW_Ucs2*)CSS_DEFAULT,
              propEntry.key,
              //Entry
              patternString == NULL ? elementString : patternString,
              propTableEntry->strName,
              valString,
              TCSSPropertyTable::GetPropValType(propEntry.entry->valType));

      delete patternString;
    }
  }

  if (iStringList != NULL){
    count = iStringList->Count();
    NW_LOG1(NW_LOG_LEVEL3, "CSS String List - %d entries", count);
    for(index=0; index<count; index++){
      TCSSStringEntry& stringEntry = iStringList->operator[](index);
      NW_LOG3(NW_LOG_LEVEL3, "%2d  %s  %s",
              index,
              stringEntry.variableEntry == TRUE ? (NW_Ucs2*)CSS_VARIABLE : (NW_Ucs2*)CSS_DEFAULT,
              stringEntry.entry);
    }
  }
  return KErrNone;
}
#endif /* _DEBUG */
