#ifndef __MEMORY_H
#define __MEMORY_H

#include <stdint.h>

void store();
void store2();
void c_store();
void plus_store();
void fetch();
void fetch2();
void c_fetch();
void query();
void cell();
void cells();
void chars();
void char_plus();
void cell_plus();
void fill();
void erase();
void dump();

void variable();
void variable2();
void create();
void constant();
void constant2();
void allot();
void comma();
void c_comma();

void create_runtime();
void constant_runtime();
void constant2_runtime();

void sp_at();
void tib();
void pound_tib();
void to_body();

void align();
void aligned();

void here();

void move();

#endif
