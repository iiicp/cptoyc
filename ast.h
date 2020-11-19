/**********************************
* File:     ast.h
*
* Author:   caipeng
*
* Email:    iiicp@outlook.com
*
* Date:     2020/11/19
***********************************/

#ifndef CPTOYC_AST_H
#define CPTOYC_AST_H
#include <memory>
#include "token.h"

namespace CPToyC {
    namespace Compiler {

        enum class NodeKind {
            Null, Add, Sub, Mul, Div, Mod, BitAnd, BitOr, BitXor, BitNot, SHL, SHR, Eq, UnEq,
            LT, LE, RT, RE, Assign, Ternary, PreInc, PreDec, PostInc, PostDec,
            AddAssign, SubAssign, MulAssign, DivAssign, ModAssign, SHLAssign,
            SHRAssign, Comma, Member, Addr, Deref, LogicalNot, LogicalAnd, LogicalOr,
            Var, IntNum, FloatNum, Case
        };

        class Node {
        public:
            Node() {
                Kind = NodeKind::Null;
                Tok = std::make_shared<Token>();
            }
            Node(NodeKind kind, std::shared_ptr<Token> &tok): Kind(kind), Tok(tok) {};
            virtual ~Node() {}
        public:
            NodeKind Kind;
            std::shared_ptr<Token> Tok;
        };

        class UnaryExprNode : public Node {
        public:
            UnaryExprNode(NodeKind kind, std::shared_ptr<Token> &tok, std::shared_ptr<Node> &lhs):Node(kind, tok), Lhs(lhs) {}
            virtual ~UnaryExprNode() {}
        private:
            std::shared_ptr<Node> Lhs;
        };

        class BinaryExprNode : public Node {
        public:
            BinaryExprNode(NodeKind kind, std::shared_ptr<Token> &tok, std::shared_ptr<Node> &lhs, std::shared_ptr<Node> &rhs)
                :Node(kind, tok), Lhs(lhs), Rhs(rhs) {}
            virtual ~BinaryExprNode() {}
        private:
            std::shared_ptr<Node> Lhs;
            std::shared_ptr<Node> Rhs;
        };

        class TernaryExprNode : public Node {
        public:
            TernaryExprNode(NodeKind kind, std::shared_ptr<Token> &tok, std::shared_ptr<Node> &cond, std::shared_ptr<Node> &then,
                            std::shared_ptr<Node> &els):Node(kind, tok), Cond(cond), Then(then), Els(els) {}
            virtual ~TernaryExprNode() {}
        private:
            std::shared_ptr<Node> Cond;
            std::shared_ptr<Node> Then;
            std::shared_ptr<Node> Els;
        };

        class ReturnStmtNode : public Node {
        public:
            ReturnStmtNode() {}
            virtual ~ReturnStmtNode() {}
        private:
            std::shared_ptr<Node> Expr;
        };

        class IfStmtNode : public Node {
        public:
            IfStmtNode() {}
            virtual ~IfStmtNode() {}

        private:
            std::shared_ptr<Node> Cond;
            std::shared_ptr<Node> Then;
            std::shared_ptr<Node> Els;
        };

        class WhileStmtNode : public Node {
        public:
            WhileStmtNode() {}
            virtual ~WhileStmtNode() {}

        private:
            std::shared_ptr<Node> Cond;
            std::shared_ptr<Node> Then;
        };

        class DoWhileStmtNode : public Node {
        public:
            DoWhileStmtNode() {}
            virtual ~DoWhileStmtNode() {}

        private:
            std::shared_ptr<Node> Stmt;
            std::shared_ptr<Node> Cond;
        };
    }
}

#endif //CPTOYC_AST_H
