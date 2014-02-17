
#include "precomp.h"
#include "program.h"

using namespace clan;

int main(int argc, char **argv)
{
	std::vector<std::string> args;
	for (int i = 1; i < argc; i++)
		args.push_back(argv[i]);

	if (args.size() == 0)
	{
		std::cout << "Syntax: " << argv[0] << " <input filename> (<output filename>)" << std::endl;
		return 1;
	}

	return Program::main(args);
}
