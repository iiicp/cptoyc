/**********************************
* File:     lexer.cpp
*
* Author:   caipeng
*
* Email:    iiicp@outlook.com
*
* Date:     2020/11/13
***********************************/

#include "lexer.h"
#include <fstream>
#include <iostream>
#include <streambuf>
#include <cctype>
#include <string>
#include <unordered_map>

namespace CPToyC {
	namespace Compiler {

		using std::ifstream;
        using std::string;

		enum class State {
		    Start, Identifier, Space, Int, Double, Char, String, Operator,
		    SingleComment, MultiComment
		};

        bool IsLetter(char ch) {
            return ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || (ch == '_');
        }

        bool IsDigit(char ch) {
            return '0' <= ch && ch <= '9';
        }

        bool IsOctDigit(char ch) {
            return '0' <= ch && ch <= '7';
        }

        bool IsHexDigit(char ch) {
            return IsDigit(ch) || ('a' <= ch && ch <= 'f') || ('A' <= ch && ch <= 'Z');
        }

        bool IsLetterOrDigit(char ch) {
            return IsLetter(ch) || IsDigit(ch);
        }

        bool IsSpace(char ch) {
            return ch == ' ' || ch == '\t';
        }

        bool IsOperator(char ch) {
            return ch == '{' || ch == '}' || ch == '[' || ch == ']' || ch == '(' || ch == ')' ||
                   ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%' || ch == '.' ||
                   ch == '&' || ch == '!' || ch == '<' || ch == '>' || ch == '|' || ch == '^' ||
                   ch == '?' || ch == ',' || ch == ';' || ch == ':' || ch == '=' || ch == '~';
        }

        char GetEscapeChar(char ch) {
            switch (ch) {
                case 'a':
                    return '\a';
                case 'b':
                    return '\b';
                case 'r':
                    return '\r';
                case 'n':
                    return '\n';
                case 'f':
                    return '\f';
                case 'v':
                    return '\v';
                case '0':
                    return 0;
                default:
                    return ch;
            }
        }

		string ReadSourceFile(const string & filename) {
			ifstream is(filename);
			if (!is) {
				std::cout << "open " << filename << " failed" << std::endl;
				return "";
			}
            string str;

#if 1
			is.seekg(0, std::ios::end);
			str.reserve(is.tellg());
			is.seekg(0, std::ios::beg);

			str.assign((std::istreambuf_iterator<char>(is)),
				std::istreambuf_iterator<char>());
#else
            std::stringstream strstream;
            strstream << is.rdbuf();
            str = strstream.str();
#endif
            //std::cout << str.length() << std::endl;
			return str;
		}

		void Lexer::Dumplist(list<std::shared_ptr<Token>> &ls) {
            for (auto &token : ls) {
                token->ToString();
            }
        }

        list<std::shared_ptr<Token>> Lexer::Tokenize(const string &filename, list<Error> &errlist) {

            list<std::shared_ptr<Token>> tokenList;
			string s = ReadSourceFile(filename);
			//std::cout << str << std::endl;
			int i = 0, len = s.size();
            int row = 1, col = 1;

            auto IndexLegal = [&](int i) {
                return i < len;
            };

            // [begin, end]
            auto InsertToken = [&](TokenKind kind, string &val, int begin, int end, int &col, int &cursor, int base = 10) {
                int tCol = col;
                for (int i = begin; i <= end; ++i) {
                    val += s[i];
                    cursor++;
                    col++;
                }

                // ID or Keyword
                if (kind == TokenKind::ID) {
                    kind = StrToKind(val);
                }

                std::shared_ptr<Token> token = std::make_shared<Token>(kind);
                token->Loc = {filename.c_str(), row, tCol};
                if (kind >= TokenKind::INTCONST && kind <= TokenKind::ULLONGCONST) {
                    token->Val.i = strtol(val.c_str(), nullptr, base);
                }else if (kind == TokenKind::FLOATCONST) {
                    token->Val.f = strtof(val.c_str(), nullptr);
                }else if (kind == TokenKind::DOUBLECONST || kind == TokenKind::LDOUBLECONST) {
                    token->Val.d = strtod(val.c_str(), nullptr);
                }else {
                    // need alloc memory
                    token->Val.p = (new string(val))->c_str();
                }
                tokenList.push_back(token);
            };

            /**
             * integer-constant:
             *       decimal-constant integer-suffix(opt)
             *       octal-constant integer-suffix(opt)
             *       hexadecimal-constant integer-suffix(opt)
             *
             *   decimal-constant:
             *       nonzero-digit
             *       decimal-constant digit
             *
             *   octal-constant:
             *       0
             *       octal-constant octal-digit
             *
             *   hexadecimal-constant:
             *       hexadecimal-prefix hexadecimal-digit
             *       hexadecimal-constant hexadecimal-digit
             *
             *   hexadecimal-prefix: one of
             *       0x 0X
             *
             *   nonzero-digit: one of
             *       1 2 3 4 5 6 7 8 9
             *
             *   octal-digit: one of
             *       0 1 2 3 4 5 6 7
             *
             *   hexadecimal-digit: one of
             *       0 1 2 3 4 5 6 7 8 9 a b c d e f A B C D E F
             *
             *   integer-suffix:
             *       unsigned-suffix long-suffix(opt)
             *       unsigned-suffix long-long-suffix
             *       long-suffix unsigned-suffix(opt)
             *       long-long-suffix unsigned-suffixopt
             *
             *   unsigned-suffix: one of
             *       u U
             *
             *   long-suffix: one of
             *       l L
             *   long-long-suffix: one of
             *       l l L L
             */
            // false -> should parse float
            auto ParseInt = [&](string &val, int &i, int &col) -> bool {
                int begin = i, cur = i;
                int base;
                if (s[cur] == '0' && IndexLegal(cur + 1) && (s[cur + 1] == 'x'|| s[cur + 1] == 'X')) {
                    base = 16;
                    cur += 2;
                    while (IndexLegal(cur) && IsHexDigit(s[cur])) {
                        cur++;
                    }
                }else if (s[cur] == '0') {
                    base = 8;
                    while (IndexLegal(cur) && IsOctDigit(s[cur])) {
                        cur++;
                    }
                }else {
                    base = 10;
                    while (IndexLegal(cur) && IsDigit(s[cur])) {
                        cur++;
                    }
                }
                if (IndexLegal(cur) && (s[cur] == '.' || s[cur] == 'e' || s[cur] == 'E' || s[cur] == 'p' || s[cur] == 'P'))  {
                    return false;
                }

                if (IndexLegal(cur) && (s[cur] == 'u' || s[cur] == 'U')) {
                    cur++;
                    if (IndexLegal(cur) && (s[cur] == 'l' || s[cur] == 'L')) {
                        cur++;
                        if (IndexLegal(cur) && (s[cur] == 'l' || s[cur] == 'L')) {
                            cur++;
                            InsertToken(TokenKind::ULLONGCONST, val, begin, cur - 1, col, i, base);
                        }else {
                            InsertToken(TokenKind::ULONGCONST, val, begin, cur - 1, col, i, base);
                        }
                    }else {
                        InsertToken(TokenKind::UINTCONST, val, begin, cur - 1, col, i, base);
                    }
                }else {
                    if (IndexLegal(cur) && (s[cur] == 'l' || s[cur] == 'L')) {
                        cur++;
                        if (IndexLegal(cur) && (s[cur] == 'l' || s[cur] == 'L')) {
                            cur++;
                            InsertToken(TokenKind::LLONGCONST, val, begin, cur - 1, col, i, base);
                        }else {
                            InsertToken(TokenKind::LONGCONST, val, begin, cur - 1, col, i, base);
                        }
                    }else {
                        InsertToken(TokenKind::INTCONST, val, begin, cur - 1, col, i, base);
                    }
                }

                return true;
            };

            auto ParseFloatWithBase = [&](string &val, int &i, int &col, bool startDot, int base) {
                int begin = i, cur = i;

                if (base == 16) {
                    cur += 2;
                }

                if (startDot) {
                    if (IndexLegal(cur) && s[cur] == '.') {
                        cur++;
                    }
                }

                while (IndexLegal(cur) && (base == 10 ? IsDigit(s[cur]) : IsHexDigit(s[cur]))) {
                    cur++;
                }

                if (!startDot) {
                    if (IndexLegal(cur) && s[cur] == '.') {
                        cur++;
                    }
                }
                char c1 = (base == 10) ? 'e' : 'p';
                char c2 = (base == 10) ? 'E' : 'P';
                if (IndexLegal(cur) && (s[cur] == c1 || s[cur] == c2)) {
                    cur++;
                    if (IndexLegal(cur) && (s[cur] == '+' || s[cur] == '-')) {
                        cur++;
                    }
                    while (IndexLegal(cur) && IsDigit(s[cur])) {
                        cur++;
                    }
                }
                if (IndexLegal(cur) && (s[cur] == 'f' || s[cur] == 'F')) {
                    cur++;
                    InsertToken(TokenKind::FLOATCONST, val, begin, cur-1, i, base);
                }else if (IndexLegal(cur) && (s[cur] == 'l' || s[cur] == 'L')) {
                    cur++;
                    InsertToken(TokenKind::LDOUBLECONST, val, begin, cur-1, i, base);
                }else {
                    InsertToken(TokenKind::DOUBLECONST, val, begin, cur-1, i, base);
                }
            };

            auto ParseFloat = [&](string &val, int &i, int &col) {
                if (IndexLegal(i) && s[i] == '.') {
                    ParseFloatWithBase(val, i, col, true, 10);
                }else {
                    if (s[i] == '0' && IndexLegal(i+1) && (s[i+1] == 'x' || s[i+1] == 'X')) {
                        bool startDot = false;
                        if (IndexLegal(i + 2) && s[i+2] == '.') {
                            startDot = true;
                        }
                        ParseFloatWithBase(val, i, col, startDot, 16);
                    }else {
                        ParseFloatWithBase(val, i, col, false, 10);
                    }
                }
            };

            auto ParseChar = [&](string &val, int &i, int &col) {
                int tCol = col;
                i++;    //eat '
                tCol++;
                if (s[i] == '\0') {
                    std::string msg = std::string("unclosed char literal: ch = ") + s[i];
                    errlist.emplace_back(Error(filename, msg, row, col));
                    i++;
                    col = tCol;
                    return;
                }
                char c;
                if (s[i] == '\\') {
                    i++;
                    tCol++;
                    c = GetEscapeChar(s[i++]);
                    tCol++;
                }else {
                    c = s[i++];
                    tCol++;
                }
                if (s[i] != '\'') {
                    std::string msg = std::string("char literal too long: ch = ") + s[i];
                    errlist.emplace_back(Error(filename, msg, row, col));
                    i++;
                    col = tCol;
                    return;
                }
                i++;        //eat '
                tCol++;

                val += c;
                auto token = std::make_shared<Token>(TokenKind::INTCONST);
                token->Loc = {filename.c_str(), row, col};
                token->Val.i = c;
                tokenList.push_back(token);
                col = tCol;
            };

            auto ParseString = [&](string &val, int &i, int &col) {
                int tCol = col;
                int p = i + 1;
                tCol++;
                while (IndexLegal(p) && s[p] != '"') {
                    char c;
                    if (s[p] == '\\') {
                        p++;
                        tCol++;
                        c = GetEscapeChar(s[p++]);
                        tCol++;
                        val += c;
                    }else if (s[p] == '\n') {
                        break;
                    }else {
                        c = s[p++];
                        val += c;
                        tCol++;
                    }
                }
                if (p >= len || s[p] == '\n') {
                    std::string msg = std::string("unclosed string literal: val = ") + val;
                    errlist.emplace_back(Error(filename, msg, row, col));
                    i = p;
                    col = tCol;
                    return;
                }else {
                    auto token = std::make_shared<Token>(TokenKind::STRING);
                    token->Loc = {filename.c_str(), row, col};
                    token->Val.p = val.c_str();
                    tokenList.push_back(token);
                    p++; // eat rhs "
                    i = p;
                    tCol++;
                    col = tCol;
                    return;
                }
            };

			State state = State::Start;

			while (i < len) {
                switch (state) {
                    case State::Start: {
                        if (s[i] == '\n') {
                            i++; row++; col = 1;
                        }else if (s[i] == '\r') {
                            i++; col++;
                            if (IndexLegal(i) && s[i] == '\n') {
                                i++; row++; col = 1;
                            }
                        }else if (IsSpace(s[i])) {
                            state = State::Space;
                        }else if (IsLetter(s[i])) {
                            state = State::Identifier;
                        }else if (IsDigit(s[i])) {
                            state = State::Int;
                        }else if (s[i] == '\'') {
                            state = State::Char;
                        }else if (s[i] == '"') {
                            state = State::String;
                        }else if (s[i] == '.' && IndexLegal(i + 1) && IsDigit(s[i + 1])) {
                            state = State::Double;
                        }else if (s[i] == '/' && IndexLegal(i + 1) && s[i + 1] == '/') {
                            state = State::SingleComment;
                        }else if (s[i] == '/' && IndexLegal(i + 1) && s[i + 1] == '*') {
                            state = State::MultiComment;
                        }else if (IsOperator(s[i])) {
                            state = State::Operator;
                        }else {
                            std::string msg = std::string("not support state: ch = ") + s[i];
                            errlist.emplace_back(Error(filename, msg, row, col));
                            i++;
                        }
                        break;
                    }
                    case State::Space: {
                        while (IndexLegal(i) && isspace(s[i])) {
                            i++; col++;
                        }
                        state = State::Start;
                        break;
                    }
                    case State::Identifier: {
                        string val;
                        int begin = i, cur = i;
                        while (IndexLegal(cur) && IsLetterOrDigit(s[cur])) {
                            cur++;
                        }
                        InsertToken(TokenKind::ID, val, begin, cur - 1, col, i);
                        state = State::Start;
                        break;
                    }
                    case State::Int: { // 0x123, 0123, 123ull
                        string val;
                        if (ParseInt(val, i, col)) {
                            state = State::Start;
                        }else {
                            state = State::Double;
                        }
                        break;
                    }
                    case State::Double: {
                        string val;
                        ParseFloat(val, i, col);
                        state = State::Start;
                        break;
                    }
                    case State::Char: {
                        string val;
                        ParseChar(val, i, col);
                        state = State::Start;
                        break;
                    }
                    case State::String: {
                        string val;
                        ParseString(val, i, col);
                        state = State::Start;
                        break;
                    }
                    case State::SingleComment: {
                        while (i < len && s[i] != '\n') {
                            i++; col++;
                        }
                        state = State::Start;
                        break;
                    }
                    case State::MultiComment: {
                        i += 2;
                        col += 2;
                        while (IndexLegal(i) && s[i] != '*' && IndexLegal(i+1) && s[i + 1] != '/') {
                            i++; col++;
                        }
                        if (i == len) {
                            std::string msg = std::string("unclosed multicomment: ch = ") + s[i];
                            errlist.emplace_back(Error(filename, msg, row, col));
                            state = State::Start;
                            break;
                        } else {
                            i += 2;
                            col += 2;
                            state = State::Start;
                        }
                        break;
                    }
                    case State::Operator: {
                        string val;
                        switch (s[i]) {
                            case '{':
                            {
                                int begin = i, end = i;
                                InsertToken(TokenKind::LBRACE, val, begin, end, col, i);
                                state = State::Start;
                                break;
                            }
                            case '}':
                            {
                                int begin = i, end = i;
                                InsertToken(TokenKind::RBRACE, val, begin, end, col, i);
                                state = State::Start;
                                break;
                            }
                            case '[':
                            {
                                int begin = i, end = i;
                                InsertToken(TokenKind::LBRACKET, val, begin, end, col, i);
                                state = State::Start;
                                break;
                            }
                            case ']':
                            {
                                int begin = i, end = i;
                                InsertToken(TokenKind::RBRACKET, val, begin, end, col, i);
                                state = State::Start;
                                break;
                            }
                            case '(':
                            {
                                int begin = i, end = i;
                                InsertToken(TokenKind::LPAREN, val, begin, end, col, i);
                                state = State::Start;
                                break;
                            }
                            case ')':
                            {
                                int begin = i, end = i;
                                InsertToken(TokenKind::RPAREN, val, begin, end, col, i);
                                state = State::Start;
                                break;
                            }
                            case ';':
                            {
                                int begin = i, end = i;
                                InsertToken(TokenKind::SEMICOLON, val, begin, end, col, i);
                                state = State::Start;
                                break;
                            }
                            case ':':
                            {
                                int begin = i, end = i;
                                InsertToken(TokenKind::COLON, val, begin, end, col, i);
                                state = State::Start;
                                break;
                            }
                            case ',':
                            {
                                int begin = i, end = i;
                                InsertToken(TokenKind::COMMA, val, begin, end, col, i);
                                state = State::Start;
                                break;
                            }
                            case '~':
                            {
                                int begin = i, end = i;
                                InsertToken(TokenKind::BITNOT, val, begin, end, col, i);
                                state = State::Start;
                                break;
                            }
                            case '?':
                            {
                                int begin = i, end = i;
                                InsertToken(TokenKind::QUESTION, val, begin, end, col, i);
                                state = State::Start;
                                break;
                            }
                            case '.':
                            {
                                int begin = i, end = i;
                                if (IndexLegal(i+1) && s[i+1] == '.' && IndexLegal(i+2) && s[i+2]) {
                                    end += 2;
                                    InsertToken(TokenKind::ELLIPSE, val, begin, end, col, i);
                                }else {
                                    InsertToken(TokenKind::DOT, val, begin, end, col, i);
                                }
                                state = State::Start;
                                break;
                            }
                            case '+':
                            {
                                int begin = i, end = i;
                                if (IndexLegal(i+1) && s[i+1] == '+') {
                                    end++;
                                    InsertToken(TokenKind::INC, val, begin, end, col, i);
                                }else if (IndexLegal(i+1) && s[i+1] == '=') {
                                    end++;
                                    InsertToken(TokenKind::ADD_ASSIGN, val, begin, end, col, i);
                                }else {
                                    InsertToken(TokenKind::ADD, val, begin, end, col, i);
                                }
                                state = State::Start;
                                break;
                            }
                            case '-':
                            {
                                int begin = i, end = i;
                                if (IndexLegal(i+1) && s[i+1] == '-') {
                                    end++;
                                    InsertToken(TokenKind::DEC, val, begin, end, col, i);
                                }else if (IndexLegal(i+1) && s[i+1] == '=') {
                                    end++;
                                    InsertToken(TokenKind::SUB_ASSIGN, val, begin, end, col, i);
                                }else if (IndexLegal(i+1) && s[i+1] == '>') {
                                    end++;
                                    InsertToken(TokenKind::POINTER, val, begin, end, col, i);
                                }else {
                                    InsertToken(TokenKind::SUB, val, begin, end, col, i);
                                }
                                state = State::Start;
                                break;
                            }
                            case '*':
                            {
                                int begin = i, end = i;
                                if (IndexLegal(i+1) && s[i+1] == '=') {
                                    end++;
                                    InsertToken(TokenKind::MUL_ASSIGN, val, begin, end, col, i);
                                }else {
                                    InsertToken(TokenKind::MUL, val, begin, end, col, i);
                                }
                                state = State::Start;
                                break;
                            }
                            case '/':
                            {
                                int begin = i, end = i;
                                if (IndexLegal(i+1) && s[i+1] == '=') {
                                    end++;
                                    InsertToken(TokenKind::DIV_ASSIGN, val, begin, end, col, i);
                                }else {
                                    InsertToken(TokenKind::DIV, val, begin, end, col, i);
                                }
                                state = State::Start;
                                break;
                            }
                            case '%':
                            {
                                int begin = i, end = i;
                                if (IndexLegal(i+1) && s[i+1] == '=') {
                                    end++;
                                    InsertToken(TokenKind::MOD_ASSIGN, val, begin, end, col, i);
                                }else {
                                    InsertToken(TokenKind::MOD, val, begin, end, col, i);
                                }
                                state = State::Start;
                                break;
                            }
                            case '=':
                            {
                                int begin = i, end = i;
                                if (IndexLegal(i+1) && s[i+1] == '=') {
                                    end++;
                                    InsertToken(TokenKind::EQUAL, val, begin, end, col, i);
                                }else {
                                    InsertToken(TokenKind::ASSIGN, val, begin, end, col, i);
                                }
                                state = State::Start;
                                break;
                            }
                            case '!':
                            {
                                int begin = i, end = i;
                                if (IndexLegal(i+1) && s[i+1] == '=') {
                                    end++;
                                    InsertToken(TokenKind::UNEQUAL, val, begin, end, col, i);
                                }else {
                                    InsertToken(TokenKind::NOT, val, begin, end, col, i);
                                }
                                state = State::Start;
                                break;
                            }
                            case '^':
                            {
                                int begin = i, end = i;
                                if (IndexLegal(i+1) && s[i+1] == '=') {
                                    end++;
                                    InsertToken(TokenKind::BITXOR_ASSIGN, val, begin, end, col, i);
                                }else {
                                    InsertToken(TokenKind::BITXOR, val, begin, end, col, i);
                                }
                                state = State::Start;
                                break;
                            }
                            case '<':
                            {
                                int begin = i, end = i;
                                if (IndexLegal(i+1) && s[i+1] == '<') {
                                    if (IndexLegal(i+2) && s[i+2] == '=') {
                                        end+=2;
                                        InsertToken(TokenKind::LSHIFT_ASSIGN, val, begin, end, col, i);
                                    }else {
                                        end++;
                                        InsertToken(TokenKind::LSHIFT, val, begin, end, col, i);
                                    }
                                }else if (IndexLegal(i+1) && s[i+1] == '=') {
                                    end++;
                                    InsertToken(TokenKind::LESS_EQ, val, begin, end, col, i);
                                }else {
                                    InsertToken(TokenKind::LESS, val, begin, end, col, i);
                                }
                                state = State::Start;
                                break;
                            }
                            case '>':
                            {
                                int begin = i, end = i;
                                if (IndexLegal(i+1) && s[i+1] == '>') {
                                    if (IndexLegal(i+2) && s[i+2] == '=') {
                                        end+=2;
                                        InsertToken(TokenKind::RSHIFT_ASSIGN, val, begin, end, col, i);
                                    }else {
                                        InsertToken(TokenKind::RSHIFT, val, begin, end, col, i);
                                    }
                                }else if (IndexLegal(i+1) && s[i+1] == '=') {
                                    end++;
                                    InsertToken(TokenKind::GREAT_EQ, val, begin, end, col, i);
                                }else {
                                    InsertToken(TokenKind::GREAT, val, begin, end, col, i);
                                }
                                state = State::Start;
                                break;
                            }
                            case '&':
                            {
                                int begin = i, end = i;
                                if (IndexLegal(i+1) && s[i+1] == '&') {
                                    end++;
                                    InsertToken(TokenKind::AND, val, begin, end, col, i);
                                }else if (IndexLegal(i+1) && s[i+1] == '=') {
                                    end++;
                                    InsertToken(TokenKind::BITAND_ASSIGN, val, begin, end, col, i);
                                }else {
                                    InsertToken(TokenKind::BITAND, val, begin, end, col, i);
                                }
                                state = State::Start;
                                break;
                            }
                            case '|':
                            {
                                int begin = i, end = i;
                                if (IndexLegal(i+1) && s[i+1] == '|') {
                                    end++;
                                    InsertToken(TokenKind::OR, val, begin, end, col, i);
                                }else if (IndexLegal(i+1) && s[i+1] == '=') {
                                    end++;
                                    InsertToken(TokenKind::BITOR_ASSIGN, val, begin, end, col, i);
                                }else {
                                    InsertToken(TokenKind::BITOR, val, begin, end, col, i);
                                }
                                state = State::Start;
                                break;
                            }
                            default:
                                break;
                        }
                    }
                        break;
                }
			}
			return tokenList;
		}
	}
}