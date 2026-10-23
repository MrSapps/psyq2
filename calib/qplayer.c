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

int sound_jump1;
extern int NewVcounts;
extern struct PARTCOLOURS ParticleColours_Blue;
void PlayerNoise(struct edict_s *who, struct VECT *where, int type);
struct PARTICLE *AddParticlesSL(struct VECT *Pos, struct SVECT *Vel, struct PARTCOLOURS *PColour0, struct PARTCOLOURS *PColour1, int NumToDraw, int Timer, int Size, int Area);
int rand(void);

void GenerateWaterSplash(struct VECT *Location, int Area)
{
    int a;
    int b;
    struct SVECT Vel[15];
    struct PARTICLE *Particle;

    for (b = 0; b < 4; b++) {
        for (a = 0; a < 15; a++) {
            Vel[a].vx = (rand() - 0x4000) >> 9;
            Vel[a].vy = (rand() - 0x4000) >> 9;
            Vel[a].vz = (rand() - 0x4000) >> 9;
        }
        Particle = AddParticlesSL(Location, Vel, &ParticleColours_Blue, &ParticleColours_Blue, 15, 25, 0xc00, Area);
        if (Particle)
            Particle->Drift.vy = 4;
    }
}

int QuakePlayerJump(struct CREATURE *Cre, int JumpTapped)
{
    struct PLAYER *Player;

    Player = Cre->Player;
    if (Player->JumpCount) {
        if (Cre->Vel.vy >= 0 || Player->JumpCount < NewVcounts)
            Player->JumpCount = 0;
        else
            Player->JumpCount -= NewVcounts;
        return 0;
    }
    if (!JumpTapped)
        return 0;
    if (Cre->Box.DontJump)
        return 0;
    if (Cre->Box.FloorNorm.vy > Cre->Box.FloorNormMin || Cre->Box.OnLadder) {
        if (Cre->Box.OnLadder) {
            Cre->ForceVel.vx = -Cre->Box.WallNorm.vx;
            Cre->ForceVel.vz = -Cre->Box.WallNorm.vz;
        }
        Cre->ForceVel.vy = -0xc00;
        Player->JumpCount = 0x240;
        Cre->Control |= 0x4000;
        PlayerNoise(Cre->EdictT, &Cre->Box.Location, 0);
        SoundByCreature(sound_jump1, Cre);
        return 1;
    }
    return 0;
}

extern int NewVcounts;
int EffectyDeath(struct CREATURE *Cre);
void ProcessQuakeGeneralCreEffects(struct CREATURE *Cre);
void AddCreToNextBumpList(struct CREATURE *Cre);
void QuakeCreatureSmash(struct CREATURE *Cre, struct SVECT *Force);
void QCre_Move(struct CREATURE *Cre);
void PrimaryKillFade(struct CREATURE *Cre);

extern int LevelIsMultiPlayer;
int Seek(short *a, short b, short c);
void AnimateQuakePlayer(struct CREATURE *Cre, int AnimType);
void ProcessQuakeDeath(struct CREATURE *Cre);
void PrimaryQuakeDeadPlayerBody(struct CREATURE *Cre);

void PrimaryQuakeDyingPlayer(struct CREATURE *Cre)
{
    struct SVECT Vel;

    if (Cre->Health <= Cre->Params[0]) {
        QuakeCreatureSmash(Cre, &(struct SVECT){0, 0, 0});
        return;
    }
    AddCreToNextBumpList(Cre);
    Seek(&Cre->Vel.vx, 0, NewVcounts * 40);
    Seek(&Cre->Vel.vy, 0, NewVcounts * 40);
    Seek(&Cre->Vel.vz, 0, NewVcounts * 40);
    QCre_Move(Cre);
    if (LevelIsMultiPlayer) {
        if (!Cre->AnimFlags)
            AnimateQuakePlayer(Cre, 4);
        else {
            Cre->Control |= 0x80000;
            ProcessQuakeDeath(Cre);
            Cre->Box.Min.vy = 0x8f;
            Cre->Primary = PrimaryQuakeDeadPlayerBody;
            Cre->Box.VectBox.Min.vy = Cre->Box.Location.vy + 0x8f;
            Cre->Control |= 0x8000;
        }
    }
    if (!LevelIsMultiPlayer)
        Cre->Control |= 0x80000;
}

void PrimaryQuakeDeadPlayerBody(struct CREATURE *Cre)
{
    if (!EffectyDeath(Cre)) {
        ProcessQuakeGeneralCreEffects(Cre);
        AddCreToNextBumpList(Cre);
        if (Cre->Health <= Cre->Params[0]) {
            if (Cre->EdictT)
                Cre->EdictT->takedamage = 0;
            QuakeCreatureSmash(Cre, &(struct SVECT){0, 0, 0});
            return;
        }
    }
    QCre_Move(Cre);
    if ((short)(Cre->Life -= NewVcounts) <= 0) {
        Cre->Life = 1;
        Cre->Primary = PrimaryKillFade;
    }
}

void MP_player_male(struct edict_s *self)
{
    struct CREATURE *Cre;

    Cre = self->Extra.Creature;
    self->s.skinnum = 2;
    ((struct CREATURE *)self->Extra.Creature)->Health = 30;
    self->gib_health = -30;
    StartAnim(Cre, QMMAnim_Stand);
}

void PlayerLavaSound(struct CREATURE *Cre)
{
    static int Silent = 0;

    if ((++Silent & 3) == 0)
        SoundByCreature(sound_burn1, Cre);
}

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
