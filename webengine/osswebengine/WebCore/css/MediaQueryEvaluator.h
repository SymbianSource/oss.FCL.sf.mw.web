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


#ifndef MediaQueryEvaluator_h
#define MediaQueryEvaluator_h

#include "PlatformString.h"

namespace WebCore {
class Page;
class RenderStyle;
class MediaList;
class MediaQueryExp;

/**
 * Class that evaluates css media queries as defined in
 * CSS3 Module "Media Queries" (http://www.w3.org/TR/css3-mediaqueries/)
 * Special constructors are needed, if simple media queries are to be
 * evaluated without knowledge of the medium features. This can happen
 * for example when parsing UA stylesheets, if evaluation is done
 * right after parsing.
 *
 * the boolean parameter is used to approximate results of evaluation, if
 * the device characteristics are not known. This can be used to prune the loading
 * of stylesheets to only those which are probable to match.
 */
class MediaQueryEvaluator
{
public:
    /** Creates evaluator which evaluates only simple media queries
     *  Evaluator returns true for "all", and returns value of \mediaFeatureResult
     *  for any media features
     */
    MediaQueryEvaluator(bool mediaFeatureResult = false);

    /** Creates evaluator which evaluates only simple media queries
     *  Evaluator  returns true for acceptedMediaType and returns value of \mediafeatureResult
     *  for any media features
     */
    MediaQueryEvaluator(const String& acceptedMediaType, bool mediaFeatureResult = false);
    MediaQueryEvaluator(const char* acceptedMediaType, bool mediaFeatureResult = false);

    /** Creates evaluator which evaluates full media queries
     */
    MediaQueryEvaluator(const String& acceptedMediaType, Page* page, RenderStyle* style);

    ~MediaQueryEvaluator();

    bool mediaTypeMatch(const String& mediaTypeToMatch) const;
    bool mediaTypeMatchSpecific(const char* mediaTypeToMatch) const;

    /** Evaluates a list of media queries */
    bool eval(const MediaList* query) const;

    /** Evaluates media query subexpression, ie "and (media-feature: value)" part */
    bool eval(const MediaQueryExp* expr) const;

private:
    String m_mediaType;
    Page* m_page; // not owned
    RenderStyle* m_style; // not owned
    bool m_expResult;
};

} // namespace
#endif
