#ifndef WINDOWAPI_H
#define WINDOWAPI_H

#include <QtGui/qwindowdefs.h>

#include <QObject>
#include <QtGlobal>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

// NOTE: Never include <X11/Xlib.h> here.
// Xlib defines the macro `Bool` (and others) that clashes with QMetaType::Bool
// and breaks every MOC-generated file. X11 headers are only included in the
// .cpp.

class QTimer;

class WindowApi : public QObject
{
    Q_OBJECT
public:
    static WindowApi& instance();

    void installHook();
    void cleanUp();
    WId GetForegroundWindowInvoke();

signals:
    void showApp();

private:
    explicit WindowApi(QObject* parent = nullptr);
    ~WindowApi();

#ifdef Q_OS_WIN
    static HHOOK SetWindowsHookExInvoke(int idHook, HOOKPROC lpfn,
                                        HINSTANCE hmod, DWORD dwThreadId);
    static BOOL UnhookWindowsHookExInoke(HHOOK hhk);
    static LRESULT CallNextHookExInvoke(HHOOK hhk, int nCode, WPARAM wParam,
                                        LPARAM lParam);
    static LRESULT CALLBACK detectKeys(int code, WPARAM wParam, LPARAM lParam);

    HHOOK keyboardProcHook = nullptr;
    bool isKeyJPressedDown = false;
    bool isKeyAltPressedDown = false;
#elif defined(Q_OS_LINUX)
    bool isWayland() const;
    void pollEvents();

    // X11 types are hidden behind opaque handles to avoid leaking Xlib into
    // MOC.
    void *display = nullptr;       // Display*
    unsigned long rootWindow = 0;  // Window
    int keycodeJ = 0;
    QTimer *timer = nullptr;
    bool hookInstalled = false;
#endif
};

#endif  // WINDOWAPI_H
