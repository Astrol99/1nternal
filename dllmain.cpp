// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>

DWORD WINAPI MainThread(HMODULE hModule)
{
    // Create console from thread
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);

    std::cout << "1nternal" << std::endl;

    // Get addr of .exe module
    uintptr_t moduleBase = (uintptr_t)GetModuleHandle(NULL);

    // Main cheat loop
    while (true)
    {
        if (GetAsyncKeyState(VK_END) & 1)
            break;

        uintptr_t* LocalPlayerPtr = (uintptr_t*)(moduleBase + 0x10F4F4);

        if (LocalPlayerPtr)
            std::cout << "Health: " << *(int*)(*LocalPlayerPtr + 0xF8) << std::endl;

        Sleep(5); // Preserve some resources
    }

    // Cleanup
    fclose(f);
    FreeConsole();
    FreeLibraryAndExitThread(hModule, 0);
    
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)MainThread, hModule, 0, nullptr));
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

