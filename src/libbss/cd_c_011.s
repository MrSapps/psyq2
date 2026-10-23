# The PSY-Q library variables (commons) that the original link placed after libcd\C_011.OBJ.
# psylink 2.73 would allocate these itself, in hash order, and after the first one of 8
# bytes or more it aligns every later bss piece to 8, which the original link did not.
# Defined here, in the original order, psylink uses these definitions instead.
	.section	.bss
	.globl	StFunc1
StFunc1:
	.space	4
	.globl	StFunc2
StFunc2:
	.space	4
	.globl	StEmu_Addr
StEmu_Addr:
	.space	4
	.globl	StCdIntrFlag
StCdIntrFlag:
	.space	4
	.globl	CChannel
CChannel:
	.space	4
	.globl	StCHANNEL
StCHANNEL:
	.space	4
	.globl	Stframe_no
Stframe_no:
	.space	4
	.globl	StRgb24
StRgb24:
	.space	4
	.globl	StEndFrame
StEndFrame:
	.space	4
	.globl	StSTART_FLAG
StSTART_FLAG:
	.space	4
	.globl	StEmu_Idx
StEmu_Idx:
	.space	4
	.globl	Stsector_offset
Stsector_offset:
	.space	2
	.space	2
	.globl	StFinalSector
StFinalSector:
	.space	4
	.globl	StRingBase
StRingBase:
	.space	4
	.globl	StRingAddr
StRingAddr:
	.space	4
	.globl	StRingIdx1
StRingIdx1:
	.space	4
	.globl	StRingIdx2
StRingIdx2:
	.space	4
	.globl	StRingIdx3
StRingIdx3:
	.space	4
	.globl	StRingSize
StRingSize:
	.space	4
	.globl	StStartFrame
StStartFrame:
	.space	4
