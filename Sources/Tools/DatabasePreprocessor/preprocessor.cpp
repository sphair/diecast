
#include "precomp.h"
#include "preprocessor.h"

std::string Preprocessor::process(const std::string &text)
{
	Preprocessor processor(text);
	return processor.output;
}

Preprocessor::Preprocessor(const std::string &text)
	: text(text)
{
	size_t line_start = 0;
	while (true)
	{
		size_t line_end = text.find('\n', line_start);
		if (line_end == std::string::npos)
			line_end = text.length();
		else
			line_end++;

		std::string line = text.substr(line_start, line_end - line_start);
		preprocess_line(line);
		output += line;

		if (line_end == text.length())
			break;
		line_start = line_end;
	}
}

void Preprocessor::preprocess_line(std::string &line)
{
	size_t first_char_pos = line.find_first_not_of(" \t\r\n");
	if (first_char_pos == std::string::npos)
		return;

	if (line[first_char_pos] == '#')
	{
		parse_preprocessor_directive(line.substr(first_char_pos));
		line.clear();
	}
	else
	{
		replace_defines(line);
	}
}

void Preprocessor::parse_preprocessor_directive(const std::string &line)
{
	if (line.substr(0, 7) == "#define")
	{
		std::string identifier, value;

		size_t identifier_start = line.find_first_not_of(" \t\r\n", 7);
		if (identifier_start == std::string::npos)
			return;

		size_t identifier_end = line.find_first_of(" \t\r\n", identifier_start);
		if (identifier_end == std::string::npos)
		{
			identifier = line.substr(identifier_start);
			identifier_end = line.length();
		}
		else
		{
			identifier = line.substr(identifier_start, identifier_end - identifier_start);
		}

		size_t value_start = line.find_first_not_of(" \t\r\n", identifier_end);
		if (value_start != std::string::npos)
		{
			size_t value_end = line.find_first_of(" \t\r\n", value_start);
			if (value_end == std::string::npos)
				value_end = line.length();

			value = line.substr(value_start, value_end - value_start);
		}

		defines[identifier] = value;
	}
}

void Preprocessor::replace_defines(std::string &line)
{
	bool found_identifier = false;
	size_t identifier_start = 0;

	for (size_t i = 0; i < line.length(); )
	{
		if (!found_identifier)
		{
			if ((line[i] >= 'a' && line[i] <= 'z') || (line[i] >= 'A' && line[i] <= 'Z') || line[i] == '_')
			{
				identifier_start = i;
				found_identifier = true;
			}
		}
		else if (found_identifier)
		{
			if (!(line[i] >= 'a' && line[i] <= 'z') && !(line[i] >= 'A' && line[i] <= 'Z') && !(line[i] >= '0' && line[i] <= '9') && line[i] != '_')
			{
				found_identifier = false;
				std::string identifier = line.substr(identifier_start, i - identifier_start);
				auto it = defines.find(identifier);
				if (it != defines.end())
				{
					line = line.substr(0, identifier_start) + it->second + line.substr(i);
					i = identifier_start + it->second.length();
					continue;
				}
			}
		}

		i++;
	}
}
