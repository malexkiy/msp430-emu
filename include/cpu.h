#ifndef _CPU_H_
#define _CPU_H_

#include "registers.h"
#include "memspace.h"

#include <cstdint>
#include <string>

class Debugger;

class Cpu
{
protected:
	friend class Debugger;

	Registers_t _cpu_regs;
	Memspace _memspace;

	bool _is_running;

public:
	Cpu();
	virtual ~Cpu();

	virtual int32_t load_fw(const std::string& fw_filename) = 0;
	virtual bool step(void) = 0;

protected:
	virtual bool decode(const uint16_t instr) = 0;
	virtual uint16_t fetch() = 0;
};

#endif
