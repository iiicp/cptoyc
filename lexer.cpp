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

        TokenKind GetTokenKind(const string &str) {
            static std::unordered_map<std::string, TokenKind> hash = {
                    {"auto",TokenKind::Auto}, {"extern", TokenKind::Extern}, {"register", TokenKind::Register},
                    {"static",TokenKind::Static},{"typedef", TokenKind::Typedef},{"const", TokenKind::Const},
                    {"volatile", TokenKind::Volatile},{"signed",TokenKind::Signed},{"unsigned", TokenKind::Unsigned},
                    {"short",TokenKind::Short},{"long",TokenKind::Long},{"char", TokenKind::Char},
                    {"int", TokenKind::Int},{"float", TokenKind::Float},{"double",TokenKind::Double},
                    {"enum",TokenKind::Enum},{"struct",TokenKind::Struct},{"union",TokenKind::Union},
                    {"void",TokenKind::Void},{"break",TokenKind::Break},{"case", TokenKind::Case},
                    {"continue",TokenKind::Continue},{"default",TokenKind::Default},{"do", TokenKind::Do},
                    {"else",TokenKind::Else},{"for",TokenKind::For},{"goto",TokenKind::Goto},
                    {"if", TokenKind::If},{"return",TokenKind::Return},{"switch", TokenKind::Switch},
                    {"while", TokenKind::While},{"sizeof", TokenKind::Sizeof}
            };

            if (hash.find(str) != hash.end()) {
                return hash[str];
            }

            return TokenKind::ID;
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
                if (kind == TokenKind::ID) {
                    kind = GetTokenKind(val);
                }
                tokenList.push_back(std::make_shared<Token>(kind, val, row, tCol, base));
            };

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
                            InsertToken(TokenKind::ULLongLiteral, val, begin, cur - 1, col, i, base);
                        }else {
                            InsertToken(TokenKind::ULongLiteral, val, begin, cur - 1, col, i, base);
                        }
                    }else {
                        InsertToken(TokenKind::UIntLiteral, val, begin, cur - 1, col, i, base);
                    }
                }else {
                    if (IndexLegal(cur) && (s[cur] == 'l' || s[cur] == 'L')) {
                        cur++;
                        if (IndexLegal(cur) && (s[cur] == 'l' || s[cur] == 'L')) {
                            cur++;
                            InsertToken(TokenKind::LLongLiteral, val, begin, cur - 1, col, i, base);
                        }else {
                            InsertToken(TokenKind::LongLiteral, val, begin, cur - 1, col, i, base);
                        }
                    }else {
                        InsertToken(TokenKind::IntLiteral, val, begin, cur - 1, col, i, base);
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
                    InsertToken(TokenKind::FloatLiteral, val, begin, cur-1, i, base);
                }else if (IndexLegal(cur) && (s[cur] == 'l' || s[cur] == 'L')) {
                    cur++;
                    InsertToken(TokenKind::LDoubleLiteral, val, begin, cur-1, i, base);
                }else {
                    InsertToken(TokenKind::DoubleLiteral, val, begin, cur-1, i, base);
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
                        int tCol = col;
                        i++;    //eat '
                        tCol++;
                        if (s[i] == '\0') {
                            std::string msg = std::string("unclosed char literal: ch = ") + s[i];
                            errlist.emplace_back(Error(filename, msg, row, col));
                            i++;
                            col = tCol;
                            state = State::Start;
                            break;
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
                            state = State::Start;
                            break;
                        }
                        i++;        //eat '
                        tCol++;

                        string val;
                        val += c;
                        tokenList.push_back(std::make_shared<Token>(TokenKind::IntLiteral, val, row, col));
                        col = tCol;
                        state = State::Start;
                        break;
                    }
                    case State::String: {
                        int tCol = col;
                        int p = i + 1;
                        string val;
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
                            state = State::Start;
                            break;
                        }else {
                            tokenList.push_back(std::make_shared<Token>(TokenKind::StringLiteral, val, row, col));
                            p++; // eat rhs "
                            i = p;
                            tCol++;
                            col = tCol;
                            state = State::Start;
                        }
                        break;
                    }
                    case State::SingleComment: {
                        while (i < len && s[i] != '\n') {
                            i++;
                            col++;
                        }
                        state = State::Start;
                        break;
                    }
                    case State::MultiComment: {
                        i += 2;
                        col += 2;
                        while (IndexLegal(i) && s[i] != '*' && IndexLegal(i+1) && s[i + 1] != '/') {
                            i++;
                            col++;
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
                                InsertToken(TokenKind::LBrace, val, begin, end, col, i);
                                state = State::Start;
                                break;
                            }
                            case '}':
                            {
                                int begin = i, end = i;
                                InsertToken(TokenKind::RBrace, val, begin, end, col, i);
                                state = State::Start;
                                break;
                            }
                            case '[':
                            {
                                int begin = i, end = i;
                                InsertToken(TokenKind::LBracket, val, begin, end, col, i);
                                state = State::Start;
                                break;
                            }
                            case ']':
                            {
                                int begin = i, end = i;
                                InsertToken(TokenKind::RBracket, val, begin, end, col, i);
                                state = State::Start;
                                break;
                            }
                            case '(':
                            {
                                int begin = i, end = i;
                                InsertToken(TokenKind::LParent, val, begin, end, col, i);
                                state = State::Start;
                                break;
                            }
                            case ')':
                            {
                                int begin = i, end = i;
                                InsertToken(TokenKind::RParent, val, begin, end, col, i);
                                state = State::Start;
                                break;
                            }
                            case ';':
                            {
                                int begin = i, end = i;
                                InsertToken(TokenKind::Semicolon, val, begin, end, col, i);
                                state = State::Start;
                                break;
                            }
                            case ':':
                            {
                                int begin = i, end = i;
                                InsertToken(TokenKind::Colon, val, begin, end, col, i);
                                state = State::Start;
                                break;
                            }
                            case ',':
                            {
                                int begin = i, end = i;
                                InsertToken(TokenKind::Comma, val, begin, end, col, i);
                                state = State::Start;
                                break;
                            }
                            case '~':
                            {
                                int begin = i, end = i;
                                InsertToken(TokenKind::BitNot, val, begin, end, col, i);
                                state = State::Start;
                                break;
                            }
                            case '?':
                            {
                                int begin = i, end = i;
                                InsertToken(TokenKind::Question, val, begin, end, col, i);
                                state = State::Start;
                                break;
                            }
                            case '.':
                            {
                                int begin = i, end = i;
                                if (IndexLegal(i+1) && s[i+1] == '.' && IndexLegal(i+2) && s[i+2]) {
                                    end += 2;
                                    InsertToken(TokenKind::Ellipsis, val, begin, end, col, i);
                                }else {
                                    InsertToken(TokenKind::Dot, val, begin, end, col, i);
                                }
                                state = State::Start;
                                break;
                            }
                            case '+':
                            {
                                int begin = i, end = i;
                                if (IndexLegal(i+1) && s[i+1] == '+') {
                                    end++;
                                    InsertToken(TokenKind::Inc, val, begin, end, col, i);
                                }else if (IndexLegal(i+1) && s[i+1] == '=') {
                                    end++;
                                    InsertToken(TokenKind::AddAssign, val, begin, end, col, i);
                                }else {
                                    InsertToken(TokenKind::Add, val, begin, end, col, i);
                                }
                                state = State::Start;
                                break;
                            }
                            case '-':
                            {
                                int begin = i, end = i;
                                if (IndexLegal(i+1) && s[i+1] == '-') {
                                    end++;
                                    InsertToken(TokenKind::Dec, val, begin, end, col, i);
                                }else if (IndexLegal(i+1) && s[i+1] == '=') {
                                    end++;
                                    InsertToken(TokenKind::SubAssign, val, begin, end, col, i);
                                }else if (IndexLegal(i+1) && s[i+1] == '>') {
                                    end++;
                                    InsertToken(TokenKind::Pointer, val, begin, end, col, i);
                                }else {
                                    InsertToken(TokenKind::Sub, val, begin, end, col, i);
                                }
                                state = State::Start;
                                break;
                            }
                            case '*':
                            {
                                int begin = i, end = i;
                                if (IndexLegal(i+1) && s[i+1] == '=') {
                                    end++;
                                    InsertToken(TokenKind::MulAssign, val, begin, end, col, i);
                                }else {
                                    InsertToken(TokenKind::Mul, val, begin, end, col, i);
                                }
                                state = State::Start;
                                break;
                            }
                            case '/':
                            {
                                int begin = i, end = i;
                                if (IndexLegal(i+1) && s[i+1] == '=') {
                                    end++;
                                    InsertToken(TokenKind::DivAssign, val, begin, end, col, i);
                                }else {
                                    InsertToken(TokenKind::Div, val, begin, end, col, i);
                                }
                                state = State::Start;
                                break;
                            }
                            case '%':
                            {
                                int begin = i, end = i;
                                if (IndexLegal(i+1) && s[i+1] == '=') {
                                    end++;
                                    InsertToken(TokenKind::ModAssign, val, begin, end, col, i);
                                }else {
                                    InsertToken(TokenKind::Mod, val, begin, end, col, i);
                                }
                                state = State::Start;
                                break;
                            }
                            case '=':
                            {
                                int begin = i, end = i;
                                if (IndexLegal(i+1) && s[i+1] == '=') {
                                    end++;
                                    InsertToken(TokenKind::Equal, val, begin, end, col, i);
                                }else {
                                    InsertToken(TokenKind::Assign, val, begin, end, col, i);
                                }
                                state = State::Start;
                                break;
                            }
                            case '!':
                            {
                                int begin = i, end = i;
                                if (IndexLegal(i+1) && s[i+1] == '=') {
                                    end++;
                                    InsertToken(TokenKind::UnEqual, val, begin, end, col, i);
                                }else {
                                    InsertToken(TokenKind::LogicNot, val, begin, end, col, i);
                                }
                                state = State::Start;
                                break;
                            }
                            case '^':
                            {
                                int begin = i, end = i;
                                if (IndexLegal(i+1) && s[i+1] == '=') {
                                    end++;
                                    InsertToken(TokenKind::BitXorAssign, val, begin, end, col, i);
                                }else {
                                    InsertToken(TokenKind::BitXor, val, begin, end, col, i);
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
                                        InsertToken(TokenKind::LshAssign, val, begin, end, col, i);
                                    }else {
                                        end++;
                                        InsertToken(TokenKind::LSH, val, begin, end, col, i);
                                    }
                                }else if (IndexLegal(i+1) && s[i+1] == '=') {
                                    end++;
                                    InsertToken(TokenKind::LessEq, val, begin, end, col, i);
                                }else {
                                    InsertToken(TokenKind::Less, val, begin, end, col, i);
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
                                        InsertToken(TokenKind::RshAssign, val, begin, end, col, i);
                                    }else {
                                        InsertToken(TokenKind::RSH, val, begin, end, col, i);
                                    }
                                }else if (IndexLegal(i+1) && s[i+1] == '=') {
                                    end++;
                                    InsertToken(TokenKind::GreatEq, val, begin, end, col, i);
                                }else {
                                    InsertToken(TokenKind::Great, val, begin, end, col, i);
                                }
                                state = State::Start;
                                break;
                            }
                            case '&':
                            {
                                int begin = i, end = i;
                                if (IndexLegal(i+1) && s[i+1] == '&') {
                                    end++;
                                    InsertToken(TokenKind::LogicAnd, val, begin, end, col, i);
                                }else if (IndexLegal(i+1) && s[i+1] == '=') {
                                    end++;
                                    InsertToken(TokenKind::BitAndAssign, val, begin, end, col, i);
                                }else {
                                    InsertToken(TokenKind::BitAnd, val, begin, end, col, i);
                                }
                                state = State::Start;
                                break;
                            }
                            case '|':
                            {
                                int begin = i, end = i;
                                if (IndexLegal(i+1) && s[i+1] == '|') {
                                    end++;
                                    InsertToken(TokenKind::LogicOr, val, begin, end, col, i);
                                }else if (IndexLegal(i+1) && s[i+1] == '=') {
                                    end++;
                                    InsertToken(TokenKind::BitorAssign, val, begin, end, col, i);
                                }else {
                                    InsertToken(TokenKind::Bitor, val, begin, end, col, i);
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