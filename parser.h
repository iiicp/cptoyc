/**********************************
* File:     parser.h
*
* Author:   caipeng
*
* Email:    iiicp@outlook.com
*
* Date:     2020/11/19
***********************************/

#ifndef CPTOYC_PARSER_H
#define CPTOYC_PARSER_H

#include "ast.h"
#include "error.h"

namespace CPToyC {
    namespace Compiler {
        class Parser {
        private:
            std::list<std::shared_ptr<Token>> &Tokens;
            std::list<Error> &ErrLists;
            std::string Filename;
            std::list<std::shared_ptr<Token>>::iterator Cursor;
            std::list<std::shared_ptr<Token>>::iterator End;
        public:
            Parser (std::list<std::shared_ptr<Token>> &tokens, std::list<Error> &errors, std::string filename)
                :Tokens(tokens), ErrLists(errors), Filename(filename), Cursor(tokens.begin()), End(tokens.end()) {}
            std::list<std::shared_ptr<ExprNode>> Parse();
        private:
            std::shared_ptr<Token> GetNextToken();
            std::shared_ptr<Token> PeekToken();
            void ExpectToken(TokenKind kind);

            std::shared_ptr<ExprNode> ParseExpression();
            std::shared_ptr<ExprNode> ParseConstantExpression();
            std::shared_ptr<ExprNode> ParseAssignmentExpression();
            std::shared_ptr<ExprNode> ParseConditionalExpression();
            std::shared_ptr<ExprNode> ParseLogicalOrExpression();
            std::shared_ptr<ExprNode> ParseLogicalAndExpression();
            std::shared_ptr<ExprNode> ParseBitOrExpression();
            std::shared_ptr<ExprNode> ParseBitXorExpression();
            std::shared_ptr<ExprNode> ParseBitAndExpression();
            std::shared_ptr<ExprNode> ParseEqualityExpression();
            std::shared_ptr<ExprNode> ParseRelationalExpression();
            std::shared_ptr<ExprNode> ParseShiftExpression();
            std::shared_ptr<ExprNode> ParseAdditiveExpression();
            std::shared_ptr<ExprNode> ParseMultiExpression();
            std::shared_ptr<ExprNode> ParseCastExpression();
            std::shared_ptr<ExprNode> ParseUnaryExpression();
            std::shared_ptr<ExprNode> ParsePostfixExpression();
            std::shared_ptr<ExprNode> ParsePrimaryExpression();
        };
    }
}

#endif //CPTOYC_PARSER_H
