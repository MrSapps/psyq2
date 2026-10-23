# PSY-Q libsn SNMAIN: the start-up code (__SN_ENTRY_POINT) and __main.
# The game was linked with a libsn release that is not in the SDKs at hand: 4.3-4.5
# have the same data but different code (their __main runs the constructors itself,
# this one calls __sn_cpp_structors), so this module is rebuilt from the original.
	.set	noreorder
	.set	noat

	.text
	.globl	__SN_ENTRY_POINT
__SN_ENTRY_POINT:
	# clear .sbss and .bss
	lui	$2, %hi(__sbss_org)
	addiu	$2, $2, %lo(__sbss_org)
	lui	$3, %hi(__bss_orgend)
	addiu	$3, $3, %lo(__bss_orgend)
$Lclear:
	sw	$0, 0x0($2)
	addiu	$2, $2, 0x4
	sltu	$1, $2, $3
	bnez	$1, $Lclear
	nop
	# stack at the top of RAM, heap from the end of bss up to the stack
	lui	$2, %hi(_ramsize)
	lw	$2, %lo(_ramsize)($2)
	nop
	addi	$2, $2, -0x8
	lui	$8, 0x8000
	or	$29, $2, $8
	lui	$4, %hi(__bss_orgend)
	addiu	$4, $4, %lo(__bss_orgend)
	sll	$4, $4, 3
	srl	$4, $4, 3
	lui	$3, %hi(_stacksize)
	lw	$3, %lo(_stacksize)($3)
	nop
	subu	$5, $2, $3
	subu	$5, $5, $4
	lui	$1, %hi(__heapsize)
	sw	$5, %lo(__heapsize)($1)
	or	$4, $4, $8
	lui	$1, %hi(__heapbase)
	sw	$4, %lo(__heapbase)($1)
	lui	$1, %hi(ra_save)
	sw	$31, %lo(ra_save)($1)
	lui	$28, %hi(__SN_GP_BASE)
	addiu	$28, $28, %lo(__SN_GP_BASE)
	addu	$30, $29, $0
	jal	InitHeap
	addi	$4, $4, 4
	lui	$31, %hi(ra_save)
	lw	$31, %lo(ra_save)($31)
	nop
	jal	main
	nop
	# destructors
	lui	$4, %hi(__dtors_obj)
	addiu	$4, $4, %lo(__dtors_obj)
	lui	$5, %hi(__dtors_objend)
	addiu	$5, $5, %lo(__dtors_objend)
	jal	__sn_cpp_structors
	nop
	break	1

	.globl	__main
__main:
	lui	$8, %hi(main_called)
	lw	$8, %lo(main_called)($8)
	addiu	$29, $29, -0x10
	sw	$16, 0x4($29)
	sw	$17, 0x8($29)
	sw	$31, 0xC($29)
	bnez	$8, $Ldone
	addiu	$8, $0, 0x1
	lui	$1, %hi(main_called)
	sw	$8, %lo(main_called)($1)
	# constructors
	lui	$4, %hi(__ctors_obj)
	addiu	$4, $4, %lo(__ctors_obj)
	lui	$5, %hi(__ctors_objend)
	addiu	$5, $5, %lo(__ctors_objend)
	jal	__sn_cpp_structors
	nop
$Ldone:
	lw	$31, 0xC($29)
	lw	$17, 0x8($29)
	lw	$16, 0x4($29)
	addiu	$29, $29, 0x10
	jr	$31
	nop

	.data
main_called:
	.word	0
	.globl	__heapbase
__heapbase:
	.word	0
	.globl	__heapsize
__heapsize:
	.word	0
	.globl	__text
__text:
	.word	__text_org
	.globl	__textlen
__textlen:
	.word	__text_size
	.globl	__data
__data:
	.word	__data_org
	.globl	__datalen
__datalen:
	.word	__data_size
	.globl	__bss
__bss:
	.word	__bss_org
	.globl	__bsslen
__bsslen:
	.word	__bss_size

	.section	.sbss
ra_save:
	.space	4
