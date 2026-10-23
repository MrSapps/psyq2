# The PSY-Q library variables (commons) that the original link placed after libcd\BIOS.OBJ.
# psylink 2.73 would allocate these itself, in hash order, and after the first one of 8
# bytes or more it aligns every later bss piece to 8, which the original link did not.
# Defined here, in the original order, psylink uses these definitions instead.
	.section	.bss
	.globl	StMode
StMode:
	.space	4
	.globl	_spu_RQ
_spu_RQ:
	.space	20
	.globl	_snd_ev_flag
_snd_ev_flag:
	.space	4
	.globl	_SsMarkCallback
_SsMarkCallback:
	.space	2048
	.globl	VBLANK_MINUS
VBLANK_MINUS:
	.space	4
	.globl	_snd_openflag
_snd_openflag:
	.space	4
	.globl	_snd_seq_t_max
_snd_seq_t_max:
	.space	2
	.space	2
	.globl	_snd_seq_s_max
_snd_seq_s_max:
	.space	2
	.space	2
	.globl	SsFCALL
SsFCALL:
	.space	148
	.globl	_ss_score
_ss_score:
	.space	128
	.globl	_svm_vab_vh
_svm_vab_vh:
	.space	64
	.globl	_svm_vab_tn
_svm_vab_tn:
	.space	64
	.globl	_svm_vab_pg
_svm_vab_pg:
	.space	64
	.globl	_svm_stereo_mono
_svm_stereo_mono:
	.space	2
	.space	2
	.globl	_svm_rattr
_svm_rattr:
	.space	20
	.globl	_svm_cur
_svm_cur:
	.space	26
	.space	2
	.globl	_svm_vab_used
_svm_vab_used:
	.space	16
	.globl	_svm_damper
_svm_damper:
	.space	2
	.space	2
	.globl	_svm_vab_count
_svm_vab_count:
	.space	2
	.space	2
	.globl	_svm_vab_not_send_size
_svm_vab_not_send_size:
	.space	4
	.globl	_svm_vab_total
_svm_vab_total:
	.space	64
	.globl	_svm_vab_start
_svm_vab_start:
	.space	64
	.globl	_svm_vh
_svm_vh:
	.space	4
	.globl	_svm_tn
_svm_tn:
	.space	4
	.globl	_svm_vg
_svm_vg:
	.space	4
	.globl	_svm_pg
_svm_pg:
	.space	4
	.globl	kMaxPrograms
kMaxPrograms:
	.space	2
	.space	2
	.globl	_svm_auto_kof_mode
_svm_auto_kof_mode:
	.space	1
	.space	3
	.globl	_SsVmMaxVoice
_SsVmMaxVoice:
	.space	1
	.space	3		# the original link rounded each piece of bss up to a word: see tools/build.py
