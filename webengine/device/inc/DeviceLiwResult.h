/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __DEVICELIWRESULT_H
#define __DEVICELIWRESULT_H

//  INCLUDES
#include <object.h>

/**
*  Device
*
*  @lib jsdevice.dll
*  @since 5.0
*/


namespace KJS
    {
    class DeviceLiwResultPrivate;    
    class DeviceLiwResult: public JSObject
        {

        public: // constructor and destructor

           /**
            * Constructor
            * @return none
            * @since 5.0
            **/
            DeviceLiwResult( JSValue* proto);


           /**
            * Destructor
            * @return none
            * @since 5.0
            **/
            virtual ~DeviceLiwResult();


        public: // From JSObject

           /**
            * getOwnPropertySlot
            * @return bool
            * @since 5.0
            **/
            bool getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot);
           /**
            * getValueProperty
            * @return JSValue*
            * @since 5.0
            **/
            JSValue* getValueProperty(KJS::ExecState* exec, int token) const;

           /**
            * class info
            * @return const ClassInfo*
            * @since 5.0
            **/
            const ClassInfo* classInfo() const { return &info; }

           /**
            * whether this object is valid
            * @return boolean
            * @since 5.0
            */
            const bool isValid() const { return m_valid; }
       
           /**
            * close jsobject array
            * @return 
            * @since 5.0
            **/
            void Close(ExecState* exec, bool unmark );  
              
           /**
            * close jsobject array
            * @return 
            * @since 5.0
            **/
            void quickClose();  
 
            static const ClassInfo info;


           /**
            * toString
            * @return UString
            * @since 5.0
            **/
            virtual UString toString( ExecState* exec ) const;

            enum
                {
                close
                };

        private:

            DeviceLiwResultPrivate* m_privateData;  // private object to hold data
            bool m_valid;                           // bject is valid or not
            

        };
        
    class DeviceLiwResultPrivate
        {
            friend class DeviceLiwResult;
            friend class DeviceLiwResultFunc;
            public:
                DeviceLiwResultPrivate();
                ~DeviceLiwResultPrivate()   { Close(); }
                void Close();
                Identifier m_propName;
        };

    class DeviceLiwResultFunc : public JSObject
        {
        public: // constructor and destructor

           /**
            * Constructor
            * @return none
            * @since 5.0
            **/
            DeviceLiwResultFunc( ExecState* exec, int token );

           /**
            * Destructor
            * @return none
            * @since 5.0
            **/
            virtual ~DeviceLiwResultFunc() {}

        public: // From ObjectImp  JSObject

           /**
            * Whether implements the call
            * @return bool
            * @since 5.0
            **/
            bool implementsCall() const { return true; }

           /**
            * Call the function
            * @return Value
            * @since 5.0
            **/
            JSValue* callAsFunction(ExecState *exec, JSObject *thisObj, const List &args);

        private:
            int m_func;
        };

    }
#include "DeviceLiwResult.lut.h"
#endif //__DEVICE_H
