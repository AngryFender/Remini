#include "windowapi.h"

void WindowApi::SetWindowsHookExInvoke(int idHook, HOOKPROC lpfn, HINSTANCE hmod, DWORD dwThreadId)
{
   keyboardHookProc = SetWindowsHookExW(idHook, (HOOKPROC)lpfn ,(HINSTANCE) hmod, (DWORD)dwThreadId);
}

LRESULT WindowApi::CallNextHookExInvoke(HHOOK hhk, int nCode, WPARAM wParam, LPARAM lParam)
{
    return CallNextHookEx(hhk, nCode, wParam, lParam);
}

LRESULT WindowApi::detechKeys(int code, WPARAM wParam, LPARAM lParam)
{
    return instance().CallNextHookExInvoke(keyboardHookProc,code ,wParam,lParam)    ;
}
