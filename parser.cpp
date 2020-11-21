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

        /**
         *  constant-expression:
         *      conditional-expression
         */
        std::shared_ptr<ExprNode> Parser::ParseConstantExpression() {
            return ParseConditionalExpression();
        }

        /**
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

        /**
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

        /**
         * logical-OR-expression:
         *   logical-AND-expression
         *   logical-OR-expression || logical-AND-expression
         */
        std::shared_ptr<ExprNode> Parser::ParseLogicalOrExpression() {
            std::shared_ptr<ExprNode> expr = ParseLogicalAndExpression();
            std::shared_ptr<ExprNode> logicalOr;
            while (CurrentTokenKind() == TokenKind::LogicOr) {
                auto token = CurrentToken();
                NextToken();
                logicalOr = std::make_shared<BinaryExprNode>(NodeKind::LogicalOr, token, expr, ParseLogicalAndExpression());
                expr = logicalOr;
            }
            return expr;
        }

        /**
         *  logical-AND-expression:
         *      inclusive-OR-expression
         *      logical-AND-expression && inclusive-OR-expression
         */
        std::shared_ptr<ExprNode> Parser::ParseLogicalAndExpression() {
            std::shared_ptr<ExprNode> expr = ParseBitOrExpression();
            std::shared_ptr<ExprNode> logicalAnd;
            while (CurrentTokenKind() == TokenKind::LogicAnd) {
                auto token = CurrentToken();
                NextToken();
                logicalAnd = std::make_shared<BinaryExprNode>(NodeKind::LogicalAnd, token, expr, ParseBitOrExpression());
                expr = logicalAnd;
            }
            return expr;
        }

        /**
         *  inclusive-OR-expression:
         *      exclusive-OR-expression
         *      inclusive-OR-expression | exclusive-OR-expression
         */
        std::shared_ptr<ExprNode> Parser::ParseBitOrExpression() {
            std::shared_ptr<ExprNode> expr = ParseBitXorExpression();
            std::shared_ptr<ExprNode> bitOr;
            while (CurrentTokenKind() == TokenKind::Bitor) {
                auto token = CurrentToken();
                NextToken();
                bitOr = std::make_shared<BinaryExprNode>(NodeKind::BitOr, token, expr, ParseBitXorExpression());
                expr = bitOr;
            }
            return expr;
        }

        /**
         *  exclusive-OR-expression:
         *      AND-expression
         *      exclusive-OR-expression ^ AND-expression
         */
        std::shared_ptr<ExprNode> Parser::ParseBitXorExpression() {
            std::shared_ptr<ExprNode> expr = ParseBitAndExpression();
            std::shared_ptr<ExprNode> bitXor;
            while (CurrentTokenKind() == TokenKind::BitXor) {
                auto token = CurrentToken();
                NextToken();
                bitXor = std::make_shared<BinaryExprNode>(NodeKind::BitXor, token, expr, ParseBitAndExpression());
                expr = bitXor;
            }
            return expr;
        }

        /**
         *  AND-expression:
         *      equality-expression
         *      AND-expression & equality-expression
         */
        std::shared_ptr<ExprNode> Parser::ParseBitAndExpression() {
            std::shared_ptr<ExprNode> expr = ParseEqualityExpression();
            std::shared_ptr<ExprNode> bitAnd;
            while (CurrentTokenKind() == TokenKind::BitAnd) {
                auto token = CurrentToken();
                NextToken();
                bitAnd = std::make_shared<BinaryExprNode>(NodeKind::BitAnd, token, expr, ParseEqualityExpression());
                expr = bitAnd;
            }
            return expr;
        }

        /**
         * equality-expression:
         *      relational-expression
         *      equality-expression == relational-expression
         *      equality-expression != relational-expression
         */
        std::shared_ptr<ExprNode> Parser::ParseEqualityExpression() {
            std::shared_ptr<ExprNode> expr = ParseRelationalExpression();
            std::shared_ptr<ExprNode> eq;
            while (CurrentTokenKind() == TokenKind::Equal || CurrentTokenKind() == TokenKind::UnEqual) {
                auto token = CurrentToken();
                NextToken();
                eq = std::make_shared<BinaryExprNode>(TokenKindToNodeKind(CurrentTokenKind()), token, expr, ParseRelationalExpression());
                expr = eq;
            }
            return expr;
        }

        /**
         * relational-expression:
         *      shift-expression
         *      relational-expression < shift-expression
         *      relational-expression > shift-expression
         *      relational-expression <= shift-expression
         *      relational-expression >= shift-expression
         */
        std::shared_ptr<ExprNode> Parser::ParseRelationalExpression() {
            std::shared_ptr<ExprNode> expr = ParseShiftExpression();
            std::shared_ptr<ExprNode> op;
            while (CurrentTokenKind() == TokenKind::Less || CurrentTokenKind() == TokenKind::LessEq ||
                CurrentTokenKind() == TokenKind::Great || CurrentTokenKind() == TokenKind::GreatEq) {
                auto token = CurrentToken();
                NextToken();
                op = std::make_shared<BinaryExprNode>(TokenKindToNodeKind(CurrentTokenKind()), token, expr, ParseShiftExpression());
                expr = op;
            }
            return expr;
        }

        /**
         * shift-expression:
         *      additive-expression
         *      shift-expression << additive-expression
         *      shift-expression >> additive-expression
         */
        std::shared_ptr<ExprNode> Parser::ParseShiftExpression() {
            std::shared_ptr<ExprNode> expr = ParseAdditiveExpression();
            std::shared_ptr<ExprNode> op;
            while (CurrentTokenKind() == TokenKind::LSH || CurrentTokenKind() == TokenKind::RSH) {
                auto token = CurrentToken();
                NextToken();
                op = std::make_shared<BinaryExprNode>(TokenKindToNodeKind(CurrentTokenKind()), token, expr, ParseAdditiveExpression());
                expr = op;
            }
            return expr;
        }

        /**
         * additive-expression:
         *      multiplicative-expression
         *      additive-expression + multiplicative-expression
         *      additive-expression - multiplicative-expression
         */
        std::shared_ptr<ExprNode> Parser::ParseAdditiveExpression() {
            std::shared_ptr<ExprNode> expr = ParseMultiExpression();
            std::shared_ptr<ExprNode> op;
            while (CurrentTokenKind() == TokenKind::Add || CurrentTokenKind() == TokenKind::Sub) {
                auto token = CurrentToken();
                NextToken();
                op = std::make_shared<BinaryExprNode>(TokenKindToNodeKind(CurrentTokenKind()), token, expr, ParseMultiExpression());
                expr = op;
            }
            return expr;
        }

        /**
         *  multiplicative-expression:
         *      cast-expression
         *      multiplicative-expression * cast-expression
         *      multiplicative-expression / cast-expression
         *      multiplicative-expression % cast-expression
         */
        std::shared_ptr<ExprNode> Parser::ParseMultiExpression() {
            std::shared_ptr<ExprNode> expr = ParseCastExpression();
            std::shared_ptr<ExprNode> op;
            while (CurrentTokenKind() == TokenKind::Mul || CurrentTokenKind() == TokenKind::Div
            || CurrentTokenKind() == TokenKind::Mod) {
                auto token = CurrentToken();
                NextToken();
                op = std::make_shared<BinaryExprNode>(TokenKindToNodeKind(CurrentTokenKind()), token, expr, ParseCastExpression());
                expr = op;
            }
            return expr;
        }

        /**
         *  cast-expression:
         *      unary-expression
         *      ( type-name ) cast-expression
         */
        std::shared_ptr<ExprNode> Parser::ParseCastExpression() {
            std::shared_ptr<ExprNode> expr = ParseUnaryExpression();

        }

        /**
         * unary-expression:
         *      postfix-expression
         *      ++ unary-expression
         *      -- unary-expression
         *      unary-operator cast-expression
         *      sizeof unary-expression
         *      sizeof ( type-name )
         *      _Alignof ( type-name )
         *
         *  unary-operator: one of
         *      &*+-~!
         */
        std::shared_ptr<ExprNode> Parser::ParseUnaryExpression() {

        }

        /**
         * postfix-expression:
         *      primary-expression
         *      postfix-expression [expression]
         *      postfix-expression (argument-expression-list(opt) )
         *      postfix-expression . identifier
         *      postfix-expression -> identifier
         *      postfix-expression ++
         *      postfix-expression --
         *      ( type-name ) { initializer-list }
         *      ( type-name ) { initializer-list , }
         *
         * argument-expression-list:
         *      assignment-expression
         *      argument-expression-list , assignment-expression
         */
        std::shared_ptr<ExprNode> Parser::ParsePostfixExpression() {

        }

        /**
         * primary-expression:
         *      identifier
         *      constant
         *      string-literal
         *      ( expression )
         *
         */
        std::shared_ptr<ExprNode> Parser::ParsePrimaryExpression() {

        }

        void Parser::NextToken() {
            if (Cursor != End) {
                Cursor++;
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
                    {TokenKind::MulAssign, NodeKind::MulAssign}, {TokenKind::DivAssign, NodeKind::DivAssign} ,{TokenKind::ModAssign, NodeKind::ModAssign},
                    {TokenKind::Equal, NodeKind::Eq}, {TokenKind::UnEqual, NodeKind::UnEq}, {TokenKind::Less, NodeKind::LT},
                    {TokenKind::LessEq, NodeKind::LE}, {TokenKind::Great, NodeKind::GT}, {TokenKind::GreatEq, NodeKind::GE},
                    {TokenKind::LSH, NodeKind::LSH}, {TokenKind::RSH, NodeKind::RSH}, {TokenKind::Add, NodeKind::Add},
                    {TokenKind::Sub, NodeKind::Sub}, {TokenKind::Mul, NodeKind::Mul}, {TokenKind::Div, NodeKind::Div},
                    {TokenKind::Mod, NodeKind::Mod}
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
