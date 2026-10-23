# The PSY-Q library variables (commons) that the original link placed after libmcrd\LIBMCRD.OBJ.
# psylink 2.73 would allocate these itself, in hash order, and after the first one of 8
# bytes or more it aligns every later bss piece to 8, which the original link did not.
# Defined here, in the original order, psylink uses these definitions instead.
	.section	.bss
	.globl	SpuCommonError
SpuCommonError:
	.space	4
