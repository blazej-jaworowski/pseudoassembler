#include "disassembly.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utils.h"

char * find_label_by_addr(pair ** labels, uint16_t address) {
	int i;
	if(labels == NULL) {
		return "";
	}
	for(i = 0; labels[i] != NULL; i++) {
		if(labels[i]->address == address) {
			return labels[i]->label;
		}
	}
	return "";
}

char * disassemble_2B(uint16_t instruction) {
	char * buf;
	buf = malloc(DISASSEMBLE_INSTRUCTION_BUFFER_SIZE);
	sprintf(buf, "%s\t%d,%d", op_to_str(instruction % 256), 
			instruction >> 12, 
				(instruction >> 8) % 16);
	return buf;
}

char * disassemble_4B(uint32_t instruction, pair ** labels) {
	char * buf;
	char * label = "";
	uint8_t opcode;
	uint8_t reg1;
	uint8_t reg2;
	uint16_t addr;
	buf = malloc(DISASSEMBLE_INSTRUCTION_BUFFER_SIZE);
	opcode = instruction % 256;
	addr = instruction >> 16;
	reg1 = (instruction >> 12) % 16;
	reg2 = (instruction >> 8) % 16;
	if(labels != NULL) {
		label = find_label_by_addr(labels, addr);
	}
	if(*label == 0 || (reg2 != DATA_REG && reg2 != INSTRUCTION_REG)) {
		if(opcode == OP_J || opcode == OP_JZ || opcode == OP_JP || opcode == OP_JN) {
			sprintf(buf, "%s\t%d(%d)", op_to_str(opcode), addr, reg2);
		} else {
			sprintf(buf, "%s\t%d,%d(%d)", op_to_str(opcode), reg1, addr, reg2);
		}
	} else {
		if(opcode == OP_J || opcode == OP_JZ || opcode == OP_JP || opcode == OP_JN) {
			if(reg2 == INSTRUCTION_REG) {
				sprintf(buf, "%s\t%s", op_to_str(opcode), label, reg2);
			} else {
				sprintf(buf, "%s\t%d(%d)", op_to_str(opcode), addr, reg2);
			}
		} else {
			if(reg2 == DATA_REG) {
				sprintf(buf, "%s\t%d,%s", op_to_str(opcode), reg1, label, reg2);
			} else {
				sprintf(buf, "%s\t%d,%d(%d)", op_to_str(opcode), reg1, addr, reg2);
			}
		}
	}
	return buf;
}

pair ** create_labels(machine_state machine) {
	int i, size;
	int index = 0;
	uint8_t reg;
	uint16_t addr;
	uint32_t instruction;
	pair ** r;
	pair * p;
	int len = 0;
	int ls = 0;
	char * label;
	for(i = machine.registers[INSTRUCTION_REG]; i < machine.exit_address;) {
		size = *(uint8_t*)(machine.memory + i) % 4 * 2;
		if(size == 2) {
			i += 2;
			continue;
		}
		reg = (*(uint32_t*)(machine.memory + i) >> 8) % 16;
		len++;
		i += size;
	}
	r = malloc(sizeof(pair) * (len + 1));
	r[0] = NULL;
	for(i = machine.registers[INSTRUCTION_REG]; i < machine.exit_address;) {
		size = *(uint8_t*)(machine.memory + i) % 4 * 2;
		if(size == 2) {
			i += 2;
			continue;
		}
		instruction = *(uint32_t*)(machine.memory + i);
		reg = (instruction >> 8) % 16;
		addr = instruction >> 16;
		if(reg == INSTRUCTION_REG && strcmp(find_label_by_addr(r, addr), "") == 0) {
			p = malloc(sizeof(pair));
			label = malloc(8);
			sprintf(label, "LAB%d", ls++);
			p->label = label;
			p->address = addr;
			r[index++] = p;
			r[index] = NULL;
		}
		i += size;
	}
	return r;
}

pair ** create_variables(machine_state machine) {
	int i, size;
	int index = 0;
	uint8_t reg;
	uint16_t addr;
	uint32_t instruction;
	pair ** r;
	pair * p;
	int len = 0;
	int vs = 0;
	char * label;
	for(i = machine.registers[INSTRUCTION_REG]; i < machine.exit_address;) {
		size = *(uint8_t*)(machine.memory + i) % 4 * 2;
		if(size == 2) {
			i += 2;
			continue;
		}
		reg = (*(uint32_t*)(machine.memory + i) >> 8) % 16;
		len++;
		i += size;
	}
	r = malloc(sizeof(pair) * (len + 1));
	r[0] = NULL;
	for(i = machine.registers[INSTRUCTION_REG]; i < machine.exit_address;) {
		size = *(uint8_t*)(machine.memory + i) % 4 * 2;
		if(size == 2) {
			i += 2;
			continue;
		}
		instruction = *(uint32_t*)(machine.memory + i);
		reg = (instruction >> 8) % 16;
		addr = instruction >> 16;
		if(reg == DATA_REG && strcmp(find_label_by_addr(r, addr), "") == 0) {
			p = malloc(sizeof(pair));
			label = malloc(8);
			sprintf(label, "VAR%d", vs++);
			p->label = label;
			p->address = addr;
			r[index++] = p;
			r[index] = NULL;
		}
		i += size;
	}
	return r;
}

char * disassemble(machine_state machine, int labs, int addresses) {
	int i, size;
	int32_t value;
	uint8_t opcode;
	int index = 0;
	char * buf;
	pair ** labels, ** variables, ** k;
	int dr = machine.registers[DATA_REG];
	int ir = machine.registers[INSTRUCTION_REG];
	buf = malloc(DISASSEMBLE_BUFFER_SIZE);

	if(labs) {
		labels = create_labels(machine);
		variables = create_variables(machine);
	} else {
		labels = NULL;
		variables = NULL;
	}
	for(i = dr; i < ir; i += 4) {
		if(addresses) {
			sprintf(&buf[index], "%04x:\t", i);
			index = strlen(buf);
			buf[index] = 0;
		}
		value = *(int32_t*)(machine.memory + i);
		sprintf((buf + index), "%s\tDC\tINTEGER(%d)\n",
				find_label_by_addr(variables, i - dr), value);
		index = strlen(buf);
	}
	buf[index++] = '\n';
	buf[index] = 0;

	for(i = ir; i < machine.exit_address;) {
		opcode = *(uint8_t*)(machine.memory + i);
		size = opcode % 4 * 2;
		if(addresses) {
			sprintf(&buf[index], "%04x:\t", i);
			index = strlen(buf);
			buf[index] = 0;
		}
		if(size == 2) {
			sprintf(&buf[index], "%s\t%s", find_label_by_addr(labels, i - ir),
					disassemble_2B(*(uint16_t*)(machine.memory + i)));
		} else {
			if(opcode == OP_J || opcode == OP_JP || opcode == OP_JZ || opcode == OP_JN) {
				k = labels;
			} else {
				k = variables;
			}
			sprintf(&buf[index], "%s\t%s", find_label_by_addr(labels, i - ir),
					disassemble_4B(*(uint32_t*)(machine.memory + i), k));
		}
		index = strlen(buf);
		buf[index++] = '\n';
		buf[index] = 0;
		i += size;
	}

	return buf;
}

