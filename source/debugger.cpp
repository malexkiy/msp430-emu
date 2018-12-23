#include "debugger.h"
#include "cpu.h"

#include <iostream>
#include <iomanip>


Debugger::Debugger(Cpu *cpu)
	:_cpu(cpu)
{
}


Debugger::~Debugger()
{
}


bool Debugger::step(void) const
{
	return _cpu->step();
}


std::ostream& Debugger::print_cpu_state(std::ostream &os)
{
	using std::endl;
	using std::hex;
	using std::setfill;
	using std::setw;
	using std::dec;
	using std::uppercase;

	os << "CPU state: " << (_cpu->_is_running ? "running" : "halt") << endl;

	os << "PC:  ";
	os << setfill('0') << setw(4) << hex << uppercase << _cpu->_cpu_regs.PC << "  ";
	os << "SP:  ";
	os << setfill('0') << setw(4) << hex << uppercase << _cpu->_cpu_regs.SP << "  ";
	os << "SR:  ";
	os << setfill('0') << setw(4) << hex << uppercase << _cpu->_cpu_regs.SR_ << "  ";
	os << "CG2: ";
	os << setfill('0') << setw(4) << hex << uppercase << _cpu->_cpu_regs.CG2 << endl;

	for (uint32_t i = 4; i < _cpu->_cpu_regs._count; i++)
	{
		if (i % 4)
			os << "  ";

		os << "R" << dec << i << ": ";
		if (i < 10)
			os << " ";
		os << setfill('0') << setw(4) << hex << uppercase << _cpu->_cpu_regs.R[i];
		if (i % 4 == 3)
			os << endl;
	}

	os << endl << endl;

	return os;
}
