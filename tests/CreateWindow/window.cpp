#include "window.h"

#include <QDebug>
#include <QEventLoop>
#include <QTimer>

#ifdef Q_OS_LINUX
#include <X11/keysymdef.h>
#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/Xos.h>
#include <X11/Xproto.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/XInput2.h>
#endif

#ifdef Q_OS_WIN
#   include <qt_windows.h>
#endif

CrossWindow::CrossWindow()
{

}

#ifdef Q_OS_LINUX
void CrossWindow::show()
{
    Display *d;
    int s;
    Window w;
    XEvent e;

    /* open connection with the server */
    d=XOpenDisplay(NULL);
    if(d==NULL) {
        printf("Cannot open display\n");
        exit(1);
    }
    s=DefaultScreen(d);

    /* create window */
    w=XCreateSimpleWindow(d, RootWindow(d, s), 10, 10, 100, 100, 1,
                          BlackPixel(d, s), WhitePixel(d, s));

//    Atom atoms[2] = { XInternAtom(d, "_NET_WM_STATE_FULLSCREEN", False), None };
#define ATOM(x) XInternAtom(d, #x, False)
    Atom atoms[] = {
        ATOM(_NET_WM_STATE_FULLSCREEN),
        ATOM(_NET_WM_STATE_SKIP_TASKBAR),
        ATOM(_NET_WM_STATE_SKIP_PAGER),
        ATOM(_NET_WM_STATE_ABOVE),
        ATOM(_NET_WM_STATE_STICKY),
        0,
    };
      XChangeProperty(
          d,
          w,
          XInternAtom(d, "_NET_WM_STATE", False),
          XA_ATOM, 32, PropModeReplace,
          reinterpret_cast<unsigned char*>(&atoms),
          5
      );


    // Process Window Close Event through event handler so XNextEvent does Not fail
    Atom delWindow = XInternAtom( d, "WM_DELETE_WINDOW", 0 );
    XSetWMProtocols(d , w, &delWindow, 1);

    /* select kind of events we are interested in */
    XSelectInput(d, w, ExposureMask | KeyPressMask);

    /* map (show) the window */
    XMapWindow(d, w);

    /* event loop */
    while(1) {
        XNextEvent(d, &e);
        /* draw or redraw the window */
        if(e.type==Expose) {
//            XFillRectangle(d, w, DefaultGC(d, s), 20, 20, 10, 10);
        }
        /* exit on key press */
        if(e.type==KeyPress)
            break;

        // Handle Windows Close Event
        if(e.type==ClientMessage)
            break;

    }
    QEventLoop loop;
    QTimer::singleShot(3000, &loop, &QEventLoop::quit);
    loop.exec();
qDebug() << "quit";
    /* destroy our window */
    XDestroyWindow(d, w);

    /* close connection to server */
    XCloseDisplay(d);
}
#endif

#ifdef Q_OS_WIN
LRESULT CALLBACK windowprocessforwindow1(HWND handleforwindow1,UINT message,WPARAM wParam,LPARAM lParam);

bool window1closed=false;

void CrossWindow::show()
{
    HINSTANCE hInst = (HINSTANCE)::GetModuleHandle(NULL);


    bool endprogram=false;

    //create window 1

    WNDCLASSEX windowclassforwindow1;
    ZeroMemory(&windowclassforwindow1,sizeof(WNDCLASSEX));
    windowclassforwindow1.cbClsExtra=NULL;
    windowclassforwindow1.cbSize=sizeof(WNDCLASSEX);
    windowclassforwindow1.cbWndExtra=NULL;
    windowclassforwindow1.hbrBackground=(HBRUSH)COLOR_INACTIVECAPTION;
    windowclassforwindow1.hCursor=LoadCursor(NULL,IDC_ARROW);
    windowclassforwindow1.hIcon=NULL;
    windowclassforwindow1.hIconSm=NULL;
    windowclassforwindow1.hInstance=hInst;
    windowclassforwindow1.lpfnWndProc=(WNDPROC)windowprocessforwindow1;
    windowclassforwindow1.lpszClassName=L"windowclass 1";
    windowclassforwindow1.lpszMenuName=NULL;
    windowclassforwindow1.style=CS_HREDRAW|CS_VREDRAW;

    if(!RegisterClassEx(&windowclassforwindow1))
    {
        int nResult=GetLastError();
        MessageBox(NULL,
            L"Window class creation failed",
            L"Window Class Failed",
            MB_ICONERROR);
    }

    HWND handleforwindow1=CreateWindowEx(NULL,
        windowclassforwindow1.lpszClassName,
            L"Parent Window",
            WS_OVERLAPPEDWINDOW,
            200,
            150,
            640,
            480,
            NULL,
            NULL,
            hInst,
            NULL                /* No Window Creation data */
);

    if(!handleforwindow1)
    {
        int nResult=GetLastError();

        MessageBox(NULL,
            L"Window creation failed",
            L"Window Creation Failed",
            MB_ICONERROR);
    }
    auto style = GetWindowLong(handleforwindow1, GWL_STYLE);
    auto ex_style = GetWindowLong(handleforwindow1, GWL_EXSTYLE);
    ex_style |= WS_EX_TOPMOST;
    ex_style &= ~WS_EX_DLGMODALFRAME;
    ex_style &= ~WS_EX_WINDOWEDGE;
    ex_style &= ~WS_EX_CLIENTEDGE;
    ex_style &= ~WS_EX_STATICEDGE;
    SetWindowLong(handleforwindow1, GWL_STYLE,
                      style & ~(WS_CAPTION | WS_THICKFRAME));
        SetWindowLong(handleforwindow1, GWL_EXSTYLE, ex_style);
    ShowWindow(handleforwindow1, SW_SHOWMAXIMIZED);
    SetWindowPos(handleforwindow1, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);


    MSG msg;
    ZeroMemory(&msg,sizeof(MSG));
    while (endprogram==false) {
        if (GetMessage(&msg,NULL,0,0));
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        if (window1closed) {
            endprogram=true;
        }
    }
    MessageBox(NULL,
    L"Both Windows are closed.  Program will now close.",
    L"",
    MB_ICONINFORMATION);
//    return 0;
}

LRESULT CALLBACK windowprocessforwindow1(HWND handleforwindow,UINT msg,WPARAM wParam,LPARAM lParam)
{
    switch(msg)
    {
        case WM_DESTROY: {
            MessageBox(NULL,
            L"Window 1 closed",
            L"Message",
            MB_ICONINFORMATION);

            window1closed=true;
            return 0;
        }
        break;
    }

    return DefWindowProc(handleforwindow,msg,wParam,lParam);
}
#endif
