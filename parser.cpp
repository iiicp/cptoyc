/**********************************
* File:     parser.cpp
*
* Author:   caipeng
*
* Email:    iiicp@outlook.com
*
* Date:     2020/11/19
***********************************/

#include "parser.h"

namespace CPToyC {
    namespace Compiler {

        std::list<std::shared_ptr<ExprNode>> Parser::Parse() {

        }

        std::shared_ptr<ExprNode> Parser::ParseExpression() {

        }
        std::shared_ptr<ExprNode> Parser::ParseConstantExpression() {

        }
        std::shared_ptr<ExprNode> Parser::ParseAssignmentExpression() {

        }
        std::shared_ptr<ExprNode> Parser::ParseConditionalExpression() {

        }
        std::shared_ptr<ExprNode> Parser::ParseLogicalOrExpression() {

        }
        std::shared_ptr<ExprNode> Parser::ParseLogicalAndExpression() {

        }
        std::shared_ptr<ExprNode> Parser::ParseBitOrExpression() {

        }
        std::shared_ptr<ExprNode> Parser::ParseBitXorExpression() {

        }
        std::shared_ptr<ExprNode> Parser::ParseBitAndExpression() {

        }
        std::shared_ptr<ExprNode> Parser::ParseEqualityExpression() {

        }
        std::shared_ptr<ExprNode> Parser::ParseRelationalExpression() {

        }
        std::shared_ptr<ExprNode> Parser::ParseShiftExpression() {

        }
        std::shared_ptr<ExprNode> Parser::ParseAdditiveExpression() {

        }
        std::shared_ptr<ExprNode> Parser::ParseMultiExpression() {

        }
        std::shared_ptr<ExprNode> Parser::ParseCastExpression() {

        }
        std::shared_ptr<ExprNode> Parser::ParseUnaryExpression() {

        }
        std::shared_ptr<ExprNode> Parser::ParsePostfixExpression() {

        }
        std::shared_ptr<ExprNode> Parser::ParsePrimaryExpression() {

        }

        std::shared_ptr<Token> Parser::GetNextToken() {
            if (Cursor != End) {
                Cursor++;
                return *Cursor;
            }else {
                std::string msg("No tokens left");
                ErrLists.emplace_back(Error(Filename, msg));
            }
        }

        std::shared_ptr<Token> Parser::PeekToken() {

        }

        void Parser::ExpectToken(TokenKind kind) {

        }
    }
}
