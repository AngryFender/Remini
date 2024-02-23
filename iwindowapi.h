#ifndef IWINDOWAPI_H
#define IWINDOWAPI_H

class IWindowApi{

public:
    using keyboardEventCallBack = long long (*)(int, unsigned long, long);

    IWindowApi(){};
    virtual ~IWindowApi()=default;
    virtual void SetWindowsHookExInvoke(int idHook,
                                        keyboardEventCallBack  lpfn,
                                        void* hmod,
                                        unsigned long  dwThreadId)=0;

};

#endif // IWINDOWAPI_H
