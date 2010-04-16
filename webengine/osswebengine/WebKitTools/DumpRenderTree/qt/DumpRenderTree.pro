#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description: 
#

TARGET = DumpRenderTree

include(../../../WebKit.pri)
INCLUDEPATH += /usr/include/freetype2
INCLUDEPATH += ../../../JavaScriptCore/kjs
DESTDIR = ../../../bin


QT = core gui
macx: QT += xml network

HEADERS = DumpRenderTree.h jsobjects.h testplugin.h
SOURCES = DumpRenderTree.cpp main.cpp jsobjects.cpp testplugin.cpp

unix:!mac {
    QMAKE_RPATHDIR = $$OUTPUT_DIR/lib $$QMAKE_RPATHDIR
}
