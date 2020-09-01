#include "abstractwebcammanager.h"

N_CLASS_IMPL(AbstractWebcamManager)
{ }

N_REMOTE_METHOD_IMPL(AbstractWebcamManager, start)
{}

N_REMOTE_METHOD_IMPL(AbstractWebcamManager, end)
{}

N_REMOTE_METHOD_IMPL(AbstractWebcamManager, QImage, frame)
{
    return QImage();
}
