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
#include <unordered_map>
#include <iostream>

namespace CPToyC {
    namespace Compiler {

        std::list<std::shared_ptr<ExprNode>> Parser::Parse() {

        }

        /**
         *  expression:
         *      assignment-expression
         *      expression, assignment-expression
         */
        std::shared_ptr<ExprNode> Parser::ParseExpression() {
            std::shared_ptr<ExprNode> expr = ParseAssignmentExpression();
            std::shared_ptr<ExprNode> commaExpr;
            while (CurrentTokenKind() == TokenKind::Comma) {
                auto token = CurrentToken();
                NextToken();
                commaExpr = std::make_shared<BinaryExprNode>(NodeKind::Comma, token, expr, ParseAssignmentExpression());
                expr = commaExpr;
            }
            return expr;
        }

        std::shared_ptr<ExprNode> Parser::ParseConstantExpression() {

        }

        /*
         *  assignment-expression:
         *      conditional-expression
         *      unary-expression assignment-operator assignment-expression
         *      assignment-operator: one of
         *      =  *=  /=  %=  +=  -=  <<=  >>=  &=  ^=  |=
         */
        std::shared_ptr<ExprNode> Parser::ParseAssignmentExpression() {
            std::shared_ptr<ExprNode> expr = ParseConditionalExpression();
            std::shared_ptr<ExprNode> op;
            if (CurrentTokenKind() >= TokenKind::Assign && CurrentTokenKind() <= TokenKind::ModAssign) {
                auto token = CurrentToken();
                NextToken();
                op = std::make_shared<BinaryExprNode>(TokenKindToNodeKind(token->Kind), token, expr, ParseAssignmentExpression());
                expr = op;
            }
            return expr;
        }

        /*
         *  conditional-expression:
         *      logical-OR-expression
         *      logical-OR-expression ? expression : conditional-expression
         */
        std::shared_ptr<ExprNode> Parser::ParseConditionalExpression() {
            std::shared_ptr<ExprNode> expr = ParseLogicalOrExpression();
            if (CurrentTokenKind() == TokenKind::Question) {
                std::shared_ptr<ExprNode> ternary, then;
                auto token = CurrentToken();
                NextToken();
                then = ParseExpression();
                ExpectToken(TokenKind::Colon);
                ternary = std::make_shared<TernaryExprNode>(NodeKind::Ternary, token, expr, then, ParseConditionalExpression());
                return ternary;
            }
            return expr;
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

        std::shared_ptr<Token> Parser::NextToken() {
            if (Cursor != End) {
                Cursor++;
                return *Cursor;
            }else {
                std::string msg("No tokens left");
                ErrLists.emplace_back(Error(Filename, msg));
            }
        }

        std::shared_ptr<Token> Parser::CurrentToken() {
            return *Cursor;
        }

        TokenKind Parser::CurrentTokenKind() {
            std::shared_ptr<Token> token = *Cursor;
            return token->Kind;
        }

        std::shared_ptr<Token> Parser::PeekToken() {

        }

        void Parser::ExpectToken(TokenKind kind) {
            if (CurrentTokenKind() != kind) {
                std::cerr << "Expect: " << Token::KindToStr(CurrentTokenKind()) << ", but get " << Token::KindToStr(kind) << std::endl;
                exit(-1);
            }
            NextToken();
        }

        NodeKind Parser::TokenKindToNodeKind(TokenKind kind) {
            static std::unordered_map<TokenKind, NodeKind> hash = {
                    {TokenKind::Assign, NodeKind::Assign}, {TokenKind::AddAssign, NodeKind::AddAssign}, {TokenKind::SubAssign, NodeKind::SubAssign},
                    {TokenKind::MulAssign, NodeKind::MulAssign}, {TokenKind::DivAssign, NodeKind::DivAssign} ,{TokenKind::ModAssign, NodeKind::ModAssign}
            };
            if (hash.count(kind)) {
                return hash[kind];
            }else {
                std::cerr << "error.. TokenKindToNodeKind" << std::endl;
                exit(-1);
            }
        }
    }
}
