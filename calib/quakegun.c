#include "types.h"

extern char WeaponNameTokens[12][3];
extern int NewVcounts;
void bprintf(struct WINDOW *Window, char *_format, ...);
char QWeapon_NextPrev(struct PLAYER *Player, int Weapon_Type, int Next);

void QuakeWeaponChange(struct CREATURE *Cre, struct PLAYERCONTROLS *Controls)
{
    struct PLAYER *Player;
    struct PTIMERS *PTimers;
    struct CREATURE *Gun;
    int ChangeWeaponIcon;
    char Temp_Weapon;

    ChangeWeaponIcon = 0;
    Player = Cre->Player;
    Gun = (struct CREATURE *)Cre->Params[0];
    PTimers = &Player->Extras.PTimers;
    if (Controls->ChangeWeapUpTapped) {
        PTimers->WeapChange_Timer = 70;
        Temp_Weapon = QWeapon_NextPrev(Player, Player->Extras.FastWeaponSelect, 1);
        if (Temp_Weapon)
            Player->Extras.FastWeaponSelect = Temp_Weapon;
        ChangeWeaponIcon = 1;
    } else if (Controls->ChangeWeapDownTapped) {
        PTimers->WeapChange_Timer = 70;
        Temp_Weapon = QWeapon_NextPrev(Player, Player->Extras.FastWeaponSelect, 0);
        if (Temp_Weapon)
            Player->Extras.FastWeaponSelect = Temp_Weapon;
        ChangeWeaponIcon = 1;
    } else {
        PTimers->WeapChange_Timer -= NewVcounts;
        if ((signed char)PTimers->WeapChange_Timer < -1)
            *(signed char *)&PTimers->WeapChange_Timer = -1;
        if (Player->Extras.FastWeaponSelect != Gun->WayDest)
            Gun->WayDest = Player->Extras.FastWeaponSelect;
    }
    if (ChangeWeaponIcon) {
        Player->Extras.Next_WeaponSelected = QWeapon_NextPrev(Player, Player->Extras.FastWeaponSelect, 1);
        Player->Extras.Previous_WeaponSelected = QWeapon_NextPrev(Player, Player->Extras.FastWeaponSelect, 0);
        if (Cre->Player->Window)
            bprintf(Cre->Player->Window, "Selected %s", WeaponNameTokens[Player->Extras.FastWeaponSelect]);
    }
}

extern enum QuakeWeaponTypes WeaponBits[];
extern enum QuakeAmmoTypes AmmoTypes[];
int MinAmmo[12];
static enum QuakeWeaponTypes WeaponTypes[8];

void NoAmmoWeaponChange(struct PLAYER *Player)
{
    struct CREATURE *PlayerCre;
    struct CREATURE *Gun;
    int i;

    PlayerCre = Player->Creature;
    Gun = (struct CREATURE *)PlayerCre->Params[0];
    for (i = 0;; i++) {
        if ((Player->Extras.WeaponsAvailable & WeaponBits[WeaponTypes[i]]) &&
            Player->Extras.AmmoCount[AmmoTypes[WeaponTypes[i]]] >= MinAmmo[WeaponTypes[i]]) {
            Gun->WayDest = Player->Extras.FastWeaponSelect = WeaponTypes[i];
            return;
        }
    }
}

char QWeapon_NextPrev(struct PLAYER *Player, int Weapon_Type, int Next)
{
    int Increment;

    Increment = -1;
    if (Next)
        Increment = 1;
    while (1) {
        Weapon_Type += Increment;
        if (Weapon_Type <= 0)
            Weapon_Type = 12;
        else if (Weapon_Type > 12)
            Weapon_Type = 1;
        if (!(Player->Extras.WeaponsAvailable & WeaponBits[Weapon_Type]))
            continue;
        if (Player->Extras.AmmoCount[AmmoTypes[Weapon_Type]] < MinAmmo[Weapon_Type])
            continue;
        break;
    }
    if (Weapon_Type == Player->Extras.FastWeaponSelect)
        return 0;
    return Weapon_Type;
}
