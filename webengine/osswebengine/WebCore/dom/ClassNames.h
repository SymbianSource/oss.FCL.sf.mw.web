/*
  This file is part of the DOM implementation for KDE.
 
  Copyright (C) 2007 Apple Computer, Inc.
 
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB. If not, write to
 *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */
 
#ifndef ClassNames_h
#define ClassNames_h 
 
 #include "AtomicString.h"
 #include <wtf/Vector.h>
 #include <wtf/OwnPtr.h> 
 
 namespace WebCore {
class ClassNames {
public:
    ClassNames()
    : m_nameVector(0)
    {
    }
 
    bool contains(const AtomicString&) const;
    void parseClassAttribute(const String&, const bool inCompatMode);
 
    size_t size() const { return m_nameVector ? m_nameVector->size() : 0; }
    void clear() { if (m_nameVector) m_nameVector->clear(); }
    const AtomicString& operator[](size_t i) const { return m_nameVector ? (*m_nameVector)[i] : nullAtom; }
 
private:
    OwnPtr<Vector<AtomicString> > m_nameVector;
};
 
inline static bool isClassWhitespace(UChar c)
    {
    return c == ' ' || c == '\r' || c == '\n' || c == '\t' || c == '\f';
    }
} // namespace WebCore

 #endif // ClassNames_h  