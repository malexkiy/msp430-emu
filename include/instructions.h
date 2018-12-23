#ifndef _INSTRUCTION_H_
#define _INSTRUCTION_H_

#include <cstdint>

typedef struct _dual_op DualOperand_t;
struct _dual_op {
	union {
		uint16_t _instr;
		struct {
			uint16_t d_reg : 4;
			uint16_t as : 2;
			uint16_t bw : 1;
			uint16_t ad : 1;
			uint16_t s_reg : 4;
			uint16_t op_code : 4;
		};
	};
};

typedef struct _single_op SingleOperand_t;
struct _single_op {
	union {
		uint16_t _instr;
		struct {
			uint16_t ds_reg : 4;
			uint16_t ad : 2;
			uint16_t bw : 1;
			uint16_t op_code : 9;
		};
	};
};

typedef struct _jumps Jumps_t;
struct _jumps {
	union {
		uint16_t _instr;
		struct {
			uint16_t pc_offset : 10;
			uint16_t c : 3;
			uint16_t op_code : 3;
			//PCnew = PCold + 2 + PCoffset*2
		};
	};
};

enum InstructionLenght_t{
	WORD = 16,
	BYTE = 8
};

#endif
