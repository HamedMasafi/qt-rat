#include "ThrottlingDecorator.h"

#include <QtDebug>

ThrottlingDecorator::ThrottlingDecorator(QAbstractSocket *toDecorate, QObject *parent) :
    QIODeviceDecorator(toDecorate, parent)
{
    /*
     We need to keep a QAbstractSocket refernence (not just QIODevice) so we can adjust buffer
     sizes later.
    */
    _cheaterSocketReference = toDecorate;

    //The rest of the constructor code is in this method to reduce code duplication
    this->commonConstructor();
}

ThrottlingDecorator::ThrottlingDecorator(qreal readBytesPerSecond, qreal writeBytesPerSecond, QAbstractSocket *toDecorate, QObject *parent) :
    QIODeviceDecorator(toDecorate, parent)
{
    _cheaterSocketReference = toDecorate;

    //The rest of the constructor code is in this method to reduce code duplication
    this->commonConstructor();

    this->setReadBytesPerSecond(readBytesPerSecond);
    this->setWriteBytesPerSecond(writeBytesPerSecond);
}

ThrottlingDecorator::~ThrottlingDecorator()
{
}

//virtual from QIODeviceDecorator
bool ThrottlingDecorator::atEnd() const
{
    return (this->bytesAvailable() <= 0);
}

//virtual from QIODeviceDecorator
qint64 ThrottlingDecorator::bytesAvailable() const
{
    qint64 toRet = _readQueue.size() + QIODevice::bytesAvailable();
    return toRet;
}

//virtual from QIODeviceDecorator
qint64 ThrottlingDecorator::bytesToWrite() const
{
    return _writeQueue.size();
}

//virtual from QIODeviceDecorator
bool ThrottlingDecorator::canReadLine() const
{
    return _readQueue.contains('\n') || QIODevice::canReadLine();
}

//virtual from QIODeviceDecorator
bool ThrottlingDecorator::waitForBytesWritten(int msecs)
{
    Q_UNUSED(msecs)
    return false;
}

//virtual from QIODeviceDecorator
bool ThrottlingDecorator::waitForReadyRead(int msecs)
{
    Q_UNUSED(msecs)
    return false;
}

void ThrottlingDecorator::setReadBytesPerSecond(qint64 maxReadBytesPerSecond)
{
    _readBytesPerSecond = maxReadBytesPerSecond;
    _cheaterSocketReference->setReadBufferSize(maxReadBytesPerSecond * 3);
}

void ThrottlingDecorator::setWriteBytesPerSecond(qint64 maxWriteBytesPerSecond)
{
    _writeBytesPerSecond = maxWriteBytesPerSecond;
}

//protected
//pure-virtual from QIODevice
qint64 ThrottlingDecorator::readData(char *data, qint64 maxlen)
{
    if (this->bytesAvailable() == 0)
        return 0;

    qint64 actuallyRead = qMin<qint64>(maxlen,_readQueue.size());

    QByteArray temp = _readQueue.mid(0,actuallyRead);
    _readQueue.remove(0,actuallyRead);

    for (int i = 0; i < actuallyRead; i++)
        data[i] = temp[i];

    return actuallyRead;
}

//protected
//pure-virtual from QIODevice
qint64 ThrottlingDecorator::readLineData(char *data, qint64 maxlen)
{
    if (!this->canReadLine())
        return 0;

    QByteArray temp = _readQueue.mid(0,maxlen);
    int index = temp.indexOf('\n');
    QByteArray toRet = temp.mid(0,index + 1);
    _readQueue.remove(0,index + 1);

    for (int i = 0; i < index + 1; i++)
        data[i] = toRet[i];

    return toRet.size();
}

//protected
//pure-virtual from QIODevice
qint64 ThrottlingDecorator::writeData(const char *data, qint64 len)
{
    for (int i = 0; i < len; i++)
        _writeQueue.append(data[i]);

    if (_writeBucket > 0)
        this->handleWriteQueue();

    return len;
}

//protected slot
//virtual from QIODeviceDecorator
void ThrottlingDecorator::handleChildAboutToClose()
{
    _childIsFinished = true;
}

//protected slot
//virtual from QIODeviceDecorator
void ThrottlingDecorator::handleChildReadChannelFinished()
{
    _childIsFinished = true;
}

//protected slot
//virtual from QIODeviceDecorator
void ThrottlingDecorator::handleChildReadyRead()
{
    //Read it all into our read queue
    //_readQueue.append(_toDecorate->readAll());
}

//private slot
void ThrottlingDecorator::handleBuckets()
{
    //Figure out how much time REALLY elapsed since the last tick
    int msElapsed = _lastBucketTime.elapsed();
    _lastBucketTime.restart();

    /*
      If the clock has jumped more than thirty seconds in either direction, we'll assume it's changed
      either because the system was suspended or we entered/left DST or the clock was otherwise changed.
      Come back next tick.
    */
    if (qAbs(msElapsed) > 30000)
        return;

    //Based on how much time elapsed since last tick, figure out how many bytes to add to the buckets
    qreal fractionOfSecond = msElapsed / 1000.0;
    int toAddRead = fractionOfSecond * _readBytesPerSecond;
    int toAddWrite = fractionOfSecond * _writeBytesPerSecond;

    _readBucket = qMin<qint64>(toAddRead,
                               _readBucket + toAddRead);
    _writeBucket = qMin<qint64>(toAddWrite,
                                _writeBucket + toAddWrite);

    if (_readQueue.size() > 0)
    {
        //Nothing
    }
    else if (_toDecorate->bytesAvailable() <= 0 && _childIsFinished)
    {
        this->aboutToClose();
        this->close();
    }

    //Send as many bytes as we can from the write queue
    this->handleWriteQueue();

    //Read as many bytes as we can into the read queue
    this->handleReadQueue();
}

//private slot
void ThrottlingDecorator::handleWriteQueue()
{
    qint64 numToSend = qMin<qint64>(_writeBucket,_writeQueue.size());

    if (numToSend == 0)
        return;

    QByteArray toSend = _writeQueue.mid(0,numToSend);
    _writeQueue.remove(0,numToSend);

    _toDecorate->write(toSend);

    _writeBucket -= numToSend;
    _bytesWrittenSinceLastMetric += numToSend;
}

//private slot
void ThrottlingDecorator::handleReadQueue()
{
    qint64 childBytesAvailable = _toDecorate->bytesAvailable();
    if (childBytesAvailable <= 0)
        return;

    qint64 numBytesToRead = qMin<qint64>(_readBucket, childBytesAvailable);
    _readQueue.append(_toDecorate->read(numBytesToRead));
    _readBucket -= numBytesToRead;
    _bytesReadSinceLastMetric += numBytesToRead;
    this->readyRead();
}

//private slot
void ThrottlingDecorator::handleMetrics()
{
    int milliDiff = _lastMetricTime.restart();
    qreal seconds = milliDiff / 1000.0;

    qreal kibibytesReadPerSecond = (_bytesReadSinceLastMetric / seconds) / 1024;
    qreal kibibytesWrittenPerSecond = (_bytesWrittenSinceLastMetric / seconds) / 1024;

    _bytesReadSinceLastMetric = 0.0;
    _bytesWrittenSinceLastMetric = 0.0;

    if (kibibytesReadPerSecond < 1.0 && kibibytesWrittenPerSecond < 1.0)
        return;

    //qDebug() << "Read:" << kibibytesReadPerSecond << "KiB/s. Write:" << kibibytesWrittenPerSecond << "KiB/s.";
}

//private
void ThrottlingDecorator::commonConstructor()
{
    _readBucket = 0;
    _writeBucket = 0;
    this->setReadBytesPerSecond(1024*500);
    this->setWriteBytesPerSecond(1024*500);

    //This timer is what makes the throttling work
    _bucketTimer = new QTimer(this);
    connect(_bucketTimer,
            SIGNAL(timeout()),
            this,
            SLOT(handleBuckets()));
    _bucketTimer->start(40);
    _lastBucketTime.start();

    //This timer is what makes the metrics work
    QTimer * metricTimer = new QTimer(this);
    connect(metricTimer,
            SIGNAL(timeout()),
            this,
            SLOT(handleMetrics()));
    metricTimer->start(1000);
    _bytesReadSinceLastMetric = 0.0;
    _bytesWrittenSinceLastMetric = 0.0;
    _lastMetricTime.start();


    _childIsFinished = false;
}
