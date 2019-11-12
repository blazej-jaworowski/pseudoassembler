#include <stddef.h>

#define OP_A	(0<<2)+2
#define OP_AR	(1<<2)+1
#define OP_S	(2<<2)+2
#define OP_SR	(3<<2)+1
#define OP_M	(4<<2)+2
#define OP_MR	(5<<2)+1
#define OP_D	(6<<2)+2
#define OP_DR	(7<<2)+1
#define OP_C	(8<<2)+2
#define OP_CR	(9<<2)+1
#define OP_J	(10<<2)+2
#define OP_JZ	(11<<2)+2
#define OP_JP	(12<<2)+2
#define OP_JN	(13<<2)+2
#define OP_L	(14<<2)+2
#define OP_LA	(15<<2)+2
#define OP_LR	(16<<2)+1
#define OP_ST	(17<<2)+2
#define OP_DC	(18<<2)
#define OP_DS	(19<<2)

#define DATA_REG 14
#define INSTRUCTION_REG 15

#define NO_VALUE 0xffffffff

#define REGISTERS 16
#define PROGRAM_START 0

#define DISASSEMBLE_BUFFER_SIZE 2000
#define DISASSEMBLE_INSTRUCTION_BUFFER_SIZE 100
