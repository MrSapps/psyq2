#include "types.h"

extern struct POLY_GT4 *LastFMenuPoly;
extern struct POLY_FT4 *MenuPoly;
extern struct POLY_GT4 *FMenuPoly;
extern struct POLY_FT4 *LastMenuPoly;
extern struct DB *currdb;
extern unsigned int LevelTicks;
extern int LevelIsMultiPlayer;
extern struct QENGINELEVINTERFACE Engine;

int QWeapon_NextPrev();
void Assign_NoSprite(int Num, struct SPRITEDATA *Hundred, struct SPRITEDATA *Ten, struct SPRITEDATA *One, int Minus_Sign);

void InitialiseMenuEveryFrame(void)
{
    MenuPoly = (struct POLY_FT4 *)((char *)currdb + 0x3984);
    FMenuPoly = (struct POLY_GT4 *)((char *)currdb + 0x4014);
    LastMenuPoly = (struct POLY_FT4 *)((char *)currdb + 0x4014);
    LastFMenuPoly = (struct POLY_GT4 *)((char *)currdb + 0x41b4);
}

void MegaHealth_Think(struct CREATURE *Cre)
{
    struct PLAYER *Player;
    struct PTIMERS *PTimers;

    Player = Cre->Player;
    PTimers = &Player->Extras.PTimers;
    if (Player->Extras.ExtraBits & 0x20000) {
        if (Cre->Health <= Player->Extras.Max_Health) {
            PTimers->MegaHealth_Timer = 0;
            Player->Extras.ExtraBits ^= 0x20000;
        } else if (LevelTicks > PTimers->MegaHealth_Timer) {
            PTimers->MegaHealth_Timer = LevelTicks + 300;
            Cre->Health--;
            Cre->LastHealth--;
        }
    }
}

void Display_FragCount(struct WINDOW *Window, int Score, struct SPRITEDATA *Hundred, struct SPRITEDATA *Ten, struct SPRITEDATA *One)
{
    if (Score < -99)
        Score = -99;
    if (Score < 0) {
        Hundred->pal = 7;
        Ten->pal = 7;
        One->pal = 7;
    }
    Assign_NoSprite(Score, Hundred, Ten, One, 1);
}

short Pickup_Value(short Max_Value, short Current_Value, short AddOn_Value)
{
    Current_Value += AddOn_Value;
    if (Current_Value > Max_Value)
        return Max_Value;
    return Current_Value;
}

int Pickup_Weapon(struct PLAYER *Player, int WeaponBit, short WeaponNo)
{
    if (Player->Extras.WeaponsAvailable & WeaponBit) {
        if (LevelIsMultiPlayer && Engine.GameConfig.bWeaponStay)
            return 0;
    } else {
        Player->Extras.WeaponsAvailable |= WeaponBit;
        if (Player->Extras.Current_WeaponSelected == 1)
            Player->Extras.FastWeaponSelect = WeaponNo;
    }
    Player->Extras.Next_WeaponSelected = QWeapon_NextPrev(Player, Player->Extras.FastWeaponSelect, 1);
    Player->Extras.Previous_WeaponSelected = QWeapon_NextPrev(Player, Player->Extras.FastWeaponSelect, 0);
    return 1;
}

void Pickup_Ammo(struct PLAYER *Player)
{
    Player->Extras.Next_WeaponSelected = QWeapon_NextPrev(Player, Player->Extras.FastWeaponSelect, 1);
    Player->Extras.Previous_WeaponSelected = QWeapon_NextPrev(Player, Player->Extras.FastWeaponSelect, 0);
}
