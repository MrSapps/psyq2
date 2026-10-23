# PSY-Q libsn module. The game was linked with a libsn release that is not in the
# SDKs at hand (4.3-4.5 differ in code), so this module is rebuilt from the original.
	.set	noreorder
	.set	noat
	.text

	.globl	PCread
PCread:
	addiu	$29, $29, -0x30
	sw	$22, 0x28($29)
	addu	$22, $4, $0
	sw	$19, 0x1C($29)
	addu	$19, $5, $0
	sw	$16, 0x10($29)
	addu	$16, $6, $0
	sw	$18, 0x18($29)
	addu	$18, $0, $0
	sw	$31, 0x2C($29)
	sw	$21, 0x24($29)
	sw	$20, 0x20($29)
	beqz	$16, $L8008AEDC
	sw	$17, 0x14($29)
	ori	$21, $0, 0x8000
	addiu	$20, $0, -0x1
	sltu	$2, $21, $16
$L8008AE90:
	beqz	$2, $L8008AE9C
	addu	$17, $16, $0
	ori	$17, $0, 0x8000
$L8008AE9C:
	addu	$4, $0, $0
	addu	$5, $22, $0
	addu	$6, $17, $0
	jal	_SN_read
	addu	$7, $19, $0
	bne	$2, $20, $L8008AEC0
	addu	$18, $18, $2
	j	$L8008AEE0
	addiu	$2, $0, -0x1
$L8008AEC0:
	addu	$19, $19, $2
	subu	$16, $16, $2
	slt	$2, $2, $17
	bnez	$2, $L8008AEE0
	addu	$2, $18, $0
	bnez	$16, $L8008AE90
	sltu	$2, $21, $16
$L8008AEDC:
	addu	$2, $18, $0
$L8008AEE0:
	lw	$31, 0x2C($29)
	lw	$22, 0x28($29)
	lw	$21, 0x24($29)
	lw	$20, 0x20($29)
	lw	$19, 0x1C($29)
	lw	$18, 0x18($29)
	lw	$17, 0x14($29)
	lw	$16, 0x10($29)
	addiu	$29, $29, 0x30
	jr	$31
	nop
