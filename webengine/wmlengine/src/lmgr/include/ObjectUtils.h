/*
* Copyright (c) 2003 - 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Performs a set of util methods related to objects.
*
*/

#ifndef OBJECTUTILS_H
#define OBJECTUTILS_H

// INCLUDES
#include <e32def.h>
#include <badesca.h>

#include "nwx_defs.h"
#include <nw_dom_element.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct NW_LMgr_Box_s NW_LMgr_Box_t;
typedef struct NW_HED_DomHelper_s NW_HED_DomHelper_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

class CBrCtlObjectInfo;

// CLASS DECLARATION

class ObjectUtils
    {
    public:  // Datatypes
        typedef enum
            {
            EHandlerImage,
            EHandlerPlugin,
            EHandlerExternalApp,
            EHandlerAny
            } THandlerType;


    public:  // Constructors and destructor
        /**
        * Destructor.
        */
        ~ObjectUtils();


    public:  // New Methods
        /**
        * Returns ETrue if aBox is an object box.
        *
        * @param aBox a box.
        * @param aOnlyIfPlaceHolder if true this method will only return true
        *        if the box is showing its place-holder (not an active plugin).
        *        Otherwise it was true true if it an object-box.
        *
        * @return ETrue or EFalse.
        */
        static TBool IsObjectBox(const NW_LMgr_Box_t& aBox, TBool aOnlyIfPlaceHolder);

        /**
        * Returns ETrue if aBox is an object box in the pre-active state.
        *
        * @param aBox a box.
        *
        * @return ETrue or EFalse.
        */
        static TBool IsDownloadedObjectBox(const NW_LMgr_Box_t& aBox);

        /**
        * Gets the following information about the object's resource: whether a third party
        * plugin or external application can open the resource, the size of the resource,
        * the mime-type of the resource, and the name of the plugin or application to
        * be used to open the resource.
        *
        * @param aBox the object's associated box.
        * @param aBrCtlObjectInfo, upon completion it holds the info about the object.
        *
        * @return void.
        */
	    static void GetBrCtlObjectInfoL(const NW_LMgr_Box_t& aBox, CBrCtlObjectInfo& aBrCtlObjectInfo);

        /**
        * Determines whether a given content-type of dot-extension has a cooresponding
        * plugin or external application.  aContentType and aUrl can be NULL, but at least
        * one must be non-NULL (otherwise it will always return EFalse).  If aHandlerType 
        * is EPlugin then it returns ETrue only if their is a supported plugin.  If aHandlerType 
        * is EExternalApp then it returns ETrue only if their is a supported external
        * application.  If aHandlerType is EBoth then it returns ETrue only if their is a 
        * supported plugin or external application.
        *
        * @param aContentType the content type to compare against, may be NULL.
        * @param aUrl the url to extract the dot-extension and to compare against, may be NULL.
        * @param aHandlerType the type of handler to consider.
        *
        * @return ETrue or EFalse.
        */
        static TBool IsSupported(const TDesC* aContentType, const TDesC* aUrl, 
                THandlerType aHandlerType);

        /**
        * Returns ETrue if plugin support is enabled.
        *
        * @return ETrue or EFalse.
        */
        static TBool Enabled();

        /**
        * Extracts the attribute's names and values from the given aElementNode.
        *
        * @param aDomHelper the caller's DomHelper, used to interact with its
        *        dom-tree.
        * @param aElementNode the element to extract attributes from.
        * @param aNameArray the attribute names.
        * @param aValueArray the attribute values.
        *
        * @return void
        */
        static void GetAttributesL(const NW_HED_DomHelper_t& aDomHelper, 
                const NW_DOM_ElementNode_t& aElementNode, CDesCArray** aNameArray, 
                CDesCArray** aValueArray);

        /**
        * Extracts the <param> tag attribute's name and value from the given aElementNode.
        *
        * @param aDomHelper the caller's DomHelper, used to interact with its
        *        dom-tree.
        * @param aElementNode the element to extract attributes from.
        * @param aNameArray the attribute names.
        * @param aValueArray the attribute values.
        *
        * @return void
        */
        static void GetParamAttributesL(const NW_HED_DomHelper_t& aDomHelper, 
                const NW_DOM_ElementNode_t& aElementNode, CDesCArray** aNameArray, 
                CDesCArray** aValueArray);

        /**
        * Given a name and value array attained from GetAttributesL it returns
        * a copy of the attribute value associated with the given name.  Upon
        * success the caller adopts aValue.  aValue is set to NULL if no 
        * attribute with the given name is present.
        *
        * @param aNameArray the attribute names.
        * @param aValueArray the attribute values.
        * @param aName the name of the attribute to be extracted.
        * @param aValue upon success it contains a copy of the attribute value or 
        *        NULL if the attribute wasn't present.
        *
        * @return void
        */
        static void GetAttributeValueL(const CDesCArray& aNameArray,
                const CDesCArray& aValueArray, const TDesC& aName, TDesC** aValue);

        /**
        * Given a name and value array this method inserts name/value pair to array.
        * If the name already is in the array, replace value 
        * by deleting old pair and adding new pair
        *
        * @param aNameArray the attribute names.
        * @param aValueArray the attribute values.
        * @param aName the name of the attribute to be replaced.
        * @param aValue the value of the attribute to be replaced.
        *
        * @return void
        */
        static void SetAttributeValueL( CDesCArray& aNameArray,
                CDesCArray& aValueArray, const TDesC& aName, const TDesC& aValue);

        /**
        * Given a name and value array this method remove name/value pair from array.
        * If the name is not found in the array, do nothing 
        *
        * @param aNameArray the attribute names.
        * @param aValueArray the attribute values.
        * @param aName the name of the attribute to be removed.
        *
        * @return void
        */
        static void RemoveAttributeL( CDesCArray& aNameArray,
                 CDesCArray& aValueArray, const TDesC& aName );

        /**
        * Returns the content type associated with the given class-id.
        *
        * @param aClassId the classid to lookup.
        *
        * @return the associated content type of NULL if no association exists.
        */
        static TDesC* GetAssociatedContentType(const TDesC& aClassId);

        /**
        * Returns the source param name associated with the given class-id.
        *
        * @param aClassId the classid to lookup.
        *
        * @return the associated source param name of NULL if no association exists.
        */
        static TDesC* GetAssociatedSourceParamName(const TDesC& aClassId);

        /**
        * General purpose panic function for Object related errors.
        *
        * @since 2.6
        * @param aError The reason for panic. Could be left out.
        * @return void
        */
        static void Panic(TInt aError = KErrNone);

        /**
        * General purpose panic function for Object related errors.
        *
        * @since 2.8
        * @param aUrl The uri with file name and extension.
        * @return the content type of the uri
        */
		static HBufC* GetContentTypeByUrl(const TDesC* aUri);

    private:  // Private Methods
        /**
        * C++ default constructor.
        */
        ObjectUtils();

        /**
        * Returns the name of the plugin that supports the given content-type or
        * dot-extension otherwise NULL if no plugin support this type.  aContentType 
        * and aUrl can be NULL, but at least one must be non-NULL (otherwise it will 
        * always return EFalse). The caller adopts the result.
        *
        * @param aContentType the content type to compare against, may be NULL.
        * @param aUrl the url to compare against, may be NULL.
        *
        * @return The name of the plugin or NULL
        */
        static TDesC* GetPluginNameL(const TDesC* aContentType,
                const TDesC* aUrl);

        /**
        * Returns the name of the external application that supports the given 
        * content-type or NULL if no application support this type.  
        * The caller adopts the result.
        *
        * @return The name of the application or NULL
        */
        static TDesC* GetExternalAppNameL(const TDesC* aContentType);

        /**
        * Returns ETrue if the content-type or dot-extension is a supported image type. 
        * aContentType and aUrl can be NULL, but at least one must be non-NULL (otherwise 
        * it will always return EFalse).
        *
        * @param aContentType the content type to compare against, may be NULL.
        * @param aUrl the url to compare against, may be NULL.
        *
        * @return ETrue if its a supported image type.
        */
        static TBool IsImageSupported(const TDesC* aContentType, const TDesC* aUrl);

        /**
        * Returns the cooresponding value (either source-param-name or content-type) 
        * given the class-id.
        *
        * @param aClassId the class-id used to map to the requested value.
        * @param aValueSelector used to select the cooresponding value 
        *                       (either KParamName or KContentType)
        * @return the value or NULL on out of memory.
        */
        static TDesC* GetClassidAssociation(const TDesC& aClassId, 
                TInt aValueSelector);
    };

#endif  // OBJECTUTILS_H
