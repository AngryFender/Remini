#ifndef WINDOWAPI_H
#define WINDOWAPI_H

#include <Windows.h>
#include <QSharedPointer>
#include "iwindowapi.h"


class WindowApi : public IWindowApi
{
private:
    static HHOOK keyboardProcHook;
    WindowApi(){};
    ~WindowApi(){ cleanUp();};
public:
    static WindowApi& instance(){
        static WindowApi instance;
        instance.SetWindowsHookExInvoke((int)WH_KEYBOARD_LL, detectKeys, 0, 0);
        return instance;
    };

    static LRESULT CALLBACK detectKeys(int code, WPARAM wParam, LPARAM lParam);

    void SetWindowsHookExInvoke(int idHook, HOOKPROC lpfn, HINSTANCE hmod, DWORD dwThreadId) override;
    BOOL UnhookWindowsHookExInoke(HHOOK hhk) override;

    LRESULT CallNextHookExInvoke(HHOOK hhk, int nCode, WPARAM wParam, LPARAM lParam) override;

protected:
    void cleanUp();
};

#endif // WINDOWAPI_H
