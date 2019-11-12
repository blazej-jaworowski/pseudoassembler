#include "utils.h"

#include <string.h>

#include "execution.h"
#include "parsing.h"

char * load_file(char * file_name) {
	FILE * file;
	int size;
	char * src;
	file = fopen(file_name, "rb");
	if(file == NULL) {
		printf("Couldn't open the file.\n");
		return NULL;
	}
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	src = malloc(size + 1);
	rewind(file);
	fread(src, 1, size, file);
	src[size] = 0;
	fclose(file);
	return src;
}

int program_size(operation ** ops) {
	int i, size;
	size = 0;
	for(i = 0; ops[i] != NULL; i++) {
		if(ops[i]->type == ALLOCATION) {
			size += ops[i]->op.a.count * 4;
		} else {
			size += ops[i]->op.i.opcode % 4 * 2;
		}
	}
	return size;
}

char * op_to_str(uint8_t opcode) {
	char * r;
	switch(opcode) {
	case OP_A:
		r = "A";
		break;
	case OP_AR:
		r = "AR";
		break;
	case OP_S:
		r = "S";
		break;
	case OP_SR:
		r = "SR";
		break;
	case OP_M:
		r = "M";
		break;
	case OP_MR:
		r = "MR";
		break;
	case OP_D:
		r = "D";
		break;
	case OP_DR:
		r = "DR";
		break;
	case OP_C:
		r = "C";
		break;
	case OP_CR:
		r = "CR";
		break;
	case OP_J:
		r = "J";
		break;
	case OP_JZ:
		r = "JZ";
		break;
	case OP_JP:
		r = "JP";
		break;
	case OP_JN:
		r = "JN";
		break;
	case OP_L:
		r = "L";
		break;
	case OP_LA:
		r = "LA";
		break;
	case OP_LR:
		r = "LR";
		break;
	case OP_ST:
		r = "ST";
		break;
	}
	return r;
}

machine_state compile_from_src(char * src) {
	operation ** ops;
	ops = parse(load_file(src));
	machine_state machine;
	machine.memory_size = program_size(ops);
	machine.memory = malloc(machine.memory_size);
	compile(&machine, ops);
	return machine;
}

void save_to_file(machine_state machine, char * filename) {
	FILE * file;
	file = fopen(filename, "wb");
	fwrite(&machine.registers[DATA_REG], 4, 1, file);
	fwrite(&machine.registers[INSTRUCTION_REG], 4, 1, file);
	fwrite(&machine.exit_address, 4, 1, file);
	fwrite(machine.memory, 1, machine.memory_size, file);
	fclose(file);
}

machine_state read_from_file(char * filename) {
	FILE * file;
	file = fopen(filename, "rb");
	fseek(file, 0, SEEK_END);
	machine_state machine;
	machine.memory_size = ftell(file) - 12;
	machine.memory = malloc(machine.memory_size);
	rewind(file);
	fread(&machine.registers[DATA_REG], 4, 1, file);
	fread(&machine.registers[INSTRUCTION_REG], 4, 1, file);
	fread(&machine.exit_address, 4, 1, file);
	fread(machine.memory, 1, machine.memory_size, file);
	fclose(file);
	machine.instruction_address = machine.registers[INSTRUCTION_REG];
	return machine;
}

void print_machine(machine_state machine, machine_state previous) {
	uint8_t * memory;
	int i, j;
	int prev_changed = 0;
	char sep = ' ';
	printf("REGISTERS:\n");
	for(i = 0; i < REGISTERS; i++) {
		printf("[%d]\t%c%08x%c\n", 
				i,
				machine.registers[i] == previous.registers[i] ? ' ' : '>',	
				machine.registers[i],
				machine.registers[i] == previous.registers[i] ? ' ' : '<');
	}
	printf("\n");
	printf("FLAGS:                 \t%c%d%d%c ", 
			previous.flags == machine.flags ? ' ' : '>', 
			(machine.flags % 4) / 2, machine.flags % 2, 
			previous.flags == machine.flags ? ' ' : '<');
	switch(machine.flags) {
	case 0:
		printf("(ZERO)\n");
		break;
	case 1:
		printf("(POSITIVE)\n");
		break;
	case 2:
		printf("(NEGATIVE)\n");
		break;
	case 3:
		printf("(ERROR)\n");
		break;
	default:
		printf("\n");
	}
	printf("INSTRUCTION ADDRESS:\t%04x\n", machine.instruction_address);
	printf("EXIT ADDRESS:       \t%04x\n", machine.exit_address);
	memory = (uint8_t*)machine.memory;
	for(i = 0; i < machine.memory_size; i++) {
		if(previous.memory != NULL) {
			if(*(uint8_t*)(machine.memory + i) != *(uint8_t*)(previous.memory + i)) {
				if(prev_changed) {
					sep = '-';
				} else {
					sep = '>';
				}
				prev_changed = 1;
			} else {
				if(prev_changed) {
					sep = '<';
				} else {
					sep = ' ';
				}
				prev_changed = 0;
			}
		} else {
			sep = ' ';
		}
		printf("%c%02x", sep, memory[i]);
		if(i % 8 == 7 ) {
			printf("\n");
		}
	}
	if(prev_changed == 1) {
		printf("<");
	}
	printf("\n");
}

void execute_verbose(machine_state * machine) {
	int i, index, len, addr;
	int d = 0;
	int l = 0;
	void * address;
	uint8_t opcode;
	char * k, * disassembled;
	char c;
	machine_state previous;
	void * prev_memory = NULL;
	previous.memory = NULL;
	do { 
		system("@cls||clear");
		address = (void*)(machine->memory + machine->instruction_address);
		opcode = *(uint8_t*)address;
		
		print_machine(*machine, previous);
		printf("INSTRUCTION:\t%02x ", opcode);
		if(opcode % 4 == 1) {
			k = disassemble_2B(*(uint16_t*)address);
			printf("%02x\n        \t", *(uint8_t*)(address + 1));
			printf(k);
		} else {
			k = disassemble_4B(*(uint32_t*)address, NULL);
			printf("%02x ", *(uint8_t*)(address + 1));
			printf("%04x\n        \t", *(uint16_t*)(address + 2));
			printf(k);
		}
		printf("\n\n");
		free(k);
		if(d) {
			disassembled = disassemble(*machine, l != 0, 1);
			len = strlen(disassembled);
			index = 0;
			for(i = 0; i < len; i++) {
				if(disassembled[i] == '\n') {
					disassembled[i] = 0;
					printf(&disassembled[index]);
					sscanf(&disassembled[index], "%x", &addr);
					if(addr == machine->instruction_address) {
						printf("\t<");
					}
					printf("\n");
					index = i + 1;
				}
			}
			free(disassembled);
		}
		scanf("%c", &c);
		switch(c) {
		case 'd':
			d = !d;
			break;
		case 'l':
			l = !l;
			break;
		}
		previous = *machine;
		if(prev_memory == NULL) {
			prev_memory = malloc(previous.memory_size);
		}
		previous.memory = prev_memory;
		memcpy(prev_memory, machine->memory, previous.memory_size);
	} while(execute_step(machine) == 1);
	
	system("@cls||clear");
	print_machine(*machine, *machine);
}

void execute_file(char * filename) {
	char * src;
	machine_state machine;
	machine = compile_from_src(filename);
	execute_verbose(&machine);
}
