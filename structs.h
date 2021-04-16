#pragma once
#include <Windows.h>
#define OFFSET_GAME                0x50F4E8

struct Vec3
{
	float x, y, z;
};

class Player
{
public:
    Vec3 headpos; //0x0004
    char pad_0010[36]; //0x0010
    Vec3 pos; //0x0034
    Vec3 angle; //0x0040
    char pad_004C[172]; //0x004C
    int health; //0x00F8
    char pad_00FC[297]; //0x00FC
    char N000002B6[16]; //0x0225
    char pad_0235[319]; //0x0235
    class weapon* currWeapon; //0x0374
    char pad_0378[1248]; //0x0378

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

class weapon
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