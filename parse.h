//
// Created by caipeng on 2020/11/17.
//

#ifndef CPTOYC_PARSE_H
#define CPTOYC_PARSE_H

namespace CPToyC
{
    namespace Compiler
    {
        enum class NodeKind
        {
            NULL, Add, Sub, Mul, Div, BitAnd, BitOr, BitXor, Shl, Shr, Equal, NotEqual,
            LT, LE, Assign, Ternary, PreInc, PreDec, PostInc, PostDec, AddEq, SubEq,
            MulEq, DivEq, ShlEq, ShrEq, Comma, Member, Addr, Deref, Not, BitNot, LogAnd,
            LogOr, Return, If, While, For, Switch, Case, Sizeof, Block, Break, Continue,
            Goto, Label, FunCall,
        };

        class AstNode
        {
            AstNode() = default;
            virtual ~AstNode() = default;
        };

        class AddAstNode
        {

        };

        class Parse
        {

        };
    }
}


#endif //CPTOYC_PARSE_H
