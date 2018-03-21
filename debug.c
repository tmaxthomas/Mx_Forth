#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

//Grabs the contents of a definition and dumps them as part of a string. Used for debugging ONLY.
char* word_dump(uint32_t *word) {
    uint8_t *ccp = (uint8_t*) word;
    //Get precedence bit
    uint8_t precedence = *ccp;
    ccp++;
    //Get length
    uint8_t len = *ccp;
    ccp++;
    //Extract name
    char* name = malloc(len + 1);
    for(int i = 0; i < len; i++) {
        name[i] = *ccp;
        ccp++;
    }
    name[len] = 0;

    //Get back pointer and xt
    uint32_t* cp = (uint32_t*) ccp;
    uint32_t* back_ptr = (uint32_t*)*cp;
    cp++;
    uint32_t* xt = cp;
    //Create and return result of dump
    char* ret_string = malloc(len + 10);
    sprintf(ret_string, "precedence: %hhu, length: %hhu, name: %s, back pointer: %p, xt: %p", precedence, len, name, (void*) back_ptr, (void*) xt);
    free(name);
    return ret_string;
}
