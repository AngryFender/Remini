#include "windowapi.h"

WindowApi::WindowApi()
{

}

WindowApi::~WindowApi()
{

}

void WindowApi::SetWindowsHookExInvoke(int idHook, keyboardEventCallBack lpfn, void *hmod, unsigned long dwThreadId)
{
      SetWindowsHookExA(idHook, (HOOKPROC)lpfn,(HINSTANCE) hmod, (DWORD)dwThreadId)	;
}


