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
* Description:  Map containing the Variable CSS style rules
*
*/


#ifndef CCSSVariableStyleSheet_H
#define CCSSVariableStyleSheet_H

//  INCLUDES
#include <e32base.h>
#include "nw_css_defaultstylesheeti.h"
#include "CSSRuleList.h"

// CONSTANTS

// MACROS

// FUNCTION PROTOTYPES
class TCSSParser;
struct TCSSPropertyTablePropEntry;

// CLASS DECLARATION

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

  extern NW_WBXML_Dictionary_t NW_XHTML_WBXMLDictionary;

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */



/** *****************************************************************
 *  These structs are simple wrappers for the css property and string
 *  entries. They wrap the same structures found in the default style
 *  sheets. Note that a flag indicates whether they entry is a static
 *  referring to the fixed table or a dynamic one referring to the
 *  dynamic one. The dynamic (e.g. variable) entry is heap allocated and
 *  as such it and its components should be deallocated.
 *
 *  @lib css.lib
 *  @since 2.7
 ******************************************************************* */

typedef struct _TCSSStringEntry{
  TBool variableEntry;
  NW_Ucs2* entry;
}TCSSStringEntry;


typedef struct _TCSSPropEntry{
  TInt key; // MUST BE FIRST
  TBool variableEntry;
  NW_CSS_DefaultStyleSheet_PropEntry_s* entry;
}TCSSPropEntry;

// DATA TYPES
typedef RArray<TCSSStringEntry> CCSSStringEntryList;
typedef RArray<TCSSPropEntry> CCSSPropEntryList;

/** *****************************************************************
 *  The CCSSVariableStyleSheet is a repository for the default and
 *  variable style sheet entries. If no entries variable entries are
 *  found (i.e. file not found or corrupt) then only the default entries
 *  will be used to populate the structure.
 *
 *  @lib css.lib
 *  @since 2.7
 ******************************************************************* */
class CCSSVariableStyleSheet : public CBase
{


public:   // Constructors and destructor

  /**
   * Two-phased constructor. It is private because it is created as a
   * singleton and shared.
   * @return CCSSVariableStyleSheet*
   */
  static CCSSVariableStyleSheet* NewL();

  /**
   * Creates the CCSSVariableStyleSheet singleton.
   * @since 2.7
   * @return style sheet.
   */
  static CCSSVariableStyleSheet* CreateSingleton();

  /**
   * Deletes the CCSSVariableStyleSheet singleton.
   * @since 2.7
   * @return void.
   */
  static void DeleteSingleton();

  /**
   * Get the CCSSVariableStyleSheet singleton. If it doesn't exist
   * then return NULL.
   * @since 2.7
   * @return a pointer to CCSSVariableStyleSheet
   */
  static CCSSVariableStyleSheet* GetSingleton();

/**
 * Applies the combined variable and default styles to the element node
 * in the document after pattern matching. This is called frequently by
 * XHTMLElementHandler. Duplicates have already been removed so that the
 * default and variable styles do not need to be treated differently or
 * applied separately.
 *
 * @param elementNode Element node for which pattern matching needs to be done
 * @param box Box to which CSS is applied
 * @param dictionary
 * @param event event resulting in style application (for e.g. focus)
 *                This is usually NULL.
 * @param documentNode
 * @param domHelper for resolving entities
 * @since 2.7
 * @return KBrsrSuccess or KBrsrOutOfMemory
 */
TBrowserStatusCode ApplyStyles(NW_DOM_ElementNode_t* aElementNode,
                               NW_LMgr_Box_t* aBox,
                               NW_WBXML_Dictionary_t* aDictionary,
                               const NW_Evt_Event_t* aEvent,
                               NW_HED_DocumentNode_t* aDocumentNode,
                               NW_HED_DomHelper_t* aDomHelper);

/**
 * Parses the loaded variable CSS buffer stores entries in the tables.
 * @param buffer data to parse. Stored as text
 * @param length size of buffer in bytes. Does not include a null terminator
 * @return ETrue if parsed.
 */
TBool ProcessBuffer(const TUint8* aBuffer, TUint32 aLength);

/**
 * Accessor for the state of the default property sheet. It is true if
 * the values were successfully loaded.
 * @return ETrue if loaded
 */
TBool IsStaticCSSLoaded(){
  return  iStaticCSSLoaded;
}


/**
 * Accessor for the state of the variable property sheet. It is true if
 * the values were successfully loaded.
 * @return ETrue if loaded
 */
TBool IsVariableCSSLoaded(){
  return  iVariableCSSLoaded;
}

private:    // Data

 TBool iStaticCSSLoaded;
 TBool iVariableCSSLoaded;
 CCSSPropEntryList* iPropList;
 CCSSStringEntryList* iStringList;

private:

/**
 * C++ default constructor.
 */

 /**
  * By default Symbian 2nd phase constructor is private.
  */
void ConstructL();


/**
 * Destructor. It is private because it is created as a singleton.
 */
 virtual ~CCSSVariableStyleSheet();


/**
 * Assignment operator. It is private because it should not be called.
 */
void operator=(const CCSSVariableStyleSheet& obj);


/**
 * Loads a file into the buffer. Looks first in on the C drive and then
 * if not found looks on the Z drive.
 * @return Pointer to buffer containing loaded file contents.
 * Otherwise returns NULL.
 */
 HBufC8* ReadVariableCSSFile();


/**
 * Loads a file into the buffer. This is a generic utility.
 * @return Pointer to buffer containing loaded file contents.
 * Otherwise returns NULL.
 */
 HBufC8* ReadFile(const TDesC& aFile);


/**
 * Parses the css file that has been loaded. The results are stored into
 * the variable style sheet list. Duplicates are ignored. That is, the
 * first entry is added and subsequent duplicates are ignored. This list
 * is added before the lower priority default list so if a duplicate is
 * found it is assumed to be equal or higher priority.
 * @param buffer The buffer contains the css variable file.
 * @return ETrue if successfully parsed. Otherwise it returns EFalse
 */
 TBool AddVariableStyles(HBufC8* aBuffer);


/**
 * Loads the CSS strings from the default tables. These are placed in a
 * string table.
 */
 void AddDefaultStyleStrings();


/**
 * Loads the default css styles. They are merged with the previously
 * loaded variable styles. That is, if a style is not found in the list
 * it is added. If it is already present the default value is
 * ignored. Duplicates are not allowed. Note that if duplicates exist in
 * this default list, only the first entry is applied.
 */
 void AddDefaultStyleProperties();


/**
 * Initializes the property using the entry loaded from the variable table.
 * @param propEntry Default/variable entry to be applied.
 * @param prop Property that is being initialized.
 * @param box Box to which CSS is applied
 * @param val Value to apply. Requires valType for interpretation.
 * @param valType Data type
 * @return KBrsrSuccess if operation successful. Otherwise  KBrsrFailure.
 */
TBrowserStatusCode InitProperty(const TCSSPropertyTablePropEntry* aPropEntry,
                                NW_LMgr_Property_t* aProp,
                                NW_LMgr_Box_t* aBox,
                                NW_Float32 aVal,
                                NW_Uint8 aValType);


/**
 * Adds the styles for the element to the list.
 * @param aParser loaded parser for the style sheet
 * @param aRuleList list for this element token
 * @param aElementToken
 * @return KBrsrSuccess if operation successful. Otherwise  KBrsrFailure.
 */
TBrowserStatusCode UpdateList(TCSSParser* aParser,
                              CCSSRules* aRuleList,
                              NW_Uint16 aElementToken);

/**
 * Updates one entry in the list.
 * @param aParser The parser
 * @param aElementToken The token to add
 * @param aAsciiPatternStr Optional pattern to include with token.
 * @return Status of operation
 */
TBrowserStatusCode UpdateEntry(TCSSParser* aParser,
                               TUint16 aElementToken,
                               char* aAsciiPatternStr);


/**
 * Wrapper method to call the correct setting method.
 * @param aParser The parser.
 * @param aVal The property value type
 * @param  aPropertyValue The actual value.
 * @param aValStorage The text value
 * @return Property type
 */
NW_LMgr_PropertyValueType_t SetPropertyVal(TCSSParser* aParser,
                                           const TCSSPropertyTablePropEntry* aPropEntry,
                                           NW_LMgr_PropertyValue_t* aVal,
                                           TCSSPropertyVal* aPropertyVal,
                                           TText16* aValStorage);

/**
 * Inserts a value into the list. If there is an error or a duplicate
 * the value is not entered.
 * @param aPropEntry Value to enter into the list.
 * @return index of inserted value or -1 if error
 */
TInt InsertProperty(TCSSPropEntry aPropEntry);


/**
 * Dumps the contents of the style sheet to the log.
 */
TBrowserStatusCode DumpVariableStyleSheet();

void ConstructCSSFileName();

TFileName CSS_USER_SUPPLIED_FILE;
TFileName CSS_OPERATOR_SUPPLIED_FILE;



};


#endif /*  CCSSVariableStyleSheet_H */
