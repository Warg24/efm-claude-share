// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <windows.h>
#include <iostream>
#include <WinCon.h>
#include <string>
#include <winsock.h>

FILE* flog = 0;
//SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{


    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        // //flog = fopen("E:\test.log", "w");
        // /* COnsole*/
        // 
        // if (AllocConsole())
        // {
        //     freopen("CONOUT$", "w", stdout);
        //     SetConsoleTitle((L"Hawk Debug Console"));
        //     SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
        // }
        // else
        // {
        //     //MessageBox(0,L"FAIL_console",L"Error",0);
        // }
        // 
        // break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        //if (flog) fclose(flog);

        break;
    }
    return TRUE;
}