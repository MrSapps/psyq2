/* Overlay modules (src/ovl/): what every module shares.
 *
 * A module starts with its interface, the struct the engine reads at offset 0
 * (QLEVELINTERFACE for a level, QCREINTERFACE for a creature). It is an ordinary
 * writable variable (the engine fills in its function pointers; const would let gcc
 * keep its fields in registers across calls, which the original does not), placed by
 * tools/ovllink.py ahead of everything else through the .header section.
 */
#ifndef OVL_H
#define OVL_H
#include "types.h"

#define OVL_HEADER __attribute__((section(".header")))

#endif
