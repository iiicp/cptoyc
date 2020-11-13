#include <iostream>
#include "lexer.h"
#include <string>

using namespace CPToyC::Compiler;

int main(int argc, char *argv[])
{
	if (argc != 2) {
		std::cout << "./cptoyc filename" << std::endl;
		return 0;
	}

	std::string filename = argv[1];
	Lexer lexer;

	lexer.tokenize(filename);

	return 0;
}