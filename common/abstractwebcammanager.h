#ifndef ABSTRACTWEBCAMMANAGER_H
#define ABSTRACTWEBCAMMANAGER_H

#include <SharedObject>
#include <QImage>

class AbstractWebcamManager : public Neuron::SharedObject
{
    Q_OBJECT

public:
    N_CLASS_DECL(AbstractWebcamManager)

    N_REMOTE_METHOD_DECL(start)
    N_REMOTE_METHOD_DECL(end)
    N_REMOTE_METHOD_DECL(QImage, frame)
};

#endif // ABSTRACTWEBCAMMANAGER_H
