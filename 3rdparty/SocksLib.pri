DEFINES += SOCKSLIB_LIBRARY

INCLUDEPATH += $$PWD/Qt-Socks-Server/SocksLib

HEADERS += \
    $$PWD/Qt-Socks-Server/SocksLib/SocksSessionManager.h \
    $$PWD/Qt-Socks-Server/SocksLib/SocksSession.h \
    $$PWD/Qt-Socks-Server/SocksLib/SocksServer.h \
    $$PWD/Qt-Socks-Server/SocksLib/SocksConnection.h \
    $$PWD/Qt-Socks-Server/SocksLib/decorators/ThrottlingDecorator.h \
    $$PWD/Qt-Socks-Server/SocksLib/decorators/QIODeviceDecorator.h \
    $$PWD/Qt-Socks-Server/SocksLib/protocol/SocksReplyMessage4a.h \
    $$PWD/Qt-Socks-Server/SocksLib/protocol/SocksProtocolMessage.h \
    $$PWD/Qt-Socks-Server/SocksLib/protocol/Socks5UDPRequestMessage.h \
    $$PWD/Qt-Socks-Server/SocksLib/protocol/Socks5RequestMessage.h \
    $$PWD/Qt-Socks-Server/SocksLib/protocol/Socks5ReplyMessage.h \
    $$PWD/Qt-Socks-Server/SocksLib/protocol/Socks5MethodSelectionMessage.h \
    $$PWD/Qt-Socks-Server/SocksLib/protocol/Socks5GreetingMessage.h \
    $$PWD/Qt-Socks-Server/SocksLib/protocol/Socks4RequestMessage.h \
    $$PWD/Qt-Socks-Server/SocksLib/protocol/Socks4ReplyMessage.h \
    $$PWD/Qt-Socks-Server/SocksLib/states/SocksState.h \
    $$PWD/Qt-Socks-Server/SocksLib/states/Socks5UDPAssociateState.h \
    $$PWD/Qt-Socks-Server/SocksLib/states/Socks5UDPAssociatedState.h \
    $$PWD/Qt-Socks-Server/SocksLib/states/Socks5ConnectState.h \
    $$PWD/Qt-Socks-Server/SocksLib/states/Socks5ConnectedState.h \
    $$PWD/Qt-Socks-Server/SocksLib/states/Socks5AuthState.h \
    $$PWD/Qt-Socks-Server/SocksLib/states/Socks5AuthenticatedState.h \
    $$PWD/Qt-Socks-Server/SocksLib/states/Socks4InitialState.h \
    $$PWD/Qt-Socks-Server/SocksLib/states/Socks4ConnectState.h \
    $$PWD/Qt-Socks-Server/SocksLib/states/Socks4ConnectedState.h \
    $$PWD/Qt-Socks-Server/SocksLib/states/InitialState.h \
    $$PWD/Qt-Socks-Server/SocksLib/SocksLib_global.h \
    $$PWD/Qt-Socks-Server/SocksLib/decorators/QTcpSocketDecorator.h

SOURCES += \
    $$PWD/Qt-Socks-Server/SocksLib/SocksSessionManager.cpp \
    $$PWD/Qt-Socks-Server/SocksLib/SocksSession.cpp \
    $$PWD/Qt-Socks-Server/SocksLib/SocksServer.cpp \
    $$PWD/Qt-Socks-Server/SocksLib/SocksConnection.cpp \
    $$PWD/Qt-Socks-Server/SocksLib/decorators/ThrottlingDecorator.cpp \
    $$PWD/Qt-Socks-Server/SocksLib/decorators/QIODeviceDecorator.cpp \
    $$PWD/Qt-Socks-Server/SocksLib/protocol/SocksReplyMessage4a.cpp \
    $$PWD/Qt-Socks-Server/SocksLib/protocol/SocksProtocolMessage.cpp \
    $$PWD/Qt-Socks-Server/SocksLib/protocol/Socks5UDPRequestMessage.cpp \
    $$PWD/Qt-Socks-Server/SocksLib/protocol/Socks5RequestMessage.cpp \
    $$PWD/Qt-Socks-Server/SocksLib/protocol/Socks5ReplyMessage.cpp \
    $$PWD/Qt-Socks-Server/SocksLib/protocol/Socks5MethodSelectionMessage.cpp \
    $$PWD/Qt-Socks-Server/SocksLib/protocol/Socks5GreetingMessage.cpp \
    $$PWD/Qt-Socks-Server/SocksLib/protocol/Socks4RequestMessage.cpp \
    $$PWD/Qt-Socks-Server/SocksLib/protocol/Socks4ReplyMessage.cpp \
    $$PWD/Qt-Socks-Server/SocksLib/states/SocksState.cpp \
    $$PWD/Qt-Socks-Server/SocksLib/states/Socks5UDPAssociateState.cpp \
    $$PWD/Qt-Socks-Server/SocksLib/states/Socks5UDPAssociatedState.cpp \
    $$PWD/Qt-Socks-Server/SocksLib/states/Socks5ConnectState.cpp \
    $$PWD/Qt-Socks-Server/SocksLib/states/Socks5ConnectedState.cpp \
    $$PWD/Qt-Socks-Server/SocksLib/states/Socks5AuthState.cpp \
    $$PWD/Qt-Socks-Server/SocksLib/states/Socks5AuthenticatedState.cpp \
    $$PWD/Qt-Socks-Server/SocksLib/states/Socks4InitialState.cpp \
    $$PWD/Qt-Socks-Server/SocksLib/states/Socks4ConnectState.cpp \
    $$PWD/Qt-Socks-Server/SocksLib/states/Socks4ConnectedState.cpp \
    $$PWD/Qt-Socks-Server/SocksLib/states/InitialState.cpp \
    $$PWD/Qt-Socks-Server/SocksLib/decorators/QTcpSocketDecorator.cpp
