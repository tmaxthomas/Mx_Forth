#ifndef __SYS_H
#define __SYS_H

#include <stdbool.h>

//Struct containing FORTH system data
struct System {
    uint32_t *sys;
    char *idx, *tib, *pad;
    uint32_t tib_len, pad_len, base;
    uint32_t *stack, *stack_0;
    uint32_t *rstack, *rstack_0;
    uint32_t *cp, *gloss_head, *gloss_base;
    uint32_t *inst;
    bool BYE, PAGE, COMPILE;
};

extern struct System sys;

#endif
