#include "windowapi.h"

HHOOK WindowApi::SetWindowsHookExInvoke(int idHook, HOOKPROC lpfn, HINSTANCE hmod, DWORD dwThreadId)
{
    return SetWindowsHookExW(idHook, lpfn , hmod, dwThreadId);
}

BOOL WindowApi::UnhookWindowsHookExInoke(HHOOK hhk)
{
    return UnhookWindowsHookEx(hhk);
}

LRESULT WindowApi::CallNextHookExInvoke(HHOOK hhk, int nCode, WPARAM wParam, LPARAM lParam)
{
    return CallNextHookEx(hhk, nCode, wParam, lParam);
}

BOOL WindowApi::BringWindowToTopInvoke(HWND hWnd)
{
    return BringWindowToTop(hWnd);
}

void WindowApi::cleanUp()
{
    UnhookWindowsHookExInoke(keyboardProcHook);
}

LRESULT WindowApi::detectKeys(int code, WPARAM wParam, LPARAM lParam)
{
    if(code >= 0){
        bool isKeyDown 	= wParam == WM_KEYDOWN 	|| wParam == WM_SYSKEYDOWN;
        bool isKeyUp 	= wParam == WM_KEYUP 	|| wParam == WM_SYSKEYUP;

        KBDLLHOOKSTRUCT* kbStruct = (KBDLLHOOKSTRUCT*)lParam;

        DWORD vkCode = kbStruct->vkCode;
        if (vkCode == KEY_J || vkCode == KEY_ALT)
        {
            if (isKeyDown)
            {
                if(vkCode == KEY_J){
                    isKeyJPressedDown = true;
                }

                if(vkCode == KEY_ALT){
                    isKeyAltPressedDown = true;
                }
            }

            if (isKeyUp){
                if(vkCode == KEY_J){
                    isKeyJPressedDown = false;
                }

                if(vkCode == KEY_ALT){
                    isKeyAltPressedDown = false;
                }
            }

            if(isKeyAltPressedDown && isKeyJPressedDown){
                emit WindowApi::instance().showApp();
                isKeyJPressedDown = false;
                return 1;
            }
        }
    }
    return instance().CallNextHookExInvoke(keyboardProcHook,code ,wParam,lParam)    ;
}
