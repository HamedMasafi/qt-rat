#ifndef VNCDIALOG_H
#define VNCDIALOG_H

#include "ui_vncdialog.h"
#include "../core/controllerbase.h"

class RemoteView;
class QUdpSocket;
class VNCDialog : public ControllerBase, private Ui::VNCDialog
{
    Q_OBJECT

    QUdpSocket *_socket;
    QSize _resulation;
    int _port;
    QImage::Format _format;
    RemoteView *widgetView;
    QByteArray buffer;

public:
    explicit VNCDialog(AbstractClient *client, const QSize &resulation,
                       const int &port,
                       const QImage::Format &format, QWidget *parent = nullptr);
    ~VNCDialog();

protected:
    void changeEvent(QEvent *e);
private slots:
    void on_widgetView_mouseClick(const QPoint &pt, const Qt::MouseButton &button);
    void on_widgetView_mousePress(const QPoint &pt, const Qt::MouseButton &button);
    void on_widgetView_mouseRelease(const QPoint &pt, const Qt::MouseButton &button);
    void on_widgetView_mouseMove(const QPoint &pt, const Qt::MouseButton &button);
    void on_widgetView_mouseDoubleClick(const QPoint &pt, const Qt::MouseButton &button);
    void on_widgetView_keyPress(Qt::Key key, Qt::KeyboardModifiers modifs);
    void on_widgetView_keyRelease(int key, Qt::KeyboardModifiers modifs);


    void readPendingDatagrams();
    void start();
    void on_pushButtonLock_clicked(bool checked);
    void processPacket(const QByteArray &data);

    // QWidget interface
protected:
    bool focusNextPrevChild(bool next);
};

#endif // VNCDIALOG_H
