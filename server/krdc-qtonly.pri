DEFINES += QTONLY

HEADERS += \
    $$PWD/../3rdparty/krdc/core/remoteview.h \
#    $$PWD/../3rdparty/krdc/core/remoteviewfactory.h \
    $$PWD/../3rdparty/krdc/vnc/vncclientthread.h \
    $$PWD/../3rdparty/krdc/vnc/vncview.h
#    $$PWD/../3rdparty/krdc/vnc/vncviewfactory.h

SOURCES += \
    $$PWD/../3rdparty/krdc/core/remoteview.cpp \
#    $$PWD/../3rdparty/krdc/core/remoteviewfactory.cpp \
    $$PWD/../3rdparty/krdc/vnc/vncclientthread.cpp \
    $$PWD/../3rdparty/krdc/vnc/vncview.cpp
#    $$PWD/../3rdparty/krdc/vnc/vncviewfactory.cpp

INCLUDEPATH +=  \
    $$PWD/../3rdparty/krdc/core/ \
    $$PWD/../3rdparty/krdc/vnc/
