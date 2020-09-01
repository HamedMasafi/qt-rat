#ifndef QIODEVICEDECORATOR_H
#define QIODEVICEDECORATOR_H

#include <QIODevice>
#include <QPointer>

#include "SocksLib_global.h"

class SOCKSLIBSHARED_EXPORT QIODeviceDecorator : public QIODevice
{
    Q_OBJECT
public:
    explicit QIODeviceDecorator(QIODevice * toDecorate, QObject *parent = 0);
    virtual ~QIODeviceDecorator();

    //These are all virtual from QIODevice
    virtual bool atEnd() const;
    virtual qint64 bytesAvailable() const;
    virtual qint64 bytesToWrite() const;
    virtual bool canReadLine() const;
    virtual void close();
    virtual bool isSequential() const;
    virtual bool open(OpenMode mode);
    virtual qint64 pos() const;
    virtual bool reset();
    virtual bool seek(qint64 pos);
    virtual qint64 size() const;
    virtual bool waitForBytesWritten(int msecs);
    virtual bool waitForReadyRead(int msecs);

protected:
    //These are all pure-virtual from QIODevice
    virtual qint64 readData(char *data, qint64 maxlen);
    virtual qint64 readLineData(char *data, qint64 maxlen);
    virtual qint64 writeData(const char *data, qint64 len);

    QPointer<QIODevice> _toDecorate;
    
signals:
    
public slots:

protected slots:
    virtual void handleChildAboutToClose();
    virtual void handleChildBytesWritten(qint64);
    virtual void handleChildReadChannelFinished();
    virtual void handleChildReadyRead();
    
};

#endif // QIODEVICEDECORATOR_H
