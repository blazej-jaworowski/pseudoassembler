#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "parsing.h"

operation ** parse(char * src) {
	int i, len;
	int start = 0;
	int lines = 0;
	int index = 0;
	operation ** r;
	src = sanitize(src);
	len = strlen(src);
	for(i = 0; i < len; i++) {
		if(src[i] == '\n') {
			lines++;
		}
	}
	r = malloc(sizeof(operation*) * (lines + 1));
	for(i = 0; i < len; i++) {
		if(src[i] == '\n') {
			src[i] = 0;
			r[index++] = parse_operation(&src[start]);
			start = i + 1;
			src[i] = '\n';
		}
	}
	r[lines] = NULL;
	free(src);
	return r;
}

uint8_t str_to_op(char * op_str) {
	uint8_t opcode = 0xff;
	if(strcmp(op_str, "A") == 0) {
		opcode = OP_A;
	} else if(strcmp(op_str, "AR") == 0) {
		opcode = OP_AR;
	} else if(strcmp(op_str, "S") == 0) {
		opcode = OP_S;
	} else if(strcmp(op_str, "SR") == 0) {
		opcode = OP_SR;
	} else if(strcmp(op_str, "M") == 0) {
		opcode = OP_M;
	} else if(strcmp(op_str, "MR") == 0) {
		opcode = OP_MR;
	} else if(strcmp(op_str, "D") == 0) {
		opcode = OP_D;
	} else if(strcmp(op_str, "DR") == 0) {
		opcode = OP_DR;
	} else if(strcmp(op_str, "C") == 0) {
		opcode = OP_C;
	} else if(strcmp(op_str, "CR") == 0) {
		opcode = OP_CR;
	} else if(strcmp(op_str, "J") == 0) {
		opcode = OP_J;
	} else if(strcmp(op_str, "JZ") == 0) {
		opcode = OP_JZ;
	} else if(strcmp(op_str, "JP") == 0) {
		opcode = OP_JP;
	} else if(strcmp(op_str, "JN") == 0) {
		opcode = OP_JN;
	} else if(strcmp(op_str, "L") == 0) {
		opcode = OP_L;
	} else if(strcmp(op_str, "LA") == 0) {
		opcode = OP_LA;
	} else if(strcmp(op_str, "LR") == 0) {
		opcode = OP_LR;
	} else if(strcmp(op_str, "ST") == 0) {
		opcode = OP_ST;
	} else if(strcmp(op_str, "DC") == 0) {
		opcode = OP_DC;
	} else if(strcmp(op_str, "DS") == 0) {
		opcode = OP_DS;
	}
	return opcode;
}

operation * parse_operation(char * src) {
	int space_count = 0;
	int i, len, opcode;
	int reg, reg2;
	operation * r;
	char * label = NULL;
	char * k;
	r = malloc(sizeof(operation));
	len = strlen(src);
	k = malloc(len + 1);
	strcpy(k, src);
	src = k;

	for(i = 0; i < len; i++) {
		if(src[i] == ' ') {
			space_count++;
		}
	}
	if(space_count == 2) {
		for(i = 0; i < len; i++) {
			if(src[i] == ' ') {
				label = malloc(i + 1);
				sscanf(src, "%s", label);
				src = &src[i + 1];
				len -= i;
				break;
			}
		}
	}
	for(i = 0; i < len; i++) {
		if(src[i] == ' ') {
			src[i] = 0;
			opcode = str_to_op(src);
			src = &src[i + 1];
			len -= i;
			break;
		}
	}
	
	switch(opcode) {
	case OP_A:
	case OP_S:
	case OP_M:
	case OP_D:
	case OP_C:
	case OP_L:
	case OP_LA:
	case OP_ST:
		r->type = INSTRUCTION;
		r->op.i.label = label;
		r->op.i.opcode = opcode;
		sscanf(src, "%d", &reg);
		reg <<= 4;
		r->op.i.registers = reg;
		for(i = 0; i < len; i++) {
			if(src[i] == ',') {
				src = &src[i + 1];
				len -= i;
			}
		}
		reg = DATA_REG;
		for(i = 0; i < len; i++) {
			if(src[i] == '(') {
				src[i] = 0;
				sscanf(&src[i + 1], "%d", &reg);
				break;
			}
		}
		r->op.i.registers += reg;
		len = strlen(src);
		r->op.i.address = malloc(len + 1);
		sscanf(src, "%s", r->op.i.address);
		break;
	case OP_AR:
	case OP_SR:
	case OP_MR:
	case OP_DR:
	case OP_CR:
	case OP_LR:
		r->type = INSTRUCTION;
		r->op.i.label = label;
		r->op.i.opcode = opcode;
		sscanf(src, "%d,%d", &reg, &reg2);
		reg <<= 4;
		r->op.i.registers = reg + reg2;
		break;
	case OP_J:
	case OP_JZ:
	case OP_JP:
	case OP_JN:
		r->type = INSTRUCTION;
		r->op.i.label = label;
		r->op.i.opcode = opcode;
		reg = INSTRUCTION_REG;
		for(i = 0; i < len; i++) {
			if(src[i] == '(') {
				src[i] = 0;
				sscanf(&src[i  + 1], "%d", &reg);
				break;
			}
		}
		r->op.i.registers = reg;
		len = strlen(src);
		r->op.i.address = malloc(len + 1);
		sscanf(src, "%s", r->op.i.address);
		break;
	case OP_DC:
		r->type = ALLOCATION;
		r->op.a.label = label;
		r->op.a.count = 1;
		if(isdigit(src[0])) {
			sscanf(src, "%d*INTEGER(%d)", &r->op.a.count, &r->op.a.value);
		} else {
			sscanf(src, "INTEGER(%d)", &r->op.a.value);
		}
		break;
	case OP_DS:
		r->op.a.count = 0;
		r->type = ALLOCATION;
		r->op.a.label = label;
		r->op.a.count = 1;
		if(isdigit(src[0])) {
			sscanf(src, "%d", &r->op.a.count);
		}
		r->op.a.value = NO_VALUE;
		break;
	}
	free(k);
	return r;
}

char * sanitize(char * src) {
	int len, new_len;
	char * buf;
	char * r;
	int nl_prev = 0;
	int i, index;

	len = strlen(src);
	new_len = len;

	buf = malloc(len);

	for(i = 0; i < len - 1; i++) {
		buf[i] = src[i];
		if(src[i] == '\n') {
			nl_prev = 1;
			continue;
		}
		if(isspace(src[i])) {
			buf[i] = ' ';
			if(isspace(src[i + 1]) || nl_prev) {
				buf[i] = 0;
				new_len--;
			}
		}
		nl_prev = 0;
	}

	nl_prev = 0;
	for(i = 0; i < len; i++) {
		if(buf[i] == '\n' && nl_prev) {
			nl_prev = 1;
			buf[i] = 0;
			new_len--;
			continue;
		}
		if(buf[i] == '\n') {
			nl_prev = 1;
		} else if(buf[i] != 0) {
			nl_prev = 0;
		}
	}

	if(isspace(src[len - 1])) {
		buf[len - 1] = 0;
		new_len--;
	} else {
		buf[len - 1] = src[len - 1];
	}
	index = 0;
	r = malloc(new_len + 2);
	for(i = 0; i < len; i++) {
		if(buf[i] == 0) {
			continue;
		}
		r[index++] = buf[i];
	}
	free(buf);
	if(r[new_len - 1] != '\n') {
		r[new_len] = '\n';
		r[new_len + 1] = 0;
	} else {
		r[new_len] = 0;
	}
	return r;
}
