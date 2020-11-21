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
#include <list>

namespace CPToyC {
    namespace Compiler {

        enum class NodeKind {
            Null, Add, Sub, Mul, Div, Mod, BitAnd, BitOr, BitXor, BitNot, LSH, RSH, Eq, UnEq,
            LT, LE, GT, GE, Assign, Ternary, PreInc, PreDec, PostInc, PostDec,
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
            Node(NodeKind kind, std::shared_ptr<Token> tok): Kind(kind), Tok(tok) {};
            virtual ~Node() {}

        private:
            NodeKind Kind;
            std::shared_ptr<Token> Tok;
        };

        class ExprNode : public Node {
        public:
            ExprNode(NodeKind kind, std::shared_ptr<Token> tok): Node(kind, tok) {}
            virtual ~ExprNode() {}
        };

        class UnaryExprNode : public ExprNode {
        public:
            UnaryExprNode(NodeKind kind, std::shared_ptr<Token> tok, std::shared_ptr<ExprNode> lhs):ExprNode(kind, tok), Lhs(lhs) {}
            virtual ~UnaryExprNode() {}
        private:
            std::shared_ptr<ExprNode> Lhs;
        };

        class BinaryExprNode : public ExprNode {
        public:
            BinaryExprNode(NodeKind kind, std::shared_ptr<Token> tok, std::shared_ptr<ExprNode> lhs, std::shared_ptr<ExprNode> rhs)
                :ExprNode(kind, tok), Lhs(lhs), Rhs(rhs) {}
            virtual ~BinaryExprNode() {}
        private:
            std::shared_ptr<ExprNode> Lhs;
            std::shared_ptr<ExprNode> Rhs;
        };

        class TernaryExprNode : public ExprNode {
        public:
            TernaryExprNode(NodeKind kind, std::shared_ptr<Token> tok, std::shared_ptr<ExprNode> cond, std::shared_ptr<ExprNode> then,
                            std::shared_ptr<ExprNode> els):ExprNode(kind, tok), Cond(cond), Then(then), Els(els) {}
            virtual ~TernaryExprNode() {}
        private:
            std::shared_ptr<ExprNode> Cond;
            std::shared_ptr<ExprNode> Then;
            std::shared_ptr<ExprNode> Els;
        };

        class ProgramNode : public Node {
        public:
            ProgramNode() {}
            virtual ~ProgramNode() {}

        private:
            std::list<std::shared_ptr<ExprNode>> Exprs;
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
