# PSY-Q libsn module. The game was linked with a libsn release that is not in the
# SDKs at hand (4.3-4.5 differ in code), so this module is rebuilt from the original.
	.set	noreorder
	.set	noat
	.text

	.globl	__sn_cpp_structors
__sn_cpp_structors:
	addiu	$29, $29, -0x20
	sw	$16, 0x10($29)
	addu	$16, $4, $0
	sw	$17, 0x14($29)
	addu	$17, $5, $0
	slt	$2, $16, $17
	beqz	$2, $L8008FB38
	sw	$31, 0x18($29)
$L8008FB10:
	lw	$2, 0x0($16)
	nop
	beqz	$2, $L8008FB28
	nop
	jalr	$2
	nop
$L8008FB28:
	addiu	$16, $16, 0x4
	slt	$2, $16, $17
	bnez	$2, $L8008FB10
	nop
$L8008FB38:
	lw	$31, 0x18($29)
	lw	$17, 0x14($29)
	lw	$16, 0x10($29)
	jr	$31
	addiu	$29, $29, 0x20
