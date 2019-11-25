#ifndef __DICT_H_
#define __DICT_H_

#include <stdint.h>

typedef struct dict_entry_t {
	uint8_t flags;
	union {
		uint8_t short_str[sizeof(uint8_t *)];
		uint8_t *long_str;
	};
	struct dict_entry_t *prev;
	uint32_t data[];	
} dict_entry;

#endif
