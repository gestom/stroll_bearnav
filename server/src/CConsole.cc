#include <cstdlib>
#include <iostream>

#include "CConsole.h"

Console * Console::myself = NULL;

Console::Console() { }

Console::~Console() { }

Console * Console::instance() {
	if (myself == NULL)
		myself = new Console();
	
	return myself;
}

std::ostream & Console::colorMsg(const std::string & lbl, unsigned int color) {
	std::cout << "\033[1;" << color << "m" << lbl << "\033[0m ";
	return std::cout;
}

std::ostream & Console::colorErr(const std::string & lbl, const std::string & file,
		unsigned int line, unsigned int color) {
	int index = file.find_last_of("/") + 1;
	std::cerr << "\033[1;" << color << "m" << lbl << " [" <<
		file.substr(index, file.size() - index) << ":" << line << "]\033[0m ";
	return std::cerr;
}
