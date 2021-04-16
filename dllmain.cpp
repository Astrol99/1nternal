// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "structs.h"
#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <cmath>

// Make src the origin (0,0,0) relative to dst
Vec3 normalize(Vec3 src, Vec3 dst)
{
    return Vec3{
        dst.x - src.x,
        dst.y - src.y,
        dst.z - dst.z
    };
}

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

        Game* game = new Game;
        game = game->GetInstance();

        if (game->m_LocalPlayer->m_IsShooting)
        {
            Vec3 test = { 120, 120, 3.5 };
            Vec3 relative = normalize(game->m_LocalPlayer->m_HeadPos, test);

            std::cout << relative.x << " / " << relative.y << " / " << relative.z << std::endl;;
        }

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

