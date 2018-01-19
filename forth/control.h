#ifndef __CONTROL_H
#define __CONTROL_H

//Utility macro for getting char-delimited substrings of C strings
#define GetSubstring(boolean) tmp_idx = sys.idx; for (i = 0; !(boolean); i++) tmp_idx++; \
                           tmp_buf = (char *) malloc(i + 1); for (size_t j = 0; j < i; j++) tmp_buf[j] = sys.idx[j]; \
                           tmp_buf[i] = 0; sys.idx = tmp_idx; if (*sys.idx) sys.idx++

void find();
void tick();
void execute();
void exit_();
void if_();
void else_();
void then();
void do_();
void loop();
void begin();
void while_();
void repeat();
void until();
void again();
void jump();
void cond_jump();

#endif
