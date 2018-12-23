#ifndef _EMULATOR_H_
#define _EMULATOR_H_

#include "msp430.h"
#include "debugger.h"

#include <string>

class Emulator
{
private:
	Cpu *_cpu;
	Debugger *_dbgr;

public:
	Emulator();
	~Emulator();

	void run(const std::string& fw_file, const std::string& log_file);
};

#endif
