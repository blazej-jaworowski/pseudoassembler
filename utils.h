#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "compilation.h"
#include "disassembly.h"

char * load_file(char * file_name);
void print_machine(machine_state machine, machine_state previous);
int program_size(operation ** ops);
char * op_to_str(uint8_t);
void execute_file(char * file_name);
machine_state compile_from_src(char * src);
void save_to_file(machine_state machine, char * filename);
machine_state read_from_file(char * filename);
void execute_verbose(machine_state * machine);
