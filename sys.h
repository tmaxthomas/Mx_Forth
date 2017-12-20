#ifndef __SYS_H
#define __SYS_H

struct System {
    uint32_t *sys;
    char *idx, *buf;
    uint32_t buf_len;
    char *swp;
    uint32_t swp_len;
    uint32_t *stack, *stack_0, *rstack, *rstack_0;
    uint32_t base;
};

extern struct System sys;

#endif
