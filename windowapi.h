#ifndef WINDOWAPI_H
#define WINDOWAPI_H

#include "Windows.h"
#include "iwindowapi.h"

class WindowApi : public IWindowApi
{
public:
    WindowApi();
    ~WindowApi();

    // IWindowApi interface
public:
    void SetWindowsHookExInvoke(int idHook, keyboardEventCallBack lpfn, void *hmod, unsigned long dwThreadId) override;
};

#endif // WINDOWAPI_H
