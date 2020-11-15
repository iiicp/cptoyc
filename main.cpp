#include <iostream>
#include "lexer.h"
#include <string>
#include <exception>

using namespace CPToyC::Compiler;

int main(int argc, char *argv[])
{
	if (argc != 2) {
		std::cout << "./cptoyc filename" << std::endl;
		return 0;
	}

	std::string filename = argv[1];
	Lexer lexer;

    try {
        list<Token> res = lexer.Tokenize(filename);
        lexer.Dumplist(res);
    } catch (const char *msg) {
        std::cout << msg << std::endl;
    }

	return 0;
}