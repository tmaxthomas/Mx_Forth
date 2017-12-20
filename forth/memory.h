#ifndef __MEMORY_H
#define __MEMORY_H

#include <stdint.h>

int32_t store();
int32_t store2();
int32_t c_store();
int32_t plus_store();
int32_t fetch();
int32_t fetch2();
int32_t c_fetch();
int32_t query();
int32_t cell();
int32_t cells();
int32_t cell_plus();
int32_t fill();
int32_t erase();
int32_t dump();

int32_t sp_at();
int32_t tib();
int32_t pound_tib();

#endif
