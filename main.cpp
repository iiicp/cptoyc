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

	std::list<Error> errList;
	list<shared_ptr<Token>> res = lexer.Tokenize(filename, errList);
	if (errList.size() > 0) {
        std::cout << "lexer..." << std::endl;
        for (auto &err : errList) {
	        std::cout << err.Filename << ": " << err.Msg << ", row = " << err.Row << ", col = " << err.Col << std::endl;
	    }
        return -1;
	}

	lexer.Dumplist(res);

	return 0;
}