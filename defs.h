#ifndef __DEFS_H_
#define __DEFS_H_

//This file contains system-wide defines

//Size of the FORTH system size, in words (for now, 4 MB or 2^20 words)
#define SYSTEM_SIZE 4194304

// Some global constants
#define EXIT_ADDR 0
#define DNUM_RUNTIME_ADDR 1
#define NUM_RUNTIME_ADDR 2
#define COND_JUMP_ADDR 3
#define JUMP_ADDR 4
#define DO_RUNTIME_ADDR 5
#define LOOP_RUNTIME_ADDR 6
#define PLUS_LOOP_RUNTIME_ADDR 7
#define ABORT_QUOTE_RUNTIME_ADDR 8
#define DOES_RUNTIME_ADDR 9
#define UNLOOP_ADDR 10

#endif
