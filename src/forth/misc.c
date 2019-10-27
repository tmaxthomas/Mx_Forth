#include "misc.h"
#include "../stack.h"
#include "../sys.h"
#include "../defs.h"
#include <string.h>

void environment() {
    char *query = (char *) sys_addr(*stack_at(1));
    uint32_t len = *stack_at(0);
    stack_pop(2);

    if(len == 15 && strncmp("/COUNTED-STRING", query, len) == 0) {
        stack_push(255);
        stack_push(-1);
        return;
    } else if (len == 5 && strncmp("/HOLD", query, len) == 0) {
        stack_push(255);
        stack_push(-1);
        return;
    } else if (len == 4 && strncmp("/PAD", query, len) == 0) {
        stack_push(255);
        stack_push(-1);
        return;
    } else if (len == 17 && strncmp("ADDRESS-UNIT-BITS", query, len) == 0) {
        stack_push(8);
        stack_push(-1);
        return;
    } else if (len == 4 && strncmp("CORE", query, len) == 0) {
        stack_push(-1);
        stack_push(-1);
        return;
    } else if (len == 8 && strncmp("CORE-EXT", query, len) == 0) {
        stack_push(0); 
        stack_push(-1);
        return;
    } else if (len == 7 && strncmp("FLOORED", query, len) == 0) {
        stack_push(-1);
        stack_push(-1);
        return;
    } else if (len == 8 && strncmp("MAX-CHAR", query, len) == 0) {
        stack_push(0x7F);
        stack_push(-1);
        return;
    } else if (len == 5 && strncmp("MAX-D", query, len) == 0) {
        stack_push_d(9223372036854775807);
        stack_push(-1);
        return;
    } else if (len == 7 && strncmp("MAX-N", query, len) == 0) {
        stack_push(2147483647);
        stack_push(-1);
        return;
    } else if (len == 7 && strncmp("MAX-U", query, len) == 0) {
        stack_push(-1);
        stack_push(-1);
        return;
    } else if (len == 7 && strncmp("MAX-UD", query, len) == 0) {
        stack_push_d(-1);
        stack_push(-1);
        return;
    } else if (len == 18 && strncmp("RETURN-STACK-CELLS", query, len) == 0) {
        stack_push(SYSTEM_SIZE / 4);
        stack_push(-1);
        return;
    } else if (len == 11 && strncmp("STACK-CELLS", query, len) == 0) {
        stack_push(SYSTEM_SIZE / 4);
        stack_push(-1);
        return;
    }
    
    stack_push(0);
}
