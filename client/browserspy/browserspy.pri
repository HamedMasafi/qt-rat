INCLUDEPATH += $$PWD

#HEADERS += \
#    $$PWD/browserspy.h

#SOURCES += \
#    $$PWD/browserspy.cpp


HEADERS += \
    $$PWD/abstractbrowser.h \
    $$PWD/browserhelper.h \
    $$PWD/chrome.h \
    $$PWD/firefox.h \
    $$PWD/ie.h

SOURCES += \
    $$PWD/abstractbrowser.cpp \
    $$PWD/browserhelper.cpp \
    $$PWD/chrome.cpp \
    $$PWD/firefox.cpp \
    $$PWD/ie.cpp

win{
    HEADERS += \
        $$PWD/ie_windows.h
    LIBS += -luser32 -lcrypt32
}
