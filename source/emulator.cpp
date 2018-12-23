#include "emulator.h"
#include <fstream>


Emulator::Emulator()
{
	_cpu = new Msp430();
	_dbgr = new Debugger(_cpu);
}


Emulator::~Emulator()
{
	delete _cpu;
	delete _dbgr;
}


void Emulator::run(const std::string& fw_file, const std::string& log_file)
{
	bool is_running = true;
	std::ofstream out(log_file);

	_cpu->load_fw(fw_file);

	_dbgr->print_cpu_state(out);
	while (is_running)
	{
		is_running = _dbgr->step();
		_dbgr->print_cpu_state(out);
	}

	out.close();
}
