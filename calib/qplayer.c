#include "types.h"

/* defined in this file (qplayer.obj), so $gp-relative */
int sound_burn1;
int sound_spawnplayer;
struct ANIMHDR *QMMAnim_Stand;

extern struct QENGINELEVINTERFACE Engine;

struct STARTPOSITION *GetRandomMultiplayerStart(void);
struct CREATURE *InitialiseQuakePlayer(int Win, struct STARTPOSITION *Start);
void CheckForAllWeap(struct PLAYER *Player);
int SoundByCreature(int SFXNumber, struct CREATURE *Cre);
void StartAnim(struct CREATURE *Cre, struct ANIMHDR *Anim);
void InitMultiQuakePlayer(int Win);

void ReInitMultiQuakePlayer(int Win)
{
    if (Engine.GameConfig.sMultiMode != 5)
        InitMultiQuakePlayer(Win);
}

void InitMultiQuakePlayer(int Win)
{
    struct PLAYER *Player;
    struct CREATURE *Cre;

    Cre = InitialiseQuakePlayer(Win, GetRandomMultiplayerStart());
    if (!Cre)
        for (;;)
            ;
    Cre->HitBy = 4;
    Player = Cre->Player;
    CheckForAllWeap(Player);
    Player->Extras.ExtraBits = 0;
    StartAnim(Cre, QMMAnim_Stand);
    SoundByCreature(sound_spawnplayer, Cre);
}

void PlayerToxicSound(struct CREATURE *Cre)
{
    SoundByCreature(sound_burn1, Cre);
}

int AnimateCreature(struct CREATURE *Cre, int TicksPassed, int *EndFrame)
{
    if (Cre->SequenceHdr) {
        Cre->AnimFrame += TicksPassed;
        if (Cre->AnimFrame > Cre->SequenceHdr->AnimDetails.Stop) {
            Cre->AnimFlags |= 1;
            TicksPassed = Cre->AnimFrame - Cre->SequenceHdr->AnimDetails.Stop;
            *EndFrame = Cre->SequenceHdr->AnimDetails.Stop - Cre->SequenceHdr->AnimDetails.Start + 10;
            Cre->AnimFrame = Cre->SequenceHdr->AnimDetails.Wrap;
            return TicksPassed;
        }
        *EndFrame = Cre->AnimFrame - Cre->SequenceHdr->AnimDetails.Start;
        return 0;
    }
    return 0;
}

void StartAnim(struct CREATURE *Cre, struct ANIMHDR *Anim)
{
    if (Anim && Cre) {
        Cre->SequenceHdr = Anim;
        Cre->AnimFrame = Anim->AnimDetails.Start;
        Cre->AnimFlags &= ~3;
    }
}
