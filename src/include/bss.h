/* Uninitialised variables.
 *
 * The original link placed each object's uninitialised variables (gcc's
 * .comm and .lcomm symbols) in definition order, statics first, every one on a
 * word boundary. The PSY-Q psylink we have (2.73, and 2.82) allocates .comm
 * symbols in hash order instead, so the variables an object owns are defined
 * straight into .sbss/.bss here, which gives the original layout with the
 * original tools. Small ones go to .sbss so that ASPSX still addresses them
 * through $gp.
 *
 * Variables that a file uses but another object owns stay plain tentative
 * definitions (commons) where the original code reaches them through $gp
 * (e.g. models.c and loadlev.c with QLevIfce's variables): psylink merges the
 * common with the definition.
 */
#ifndef BSS_H
#define BSS_H

#define SBSS __attribute__((section(".sbss"), aligned(4)))
#define BSS __attribute__((section(".bss"), aligned(4)))
#define BSS8 __attribute__((section(".bss"), aligned(8)))

#endif
