#include "emulator.h"


int main(int argc, char* argv[])
{
	if (argc == 3)
	{
		Emulator emul;

		emul.run(argv[1], argv[2]);
	}

	return 0;
}
