/* Overlay modules (src/ovl/): what every module shares.
 *
 * A module starts with its interface, the struct the engine reads at offset 0
 * (QLEVELINTERFACE for a level, QCREINTERFACE for a creature). It is an ordinary
 * writable variable (the engine fills in its function pointers; const would let gcc
 * keep its fields in registers across calls, which the original does not), placed by
 * the link script (tools/ovlbuild.py) ahead of everything else through the .header section.
 */
#ifndef OVL_H
#define OVL_H
#include "types.h"

#define OVL_HEADER __attribute__((section(".header")))

/* A module's commons (uninitialised globals) were allocated by the original linker after its
 * .bss, in order of definition; PSYLINK 2.73 allocates them in hash order. Where that differs
 * (level_security), they are defined into this section, which tools/ovlbuild.py's link script
 * puts after .bss. */
#define OVL_COMMON __attribute__((section(".commons")))

#endif
