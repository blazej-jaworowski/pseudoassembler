#pragma once

#include <stdint.h>

#include "defines.h"
#include "compilation.h"

void execute(machine_state * machine);
int execute_step(machine_state * machine);
void execute_2B(uint32_t instruction_address, machine_state * machine);
void execute_4B(uint32_t instruction_address, machine_state * machine);
