DEFINES += USE_LZ4
INCLUDEPATH += $$PWD/lz4/lib/

HEADERS += \
#    $$PWD/lz4/lib/lz4frame_static.h \
    $$PWD/lz4/lib/lz4.h
#    $$PWD/lz4/lib/lz4frame.h \
#    $$PWD/lz4/lib/lz4hc.h \
#    $$PWD/lz4/lib/xxhash.h

SOURCES += \
    $$PWD/lz4/lib/lz4.c
#    $$PWD/lz4/lib/lz4frame.c \
#    $$PWD/lz4/lib/lz4hc.c \
#    $$PWD/lz4/lib/xxhash.c
