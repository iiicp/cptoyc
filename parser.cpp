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
            std::shared_ptr<ExprNode> node = ParseAssignmentExpression();
            while (CurrentTokenKind() == TokenKind::COMMA) {
                auto token = CurrentToken();
                NextToken();
                node = std::make_shared<BinaryExprNode>(NodeKind::Comma, token, node, ParseAssignmentExpression());
            }
            return node;
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
         *
         *      assume  conditional-expression (assignment-operator assignment-expression) ?
         */
        std::shared_ptr<ExprNode> Parser::ParseAssignmentExpression() {
            std::shared_ptr<ExprNode> node = ParseConditionalExpression();
            if (CurrentTokenKind() >= TokenKind::ASSIGN && CurrentTokenKind() <= TokenKind::MOD_ASSIGN) {
                auto token = CurrentToken();
                NextToken();
                node = std::make_shared<BinaryExprNode>(TokenKindToNodeKind(token->Kind), token, node, ParseAssignmentExpression());
            }
            return node;
        }

        /**
         *  conditional-expression:
         *      logical-OR-expression
         *      logical-OR-expression ? expression : conditional-expression
         *
         *      logical-OR-expression (? expression : conditional-expression)?
         */
        std::shared_ptr<ExprNode> Parser::ParseConditionalExpression() {
            std::shared_ptr<ExprNode> node = ParseLogicalOrExpression();
            if (CurrentTokenKind() == TokenKind::QUESTION) {
                std::shared_ptr<ExprNode> ternary, then;
                auto token = CurrentToken();
                NextToken();
                then = ParseExpression();
                ExpectToken(TokenKind::COLON);
                ternary = std::make_shared<TernaryExprNode>(NodeKind::Ternary, token, node, then, ParseConditionalExpression());
                return ternary;
            }
            return node;
        }

        /**
         * logical-OR-expression:
         *   logical-AND-expression
         *   logical-OR-expression || logical-AND-expression
         *
         *   logical-AND-expression ("||" logical-AND-expression)*
         */
        std::shared_ptr<ExprNode> Parser::ParseLogicalOrExpression() {
            std::shared_ptr<ExprNode> node = ParseLogicalAndExpression();
            while (CurrentTokenKind() == TokenKind::OR) {
                auto token = CurrentToken();
                NextToken();
                node = std::make_shared<BinaryExprNode>(NodeKind::LogicalOr, token, node, ParseLogicalAndExpression());
            }
            return node;
        }

        /**
         *  logical-AND-expression:
         *      inclusive-OR-expression
         *      logical-AND-expression && inclusive-OR-expression
         *
         *      inclusive-OR-expression (&& inclusive-OR-expression)*
         */
        std::shared_ptr<ExprNode> Parser::ParseLogicalAndExpression() {
            std::shared_ptr<ExprNode> node = ParseBitOrExpression();
            while (CurrentTokenKind() == TokenKind::AND) {
                auto token = CurrentToken();
                NextToken();
                node = std::make_shared<BinaryExprNode>(NodeKind::LogicalAnd, token, node, ParseBitOrExpression());
            }
            return node;
        }

        /**
         *  inclusive-OR-expression:
         *      exclusive-OR-expression
         *      inclusive-OR-expression | exclusive-OR-expression
         *
         *      exclusive-OR-expression (| exclusive-OR-expression)*
         */
        std::shared_ptr<ExprNode> Parser::ParseBitOrExpression() {
            std::shared_ptr<ExprNode> node = ParseBitXorExpression();
            while (CurrentTokenKind() == TokenKind::BITOR) {
                auto token = CurrentToken();
                NextToken();
                node = std::make_shared<BinaryExprNode>(NodeKind::BitOr, token, node, ParseBitXorExpression());
            }
            return node;
        }

        /**
         *  exclusive-OR-expression:
         *      AND-expression
         *      exclusive-OR-expression ^ AND-expression
         *
         *      AND-expression (^ AND-expression)*
         */
        std::shared_ptr<ExprNode> Parser::ParseBitXorExpression() {
            std::shared_ptr<ExprNode> node = ParseBitAndExpression();
            while (CurrentTokenKind() == TokenKind::BITXOR) {
                auto token = CurrentToken();
                NextToken();
                node = std::make_shared<BinaryExprNode>(NodeKind::BitXor, token, node, ParseBitAndExpression());
            }
            return node;
        }

        /**
         *  AND-expression:
         *      equality-expression
         *      AND-expression & equality-expression
         *
         *      equality-expression (& equality-expression)*
         */
        std::shared_ptr<ExprNode> Parser::ParseBitAndExpression() {
            std::shared_ptr<ExprNode> node = ParseEqualityExpression();
            while (CurrentTokenKind() == TokenKind::BITAND) {
                auto token = CurrentToken();
                NextToken();
                node = std::make_shared<BinaryExprNode>(NodeKind::BitAnd, token, node, ParseEqualityExpression());
            }
            return node;
        }

        /**
         * equality-expression:
         *      relational-expression
         *      equality-expression == relational-expression
         *      equality-expression != relational-expression
         *
         *      relational-expression (== relational-expression | != relational-expression)*
         */
        std::shared_ptr<ExprNode> Parser::ParseEqualityExpression() {
            std::shared_ptr<ExprNode> node = ParseRelationalExpression();
            while (CurrentTokenKind() == TokenKind::EQUAL || CurrentTokenKind() == TokenKind::UNEQUAL) {
                auto token = CurrentToken();
                NextToken();
                node = std::make_shared<BinaryExprNode>(TokenKindToNodeKind(CurrentTokenKind()), token, node, ParseRelationalExpression());
            }
            return node;
        }

        /**
         * relational-expression:
         *      shift-expression
         *      relational-expression < shift-expression
         *      relational-expression > shift-expression
         *      relational-expression <= shift-expression
         *      relational-expression >= shift-expression
         *
         *      shift-expression (< shift-expression | > shift-expression | <= shift-expression | >= shift-expression)*
         */
        std::shared_ptr<ExprNode> Parser::ParseRelationalExpression() {
            std::shared_ptr<ExprNode> node = ParseShiftExpression();
            while (CurrentTokenKind() >= TokenKind::GREAT && CurrentTokenKind() <= TokenKind::LESS_EQ) {
                auto token = CurrentToken();
                NextToken();
                node = std::make_shared<BinaryExprNode>(TokenKindToNodeKind(CurrentTokenKind()), token, node, ParseShiftExpression());
            }
            return node;
        }

        /**
         * shift-expression:
         *      additive-expression
         *      shift-expression << additive-expression
         *      shift-expression >> additive-expression
         *
         *      additive-expression (<< additive-expression | >> additive-expression)*
         */
        std::shared_ptr<ExprNode> Parser::ParseShiftExpression() {
            std::shared_ptr<ExprNode> node = ParseAdditiveExpression();
            while (CurrentTokenKind() == TokenKind::LSHIFT || CurrentTokenKind() == TokenKind::RSHIFT) {
                auto token = CurrentToken();
                NextToken();
                node = std::make_shared<BinaryExprNode>(TokenKindToNodeKind(CurrentTokenKind()), token, node, ParseAdditiveExpression());
            }
            return node;
        }

        /**
         * additive-expression:
         *      multiplicative-expression
         *      additive-expression + multiplicative-expression
         *      additive-expression - multiplicative-expression
         *
         *      multiplicative-expression (+ multiplicative-expression | - multiplicative-expression)*
         */
        std::shared_ptr<ExprNode> Parser::ParseAdditiveExpression() {
            std::shared_ptr<ExprNode> node = ParseMultiExpression();
            while (CurrentTokenKind() == TokenKind::ADD || CurrentTokenKind() == TokenKind::SUB) {
                auto token = CurrentToken();
                NextToken();
                node = std::make_shared<BinaryExprNode>(TokenKindToNodeKind(CurrentTokenKind()), token, node, ParseMultiExpression());
            }
            return node;
        }

        /**
         *  multiplicative-expression:
         *      cast-expression
         *      multiplicative-expression * cast-expression
         *      multiplicative-expression / cast-expression
         *      multiplicative-expression % cast-expression
         *
         *      cast-expression ("*" cast-expression | "/" cast-expression | "%" cast-expression)*
         */
        std::shared_ptr<ExprNode> Parser::ParseMultiExpression() {
            std::shared_ptr<ExprNode> node = ParseCastExpression();
            while (CurrentTokenKind() >= TokenKind::MUL && CurrentTokenKind() <= TokenKind::MOD) {
                auto token = CurrentToken();
                NextToken();
                node = std::make_shared<BinaryExprNode>(TokenKindToNodeKind(CurrentTokenKind()), token, node, ParseCastExpression());
            }
            return node;
        }

        /**
         *  cast-expression:
         *      unary-expression
         *      ( type-name ) cast-expression
         *
         *      (type-name) cast-expression | unary-expression
         */
        std::shared_ptr<ExprNode> Parser::ParseCastExpression() {

            if (CurrentTokenKind() == TokenKind::LPAREN) {
                ++Cursor;
                if (IsTypeName()) {
                    // todo
                    // parser type name
                }
                --Cursor;
            }
            return ParseUnaryExpression();
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
         *
         *      unary = (|"&"|"*"|"+"|"-"|"~"|"!")? cast-expression
         *              | (++ | --) unary-expression
         *              | postfix-expression
         */
        std::shared_ptr<ExprNode> Parser::ParseUnaryExpression() {
//            auto token = CurrentToken();
//            if (CurrentTokenKind() == TokenKind::ADD) {
//                NextToken();
//                return ParseCastExpression();
//            }else if (CurrentTokenKind() == TokenKind::SUB) {
//                NextToken();
//                std::string num = "0";
//                std::shared_ptr<Token> token = std::make_shared<Token>(TokenKind::INTCONST);
//                return std::make_shared<BinaryExprNode>(NodeKind::Sub, token, std::make_shared<NumNode>(NodeKind::IntNum,token), ParseCastExpression());
//            }else if (CurrentTokenKind() == TokenKind::MUL) {
//                NextToken();
//                return std::make_shared<UnaryExprNode>(NodeKind::Deref, token, ParseCastExpression());
//            }else if (CurrentTokenKind() == TokenKind::BITAND) {
//                NextToken();
//                return std::make_shared<UnaryExprNode>(NodeKind::Addr, token, ParseCastExpression());
//            }else if (CurrentTokenKind() == )
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

        bool Parser::PeekToken(TokenKind kind) {
            ++Cursor;
            if (Cursor != End) {
                std::shared_ptr<Token> token = *Cursor;
                if (token->Kind == kind) {
                    --Cursor;
                    return true;
                }
            }
            --Cursor;
            return false;
        }

        void Parser::ExpectToken(TokenKind kind) {
//            if (CurrentTokenKind() != kind) {
//                std::cerr << "Expect: " << Token::KindToStr(CurrentTokenKind()) << ", but get " << Token::KindToStr(kind) << std::endl;
//                exit(-1);
//            }
//            NextToken();
        }

        bool Parser::IsTypeName() {
            std::shared_ptr<Token> token = *Cursor;
            return (token->Kind == TokenKind::ID) ? IsTypedefName() : (token->Kind >= TokenKind::AUTO && token->Kind <= TokenKind::VOID);
        }

        bool Parser::IsTypedefName() {
            return false;
        }

        NodeKind Parser::TokenKindToNodeKind(TokenKind kind) {
//            static std::unordered_map<TokenKind, NodeKind> hash = {
//                    {TokenKind::Assign, NodeKind::Assign}, {TokenKind::AddAssign, NodeKind::AddAssign}, {TokenKind::SubAssign, NodeKind::SubAssign},
//                    {TokenKind::MulAssign, NodeKind::MulAssign}, {TokenKind::DivAssign, NodeKind::DivAssign} ,{TokenKind::ModAssign, NodeKind::ModAssign},
//                    {TokenKind::Equal, NodeKind::Eq}, {TokenKind::UnEqual, NodeKind::UnEq}, {TokenKind::Less, NodeKind::LT},
//                    {TokenKind::LessEq, NodeKind::LE}, {TokenKind::Great, NodeKind::GT}, {TokenKind::GreatEq, NodeKind::GE},
//                    {TokenKind::LSH, NodeKind::LSH}, {TokenKind::RSH, NodeKind::RSH}, {TokenKind::Add, NodeKind::Add},
//                    {TokenKind::Sub, NodeKind::Sub}, {TokenKind::Mul, NodeKind::Mul}, {TokenKind::Div, NodeKind::Div},
//                    {TokenKind::Mod, NodeKind::Mod}
//            };
//            if (hash.count(kind)) {
//                return hash[kind];
//            }else {
//                std::cerr << "error.. TokenKindToNodeKind" << std::endl;
//                exit(-1);
//            }
        }
    }
}
