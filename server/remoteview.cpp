#include "remoteview.h"

#include <QPaintEvent>
#include <QPainter>
#include <QImage>
#include <QDebug>
#include <QDebug>

RemoteView::RemoteView(QWidget *parent) : QWidget(parent)
    , p(nullptr), img(nullptr), m_lockMouse(false), m_lockKeyboard(false)
{
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
}

bool RemoteView::lockMouse() const
{
    return m_lockMouse;
}

bool RemoteView::lockKeyboard() const
{
    return m_lockKeyboard;
}

void RemoteView::resize(int w, int h)
{
    QWidget::resize(w, h);
    img = new QImage(w, h, QImage::Format_ARGB32);
    p = new QPainter(img);
    img->fill(Qt::lightGray);
}

void RemoteView::drawImage(int x, int y, QImage &img)
{
    if (p && !img.isNull())
        p->drawImage(x, y, img);
}

void RemoteView::setLockMouse(bool lockMouse)
{
    if (m_lockMouse == lockMouse)
        return;

    m_lockMouse = lockMouse;
    emit lockMouseChanged(m_lockMouse);
}

void RemoteView::setLockKeyboard(bool lockKeyboard)
{
    if (m_lockKeyboard == lockKeyboard)
        return;

    m_lockKeyboard = lockKeyboard;
    emit lockKeyboardChanged(m_lockKeyboard);
}

void RemoteView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    if (p && img) {
        QPainter p(this);
        p.drawImage(QPoint(0, 0), *img);
    }
}

void RemoteView::mousePressEvent(QMouseEvent *event)
{
    if (m_lockMouse)
        return;
    _lastMousePressPoint = event->pos();
    _lastPressedButton = event->button();
}

void RemoteView::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_lockMouse)
        return;
    if (_lastMousePressPoint.isNull())
        emit mouseRelease(mapToScreen(event->pos()), event->button());
    else {
        emit mouseClick(mapToScreen(event->pos()), event->button());
        _lastPressedButton = Qt::NoButton;
        _lastMousePressPoint = QPoint();
    }
}

void RemoteView::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (m_lockMouse)
        return;
    emit mouseDoubleClick(mapToScreen(event->pos()), event->button());
}

void RemoteView::mouseMoveEvent(QMouseEvent *event)
{
    if (m_lockMouse)
        return;

    if (!_lastMousePressPoint.isNull()) {
        auto diff = (event->pos() - _lastMousePressPoint).manhattanLength();
        if (diff > 10) {
            qDebug() << "emit press" << event->button();
            emit mousePress(mapToScreen(event->pos()), _lastPressedButton);
            _lastMousePressPoint = QPoint();
        }
    } else {
        emit mouseMove(mapToScreen(event->pos()), event->button());
    }
}

void RemoteView::keyPressEvent(QKeyEvent *event)
{
    if (m_lockKeyboard) {
        qDebug() << "is locked";
        return;
    }
    emit keyPress(static_cast<Qt::Key>(event->key()), event->modifiers());
}

void RemoteView::keyReleaseEvent(QKeyEvent *event)
{
    if (m_lockKeyboard)
        return;
    emit keyRelease(event->key(), event->modifiers());
}

QPoint RemoteView::mapToScreen(const QPoint &pt)
{
    return QPoint(
        (_screenSize.width() / _widgetSize.width()) * pt.x(),
        (_screenSize.height() / _widgetSize.height()) * pt.y()
        );
}

