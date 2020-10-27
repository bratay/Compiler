#include "ast.hpp"

namespace holeyc{

IRProgram * ProgramNode::to3AC(TypeAnalysis * ta){
	IRProgram * prog = new IRProgram(ta);
	for (auto global : *myGlobals){
		global->to3AC(prog);
	}
	return prog;
}

void FnDeclNode::to3AC(IRProgram * prog){
	Procedure * proc = prog->makeProc(myID->getName());
	size_t fIndex = 1;

	for(auto cur : *myFormals){
		cur->to3AC(proc);
		auto sym = cur->ID()->getSymbol();
		Opd* opd = proc->getSymOpd(sym);
		GetArgQuad* argquad = new GetArgQuad(fIndex, opd);
		proc->addQuad(argquad);
		fIndex++;
	}
	for (auto stmt : *myBody){
		stmt->to3AC(proc);
	}
}

void FnDeclNode::to3AC(Procedure * proc){
	//This never needs to be implemented,
	// the function only exists because of 
	// inheritance needs
}

void FormalDeclNode::to3AC(IRProgram * prog){}

void FormalDeclNode::to3AC(Procedure * proc){}

Opd * IntLitNode::flatten(Procedure * proc){
	return new LitOpd(std::to_string(myNum), QUADWORD);
}

Opd * StrLitNode::flatten(Procedure * proc){
	Opd * res = proc->getProg()->makeString(myStr);
	return res;
}

Opd * CharLitNode::flatten(Procedure * proc){
	return new LitOpd(std::to_string(myVal), BYTE);
}

Opd * NullPtrNode::flatten(Procedure * proc){
	return new LitOpd("0", ADDR);
}

Opd * TrueNode::flatten(Procedure * prog){
	Opd * res = new LitOpd("1", BYTE);
	return res;
}

Opd * FalseNode::flatten(Procedure * prog){
	Opd * res = new LitOpd("0", BYTE);
	return res;
}

Opd * AssignExpNode::flatten(Procedure * proc){
	Opd * dst = myDst->flatten(proc);
	Opd * src = mySrc->flatten(proc);
	Opd * d = proc->makeTmp(dst->getWidth());

	Quad * result = new AssignQuad(dst, src);
	proc->addQuad(result);

	return dst;
}

Opd * LValNode::flatten(Procedure * proc){
	TODO(Implement me)
}

Opd * DerefNode::flatten(Procedure * proc){
	TODO(Implement me)
}

Opd * RefNode::flatten(Procedure * proc){
	TODO(Implement me)
}

Opd * CallExpNode::flatten(Procedure * proc){
	size_t argIdx = 1;
	for (auto cur : *myArgs){
		Opd * arg = cur->flatten(proc);
		Quad * argQuad = new SetArgQuad(argIdx, arg);
		proc->addQuad(argQuad);
		argIdx++;
	}

	Quad * callQuad = new CallQuad(myID->getSymbol());
	proc->addQuad(callQuad);

	SemSymbol * idSym = myID->getSymbol();
	const FnType * calleeType = idSym->getDataType()->asFn();

	if (calleeType->getReturnType()->isVoid()) return nullptr;

	Opd * retVal = proc->makeTmp(QUADWORD);

	Quad * getRet = new GetRetQuad(retVal);
	proc->addQuad(getRet);

	return retVal;
}

Opd * NegNode::flatten(Procedure * proc){
	Opd * s = myExp->flatten(proc);
	Opd * d = proc->makeTmp(s->getWidth());

	Quad * q = new UnaryOpQuad(d,NEG,s);
	proc->addQuad(q);

	return d;
}

Opd * NotNode::flatten(Procedure * proc){
	Opd * s = myExp->flatten(proc);
	Opd * d = proc->makeTmp(OpdWidth::BYTE);

	Quad * q = new UnaryOpQuad(d,NOT,s);
	proc->addQuad(q);

	return d;
}

Opd * PlusNode::flatten(Procedure * proc){
	Opd * l = myExp1->flatten(proc);
	Opd * r = myExp2->flatten(proc);
	Opd * d = proc->makeTmp(OpdWidth::QUADWORD);

	Quad * q = new BinOpQuad(d,ADD,l,r);
	proc->addQuad(q);

	return d;
}

Opd * MinusNode::flatten(Procedure * proc){
	Opd * l = myExp1->flatten(proc);
	Opd * r = myExp2->flatten(proc);
	Opd * d = proc->makeTmp(OpdWidth::QUADWORD);

	Quad * q = new BinOpQuad(d,SUB,l,r);
	proc->addQuad(q);

	return d;
}

Opd * TimesNode::flatten(Procedure * proc){
	Opd * l = myExp1->flatten(proc);
	Opd * r = myExp2->flatten(proc);
	Opd * d = proc->makeTmp(OpdWidth::QUADWORD);

	Quad * q = new BinOpQuad(d,MULT,l,r);
	proc->addQuad(q);

	return d;
}

Opd * DivideNode::flatten(Procedure * proc){
	Opd * l = myExp1->flatten(proc);
	Opd * r = myExp2->flatten(proc);
	Opd * d = proc->makeTmp(OpdWidth::QUADWORD);

	Quad * q = new BinOpQuad(d,DIV,l,r);
	proc->addQuad(q);

	return d;
}

Opd * AndNode::flatten(Procedure * proc){
	Opd * l = myExp1->flatten(proc);
	Opd * r = myExp2->flatten(proc);
	Opd * d = proc->makeTmp(OpdWidth::BYTE);

	Quad * q = new BinOpQuad(d,AND,l,r);
	proc->addQuad(q);

	return d;
}

Opd * OrNode::flatten(Procedure * proc){
	Opd * l = myExp1->flatten(proc);
	Opd * r = myExp2->flatten(proc);
	Opd * d = proc->makeTmp(OpdWidth::BYTE);

	Quad * q = new BinOpQuad(d,OR,l,r);
	proc->addQuad(q);

	return d;
}

Opd * EqualsNode::flatten(Procedure * proc){
	Opd * l = myExp1->flatten(proc);
	Opd * r = myExp2->flatten(proc);
	Opd * d = proc->makeTmp(l->getWidth());

	Quad * q = new BinOpQuad(d,EQ,l,r);
	proc->addQuad(q);

	return d;
}

Opd * NotEqualsNode::flatten(Procedure * proc){
	Opd * l = myExp1->flatten(proc);
	Opd * r = myExp2->flatten(proc);
	Opd * d = proc->makeTmp(l->getWidth());

	Quad * q = new BinOpQuad(d,NEQ,l,r);
	proc->addQuad(q);

	return d;
}

Opd * LessNode::flatten(Procedure * proc){
	Opd * l = myExp1->flatten(proc);
	Opd * r = myExp2->flatten(proc);
	Opd * d = proc->makeTmp(OpdWidth::QUADWORD);

	Quad * q = new BinOpQuad(d,LT,l,r);
	proc->addQuad(q);

	return d;
}

Opd * GreaterNode::flatten(Procedure * proc){
	Opd * l = myExp1->flatten(proc);
	Opd * r = myExp2->flatten(proc);
	Opd * d = proc->makeTmp(OpdWidth::QUADWORD);

	Quad * q = new BinOpQuad(d,GT,l,r);
	proc->addQuad(q);

	return d;
}

Opd * LessEqNode::flatten(Procedure * proc){
	Opd * l = myExp1->flatten(proc);
	Opd * r = myExp2->flatten(proc);
	Opd * d = proc->makeTmp(OpdWidth::QUADWORD);

	Quad * q = new BinOpQuad(d,LTE,l,r);
	proc->addQuad(q);

	return d;
}

Opd * GreaterEqNode::flatten(Procedure * proc){
	Opd * l = myExp1->flatten(proc);
	Opd * r = myExp2->flatten(proc);
	Opd * d = proc->makeTmp(OpdWidth::QUADWORD);

	Quad * q = new BinOpQuad(d,GTE,l,r);
	proc->addQuad(q);

	return d;
}

void AssignStmtNode::to3AC(Procedure * proc){
	Opd * res = myExp->flatten(proc);
}

void PostIncStmtNode::to3AC(Procedure * proc){
	Opd * l = myLVal->flatten(proc);
	Opd * r = new LitOpd(to_string(1), QUADWORD);

	Quad * q = new BinOpQuad(l, ADD, l, r);
	proc->addQuad(q);
}

void PostDecStmtNode::to3AC(Procedure * proc){
	Opd * l = myLVal->flatten(proc);
	Opd * r = new LitOpd(to_string(1), QUADWORD);

	Quad * q = new BinOpQuad(l, SUB, l, r);
	proc->addQuad(q);
}

void FromConsoleStmtNode::to3AC(Procedure * proc){
	Opd * dst = myDst->flatten(proc);
	Quad * fromQuad = new IntrinsicQuad(INPUT, dst);
	
	proc->addQuad(fromQuad);
}

void ToConsoleStmtNode::to3AC(Procedure * proc){
	Opd * src = mySrc->flatten(proc);
	Quad * toQuad = new IntrinsicQuad(INPUT, src);
	
	proc->addQuad(toQuad);
}

void IfStmtNode::to3AC(Procedure * proc){
	Opd * cond = myCond->flatten(proc);
	Label * lbl = proc->makeLabel();
	Quad * a = new NopQuad();
	a->addLabel(lbl);

	Quad * q = new JmpIfQuad(cond, lbl);
	proc->addQuad(q);

	for (auto stmt : *myBody){
		stmt->to3AC(proc);
	}
	proc->addQuad(a);
}

void IfElseStmtNode::to3AC(Procedure * proc){
	Label * el = proc->makeLabel();
	Quad * en = new NopQuad();
	en->addLabel(el);
	Label * al = proc->makeLabel();
	Quad * an = new NopQuad();
	an->addLabel(al);

	Opd * cond = myCond->flatten(proc);

	Quad * jmpFalse = new JmpIfQuad(cond, el);
	proc->addQuad(jmpFalse);

	for(auto cur : *myBodyTrue){
		cur->to3AC(proc);
	}
	
	Quad * skipFall = new JmpQuad(al);
	proc->addQuad(skipFall);

	proc->addQuad(en);

	for(auto cur : *myBodyFalse){
		cur->to3AC(proc);
	}

	proc->addQuad(an);
}

void WhileStmtNode::to3AC(Procedure * proc){
	Quad * hn = new NopQuad();
	Label * hl = proc->makeLabel();
	hn->addLabel(hl);

	Label * al = proc->makeLabel();
	Quad * afterQuad = new NopQuad();
	afterQuad->addLabel(al);

	proc->addQuad(hn);
	Opd * cond = myCond->flatten(proc);
	Quad * jf = new JmpIfQuad(cond, al);
	proc->addQuad(jf);

	for(auto cur : *myBody){
		cur->to3AC(proc);
	}

	Quad * goBack = new JmpQuad(hl);
	proc->addQuad(goBack);
	proc->addQuad(afterQuad);
}

void CallStmtNode::to3AC(Procedure * proc){
	Opd * res = myCallExp->flatten(proc);
	if( res == nullptr ) return;

	Quad * last = proc->popQuad();
}

void ReturnStmtNode::to3AC(Procedure * proc){
	if (myExp != nullptr){
		Opd * res = myExp->flatten(proc);
		Quad * setOut = new SetRetQuad(res);
		proc->addQuad(setOut);
	}

	Label * lbl = proc->getLeaveLabel();
	Quad * jump = new JmpQuad(lbl);
	proc->addQuad(jump);

}

void VarDeclNode::to3AC(Procedure * proc){
	SemSymbol * sym = ID()->getSymbol();
	if (sym == nullptr){
		throw new InternalError("null sym");
	}
	proc->gatherLocal(sym);
}

void VarDeclNode::to3AC(IRProgram * prog){
	SemSymbol * sym = ID()->getSymbol();
	if (sym == nullptr){
		throw new InternalError("null sym");
	}
	prog->gatherGlobal(sym);
}

//We only get to this node if we are in a stmt
// context (DeclNodes protect descent) 
Opd * IDNode::flatten(Procedure * proc){
	SemSymbol * sym = this->getSymbol();
	Opd * res = proc->getSymOpd(sym);
	if (!res){
		throw new InternalError("null id");;
	}
	return res;
}


}
