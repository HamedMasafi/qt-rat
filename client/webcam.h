#ifndef WEBCAM_H
#define WEBCAM_H

#include "../common/abstractwebcammanager.h"

#include <QAbstractVideoSurface>

class CameraFrameGrabber : public QAbstractVideoSurface
{
    Q_OBJECT
public:
    explicit CameraFrameGrabber(QObject *parent = 0);

    QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const;

    bool present(const QVideoFrame &frame);

signals:
    void frameAvailable(QImage frame);

public slots:
};

class QCamera;
class Webcam : public AbstractWebcamManager
{
    Q_OBJECT

    QCamera *_camera;
public:
    Webcam();

    // AbstractWebcamManager interface
public slots:
    void startSlot();
    void endSlot();
    QImage frameSlot();
};

#endif // WEBCAM_H
