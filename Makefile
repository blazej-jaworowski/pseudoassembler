main.exe : main.c parsing.c compilation.c execution.c utils.c disassembly.c
	gcc -g -o main main.c parsing.c compilation.c execution.c utils.c disassembly.c
