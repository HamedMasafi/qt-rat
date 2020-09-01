SOURCES += \
    keylogger_windows.cpp \
    rawcopy.cpp \
    regkey.cpp
HEADERS += \
    rawcopy.h \
    regkey.h
LIBS += -luser32 -lcrypt32
