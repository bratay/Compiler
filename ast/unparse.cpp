#include "ast.hpp"

namespace holeyc{

void ProgramNode::unparse(std::ostream& out, int indent){
	for (auto global : *myGlobals){
		global->unparse(out, indent);
	}
}

void VarDeclNode::unparse(std::ostream& out, int indent){
	shift(out, indent);
	this->myType->unparse(out, 0);
	out << " ";
	this->myId->unparse(out, 0);
	out << ";\n";
}

void FnDeclNode::unparse(std::ostream& out, int indent){
	shift(out, indent);
	this->myType->unparse(out, 0);
	out << " ";
	this->myId->unparse(out, 0);
	out << "(";
	for (auto param: *myParams)
		param->unparse(out, 0);
	out << ") {\n";
	for (auto line: *myBody){
		line->unparse(out, indent + 1);
	}
	out << "}\n";
}

void PostDecStmtNode::unparse(std::ostream& out, int indent){
	shift(out, indent);
	this->myExp->unparse(out, 0);
	out<<"--;\n";
}

void PostIncStmtNode::unparse(std::ostream& out, int indent){
	shift(out, indent);
	this->myExp->unparse(out, 0);
	out<<"++;\n";
}

void FromConsoleStmtNode::unparse(std::ostream& out, int indent){         
	shift(out, indent);
	out<<"FROMCONSOLE ";
	this->myLVal->unparse(out, 0);
	out<<";\n";
}

void ToConsoleStmtNode::unparse(std::ostream& out, int indent){
	shift(out, indent);
	out<<"TOCONSOLE ";
	this->myExp->unparse(out, 0);
	out<<";\n";
}

void AssignStmtNode::unparse(std::ostream& out, int indent){
	shift(out, indent);
	this->myAssign->unparse(out, 0);
	out<< ";\n";
}

void IfStmtNode::unparse(std::ostream& out, int indent){
	shift(out, indent);
	out<<"if (";
	this->myExp->unparse(out, 0);
	out<<") {\n";
	for (auto stmt: *myStmtList)
		stmt->unparse(out, indent+1);
	shift(out, indent);
	out<<"}\n";
}

void IfElseStmtNode::unparse(std::ostream& out, int indent){
	shift(out, indent);
	out<<"if (";
	this->myExp->unparse(out, 0);
	out<<") {\n";
	for (auto stmt: *myTList)
		stmt->unparse(out, indent+1);
	shift(out, indent);
	out<<"} else {\n";
	for (auto stmt: *myFList)
		stmt->unparse(out, indent+1);
	shift(out, indent);
	out<<"}\n";
}

void ReturnStmtNode::unparse(std::ostream& out, int indent){
	shift(out, indent);
	out<<"return";
	if(!this->empty){
		out<<" ";
		this->myExp->unparse(out, 0);
	}
	out<<";\n";
}

static void shift(std::ostream& out, int indent){
	for (int k = 0 ; k < indent; k++){ out << "\t"; }
}

void WhileStmtNode::unparse(std::ostream& out, int indent){
	shift(out, indent);
	out<<"while (";
	this->myExp->unparse(out, 0);
	out<<") {\n";
	for (auto stmt: *myStmtList)
		stmt->unparse(out, indent+1);
	shift(out, indent);
	out<<"}\n";
}

void CallStmtNode::unparse(std::ostream& out, int indent){
	shift(out, indent);
	this->myCall->unparse(out, 0);
	out<<";\n";
}

void AssignExpNode::unparse(std::ostream& out, int indent){
	shift(out, indent);
	this->myLVal->unparse(out, 0);
	out << " = ";
	this->myExp->unparse(out, 0);
}

void MinusNode::unparse(std::ostream& out, int indent){
	shift(out, indent);
	this->myLhs->unparse(out, 0);
	out << " - ";
	this->myRhs->unparse(out, 0);
}

void PlusNode::unparse(std::ostream& out, int indent){
	shift(out, indent);
	this->myLhs->unparse(out, 0);
	out << " + ";
	this->myRhs->unparse(out, 0);
}

void TimesNode::unparse(std::ostream& out, int indent){
	shift(out, indent);
	this->myLhs->unparse(out, 0);
	out << " * ";
	this->myRhs->unparse(out, 0);
}

void DivideNode::unparse(std::ostream& out, int indent){
	shift(out, indent);
	this->myLhs->unparse(out, 0);
	out << " / ";
	this->myRhs->unparse(out, 0);
}

void AndNode::unparse(std::ostream& out, int indent){
	shift(out, indent);
	this->myLhs->unparse(out, 0);
	out << " && ";
	this->myRhs->unparse(out, 0);
}

void OrNode::unparse(std::ostream& out, int indent){
	shift(out, indent);
	this->myLhs->unparse(out, 0);
	out << " || ";
	this->myRhs->unparse(out, 0);
}

void EqualsNode::unparse(std::ostream& out, int indent){
	shift(out, indent);
	this->myLhs->unparse(out, 0);
	out << " == ";
	this->myRhs->unparse(out, 0);
}

void NotEqualsNode::unparse(std::ostream& out, int indent){
	shift(out, indent);
	this->myLhs->unparse(out, 0);
	out << " != ";
	this->myRhs->unparse(out, 0);
}

void GreaterNode::unparse(std::ostream& out, int indent){
	shift(out, indent);
	this->myLhs->unparse(out, 0);
	out << " > ";
	this->myRhs->unparse(out, 0);
}

void GreaterEqNode::unparse(std::ostream& out, int indent){
	shift(out, indent);
	this->myLhs->unparse(out, 0);
	out << " >= ";
	this->myRhs->unparse(out, 0);
}

void LessNode::unparse(std::ostream& out, int indent){
	shift(out, indent);
	this->myLhs->unparse(out, 0);
	out << " < ";
	this->myRhs->unparse(out, 0);
}

void LessEqNode::unparse(std::ostream& out, int indent){
	shift(out, indent);
	this->myLhs->unparse(out, 0);
	out << " <= ";
	this->myRhs->unparse(out, 0);
}

void NotNode::unparse(std::ostream& out, int indent){
	shift(out, indent);
	out << "!";
	this->myExp->unparse(out, 0);
}

void NegNode::unparse(std::ostream& out, int indent){
	shift(out, indent);
	out << "-";
	this->myExp->unparse(out, 0);
}

void FormalDeclNode::unparse(std::ostream& out, int indent){
	shift(out, indent);
	this->myType->unparse(out, 0);
	out << " ";
	this->myId->unparse(out, 0);
}

void NullPtrNode::unparse(std::ostream& out, int indent){
	shift(out, indent);
	out<<"NULLPTR";
}

void IntLitNode::unparse(std::ostream& out, int indent){
	shift(out, indent);
	out << myInt;
}

void StrLitNode::unparse(std::ostream& out, int indent){
	shift(out, indent);
	out << myStr;
}

void CharLitNode::unparse(std::ostream& out, int indent){
	shift(out, indent);
	out << myChar;
}

void TrueNode::unparse(std::ostream& out, int indent){
	shift(out, indent);
	out << "True";
	
}

void FalseNode::unparse(std::ostream& out, int indent){
	shift(out, indent);
	out << "False";
}

void LValNode::unparse(std::ostream& out, int indent){
	shift(out, indent);
	this->myId->unparse(out, 0);
}

void IndexNode::unparse(std::ostream& out, int indent){
	shift(out, indent);
	this->myId->unparse(out, 0);
	out<<"[";
	this->myExp->unparse(out, 0);
	out<<"]";
}

void DerefNode::unparse(std::ostream& out, int indent){
	shift(out, indent);
	out << "@";
	this->myId->unparse(out, 0);
}

void RefNode::unparse(std::ostream& out, int indent){
	shift(out, indent);
	out << "^";
	this->myId->unparse(out, 0);
}

void CallExpNode::unparse(std::ostream& out, int indent){
	shift(out, indent);
	this->myId->unparse(out, 0);
	out << "(";
	for (auto param : *myParams)
		param->unparse(out, 0);
	out << ")";
}

void VoidTypeNode::unparse(std::ostream& out, int indent){
	out << "void";
}

void IDNode::unparse(std::ostream& out, int indent){
	out << this->myStrVal;
}

void IntTypeNode::unparse(std::ostream& out, int indent){
	out << "int";
}

void BoolTypeNode::unparse(std::ostream& out, int indent){
	out << "bool";
}

void IntPtrNode::unparse(std::ostream& out, int indent){
	out << "int *";
}

void CharTypeNode::unparse(std::ostream& out, int indent){
	out << "char";
}

void BoolPtrNode::unparse(std::ostream& out, int indent){
	out << "bool *";
}

void CharPtrNode::unparse(std::ostream& out, int indent){
	out << "char *";
}

} // End namespace holeyc