
#include "precomp.h"
#include "program.h"
#include "preprocessor.h"
#include <algorithm>

using namespace clan;

int Program::main(const std::vector<std::string> &args)
{
	try
	{
		std::string input = File::read_text(args[0]);
		std::string output = Preprocessor::process(input);

		if(args.size() > 1)
		{
			File::write_text(args[1], output);
		}
		else
		{
			std::cout << output;
		}

		return 0;
	}
	catch (Exception &e)
	{
		std::cout << e.message << std::endl;
		return 1;
	}
}
