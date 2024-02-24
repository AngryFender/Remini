#include "windowapi.h"

void WindowApi::SetWindowsHookExInvoke(int idHook, HOOKPROC lpfn, HINSTANCE hmod, DWORD dwThreadId)
{
    keyboardProcHook = SetWindowsHookExW(idHook, (HOOKPROC)lpfn ,(HINSTANCE) hmod, (DWORD)dwThreadId);
}

BOOL WindowApi::UnhookWindowsHookExInoke(HHOOK hhk)
{
    return UnhookWindowsHookEx(hhk);
}

LRESULT WindowApi::CallNextHookExInvoke(HHOOK hhk, int nCode, WPARAM wParam, LPARAM lParam)
{
    return CallNextHookEx(hhk, nCode, wParam, lParam);
}

void WindowApi::cleanUp()
{
    UnhookWindowsHookExInoke(keyboardProcHook);
}

LRESULT WindowApi::detectKeys(int code, WPARAM wParam, LPARAM lParam)
{
    return instance().CallNextHookExInvoke(keyboardProcHook,code ,wParam,lParam)    ;
}
