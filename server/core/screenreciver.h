#ifndef SCREENRECIVER_H
#define SCREENRECIVER_H

#include <QtCore/QObject>

class ScreenReciver : public QObject
{
    Q_OBJECT
public:
    explicit ScreenReciver(QObject *parent = nullptr);

signals:

public slots:
};

#endif // SCREENRECIVER_H
