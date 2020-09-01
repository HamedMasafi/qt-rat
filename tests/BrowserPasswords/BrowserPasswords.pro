QT       += core gui sql widgets

TARGET = BrowserPasswords
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS
INCLUDEPATH +=../../client/
CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    ../../server/core/variantlistmodel.cpp

HEADERS += \
        mainwindow.h \
    ../../server/core/variantlistmodel.h

FORMS += \
        mainwindow.ui


    SOURCES += \
        ../../3rdparty/Qt-AES/qaesencryption.cpp

    HEADERS += \
        ../../3rdparty/Qt-AES/qaesencryption.h

#win:{
    LIBS+= -lcrypt32
    HEADERS += ../../client/rawcopy.h
    SOURCES += ../../client/rawcopy.cpp
#}

include(../../3rdparty/lz4.pri)
include(../../client/browserspy/browserspy.pri)
