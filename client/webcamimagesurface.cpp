#include "webcamimagesurface.h"

#include <QCamera>

WebcamImageSurface::WebcamImageSurface(QByteArray deviceName, QObject *parent) : QAbstractVideoSurface(parent)
{
    if (deviceName.isEmpty())
        cam = new QCamera(this);
    else
        cam = new QCamera(deviceName, this);
    cam->setViewfinder(this);
    cam->setCaptureMode(QCamera::CaptureViewfinder);
    cam->start();
}

QList<QVideoFrame::PixelFormat> WebcamImageSurface::supportedPixelFormats(QAbstractVideoBuffer::HandleType type) const
{
    Q_UNUSED(type)
    return QList<QVideoFrame::PixelFormat>()
        << QVideoFrame::Format_RGB24
        << QVideoFrame::Format_RGB32; // here return whatever formats you will handle
}

bool WebcamImageSurface::present(const QVideoFrame &frame)
{
    if (frame.isValid()) {
        lastFrame = QVideoFrame(frame);
        return true;
    }
    return false;
}

QImage WebcamImageSurface::image() const
{
    QVideoFrame cloneFrame(lastFrame);
    cloneFrame.map(QAbstractVideoBuffer::ReadOnly);
    const QImage img(cloneFrame.bits(),
                     cloneFrame.width(),
                     cloneFrame.height(),
                     QVideoFrame::imageFormatFromPixelFormat(cloneFrame.pixelFormat()));

    cloneFrame.unmap();
#ifdef Q_OS_WIN
    return img.mirrored();
#else
    return img;
#endif
}
