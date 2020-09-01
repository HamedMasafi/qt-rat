#include "QIODeviceDecorator.h"

#include <QtDebug>

QIODeviceDecorator::QIODeviceDecorator(QIODevice *toDecorate, QObject *parent) :
    QIODevice(parent), _toDecorate(toDecorate)
{
    connect(toDecorate,
            SIGNAL(aboutToClose()),
            this,
            SLOT(handleChildAboutToClose()));

    connect(toDecorate,
            SIGNAL(bytesWritten(qint64)),
            this,
            SLOT(handleChildBytesWritten(qint64)));

    connect(toDecorate,
            SIGNAL(readChannelFinished()),
            this,
            SLOT(handleChildReadChannelFinished()));

    connect(toDecorate,
            SIGNAL(readyRead()),
            this,
            SLOT(handleChildReadyRead()));

    /*
      This line is very important --- QIODevice will not call our virtual functions (readData, etc.)
      if it appears that we aren't open. isOpen() uses openMode() to decide. Make sure our openMode matches
      our child _toDecorate
    */
    this->setOpenMode(_toDecorate->openMode());

    //If _toDecorate is some other class's child, it could be deleted out from under us
    _toDecorate->setParent(this);
}

QIODeviceDecorator::~QIODeviceDecorator()
{
    qDebug() << "QIODeviceDecorator" << this << "shutting down";
    if (_toDecorate.isNull())
        return;
    _toDecorate->close();
    _toDecorate->deleteLater();
}

//virtual from QIODevice
bool QIODeviceDecorator::atEnd() const
{
    return _toDecorate->atEnd();
}

//virtual from QIODevice
qint64 QIODeviceDecorator::bytesAvailable() const
{
    return _toDecorate->bytesAvailable();
}

//virtual from QIODevice
qint64 QIODeviceDecorator::bytesToWrite() const
{
    return _toDecorate->bytesToWrite();
}

//virtual from QIODevice
bool QIODeviceDecorator::canReadLine() const
{
    return _toDecorate->canReadLine();
}

//virtual from QIODevice
void QIODeviceDecorator::close()
{
    _toDecorate->close();
    this->setOpenMode(_toDecorate->openMode());
}

//virtual from QIODevice
bool QIODeviceDecorator::isSequential() const
{
    return _toDecorate->isSequential();
}

//virtual from QIODevice
bool QIODeviceDecorator::open(QIODevice::OpenMode mode)
{
    bool toRet = _toDecorate->open(mode);
    this->setOpenMode(_toDecorate->openMode());
    return toRet;
}

//virtual from QIODevice
qint64 QIODeviceDecorator::pos() const
{
    return _toDecorate->pos();
}

//virtual from QIODevice
bool QIODeviceDecorator::reset()
{
    return _toDecorate->reset();
}

//virtual from QIODevice
bool QIODeviceDecorator::seek(qint64 pos)
{
    return _toDecorate->seek(pos);
}

//virtual from QIODevice
qint64 QIODeviceDecorator::size() const
{
    return _toDecorate->size();
}

//virtual from QIODevice
bool QIODeviceDecorator::waitForBytesWritten(int msecs)
{
    return _toDecorate->waitForBytesWritten(msecs);
}

//virtual from QIODevice
bool QIODeviceDecorator::waitForReadyRead(int msecs)
{
    return _toDecorate->waitForReadyRead(msecs);
}

//pure-virtual from QIODevice
qint64 QIODeviceDecorator::readData(char *data, qint64 maxlen)
{
    //return _toDecorate->readData(data, maxlen);
    return _toDecorate->read(data, maxlen);
}

//pure-virtual from QIODevice
qint64 QIODeviceDecorator::readLineData(char *data, qint64 maxlen)
{
    //return _toDecorate->readLineData(data, maxlen);
    return _toDecorate->readLine(data, maxlen);
}

//pure-virtual from QIODevice
qint64 QIODeviceDecorator::writeData(const char *data, qint64 len)
{
    //return _toDecorate->writeData(data, len);
    return _toDecorate->write(data, len);
}

//protected slot
void QIODeviceDecorator::handleChildAboutToClose()
{
    this->aboutToClose();
}

//protected slot
void QIODeviceDecorator::handleChildBytesWritten(qint64 bytesWritten)
{
    this->bytesWritten(bytesWritten);
}

//protected slot
void QIODeviceDecorator::handleChildReadChannelFinished()
{
    this->readChannelFinished();
}

//protected slot
void QIODeviceDecorator::handleChildReadyRead()
{
    this->readyRead();
}
