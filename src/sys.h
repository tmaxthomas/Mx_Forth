#ifndef __SYS_H_
#define __SYS_H_

#include <stdint.h>
#include <stdbool.h>

#include "dict.h"

// Struct containing FORTH system data
// All pointers contain system addresses unless otherwise noted
struct system_t {
    uint32_t *sys, *sys_top;
    char *idx, *tib, *pad;
    uint32_t tib_loc, idx_loc;
    uint32_t idx_len, tib_len, pad_len, base;
    uint32_t *stack, *stack_0;
    uint32_t *rstack, *rstack_0;
    uint32_t *cp, *old_cp;
	dict_entry *gloss_head, *gloss_base, *curr_def;
    uint32_t *inst;
    uint32_t *q_addr, q_fth_addr;
    uint32_t source_id;
    uintptr_t addr_offset;
    
    uint32_t *COMPILE;
    bool OKAY, ABORT;
};

struct sys_util_t {
    uint32_t alloc;
};

extern struct system_t sys;
extern struct sys_util_t sys_util;
extern void (* func_table[])();
extern uint32_t ft_size;

void *sys_addr(uint32_t);
uint32_t forth_addr(void *);

#endif
