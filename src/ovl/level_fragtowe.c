/* Multiplayer arena module, shared by FRAGTOWE, MATRIX1-9, PODCITY, THEVAT and TIMS
   (the original's PrintF names it QMULTI.C). */
#include "ovl.h"

#define Engine LevelInterface.Engine
#define Config Engine->GameConfig

void InitLevel(void);
void QMultiPlayer_AdvanceScoresCheckLevelComplete(int Killer, int Victim);
struct QLEVELINTERFACE LevelInterface OVL_HEADER = { InitLevel, QMultiPlayer_AdvanceScoresCheckLevelComplete, 0 };

void CheckTimeLimit(void);

static void SpawnPickups(void)
{
    Engine->LoadSaveCreatureBatches(&(struct IDENTITYNAME){"MultiBatches"}, 0);
    Engine->ActivateCreatureBatch((struct IDENTITYNAME){"Weapons"});
    if (Config.sMultiMode != 5) {
        Engine->ActivateCreatureBatch((struct IDENTITYNAME){"Health"});
        Engine->ActivateCreatureBatch((struct IDENTITYNAME){"Armour"});
        Engine->ActivateCreatureBatch((struct IDENTITYNAME){"Ammo"});
    }
    Engine->ActivateCreatureBatch((struct IDENTITYNAME){"Specials"});
    if (Config.sMultiMode >= 2 && Config.sMultiMode <= 4)
        Engine->ActivateCreatureBatch((struct IDENTITYNAME){"RedFlag"});
    if (Config.sMultiMode == 2)
        Engine->ActivateCreatureBatch((struct IDENTITYNAME){"BlueFlag"});
}

static short SelfPlay0[2] = { -1, 0 };
static short SelfPlay1[2] = { -1, 0 };
static short SelfPlay2[2] = { -1, 0 };
static short SelfPlay3[2] = { -1, 0 };
static short SelfPlay4[2] = { -1, 0 };
static short SelfPlay5[2] = { -1, 0 };
static struct FET_MESSAGE FET_Blank[2] = { { "", 256, 20, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0 } };
static struct FET_MESSAGE FET_TimeUp[2] = { { "TIME UP", 256, 120, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0 } };
static struct FET_MESSAGE FET_GameOver[2] = { { "GAME OVER", 256, 120, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0 } };
static struct FET_MESSAGE FET_RoundOver[2] = { { "ROUND OVER", 256, 120, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0 } };
static struct FET_MESSAGE FET_RoundDrawn[2] = { { "ROUND DRAWN", 256, 120, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0 } };
static int MessageShown = 0;
static int EndCountdown = 450;
static int LastAlive = 0;
static int GameOverState = 0;

/* 0-3: that player won, 4-7: that team won, 8: a draw */
static int FindWinner(void)
{
    short Score[4];
    short Index[4];
    int Present[4];
    int Total[4];
    int i, j, n;
    int Sorted;

    Sorted = 0;
    if (Config.sMultiMode == 5)
        return LastAlive;
    if (Config.sMultiMode == 1) {
        for (i = 0; i < 4; i++) {
            Present[i] = 0;
            Total[i] = 0;
        }
        for (i = 0; i < *Engine->MaxWindows; i++) {
            Present[Config.MultiPlayerParams[i].teamSelected] = 1;
            Total[i] += Engine->Frags[i];
        }
        n = 0;
        for (i = 0; i < *Engine->MaxWindows; i++)
            if (Present[i])
                n++;
        for (i = 0, j = 0; i < 4; i++) {
            if (Present[i]) {
                Score[j] = Total[i];
                Index[j] = i;
                j++;
            }
        }
    } else {
        n = *Engine->MaxWindows;
        for (i = 0; i < *Engine->MaxWindows; i++) {
            Score[i] = Engine->Frags[i];
            Index[i] = i;
        }
    }
    while (!Sorted) {
        Sorted = 1;
        for (i = 0; i < n - 1; i++) {
            if (Score[i] < Score[i + 1]) {
                Score[i] ^= Score[i + 1];
                Score[i + 1] ^= Score[i];
                Score[i] ^= Score[i + 1];
                Index[i] ^= Index[i + 1];
                Index[i + 1] ^= Index[i];
                Index[i] ^= Index[i + 1];
                Sorted = 0;
            }
        }
    }
    if (Score[0] == Score[1])
        return 8;
    if (Config.sMultiMode == 1)
        return Index[0] + 4;
    return Index[0];
}

static int PrintWinner(char *Buf)
{
    switch (FindWinner()) {
    case 0:
        Engine->sprintf(Buf, "%s WINS", Config.MultiPlayerParams[0].playerName);
        if (Config.sMultiMode == 1)
            return Config.MultiPlayerParams[0].teamSelected;
        return Config.MultiPlayerParams[0].playerSelected;
    case 1:
        Engine->sprintf(Buf, "%s WINS", Config.MultiPlayerParams[1].playerName);
        if (Config.sMultiMode == 1)
            return Config.MultiPlayerParams[1].teamSelected;
        return Config.MultiPlayerParams[1].playerSelected;
    case 2:
        Engine->sprintf(Buf, "%s WINS", Config.MultiPlayerParams[2].playerName);
        if (Config.sMultiMode == 1)
            return Config.MultiPlayerParams[2].teamSelected;
        return Config.MultiPlayerParams[2].playerSelected;
    case 3:
        /* the original reads player 4's (nonexistent) team and colour here */
        Engine->sprintf(Buf, "%s WINS", Config.MultiPlayerParams[3].playerName);
        if (Config.sMultiMode == 1)
            return Config.MultiPlayerParams[4].teamSelected;
        return Config.MultiPlayerParams[4].playerSelected;
    case 4:
        Engine->sprintf(Buf, "BLUE TEAM WIN");
        return 0;
    case 5:
        Engine->sprintf(Buf, "RED TEAM WIN");
        return 1;
    case 6:
        Engine->sprintf(Buf, "PURPLE TEAM WIN");
        return 2;
    case 7:
        Engine->sprintf(Buf, "GREEN TEAM WIN");
        return 3;
    case 8:
        Engine->sprintf(Buf, "DRAWN MATCH");
        return 0;
    }
    return 0;
}

void QMultiPlayer_AdvanceScoresCheckLevelComplete(int Killer, int Victim)
{
    short Mode;
    int Score;
    int i, Alive;

    if (GameOverState)
        return;
    if (Killer < 0)
        Killer = Victim;
    Engine->KillsMatrix[Victim][Victim]++; /* sic: the killer is not used */
    if (Config.sMultiMode == 1) {
        if (Config.MultiPlayerParams[Killer].teamSelected == Config.MultiPlayerParams[Victim].teamSelected) {
            Engine->Frags[Killer]--;
            Engine->Scores[Config.MultiPlayerParams[Killer].teamSelected]--;
        } else {
            Engine->Frags[Killer]++;
            Engine->Scores[Config.MultiPlayerParams[Killer].teamSelected]++;
        }
        Score = Engine->Scores[Config.MultiPlayerParams[Killer].teamSelected];
    } else {
        if (Killer == Victim)
            Engine->Frags[Victim]--;
        else
            Engine->Frags[Killer]++;
        Score = Engine->Frags[Killer];
    }
    Mode = Config.sMultiMode;
    if (Mode == 0 || Mode == 1) {
        if (Config.sFragLimit != -1 && Score == Config.sFragLimit)
            GameOverState = 2;
    } else if (Mode == 5) {
        Alive = 0;
        for (i = 0; i < Config.sNumPlayers; i++) {
            if (Engine->GameWindows[i].Creature && ((struct CREATURE *)Engine->GameWindows[i].Creature)->Health > 0) {
                LastAlive = i;
                Alive++;
            }
        }
        if (Alive == 1) {
            Engine->Scores[LastAlive]++;
            if (Engine->Scores[LastAlive] == Config.sScoreLimit)
                GameOverState = 4;
            else
                GameOverState = 3;
        } else if (Alive == 0) {
            GameOverState = 5;
        }
    }
}

void CheckTimeLimit(void)
{
    if (!GameOverState && Config.sTimeLimit != -1 && Config.sTimeLimit * 18000 < *Engine->LevelTicks)
        GameOverState = 1;
}

static void LevelSpecific(void)
{
    char Buf[32];
    struct FET_MESSAGE *Message;
    int Winner;

    CheckTimeLimit();
    if (GameOverState) {
        EndCountdown -= *Engine->NewVcounts;
        if (!MessageShown) {
            MessageShown = GameOverState;
            if (GameOverState == 1)
                Message = FET_TimeUp;
            else if (GameOverState == 3)
                Message = FET_RoundOver;
            else if (GameOverState == 5)
                Message = FET_RoundDrawn;
            else
                Message = FET_GameOver;
            Engine->cursorupper = 0;
            Engine->cursorlower = 0;
            Engine->CantPause = 1;
            Engine->InGameInitScreen(45);
            Engine->FrontEndCreateMenu(FET_Blank, 16);
            if (GameOverState != 5) {
                Winner = PrintWinner(Buf);
                Engine->sprintf(Engine->MyStringTable[0].message, Buf);
                Engine->FrontEndModifyText(Engine->MyStringTable, Buf);
                Engine->MyStringTable[0].window = 4;
                Engine->MyStringTable[0].cursorItem = 1;
                Engine->MyStringTable[0].bannerColour = Winner;
            }
            Engine->FrontEndCreateMenu(Message, 32);
            Engine->MyStringTable[1].window = 4;
            Engine->MyStringTable[1].cursorItem = 1;
        }
        Engine->DoFrontEndStuff();
        if (EndCountdown < 0) {
            if (GameOverState == 3)
                *Engine->LevelComplete = 19;
            else if (GameOverState == 5)
                *Engine->LevelComplete = 19;
            else
                *Engine->LevelComplete = 11;
        }
    }
}

void InitLevel(void)
{
    int i;

    GameOverState = 0;
    Engine->PrintF("QMULTI.C : LEVEL IS %s\n", Engine->ThisLevelName, Engine->PrintIdentityName(*Engine->ThisLevelName));
    Engine->PrintF("MaxWindows %d\n", *Engine->MaxWindows);
    for (i = 0; i < *Engine->MaxWindows; i++)
        Engine->InitMultiQuakePlayer(i);
    SpawnPickups();
    *Engine->LevelSpecific = LevelSpecific;
    /* An empty loop (presumably a compiled-out one in the original): its labels end gcc's CSE
       block here, so the store above addresses LevelInterface.Engine directly and the code
       below loads &LevelInterface into a register of its own. */
    for (i = 0; i < 0; i++)
        ;
    *Engine->InSelfPlayMode = 0;
    if (*Engine->InSelfPlayMode) {
        if (*Engine->MaxWindows == 2) {
            Engine->SelfPlayOn(0, SelfPlay0, Engine->QuakeEndSelfPlay);
            Engine->SelfPlayOn(1, SelfPlay1, Engine->QuakeEndSelfPlay);
            *Engine->NoInputTime = 30;
            Engine->SetNoInputTimeOutRoutine(Engine->QuakeNextDemoLevel);
        } else if (*Engine->MaxWindows == 4) {
            Engine->SelfPlayOn(0, SelfPlay2, Engine->QuakeEndSelfPlay);
            Engine->SelfPlayOn(1, SelfPlay3, Engine->QuakeEndSelfPlay);
            Engine->SelfPlayOn(2, SelfPlay4, Engine->QuakeEndSelfPlay);
            Engine->SelfPlayOn(3, SelfPlay5, Engine->QuakeEndSelfPlay);
            *Engine->NoInputTime = 30;
            Engine->SetNoInputTimeOutRoutine(Engine->QuakeNextDemoLevel);
        }
    }
}
