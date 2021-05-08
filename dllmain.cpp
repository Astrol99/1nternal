// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "structs.h"
#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <cmath>

// 3D Euclidian distance forumula function
float dist(Vec3* src, Vec3* dst)
{
    // Normalize src to the origin (0,0,0) relative to dst
    Vec3 normalized = Vec3 {
        dst->x - src->x,
        dst->y - src->y,
        dst->z - dst->z
    };

    return sqrtf(powf(normalized.x, 2) + powf(normalized.y, 2) + powf(normalized.z, 2));
}

Player* getClosestPlayer(Game* game)
{
    Player* closest = nullptr;

    for (int i = 1; i < game->m_PlayerCount; i++)
    {
        Player* player = game->GetPlayer(i);

        // If the closest player is null (first loop), assign closest as first
        // player in loop then skip to second loop to continue normal operations
        if (!closest)
            closest = player;

        // If player team is not on the same team as localplayer
        if (player->m_Team != game->m_LocalPlayer->m_Team)
        {
            // Get lowest dist between previous closest player and current indexed player
            if (dist(&game->m_LocalPlayer->m_HeadPos, &player->m_HeadPos) < dist(&game->m_LocalPlayer->m_HeadPos, &closest->m_HeadPos))
                closest = player;
        }
    }
    
    return closest;
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

        Player* closestPlayer = getClosestPlayer(game);

        Sleep(5); // Preserve some resources
    }

    // Cleanup
    if (f)
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
    {
        HANDLE hThread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)MainThread, hModule, 0, nullptr);
        if (hThread)
            CloseHandle(hThread);
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}