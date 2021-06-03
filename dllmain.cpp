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
        float currentDist = dist(game->m_LocalPlayer->m_HeadPos, player->m_HeadPos);
        float closestDist = dist(game->m_LocalPlayer->m_HeadPos, closest->m_HeadPos);
        if (currentDist < closestDist)
            closest = player;
    }
    
    return closest;
}

bool worldToScreen(Vec3 pos, Vec2 &screen)
{
    ScreenSettings* settings = settings->GetInstance();
    float* viewMatrix = (float*)(VIEW_MATRIX_ADDR); // Built-in assaultcube Model-View-Projection Matrix
    
    // Get clip coords by matrix multiplication of world position by view matrix
    Vec4 clipCoords = {
        pos.x * viewMatrix[0] + pos.y * viewMatrix[4] + pos.z * viewMatrix[8]  + viewMatrix[12],
        pos.x * viewMatrix[1] + pos.y * viewMatrix[5] + pos.z * viewMatrix[9]  + viewMatrix[13],
        pos.x * viewMatrix[2] + pos.y * viewMatrix[6] + pos.z * viewMatrix[10] + viewMatrix[14],
        pos.x * viewMatrix[3] + pos.y * viewMatrix[7] + pos.z * viewMatrix[11] + viewMatrix[15]
    };

    // Check if clipcoords are within view
    if (clipCoords.w < 0.1f)
        return false;

    // Perspective division - divide clip coords by its w to get Normalized Device Coordinates
    Vec3 NDC = {
        clipCoords.x / clipCoords.w,
        clipCoords.y / clipCoords.w,
        clipCoords.z / clipCoords.w
    };

    // Viewport Transform - get actual window coordinates
    screen.x = (settings->m_Width / 2 * NDC.x) + (NDC.x + settings->m_Width / 2);
    screen.y = -(settings->m_Height / 2 * NDC.y) + (NDC.y + settings->m_Height / 2);

    return true;
}

DWORD WINAPI MainThread(HMODULE hModule)
{
    // Create console from thread
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);

    std::cout << R"(
   /$$               /$$                                             /$$
 /$$$$              | $$                                            | $$
|_  $$   /$$$$$$$  /$$$$$$    /$$$$$$   /$$$$$$  /$$$$$$$   /$$$$$$ | $$
  | $$  | $$__  $$|_  $$_/   /$$__  $$ /$$__  $$| $$__  $$ |____  $$| $$
  | $$  | $$  \ $$  | $$    | $$$$$$$$| $$  \__/| $$  \ $$  /$$$$$$$| $$
  | $$  | $$  | $$  | $$ /$$| $$_____/| $$      | $$  | $$ /$$__  $$| $$
 /$$$$$$| $$  | $$  |  $$$$/|  $$$$$$$| $$      | $$  | $$|  $$$$$$$| $$
|______/|__/  |__/   \___/   \_______/|__/      |__/  |__/ \_______/|__/)" << std::endl;

    std::cout << "\nPress END key to stop" << std::endl;

    uintptr_t moduleBase = (uintptr_t)GetModuleHandle(NULL);    // Get addr of .exe module
    HDC deviceContext = GetDC(FindWindow(0, L"AssaultCube"));   // Get device context from window handle

    HBRUSH brush = CreateSolidBrush(RGB(255, 0, 0));

    // Main cheat loop
    while (!(GetAsyncKeyState(VK_END) & 1))
    {
        // Refresh game struct
        Game* game = game->GetInstance();

        // Only run following code if other players are present and valid
        if (Player* closestPlayer = getClosestPlayer(game))
        {
            // Misc
            game->m_LocalPlayer->m_Health = 999;
            game->m_LocalPlayer->m_PrimaryAmmo = 999;
            game->m_LocalPlayer->m_SecondaryAmmo = 999;
            game->m_LocalPlayer->m_GrenadeAmmo = 99;

            // Only aimbot when firing gun
            if (game->m_LocalPlayer->m_IsShooting || game->m_LocalPlayer->m_Shooting)
            {
                // Main aimbot calculation stuff
                Vec3 normalized = normalize(game->m_LocalPlayer->m_HeadPos, closestPlayer->m_HeadPos);

                game->m_LocalPlayer->m_Yaw = (float)(atan2f(normalized.y, normalized.x) * (180 / M_PI)) + 90;
                game->m_LocalPlayer->m_Pitch = (float)(atan2f(normalized.z, dist(game->m_LocalPlayer->m_HeadPos, closestPlayer->m_HeadPos)) * (180 / M_PI));
            }

            // ESP
            
        }

        Sleep(1); // Preserve some resources
    }

    // Cleanup and exit
    DeleteObject(brush);
    DeleteObject(deviceContext);
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
        HANDLE hThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)MainThread, hModule, NULL, NULL);
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