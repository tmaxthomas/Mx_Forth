#include <stdlib.h>

#include "sys.h"

uint32_t *sys_addr(uint32_t forth_addr) {
    uintptr_t forth_addr_ = forth_addr;
    return (uint32_t *) (forth_addr_ - sys.addr_offset + (uintptr_t) sys.sys);
}

uint32_t forth_addr(uint32_t *sys_addr) {
    return ((uintptr_t) sys_addr) - ((uintptr_t) sys.sys) + sys.addr_offset;
}
