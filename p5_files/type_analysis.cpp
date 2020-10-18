#include "ast.hpp"
#include "symbol_table.hpp"
#include "errors.hpp"
#include "types.hpp"
#include "name_analysis.hpp"
#include "type_analysis.hpp"

// (TypeAnalysis *ta, DataType * type)

namespace holeyc
{

	TypeAnalysis *TypeAnalysis::build(NameAnalysis *nameAnalysis)
	{
		//To emphasize that type analysis depends on name analysis
		// being complete, a name analysis must be supplied for
		// type analysis to be performed.
		TypeAnalysis *typeAnalysis = new TypeAnalysis();
		auto ast = nameAnalysis->ast;
		typeAnalysis->ast = ast;

		ast->typeAnalysis(typeAnalysis);

		if (typeAnalysis->hasError)
		{
			return nullptr;
		}

		return typeAnalysis;
	}

	void ProgramNode::typeAnalysis(TypeAnalysis *ta)
	{

		//pass the TypeAnalysis down throughout
		// the entire tree, getting the types for
		// each element in turn and adding them
		// to the ta object's hashMap
		for (auto global : *myGlobals)
		{
			global->typeAnalysis(ta);
		}

		//The type of the program node will never
		// be needed. We can just set it to VOID
		//(Alternatively, we could make our type
		// be error if the DeclListNode is an error)
		ta->nodeType(this, BasicType::produce(VOID));
	}

	void FnDeclNode::typeAnalysis(TypeAnalysis *ta) //, DataType* type)
	{

		//HINT: you might want to change the signature for
		// typeAnalysis on FnBodyNode to take a second
		// argument which is the type of the current
		// function. This will help you to know at a
		// return statement whether the return type matches
		// the current function

		//Note: this function may need extra code

		// ta->nodeType(this, myRetType->getType());
		ta->nodeType(myID, myRetType->getType());

		for (auto stmt : *myFormals)
		{
			stmt->typeAnalysis(ta);
		}

		for (auto stmt : *myBody)
		{
			stmt->typeAnalysis(ta);
		}
	}

	void StmtNode::typeAnalysis(TypeAnalysis *ta)
	{
	}

	void StmtNode::typeAnalysis(TypeAnalysis *ta, DataType *type)
	{
	}

	void AssignStmtNode::typeAnalysis(TypeAnalysis *ta)
	{
		myExp->typeAnalysis(ta);

		//It can be a bit of a pain to write
		// "const DataType *" everywhere, so here
		// the use of auto is used instead to tell the
		// compiler to figure out what the subType variable
		// should be
		auto subType = ta->nodeType(myExp);

		// As error returns null if subType is NOT an error type
		// otherwise, it returns the subType itself
		if (subType->asError())
		{
			ta->nodeType(this, subType);
		}
		else
		{
			ta->nodeType(this, BasicType::produce(VOID));
		}
	}

	void ExpNode::typeAnalysis(TypeAnalysis *ta)
	{
	}

	void ExpNode::typeAnalysis(TypeAnalysis *ta, DataType *type)
	{
	}

	void AssignExpNode::typeAnalysis(TypeAnalysis *ta)
	{
		//TODO: Note that this function is incomplete.
		// and needs additional code

		//Do typeAnalysis on the subexpressions
		myDst->typeAnalysis(ta);
		mySrc->typeAnalysis(ta);

		const DataType *lhs = ta->nodeType(myDst);
		const DataType *rhs = ta->nodeType(mySrc);

		if (lhs->isVoid())
		{
			ta->nodeType(this, ErrorType::produce());
			ta->badWriteVoid(myDst->line(), myDst->col());
			return;
		}

		if (rhs->isVoid())
		{
			ta->nodeType(this, ErrorType::produce());
			ta->badWriteVoid(mySrc->line(), mySrc->col());
			return;
		}

		if (lhs != rhs)
		{
			ta->nodeType(this, ErrorType::produce());
			ta->badAssignOpr(this->line(), this->col());
			return;
		}

		ta->nodeType(this, lhs);
	}

	void DeclNode::typeAnalysis(TypeAnalysis *ta)
	{
	}

	// void DeclNode::typeAnalysis(TypeAnalysis *ta, DataType *type)
	// {
	// }

	void VarDeclNode::typeAnalysis(TypeAnalysis *ta)
	{
		// VarDecls always pass type analysis, since they
		// are never used in an expression. You may choose
		// to type them void (like this), as discussed in class
		ta->nodeType(this, BasicType::produce(VOID));
	}

	void IDNode::typeAnalysis(TypeAnalysis *ta)
	{
		// IDs never fail type analysis and always
		// yield the type of their symbol (which
		// depends on their definition)
		ta->nodeType(this, this->getSymbol()->getDataType());
	}

	void IntLitNode::typeAnalysis(TypeAnalysis *ta)
	{
		ta->nodeType(this, BasicType::produce(INT));
	}

	void CharLitNode::typeAnalysis(TypeAnalysis *ta)
	{
		ta->nodeType(this, BasicType::produce(CHAR));
	}

	void StrLitNode::typeAnalysis(TypeAnalysis *ta)
	{
		ta->nodeType(this, BasicType::produce(STR));
	}

	void NotNode::typeAnalysis(TypeAnalysis *ta)
	{
		myExp->typeAnalysis(ta);
		const DataType *tgtType = ta->nodeType(myExp);

		if (tgtType->isBool() || tgtType->isInt())
		{
			ta->nodeType(this, tgtType);
			return;
		}

		ta->nodeType(this, ErrorType::produce());
		ta->badLogicOpd(this->line(), this->col());
		// ta->badMathOpd(this->line(), this->col());
	}

	void ReturnStmtNode::typeAnalysis(TypeAnalysis *ta, DataType *type)
	{
		// myExp->typeAnalysis(ta);
		// const DataType *nodeType = ta->nodeType(myExp);

		if (myExp == nullptr && type->isVoid())
		{
			ta->nodeType(this, nullptr);
			return;
		}

		if (myExp == nullptr && !type->isVoid())
		{
			ta->badNoRet(this->line(), this->col());
			ta->nodeType(this, ErrorType::produce());
			return;
		}

		myExp->typeAnalysis(ta);
		const DataType *nodeType = ta->nodeType(myExp);

		if (nodeType == type)
		{
			ta->nodeType(this, nullptr);
			return;
		}
		else if (nodeType->isVoid())
		{
			ta->extraRetValue(this->line(), this->col());
			ta->nodeType(this, ErrorType::produce());
			return;
		}
		else if (nodeType != type && !nodeType->isVoid())
		{
			ta->extraRetValue(this->line(), this->col());
			ta->nodeType(this, ErrorType::produce());
			return;
		}

		ta->nodeType(this, nodeType);
	}

	void FromConsoleStmtNode::typeAnalysis(TypeAnalysis *ta)
	{
		myDst->typeAnalysis(ta);
		const DataType *nodeType = ta->nodeType(myDst);

		if (!nodeType->validVarType())
		{
			ta->nodeType(this, ErrorType::produce());
			ta->writeFn(this->line(), this->col());
			return;
		}
		else if (nodeType->isVoid())
		{
			ta->nodeType(this, ErrorType::produce());
			ta->badWriteVoid(this->line(), this->col());
			return;
		}

		ta->nodeType(this, nodeType);
	}

	void ToConsoleStmtNode::typeAnalysis(TypeAnalysis *ta)
	{
		mySrc->typeAnalysis(ta);
		const DataType *nodeType = ta->nodeType(mySrc);

		// if(nodeType->asFn() == nullptr){
		// 	ta->nodeType(this, ErrorType::produce());
		// 	ta->writeFn(this->line(), this->col());
		// 	return;
		// }

		if (!nodeType->validVarType())
		{
			ta->nodeType(this, ErrorType::produce());
			ta->readFn(this->line(), this->col());
			return;
		}
		else if (nodeType->isVoid())
		{
			ta->nodeType(this, ErrorType::produce());
			ta->badWriteVoid(this->line(), this->col());
			return;
		}

		ta->nodeType(this, nodeType);
	}

	void EqualsNode::typeAnalysis(TypeAnalysis *ta)
	{
		myExp1->typeAnalysis(ta);
		myExp2->typeAnalysis(ta);

		const DataType *tgtType = ta->nodeType(myExp1);
		const DataType *srcType = ta->nodeType(myExp2);

		// std::size_t found = tgtType->getString().find("->");
		// if (found != std::string::npos)
		// {
		// 	ta->nodeType(this, ErrorType::produce());
		// 	ta->badMathOpd(myExp1->line(), myExp1->col());
		// 	return;
		// }

		// found = srcType->getString().find("->");
		// if (found != std::string::npos)
		// {
		// 	ta->nodeType(this, ErrorType::produce());
		// 	ta->badMathOpd(myExp2->line(), myExp2->col());
		// 	return;
		// }

		if (tgtType != srcType)
		{
			ta->nodeType(this, ErrorType::produce());
			// ta->badAssignOpr(this->line(), this->col());
			return;
		}

		ta->nodeType(this, tgtType);
	}

	void NotEqualsNode::typeAnalysis(TypeAnalysis *ta)
	{
		myExp1->typeAnalysis(ta);
		myExp2->typeAnalysis(ta);

		const DataType *tgtType = ta->nodeType(myExp1);
		const DataType *srcType = ta->nodeType(myExp2);

		if (tgtType != srcType)
		{
			ta->nodeType(this, ErrorType::produce());
			// ta->badAssignOpr(this->line(), this->col());
			return;
		}

		ta->nodeType(this, tgtType);

		// ta->badMathOpr(this->line(), this->col());
	}

	void LessNode::typeAnalysis(TypeAnalysis *ta)
	{
		myExp1->typeAnalysis(ta);
		myExp2->typeAnalysis(ta);

		const DataType *lhs = ta->nodeType(myExp1);
		const DataType *rhs = ta->nodeType(myExp2);

		if (lhs->isInt() && rhs->isInt())
		{
			ta->nodeType(this, BasicType::produce(INT));
			return;
		}

		if (!lhs->isInt())
		{
			ta->badMathOpr(myExp1->line(), myExp1->col());
			ta->nodeType(this, ErrorType::produce());
		}

		if (!rhs->isInt())
		{
			ta->badMathOpr(myExp2->line(), myExp2->col());
			ta->nodeType(this, ErrorType::produce());
		}
	}

	void GreaterNode::typeAnalysis(TypeAnalysis *ta)
	{
		myExp1->typeAnalysis(ta);
		myExp2->typeAnalysis(ta);

		const DataType *lhs = ta->nodeType(myExp1);
		const DataType *rhs = ta->nodeType(myExp2);

		if (lhs->isInt() && rhs->isInt())
		{
			ta->nodeType(this, BasicType::produce(INT));
			return;
		}

		if (!lhs->isInt())
		{
			ta->badMathOpr(myExp1->line(), myExp1->col());
			ta->nodeType(this, ErrorType::produce());
		}

		if (!rhs->isInt())
		{
			ta->badMathOpr(myExp2->line(), myExp2->col());
			ta->nodeType(this, ErrorType::produce());
		}
	}

	void LessEqNode::typeAnalysis(TypeAnalysis *ta)
	{
		myExp1->typeAnalysis(ta);
		myExp2->typeAnalysis(ta);

		const DataType *lhs = ta->nodeType(myExp1);
		const DataType *rhs = ta->nodeType(myExp2);

		if (lhs->isInt() && rhs->isInt())
		{
			ta->nodeType(this, BasicType::produce(INT));
			return;
		}

		if (!lhs->isInt())
		{
			ta->badMathOpr(myExp1->line(), myExp1->col());
			ta->nodeType(this, ErrorType::produce());
		}

		if (!rhs->isInt())
		{
			ta->badMathOpr(myExp2->line(), myExp2->col());
			ta->nodeType(this, ErrorType::produce());
		}
	}

	void GreaterEqNode::typeAnalysis(TypeAnalysis *ta)
	{
		myExp1->typeAnalysis(ta);
		myExp2->typeAnalysis(ta);

		const DataType *lhs = ta->nodeType(myExp1);
		const DataType *rhs = ta->nodeType(myExp2);

		if (lhs->isInt() && rhs->isInt())
		{
			ta->nodeType(this, BasicType::produce(INT));
			return;
		}

		if (!lhs->isInt())
		{
			ta->badMathOpr(myExp1->line(), myExp1->col());
			ta->nodeType(this, ErrorType::produce());
		}

		if (!rhs->isInt())
		{
			ta->badMathOpr(myExp2->line(), myExp2->col());
			ta->nodeType(this, ErrorType::produce());
		}
	}

	void MinusNode::typeAnalysis(TypeAnalysis *ta)
	{
		myExp1->typeAnalysis(ta);
		myExp2->typeAnalysis(ta);

		const DataType *lhs = ta->nodeType(myExp1);
		const DataType *rhs = ta->nodeType(myExp2);

		if (lhs->isInt() && rhs->isInt())
		{
			ta->nodeType(this, BasicType::produce(INT));
			return;
		}

		if (!lhs->isInt())
		{
			ta->badMathOpr(myExp1->line(), myExp1->col());
			ta->nodeType(this, ErrorType::produce());
		}

		if (!rhs->isInt())
		{
			ta->badMathOpr(myExp2->line(), myExp2->col());
			ta->nodeType(this, ErrorType::produce());
		}
	}

	void PlusNode::typeAnalysis(TypeAnalysis *ta)
	{
		myExp1->typeAnalysis(ta);
		myExp2->typeAnalysis(ta);

		const DataType *lhs = ta->nodeType(myExp1);
		const DataType *rhs = ta->nodeType(myExp2);
		// const FnType *fn = ta->getCurrentFnType(); // ta->nodeType(myExp2);

		// std::size_t found = lhs->getString().find("->");
		// if (found != std::string::npos)
		// {
		// 	ta->nodeType(this, ErrorType::produce());
		// 	ta->badMathOpd(myExp1->line(), myExp1->col());
		// 	return;
		// }

		// found = rhs->getString().find("->");
		// if (found != std::string::npos)
		// {
		// 	ta->nodeType(this, ErrorType::produce());
		// 	ta->badMathOpd(myExp2->line(), myExp2->col());
		// 	return;
		// }



		if (lhs->isInt() && rhs->isInt())
		{
			ta->nodeType(this, BasicType::produce(INT));
			return;
		}

		if (!lhs->isInt())
		{
			ta->badMathOpr(myExp1->line(), myExp1->col());
			ta->nodeType(this, ErrorType::produce());
		}

		if (!rhs->isInt())
		{
			ta->badMathOpr(myExp2->line(), myExp2->col());
			ta->nodeType(this, ErrorType::produce());
		}
	}

	void TimesNode::typeAnalysis(TypeAnalysis *ta)
	{
		myExp1->typeAnalysis(ta);
		myExp2->typeAnalysis(ta);

		const DataType *lhs = ta->nodeType(myExp1);
		const DataType *rhs = ta->nodeType(myExp2);

		if (lhs->isInt() && rhs->isInt())
		{
			ta->nodeType(this, BasicType::produce(INT));
			return;
		}

		if (!lhs->isInt())
		{
			ta->badMathOpr(myExp1->line(), myExp1->col());
			ta->nodeType(this, ErrorType::produce());
		}

		if (!rhs->isInt())
		{
			ta->badMathOpr(myExp2->line(), myExp2->col());
			ta->nodeType(this, ErrorType::produce());
		}
	}

	void DivideNode::typeAnalysis(TypeAnalysis *ta)
	{
		myExp1->typeAnalysis(ta);
		myExp2->typeAnalysis(ta);

		const DataType *lhs = ta->nodeType(myExp1);
		const DataType *rhs = ta->nodeType(myExp2);

		if (lhs->isInt() && rhs->isInt())
		{
			ta->nodeType(this, BasicType::produce(INT));
			return;
		}

		if (!lhs->isInt())
		{
			ta->badMathOpr(myExp1->line(), myExp1->col());
			ta->nodeType(this, ErrorType::produce());
		}

		if (!rhs->isInt())
		{
			ta->badMathOpr(myExp2->line(), myExp2->col());
			ta->nodeType(this, ErrorType::produce());
		}
	}

	void NegNode::typeAnalysis(TypeAnalysis *ta)
	{
		myExp->typeAnalysis(ta);
		const DataType *tgtType = ta->nodeType(myExp);

		if (tgtType->isInt())
		{
			ta->nodeType(this, tgtType);
			return;
		}

		ta->nodeType(this, ErrorType::produce());
		ta->badAssignOpr(this->line(), this->col());
	}

	void PostIncStmtNode::typeAnalysis(TypeAnalysis *ta)
	{
		myLVal->typeAnalysis(ta);

		const DataType *nodeType = ta->nodeType(myLVal);

		if (nodeType->isInt())
		{
			ta->nodeType(this, BasicType::produce(INT));
			return;
		}

		ta->nodeType(this, ErrorType::produce());
	}

	void PostDecStmtNode::typeAnalysis(TypeAnalysis *ta)
	{
		myLVal->typeAnalysis(ta);

		const DataType *nodeType = ta->nodeType(myLVal);

		if (nodeType == BasicType::produce(INT))
		{
			ta->nodeType(this, BasicType::produce(INT));
			return;
		}

		ta->nodeType(this, ErrorType::produce());
	}

	void BinaryExpNode::typeAnalysis(TypeAnalysis *ta)
	{
		myExp1->typeAnalysis(ta);
		myExp2->typeAnalysis(ta);

		const DataType *lhs = ta->nodeType(myExp1);
		const DataType *rhs = ta->nodeType(myExp2);

		if (lhs != rhs)
		{
			ta->badMathOpr(this->line(), this->col());
			ta->nodeType(this, ErrorType::produce());
			return;
		}

		if (lhs->isBool())
			ta->nodeType(this, BasicType::produce(BOOL));
		else if (lhs->isInt())
			ta->nodeType(this, BasicType::produce(INT));
		else if (lhs->isChar())
			ta->nodeType(this, BasicType::produce(CHAR));
	}

	// void CallExpNode::typeAnalysis(TypeAnalysis *ta, DataType *type)
	// void CallExpNode::typeAnalysis(TypeAnalysis *ta)
	// {
	// 	myID->typeAnalysis(ta);
	// 	const DataType *nodeType = ta->nodeType(myID);

	// 	// if (nodeType != type)
	// 	// {
	// 	// 	ta->badMathOpd(this->line(), this->col());
	// 	// 	ta->nodeType(this, ErrorType::produce());
	// 	// 	return;
	// 	// }

	// 	ta->nodeType(this, nodeType);
	// }

	void AndNode::typeAnalysis(TypeAnalysis *ta)
	{
		myExp1->typeAnalysis(ta);
		myExp2->typeAnalysis(ta);

		const DataType *lhs = ta->nodeType(myExp1);
		const DataType *rhs = ta->nodeType(myExp2);

		if (lhs->isBool() && rhs->isBool())
		{
			ta->nodeType(this, rhs);
			return;
		}

		if (!lhs->isBool())
		{
			ta->badLogicOpd(myExp1->line(), myExp1->col());
			ta->nodeType(this, ErrorType::produce());
		}

		if (!rhs->isBool())
		{
			ta->badLogicOpd(myExp2->line(), myExp2->col());
			ta->nodeType(this, ErrorType::produce());
		}
	}

	void OrNode::typeAnalysis(TypeAnalysis *ta)
	{
		myExp1->typeAnalysis(ta);
		myExp2->typeAnalysis(ta);

		const DataType *lhs = ta->nodeType(myExp1);
		const DataType *rhs = ta->nodeType(myExp2);

		if (lhs->isBool() && rhs->isBool())
		{
			ta->nodeType(this, rhs);
			return;
		}

		if (!lhs->isBool())
		{
			ta->badLogicOpd(myExp1->line(), myExp1->col());
			ta->nodeType(this, ErrorType::produce());
		}

		if (!rhs->isBool())
		{
			ta->badLogicOpd(myExp2->line(), myExp2->col());
			ta->nodeType(this, ErrorType::produce());
		}
	}

	void UnaryExpNode::typeAnalysis(TypeAnalysis *ta)
	{
		myExp->typeAnalysis(ta);

		const DataType *node = ta->nodeType(myExp);

		if (node->asFn() == nullptr)
		{
			ta->badMathOpr(this->line(), this->col());
			ta->nodeType(this, ErrorType::produce());
			return;
		}

		ta->nodeType(this, node);
	}

	void FalseNode::typeAnalysis(TypeAnalysis *ta)
	{
		ta->nodeType(this, BasicType::produce(BOOL));
	}

	void TrueNode::typeAnalysis(TypeAnalysis *ta)
	{
		ta->nodeType(this, BasicType::produce(BOOL));
	}

} // namespace holeyc
