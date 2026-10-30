/* Supertank (m_supertank.c) */
#include "ovl.h"

/* ai_global_table indices (gmonster.c) */
enum { AI_NONE, AI_STAND, AI_WALK, AI_RUN, AI_CHARGE, AI_MOVE };
/* indices into this module's local ai table */
enum { L_NONE, L_UNUSED, L_DEAD, L_ROCKET, L_MACHINEGUN, L_REATTACK1, L_TREAD, L_RUN, L_EXPLODE };

#define CLASS_SUPERTANK 90

/* gcc emits the small functions last, in this order */
void PreInit_Supertank(struct GLOBMODHDR *Hdr);
void supertankRocket(struct edict_s *self);
void supertankMachineGun(struct edict_s *self);
static void supertank_dead(struct edict_s *self);
static void supertank_reattack1(struct edict_s *self);
void TreadSound(struct edict_s *self);
static void supertank_run(struct edict_s *self);
void BossExplode(struct edict_s *self);
void SP_monster_supertank(struct edict_s *self);
void Supertank_EveryFrame(struct CREATURE *Cre);
static void supertank_search(struct edict_s *self);
static void supertank_stand(struct edict_s *self);
void supertank_forward(struct edict_s *self);
static void supertank_walk(struct edict_s *self);
static void supertank_pain(struct edict_s *self, struct edict_s *other, long kick, int damage);
static void supertank_attack(struct edict_s *self);
static void supertank_die(struct edict_s *self, struct edict_s *inflictor, struct edict_s *attacker, int damage);

static struct NEWANIMREMAP AnimRemap[];

struct QCREINTERFACE Iface OVL_HEADER = { SP_monster_supertank, Supertank_EveryFrame, PreInit_Supertank, AnimRemap };

#ifndef NONMATCHING
/* NONMATCHING: PreInit_Supertank (the C version is below, under #ifdef NONMATCHING).
   Everything but one call matches: the original passes sound_search2, which nothing ever
   sets, as a stray second argument to GetSoundNum("bli_engine"), and loads it into $a3 last,
   after the IDENTITYNAME bytes (whose base pointer it gives $a3). Our cc1psx's first
   scheduling pass hoists that load to the top of the block, so the base goes to $t0 and two
   mult temporaries in the muzzle loop swap registers. Tried: int/long/unsigned/volatile and
   4-byte struct (SImode and BLKmode) types for sound_search2, an array for the two search
   sounds, a local copy, casts, a prototyped call, *(T *)& and other spellings of the argument,
   a do/while(0) around the call, the IDENTITYNAME as *(struct IDENTITYNAME *)"..."; only
   passing the value twice (an extra stack argument) keeps the load late.
   Why (gcc 2.8.1 sched.c): the load `$a3 = sound_search2` has priority 6 (its only
   predecessors are the previous call and the sound_search1 store) while the byte-assembly
   insns are 7 (lbu latency 2), and it is never boosted by adjust_priority because
   birthing_insn_p needs REG_N_SETS == 1 and $a3 is also set by the three
   GetModSubOffFrom2Verteces calls. A 7 needs a load feeding it, i.e. a pseudo copy into $a3
   that combine leaves alone: a local copy of a volatile sound_search2, or a copy loaded
   before the sound_search1 store (combine won't move a load past a store), does keep the
   load last (local-alloc ties the copy to $a3), but the extra boosted load then reorders the
   IDENTITYNAME bytes (64 lines). With an instrumented cc1 the C below matches exactly when
   two things are forced together: the $a3 load's sched1 priority is exactly 7 (not
   boosted to the maximum: that swaps it with the GetSoundNum pointer load), and local-alloc
   puts the sound_search2 lui pseudo in $8 instead of $2 (sched2 then hoists the lui to just
   after the last use of $8, where the original has it). Either alone leaves 2-4 lines. No
   spelling found gives the load a load-latency predecessor (the only way to a 7 in this
   block: anti/output deps cost 1 on MIPS) without an extra insn. No other module passes a
   stray argument to copy the shape from.
   This block also holds PreInit's string literals, in their .rdata order. It sits right after
   Iface, where gcc is in a named section and so re-emits a section directive before its next
   output. */
__asm__(
    " .rdata\n"
    "R_130:\n"
    " .ascii \"Pre Init Boss1(Supertank)\\012\\000\\000\"\n"
    "R_14c:\n"
    " .ascii \"Invalid Creature Interface size for %s\\012\\000\"\n"
    "R_174:\n"
    " .ascii \"Supertank\\000\\000\\000\"\n"
    "R_180:\n"
    " .ascii \"Invalid Creature Interface version for %s\\012\\000\\000\"\n"
    "R_1ac:\n"
    " .ascii \"Validated Creature Interface for %s\\012\\000\\000\\000\\000\"\n"
    "R_1d4:\n"
    " .ascii \"btk_pain1\\000\\000\\000\"\n"
    "R_1e0:\n"
    " .ascii \"btk_deth1\\000\\000\\000\"\n"
    "R_1ec:\n"
    " .ascii \"btk_unqv1\\000\\000\\000\"\n"
    "R_1f8:\n"
    " .ascii \"bli_engine\\000\\000\"\n"
    "R_204:\n"
    " .ascii \"SuperTank ChainGun muzzle points are from different submodels\\015\\012\\000\"\n"
    "R_244:\n"
    " .ascii \"Boss2 Rocket muzzle points are from different submodels\\015\\012\\000\\000\\000\"\n"
    "R_280:\n"
    " .ascii \"Clear Boss1(Supertank)\\012\\000\"\n"
    " .text\n"
    " .globl PreInit_Supertank\n"
    "PreInit_Supertank:\n"
    " .set noreorder\n"
    " .set nomacro\n"
    " addiu $29,$29,-56\n"
    " sw $18,48($29)\n"
    " move $18,$4\n"
    " sw $31,52($29)\n"
    " sw $17,44($29)\n"
    " beqz $18,L_PreInit_Supertank_874\n"
    " sw $16,40($29)\n"
    " lui $2,%hi(Iface)\n"
    " addiu $16,$2,%lo(Iface)\n"
    " lui $4,%hi(R_130)\n"
    " lw $2,24($16)\n"
    " nop\n"
    " jal $31,$2\n"
    " addiu $4,$4,%lo(R_130)\n"
    " lhu $3,16($16)\n"
    " li $2,304\n"
    " beq $3,$2,L_PreInit_Supertank_318\n"
    " lui $17,%hi(R_14c)\n"
    " move $18,$16\n"
    " lui $16,%hi(R_174)\n"
    " addiu $4,$17,%lo(R_14c)\n"
    "L_PreInit_Supertank_300:\n"
    " lw $2,24($18)\n"
    " nop\n"
    " jal $31,$2\n"
    " addiu $5,$16,%lo(R_174)\n"
    " j L_PreInit_Supertank_300\n"
    " addiu $4,$17,%lo(R_14c)\n"
    "L_PreInit_Supertank_318:\n"
    " lui $2,%hi(Iface)\n"
    " addiu $4,$2,%lo(Iface)\n"
    " lh $3,18($4)\n"
    " li $2,1\n"
    " beq $3,$2,L_PreInit_Supertank_354\n"
    " lui $17,%hi(R_180)\n"
    " move $18,$4\n"
    " lui $16,%hi(R_174)\n"
    " addiu $4,$17,%lo(R_180)\n"
    "L_PreInit_Supertank_33c:\n"
    " lw $2,24($18)\n"
    " nop\n"
    " jal $31,$2\n"
    " addiu $5,$16,%lo(R_174)\n"
    " j L_PreInit_Supertank_33c\n"
    " addiu $4,$17,%lo(R_180)\n"
    "L_PreInit_Supertank_354:\n"
    " lui $4,%hi(R_1ac)\n"
    " addiu $4,$4,%lo(R_1ac)\n"
    " lui $5,%hi(R_174)\n"
    " addiu $5,$5,%lo(R_174)\n"
    " lui $2,%hi(Iface)\n"
    " addiu $17,$2,%lo(Iface)\n"
    " lw $2,24($17)\n"
    " nop\n"
    " jal $31,$2\n"
    " lui $16,0xff\n"
    " li $4,90\n"
    " lui $3,%hi(AiTable)\n"
    " addiu $5,$3,%lo(AiTable)\n"
    " lui $2,%hi(ShadowVerts)\n"
    " addiu $2,$2,%lo(ShadowVerts)\n"
    " sw $2,4($18)\n"
    " lui $2,%hi(supertank_dead)\n"
    " addiu $2,$2,%lo(supertank_dead)\n"
    " sw $0,%lo(AiTable)($3)\n"
    " sw $2,8($5)\n"
    " lui $2,%hi(supertankRocket)\n"
    " addiu $2,$2,%lo(supertankRocket)\n"
    " sw $2,12($5)\n"
    " lui $2,%hi(supertankMachineGun)\n"
    " addiu $2,$2,%lo(supertankMachineGun)\n"
    " sw $2,16($5)\n"
    " lui $2,%hi(supertank_reattack1)\n"
    " addiu $2,$2,%lo(supertank_reattack1)\n"
    " sw $2,20($5)\n"
    " lui $2,%hi(TreadSound)\n"
    " addiu $2,$2,%lo(TreadSound)\n"
    " sw $2,24($5)\n"
    " lui $2,%hi(supertank_run)\n"
    " addiu $2,$2,%lo(supertank_run)\n"
    " sw $2,28($5)\n"
    " lui $2,%hi(BossExplode)\n"
    " lw $3,280($17)\n"
    " addiu $2,$2,%lo(BossExplode)\n"
    " jal $31,$3\n"
    " sw $2,32($5)\n"
    " lui $2,%hi(R_1d4)\n"
    " addiu $7,$2,%lo(R_1d4)\n"
    " ori $16,$16,0xffff\n"
    " lbu $3,1($7)\n"
    " lbu $6,%lo(R_1d4)($2)\n"
    " lbu $2,2($7)\n"
    " lbu $4,4($7)\n"
    " lbu $5,7($7)\n"
    " sll $3,$3,0x8\n"
    " or $6,$6,$3\n"
    " sll $2,$2,0x10\n"
    " or $6,$6,$2\n"
    " sll $5,$5,0x18\n"
    " lbu $3,5($7)\n"
    " lbu $2,6($7)\n"
    " sll $3,$3,0x8\n"
    " or $8,$4,$3\n"
    " sll $2,$2,0x10\n"
    " or $8,$8,$2\n"
    " and $8,$8,$16\n"
    " or $5,$8,$5\n"
    " lbu $3,9($7)\n"
    " lbu $4,8($7)\n"
    " lbu $2,10($7)\n"
    " sll $3,$3,0x8\n"
    " or $3,$4,$3\n"
    " sll $2,$2,0x10\n"
    " or $3,$3,$2\n"
    " lbu $4,3($7)\n"
    " and $3,$3,$16\n"
    " sll $4,$4,0x18\n"
    " or $4,$6,$4\n"
    " lbu $6,11($7)\n"
    " lw $2,36($17)\n"
    " sll $6,$6,0x18\n"
    " jal $31,$2\n"
    " or $6,$3,$6\n"
    " lui $5,%hi(R_1e0)\n"
    " addiu $6,$5,%lo(R_1e0)\n"
    " lbu $4,1($6)\n"
    " lui $3,%hi(sound_pain1)\n"
    " sw $2,%lo(sound_pain1)($3)\n"
    " lui $3,%hi(sound_pain2)\n"
    " sw $2,%lo(sound_pain2)($3)\n"
    " lui $3,%hi(sound_pain3)\n"
    " sw $2,%lo(sound_pain3)($3)\n"
    " lbu $3,%lo(R_1e0)($5)\n"
    " lbu $2,2($6)\n"
    " lbu $5,7($6)\n"
    " sll $4,$4,0x8\n"
    " or $8,$3,$4\n"
    " sll $2,$2,0x10\n"
    " or $8,$8,$2\n"
    " and $8,$8,$16\n"
    " sll $5,$5,0x18\n"
    " lbu $3,5($6)\n"
    " lbu $4,4($6)\n"
    " lbu $2,6($6)\n"
    " sll $3,$3,0x8\n"
    " or $7,$4,$3\n"
    " sll $2,$2,0x10\n"
    " or $7,$7,$2\n"
    " and $7,$7,$16\n"
    " or $5,$7,$5\n"
    " lbu $3,9($6)\n"
    " lbu $4,8($6)\n"
    " lbu $2,10($6)\n"
    " sll $3,$3,0x8\n"
    " or $3,$4,$3\n"
    " sll $2,$2,0x10\n"
    " or $3,$3,$2\n"
    " and $3,$3,$16\n"
    " lbu $4,3($6)\n"
    " lbu $6,11($6)\n"
    " lw $2,36($17)\n"
    " sll $4,$4,0x18\n"
    " or $4,$8,$4\n"
    " sll $6,$6,0x18\n"
    " jal $31,$2\n"
    " or $6,$3,$6\n"
    " lui $5,%hi(R_1ec)\n"
    " addiu $6,$5,%lo(R_1ec)\n"
    " lbu $4,1($6)\n"
    " lui $3,%hi(sound_death)\n"
    " sw $2,%lo(sound_death)($3)\n"
    " lbu $3,%lo(R_1ec)($5)\n"
    " lbu $2,2($6)\n"
    " lbu $5,7($6)\n"
    " sll $4,$4,0x8\n"
    " or $8,$3,$4\n"
    " sll $2,$2,0x10\n"
    " or $8,$8,$2\n"
    " and $8,$8,$16\n"
    " sll $5,$5,0x18\n"
    " lbu $3,5($6)\n"
    " lbu $4,4($6)\n"
    " lbu $2,6($6)\n"
    " sll $3,$3,0x8\n"
    " or $7,$4,$3\n"
    " sll $2,$2,0x10\n"
    " or $7,$7,$2\n"
    " and $7,$7,$16\n"
    " or $5,$7,$5\n"
    " lbu $3,9($6)\n"
    " lbu $4,8($6)\n"
    " lbu $2,10($6)\n"
    " sll $3,$3,0x8\n"
    " or $3,$4,$3\n"
    " sll $2,$2,0x10\n"
    " or $3,$3,$2\n"
    " and $3,$3,$16\n"
    " lbu $4,3($6)\n"
    " lbu $6,11($6)\n"
    " lw $2,36($17)\n"
    " sll $4,$4,0x18\n"
    " or $4,$8,$4\n"
    " sll $6,$6,0x18\n"
    " jal $31,$2\n"
    " or $6,$3,$6\n"
    " lui $5,%hi(R_1f8)\n"
    " addiu $7,$5,%lo(R_1f8)\n"
    " lbu $4,1($7)\n"
    " lui $3,%hi(sound_search1)\n"
    " sw $2,%lo(sound_search1)($3)\n"
    " lbu $3,%lo(R_1f8)($5)\n"
    " lbu $2,2($7)\n"
    " lbu $6,8($7)\n"
    " lbu $5,7($7)\n"
    " sll $4,$4,0x8\n"
    " or $9,$3,$4\n"
    " sll $2,$2,0x10\n"
    " or $9,$9,$2\n"
    " and $9,$9,$16\n"
    " sll $5,$5,0x18\n"
    " lbu $3,5($7)\n"
    " lbu $4,4($7)\n"
    " lbu $2,6($7)\n"
    " sll $3,$3,0x8\n"
    " or $8,$4,$3\n"
    " sll $2,$2,0x10\n"
    " or $8,$8,$2\n"
    " and $8,$8,$16\n"
    " or $5,$8,$5\n"
    " lui $8,%hi(sound_search2)\n"
    " lbu $3,9($7)\n"
    " lbu $2,10($7)\n"
    " lbu $4,3($7)\n"
    " sll $3,$3,0x8\n"
    " or $6,$6,$3\n"
    " sll $2,$2,0x10\n"
    " or $6,$6,$2\n"
    " and $6,$6,$16\n"
    " sll $4,$4,0x18\n"
    " or $4,$9,$4\n"
    " lbu $3,11($7)\n"
    " lw $7,%lo(sound_search2)($8)\n"
    " lw $2,36($17)\n"
    " sll $3,$3,0x18\n"
    " jal $31,$2\n"
    " or $6,$6,$3\n"
    " move $4,$18\n"
    " li $5,412\n"
    " li $6,419\n"
    " lui $7,%hi(ChainGunMuzzle)\n"
    " addiu $7,$7,%lo(ChainGunMuzzle)\n"
    " lw $8,52($17)\n"
    " lui $3,%hi(tread_sound)\n"
    " jal $31,$8\n"
    " sw $2,%lo(tread_sound)($3)\n"
    " bnez $2,L_PreInit_Supertank_6b0\n"
    " lui $4,%hi(R_204)\n"
    " lw $2,24($17)\n"
    " nop\n"
    " jal $31,$2\n"
    " addiu $4,$4,%lo(R_204)\n"
    "L_PreInit_Supertank_6b0:\n"
    " move $4,$18\n"
    " li $5,564\n"
    " li $6,571\n"
    " lw $2,52($17)\n"
    " nop\n"
    " jal $31,$2\n"
    " addiu $7,$29,16\n"
    " beqz $2,L_PreInit_Supertank_6f4\n"
    " li $5,570\n"
    " move $4,$18\n"
    " li $6,572\n"
    " lw $2,52($17)\n"
    " nop\n"
    " jal $31,$2\n"
    " addiu $7,$29,24\n"
    " bnez $2,L_PreInit_Supertank_70c\n"
    " lui $6,0x5555\n"
    "L_PreInit_Supertank_6f4:\n"
    " lui $4,%hi(R_244)\n"
    " lw $2,24($17)\n"
    " nop\n"
    " jal $31,$2\n"
    " addiu $4,$4,%lo(R_244)\n"
    " lui $6,0x5555\n"
    "L_PreInit_Supertank_70c:\n"
    " lh $5,24($29)\n"
    " lh $2,16($29)\n"
    " ori $6,$6,0x5556\n"
    " subu $5,$5,$2\n"
    " sll $5,$5,0x5\n"
    " mult $5,$6\n"
    " lh $4,26($29)\n"
    " lh $2,18($29)\n"
    " mfhi $9\n"
    " subu $4,$4,$2\n"
    " sll $4,$4,0x5\n"
    " mult $4,$6\n"
    " lh $3,20($29)\n"
    " lh $2,28($29)\n"
    " mfhi $8\n"
    " subu $2,$2,$3\n"
    " sll $2,$2,0x5\n"
    " mult $2,$6\n"
    " move $7,$0\n"
    " sra $5,$5,0x1f\n"
    " lui $3,%hi(RocketMuzzle)\n"
    " subu $5,$9,$5\n"
    " sra $4,$4,0x1f\n"
    " sh $5,32($29)\n"
    " addiu $6,$3,%lo(RocketMuzzle)\n"
    " subu $4,$8,$4\n"
    " sra $2,$2,0x1f\n"
    " sh $4,34($29)\n"
    " mfhi $11\n"
    " subu $2,$11,$2\n"
    " sh $2,36($29)\n"
    "L_PreInit_Supertank_788:\n"
    " lhu $2,32($29)\n"
    " nop\n"
    " sll $2,$2,0x10\n"
    " sra $3,$2,0x10\n"
    " mult $7,$3\n"
    " srl $2,$2,0x1f\n"
    " addu $3,$3,$2\n"
    " sra $3,$3,0x1\n"
    " mflo $10\n"
    " addu $3,$3,$10\n"
    " bgez $3,L_PreInit_Supertank_7bc\n"
    " nop\n"
    " addiu $3,$3,31\n"
    "L_PreInit_Supertank_7bc:\n"
    " lhu $2,16($29)\n"
    " sra $3,$3,0x5\n"
    " addu $2,$2,$3\n"
    " sh $2,0($6)\n"
    " lhu $2,34($29)\n"
    " nop\n"
    " sll $2,$2,0x10\n"
    " sra $3,$2,0x10\n"
    " mult $7,$3\n"
    " srl $2,$2,0x1f\n"
    " addu $3,$3,$2\n"
    " sra $3,$3,0x1\n"
    " mflo $10\n"
    " addu $3,$3,$10\n"
    " bgez $3,L_PreInit_Supertank_800\n"
    " nop\n"
    " addiu $3,$3,31\n"
    "L_PreInit_Supertank_800:\n"
    " lhu $2,18($29)\n"
    " sra $3,$3,0x5\n"
    " addu $2,$2,$3\n"
    " sh $2,2($6)\n"
    " lhu $2,36($29)\n"
    " nop\n"
    " sll $2,$2,0x10\n"
    " sra $3,$2,0x10\n"
    " mult $7,$3\n"
    " srl $2,$2,0x1f\n"
    " addu $3,$3,$2\n"
    " sra $3,$3,0x1\n"
    " mflo $10\n"
    " addu $3,$3,$10\n"
    " bgez $3,L_PreInit_Supertank_844\n"
    " nop\n"
    " addiu $3,$3,31\n"
    "L_PreInit_Supertank_844:\n"
    " lhu $2,20($29)\n"
    " sra $3,$3,0x5\n"
    " addu $2,$2,$3\n"
    " sh $2,4($6)\n"
    " lhu $2,22($29)\n"
    " addiu $7,$7,1\n"
    " sh $2,6($6)\n"
    " slti $2,$7,3\n"
    " bnez $2,L_PreInit_Supertank_788\n"
    " addiu $6,$6,8\n"
    " j L_PreInit_Supertank_88c\n"
    " nop\n"
    "L_PreInit_Supertank_874:\n"
    " lui $2,%hi(Iface+24)\n"
    " lui $4,%hi(R_280)\n"
    " lw $2,%lo(Iface+24)($2)\n"
    " nop\n"
    " jal $31,$2\n"
    " addiu $4,$4,%lo(R_280)\n"
    "L_PreInit_Supertank_88c:\n"
    " lw $31,52($29)\n"
    " lw $18,48($29)\n"
    " lw $17,44($29)\n"
    " lw $16,40($29)\n"
    " jr $31\n"
    " addiu $29,$29,56\n"
    " .set macro\n"
    " .set reorder\n");
#endif

static struct NEWANIMREMAP AnimRemap[] = {
    { 194, 253, {"Stand"}, 0, 0 },
    { 0, 5, {"Start Attack"}, 0, 0 },
    { 6, 19, {"Attack a"}, 0, 0 },
    { 20, 46, {"Attack b"}, 0, 0 },
    { 47, 73, {"Attack c"}, 0, 0 },
    { 74, 79, {"Attack d"}, 0, 0 },
    { 300, 333, {"Death"}, 0, 0 },
    { 128, 145, {"Forward"}, 0, 0 },
    { 146, 163, {"Left"}, 0, 0 },
    { 164, 167, {"Pain 1"}, 0, 0 },
    { 168, 171, {"Pain 2"}, 0, 0 },
    { 172, 175, {"Pain 3"}, 0, 0 },
    { 176, 193, {"Right"}, 0, 0 },
    { -1, -1, {"\377"}, 0, 0 },
};

static short ShadowVerts[] = { -1, 0 };

static int sound_pain1;
static int sound_pain2;
static int sound_pain3;
static int sound_death;
static int sound_search1;
static int sound_search2;
static int tread_sound;
static unsigned char ExplodeCount;
static struct MODSUBOFF ChainGunMuzzle;
static int Unused[2]; /* space the original has that no code uses */
static struct MODSUBOFF RocketMuzzle[3];
static int Unused2[2];
static struct ai_local_table_t AiTable[32];
static int Unused3[2];

#ifdef NONMATCHING
void PreInit_Supertank(struct GLOBMODHDR *Hdr)
{
    struct MODSUBOFF First, Last;
    struct SVECT Step;
    int i;

    if (Hdr) {
        Iface.PrintF("Pre Init Boss1(Supertank)\n");
        if (Iface.StructSize != sizeof(struct QCREINTERFACE))
            for (;;)
                Iface.PrintF("Invalid Creature Interface size for %s\n", "Supertank");
        if (Iface.Version != 1)
            for (;;)
                Iface.PrintF("Invalid Creature Interface version for %s\n", "Supertank");
        Iface.PrintF("Validated Creature Interface for %s\n", "Supertank");
        Hdr->ShadowVerts = ShadowVerts;
        AiTable[L_NONE].thinkfunc = 0;
        AiTable[L_DEAD].thinkfunc = supertank_dead;
        AiTable[L_ROCKET].thinkfunc = supertankRocket;
        AiTable[L_MACHINEGUN].thinkfunc = supertankMachineGun;
        AiTable[L_REATTACK1].thinkfunc = supertank_reattack1;
        AiTable[L_TREAD].thinkfunc = TreadSound;
        AiTable[L_RUN].thinkfunc = supertank_run;
        AiTable[L_EXPLODE].thinkfunc = BossExplode;
        Iface.SetMyAiTable(CLASS_SUPERTANK, AiTable);
        sound_pain3 = sound_pain2 = sound_pain1 = Iface.GetSoundNum((struct IDENTITYNAME){"btk_pain1"});
        sound_death = Iface.GetSoundNum((struct IDENTITYNAME){"btk_deth1"});
        sound_search1 = Iface.GetSoundNum((struct IDENTITYNAME){"btk_unqv1"});
        /* sound_search2 is never set; the original passes it as a stray extra argument */
        tread_sound = Iface.GetSoundNum((struct IDENTITYNAME){"bli_engine"}, sound_search2);
        if (!Iface.GetModSubOffFrom2Verteces(Hdr, 412, 419, &ChainGunMuzzle))
            Iface.PrintF("SuperTank ChainGun muzzle points are from different submodels\r\n");
        if (!Iface.GetModSubOffFrom2Verteces(Hdr, 564, 571, &First) || !Iface.GetModSubOffFrom2Verteces(Hdr, 570, 572, &Last))
            Iface.PrintF("Boss2 Rocket muzzle points are from different submodels\r\n");
        Step.vx = (Last.Offset.vx - First.Offset.vx) * 32 / 3;
        Step.vy = (Last.Offset.vy - First.Offset.vy) * 32 / 3;
        Step.vz = (Last.Offset.vz - First.Offset.vz) * 32 / 3;
        for (i = 0; i < 3; i++) {
            RocketMuzzle[i].Offset.vx = First.Offset.vx + (Step.vx / 2 + i * Step.vx) / 32;
            RocketMuzzle[i].Offset.vy = First.Offset.vy + (Step.vy / 2 + i * Step.vy) / 32;
            RocketMuzzle[i].Offset.vz = First.Offset.vz + (Step.vz / 2 + i * Step.vz) / 32;
            RocketMuzzle[i].SubModel = First.SubModel;
        }
    } else
        Iface.PrintF("Clear Boss1(Supertank)\n");
}
#endif

static struct mframe_t supertank_frames_stand[] = {
    { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE },
    { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE },
    { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE },
    { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE },
    { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE },
    { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE },
    { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE },
    { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE },
    { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE },
    { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE },
    { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE },
    { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE },
    { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE },
    { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE },
    { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE },
};
static struct mmove_t supertank_move_stand = { 194, 253, supertank_frames_stand, 0 };

static struct mframe_t supertank_frames_run[] = {
    { AI_RUN, 12, L_TREAD }, { AI_RUN, 12, L_NONE }, { AI_RUN, 12, L_NONE }, { AI_RUN, 12, L_NONE },
    { AI_RUN, 12, L_NONE }, { AI_RUN, 12, L_NONE }, { AI_RUN, 12, L_NONE }, { AI_RUN, 12, L_NONE },
    { AI_RUN, 12, L_NONE }, { AI_RUN, 12, L_NONE }, { AI_RUN, 12, L_NONE }, { AI_RUN, 12, L_NONE },
    { AI_RUN, 12, L_NONE }, { AI_RUN, 12, L_NONE }, { AI_RUN, 12, L_NONE }, { AI_RUN, 12, L_NONE },
    { AI_RUN, 12, L_NONE }, { AI_RUN, 12, L_NONE },
};
static struct mmove_t supertank_move_run = { 128, 145, supertank_frames_run, 0 };

static struct mframe_t supertank_frames_forward[] = {
    { AI_WALK, 4, L_TREAD }, { AI_WALK, 4, L_NONE }, { AI_WALK, 4, L_NONE }, { AI_WALK, 4, L_NONE },
    { AI_WALK, 4, L_NONE }, { AI_WALK, 4, L_NONE }, { AI_WALK, 4, L_NONE }, { AI_WALK, 4, L_NONE },
    { AI_WALK, 4, L_NONE }, { AI_WALK, 4, L_NONE }, { AI_WALK, 4, L_NONE }, { AI_WALK, 4, L_NONE },
    { AI_WALK, 4, L_NONE }, { AI_WALK, 4, L_NONE }, { AI_WALK, 4, L_NONE }, { AI_WALK, 4, L_NONE },
    { AI_WALK, 4, L_NONE }, { AI_WALK, 4, L_NONE },
};
static struct mmove_t supertank_move_forward = { 128, 145, supertank_frames_forward, 0 };

static struct mframe_t supertank_frames_turn_right[] = {
    { AI_MOVE, 0, L_TREAD }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
};
static struct mmove_t supertank_move_turn_right = { 176, 193, supertank_frames_turn_right, supertank_run };

static struct mframe_t supertank_frames_turn_left[] = {
    { AI_MOVE, 0, L_TREAD }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
};
static struct mmove_t supertank_move_turn_left = { 146, 163, supertank_frames_turn_left, supertank_run };

static struct mframe_t supertank_frames_pain3[] = {
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
};
static struct mmove_t supertank_move_pain3 = { 172, 175, supertank_frames_pain3, supertank_run };

static struct mframe_t supertank_frames_pain2[] = {
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
};
static struct mmove_t supertank_move_pain2 = { 168, 171, supertank_frames_pain2, supertank_run };

static struct mframe_t supertank_frames_pain1[] = {
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
};
static struct mmove_t supertank_move_pain1 = { 164, 167, supertank_frames_pain1, supertank_run };

static struct mframe_t supertank_frames_death[] = {
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_EXPLODE }, { AI_MOVE, 0, L_EXPLODE },
    { AI_MOVE, 0, L_EXPLODE }, { AI_MOVE, 0, L_EXPLODE }, { AI_MOVE, 0, L_EXPLODE }, { AI_MOVE, 0, L_EXPLODE },
    { AI_MOVE, 0, L_EXPLODE }, { AI_MOVE, 0, L_EXPLODE }, { AI_MOVE, 0, L_EXPLODE }, { AI_MOVE, 0, L_EXPLODE },
    { AI_MOVE, 0, L_EXPLODE }, { AI_MOVE, 0, L_EXPLODE },
};
static struct mmove_t supertank_move_death = { 300, 333, supertank_frames_death, supertank_dead };

static struct mframe_t supertank_frames_backward[] = {
    { AI_WALK, 0, L_TREAD }, { AI_WALK, 0, L_NONE }, { AI_WALK, 0, L_NONE }, { AI_WALK, 0, L_NONE },
    { AI_WALK, 0, L_NONE }, { AI_WALK, 0, L_NONE }, { AI_WALK, 0, L_NONE }, { AI_WALK, 0, L_NONE },
    { AI_WALK, 0, L_NONE }, { AI_WALK, 0, L_NONE }, { AI_WALK, 0, L_NONE }, { AI_WALK, 0, L_NONE },
    { AI_WALK, 0, L_NONE }, { AI_WALK, 0, L_NONE }, { AI_WALK, 0, L_NONE }, { AI_WALK, 0, L_NONE },
    { AI_WALK, 0, L_NONE }, { AI_WALK, 0, L_NONE },
};
static struct mmove_t supertank_move_backward = { 80, 97, supertank_frames_backward, 0 };

static struct mframe_t supertank_frames_attack4[] = {
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
};
static struct mmove_t supertank_move_attack4 = { 74, 79, supertank_frames_attack4, supertank_run };

static struct mframe_t supertank_frames_attack3[] = {
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
};
static struct mmove_t supertank_move_attack3 = { 47, 73, supertank_frames_attack3, supertank_run };

static struct mframe_t supertank_frames_attack2[] = {
    { AI_CHARGE, 0, L_NONE }, { AI_CHARGE, 0, L_NONE }, { AI_CHARGE, 0, L_NONE }, { AI_CHARGE, 0, L_NONE },
    { AI_CHARGE, 0, L_NONE }, { AI_CHARGE, 0, L_NONE }, { AI_CHARGE, 0, L_NONE }, { AI_CHARGE, 0, L_ROCKET },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_ROCKET }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_ROCKET }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
};
static struct mmove_t supertank_move_attack2 = { 20, 46, supertank_frames_attack2, supertank_run };

static struct mframe_t supertank_frames_attack1[] = {
    { AI_CHARGE, 0, L_MACHINEGUN }, { AI_CHARGE, 0, L_MACHINEGUN }, { AI_CHARGE, 0, L_MACHINEGUN }, { AI_CHARGE, 0, L_MACHINEGUN },
    { AI_CHARGE, 0, L_MACHINEGUN }, { AI_CHARGE, 0, L_MACHINEGUN },
};
static struct mmove_t supertank_move_attack1 = { 0, 5, supertank_frames_attack1, supertank_reattack1 };

static struct mframe_t supertank_frames_end_attack1[] = {
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
};
static struct mmove_t supertank_move_end_attack1 = { 6, 19, supertank_frames_end_attack1, supertank_run };

void supertankRocket(struct edict_s *self)
{
    struct CREATURE *Cre = self->Extra.Creature;
    struct VECT start, dir, vec;
    struct SVECT Pos;
    int n;

    if (self->s.frame == 27)
        n = 0;
    else if (self->s.frame == 30)
        n = 1;
    else
        n = 2;
    Pos = Iface.GetSubModXYZ(Cre->ModelHdr, RocketMuzzle[n], Cre->AnimFrame, Cre->Draw.AnimJump ? Cre->Draw.TargetAnimFrame : -1);
    Iface.RotateS(&Cre->Matrix, &Pos, &Pos);
    start.vx = Cre->Draw.Location.vx + Pos.vx;
    start.vy = Cre->Draw.Location.vy + Pos.vy;
    start.vz = Cre->Draw.Location.vz + Pos.vz;
    vec.vx = self->enemy->sorigin.vx;
    vec.vy = self->enemy->sorigin.vy;
    vec.vz = self->enemy->sorigin.vz;
    vec.vy += self->enemy->viewheight;
    dir.vx = vec.vx - start.vx;
    dir.vy = vec.vy - start.vy;
    dir.vz = vec.vz - start.vz;
    Iface.VectorNormalize(&dir);
    Iface.monster_fire_rocket(self, &start, &dir, 50, 500, 0);
}

void supertankMachineGun(struct edict_s *self)
{
    struct CREATURE *Cre = self->Extra.Creature;
    struct SVECT dir;
    struct VECT vec, start, forward;
    struct SVECT Pos;

    dir.vx = 0;
    dir.vz = self->sangles.vz;
    dir.vy = 0;
    Pos = Iface.GetSubModXYZ(Cre->ModelHdr, ChainGunMuzzle, Cre->AnimFrame, Cre->Draw.AnimJump ? Cre->Draw.TargetAnimFrame : -1);
    Iface.RotateS(&Cre->Matrix, &Pos, &Pos);
    start.vx = Cre->Draw.Location.vx + Pos.vx;
    start.vy = Cre->Draw.Location.vy + Pos.vy;
    start.vz = Cre->Draw.Location.vz + Pos.vz;
    if (self->enemy) {
        vec.vx = self->enemy->sorigin.vx;
        vec.vy = self->enemy->sorigin.vy;
        vec.vz = self->enemy->sorigin.vz;
        Iface.VectorMA(&vec, 0, &self->enemy->velocity, &vec);
        vec.vy += self->enemy->viewheight;
        forward.vx = vec.vx - start.vx;
        forward.vy = vec.vy - start.vy;
        forward.vz = vec.vz - start.vz;
        Iface.VectorNormalize(&forward);
    }
    Iface.monster_fire_bullet(self, &start, &forward, 6, 4, 300, 500, 0);
}

static void supertank_dead(struct edict_s *self)
{
    ((struct CREATURE *)self->Extra.Creature)->Health = self->gib_health;
    self->nextthink = 0;
    self->takedamage = 0;
    self->movetype = 7;
    self->svflags |= 2;
}

static void supertank_reattack1(struct edict_s *self)
{
    if (Iface.visible(self, self->enemy))
        if (Iface.rand() < 29490)
            self->monsterinfo.currentmove = &supertank_move_attack1;
        else
            self->monsterinfo.currentmove = &supertank_move_end_attack1;
    else
        self->monsterinfo.currentmove = &supertank_move_end_attack1;
}

void TreadSound(struct edict_s *self)
{
}

static void supertank_run(struct edict_s *self)
{
    if (self->monsterinfo.aiflags & 1)
        self->monsterinfo.currentmove = &supertank_move_stand;
    else
        self->monsterinfo.currentmove = &supertank_move_run;
}

void BossExplode(struct edict_s *self)
{
    struct VECT org;
    int Area;

    if (++ExplodeCount & 1)
        return;
    Area = ((struct CREATURE *)self->Extra.Creature)->Box.Area;
    org.vx = self->sorigin.vx + (Iface.rand() - 16384) / 32;
    org.vy = self->sorigin.vy + (Iface.rand() - 16384) / 64 - 60;
    org.vz = self->sorigin.vz + (Iface.rand() - 16384) / 32;
    Iface.QuakeGenExplosion(&org, Area, 4096, 0);
}

void SP_monster_supertank(struct edict_s *self)
{
    Iface.PrintF("Spawning SuperTank\n");
    self->mass = 800;
    self->pain = supertank_pain;
    self->die = supertank_die;
    self->monsterinfo.stand = supertank_stand;
    self->monsterinfo.walk = supertank_walk;
    self->monsterinfo.run = supertank_run;
    self->monsterinfo.dodge = 0;
    self->monsterinfo.attack = supertank_attack;
    self->monsterinfo.search = supertank_search;
    self->monsterinfo.melee = 0;
    self->monsterinfo.sight = 0;
    self->movetype = 5;
    self->solid = 2;
    Iface.linkentity(self, 1);
    Iface.linkentity(self, 0x81);
    self->monsterinfo.currentmove = &supertank_move_stand;
    self->monsterinfo.scale = 10;
    Iface.walkmonster_start(self);
}

void Supertank_EveryFrame(struct CREATURE *Cre)
{
    if (Cre->Control < 0)
        Iface.MuzzleFlash(Cre, ChainGunMuzzle, 240);
}

static void supertank_search(struct edict_s *self)
{
}

static void supertank_stand(struct edict_s *self)
{
    self->monsterinfo.currentmove = &supertank_move_stand;
}

void supertank_forward(struct edict_s *self)
{
    self->monsterinfo.currentmove = &supertank_move_forward;
}

static void supertank_walk(struct edict_s *self)
{
    self->monsterinfo.currentmove = &supertank_move_forward;
}

static void supertank_pain(struct edict_s *self, struct edict_s *other, long kick, int damage)
{
    if (((struct CREATURE *)self->Extra.Creature)->Health < self->max_health / 2)
        self->s.skinnum = 1;
    if (Iface.level->time < self->pain_debounce_time)
        return;
    /* lessen the chance of him going into his pain frames */
    if (damage <= 25)
        if (Iface.rand() < 6553)
            return;
    /* don't go into pain if he's firing his rockets */
    if ((*Iface.skill)->value >= 2)
        if (self->s.frame >= 20 && self->s.frame <= 33)
            return;
    self->pain_debounce_time = Iface.level->time + 30;
    if ((*Iface.skill)->value == 3)
        return;
    if (damage <= 10)
        self->monsterinfo.currentmove = &supertank_move_pain1;
    else if (damage <= 25)
        self->monsterinfo.currentmove = &supertank_move_pain2;
    else
        self->monsterinfo.currentmove = &supertank_move_pain3;
}

static void supertank_attack(struct edict_s *self)
{
    struct VECT vec;
    long range;

    vec.vx = self->enemy->sorigin.vx - self->sorigin.vx;
    vec.vy = self->enemy->sorigin.vy - self->sorigin.vy;
    vec.vz = self->enemy->sorigin.vz - self->sorigin.vz;
    range = Iface.VectorLength2(&vec);
    if (range <= 1920 * 1920)
        self->monsterinfo.currentmove = &supertank_move_attack1;
    else if (Iface.rand() < 9830)
        self->monsterinfo.currentmove = &supertank_move_attack1;
    else
        self->monsterinfo.currentmove = &supertank_move_attack2;
}

static void supertank_die(struct edict_s *self, struct edict_s *inflictor, struct edict_s *attacker, int damage)
{
    self->deadflag = 2;
    self->takedamage = 0;
    self->count = 0;
    self->monsterinfo.currentmove = &supertank_move_death;
}
