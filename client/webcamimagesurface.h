#ifndef WEBCAMIMAGESURFACE_H
#define WEBCAMIMAGESURFACE_H

#include <QAbstractVideoSurface>
#include <QImage>


class QCamera;
class WebcamImageSurface: public QAbstractVideoSurface {
    Q_OBJECT
    QVideoFrame lastFrame;
    QCamera *cam;

public:
    WebcamImageSurface(QByteArray deviceName, QObject *parent = nullptr);

    QList<QVideoFrame::PixelFormat>
    supportedPixelFormats(QAbstractVideoBuffer::HandleType type) const;

    bool present(const QVideoFrame& frame);
    QImage image() const;
};

#endif // WEBCAMIMAGESURFACE_H
