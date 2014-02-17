
#pragma once

#include <string>
#include <map>

class Preprocessor
{
public:
	static std::string process(const std::string &text);

private:
	Preprocessor(const std::string &text);
	void preprocess_line(std::string &line);
	void parse_preprocessor_directive(const std::string &line);
	void replace_defines(std::string &line);

	std::string output;
	std::string text;

	std::map<std::string, std::string> defines;
};
