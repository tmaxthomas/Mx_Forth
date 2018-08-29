#include <unistd.h>
#include <stdint.h>
#include <termios.h>
#include <stdio.h>

#include "input.h"
#include "../stack.h"



void accept() {
    int32_t num = *(int32_t *) stack_at(0);
    char *buf = *(char **) stack_at(1);
    stack_pop(1);
    int n_read = read(0, buf, num);
    stack_push(n_read);
}

void key() {
   struct termios term;
   tcgetattr(0, &term);
   term.c_lflag &= ~(ICANON | ECHO);
   tcsetattr(0, TCSANOW, &term);
   char ch = getchar();
   stack_push(ch);
   term.c_lflag |= (ICANON | ECHO);
   tcsetattr(0, TCSANOW, &term);
}
