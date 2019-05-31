#ifndef __CONTROL_H
#define __CONTROL_H

#include <stdint.h>

void exec(uint32_t *);
uint32_t *add_def(char *, uint8_t);
char *get_substring(int(*)(int));
int to_num(int);
uint32_t cfind(char *, int *);
void interpret(char *);

void find();
void tick();
void bracket_tick_bracket();
void bracket_char_bracket();
void execute();
void exit_();
void if_();
void else_();
void then();
void do_();
void do_runtime();
void loop();
void loop_runtime();
void plus_loop();
void plus_loop_runtime();
void begin();
void while_();
void repeat();
void until();
void jump();
void cond_jump();
void lbracket();
void rbracket();
void colon();
void semicolon();
void num_runtime();
void dnum_runtime();
void quit();
void bye();
void abort_();
void abort_quote();
void abort_quote_runtime();
void paren();
void does();
void does_runtime();
void evaluate();
void immediate();
void literal();
void postpone();
void state();
void unloop();
void leave();
void recurse();

#endif
