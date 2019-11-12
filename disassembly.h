#include "compilation.h"

char * find_label_by_addr(pair ** labels, uint16_t address);
char * disassemble_2B(uint16_t instruction);
char * disassemble_4B(uint32_t instruction, pair ** labels);
pair ** create_labels(machine_state machine);
pair ** create_variables(machine_state machine);
char * disassemble(machine_state machine, int labs, int addresses);
