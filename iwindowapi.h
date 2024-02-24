#ifndef IWINDOWAPI_H
#define IWINDOWAPI_H

#include <Windows.h>

class IWindowApi{

public:

    IWindowApi(){};
    virtual ~IWindowApi()=default;
    virtual void SetWindowsHookExInvoke(int idHook,
                                        HOOKPROC  lpfn,
                                        HINSTANCE hmod,
                                        DWORD dwThreadId)=0;


    virtual LRESULT CallNextHookExInvoke( HHOOK  hhk,
                                int    nCode,
                                WPARAM wParam,
                                LPARAM  lParam) = 0;

};

#endif // IWINDOWAPI_H
