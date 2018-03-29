#ifndef __CONTROL_H
#define __CONTROL_H

uint32_t *add_def(char *name, uint8_t precedence);

void find();
void tick();
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
void quit();
void bye();
void abort_();
void abort_quote();

#endif
