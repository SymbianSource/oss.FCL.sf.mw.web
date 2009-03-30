/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#include "config.h"

#include "RenderTheme.h"
#include "Event.h"
#include "Document.h"
#include "Image.h"
#include "GraphicsContext.h"
#include <../bidi.h>
#include <eikenv.h>
#include "RenderBox.h"
#include <BitmapTransforms.h>
#include "WebCoreGraphicsContext.h"
#include "MaskedBitmap.h"
#include "BitmapImage.h"
#include "WebCoreWidget.h"
// implement Symbian theme here

namespace WebCore {

enum {
    ECheckBoxOn,
    ECheckBoxOff,
    ERadioOn,
    ERadioOff,
    ESelectArrow
};

class RenderThemeSymbian : public RenderTheme
{
public:
    RenderThemeSymbian();
    // A method asking if the theme's controls actually care about redrawing when hovered.
    bool supportsHover(const RenderStyle*) const { return true; }
    
    void setCheckboxSize(RenderStyle*) const;    
    void setRadioSize(RenderStyle*) const;

    void adjustButtonStyle(CSSStyleSelector*, RenderStyle*, Element*) const;
    void adjustTextFieldStyle(CSSStyleSelector*, RenderStyle*, Element*) const;
    void adjustMenuListStyle(CSSStyleSelector* selector, RenderStyle* style, Element* e) const;
    void adjustMenuListButtonStyle(CSSStyleSelector* selector, RenderStyle* style, Element* e) const;
    
    bool paintCheckbox(RenderObject* o, const RenderObject::PaintInfo& i, const IntRect& r);
    bool paintButton(RenderObject*, const RenderObject::PaintInfo&, const IntRect&);
    bool paintMenuListButton(RenderObject* o, const RenderObject::PaintInfo& i, const IntRect& r);
    bool paintMenuList(RenderObject* o, const RenderObject::PaintInfo& i, const IntRect& r);
    bool paintRadio(RenderObject* o, const RenderObject::PaintInfo& i, const IntRect& r);
    bool paintTextField(RenderObject*, const RenderObject::PaintInfo&, const IntRect&);
    bool paintTextArea(RenderObject*, const RenderObject::PaintInfo&, const IntRect&);
    

    bool isControlStyled(const RenderStyle*, const BorderData&,
                                 const BackgroundLayer&, const Color&) const;
    bool controlSupportsTints(const RenderObject*) const;
    void systemFont(int propId, FontDescription&) const;
    
    Color platformActiveSelectionBackgroundColor() const; 
    Color platformInactiveSelectionBackgroundColor() const;
    Color platformActiveSelectionForegroundColor() const;
    Color platformInactiveSelectionForegroundColor() const;
    
    void addIntrinsicMargins(RenderStyle*) const;
    void close();
    bool supportsFocus(EAppearance) const;
    void paintButtonDecorations(RenderObject* o, const RenderObject::PaintInfo& i, const IntRect& r);

    bool supportsFocusRing(const RenderStyle*) const;
    void scaleImage(int type, int scalingFactor);
    void scaleImageL(int type, int scalingFactor);

    void cancel();
    void run();

    Image* m_checkBoxOn;
    Image* m_checkBoxOff;
    Image* m_scaledCheckBoxOn;
    Image* m_scaledCheckBoxOff;
    int m_scalingForCheckBoxOn;
    int m_scalingForCheckBoxOff;

    Image* m_radioButtonOn;
    Image* m_radioButtonOff;
    Image* m_scaledRadioOn;
    Image* m_scaledRadioOff;
    int m_scalingForRadioOn;
    int m_scalingForRadioOff;

    Image* m_selectArrow;
    Image* m_scaledSelectArrow;
    int m_scalingForSelectArrow;

    CBitmapScaler* m_bitmapScaler;
    CActiveSchedulerWait* m_asw;
};

RenderTheme* theme()
{
    static RenderThemeSymbian symbianTheme;
    return &symbianTheme;
}

MScrollView* scrollView(const RenderObject::PaintInfo& i)
{
    return &(i.context->platformContext()->view());
}

class SyncScaler : public CActive
{
public:
    SyncScaler();
    ~SyncScaler();
    void init();
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);
    int m_error;
};

SyncScaler::SyncScaler() : CActive (CActive::EPriorityHigh)
{
    CActiveScheduler::Add(this);
}

SyncScaler::~SyncScaler()
{
    Cancel();
}

void SyncScaler::init()
{
    m_error = KErrNone;
    iStatus = KRequestPending;
    SetActive();
}

void SyncScaler::RunL()
{
    m_error = iStatus.Int();
    static_cast<RenderThemeSymbian*>(theme())->run();
}

void SyncScaler::DoCancel()
{
    // Since the operation is very fast, not sure what could cause it to be canceled.
    static_cast<RenderThemeSymbian*>(theme())->cancel();
}

TInt SyncScaler::RunError(TInt aError)
{
    m_error = aError;
    static_cast<RenderThemeSymbian*>(theme())->run();
    return KErrNone;
}

RenderThemeSymbian::RenderThemeSymbian()
{
    m_checkBoxOn = Image::loadPlatformResource("checkBoxOn");
    m_checkBoxOff = Image::loadPlatformResource("checkBoxOff");
    m_scaledCheckBoxOn = 0;
    m_scaledCheckBoxOff = 0;
    m_scalingForCheckBoxOn = 0;
    m_scalingForCheckBoxOff = 0;

    m_radioButtonOn = 0;
    m_radioButtonOff = 0;
    m_scaledRadioOn = 0;
    m_scaledRadioOff = 0;
    m_scalingForRadioOn = 0;
    m_scalingForRadioOff = 0;

    m_selectArrow = 0;
    m_scaledSelectArrow = 0;
    m_scalingForSelectArrow = 0;

    m_bitmapScaler = NULL;
    m_asw = NULL;
}

void RenderThemeSymbian::systemFont(int propId, FontDescription& fontDescription) const
{
}


Color RenderThemeSymbian::platformActiveSelectionBackgroundColor() const
{
    TRgb c = CEikonEnv::Static()->Color(EColorControlHighlightBackground);
    return Color(c.Red(),c.Green(),c.Blue());
}

Color RenderThemeSymbian::platformInactiveSelectionBackgroundColor() const
{
    return platformActiveSelectionBackgroundColor();
}

Color RenderThemeSymbian::platformActiveSelectionForegroundColor() const
{
    TRgb c = CEikonEnv::Static()->Color(EColorControlHighlightText);
    return Color(c.Red(),c.Green(),c.Blue());
}

Color RenderThemeSymbian::platformInactiveSelectionForegroundColor() const
{
    return platformActiveSelectionForegroundColor();
}

bool RenderThemeSymbian::paintCheckbox(RenderObject* o, const RenderObject::PaintInfo& i, const IntRect& r)
{
    paintButtonDecorations(o, i, r);
    MScrollView* wv = scrollView(i);
    // don't scale the checkbox if scalingFactor is 100% or  we are in the process of scaling another image
    if (wv->scalingFactor() <= 100  || m_bitmapScaler) {
        i.context->drawImage(isChecked(o)?m_checkBoxOn:m_checkBoxOff,r);
    }
    // use the cached scaled image if it has the same scaling factor
    else if (wv->scalingFactor() == (isChecked(o) ? m_scalingForCheckBoxOn : m_scalingForCheckBoxOff)) {
        i.context->drawImage(isChecked(o) ? m_scaledCheckBoxOn : m_scaledCheckBoxOff, r);
    }
    // scale
    else {
        scaleImage(isChecked(o) ? ECheckBoxOn : ECheckBoxOff, wv->scalingFactor());
        if (wv->scalingFactor() == (isChecked(o) ? m_scalingForCheckBoxOn : m_scalingForCheckBoxOff)) {
            i.context->drawImage(isChecked(o) ? m_scaledCheckBoxOn : m_scaledCheckBoxOff, r);
        }
        else {        
            i.context->drawImage(isChecked(o)?m_checkBoxOn:m_checkBoxOff,r);
        }
    }
    return false;
}

bool RenderThemeSymbian::paintRadio(RenderObject* o, const RenderObject::PaintInfo& i, const IntRect& r)
{
    if (!m_radioButtonOn) {
        m_radioButtonOn = Image::loadPlatformResource("radioButtonOn");
        m_radioButtonOff = Image::loadPlatformResource("radioButtonOff");
    }    
    paintButtonDecorations(o, i, r);
    MScrollView* wv = scrollView(i);
    // don't scale the checkbox if scalingFactor is 100% or  we are in the process of scaling another image
    if (wv->scalingFactor() <= 100  || m_bitmapScaler) {
        i.context->drawImage(isChecked(o)?m_radioButtonOn:m_radioButtonOff,r);
    }
    // use the cached scaled image if it has the same scaling factor
    else if (wv->scalingFactor() == (isChecked(o) ? m_scalingForRadioOn : m_scalingForRadioOff)) {
        i.context->drawImage(isChecked(o) ? m_scaledRadioOn : m_scaledRadioOff, r);
    }
    // scale
    else {
        scaleImage(isChecked(o) ? ERadioOn : ERadioOff, wv->scalingFactor());
        if (wv->scalingFactor() == (isChecked(o) ? m_scalingForRadioOn : m_scalingForRadioOff)) {
            i.context->drawImage(isChecked(o) ? m_scaledRadioOn : m_scaledRadioOff, r);
        }
        else {        
            i.context->drawImage(isChecked(o)?m_radioButtonOn:m_radioButtonOff,r);
        }
    }
    return false;
}

bool RenderThemeSymbian::isControlStyled(const RenderStyle* style, const BorderData& border,
                                     const BackgroundLayer& background, const Color& backgroundColor) const
{
    if (style->appearance() == TextFieldAppearance || style->appearance() == TextAreaAppearance)
        return style->border() != border;

    return RenderTheme::isControlStyled(style, border, background, backgroundColor);
}

bool RenderThemeSymbian::controlSupportsTints(const RenderObject* o) const
{
    return false;
}

void RenderThemeSymbian::addIntrinsicMargins(RenderStyle* style) const
{
    // Cut out the intrinsic margins completely if we end up using a small font size
    if (style->fontSize() < 11)
        return;

    // Intrinsic margin value.
    const int m = 2;

    // FIXME: Using width/height alone and not also dealing with min-width/max-width is flawed.
    if (style->width().isIntrinsicOrAuto()) {
        if (style->marginLeft().quirk())
            style->setMarginLeft(Length(m, Fixed));

        if (style->marginRight().quirk())
            style->setMarginRight(Length(m, Fixed));
    }

    if (style->height().isAuto()) {
        if (style->marginTop().quirk())
            style->setMarginTop(Length(m, Fixed));

        if (style->marginBottom().quirk())
            style->setMarginBottom(Length(m, Fixed));
    }
}

void RenderThemeSymbian::setCheckboxSize(RenderStyle* style) const
{
    // If the width and height are both specified, then we have nothing to do.
    if (!style->width().isIntrinsicOrAuto() && !style->height().isAuto())
        return;

    if (style->width().isIntrinsicOrAuto())
        style->setWidth(Length(m_checkBoxOn->width(), Fixed));

    if (style->height().isAuto())
        style->setHeight(Length(m_checkBoxOn->height(), Fixed));
}

void RenderThemeSymbian::setRadioSize(RenderStyle* style) const
{
    // This is the same as checkboxes.
    setCheckboxSize(style);
}

bool RenderThemeSymbian::supportsFocus(EAppearance appearance) const
{
    return false;
}


void RenderThemeSymbian::adjustButtonStyle(CSSStyleSelector* selector, RenderStyle* style, Element* e) const
{
    addIntrinsicMargins(style);
}

bool RenderThemeSymbian::paintButton(RenderObject* o, const RenderObject::PaintInfo& i, const IntRect& r)
{
    i.context->save();
    IntRect clipRect(intersection(r, i.rect));
    i.context->clip(clipRect);
    EAppearance appearance = o->style()->appearance();    
    IntRect innerRect(r);
    IntSize cr(1,1);
    for (int j = 4; j > 0; --j) {
        innerRect.setSize(innerRect.size() - IntSize(1,1));
        i.context->fillRoundedRect(innerRect, IntSize(1,1), IntSize(1,1), IntSize(1,1), IntSize(1,1), Color(226 - j*10, 226 - j*10, 226 - j*2));
    }

    i.context->restore();
    return false;
}

void RenderThemeSymbian::adjustTextFieldStyle(CSSStyleSelector*, RenderStyle* style, Element*) const
{
    addIntrinsicMargins(style);
}

bool RenderThemeSymbian::paintTextField(RenderObject* o, const RenderObject::PaintInfo& i, const IntRect& r)
{
    i.context->setStrokeColor(Color::black);
    i.context->setStrokeThickness(1.0f);
    i.context->setFillColor(Color::transparent);
    i.context->drawRect(r);
    return false;
}

bool RenderThemeSymbian::paintTextArea(RenderObject* o, const RenderObject::PaintInfo& i, const IntRect& r)
{
    i.context->setStrokeColor(Color::black);
    i.context->setStrokeThickness(1.0f);
    i.context->setFillColor(Color::transparent);
    i.context->drawRect(r);
    return false;
}

bool RenderThemeSymbian::paintMenuListButton(RenderObject* o, const RenderObject::PaintInfo& i, const IntRect& r)
{          
    if (!m_selectArrow) {
        m_selectArrow = Image::loadPlatformResource("selectArrow");
    }
    IntSize s = m_selectArrow->size();

    // draw the bounding area
    i.context->save();
    //i.context->setPen(Pen(Color::black,1));
    i.context->setFillColor(Color::lightGray);
    
    IntRect brect(r.x() + r.width() - s.width(), r.y(), s.width(), r.height() - 3 );
    i.context->drawRect(r);
    paintButtonDecorations(o, i, r);

    // draw the arrow 
    int y = (r.height() - s.height())/2 + r.y();
    IntRect arrowRect(IntPoint(r.x() + r.width() - s.width(), y), s);
    MScrollView* wv = scrollView(i);
    // don't scale the checkbox if scalingFactor is 100% or  we are in the process of scaling another image
    if (wv->scalingFactor() <= 100  || m_bitmapScaler) {
        i.context->drawImage(m_selectArrow, arrowRect);
    }
    // use the cached scaled image if it has the same scaling factor
    else if (wv->scalingFactor() == (m_scalingForSelectArrow)) {
        i.context->drawImage(m_scaledSelectArrow, arrowRect);
    }
    // scale
    else {
        scaleImage(ESelectArrow, wv->scalingFactor());
        if (wv->scalingFactor() == m_scalingForSelectArrow) {
            i.context->drawImage(m_scaledSelectArrow, arrowRect);
        }
        else {        
            i.context->drawImage(m_selectArrow, arrowRect);
        }
    }
    i.context->restore();
    return false;    
}

bool RenderThemeSymbian::paintMenuList(RenderObject* o, const RenderObject::PaintInfo& i, const IntRect& r)
{
    EAppearance appearance = o->style()->appearance();
    i.context->save();
    i.context->setStrokeColor(Color::black);
    i.context->setStrokeThickness(1.0f);
    i.context->setFillColor(Color::lightGray);
    i.context->drawRect(r);
    
    if (!m_selectArrow) {
        m_selectArrow = Image::loadPlatformResource("selectArrow");
    }    

    MScrollView* wv = scrollView(i);
    IntSize s = m_selectArrow->size();
    int y = (r.height() - s.height())/2 + r.y();
    IntRect imrect(IntPoint(r.x()+r.width()-s.width(), y), s);
    // don't scale the checkbox if scalingFactor is 100% or  we are in the process of scaling another image
    if (wv->scalingFactor() <= 100  || m_bitmapScaler) {
        i.context->drawImage(m_selectArrow, imrect);
    }
    // use the cached scaled image if it has the same scaling factor
    else if (wv->scalingFactor() == (m_scalingForSelectArrow)) {
        i.context->drawImage(m_scaledSelectArrow, imrect);
    }
    // scale
    else {
        scaleImage(ESelectArrow, wv->scalingFactor());
        if (wv->scalingFactor() == m_scalingForSelectArrow) {
            i.context->drawImage(m_scaledSelectArrow, imrect);
        }
        else {        
            i.context->drawImage(m_selectArrow, imrect);
        }
    }
    i.context->restore();
    return false;   
}

void RenderThemeSymbian::adjustMenuListStyle(CSSStyleSelector* selector, RenderStyle* style, Element* e) const
{
    style->setPaddingLeft(Length(3, Fixed));
    style->setPaddingRight(Length(20, Fixed));
    style->setPaddingTop(Length(3, Fixed));
    style->setPaddingBottom(Length(3, Fixed));
}

void RenderThemeSymbian::adjustMenuListButtonStyle(CSSStyleSelector* selector, RenderStyle* style, Element* e) const
{
    style->setPaddingLeft(Length(3, Fixed));
    style->setPaddingRight(Length(20, Fixed));
    style->setPaddingTop(Length(3, Fixed));
    style->setPaddingBottom(Length(3, Fixed));
}

void RenderThemeSymbian::paintButtonDecorations(RenderObject* o, const RenderObject::PaintInfo& i, const IntRect& r)
{
    RenderBox* b = static_cast<RenderBox*>(o);
    int my = max(r.y(), i.rect.y());
    int mh;
    if (r.y() < i.rect.y())
        mh = max(0, r.height() - (i.rect.y() - r.y()));
    else
        mh = std::min(i.rect.height(), r.height());

    if (o->style()->hasBackground()) {
        b->paintBackgroundExtended(i.context, o->style()->backgroundColor(), o->style()->backgroundLayers(), my, mh, r.x(), r.y(), r.width(), r.height());
    }
    if (o->style()->hasBorder()) {
        b->paintBorder(i.context, r.x(), r.y(), r.width(), r.height(), o->style());
    }
}

bool RenderThemeSymbian::supportsFocusRing(const RenderStyle* style) const
{
    // force webcore to draw focus ring.
    return false;
}

void RenderThemeSymbian::scaleImage(int type, int scalingFactor)
{
    TRAP_IGNORE(scaleImageL(type, scalingFactor));
    if (m_bitmapScaler) {
        // should happen only as result of a leave
        delete m_bitmapScaler;
        m_bitmapScaler = NULL;
    }
}

void RenderThemeSymbian::scaleImageL(int type, int scalingFactor)
{
    Image* image = NULL;
    switch (type) {
    case ECheckBoxOn:
        image = m_checkBoxOn;
        break;
    case ECheckBoxOff:
        image = m_checkBoxOff;
        break;
    case ERadioOn:
        image = m_radioButtonOn;
        break;
    case ERadioOff:
        image = m_radioButtonOff;
        break;
    case ESelectArrow:
        image = m_selectArrow;
        break;
    }
    CMaskedBitmap* maskedBitmap = image->getMaskedBitmap();
    if (!maskedBitmap) User::Leave(KErrGeneral); // should not really happen
    TSize size = maskedBitmap->SizeInPixels();
    size.iWidth = (size.iWidth * scalingFactor) / 100;
    size.iHeight = (size.iHeight * scalingFactor) / 100;
    SyncScaler syncScaler;
    TRequestStatus* status = &(syncScaler.iStatus);

    m_bitmapScaler = CBitmapScaler::NewL();
    m_bitmapScaler->SetQualityAlgorithm(CBitmapScaler::EMaximumQuality);
    // bitmap;
    CFbsBitmap* bitmap = const_cast<CFbsBitmap*>(&(maskedBitmap->Bitmap()));
    CFbsBitmap* resultBitmap = NULL;
    resultBitmap = new (ELeave) CFbsBitmap;
    CleanupStack::PushL(resultBitmap);
    resultBitmap->Create(size, EColor16M);
    syncScaler.init();
    m_bitmapScaler->Scale(status, *bitmap, *resultBitmap, ETrue);
    if (!m_asw) m_asw = new (ELeave) CActiveSchedulerWait();
    m_asw->Start();
    // If an error occured during scaling, stop!
    User::LeaveIfError(syncScaler.m_error);

    // mask
    CFbsBitmap* mask = const_cast<CFbsBitmap*>(&(maskedBitmap->Mask()));
    CFbsBitmap* resultMask = NULL;
    resultMask = new (ELeave) CFbsBitmap;
    CleanupStack::PushL(resultMask);
    resultMask->Create(size, EGray256);
    syncScaler.init();
    m_bitmapScaler->Scale(status, (*mask), *resultMask, ETrue);
    if (!m_asw) m_asw = new (ELeave) CActiveSchedulerWait();
    m_asw->Start();
    // If an error occured during scaling, stop!
    User::LeaveIfError(syncScaler.m_error);

    CMaskedBitmap* mb = NULL;
    mb = new(ELeave) CMaskedBitmap(resultBitmap, resultMask);
    CleanupStack::Pop(2); // resultBitmap, resultMask
    CleanupStack::PushL(mb);
    BitmapImage* bi = new (ELeave) BitmapImage(mb);
    CleanupStack::Pop(); // mb
    switch (type) {
    case ECheckBoxOn:
        delete m_scaledCheckBoxOn;
        m_scaledCheckBoxOn = bi;
        m_scalingForCheckBoxOn = scalingFactor;
        break;
    case ECheckBoxOff:
        delete m_scaledCheckBoxOff;
        m_scaledCheckBoxOff = bi;
        m_scalingForCheckBoxOff = scalingFactor;
        break;
    case ERadioOn:
        delete m_scaledRadioOn;
        m_scaledRadioOn = bi;
        m_scalingForRadioOn = scalingFactor;
        break;
    case ERadioOff:
        delete m_scaledRadioOff;
        m_scaledRadioOff = bi;
        m_scalingForRadioOff = scalingFactor;
        break;
    case ESelectArrow:
        delete m_scaledSelectArrow;
        m_scaledSelectArrow = bi;
        m_scalingForSelectArrow = scalingFactor;
        break;
    }
    delete m_bitmapScaler;
    m_bitmapScaler = NULL;
    delete m_asw;
    m_asw = NULL;
}

void RenderThemeSymbian::run()
{
    m_asw->AsyncStop();
}

void RenderThemeSymbian::cancel()
{
    // Since the operation is very fast, not sure what could cause it to be canceled.
    if (m_bitmapScaler) {
        m_bitmapScaler->Cancel();
    }
    if (m_asw && m_asw->IsStarted()) {
        m_asw->AsyncStop();
    }
}

}
