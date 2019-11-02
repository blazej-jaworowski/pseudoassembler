#include <stdlib.h>

#include "defines.h"
#include "execution.h"

void execute(machine_state * machine) {
	while(execute_step(machine) == 1);
}

int execute_step(machine_state * machine) {
	uint32_t instruction_addr;
	int instruction_size;
	instruction_size = *(uint8_t*)(machine->memory + machine->instruction_address) % 4 * 2;
	instruction_addr = machine->instruction_address;
	machine->instruction_address += instruction_size;
	if(instruction_size == 2) {
		execute_2B(instruction_addr, machine);
	} else {
		execute_4B(instruction_addr, machine);
	}

	if(machine->instruction_address == machine->exit_address) {
		return 0;
	}

	if(machine->flags == 3) {
		return -1;
	}

	return 1;
}

void execute_2B(uint32_t instruction_addr, machine_state * machine) {
	uint8_t opcode;
	uint8_t reg1, reg2;
	int32_t value;
	int set_flags = 1;
	opcode = *(uint8_t*)(machine->memory + instruction_addr);
	reg1 = *(uint8_t*)(machine->memory + instruction_addr + 1) >> 4;
	reg2 = *(uint8_t*)(machine->memory + instruction_addr + 1) % 16;
	switch(opcode) {
	case OP_AR:
		value = machine->registers[reg1] + machine->registers[reg2];
		machine->registers[reg1] = value;
		break;
	case OP_SR:
		value = machine->registers[reg1] - machine->registers[reg2];
		machine->registers[reg1] = value;
		break;
	case OP_MR:
		value = machine->registers[reg1] * machine->registers[reg2];
		machine->registers[reg1] = value;
		break;
	case OP_DR:
		if(machine->registers[reg2] == 0) {
			machine->flags = 3;
			return;
		}
		value = machine->registers[reg1] / machine->registers[reg2];
		machine->registers[reg1] = value;
		break;
	case OP_CR:
		value = machine->registers[reg1] - machine->registers[reg2];
		break;
	case OP_LR:
		set_flags = 0;
		machine->registers[reg1] = machine->registers[reg2];
		break;
	}

	if(!set_flags) {
		return;
	}
	if(value == 0) {
		machine->flags = 0;
	} else if(value > 0) {
		machine->flags = 1;
	} else {
		machine->flags = 2;
	}
}

void execute_4B(uint32_t instruction_addr, machine_state * machine) {
	uint8_t opcode;
	uint8_t reg;
	uint32_t addr;
	uint32_t * addr_pointer;
	int32_t value;
	int set_flags = 1;
	opcode = *(uint8_t*)(machine->memory + instruction_addr);
	reg = *(uint8_t*)(machine->memory + instruction_addr + 1) >> 4;
	addr = *(uint16_t*)(machine->memory + instruction_addr + 2)
		+ machine->registers[*(uint8_t*)(machine->memory + instruction_addr + 1) % 16];

	if(addr > machine->memory_size && opcode != OP_LR) {
		machine->flags = 3;
		return;
	}

	addr_pointer = (uint32_t*)(machine->memory + addr);

	switch(opcode) {
	case OP_A:
		value = machine->registers[reg] + *addr_pointer;
		machine->registers[reg] = value;
		break;
	case OP_S:
		value = machine->registers[reg] - *addr_pointer;
		machine->registers[reg] = value;
		break;
	case OP_M:
		value = machine->registers[reg] * *addr_pointer;
		machine->registers[reg] = value;
		break;
	case OP_D:
		if(*addr_pointer == 0) {
			machine->flags = 3;
			return;
		}
		value = machine->registers[reg] / *addr_pointer;
		machine->registers[reg] = value;
		break;
	case OP_C:
		value = machine->registers[reg] - *addr_pointer;
		break;
	case OP_J:
		set_flags = 0;
		machine->instruction_address = addr;
		break;
	case OP_JZ:
		set_flags = 0;
		if(machine->flags == 0) {
			machine->instruction_address = addr;
		}
		break;
	case OP_JP:
		set_flags = 0;
		if(machine->flags == 1) {
			machine->instruction_address = addr;
		}
		break;
	case OP_JN:
		set_flags = 0;
		if(machine->flags == 2) {
			machine->instruction_address = addr;
		}
		break;
	case OP_L:
		set_flags = 0;
		machine->registers[reg] = *addr_pointer;
		break;
	case OP_LA:
		set_flags = 0;
		machine->registers[reg] = addr;
		break;
	case OP_ST:
		set_flags = 0;
		*addr_pointer = machine->registers[reg];
		break;
	}

	if(!set_flags) {
		return;
	}
	if(value == 0) {
		machine->flags = 0;
	} else if(value > 0) {
		machine->flags = 1;
	} else {
		machine->flags = 2;
	}
}
