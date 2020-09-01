#ifndef REMOTEVIEW_H
#define REMOTEVIEW_H

#include <QtWidgets/QWidget>

class RemoteView : public QWidget
{
    Q_OBJECT
    QPainter *p;
    QImage *img;

    QSize _screenSize;
    QSize _widgetSize;

    QPoint _lastMousePressPoint;
    Qt::MouseButton _lastPressedButton;

    Q_PROPERTY(bool lockMouse READ lockMouse WRITE setLockMouse NOTIFY lockMouseChanged)
    Q_PROPERTY(bool lockKeyboard READ lockKeyboard WRITE setLockKeyboard NOTIFY lockKeyboardChanged)

public:
    explicit RemoteView(QWidget *parent = nullptr);

    bool lockMouse() const;
    bool lockKeyboard() const;

signals:

public slots:
    void resize(int w, int h);

    void drawImage(int x, int y, QImage &img);

    // QWidget interface
    void setLockMouse(bool lockMouse);
    void setLockKeyboard(bool lockKeyboard);

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

signals:
    void mouseClick(const QPoint &pt, const Qt::MouseButton &button);
    void mousePress(const QPoint &pt, const Qt::MouseButton &button);
    void mouseRelease(const QPoint &pt, const Qt::MouseButton &button);
    void mouseDoubleClick(const QPoint &pt, const Qt::MouseButton &button);
    void mouseMove(const QPoint &pt, const Qt::MouseButton &button);

    void keyPress(Qt::Key key, Qt::KeyboardModifiers modifs);
    void keyRelease(int key, Qt::KeyboardModifiers modifs);

    void lockMouseChanged(bool lockMouse);

    void lockKeyboardChanged(bool lockKeyboard);

private:
    QPoint mapToScreen(const QPoint &pt);

    bool m_lockMouse;
    bool m_lockKeyboard;
};

#endif // REMOTEVIEW_H
