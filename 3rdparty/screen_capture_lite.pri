INCLUDEPATH +=  $$PWD/screen_capture_lite/include/

HEADERS += \
    $$PWD/screen_capture_lite/include/internal/SCCommon.h \
    $$PWD/screen_capture_lite/include/internal/ThreadManager.h \
    $$PWD/screen_capture_lite/include/ScreenCapture.h

SOURCES += \
    $$PWD/screen_capture_lite/src/SCCommon.cpp \
    $$PWD/screen_capture_lite/src/ScreenCapture.cpp \
    $$PWD/screen_capture_lite/src/ThreadManager.cpp \

linux {
    HEADERS += \
        $$PWD/screen_capture_lite/include/linux/X11FrameProcessor.h \
        $$PWD/screen_capture_lite/include/linux/X11MouseProcessor.h

    SOURCES += \
        $$PWD/screen_capture_lite/src/linux/GetMonitors.cpp \
        $$PWD/screen_capture_lite/src/linux/GetWindows.cpp \
        $$PWD/screen_capture_lite/src/linux/ThreadRunner.cpp \
        $$PWD/screen_capture_lite/src/linux/X11FrameProcessor.cpp \
        $$PWD/screen_capture_lite/src/linux/X11MouseProcessor.cpp
}
windows {
    HEADERS += \
        $$PWD/screen_capture_lite/include/windows/DXFrameProcessor.h \
        $$PWD/screen_capture_lite/include/windows/GDIFrameProcessor.h \
        $$PWD/screen_capture_lite/include/windows/GDIHelpers.h \
        $$PWD/screen_capture_lite/include/windows/GDIMouseProcessor.h

    SOURCES += \
        $$PWD/screen_capture_lite/src/windows/DXFrameProcessor.cpp \
        $$PWD/screen_capture_lite/src/windows/GDIFrameProcessor.cpp \
        $$PWD/screen_capture_lite/src/windows/GDIMouseProcessor.cpp \
        $$PWD/screen_capture_lite/src/windows/GDIWindowProcessor.cpp \
        $$PWD/screen_capture_lite/src/windows/GetMonitors.cpp \
        $$PWD/screen_capture_lite/src/windows/GetWindows.cpp \
        $$PWD/screen_capture_lite/src/windows/ThreadRunner.cpp

    LIBS += -lDwmapi
}
ios {
    HEADERS += \
        $$PWD/screen_capture_lite/include/ios/CGFrameProcessor.h \
        $$PWD/screen_capture_lite/include/ios/NSFrameProcessor.h \
        $$PWD/screen_capture_lite/include/ios/NSFrameProcessorm.h \
        $$PWD/screen_capture_lite/include/ios/NSMouseCapture.h \
        $$PWD/screen_capture_lite/include/ios/NSMouseProcessor.h

SOURCES += \
        $$PWD/screen_capture_lite/src/ios/CGFrameProcessor.cpp \
        $$PWD/screen_capture_lite/src/ios/GetMonitors.cpp \
        $$PWD/screen_capture_lite/src/ios/GetWindows.cpp \
        $$PWD/screen_capture_lite/src/ios/NSFrameProcessor.cpp \
        $$PWD/screen_capture_lite/src/ios/NSMouseProcessor.cpp \
        $$PWD/screen_capture_lite/src/ios/ThreadRunner.cpp
        $$PWD/screen_capture_lite/src/ios/NSFrameProcessor.mm

    DISTFILES += \
        $$PWD/screen_capture_lite/src/ios/NSMouseCapture.m
}


