#ifndef WINDOWAPI_H
#define WINDOWAPI_H

#include <Windows.h>
#include <QSharedPointer>
#include "iwindowapi.h"
#include <QObject>

#define KEY_J 0x4A
#define KEY_ALT 164

class WindowApi : public IWindowApi
{
    Q_OBJECT
private:
    static inline HHOOK keyboardProcHook;
    static inline bool isKeyJPressedDown = false;
    static inline bool isKeyAltPressedDown = false;
    WindowApi(){};
    ~WindowApi(){ cleanUp();};

public:
    static WindowApi& instance(){
        static WindowApi instance;

        if(keyboardProcHook == nullptr){
            keyboardProcHook = instance.SetWindowsHookExInvoke((int)WH_KEYBOARD_LL, detectKeys, 0, 0);
        }
        return instance;
    };

    static LRESULT CALLBACK detectKeys(int code, WPARAM wParam, LPARAM lParam);

    HHOOK SetWindowsHookExInvoke(int idHook, HOOKPROC lpfn, HINSTANCE hmod, DWORD dwThreadId) override;
    BOOL UnhookWindowsHookExInoke(HHOOK hhk) override;
    LRESULT CallNextHookExInvoke(HHOOK hhk, int nCode, WPARAM wParam, LPARAM lParam) override;

    BOOL BringWindowToTopInvoke(WId wid) override;

    WId GetForegroundWindowInvoke();
signals:
    void showApp();

protected:
    void cleanUp();
};

#endif // WINDOWAPI_H
