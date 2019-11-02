#pragma once

#include <stdint.h>

#include "defines.h"

typedef struct {
	char * label;
	uint8_t opcode;
	uint8_t registers;
	char * address;
} instruction;

typedef struct {
	char * label;
	uint32_t count;
	uint32_t value;
} allocation;

typedef enum {
	INSTRUCTION = 0,
	ALLOCATION = 1
} operation_type;

typedef struct {
	operation_type type;
	union {
		instruction i;
		allocation a;
	} op;
} operation;

char * sanitize(char * src);
operation ** parse(char * src);
operation * parse_operation(char * src);
