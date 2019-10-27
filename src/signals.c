#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>

#include "signals.h"
#include "forth/control.h"

// This file contains signal handlers & signal handler-related functions

void register_handlers() {
    int failure = 0;
    struct sigaction s;
    bzero(&s, sizeof s);
    s.sa_handler = SIGSEGV_handler;
    failure |= sigaction(SIGSEGV, &s, NULL);

    if (failure) {
        fprintf(stderr, "ERROR: Unable to register signal handlers\n");
        exit(EXIT_FAILURE);
    }
}

void SIGSEGV_handler(int signum) {
    fprintf(stderr, "ERROR: Invalid memory read/write detected, aborting\n");
    abort_();
}
