
#ifndef HOLEYC_AST_HPP
#define HOLEYC_AST_HPP

#include <ostream>
#include <list>
#include "tokens.hpp"

// **********************************************************************
// ASTnode class (base class for all other kinds of nodes)
// **********************************************************************

namespace holeyc{

/* You may find it useful to forward declare AST subclasses
   here so that you can use a class before it's full definition
*/
class DeclNode;
class TypeNode;
class IDNode;
class ExpNode;
class FalseNode;
class ProgramNode;
class StmtNode;
class BinaryExpNode;
class CallExpNode;
class CharLitNode;
class AssignExpNode;
class IntLitNode;
class TrueNode;
class LValNode;
class StrLitNode;
class CallStmtNode;
class UnaryExpNode;
class NullPtrNode;
class AssignStmtNode;
class FromConsoleStmtNode;
class IfElseStmtNode;
class IfStmtNode;
class PostDecStmtNode;
class PostIncStmtNode;
class ReturnStmtNode;
class ToConsoleStmtNode;
class IntTypeNode;
class WhileStmtNode;
class CharTypeNode;
class VoidTypeNode;
class LessEqNode;
class BoolTypeNode;
class DivideNode;
class EqualsNode;
class AndNode;
class GreaterNode;
class NotEqualsNode;
class GreaterEqNode;
class LessNode;
class MinusNode;
class OrNode;
class PlusNode;
class TimesNode;
class DerefNode;
class RefNode;
class IndexNode;
class FnDeclNode;
class NotNode;
class VarDeclNode;
class NegNode;
class FormalDeclNode;


class ASTNode{
public:
	ASTNode(size_t lineIn, size_t colIn)
	: l(lineIn), c(colIn){
	}
	virtual void unparse(std::ostream& out, int indent) = 0;
	void shift(std::ostream& out, int indent){
		for (int k = 0 ; k < indent; k++){ out << " "; }
	}
	size_t line(){ return l; }
	size_t col() { return c; }

	/**
	* Return a string specifying the position this node begins.
	* For some node a position doesn't really make sense (i.e.
	* ProgramNode) but for the rest it's the position in the 
	* input file that represents that node
	**/
	std::string pos(){
		return "[" + std::to_string(line()) + ","
		+ std::to_string(col()) + "]";
	}

private:
	size_t l; /// The line at which the node starts in the input file
	size_t c; /// The column at which the node starts in the input file
};

class ExpNode : public ASTNode{
protected:
	ExpNode(size_t l, size_t c) : ASTNode(l, c){}
};

class ProgramNode : public ASTNode{
public:
	ProgramNode(std::list<DeclNode *> * globalsIn) : ASTNode(1, 1), myGlobals(globalsIn){}
	void unparse(std::ostream& out, int indent) override;
private:
	std::list<DeclNode * > * myGlobals;
};

class StmtNode : public ASTNode{
public:
	StmtNode(size_t l, size_t c) : ASTNode(l ,c) {}
	virtual void unparse(std::ostream& out, int indent) = 0;
};

class IDNode : public ExpNode{
public:
	IDNode(IDToken * token) : ExpNode(token->line(), token->col()), myStrVal(token->value()){
		myStrVal = token->value();
	}
	void unparse(std::ostream& out, int indent);
private:
	std::string myStrVal;
};

/**  \class TypeNode
* Superclass of nodes that indicate a data type. For example, in 
* the declaration "int a", the int part is the type node (a is an IDNode
* and the whole thing is a DeclNode).
**/
class TypeNode : public ASTNode{
protected:
	TypeNode(size_t lineIn, size_t colIn, bool refIn) 
	: ASTNode(lineIn, colIn), myIsReference(refIn){
	}
public:
	virtual void unparse(std::ostream& out, int indent) = 0;
private:
	bool myIsReference;
};

class LValNode : public ExpNode{
public:
	LValNode(IDNode* id) : ExpNode(id->line(), id->col()), myId(id){}
	void unparse(std::ostream& out, int indent);

private:
	IDNode* myId;
};

class AssignExpNode : public ExpNode{
public:
	AssignExpNode(LValNode* lVal, ExpNode* srcExp) : ExpNode(lVal->line(), lVal->col()), myLVal(lVal), myExp(srcExp){}
	void unparse(std::ostream& out, int indent);
private:
	LValNode* myLVal;
	ExpNode* myExp;
};

class CallExpNode : public ExpNode{
public:
	CallExpNode(IDNode* id, std::list<ExpNode*>* paramList) : ExpNode(id->line(), id->col()), myId(id), myParams(paramList){}
	void unparse(std::ostream& out, int indent);

private:
	IDNode* myId;
	std::list<ExpNode*>* myParams;
};

class NullPtrNode : public ExpNode{
public:
	NullPtrNode(size_t l, size_t c) : ExpNode(l, c){}
	void unparse(std::ostream& out, int indent);
};

class BinaryExpNode : public ExpNode{
public:
	BinaryExpNode(ExpNode* lhs, ExpNode* rhs) : ExpNode(lhs->line(), lhs->col()){}
	virtual void unparse(std::ostream& out, int indent) = 0;
};

class CharLitNode : public ExpNode{
public:
	CharLitNode(size_t l, size_t c, CharLitToken* charIn) : ExpNode(l, c){
		myChar = charIn->val();
	}
	void unparse(std::ostream& out, int indent);
private:
	char myChar;
};

class IntLitNode : public ExpNode{
public:
	IntLitNode(size_t l, size_t c, IntLitToken* intIn) : ExpNode(l, c){
		myInt = intIn->num();
	}
	void unparse(std::ostream& out, int indent);
private:
	int myInt;
};

class StrLitNode : public ExpNode{
public:
	StrLitNode(size_t l, size_t c, StrToken* strIn) : ExpNode(l, c){
		myStr = strIn->str();
	}
	void unparse(std::ostream& out, int indent);
private:
	std::string myStr;
};

class FalseNode : public ExpNode{
public:
	FalseNode(Token* token) : ExpNode(token->line(), token->col()){}
	void unparse(std::ostream& out, int indent);
};

class UnaryExpNode : public ExpNode{
public:
	UnaryExpNode(ExpNode* exp) : ExpNode(exp->line(), exp->col()){}
};

class TrueNode : public ExpNode{
public:
	TrueNode(Token* token) : ExpNode(token->line(), token->col()){}
	void unparse(std::ostream& out, int indent);
};

class AssignStmtNode : public StmtNode{
public:
	AssignStmtNode(AssignExpNode* assignment) : StmtNode(assignment->line(), assignment->col()), myAssign(assignment){}
	void unparse(std::ostream& out, int indent);
private:
	AssignExpNode* myAssign;
};

class DeclNode : public StmtNode{
public:
	DeclNode(size_t l, size_t c) : StmtNode(l, c) {}
	virtual void unparse(std::ostream& out, int indent) = 0;
};

class CallStmtNode : public StmtNode{
public:
	CallStmtNode(CallExpNode* call) : StmtNode(call->line(), call->col()), myCall(call){}
	void unparse(std::ostream& out, int indent);

private:
	CallExpNode* myCall;
};

class FromConsoleStmtNode : public StmtNode{
public:
	FromConsoleStmtNode(LValNode* lVal) : StmtNode(lVal->line(), lVal->col()), myLVal(lVal){}
	void unparse(std::ostream& out, int indent);

private:
	LValNode* myLVal;
};

class IfElseStmtNode : public StmtNode{
public:
	IfElseStmtNode(ExpNode* exp, std::list<StmtNode*>* trueList, std::list<StmtNode*>* falseList) : StmtNode(exp->line(), exp->col()),
		myExp(exp), myTList(trueList), myFList(falseList){}
	void unparse(std::ostream& out, int indent);
private:
	ExpNode* myExp;
	std::list<StmtNode*>* myTList;
	std::list<StmtNode*>* myFList;
};

class IfStmtNode : public StmtNode{
public:
	IfStmtNode(ExpNode* exp, std::list<StmtNode*>* stmtList) : StmtNode(exp->line(), exp->col()), myExp(exp), myStmtList(stmtList){}
	void unparse(std::ostream& out, int indent);

private:
	ExpNode* myExp;
	std::list<StmtNode*>* myStmtList;
};

class PostDecStmtNode : public StmtNode{
public:
	PostDecStmtNode(ExpNode* decId) : StmtNode(decId->line(), decId->col()), myExp(decId){}
	void unparse(std::ostream& out, int indent);

private:
	ExpNode* myExp;
};

class PostIncStmtNode : public StmtNode{
public:
	PostIncStmtNode(ExpNode* incId) : StmtNode(incId->line(), incId->col()), myExp(incId){}
	void unparse(std::ostream& out, int indent);
private:
	ExpNode* myExp;
};


class ToConsoleStmtNode : public StmtNode{
public:
	ToConsoleStmtNode(ExpNode* exp) : StmtNode(exp->line(), exp->col()), myExp(exp){}
	void unparse(std::ostream& out, int indent);

private:
	ExpNode* myExp;
};

class ReturnStmtNode : public StmtNode{
public:
	ReturnStmtNode(ExpNode* returnId, bool emptyIn) : StmtNode(returnId->line(), returnId->col()), myExp(returnId), empty(emptyIn){}
	ReturnStmtNode(size_t l, size_t c, bool emptyIn) : StmtNode(l, c), empty(emptyIn){}
	void unparse(std::ostream& out, int indent);

private:
	ExpNode* myExp;
	bool empty;
};

class WhileStmtNode : public StmtNode{
public:
	WhileStmtNode(ExpNode* condition, std::list<StmtNode*>* body) : StmtNode(condition->line(), condition->col()),
		myExp(condition), myStmtList(body){}
	void unparse(std::ostream& out, int indent);
private:
	ExpNode* myExp;
	std::list<StmtNode*>* myStmtList;
};

class BoolTypeNode : public TypeNode{
public:
	BoolTypeNode(size_t l, size_t c, bool refIn) : TypeNode(l, c, refIn){}
	void unparse(std::ostream& out, int indent);
};

class BoolPtrNode : public TypeNode{
public:
	BoolPtrNode(size_t l, size_t c, bool refIn) : TypeNode(l, c, refIn){}
	void unparse(std::ostream& out, int indent);
};

class CharTypeNode : public TypeNode{
public:
	CharTypeNode(size_t l, size_t c, bool refIn) : TypeNode(l, c, refIn){}
	void unparse(std::ostream& out, int indent);
};

class CharPtrNode : public TypeNode{
public:
	CharPtrNode(size_t l, size_t c, bool refIn) : TypeNode(l, c, refIn){}
	void unparse(std::ostream& out, int indent);
};

class IntTypeNode : public TypeNode{
public:
	IntTypeNode(size_t l, size_t c, bool isRefIn): TypeNode(l, c, isRefIn){}
	void unparse(std::ostream& out, int indent);
};

class IntPtrNode : public TypeNode{
public:
	IntPtrNode(size_t l, size_t c, bool isRefIn): TypeNode(l, c, isRefIn){}
	void unparse(std::ostream& out, int indent);
};

class VoidTypeNode : public TypeNode{
public:
	VoidTypeNode(size_t l, size_t c, bool refIn) : TypeNode(l, c, refIn){}
	void unparse(std::ostream& out, int indent);
};

class AndNode : public BinaryExpNode{
public: 
	AndNode(ExpNode* lhs, ExpNode* rhs) : BinaryExpNode(lhs, rhs), myLhs(lhs), myRhs(rhs){}
	void unparse(std::ostream& out, int indent);

private:
	ExpNode* myLhs;
	ExpNode* myRhs;
};

class DivideNode : public BinaryExpNode{
public: 
	DivideNode(ExpNode* lhs, ExpNode* rhs) : BinaryExpNode(lhs, rhs), myLhs(lhs), myRhs(rhs){}
	void unparse(std::ostream& out, int indent);

private:
	ExpNode* myLhs;
	ExpNode* myRhs;
};

class EqualsNode : public BinaryExpNode{
public: 
	EqualsNode(ExpNode* lhs, ExpNode* rhs) : BinaryExpNode(lhs, rhs), myLhs(lhs), myRhs(rhs){}
	void unparse(std::ostream& out, int indent);

private:
	ExpNode* myLhs;
	ExpNode* myRhs;
};

class GreaterEqNode : public BinaryExpNode{
public: 
	GreaterEqNode(ExpNode* lhs, ExpNode* rhs) : BinaryExpNode(lhs, rhs), myLhs(lhs), myRhs(rhs){}
	void unparse(std::ostream& out, int indent);

private:
	ExpNode* myLhs;
	ExpNode* myRhs;
};

class GreaterNode : public BinaryExpNode{
public: 
	GreaterNode(ExpNode* lhs, ExpNode* rhs) : BinaryExpNode(lhs, rhs), myLhs(lhs), myRhs(rhs){}
	void unparse(std::ostream& out, int indent);

private:
	ExpNode* myLhs;
	ExpNode* myRhs;
};

class LessEqNode : public BinaryExpNode{
public: 
	LessEqNode(ExpNode* lhs, ExpNode* rhs) : BinaryExpNode(lhs, rhs), myLhs(lhs), myRhs(rhs){}
	void unparse(std::ostream& out, int indent);

private:
	ExpNode* myLhs;
	ExpNode* myRhs;
};

class LessNode : public BinaryExpNode{
public: 
	LessNode(ExpNode* lhs, ExpNode* rhs) : BinaryExpNode(lhs, rhs), myLhs(lhs), myRhs(rhs){}
	void unparse(std::ostream& out, int indent);

private:
	ExpNode* myLhs;
	ExpNode* myRhs;
};

class MinusNode : public BinaryExpNode{
public: 
	MinusNode(ExpNode* lhs, ExpNode* rhs) : BinaryExpNode(lhs, rhs), myLhs(lhs), myRhs(rhs){}
	void unparse(std::ostream& out, int indent);

private:
	ExpNode* myLhs;
	ExpNode* myRhs;
};

class NotEqualsNode : public BinaryExpNode{
public: 
	NotEqualsNode(ExpNode* lhs, ExpNode* rhs) : BinaryExpNode(lhs, rhs), myLhs(lhs), myRhs(rhs){}
	void unparse(std::ostream& out, int indent);

private:
	ExpNode* myLhs;
	ExpNode* myRhs;
};

class OrNode : public BinaryExpNode{
public: 
	OrNode(ExpNode* lhs, ExpNode* rhs) : BinaryExpNode(lhs, rhs), myLhs(lhs), myRhs(rhs){}
	void unparse(std::ostream& out, int indent);

private:
	ExpNode* myLhs;
	ExpNode* myRhs;
};

class PlusNode : public BinaryExpNode{
public: 
	PlusNode(ExpNode* lhs, ExpNode* rhs) : BinaryExpNode(lhs, rhs), myLhs(lhs), myRhs(rhs){}
	void unparse(std::ostream& out, int indent);

private:
	ExpNode* myLhs;
	ExpNode* myRhs;
};

class TimesNode : public BinaryExpNode{
public: 
	TimesNode(ExpNode* lhs, ExpNode* rhs) : BinaryExpNode(lhs, rhs), myLhs(lhs), myRhs(rhs){}
	void unparse(std::ostream& out, int indent);

private:
	ExpNode* myLhs;
	ExpNode* myRhs;
};

class DerefNode : public LValNode{
public:
	DerefNode(IDNode* accessId) : LValNode(accessId), myId(accessId){}
	void unparse(std::ostream& out, int indent);
private:
	IDNode * myId;
};

class RefNode : public LValNode{
public:
	RefNode(IDNode* accessId) : LValNode(accessId), myId(accessId) {}
	void unparse(std::ostream& out, int indent);
private:
	IDNode * myId;
};

class IndexNode : public LValNode{
public:
	IndexNode(IDNode* accessId, ExpNode* offset) : LValNode(accessId), myId(accessId), myExp(offset){}
	void unparse(std::ostream& out, int indent);

private:
	IDNode* myId;
	ExpNode* myExp;
};

class NegNode : public UnaryExpNode{
public:
	NegNode(ExpNode* exp) : UnaryExpNode(exp), myExp(exp){}
	void unparse(std::ostream& out, int indent);

private:
	ExpNode* myExp;
};

class NotNode : public UnaryExpNode{
public:
	NotNode(ExpNode* exp) : UnaryExpNode(exp), myExp(exp){}
	void unparse(std::ostream& out, int indent);

private:
	ExpNode* myExp;
};

class FnDeclNode : public DeclNode{
public:
	FnDeclNode(TypeNode* type, IDNode* id, std::list<FormalDeclNode*>* params, std::list<StmtNode*>* body):
	DeclNode(type->line(), type->col()), myType(type), myId(id), myParams(params), myBody(body){}
	void unparse(std::ostream& out, int indent);

private:
	TypeNode* myType;
	IDNode* myId;
	std::list<FormalDeclNode*>* myParams;
	std::list<StmtNode*>* myBody;
};

/** A variable declaration. Note that this class is intended to 
 * represent a global or local variable of any type (including a struct
 * type. Note that this is not intended to represent a declaration of
 * a struct. In other words:
 * struct MyStruct { 
 *   int fieldA;
 * };
 * is NOT a VarDeclNode because it introduces a new datatype, not a new
 * variable (in this case, the example is a StructDeclNode).  * However,
 * struct MyStruct instance; *is* a VarDeclNode, since it introduces a 
 * new variable to the program. 
**/
class VarDeclNode : public DeclNode{
public:
	VarDeclNode(size_t l, size_t c, TypeNode * type, IDNode * id) : DeclNode(type->line(), type->col()), myType(type), myId(id){}
	void unparse(std::ostream& out, int indent);
private:
	TypeNode * myType;
	IDNode * myId;
};

class FormalDeclNode : public DeclNode{
public:
	FormalDeclNode(TypeNode* type, IDNode* id) : DeclNode(type->line(), type->col()), myType(type), myId(id){}
	void unparse(std::ostream& out, int indent);

private:
	TypeNode * myType;
	IDNode * myId;
};

} //End namespace holeyc

#endif