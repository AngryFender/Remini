#ifndef WINDOWAPI_H
#define WINDOWAPI_H

#include "iwindowapi.h"
#include "Windows.h"

class WindowApi : public IWindowApi
{
public:
    WindowApi();
    ~WindowApi();

    // IWindowApi interface
public:
    void SetWindowsHookExInvoke(int idHook, HOOKPROC lpfn, HINSTANCE hmod, DWORD dwThreadId) override;
};

#endif // WINDOWAPI_H
