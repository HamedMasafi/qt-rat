#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <QObject>
#include <QPoint>

#ifdef Q_OS_LINUX
#include <QTextStream>
#include <X11/Xlib.h>
#endif

#define INPUTMANAGER InputManager::instance()

class InputManager : public QObject
{
    Q_OBJECT
#ifdef Q_OS_LINUX
    Display *display;
    int scr;
    Window root_window;
#endif

public:
    InputManager();

    void moveMouse(const QPoint &pt, const Qt::MouseButton &button);
    void pressMouse(const QPoint &pos, const Qt::MouseButton &button);
    void releaseMouse(const QPoint &pos, const Qt::MouseButton &button);
    void pressKey(const Qt::Key &key, const Qt::KeyboardModifiers &modif = Qt::NoModifier);

    static InputManager *instance();
};

#endif // INPUTMANAGER_H
