/**********************************
* File:     token.h
*
* Author:   caipeng
*
* Email:    iiicp@outlook.com
*
* Date:     2020/11/19
***********************************/

#ifndef CPTOYC_TOKEN_H
#define CPTOYC_TOKEN_H

#include <string>
#include <iostream>

namespace CPToyC {
    namespace Compiler {

        using std::string;

        enum class TokenKind {
            Begin,
            // Identifier
            ID,
            // keyword
            Auto, Break, Case, Char, Const, Continue, Default, Do,
            Double, Else, Enum, Extern, Float, For, Goto, If,
            Int, Long, Register, Return, Short, Signed, Sizeof, Static,
            Struct, Switch, Typedef, Union, Unsigned, Void, Volatile, While,
            // const literal
            IntLiteral, UIntLiteral, LongLiteral, ULongLiteral, LLongLiteral, ULLongLiteral,
            FloatLiteral, DoubleLiteral, LDoubleLiteral, StringLiteral, WStringLiteral,
            // operator
            Comma, Question, Colon, Assign, BitorAssign, BitXorAssign,
            BitAndAssign, LshAssign, RshAssign, AddAssign, SubAssign, MulAssign,
            DivAssign, ModAssign, LogicOr, LogicAnd, Bitor, BitXor, BitAnd,
            Equal, UnEqual, Great, Less, GreatEq, LessEq,
            LSH, RSH, Add, Sub, Mul, Div, Mod, Inc, Dec,
            LogicNot, BitNot, Dot, Pointer, LParent, RParent, LBracket, RBracket,
            // punctuation
            LBrace, RBrace, Semicolon, Ellipsis,
            End
        };

        class Token {
        public:
            TokenKind Kind;
            string Content;
            int Row, Col;
            int Base;

            Token() {
                Kind = TokenKind::Begin;
                Row = -1;
                Col = -1;
                Base = 10;
            }

            Token(TokenKind kind, const string & content, int row, int col, int base = 10) {
                Kind = kind;
                Content = content;
                Row = row;
                Col = col;
                Base = base;
            }

            string KindToStr();

            void ToString() {
                std::cout << "( " << Content << ", " << KindToStr() << ", row: " << Row << ", col: " << Col << " )" << std::endl;
            }
        };
    }
}
#endif //CPTOYC_TOKEN_H
