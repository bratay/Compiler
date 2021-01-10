#ifndef HOLEYC_3AC_HPP
#define HOLEYC_3AC_HPP

#include <assert.h>
#include <list>
#include <map>
#include <set>
#include "err.hpp"
#include "symbol_table.hpp"
#include "types.hpp"

namespace holeyc{

class TypeAnalysis;
class Procedure;
class IRProgram;
class ControlFlowGraph;

class Label{
public:
	Label(std::string nameIn){
		this->name = nameIn;
	}
	std::string toString(){
		return this->name;
	}
	std::string getName(){
		return name;
	}
private:
	std::string name;
};

enum OpdWidth{
	ADDR, QUADWORD, BYTE
};

class Opd{
public:
	Opd(OpdWidth widthIn) : myWidth(widthIn){}
	virtual std::string valString() = 0;
	virtual std::string locString() = 0;
	virtual OpdWidth getWidth(){ return myWidth; }
	static OpdWidth width(const DataType * type){
		if (const BasicType * basic = type->asBasic()){
			if (basic->isChar()){ return BYTE; }
			if (basic->isBool()){ return BYTE; }
			return QUADWORD;
		} else if (const PtrType * ptr = type->asPtr()){
			return ADDR;
		}
		assert(false);
	}
private:
	OpdWidth myWidth;
};

class SymOpd : public Opd{
public:
	virtual std::string valString() override{
		return "[" + mySym->getName() + "]";
	}
	virtual std::string locString() override{
		return mySym->getName();
	}
	std::string getName(){
		return mySym->getName();
	}
	const SemSymbol * getSym(){ return mySym; }
private:
	//Private Constructor
	SymOpd(SemSymbol * sym, OpdWidth width)
	: Opd(width), mySym(sym) {} 
	SemSymbol * mySym;
	friend class Procedure;
	friend class IRProgram;
	std::string myLoc;
};

class LitOpd : public Opd{
public:
	LitOpd(std::string valIn, OpdWidth width)
	: Opd(width), val(valIn){ }
	virtual std::string valString() override{
		return val;
	}
	virtual std::string locString() override{
		throw InternalError("Tried to get location of a constant");
	}
private:
	std::string val;
};

class AuxOpd : public Opd{
public:
	AuxOpd(std::string nameIn, OpdWidth width) 
	: Opd(width), name(nameIn) { }
	virtual std::string valString() override{
		return "[" + getName() + "]";
	}
	virtual std::string locString() override{
		return getName();
	}
	std::string getName(){
		return name;
	}
private:
	std::string name;
};

enum BinOp {
	ADD, SUB, DIV, MULT, OR, AND, EQ, NEQ, LT, GT, LTE, GTE
};
enum UnaryOp{
	NEG, NOT
};

class Quad{
public:
	Quad();
	void addLabel(Label * label);
	Label * getLabel(){ return labels.front(); }
	void clearLabels(){ labels.clear(); }
	virtual std::string repr() = 0;
	std::string commentStr();
	virtual std::string toString(bool verbose=false);
	void setComment(std::string commentIn);
private:
	std::string myComment;
	std::list<Label *> labels;
	OpdWidth width;
};

class BinOpQuad : public Quad{
public:
	BinOpQuad(Opd * dstIn, BinOp opIn, Opd * src1In, Opd * src2In);
	std::string repr() override;
	Opd * getDst(){ return dst; }
	Opd * getSrc1(){ return src1; }
	Opd * getSrc2(){ return src2; }
private:
	Opd * dst;
	BinOp op;
	Opd * src1;
	Opd * src2;
};

class UnaryOpQuad : public Quad {
public:
	UnaryOpQuad(Opd * dstIn, UnaryOp opIn, Opd * srcIn);
	std::string repr() override ;
	Opd * getDst(){ return dst; }
	Opd * getSrc(){ return src; }
private:
	Opd * dst;
	UnaryOp op;
	Opd * src;
};

class AssignQuad : public Quad{
	
public:
	AssignQuad(Opd * dstIn, Opd * srcIn)
	: dst(dstIn), src(srcIn)
	{ }
	std::string repr() override;
	Opd * getDst(){ return dst; }
	Opd * getSrc(){ return src; }
private:
	Opd * dst;
	Opd * src;
};

class JmpQuad : public Quad {
public:
	JmpQuad(Label * tgtIn);
	std::string repr() override;
	Label * getLabel(){ return tgt; }
private:
	Label * tgt;
};

class JmpIfQuad : public Quad {
public:
	JmpIfQuad(Opd * cndIn, Label * tgtIn);
	std::string repr() override;
	Label * getLabel(){ return tgt; }
	Opd * getCnd(){ return cnd; }
private:
	Opd * cnd;
	Label * tgt;
};

class NopQuad : public Quad {
public:
	NopQuad();
	std::string repr() override;
};

class IntrinsicOutputQuad : public Quad {
public:
	IntrinsicOutputQuad(Opd * arg, const DataType * type);
	std::string repr() override;
	Opd * getSrc(){ return myArg; }
private:
	Opd * myArg;
	const DataType * myType;
};

class IntrinsicInputQuad : public Quad {
public:
	IntrinsicInputQuad(Opd * arg, const DataType * type);
	std::string repr() override;
	Opd * getDst(){ return myArg; }
private:
	Opd * myArg;
	const DataType * myType;
};

class CallQuad : public Quad{
public:
	CallQuad(SemSymbol * calleeIn);
	std::string repr() override;
private:
	SemSymbol * callee;
};

class EnterQuad : public Quad{
public:
	EnterQuad(Procedure * proc);
	virtual std::string repr() override;
private:
	Procedure * myProc;
};

class LeaveQuad : public Quad{
public:
	LeaveQuad(Procedure * proc);
	virtual std::string repr() override;
private:
	Procedure * myProc;
};

class SetArgQuad : public Quad{
public:
	SetArgQuad(size_t indexIn, Opd * opdIn);
	std::string repr() override;
	Opd * getSrc(){ return opd; }
private:
	size_t index;
	Opd * opd;
};

class GetArgQuad : public Quad{
public:
	GetArgQuad(size_t indexIn, Opd * opdIn);
	std::string repr() override;
	Opd * getDst(){ return opd; }
private:
	size_t index;
	Opd * opd;
};

class SetRetQuad : public Quad{
public:
	SetRetQuad(Opd * opdIn);
	std::string repr() override;
	Opd * getSrc(){ return opd; }
private:
	Opd * opd;
};

class GetRetQuad : public Quad{
public:
	GetRetQuad(Opd * opdIn);
	std::string repr() override;
	Opd * getDst(){ return opd; }
private:
	Opd * opd;
};

class Procedure{
public:
	Procedure(IRProgram * prog, std::string name);
	void addQuad(Quad * quad);
	Quad * popQuad();
	IRProgram * getProg();
	std::list<SymOpd *> getFormals() { return formals; }
	holeyc::Label * makeLabel();

	void gatherLocal(SemSymbol * sym);
	void gatherFormal(SemSymbol * sym);
	SymOpd * getSymOpd(SemSymbol * sym);
	AuxOpd * makeTmp(OpdWidth width);

	std::string toString(bool verbose=false); 
	std::string getName();

	holeyc::Label * getLeaveLabel();

	void toX64(std::ostream& out);
	size_t arSize() const;
	size_t numTemps() const;

	std::list<Quad *> * getQuads(){
		return bodyQuads;
	}
	EnterQuad * getEnter(){ return enter; }
	LeaveQuad * getLeave(){ return leave; }
private:
	void allocLocals();

	EnterQuad * enter;
	LeaveQuad * leave;
	Label * leaveLabel;

	IRProgram * myProg;
	std::map<SemSymbol *, SymOpd *> locals;
	std::list<AuxOpd *> temps; 
	std::list<SymOpd *> formals; 
	std::list<Quad *> * bodyQuads;
	std::string myName;
	size_t maxTmp;
};

class IRProgram{
public:
	IRProgram(TypeAnalysis * taIn) : ta(taIn){
		procs = new std::list<Procedure *>();
	}
	Procedure * makeProc(std::string name);
	std::list<Procedure *> * getProcs();
	Label * makeLabel();
	Opd * makeString(std::string val);
	void gatherGlobal(SemSymbol * sym);
	SymOpd * getGlobal(SemSymbol * sym);
	OpdWidth opDerefWidth(ASTNode * node);
	OpdWidth opWidth(ASTNode * node);
	const DataType * nodeType(ASTNode * node);

	std::string toString(bool verbose=false);

	void toX64(std::ostream& out);
	std::set<Opd *> globalSyms();
private:
	TypeAnalysis * ta;
	size_t max_label = 0;
	size_t str_idx = 0;
	std::list<Procedure *> * procs; 
	HashMap<AuxOpd *, std::string> strings;
	std::map<SemSymbol *, SymOpd *> globals;

	void datagenX64(std::ostream& out);
	void allocGlobals();
};

}

#endif 
