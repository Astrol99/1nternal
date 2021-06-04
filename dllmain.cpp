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
    return Vec3{
        dst.x - src.x,
        dst.y - src.y,
        dst.z - src.z
    };
}

// 3D Euclidian distance forumula function
float dist(Vec3 src, Vec3 dst)
{
    const Vec3 normalized = normalize(src, dst);
    return sqrtf(powf(normalized.x, 2) + powf(normalized.y, 2) + powf(normalized.z, 2));
}

bool worldToScreen(Vec3 pos, Vec2 &screen)
{
    ScreenSettings* settings = settings->GetInstance();
    const float* viewMatrix = (float*)(VIEW_MATRIX_ADDR); // Built-in assaultcube Model-View-Projection Matrix

    // Get clip coords by matrix multiplication of world position by view matrix
    const Vec4 clipCoords = {
        pos.x * viewMatrix[0] + pos.y * viewMatrix[4] + pos.z * viewMatrix[8]  + viewMatrix[12],
        pos.x * viewMatrix[1] + pos.y * viewMatrix[5] + pos.z * viewMatrix[9]  + viewMatrix[13],
        pos.x * viewMatrix[2] + pos.y * viewMatrix[6] + pos.z * viewMatrix[10] + viewMatrix[14],
        pos.x * viewMatrix[3] + pos.y * viewMatrix[7] + pos.z * viewMatrix[11] + viewMatrix[15]
    };

    // Check if clipcoords are within player view
    if (clipCoords.w < 0.1f)
        return false;

    // Perspective division - divide clip coords by its w to get Normalized Device Coordinates
    const Vec3 NDC = {
        clipCoords.x / clipCoords.w,
        clipCoords.y / clipCoords.w,
        clipCoords.z / clipCoords.w
    };

    // Viewport Transform - get actual window coordinates
    screen.x = (settings->m_Width / 2 * NDC.x) + (NDC.x + settings->m_Width / 2);
    screen.y = -(settings->m_Height / 2 * NDC.y) + (NDC.y + settings->m_Height / 2);

    return true;
}

void drawBorderBox(HDC& hdc, COLORREF color, int thickness, float x, float y, float w, float h)
{
    HBRUSH brush = CreateSolidBrush(color);

    RECT top = { x, y, w, y + thickness };
    FillRect(hdc, &top, brush);

    RECT bottom = { x, h, w, h + thickness };
    FillRect(hdc, &bottom, brush);

    RECT left = { x, y, x + thickness, h };
    FillRect(hdc, &left, brush);

    RECT right = { w, y, w + thickness, h };
    FillRect(hdc, &right, brush);

    DeleteObject(brush);
}

void drawHealthBox(HDC& hdc, int health, int thickness, float x, float y, float w, float h)
{
    // Draw box containing health
    drawBorderBox(hdc, RGB(255, 255, 255), thickness, x, y, w, h);

    HBRUSH brush;

    if (health > 75)
        brush = CreateSolidBrush(RGB(0, 255, 0));   // Green
    else
        brush = CreateSolidBrush(RGB(255, 255 * (health / 100.0f), 0)); // Scale color from orange to red based on health
    
    float healthPixelHeight = (h - y) * (health / 100.0f);  // Get amount of height pixels inside box in coorelation with health percentage
    RECT healthBox = { x, h - healthPixelHeight, w, h };    // Apply health pixels to become relative to overall health box
    FillRect(hdc, &healthBox, brush);

    DeleteObject(brush);
}

void drawString(HDC& hdc, int x, int y, COLORREF color, const char* text)
{
    HFONT Font = CreateFont(16, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Roboto"));

    SetTextAlign(hdc, TA_CENTER | TA_NOUPDATECP);

    SetBkColor(hdc, RGB(0, 0, 0));
    SetBkMode(hdc, TRANSPARENT);

    SetTextColor(hdc, color);

    SelectObject(hdc, Font);

    TextOutA(hdc, x, y, text, strlen(text));

    DeleteObject(Font);
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

    const uintptr_t moduleBase = (uintptr_t)GetModuleHandle(NULL);    // Get addr of .exe module

    // GDI related drawing stuff
    HDC hdc = GetDC(FindWindow(0, L"AssaultCube"));   // Get device context from window handle

    // Main cheat loop
    while (!(GetAsyncKeyState(VK_END) & 1))
    {
        // Refresh game-related structs
        Game* game = game->GetInstance();
        ScreenSettings* settings = settings->GetInstance();

        // Only run following code if other players are present
        if (game->m_PlayerCount > 1)
        {
            // Misc
            game->m_LocalPlayer->m_Health = 999;
            game->m_LocalPlayer->m_PrimaryAmmo = 999;
            game->m_LocalPlayer->m_SecondaryAmmo = 999;
            game->m_LocalPlayer->m_GrenadeAmmo = 99;

            Player* closestPlayer = nullptr;

            // Entity loop
            for (int i = 1; i < game->m_PlayerCount; i++)
            {   
                Player* player = game->GetPlayer(i);

                // Skip if current indexed player is either a teammate or dead
                if (player->m_Team == game->m_LocalPlayer->m_Team || player->m_Health <= 0)
                    continue;

                // Compensate for nullptr as player closest on first loop
                if (!closestPlayer)
                    closestPlayer = player;

                // ESP Stuff
                Vec2 screenHead;
                Vec2 screenFeet;
                const int widthPad = 25;
                const int heightPad = 10;
                const int thickness = 1;
                if (worldToScreen(player->m_HeadPos, screenHead) && worldToScreen(player->m_FootPos, screenFeet))
                {
                    // Draw ESP box around enemies with GDI
                    drawBorderBox(hdc, RGB(255,0,0), thickness, screenHead.x - widthPad, screenHead.y - heightPad, screenFeet.x + widthPad, screenFeet.y);
                    drawHealthBox(hdc, player->m_Health, thickness, screenFeet.x + widthPad + thickness, screenHead.y - heightPad, screenHead.x + widthPad + 10, screenFeet.y);
                    drawString(hdc, screenFeet.x, screenFeet.y, RGB(255,255,255), player->m_Name);
                }
            
                // Getting Closest Player
                // Get lowest dist between previous closest player and current indexed player
                float currentDist = dist(game->m_LocalPlayer->m_HeadPos, player->m_HeadPos);
                float closestDist = dist(game->m_LocalPlayer->m_HeadPos, closestPlayer->m_HeadPos);
                if (currentDist < closestDist)
                    closestPlayer = player;
            }

            // Only aimbot when firing gun
            if (game->m_LocalPlayer->m_IsShooting || game->m_LocalPlayer->m_Shooting)
            {
                // Main aimbot calculation stuff
                const Vec3 normalized = normalize(game->m_LocalPlayer->m_HeadPos, closestPlayer->m_HeadPos);

                // Calculate using basic trigonometry functions to find view angles given length in protrayed triangle between the localplayer and closest enemy
                game->m_LocalPlayer->m_Yaw = (float)(atan2f(normalized.y, normalized.x) * (180 / M_PI)) + 90;
                game->m_LocalPlayer->m_Pitch = (float)(atan2f(normalized.z, dist(game->m_LocalPlayer->m_HeadPos, closestPlayer->m_HeadPos)) * (180 / M_PI));
            }
        }

        Sleep(1); // Preserve some resources
    }

    // Cleanup and exit
    ReleaseDC(0, hdc);
    DeleteObject(hdc);
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