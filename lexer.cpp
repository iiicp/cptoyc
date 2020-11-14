#include "lexer.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <streambuf>

namespace CPToyC
{
	namespace Compiler
	{
		using std::ifstream;
		string Lexer::readfile(const string & filename) {
			ifstream is(filename);
			if (!is) {
				std::cout << "open " << filename << " failed" << std::endl;
				return "";
			}

			string str;
			is.seekg(0, std::ios::end);
			str.reserve(is.tellg());
			is.seekg(0, std::ios::beg);

			str.assign((std::istreambuf_iterator<char>(is)),
				std::istreambuf_iterator<char>());

			/*
				std::stringstream strstream;
				strstream << is.rdbuf();
				strstream.str();
			 */

			return str;
		}

        forward_list<Token> Lexer::tokenize(const string &filename) {
            forward_list<Token> tokenList;
			string str = readfile(filename);
			std::cout << str << std::endl;
			return tokenList;
		}
	}
}