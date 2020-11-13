#ifndef _CPTOYC_LEXER_H
#define _CPTOYC_LEXER_H
#include <string>
#include <slist>

namespace CPToyC
{
	namespace Compiler
	{
		using std::string;
		using std::list;

		enum class TokenKind
		{
			Unknow,
			Identifier,
			Keyword,
			Punctuator,
			NumLiterial,
			StringLiterial
		};

		class Token
		{
		public:
			TokenKind Kind;
			long Var;
			string Content;

			Token() 
			{
				Kind = TokenKind::Unknow;
			}

			Token(TokenKind kind, const string & content) 
			{
				Kind = kind;
				Content = content;
			}
		};

		class Lexer
		{
			public:
			list<Token> tokenize(const string & filename);	
			private:
			string readfile(const string & filename);
		};
	}
}// CPToyC

#endif //_CPTOYC_LEXER_H