#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parsing.h"
#include "compilation.h"
#include "execution.h"
#include "utils.h"


int main(int argc, char **  argv) {
	machine_state machine;
	int labels, addresses, d;

	if(argc <= 1) {
		execute_file("ex/fib");
		return 0;
	}
	
	if(argc == 2) {
		execute_file(argv[1]);
		return 0;
	}

	if(strcmp(argv[1], "compile") == 0) {
		if(argc > 3) {
			machine = compile_from_src(argv[2]);
			save_to_file(machine, argv[3]);
		}
		return 0;
	} else if(strcmp(argv[1], "disassemble") == 0) {
		if(argc > 4) {
			sscanf(argv[3], "%d", &labels);
			sscanf(argv[4], "%d", &addresses);
			
		} else {
			labels = 1;
			addresses = 0;
		}
		machine = read_from_file(argv[2]);
		printf(disassemble(machine, labels, addresses));
		return 0;
	} else if(strcmp(argv[1], "execute") == 0) {
		machine = read_from_file(argv[2]);
		execute_verbose(&machine);
		return 0;
	}

	return 0;
}
