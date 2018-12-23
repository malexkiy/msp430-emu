#ifndef _DEBUGGER_H_
#define _DEBUGGER_H_

#include <iostream>

class Cpu;

class Debugger
{
protected:
	Cpu *_cpu;

public:
	Debugger(Cpu* cpu);
	~Debugger();

	bool step(void) const;
	std::ostream& print_cpu_state(std::ostream &os);
};

#endif
