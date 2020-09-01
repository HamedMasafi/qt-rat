#include "inputmanager.h"

#include <QMap>
#include <QDebug>
#include <QTextStream>

#include "keys.h"

#ifdef Q_OS_WIN
#   include <qt_windows.h>
#endif

#ifdef Q_OS_LINUX
#   include <X11/Xlib.h>
//#   include <X11/Xutil.h>
//#   include <X11/Xcursor/Xcursor.h>
//#   include <X11/extensions/XTest.h>
#endif

InputManager::InputManager()
{
#ifdef Q_OS_LINUX
    display = XOpenDisplay(nullptr);
    if (display == NULL) {
        qDebug() << "Can't open display!";
    }
    scr = XDefaultScreen(display);
    root_window = XRootWindow(display, scr);
#endif
}

void InputManager::moveMouse(const QPoint &pt, const Qt::MouseButton &button)
{
#ifdef Q_OS_WIN
    Q_UNUSED(button)

    DWORD dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;

    INPUT input;
    input.type = INPUT_MOUSE;
    input.mi.dx = pt.x() * (65536.0f / GetSystemMetrics(SM_CXSCREEN));;
    input.mi.dy = pt.y() * (65536.0f / GetSystemMetrics(SM_CYSCREEN));;
    input.mi.dwFlags = dwFlags;
    input.mi.mouseData = 0;
    input.mi.dwExtraInfo = NULL;
    input.mi.time = 0;
    SendInput(1, &input, sizeof(INPUT));
#endif
#ifdef Q_OS_LINUX
    Q_UNUSED(button)
    XWarpPointer(display, None, root_window, 0, 0, 0, 0, pt.x(), pt.y());
    XFlush(display);
#endif
}

void InputManager::pressMouse(const QPoint &pos, const Qt::MouseButton &button)
{
#ifdef Q_OS_WIN
        DWORD dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
    switch (button) {
    case Qt::LeftButton:
        dwFlags |= MOUSEEVENTF_LEFTDOWN;
        break;
    case Qt::RightButton:
        dwFlags |= MOUSEEVENTF_RIGHTDOWN;
        break;
    case Qt::MiddleButton:
        dwFlags |= MOUSEEVENTF_MIDDLEDOWN;
        break;
    default:
        break;
    }

    INPUT input;
    input.type = INPUT_MOUSE;
    input.mi.dx = pos.x() * (65536.0f / GetSystemMetrics(SM_CXSCREEN));;
    input.mi.dy = pos.y() * (65536.0f / GetSystemMetrics(SM_CYSCREEN));;
    input.mi.dwFlags = dwFlags;
    input.mi.mouseData = 0;
    input.mi.dwExtraInfo = NULL;
    input.mi.time = 0;
    SendInput(1, &input, sizeof(INPUT));
#endif
#ifdef Q_OS_LINUX
    auto screen = ScreenOfDisplay(display, 0);
    XEvent e;

    XAllowEvents(display, SyncPointer, CurrentTime);
    XWindowEvent(display, screen->root, ButtonPressMask, &e);
    XUngrabPointer(display, CurrentTime);
    Cursor cursor = XCreateFontCursor(display, 128);
    XFreeCursor(display, cursor);

        return;
    // Create and setting up the event
    XEvent event;
    memset(&event, 0, sizeof(event));

    switch (button) {
    case Qt::LeftButton:
        event.xbutton.button = Button1;
        break;
    case Qt::MiddleButton:
        event.xbutton.button = Button2;
        break;
    case Qt::RightButton:
        event.xbutton.button = Button3;
        break;

    default:
        break;
    }
    event.xbutton.same_screen = True;
    event.xbutton.subwindow = DefaultRootWindow(display);

    while (event.xbutton.subwindow)
    {
        event.xbutton.window = event.xbutton.subwindow;
        XQueryPointer(display, event.xbutton.window,
                      &event.xbutton.root, &event.xbutton.subwindow,
                      &event.xbutton.x_root, &event.xbutton.y_root,
                      &event.xbutton.x, &event.xbutton.y,
                      &event.xbutton.state);
    }
    // Press
    event.type = ButtonPress;
    if (XSendEvent(display, PointerWindow, True, ButtonPressMask, &event) == 0)
        fprintf(stderr, "Error to send the event!\n");
    XFlush(display);
#endif
}

void InputManager::releaseMouse(const QPoint &pos, const Qt::MouseButton &button)
{
#ifdef Q_OS_WIN
    DWORD dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
    switch (button) {
    case Qt::LeftButton:
        dwFlags |= MOUSEEVENTF_LEFTUP;
        break;
    case Qt::RightButton:
        dwFlags |= MOUSEEVENTF_RIGHTUP;
        break;
    case Qt::MiddleButton:
        dwFlags |= MOUSEEVENTF_MIDDLEUP;
        break;
    default:
        break;
    }

    INPUT input;
    input.type = INPUT_MOUSE;
    input.mi.dx = pos.x() * (65536.0f / GetSystemMetrics(SM_CXSCREEN));;
    input.mi.dy = pos.y() * (65536.0f / GetSystemMetrics(SM_CYSCREEN));;
    input.mi.dwFlags = dwFlags;
    input.mi.mouseData = 0;
    input.mi.dwExtraInfo = NULL;
    input.mi.time = 0;
    SendInput(1, &input, sizeof(INPUT));
#endif
#ifdef Q_OS_LINUX
//    XTestFakeButtonEvent(display, 1, True,  CurrentTime);

    // Create and setting up the event
    XEvent event;
    memset(&event, 0, sizeof(event));
    switch (button) {
    case Qt::LeftButton:
        event.xbutton.button = Button1;
        break;
    case Qt::MiddleButton:
        event.xbutton.button = Button2;
        break;
    case Qt::RightButton:
        event.xbutton.button = Button3;
        break;

    default:
        break;
    }
    event.xbutton.same_screen = True;
    event.xbutton.subwindow = DefaultRootWindow(display);
    while (event.xbutton.subwindow)
    {
        event.xbutton.window = event.xbutton.subwindow;
        XQueryPointer(display, event.xbutton.window,
                      &event.xbutton.root, &event.xbutton.subwindow,
                      &event.xbutton.x_root, &event.xbutton.y_root,
                      &event.xbutton.x, &event.xbutton.y,
                      &event.xbutton.state);
    }
    // Press
    event.type = ButtonRelease;
    if (XSendEvent(display, PointerWindow, True, ButtonReleaseMask, &event) == 0)
        fprintf(stderr, "Error to send the event!\n");
    XFlush(display);
#endif
}

void InputManager::pressKey(const Qt::Key &key, const Qt::KeyboardModifiers &modif)
{

#ifdef Q_OS_LINUX
    KeySym ks = Keys::keyToXk(key);
    if (!ks) {
        qDebug() << "Unable to simulate" << key;
        return;
    }
    qDebug() << "key=" << ks;
    XEvent event;
    memset(&event, 0, sizeof(event));
    uint mask = 0u;
    if (modif & Qt::ControlModifier)
        mask |= ControlMask;
    if (modif & Qt::ShiftModifier)
        mask |= ShiftMask;
//    if (modif & Qt::AltModifier)
//        mask |= AltMask;

    event.xkey.state = mask;
    event.xkey.keycode = XKeysymToKeycode (display, ks);
    event.xkey.same_screen = True;
    event.xkey.subwindow = DefaultRootWindow(display);
    while (event.xbutton.subwindow)
    {
        event.xbutton.window = event.xbutton.subwindow;
        XQueryPointer(display, event.xbutton.window,
                      &event.xbutton.root, &event.xbutton.subwindow,
                      &event.xbutton.x_root, &event.xbutton.y_root,
                      &event.xbutton.x, &event.xbutton.y,
                      &event.xbutton.state);
    }
    event.type = KeyPress;
    if (XSendEvent(display, InputFocus, True, KeyPressMask, &event) == 0)
        fprintf(stderr, "Error to send the event!\n");
    XFlush(display);
//    XSendEvent(display, InputFocus, True, KeyReleaseMask, &event);
//    XFlush(display);
#endif
#ifdef Q_OS_WIN
    INPUT ip;
    ip.type = INPUT_KEYBOARD;


    ip.ki.wScan = 0;
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;
    ip.ki.dwFlags = 0;

    if (modif & Qt::ShiftModifier) {
        ip.ki.wVk = VK_SHIFT;
        SendInput(1, &ip, sizeof(INPUT));
    }
    if (modif & Qt::ControlModifier) {
        ip.ki.wVk = VK_CONTROL;
        SendInput(1, &ip, sizeof(INPUT));
    }
    if (modif & Qt::AltModifier) {
        ip.ki.wVk = VK_MENU;
        SendInput(1, &ip, sizeof(INPUT));
    }

    ip.ki.wVk = Keys::keyToVk(key);
    SendInput(1, &ip, sizeof(INPUT));


    ip.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &ip, sizeof(INPUT));

    if (modif & Qt::ShiftModifier) {
        ip.ki.wVk = VK_SHIFT;
        SendInput(1, &ip, sizeof(INPUT));
    }
    if (modif & Qt::ControlModifier) {
        ip.ki.wVk = VK_CONTROL;
        SendInput(1, &ip, sizeof(INPUT));
    }
    if (modif & Qt::AltModifier) {
        ip.ki.wVk = VK_MENU;
        SendInput(1, &ip, sizeof(INPUT));
    }
#endif
}

InputManager *InputManager::instance()
{
    static InputManager *in = nullptr;
    if (!in)
        in = new InputManager;
    return in;
}
