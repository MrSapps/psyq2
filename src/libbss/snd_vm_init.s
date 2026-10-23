# The PSY-Q library variables (commons) that the original link placed after libsnd\VM_INIT.OBJ.
# psylink 2.73 would allocate these itself, in hash order, and after the first one of 8
# bytes or more it aligns every later bss piece to 8, which the original link did not.
# Defined here, in the original order, psylink uses these definitions instead.
	.section	.bss
	.globl	_svm_sreg_buf
_svm_sreg_buf:
	.space	384
	.globl	_svm_envx_ptr
_svm_envx_ptr:
	.space	4
	.globl	_svm_sreg_dirty
_svm_sreg_dirty:
	.space	24
	.globl	_svm_envx_hist
_svm_envx_hist:
	.space	64
	.globl	_svm_voice
_svm_voice:
	.space	1344
	.globl	_svm_orev1
_svm_orev1:
	.space	2
	.space	2
	.globl	_svm_orev2
_svm_orev2:
	.space	2
	.space	2
	.globl	_svm_okof1
_svm_okof1:
	.space	2
	.space	2
	.globl	_svm_okof2
_svm_okof2:
	.space	2
	.space	2
	.globl	_svm_okon1
_svm_okon1:
	.space	2
	.space	2
	.globl	_svm_okon2
_svm_okon2:
	.space	2
	.space	2
	.globl	_svm_onos1
_svm_onos1:
	.space	2
	.space	2
	.globl	_svm_onos2
_svm_onos2:
	.space	2
	.space	2
	.globl	_autopan
_autopan:
	.space	4
	.globl	_autovol
_autovol:
	.space	4
