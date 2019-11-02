#include <stdio.h>
#include <stdlib.h>

#include "parsing.h"
#include "compilation.h"
#include "execution.h"

char * load_file(char * path) {
	FILE * file;
	int size;
	char * src;
	file = fopen(path, "rb");
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	src = malloc(size + 1);
	rewind(file);
	fread(src, 1, size, file);
	src[size] = 0;
	fclose(file);
	return src;
}

void print_machine(machine_state machine) {
	printf("REGISTERS: ");
	for(int i = 0; i < 15; i++) {
		printf("%d, ", machine.registers[i]);
	}
	printf("%d\n", machine.registers[15]);
	printf("FLAGS: %d\n", machine.flags);
	printf("INSTRUCTION ADDRESS: %d\n", machine.instruction_address);
	printf("EXIT ADDRESS: %d\n", machine.exit_address);
	uint8_t * memory = (uint8_t*)machine.memory;
	for(int i = 0; i < 16; i++) {
		for(int j = 0; j < 8; j++) {
			printf("%02x ", memory[i * 8 + j]);
		}
		printf("\n");
	}
}

int main(int argc, char **  argv) {
	char * src;

	if(argc > 1) {
		src = load_file(argv[1]);
	} else {
		src = load_file("ex/src");
	}

	operation ** ops = parse(sanitize(src));
	machine_state machine;
	machine.memory_size = 1024 * 1024;
	machine.memory = malloc(machine.memory_size);

	compile(&machine, ops);

	do { 
		void * address = (void*)(machine.memory + machine.instruction_address);
		uint8_t opcode = *(uint8_t*)address;
		printf("%02x ", opcode);
		if(opcode % 4 == 1) {
			printf("%02x\n", *(uint8_t*)(address + 1));
		} else {
			printf("%02x ", *(uint8_t*)(address + 1));
			printf("%04x\n", *(uint16_t*)(address + 2));
		}
		print_machine(machine);
		//scanf("%s", src);
	} while(execute_step(&machine) == 1);

	print_machine(machine);

	return 0;
}
