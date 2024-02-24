#ifndef WINDOWAPI_H
#define WINDOWAPI_H

#include <Windows.h>
#include <QSharedPointer>
#include "iwindowapi.h"


class WindowApi : public IWindowApi
{
private:
    static HHOOK keyboardHookProc;
    WindowApi(){};
    ~WindowApi(){};
public:
    static WindowApi& instance(){
        static WindowApi instance;
        instance.SetWindowsHookExInvoke((int)WH_KEYBOARD_LL, detechKeys, 0, 0);
        return instance;
    };

    static LRESULT CALLBACK detechKeys(int code, WPARAM wParam, LPARAM lParam);

    void SetWindowsHookExInvoke(int idHook, HOOKPROC lpfn, HINSTANCE hmod, DWORD dwThreadId) override;
    LRESULT CallNextHookExInvoke(HHOOK hhk, int nCode, WPARAM wParam, LPARAM lParam) override;
};

#endif // WINDOWAPI_H
