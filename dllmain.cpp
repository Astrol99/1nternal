// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "structs.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>

Vec3 normalize(Vec3 src, Vec3 dst)
{
    // Normalize src to the origin (0,0,0) relative to dst
     Vec3 normalized = Vec3{
        dst.x - src.x,
        dst.y - src.y,
        dst.z - src.z
    };

    return normalized;
}

// 3D Euclidian distance forumula function
float dist(Vec3 src, Vec3 dst)
{
    Vec3 normalized = normalize(src, dst);
    return sqrtf(powf(normalized.x, 2) + powf(normalized.y, 2) + powf(normalized.z, 2));
}

Player* getClosestPlayer(Game* game)
{
    Player* closest = nullptr;

    for (int i = 1; i < game->m_PlayerCount; i++)
    {
        Player* player = game->GetPlayer(i);

        // Skip if current indexed player is either a teammate or dead
        if (player->m_Team == game->m_LocalPlayer->m_Team || player->m_Health <= 0)
            continue;

        // Compensate for nullptr as player closest on first loop
        if (!closest)
            closest = player;
        
        // Get lowest dist between previous closest player and current indexed player
        if (dist(game->m_LocalPlayer->m_HeadPos, player->m_HeadPos) < dist(game->m_LocalPlayer->m_HeadPos, closest->m_HeadPos))
            closest = player;
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
    std::cout << "Press END key to stop" << std::endl;

    // Get addr of .exe module
    uintptr_t moduleBase = (uintptr_t)GetModuleHandle(NULL);

    // Main cheat loop
    while (true)
    {
        if (GetAsyncKeyState(VK_END) & 1)
            break;

        Game* game = new Game;
        game = game->GetInstance();

        // Only run following code if other players are present and valid
        if (Player* closestPlayer = getClosestPlayer(game))
        {
            // Misc
            game->m_LocalPlayer->m_Health = 999;

            // Only aimbot when firing gun
            if (game->m_LocalPlayer->m_IsShooting || game->m_LocalPlayer->m_Shooting)
            {
                // Main aimbot calculation stuff
                Vec3 normalized = normalize(game->m_LocalPlayer->m_HeadPos, closestPlayer->m_HeadPos);

                game->m_LocalPlayer->m_Yaw = (atan2f(normalized.y, normalized.x) * (180 / M_PI)) + 90;
                game->m_LocalPlayer->m_Pitch = (atan2f(normalized.z, dist(game->m_LocalPlayer->m_HeadPos, closestPlayer->m_HeadPos)) * (180 / M_PI));
            }
        }

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