#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "compilation.h"

uint16_t get_address(char * label, pair ** labels) {
	int i;
	for(i = 0; labels[i] != NULL; i++) {
		if(strcmp(label, labels[i]->label) == 0) {
			return labels[i]->address;
		}
	}
	return 0xffff;
}

void compile(machine_state * machine, operation ** ops) {
	pair **  labels;
	pair * p;
	operation * op;
	instruction instr;
	allocation alloc;
	int op_count, i, j;
	int label_index = 0;
	uint32_t memory_head, new_memory_head;
	for(op_count = 0; ops[op_count] != NULL; op_count++);

	labels = malloc(sizeof(pair*) * (op_count + 1));
	for(i = 0; i < op_count + 1; i++) {
		labels[i] = NULL;
	}
	
	machine->registers[DATA_REG] = PROGRAM_START;
	memory_head = PROGRAM_START;
	for(i = 0; ops[i] != NULL && ops[i]->type == ALLOCATION; i++) {
		alloc = ops[i]->op.a;
		if(alloc.label != NULL) {
			p = malloc(sizeof(pair));
			p->label = alloc.label;
			p->address = memory_head - PROGRAM_START;
			labels[label_index++] = p;
		}
		
		new_memory_head = memory_head + 4 * alloc.count;
		for(; memory_head < new_memory_head; memory_head += 4) {
			*(uint32_t*)(machine->memory + memory_head) = alloc.value;
		}
	}

	j = i;
	machine->registers[INSTRUCTION_REG] = memory_head;
	new_memory_head = memory_head;
	for(; ops[j] != NULL; j++) {
		instr = ops[j]->op.i;
		if(instr.label != NULL) {
			p = malloc(sizeof(pair));
			p->label = instr.label;
			p->address = new_memory_head - machine->registers[INSTRUCTION_REG];
			labels[label_index++] = p;
		}
		if(instr.opcode % 4 == 1) {
			new_memory_head += 2;
		} else {
			new_memory_head += 4;
		}
	}

	for(; ops[i] != NULL; i++) {
		instr = ops[i]->op.i;

		*(uint8_t*)(machine->memory + memory_head++) = instr.opcode;
		*(uint8_t*)(machine->memory + memory_head++) = instr.registers;

		if(instr.opcode % 4  == 2) {
			if(isdigit(instr.address[0])) {
				*(uint16_t*)(machine->memory + memory_head) =  atoi(instr.address);
			} else {
				*(uint16_t*)(machine->memory + memory_head) 
					=  get_address(instr.address, labels);
			}
			memory_head += 2;
		}
	}
	machine->exit_address = memory_head;
	machine->instruction_address = machine->registers[INSTRUCTION_REG];
	for(i = 0; labels[i] != NULL; i++) {
		free(labels[i]);
	}
	free(labels);
}
