/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

/* Automatically generated from Widget.cpp using create_hash_table. DO NOT EDIT ! */

#include "lookup.h"

namespace KJS {

static const struct HashEntry WidgetTableEntries[] = {
   { "performTransition", JSWidget::performTransition, DontDelete|Function, 0, &WidgetTableEntries[16] }/* 3947827008 */ ,
   { "openApplication", JSWidget::openApplication, DontDelete|Function, 1, &WidgetTableEntries[17] }/* 78708705 */ ,
   { "setDisplayPortrait", JSWidget::setDisplayPortrait, DontDelete|Function, 0, 0 }/* 4273345218 */ ,
   { "wrt", JSWidget::wrt, DontDelete|ReadOnly, 0, 0 }/* 1862342003 */ ,
   { "preferenceForKey", JSWidget::preferenceForKey, DontDelete|Function, 1, &WidgetTableEntries[20] }/* 2710796932 */ ,
   { 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0 },
   { "openURL", JSWidget::openURL, DontDelete|Function, 1, &WidgetTableEntries[18] }/* 120566279 */ ,
   { 0, 0, 0, 0, 0 },
   { "setNavigationEnabled", JSWidget::setNavigationEnabled, DontDelete|Function, 1, 0 }/* 2136509817 */ ,
   { "prepareForTransition", JSWidget::prepareForTransition, DontDelete|Function, 1, 0 }/* 3355888042 */ ,
   { 0, 0, 0, 0, 0 },
   { "setDisplayLandscape", JSWidget::setDisplayLandscape, DontDelete|Function, 0, 0 }/* 1431294652 */ ,
   { "setPreferenceForKey", JSWidget::setPreferenceForKey, DontDelete|Function, 2, 0 }/* 1304460605 */ ,
   { "onshow", JSWidget::onshow, DontDelete|ReadOnly, 0, 0 }/* 2917788702 */ ,
   { 0, 0, 0, 0, 0 },
   { "identifier", JSWidget::identifier, DontDelete|ReadOnly, 0, &WidgetTableEntries[19] }/* 4053269040 */ ,
   { "onhide", JSWidget::onhide, DontDelete|ReadOnly, 0, 0 }/* 236865857 */ ,
   { "onremove", JSWidget::onremove, DontDelete|ReadOnly, 0, 0 }/* 853904071 */ ,
   { "onexit", JSWidget::onexit, DontDelete|ReadOnly, 0, 0 }/* 3671980544 */ ,
   { "isrotationsupported", JSWidget::isRotationSupported, DontDelete|ReadOnly, 0, 0 }/* 3247979316 */ 
};

const struct HashTable WidgetTable = { 2, 21, WidgetTableEntries, 16 };

} // namespace
