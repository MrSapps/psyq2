# The PSY-Q library variables (commons) that the original link placed after libcard\END.OBJ.
# psylink 2.73 would allocate these itself, in hash order, and after the first one of 8
# bytes or more it aligns every later bss piece to 8, which the original link did not.
# Defined here, in the original order, psylink uses these definitions instead.
	.section	.bss
	.globl	_waitTime
_waitTime:
	.space	4
	.globl	_startTime
_startTime:
	.space	4
