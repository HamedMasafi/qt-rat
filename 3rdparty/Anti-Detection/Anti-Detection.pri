
win32 {

HEADERS += \
    $$PWD/Anti-dbg-rce.h \
    $$PWD/AntiDebug.h \
    $$PWD/anti_debugger.h \
    $$PWD/anti_virtualmachine.h \
    $$PWD/vm_detection.h

SOURCES += \
    $$PWD/AntiDebug.cpp \
    $$PWD/vm_detection.cpp

}
