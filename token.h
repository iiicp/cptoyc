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
            BEGIN,
    #define TOKEN(k, s) k,
    #include "token.h.in"
    #undef TOKEN
        };

        struct TokenLoc {
            const char *Filename;
            int Row;
            int Col;
        };

        union TokenVal {
            long i;
            double d;
            float f;
            const char *p;
        };

        string KindToStr(TokenKind kind);
        TokenKind StrToKind(string str);

        class Token {
        public:
            TokenKind Kind;
            struct TokenLoc Loc;
            union TokenVal Val;

            Token(TokenKind kind) {
                Kind = kind;
            }

            void ToString() {
                if (Kind >= TokenKind::INTCONST && Kind <= TokenKind::ULLONGCONST) {
                    std::cout << "( " << KindToStr(Kind) << ", val: " << Val.i << ", row: " << Loc.Row << ", col: " << Loc.Col << " )" << std::endl;
                }else if (Kind == TokenKind::FLOATCONST) {
                    std::cout << "( " << KindToStr(Kind) << ", val: " << Val.f << ", row: " << Loc.Row << ", col: " << Loc.Col << " )" << std::endl;
                }else if (Kind == TokenKind::DOUBLECONST || Kind == TokenKind::LDOUBLECONST) {
                    std::cout << "( " << KindToStr(Kind) << ", val: " << Val.d << ", row: " << Loc.Row << ", col: " << Loc.Col << " )" << std::endl;
                }else {
                    std::cout << "( " << KindToStr(Kind) << ", val: " << Val.p << ", row: " << Loc.Row << ", col: " << Loc.Col << " )" << std::endl;
                }
            }
        };
    }
}
#endif //CPTOYC_TOKEN_H
