#pragma once
#include <Windows.h>
#define OFFSET_GAME                0x50F4E8

struct Vec3
{
	float x, y, z;
};

class Weapon
{
public:
	char pad_0000[20]; //0x0000
	class Ammo* ammoptr; //0x0014
}; //Size: 0x0018

class Ammo
{
public:
	int ammo; //0x0000
}; //Size: 0x0004


class Player
{
public:
	Vec3 m_HeadPos; //0x0004 
	Vec3 m_Velocity; //0x0010 
	char _0x001C[24];
	Vec3 m_FootPos; //0x0034 
	float m_Yaw; //0x0040 
	float m_Pitch; //0x0044 
	float m_Roll; //0x0048 
	float m_PitchVel; //0x004C
	float m_MaxSpeed; //0x0050
	__int32 m_AirTime; //0x0054 
	float m_Radius; //0x0058 
	float m_Height; //0x005C 
	float m_NormalHeight; //0x0060 
	float m_HeightAboveHead; //0x0064 
	BYTE m_IsInWater; //0x0068 
	BYTE m_IsOnFloor; //0x0069 
	BYTE m_IsOnLadder; //0x006A 
	BYTE m_JumpNext; //0x006B 
	BYTE m_Crouching; //0x006C 
	BYTE m_AirCrouch; //0x006D 
	char _0x006E[1];
	BYTE m_CanCollide; //0x006F 
	BYTE m_IsNotInGame; //0x0070 
	BYTE m_Scoping; //0x0071 
	BYTE m_Shooting; //0x0072 
	char _0x0073[5];
	float m_LastJumpYPos; //0x0078 
	__int32 m_LastSplash; //0x007C 
	BYTE m_Move; //0x0080 
	BYTE m_Strafe; //0x0081 
	BYTE m_State; //0x0082 
	BYTE m_Type; //0x0083 
	char _0x0084[116];
	__int32 m_Health; //0x00F8 
	__int32 m_Armor; //0x00FC 
	__int32 m_Primary; //0x0100 
	char _0x0104[4];
	__int32 m_GunSelect; //0x0108 
	BYTE m_AkimboEquipped; //0x010C 
	char _0x010D[7];
	__int32 m_SecondaryAmmoInReserve; //0x0114 
	char _0x0118[16];
	__int32 m_PrimaryAmmoInReserve; //0x0128 
	char _0x012C[8];
	__int32 m_AkimboAmmoInReserve; //0x0134 
	char _0x0138[4];
	__int32 m_SecondaryAmmo; //0x013C 
	char _0x0140[16];
	__int32 m_PrimaryAmmo; //0x0150 
	char _0x0154[4];
	__int32 m_GrenadeAmmo; //0x0158 
	__int32 m_AkimboAmmo; //0x015C 
	__int32 m_MeleeDelayTime; //0x0160 
	__int32 m_SecondaryDelayTime; //0x0164 
	char _0x0168[16];
	__int32 m_PrimaryDelayTime; //0x0178 
	char _0x017C[4];
	__int32 m_GrenadeWaitingTime; //0x0180 
	__int32 m_AkimboWaitingTime; //0x0184 
	__int32 m_MeleeShotsFired; //0x0188 
	__int32 m_SecondaryShotsFired; //0x018C 
	char _0x0190[16];
	__int32 m_PrimaryShotsFired; //0x01A0 
	char _0x01A4[4];
	__int32 m_GrenadeShotsFired; //0x01A8 
	__int32 m_AkimboShotsFired; //0x01AC 
	char _0x01B0[44];
	__int32 m_Skin; //0x01DC 
	__int32 m_NextSkin; //0x01E0 
	__int32 m_ClientID; //0x01E4 
	__int32 m_LastUpdate; //0x01E8 
	__int32 m_Lag; //0x01EC 
	__int32 m_Ping; //0x01F0 
	DWORD m_IPaddress; //0x01F4 
	__int32 m_TimesRespawned; //0x01F8 
	__int32 m_Kills; //0x01FC 
	__int32 m_FlagScore; //0x0200 
	__int32 m_Deaths; //0x0204 
	__int32 m_Score; //0x0208 
	__int32 m_TeamKills; //0x020C 
	__int32 m_LastAction; //0x0210 
	__int32 m_LastMove; //0x0214 
	__int32 m_LastHurt; //0x0218 
	__int32 m_LastVoiceCom; //0x021C 
	__int32 m_ClientRole; //0x0220 
	BYTE m_IsShooting; //0x0224 
	char m_Name[260]; //0x0225 
	__int32 m_Team; //0x0329 
	__int32 m_Weaponchanging; //0x032D 
	__int32 m_NextWeapon; //0x0331 
	__int32 m_SpectateMode; //0x0335 
	__int32 m_SpectateCN; //0x0339 
	__int32 m_EarDamageMillis; //0x033D 
	__int32 m_RespawnOffset; //0x0341 
	char __0xbuf[7]; //0x0225
	Weapon* m_Weapons[10]; //0x348
	//char _0x0345[4];

    virtual void Function0();
    virtual void Function1();
    virtual void Function2();
    virtual void Function3();
    virtual void Function4();
    virtual void Function5();
    virtual void Function6();
    virtual void Function7();
    virtual void Function8();
    virtual void Function9();
}; //Size: 0x0858

class PlayerList
{
public:
    Player* m_Players[32];
};//Size=0x0044

class Game
{
public:
    char _0x0000[12];
    Player* m_LocalPlayer; //0x000C 
    PlayerList* m_OtherPlayers; //0x0010 
    __int32 m_MaxPlayerCount; //0x0014 
    __int32 m_PlayerCount; //0x0018 
    char _0x001C[20];

    Player* GetPlayer(int index) {
        if (index < 0)
            return NULL;
        if (index >= 33)
            return NULL;
        if (this->m_OtherPlayers) {
            if (this->m_OtherPlayers->m_Players[index]) {
                return this->m_OtherPlayers->m_Players[index];
            }
        }

        return NULL;
    }

    static Game* GetInstance() {
        return (Game*)(OFFSET_GAME);
    }

};//Size=0x0030