#ifndef _REGISTERS_H_
#define _REGISTERS_H_

#include <cstdint>

typedef struct _regs Registers_t;
struct _regs {
	union {
		uint16_t R[16];
		struct {
			uint16_t PC;
			uint16_t SP;
			union {
				uint16_t SR_;
				struct {
					uint16_t C : 1;
					uint16_t Z : 1;
					uint16_t N : 1;
					uint16_t GIE : 1;
					uint16_t CPU_OFF : 1;
					uint16_t OSC_OFF : 1;
					uint16_t SCG0 : 1;
					uint16_t SCG1 : 1;
					uint16_t V : 1;
					uint16_t _reserved : 7;
				} SR;
			};
			uint16_t CG2;
		};
	};

	const uint32_t _count = 16;
};

#endif
