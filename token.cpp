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

namespace CPToyC {
    namespace Compiler {
        string Token::KindToStr(TokenKind kind) {
            switch (kind) {
                case TokenKind::ID:
                    return "ID";
                case TokenKind::Auto:
                case TokenKind::Break:
                case TokenKind::Case:
                case TokenKind::Char:
                case TokenKind::Const:
                case TokenKind::Continue:
                case TokenKind::Default:
                case TokenKind::Do:
                case TokenKind::Double:
                case TokenKind::Else:
                case TokenKind::Enum:
                case TokenKind::Extern:
                case TokenKind::Float:
                case TokenKind::For:
                case TokenKind::Goto:
                case TokenKind::If:
                case TokenKind::Int:
                case TokenKind::Long:
                case TokenKind::Register:
                case TokenKind::Return:
                case TokenKind::Short:
                case TokenKind::Signed:
                case TokenKind::Sizeof:
                case TokenKind::Static:
                case TokenKind::Struct:
                case TokenKind::Switch:
                case TokenKind::Typedef:
                case TokenKind::Union:
                case TokenKind::Unsigned:
                case TokenKind::Void:
                case TokenKind::Volatile:
                case TokenKind::While:
                    return "Keyword";
                case TokenKind::IntLiteral:
                    return "IntLiteral";
                case TokenKind::UIntLiteral:
                    return "UIntLiteral";
                case TokenKind::LongLiteral:
                    return "LongLiteral";
                case TokenKind::ULongLiteral:
                    return "ULongLiteral";
                case TokenKind::LLongLiteral:
                    return "LLongLiteral";
                case TokenKind::ULLongLiteral:
                    return "ULLongLiteral";
                case TokenKind::FloatLiteral:
                    return "FloatLiteral";
                case TokenKind::DoubleLiteral:
                    return "DoubleLiteral";
                case TokenKind::LDoubleLiteral:
                    return "LDoubleLiteral";
                case TokenKind::StringLiteral:
                    return "StringLiteral";
                case TokenKind::WStringLiteral:
                    return "WStringLiteral";
                case TokenKind::Comma:
                    return "Comma";
                case TokenKind::Question:
                    return "Question";
                case TokenKind::Colon:
                    return "Colon";
                case TokenKind::Assign:
                    return "Assign";
                case TokenKind::BitorAssign:
                    return "BitorAssign";
                case TokenKind::BitXorAssign:
                    return "BitXorAssign";
                case TokenKind::BitAndAssign:
                    return "BitAndAssign";
                case TokenKind::LshAssign:
                    return "LshAssign";
                case TokenKind::RshAssign:
                    return "RshAssign";
                case TokenKind::AddAssign:
                    return "AddAssign";
                case TokenKind::SubAssign:
                    return "SubAssign";
                case TokenKind::MulAssign:
                    return "MulAssign";
                case TokenKind::DivAssign:
                    return "DivAssign";
                case TokenKind::ModAssign:
                    return "ModAssign";
                case TokenKind::LogicOr:
                    return "LogicOr";
                case TokenKind::LogicAnd:
                    return "LogicAnd";
                case TokenKind::Bitor:
                    return "Bitor";
                case TokenKind::BitXor:
                    return "BitXor";
                case TokenKind::BitAnd:
                    return "BitAnd";
                case TokenKind::Equal:
                    return "Equal";
                case TokenKind::UnEqual:
                    return "UnEqual";
                case TokenKind::Great:
                    return "Great";
                case TokenKind::Less:
                    return "Less";
                case TokenKind::GreatEq:
                    return "GreatEq";
                case TokenKind::LessEq:
                    return "LessEq";
                case TokenKind::LSH:
                    return "LSH";
                case TokenKind::RSH:
                    return "RSH";
                case TokenKind::Add:
                    return "Add";
                case TokenKind::Sub:
                    return "Sub";
                case TokenKind::Mul:
                    return "Mul";
                case TokenKind::Div:
                    return "Div";
                case TokenKind::Mod:
                    return "Mod";
                case TokenKind::Inc:
                    return "Inc";
                case TokenKind::Dec:
                    return "Dec";
                case TokenKind::LogicNot:
                    return "LogicNot";
                case TokenKind::BitNot:
                    return "BitNot";
                case TokenKind::Dot:
                    return "Dot";
                case TokenKind::Pointer:
                    return "Pointer";
                case TokenKind::LParent:
                    return "LParent";
                case TokenKind::RParent:
                    return "RParent";
                case TokenKind::LBracket:
                    return "LBracket";
                case TokenKind::RBracket:
                    return "RBracket";
                case TokenKind::LBrace:
                    return "LBrace";
                case TokenKind::RBrace:
                    return "RBrace";
                case TokenKind::Semicolon:
                    return "Semicolon";
                case TokenKind::Ellipsis:
                    return "Ellipsis";
                default:
                    return "EOF";
            }
        }
    }
}
