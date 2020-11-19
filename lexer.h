/**********************************
* File:     lexer.h
*
* Author:   caipeng
*
* Email:    iiicp@outlook.com
*
* Date:     2020/11/13
***********************************/

#ifndef _CPTOYC_LEXER_H
#define _CPTOYC_LEXER_H
#include <list>
#include <memory>
#include "token.h"
#include "error.h"

namespace CPToyC {
	namespace Compiler {

		using std::list;
		using std::shared_ptr;

		class Lexer {
			public:
			static list<shared_ptr<Token>> Tokenize(const string & filename, list<Error> & errlist);
			static void Dumplist(list<shared_ptr<Token>> &ls);
		};
	}
}// CPToyC

#endif //_CPTOYC_LEXER_H