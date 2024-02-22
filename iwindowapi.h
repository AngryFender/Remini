#ifndef IWINDOWAPI_H
#define IWINDOWAPI_H


class IWindowApi{
public:
    using HHOOK = void*;
    using HOOKPROC = long long (*)(int, unsigned long, long);
    using HINSTANCE = void*;
    using DWORD = unsigned long;

public:
    IWindowApi(){};
    virtual ~IWindowApi()=default;
    virtual void SetWindowsHookExInvoke(int idHook,
                                        HOOKPROC  lpfn,
                                        HINSTANCE hmod,
                                        DWORD     dwThreadId)=0;

};

#endif // IWINDOWAPI_H
