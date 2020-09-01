#ifndef THROTTLINGDECORATOR_H
#define THROTTLINGDECORATOR_H

#include "QIODeviceDecorator.h"
#include "SocksLib_global.h"

#include <QTimer>
#include <QByteArray>
#include <QTcpSocket>
#include <QTime>

class SOCKSLIBSHARED_EXPORT ThrottlingDecorator : public QIODeviceDecorator
{
    Q_OBJECT
public:
    explicit ThrottlingDecorator(QAbstractSocket * toDecorate, QObject *parent = 0);
    explicit ThrottlingDecorator(qreal readBytesPerSecond, qreal writeBytesPerSecond, QAbstractSocket * toDecorate, QObject * parent = 0);
    virtual ~ThrottlingDecorator();

    //These are all virtual from QIODeviceDecorator
    virtual bool atEnd() const;
    virtual qint64 bytesAvailable() const;
    virtual qint64 bytesToWrite() const;
    virtual bool canReadLine() const;
    virtual bool waitForBytesWritten(int msecs);
    virtual bool waitForReadyRead(int msecs);

    void setReadBytesPerSecond(qint64 maxReadBytesPerSecond);
    void setWriteBytesPerSecond(qint64 maxWriteBytesPerSecond);

protected:
    //pure-virtual from QIODevice
    virtual qint64 readData(char *data, qint64 maxlen);
    virtual qint64 readLineData(char *data, qint64 maxlen);
    virtual qint64 writeData(const char *data, qint64 len);
    
signals:
    
public slots:

protected slots:
    //These are all virtual from QIODeviceDecorator
    virtual void handleChildAboutToClose();
    virtual void handleChildReadChannelFinished();
    virtual void handleChildReadyRead();

private slots:
    void handleBuckets();
    void handleWriteQueue();
    void handleReadQueue();
    void handleMetrics();

private:
    void commonConstructor();
    QTimer * _bucketTimer;
    qint64 _readBucket;
    qint64 _writeBucket;
    QTime _lastBucketTime;

    qint64 _readBytesPerSecond;
    qint64 _writeBytesPerSecond;

    QByteArray _writeQueue;
    QByteArray _readQueue;

    bool _childIsFinished;
    QAbstractSocket * _cheaterSocketReference;

    qreal _bytesReadSinceLastMetric;
    qreal _bytesWrittenSinceLastMetric;
    QTime _lastMetricTime;
    
};

#endif // THROTTLINGDECORATOR_H
