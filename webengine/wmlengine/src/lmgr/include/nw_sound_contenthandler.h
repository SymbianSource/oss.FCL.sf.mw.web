/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Defines OOC class structures and public method prototypes.
*
*/



#ifndef NW_SOUND_CONTENTHANDLER_H
#define NW_SOUND_CONTENTHANDLER_H

// INCLUDES
#include <e32std.h>
#include <nw_dom_node.h>
#include "nw_hed_contenthandler.h"
#include "nw_hed_documentnode.h"
#include "nw_object_base.h"
#include "nw_object_core.h"
#include "nw_object_dynamic.h"
#include "nw_object_object.h"
#include "nw_sound.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
typedef struct NW_Sound_ContentHandler_Class_s NW_Sound_ContentHandler_Class_t;
typedef struct NW_Sound_ContentHandler_s NW_Sound_ContentHandler_t;
class CPluginInst;

// CLASS DECLARATION


/* ------------------------------------------------------------------------- *
   virtual method prototypes
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
   OOC class declaration
 * ------------------------------------------------------------------------- */
typedef struct NW_Sound_ContentHandler_ClassPart_s
    {
    void** unused;
    } NW_Sound_ContentHandler_ClassPart_t;

struct NW_Sound_ContentHandler_Class_s
    {
    NW_Object_Core_ClassPart_t NW_Object_Core;
    NW_Object_Base_ClassPart_t NW_Object_Base;
    NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
    NW_HED_DocumentNode_ClassPart_t NW_HED_DocumentNode;
    NW_HED_ContentHandler_ClassPart_t NW_HED_ContentHandler;
    NW_Sound_ContentHandler_ClassPart_t NW_Sound_ContentHandler;
    };

/* ------------------------------------------------------------------------- *
   OOC object declaration
 * ------------------------------------------------------------------------- */
struct NW_Sound_ContentHandler_s
    {
    NW_HED_ContentHandler_t super;

    // Flag indicating if the content is currently playing.
    NW_Bool iContentPlaying;

    // Flag indicating whether or not the content is fully downloaded and ready
    // to be played. Also set to false if after the content is ready, it is
    // deemed invalid.
    NW_Bool iContentReady;
    
    // Mime type of this content in two different formats - ASCII & UCS2.
    HBufC8* iContentTypeAscii8;
    HBufC* iContentTypeUcs2;

    // Flag indicating whether or not the content is valid.
    NW_Bool iContentValidated;

    // DOM element node for the anchor link that specified this sound content.
    // It is used to establish the link between this content handler and the
    // active container box for the anchor link.
    NW_DOM_ElementNode_t* iElementNode;

    // Plugin for sound.
    CPluginInst* iPluginInst;

    // URL for the sound content.
    NW_Ucs2* iRequestUrl;
    HBufC* iResponseUri;

    // How many active container box set this as SoundCH
    NW_Int16 iBoxRefCnt;
    };

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Sound_ContentHandler_GetClassPart(_object) \
    (NW_Object_GetClassPart ((_object), NW_Sound_ContentHandler))

#define NW_Sound_ContentHandlerOf(_object) \
    (NW_Object_Cast ((_object), NW_Sound_ContentHandler))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
extern const NW_Sound_ContentHandler_Class_t NW_Sound_ContentHandler_Class;


/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/**
* Starts playing the sound.
* @since 3.0
* @param aSoundCH ptr to the sound content handler that contains the sound
*   to be played. The sound content handler has an API for playing the
*   sound and it will be invoked.
* @param aLoop number of times to repeat playing the sound.
* @param aVolume sound volume setting.
* @return Browser status code:
*   KBrsrSuccess if successful,
*   KBrsrOutOfMemory if no more memory,
*   otherwise, any other browser failure.
*/
extern
TBrowserStatusCode
NW_Sound_ContentHandler_StartPlayingSound( NW_Sound_ContentHandler_t* aSoundCH,
    NW_Int16 aLoop, TSoundstartVolume aVolume );

/**
* Stops playing the sound.
* @since 3.0
* @return Browser status code:
*   KBrsrSuccess if successful,
*   KBrsrOutOfMemory if no more memory,
*   otherwise, any other browser failure.
*/
extern
TBrowserStatusCode
NW_Sound_ContentHandler_StopPlayingSound( NW_Sound_ContentHandler_t* aSoundCH );

#endif
