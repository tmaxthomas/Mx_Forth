#ifndef __SYS_H
#define __SYS_H

#include <stdint.h>
#include <stdbool.h>

//Struct containing FORTH system data
struct system_t {
    uint32_t *sys, *sys_top;
    char *idx, *tib, *pad;
    uint32_t tib_loc, idx_loc;
    uint32_t tib_len, pad_len, base;
    uint32_t *stack, *stack_0;
    uint32_t *rstack, *rstack_0;
    uint32_t *cp, *old_cp, *gloss_head, *gloss_base;
    uint32_t *inst;
    uint32_t *q_addr;
    uint32_t source_id;

    bool PAGE, COMPILE, OKAY, ABORT;
};

struct sys_util_t {
    uint32_t alloc;
};

extern struct system_t sys;
extern struct sys_util_t sys_util;

#endif
