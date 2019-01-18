#include <stdlib.h>

#include "sys.h"

uint32_t *sys_addr(uint32_t forth_addr) {
    return (uint32_t *) (forth_addr - sys.addr_offset + (uint32_t) sys.sys);
}

uint32_t forth_addr(uint32_t *sys_addr) {
    return ((uint32_t) sys_addr) - ((uint32_t) sys.sys) + sys.addr_offset;
}
