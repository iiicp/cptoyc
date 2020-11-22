/**********************************
* File:     token.cpp
*
* Author:   caipeng
*
* Email:    iiicp@outlook.com
*
* Date:     2020/11/19
***********************************/

#include "token.h"
#include <unordered_map>

namespace CPToyC {
    namespace Compiler {

        static std::unordered_map<TokenKind, const char *> tosHash = {
#define TOKEN(k, s) {TokenKind:##:k, s},
#include "token.h.in"
#undef TOKEN
        };



        static std::unordered_map<string, TokenKind> sotHash = {
                // string have hash implement
#define TOKEN(k, s) {s, TokenKind:##:k},
#include "token.h.in2"
#undef TOKEN
        };

        string KindToStr(TokenKind kind) {
            return tosHash[kind];
        }

        TokenKind StrToKind(string str) {
            if (sotHash.find(str.c_str()) != sotHash.end()) {
                return sotHash[str.c_str()];
            }
            return TokenKind::ID;
        }
    }
}
