#include "windowapi.h"

// ---------------------------------------------------------------------------
// Singleton
// ---------------------------------------------------------------------------
WindowApi &WindowApi::instance()
{
    static WindowApi s_instance;
    return s_instance;
}

WindowApi::WindowApi(QObject *parent) : QObject(parent) {}

WindowApi::~WindowApi() { cleanUp(); }

// ============================ Windows ============================
#ifdef Q_OS_WIN

HHOOK WindowApi::SetWindowsHookExInvoke(int idHook, HOOKPROC lpfn,
                                        HINSTANCE hmod, DWORD dwThreadId)
{
    return SetWindowsHookExW(idHook, lpfn, hmod, dwThreadId);
}

BOOL WindowApi::UnhookWindowsHookExInoke(HHOOK hhk)
{
    return UnhookWindowsHookEx(hhk);
}

LRESULT WindowApi::CallNextHookExInvoke(HHOOK hhk, int nCode, WPARAM wParam,
                                        LPARAM lParam)
{
    return CallNextHookEx(hhk, nCode, wParam, lParam);
}

WId WindowApi::GetForegroundWindowInvoke()
{
    return reinterpret_cast<WId>(GetForegroundWindow());
}

void WindowApi::installHook()
{
    keyboardProcHook = SetWindowsHookExInvoke(WH_KEYBOARD_LL, detectKeys,
                                              GetModuleHandle(nullptr), 0);
}

void WindowApi::cleanUp()
{
    if (keyboardProcHook)
    {
        UnhookWindowsHookExInoke(keyboardProcHook);
        keyboardProcHook = nullptr;
    }
}

// ---------------------------------------------------------------------------
// Low-level keyboard hook callback.
//
// `detectKeys` is declared `static` in the header because SetWindowsHookExW
// requires a plain function pointer (a non-static member function cannot be
// passed directly). A static member has no `this` pointer, so any access to
// the instance fields must go through the singleton accessor.
//
// Virtual-key codes:
//   'J'      - the J key
//   VK_MENU  - the Alt key (generic; VK_LMENU / VK_RMENU for left/right)
// ---------------------------------------------------------------------------
LRESULT CALLBACK WindowApi::detectKeys(int code, WPARAM wParam, LPARAM lParam)
{
    WindowApi &self = instance();

    if (code >= 0)
    {
        const bool isKeyDown =
            (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
        const bool isKeyUp = (wParam == WM_KEYUP || wParam == WM_SYSKEYUP);

        const auto *kbStruct =
            reinterpret_cast<const KBDLLHOOKSTRUCT *>(lParam);
        const DWORD vkCode = kbStruct->vkCode;

        if (vkCode == 'J' || vkCode == VK_MENU)
        {
            if (isKeyDown)
            {
                if (vkCode == 'J')
                    self.isKeyJPressedDown = true;
                if (vkCode == VK_MENU)
                    self.isKeyAltPressedDown = true;
            }
            if (isKeyUp)
            {
                if (vkCode == 'J')
                    self.isKeyJPressedDown = false;
                if (vkCode == VK_MENU)
                    self.isKeyAltPressedDown = false;
            }
            if (self.isKeyAltPressedDown && self.isKeyJPressedDown)
            {
                // `emit` is a no-op macro; this is just a normal signal call.
                emit self.showApp();
                self.isKeyJPressedDown = false;
                return 1;  // swallow the keystroke so it does not reach other
                           // apps
            }
        }
    }

    return self.CallNextHookExInvoke(self.keyboardProcHook, code, wParam,
                                     lParam);
}

// ============================ Linux ============================
#elif defined(Q_OS_LINUX)

// Qt headers first.
#include <QGuiApplication>
#include <QString>
#include <QTimer>

// X11 last, so its macros (Bool, None, Status, ...) cannot break Qt headers.
#include <X11/Xlib.h>
#include <X11/keysym.h>

bool WindowApi::isWayland() const
{
    const QString platform = QGuiApplication::platformName();
    return platform.contains(QStringLiteral("wayland"), Qt::CaseInsensitive);
}

void WindowApi::installHook()
{
    if (isWayland())
    {
        hookInstalled = false;
        return;
    }

    Display *dpy = XOpenDisplay(nullptr);
    if (!dpy)
    {
        hookInstalled = false;
        return;
    }
    display = dpy;

    Window root = DefaultRootWindow(dpy);
    rootWindow = static_cast<unsigned long>(root);
    keycodeJ = XKeysymToKeycode(dpy, XK_j);

    // Grab Alt+J while also tolerating CapsLock / NumLock being on.
    const unsigned int locks[] = {0, LockMask, Mod2Mask, LockMask | Mod2Mask};
    for (unsigned int lock : locks)
    {
        XGrabKey(dpy, keycodeJ, Mod1Mask | lock, root, True, GrabModeAsync,
                 GrabModeAsync);
    }
    XFlush(dpy);

    timer = new QTimer(this);
    timer->setInterval(50);
    connect(timer, &QTimer::timeout, this, &WindowApi::pollEvents);
    timer->start();

    hookInstalled = true;
}

void WindowApi::cleanUp()
{
    if (!hookInstalled)
        return;

    if (timer)
    {
        timer->stop();
        delete timer;  // QObject child of `this`; direct delete is safe here
        timer = nullptr;
    }

    Display *dpy = static_cast<Display *>(display);
    if (dpy)
    {
        Window root = static_cast<Window>(rootWindow);
        const unsigned int locks[] = {0, LockMask, Mod2Mask,
                                      LockMask | Mod2Mask};
        for (unsigned int lock : locks)
        {
            XUngrabKey(dpy, keycodeJ, Mod1Mask | lock, root);
        }
        XCloseDisplay(dpy);
        display = nullptr;
    }

    hookInstalled = false;
}

WId WindowApi::GetForegroundWindowInvoke()
{
    if (isWayland())
        return 0;

    Display *dpy = static_cast<Display *>(display);
    if (!dpy)
    {
        dpy = XOpenDisplay(nullptr);
        if (!dpy)
            return 0;
        display = dpy;
    }

    Window focused = 0;
    int revert = 0;
    XGetInputFocus(dpy, &focused, &revert);
    return static_cast<WId>(focused);
}

void WindowApi::pollEvents()
{
    Display *dpy = static_cast<Display *>(display);
    if (!dpy)
        return;

    while (XPending(dpy))
    {
        XEvent ev;
        XNextEvent(dpy, &ev);
        if (ev.type == KeyPress)
            emit showApp();
    }
}

#endif
