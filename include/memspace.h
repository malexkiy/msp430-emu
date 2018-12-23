#ifndef _MEMSPACE_H_
#define _MEMSPACE_H_

#include <cstdint>

class Memspace
{
private:
	uint8_t *_memspace;
	uint8_t *_code;
	uint8_t *_info;
	uint8_t *_ivt;
	uint8_t *_rom;
	uint8_t *_ram;
	uint8_t *_per16;
	uint8_t *_per8;
	uint8_t *_sfrs;

public:
	Memspace();
	~Memspace();

	uint16_t* addr_ptr(uint16_t virtual_addr) const;
	uint16_t* stack_ptr(uint16_t sp) const;

	uint8_t* code_ptr(void) const;
	uint16_t code_size(void) const;
};

#endif
