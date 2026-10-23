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

extern int LevelIsMultiPlayer;
extern struct QENGINELEVINTERFACE Engine;
extern struct WINDOW GameWindows[];
extern struct PLAYER Players[];
extern unsigned char QuakeIconTable[];

void Display_WeaponChange(struct WINDOW *Window, struct CREATURE *Cre, struct SPRITEDATA *Icon)
{
    struct PLAYER *Player;
    short PosOff;
    unsigned char *p;
    unsigned char *Table;

    Player = &Players[Window - GameWindows];
    Table = QuakeIconTable;
    PosOff = Player->Extras.FastWeaponSelect * 5;
        Icon->u = Table[PosOff];
    Icon->v = Table[PosOff + 1];
    if (LevelIsMultiPlayer) {
        if (PosOff) {
            if (Engine.GameConfig.sNumPlayers == 2) {
                Icon->w = 24;
                Icon->h = 18;
            } else {
                Icon->w = 16;
                Icon->h = 12;
            }
        } else {
            Icon->w = 1;
            Icon->h = 1;
        }
    } else {
        Icon->w = Table[PosOff + 2];
        Icon->h = Table[PosOff + 3];
    }
    Icon->pal = Table[PosOff + 4];
}

unsigned char BCDNo[3];

void BCD_Numbers(short Num, int Minus_Sign)
{
    short TempNo;
    short NormNo;

    NormNo = Num;
    if (Minus_Sign) {
        if (Num >= 0)
            Minus_Sign = 0;
        else
            NormNo = ~Num + 1;
    }
    TempNo = NormNo / 100;
    if (TempNo > 0) {
        NormNo -= TempNo * 100;
        BCDNo[0] = TempNo;
    } else
        BCDNo[0] = 11;
    TempNo = NormNo / 10;
    if (TempNo > 0) {
        NormNo -= TempNo * 10;
        BCDNo[1] = TempNo;
    } else if (BCDNo[0] == 11)
        BCDNo[1] = 11;
    if (NormNo >= 0)
        BCDNo[2] = NormNo;
    if (Minus_Sign) {
        if (BCDNo[1] == 11)
            BCDNo[1] = 10;
        else
            BCDNo[0] = 10;
    }
}

extern int LevelIsMultiPlayer;
extern struct QENGINELEVINTERFACE Engine;
extern unsigned int LevelTicks;
extern unsigned char QuakeIconTable[];
void Assign_NoSprite(int Num, struct SPRITEDATA *Hundred, struct SPRITEDATA *Ten, struct SPRITEDATA *One, int Minus_Sign);

void Display_Health(struct WINDOW *Window, struct CREATURE *Cre, struct SPRITEDATA *Icon, struct SPRITEDATA *Hundred, struct SPRITEDATA *Ten, struct SPRITEDATA *One)
{
    {
        short PosOff;
        unsigned char *Table;

        Table = QuakeIconTable;
        PosOff = 0xaa;
        Icon->u = Table[PosOff];
        Icon->v = Table[PosOff + 1];
        if (LevelIsMultiPlayer) {
            if (Engine.GameConfig.sNumPlayers == 2) {
                Icon->w = 24;
                Icon->h = 18;
            } else {
                Icon->w = 16;
                Icon->h = 12;
            }
        } else {
            Icon->w = Table[PosOff + 2];
            Icon->h = Table[PosOff + 3];
        }
        Icon->pal = Table[PosOff + 4];
    }
    if (Cre) {
        if (Cre->Health < -99)
            Cre->Health = -99;
        if (Cre->Health < 26) {
            if (Cre->Health <= 0 || !(LevelTicks & 0x80)) {
                Hundred->pal = 7;
                Ten->pal = 7;
                One->pal = 7;
            }
        }
        Window->Items.Health = Cre->Health;
        Assign_NoSprite(Cre->Health, Hundred, Ten, One, 1);
    } else {
        Hundred->pal = 7;
        Ten->pal = 7;
        One->pal = 7;
        Assign_NoSprite(Window->Items.Health, Hundred, Ten, One, 1);
    }
}

extern struct ARMOUR_TYPE Armour_Types[];

short Pickup_Armour(struct PLAYER *Player, char New_Armour)
{
    int Salvage;
    short *Old_Armour;
    short Armour_Count;

    Old_Armour = &Player->Extras.Max_ArmourType;
    Armour_Count = Player->Extras.Armour_Count;
    if (!Armour_Count) {
        Player->Extras.Max_ArmourType = 0;
        Player->Extras.ExtraBits &= 0x78fff;
        if (Armour_Count < New_Armour)
            Player->Extras.Max_ArmourType = New_Armour;
    }
    if (New_Armour < *Old_Armour) {
        Salvage = (Armour_Types[New_Armour].Normal_Protection * 4) / Armour_Types[*Old_Armour].Normal_Protection;
        Armour_Count = Player->Extras.Armour_Count + ((Salvage * Armour_Types[New_Armour].Base_Count) >> 2);
        Player->Extras.Armour_Count = 0;
    } else if (*Old_Armour != New_Armour) {
        Salvage = (Armour_Types[*Old_Armour].Normal_Protection * 4) / Armour_Types[New_Armour].Normal_Protection;
        Armour_Count = Armour_Types[New_Armour].Base_Count + ((Salvage * Player->Extras.Armour_Count) >> 2);
        Player->Extras.Armour_Count = 0;
        Player->Extras.Max_ArmourType = New_Armour;
    } else
        Armour_Count = Armour_Types[*Old_Armour].Base_Count;
    return Armour_Count;
}
