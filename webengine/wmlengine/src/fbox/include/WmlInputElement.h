/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CWMLINPUTELEMENT_H
#define CWMLINPUTELEMENT_H

//  INCLUDES

//#include "WmlRenderingElement.h"
#include "WmlReSizableEditor.h"
#include "WmlFormattedEditor.h"
#include <E32DEF.H>
#include <coemain.h>
#include <eikedwob.h>

#include <coeinput.h>
#include <frmtview.h>
#include <fepbase.h>
#include <eikrted.h>

// MACROS

//#define OOM_TEST_IN_CONSTRUCTL
//#define OOM_TEST_IN_HANDLEEDWINEVENTL
//#define OOM_TEST_IN_OFFERKEYEVENTL
//#define OOM_TEST_IN_HANDLECOMMANDL
//#define OOM_TEST_IN_REFRESHL
//#define OOM_TEST_IN_INITVALUESL

// CONSTANTS

const TInt KWmlInputMinimalWidth                = 50;   // minimal width of input's editor

// FORWARD DECLARATIONS

class CEikRichTextEditor;
class CMyFepContainer;
class CGraphicsContext;
class MGraphicsDeviceMap;
class CWmlFormatHandler;
class CIdle;
class CParaFormatLayer;
class CCharFormatLayer;
class CRenderingNode;

//typedef CWmlFormattedEditor<CWmlReSizableEditor<CEikRichTextEditor> >
//CWmlFormattedResizeableEditor;

typedef CWmlFormattedEditor CWmlFormattedResizeableEditor;


// CLASS DECLARATION

/**
*  CWmlInputElement implements the functionality
*  of the WML input element.
*/
class CWmlInputElement : public MEikEdwinObserver, CBase
{
protected:      // Constructors and destructor

    /*
     * C++ default constructor.
     * @param aNode The parent node.
     */
        CWmlInputElement();

    /*
     * Second-phase constructor. Leaves on failure.
     * @param aParent Parent control.
     * @param aPopup this control is used as popup or not
     */
        void ConstructL(CCoeControl*);

public: // Constructors and destructor
    /*
     * Two-phased constructor.
     * It leaves a pointer on the cleanup stack.
     * @param aNode The parent node.
     */
        static CWmlInputElement* NewLC(CCoeControl*);
        
    /*
     * Two-phased constructor.
     * @param aNode The parent node.
     */
        static CWmlInputElement* NewL(CCoeControl*);

    /*
     * Destructor.
     */
        virtual ~CWmlInputElement();

private:        // New functions

        enum TWmlCursorHandling
                {
                EWmlMoveCursor,
                EWmlScrollDisplay
                };

        /**
        * Makes the cursor visible by moving it to the last visible position,
        * or by scrolling the display depending on the aMode parameter.
        * @param aMode Mode.
        */
        void ShowCursorL( TWmlCursorHandling aMode = EWmlMoveCursor );

        /**
        * Handles key events in non-editing state.
        * @param aKeyEvent      Key event.
        * @param aType          Type.
        * @return Response.
        */
        TKeyResponse NavigateL( const TKeyEvent& aKeyEvent, TEventCode aType );

        /**
        * Handles key events in editing state
        * @param aKeyEvent      Key event.
        * @param aType          Type.
        * @return Response.
        */
        TKeyResponse EditL( const TKeyEvent& aKeyEvent, TEventCode aType );

        /**
        * This callback stores editor content in browser core.
        * @param aElement Input element.
        * @return EFalse
        */
        static TInt StoreTextL( TAny* aElement );

        /**
        * This callback simulates and invalidates cached key event if any.
        * @param aElement Input element.
        * @return EFalse
        */
        static TInt SimulateKeyEventL( TAny* aElement );

public: // From MEikEdwinObserver

        /**
        * Handles edwin events.
        * @param aEdwin Source of event.
        * @param aEventType Type of event.
        */
        void HandleEdwinEventL( CEikEdwin* aEdwin, TEdwinEvent aEventType );

public: // from MFocusable

        /**
        * Handles key events.
        * @param aKeyEvent      Key event.
        * @param aType          Type.
        * @return Response.
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
        
        /**
        * Handles menu events.
        * @param aCommand       Menu event.
        */
        void HandleCommandL( TInt aCommand );

        /**
        * At least one line must be visible.
        * @param aScrollDirection Scrolling direction.
        * @return Minimum height.
        */
        TInt HeightToBeVisible
                        ( TCursorPosition::TMovementType aScrollDirection ) const;

        TInt CommandSetResourceIdL();

        /**
        * Cancels active element (input element currently)
        */
        void DeactivateElement();

public: // from CWmlRenderingElement
        
        /**
        * Handles font size settings change.
        * @return -.
        */
        void NotifyVisualChangesL();

        /**
        * Querries all of the data from the core.
        */
        void RefreshL();

        /**
        * Initalize members with default values.
        */
        void InitValuesL(CCoeControl*);

        /**
        * Draws the inputbox to the given gc
        * @param aGc            Graphic context.
        * @param aTopLeft       The topleft point of the drawing rectangle.
        * @param aClipRect  The clipping rectangle.
        * @param aMap           The device map (not used).
        */
        void DrawInnerRectL( CGraphicsContext& aGc,
                                                 const TPoint& aTopLeft,
                                                 const TRect& aClipRect,
                                                 MGraphicsDeviceMap* aMap ) const;
        /**
        * Border is needed?
        * @return The answer.
        */
        virtual TBool BorderIsNeeded() const;

        /**
        * Put on, or take off the focus.
        * @param aItemFocused The direction of the operation. True means putting on.
        * @return True means that the operation was successful.
        */
    virtual TBool SetFocusBeforeDrawBorderL(
                TBool aItemFocused,     TCursorPosition::TMovementType aScrollDirection );


    CCoeControl* ComponentControl( TInt aIndex );


// ---------------------------------------------GSZ from RenderingElement    
		/*
		*Draws the specified parts of the rendering element
		*it calls the leaving version ( DrawL() )
		*@param aGc the graphics context to draw to
		*@param aTopLeft the topleft coordinate of the element on the passed GC
		*@param aClipRect is the rectangle to draw
		*@param aMap is the device map between the phisical and logical representation of the element
		*/
    void Draw( CGraphicsContext& aGc,
				   const TPoint& aTopLeft,
				   const TRect& aClipRect,
				   MGraphicsDeviceMap* aMap ) const;
		
		/**
		* Leaving version of Draw.
		* @see Draw
		*/
		void DrawL( CGraphicsContext& aGc,
				    const TPoint& aTopLeft,
				    const TRect& aClipRect,
				    MGraphicsDeviceMap* aMap ) const;
// ---------------------------------------------GSZ from RenderingElement    

private:

    /**
    * By default EPOC constructor is private.
    */
    void ConstructL();

        /**
        * Shows general note. Shows plural text if aDynamic > 1, singular otherwise.
        */
        void NoteTooFewCharacterL( TInt aDynamic = 0 );

        /**
        * Opens or closes editor.
        * @param aOn True means enabling.
        */
        void AllowEditingL( TBool aOn = ETrue );

        /**
        * Updates character counter on navipane.
        * @param aVisible EFalse hides counter.
        */
        void UpdateTextCounterL( TBool aVisible = ETrue );

        /**
        * Resizes editor, sets cursor, and relayouts card if necessary.
        */
        void HandleTextChangeL();

private:        // Data
        
        CWmlFormattedResizeableEditor* iEditor;
        TBool iOpened;
        TInt iPrevCurPos;
        TInt iMaxLength;
        TBool iEmptyOk;
        CWmlFormatHandler* iHandler;
        CIdle* iAsyncCallBack;
        TBool iConfirmed;
        TKeyEvent iCachedKeyEvent;
        TBool iCacheIsValid;
        CParaFormatLayer* iGlobalParaLayer;
        CCharFormatLayer* iGlobalCharLayer;

// ---- GSZ from RenderingElement    
    		TSize iSizeInTwipsWithoutBorder;
        TGulBorder iBorder;

// ---- GSZ as iNode->ApiProvider().Parent()    
        CCoeControl* iParent;

};
#endif

// End of File
