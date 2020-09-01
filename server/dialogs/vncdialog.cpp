#include "vncdialog.h"
//#include "surfacewidget.h"
#include "../common/abstractclient.h"
#include "remoteview.h"
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QBuffer>
#include <QPainter>
#include <QTimer>
//#include <QMediaPlayer>
//#include <QVideoWidget>
//#include <vncview.h>

//#include <widgets/WidgetVideo.h>
//#include <core/MediaPlayer.h>
//#include <core/Instance.h>
//#include <core/Media.h>

VNCDialog::VNCDialog(AbstractClient *client, const QSize &resulation, const int &port, const QImage::Format &format, QWidget *parent) :
    ControllerBase(client, parent), _resulation(resulation), _port(port), _format(format)
{
    widgetView = new RemoteView(this);
    Q_SET_OBJECT_NAME(widgetView);

    setupUi(this);

    widgetView->resize(_resulation.width(), _resulation.height());
    scrollArea->setWidget(widgetView);

    _socket = new QUdpSocket(this);
    connect(_socket, &QUdpSocket::readyRead, this, &VNCDialog::readPendingDatagrams);
    bool ok = _socket->bind(static_cast<quint16>(port), QAbstractSocket::ShareAddress);

    if (!ok)
        qDebug() << "Unable to open rdp";

    QTimer::singleShot(2000, this, &VNCDialog::start);

    widgetView->setLockMouse(true);
    widgetView->setLockKeyboard(true);

    widgetView->setFocus();

    Qt::WindowFlags flags = windowFlags();
    flags |= Qt::WindowMaximizeButtonHint;
    flags |= Qt::WindowMinimizeButtonHint;
    setWindowFlags(flags);
}

VNCDialog::~VNCDialog()
{
    _socket->close();
    if (client())
        client()->endRemoteDesktop();
}

void VNCDialog::readPendingDatagrams()
{
    qDebug() << "datagram recived";
    while (_socket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = _socket->receiveDatagram();
        buffer.append(qUncompress(datagram.data()));
    }

    int i;
    while ((i = buffer.indexOf("***")) != -1) {
        auto b = buffer.mid(0, i);
        processPacket(b);
        buffer = buffer.remove(0, i + 3);
    }
}

void VNCDialog::start()
{
    client()->startRemoteDesktop(_resulation.width(), _resulation.height(), _format, _port)->wait();
}

void VNCDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}

void VNCDialog::on_widgetView_mouseClick(const QPoint &pt, const Qt::MouseButton &button)
{
    client()->mouseClick(pt, static_cast<int>(button));
}

void VNCDialog::on_widgetView_mousePress(const QPoint &pt, const Qt::MouseButton &button)
{
    qDebug() << "mouse press";
    client()->mousePress(pt, static_cast<int>(button));
}

void VNCDialog::on_widgetView_mouseRelease(const QPoint &pt, const Qt::MouseButton &button)
{
    qDebug() << "mouse release";
    client()->mouseRelease(pt, static_cast<int>(button));
}

void VNCDialog::on_widgetView_mouseMove(const QPoint &pt, const Qt::MouseButton &button)
{
    client()->mouseMove(pt, static_cast<int>(button));
}

void VNCDialog::on_widgetView_mouseDoubleClick(const QPoint &pt, const Qt::MouseButton &button)
{
    Q_UNUSED(pt)
    Q_UNUSED(button)
//    client()->doubleClickMouse(pt, Qt::LeftButton);
}

void VNCDialog::on_widgetView_keyPress(Qt::Key key, Qt::KeyboardModifiers modifs)
{
    qDebug() << "key press" << key;
    client()->keyPress(key, static_cast<int>(modifs));
}

void VNCDialog::on_widgetView_keyRelease(int key, Qt::KeyboardModifiers modifs)
{
    Q_UNUSED(key)
    Q_UNUSED(modifs)
}

void VNCDialog::on_pushButtonLock_clicked(bool checked)
{
    widgetView->setLockMouse(checked);
    widgetView->setLockKeyboard(checked);
    pushButtonLock->setIcon(QIcon(checked ? ":/icons/lock.png" : ":/icons/lock-open.png"));
}

void VNCDialog::processPacket(const QByteArray &data)
{
//    QByteArray imData = qUncompress(data);

    int pos = data.indexOf(" ");
    if (pos == -1)
        return;

    auto meta = data.mid(0, pos);
    auto imageData = data.mid(pos + 1);
    auto command = meta.mid(0, 1);
    auto x = meta.mid(1, meta.indexOf("/") - 1).toInt();
    auto y = meta.mid(meta.indexOf("/") + 1).toInt();

    if (command == "D") {
        QBuffer buffer(&imageData, this);
        buffer.open(QIODevice::ReadOnly);
        QImage img;
        img.load(&buffer, "JPG");

        //            p->drawImage(x, y, img);
        widgetView->drawImage(x, y, img);
    }
    if (command == "I") {
        //            img = new QImage(x, y, QImage::Format_ARGB32);
        //            p = new QPainter(img);
        //            img->fill(Qt::red);
        widgetView->resize(x, y);
    }
    if (/*img && !img->isNull() && */command == "S") {
        widgetView->update();
        //            i = 0;
        //            labelImage->setPixmap(QPixmap::fromImage(*img));
        //            qApp->processEvents();
    }
}

bool VNCDialog::focusNextPrevChild(bool next)
{
    Q_UNUSED(next)
    widgetView->setFocus();
    return false;
}
