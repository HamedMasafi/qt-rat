DEFINES += SOCKSLIB_LIBRARY

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/SocksSessionManager.h \
    $$PWD/SocksSession.h \
    $$PWD/SocksConnection.h \
    $$PWD/sockshub.h \
    $$PWD/decorators/ThrottlingDecorator.h \
    $$PWD/decorators/QIODeviceDecorator.h \
    $$PWD/protocol/SocksReplyMessage4a.h \
    $$PWD/protocol/SocksProtocolMessage.h \
    $$PWD/protocol/Socks5UDPRequestMessage.h \
    $$PWD/protocol/Socks5RequestMessage.h \
    $$PWD/protocol/Socks5ReplyMessage.h \
    $$PWD/protocol/Socks5MethodSelectionMessage.h \
    $$PWD/protocol/Socks5GreetingMessage.h \
    $$PWD/protocol/Socks4RequestMessage.h \
    $$PWD/protocol/Socks4ReplyMessage.h \
    $$PWD/states/SocksState.h \
    $$PWD/states/Socks5UDPAssociateState.h \
    $$PWD/states/Socks5UDPAssociatedState.h \
    $$PWD/states/Socks5ConnectState.h \
    $$PWD/states/Socks5ConnectedState.h \
    $$PWD/states/Socks5AuthState.h \
    $$PWD/states/Socks5AuthenticatedState.h \
    $$PWD/states/Socks4InitialState.h \
    $$PWD/states/Socks4ConnectState.h \
    $$PWD/states/Socks4ConnectedState.h \
    $$PWD/states/InitialState.h \
    $$PWD/SocksLib_global.h \
    $$PWD/SocksServer.h \
    $$PWD/decorators/QTcpSocketDecorator.h

SOURCES += \
    $$PWD/SocksSessionManager.cpp \
    $$PWD/SocksSession.cpp \
    $$PWD/SocksConnection.cpp \
    $$PWD/sockshub.cpp \
    $$PWD/decorators/ThrottlingDecorator.cpp \
    $$PWD/decorators/QIODeviceDecorator.cpp \
    $$PWD/protocol/SocksReplyMessage4a.cpp \
    $$PWD/protocol/SocksProtocolMessage.cpp \
    $$PWD/protocol/Socks5UDPRequestMessage.cpp \
    $$PWD/protocol/Socks5RequestMessage.cpp \
    $$PWD/protocol/Socks5ReplyMessage.cpp \
    $$PWD/protocol/Socks5MethodSelectionMessage.cpp \
    $$PWD/protocol/Socks5GreetingMessage.cpp \
    $$PWD/protocol/Socks4RequestMessage.cpp \
    $$PWD/protocol/Socks4ReplyMessage.cpp \
    $$PWD/states/SocksState.cpp \
    $$PWD/states/Socks5UDPAssociateState.cpp \
    $$PWD/states/Socks5UDPAssociatedState.cpp \
    $$PWD/states/Socks5ConnectState.cpp \
    $$PWD/states/Socks5ConnectedState.cpp \
    $$PWD/states/Socks5AuthState.cpp \
    $$PWD/states/Socks5AuthenticatedState.cpp \
    $$PWD/states/Socks4InitialState.cpp \
    $$PWD/states/Socks4ConnectState.cpp \
    $$PWD/states/Socks4ConnectedState.cpp \
    $$PWD/states/InitialState.cpp \
    $$PWD/SocksServer.cpp \
    $$PWD/decorators/QTcpSocketDecorator.cpp
