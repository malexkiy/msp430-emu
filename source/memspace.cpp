#include "memspace.h"
#include <cstring>


#define SETVAL_8(addr, val)	*((uint8_t *)(addr)) = (val)

constexpr uint32_t MEMSPACE_SIZE = 0x10000;
constexpr uint16_t INFO_OFFSET   = 0x1000;
constexpr uint16_t INFO_SIZE     = 0x0100;
constexpr uint16_t CODE_OFFSET   = 0xC000;
constexpr uint16_t CODE_SIZE     = 0x4000;
constexpr uint16_t IVT_OFFSET    = 0xFFC0;
constexpr uint16_t ROM_OFFSET    = 0x0400;
constexpr uint16_t RAM_OFFSET    = 0x0200;
constexpr uint16_t PER16_OFFSET  = 0x0100;
constexpr uint16_t PER8_OFFSET   = 0x0010;
constexpr uint16_t SFRS_OFFSET   = 0x0000;


Memspace::Memspace()
{
	_memspace = new uint8_t[MEMSPACE_SIZE]();
	
	_info = _memspace + INFO_OFFSET;
	memset(_info, 0xFF, INFO_SIZE);
	
	_code = _memspace + CODE_OFFSET;
	
	_ivt = _memspace + IVT_OFFSET;
	
	_rom = _memspace + ROM_OFFSET;
	
	_ram = _memspace + RAM_OFFSET;
	_per16 = _memspace + PER16_OFFSET;
	_per8 = _memspace + PER8_OFFSET;
	_sfrs = _memspace + SFRS_OFFSET;
	
	constexpr uint16_t CALDCO_16MHZ_VLOC = 0x10F8;
	SETVAL_8(_memspace + CALDCO_16MHZ_VLOC, 0x95);

	constexpr uint16_t CALBC1_16MHZ_VLOC = 0x10F9;
	SETVAL_8(_memspace + CALBC1_16MHZ_VLOC, 0x8F);

	constexpr uint16_t CALDCO_12MHZ_VLOC = 0x10FA;
	SETVAL_8(_memspace + CALDCO_12MHZ_VLOC, 0x9E);

	constexpr uint16_t CALBC1_12MHZ_VLOC = 0x10FB;
	SETVAL_8(_memspace + CALBC1_12MHZ_VLOC, 0x8E);

	constexpr uint16_t CALDCO_8MHZ_VLOC = 0x10FC;
	SETVAL_8(_memspace + CALDCO_8MHZ_VLOC, 0x92);

	constexpr uint16_t CALBC1_8MHZ_VLOC = 0x10FD;
	SETVAL_8(_memspace + CALBC1_8MHZ_VLOC, 0x8D);

	constexpr uint16_t CALDCO_1MHZ_VLOC = 0x10FE;
	SETVAL_8(_memspace + CALDCO_1MHZ_VLOC, 0xD1);

	constexpr uint16_t CALBC1_1MHZ_VLOC = 0x10FF;
	SETVAL_8(_memspace + CALBC1_1MHZ_VLOC, 0x86);
}


Memspace::~Memspace()
{
	delete[] _memspace;
}


uint16_t* Memspace::addr_ptr(uint16_t virtual_addr) const
{
	return (uint16_t *)(_memspace + virtual_addr);
}


uint16_t* Memspace::stack_ptr(uint16_t sp) const
{
	return (uint16_t *)(_memspace + sp);
}


uint8_t* Memspace::code_ptr(void) const
{
	return _code;
}


uint16_t Memspace::code_size(void) const
{
	return CODE_SIZE;
}
