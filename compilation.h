#pragma once

#include <stdint.h>

#include "defines.h"
#include "parsing.h"

typedef struct {
	uint32_t registers[REGISTERS];
	uint32_t instruction_address;
	uint8_t flags;
	uint32_t exit_address;
	int memory_size;
	void * memory;
} machine_state;

void compile(machine_state * machine, operation ** ops);
