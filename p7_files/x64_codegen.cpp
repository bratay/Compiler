#include <ostream>
#include "3ac.hpp"
#include "err.hpp"

namespace holeyc
{

	void IRProgram::allocGlobals()
	{
		for (auto g : globals)
			g.second->setMemoryLoc("gbl_" + g.second->getName());
	}

	void IRProgram::datagenX64(std::ostream &out)
	{
		for (auto cur : strings)
		{
			out << "str_text:";
			out << "\t.asciz \"" + cur.second + "\";\n";
		}
	}

	void IRProgram::toX64(std::ostream &out)
	{
		out << ".data\n";

		allocGlobals();
		int x = 0;
		for (auto gbl : globals)
		{
			out << gbl.second->myLoc << ": "
				<< ".quad " << x << "\n";
			x++;
		}
		out << ".text\n";
		for (auto proc : getProcs())
			proc->toX64(out);
	}

	void Procedure::allocLocals()
	{
		int s = -24;
		for (auto gbl : locals)
		{
			gbl.second->setMemoryLoc(to_string(s));
			s = s - 8;
		}

		for (auto temp : temps)
		{
			temp->setMemoryLoc(to_string(s));
			s = s - 8;
		}

		for (auto form : formals)
		{
			form->setMemoryLoc(to_string(s));
			s = s - 8;
		}
	}

	void Procedure::toX64(std::ostream &out)
	{
		allocLocals();

		enter->codegenLabels(out);
		enter->codegenX64(out);
		
		for (auto quad : *bodyQuads)
		{
			quad->codegenLabels(out);
			quad->codegenX64(out);
		}

		leave->codegenLabels(out);
		leave->codegenX64(out);
	}

	void Quad::codegenLabels(std::ostream &out)
	{
		if (labels.empty())
			return;

		size_t numLabels = labels.size();
		size_t labelIdx = 0;

		for (Label *lab : labels)
		{
			out << lab->toString() << ": ";
			if (labelIdx != numLabels - 1)
				out << "\n";

			labelIdx++;
		}
	}

	void BinOpQuad::codegenX64(std::ostream &out)
	{
		dst->genStore(out, "rax");
		src1->genLoad(out, "rax");
		src2->genLoad(out, "rax");
	}

	void UnaryOpQuad::codegenX64(std::ostream &out)
	{
		src->genLoad(out, "rax");
		dst->genStore(out, "rax");
	}

	void AssignQuad::codegenX64(std::ostream &out)
	{
		src->genLoad(out, "%rax");
		dst->genStore(out, "%rax");
	}

	void LocQuad::codegenX64(std::ostream &out)
	{
		// (Optional)
		TODO(Implement me)
	}

	void JmpQuad::codegenX64(std::ostream &out)
	{
		out << "jmp " << tgt->toString() << "\n";
	}

	void JmpIfQuad::codegenX64(std::ostream &out)
	{
		cnd->genLoad(out, "%rax");
		std::cout << cnd->locString();

		out << "cmpq $0, %rax\n";
		out << "je lbl_" << tgt->getName() << "\n";
	}

	void NopQuad::codegenX64(std::ostream &out)
	{
		out << "nop"
			<< "\n";
	}

	void IntrinsicQuad::codegenX64(std::ostream &out)
	{
		switch (myIntrinsic)
		{
		case OUTPUT:
			myArg->genLoad(out, "%rdi");
			if (myArg->getWidth() == QUADWORD)
				out << "callq printInt\n";
			else if (myArg->getWidth() == BYTE)
				out << "callq printByte\n";
			else
				out << "callq printString";
			break;
		case INPUT:
			myArg->genStore(out, "%rax");
			if (myArg->getWidth() == QUADWORD)
				out << "callq writeInt\n";
			else if (myArg->getWidth() == BYTE)
				out << "callq writeByte\n";
			else
				out << "callq writeString";
			break;
		}
	}

	void CallQuad::codegenX64(std::ostream &out)
	{
		out << "call " << callee->getName() << "\n";
	}

	void EnterQuad::codegenX64(std::ostream &out)
	{
		out << "pushq %rbp"
			<< "\n";
		out << "movq %rsp, %rbp"
			<< "\n";
		out << "addq %16, %rbp"
			<< "\n";
		out << "subq $" << myProc->localsSize() << ", %rsp"
			<< "\n";
	}

	void LeaveQuad::codegenX64(std::ostream &out)
	{
		out << "addq $" << myProc->localsSize() << ", %rsp"
			<< "\n";
		out << "popq %rbp"
			<< "\n";
		out << "retq"
			<< "\n";
	}

	void SetArgQuad::codegenX64(std::ostream &out)
	{
		if (index == 1)
			opd->genLoad(out, "%rdi");
		else if (index == 2)
			opd->genLoad(out, "%rax");
		else if (index == 3)
			opd->genLoad(out, "%rdx");
		else if (index == 4)
			opd->genLoad(out, "%rcx");
		else if (index == 5)
			opd->genLoad(out, "%r8");
		else if (index == 6)
			opd->genLoad(out, "%r9");
		else
		{
			opd->genLoad(out, "%rax");
			out << "pushq %rax\n";
		}
	}

	void GetArgQuad::codegenX64(std::ostream &out)
	{
		//NA 
	}

	void SetRetQuad::codegenX64(std::ostream &out)
	{
		opd->genLoad(out, "%rax");
	}

	void GetRetQuad::codegenX64(std::ostream &out)
	{
		opd->genStore(out, "%rax");
	}

	void SymOpd::genLoad(std::ostream &out, std::string regStr)
	{
		std::string output = "";
		if (this->getWidth() == BYTE)
		{
			if (this->locString() != "true" && this->locString() != "false")
			{
				char var = this->valString()[1];
				int ascii = int(var);
				output = to_string(ascii);
			}
		}

		out << "movq " << myLoc << "(%rbp), " << regStr << "\n";
		output = myLoc;
	}

	void SymOpd::genStore(std::ostream &out, std::string regStr)
	{
		out << "movq " << regStr << ", " << myLoc << "(%rbp)\n";
	}

	void AuxOpd::genLoad(std::ostream &out, std::string regStr)
	{
		std::string output = "";

		if (this->getWidth() == BYTE)
		{
			if (this->valString() != "true" && this->valString() != "false")
			{
				char var = valString()[1];
				int ascii = int(var);
				output = to_string(ascii);
				out << "movq $" << output << ", " << regStr << "\n";
			}
			else
				out << "movq $" << myLoc << "(%rbp), " << regStr << "\n";
		}
		else
			out << "movq $" << myLoc << "(%rbp). " << regStr << "\n";
	}

	void AuxOpd::genStore(std::ostream &out, std::string regStr)
	{
		out << "movq " << regStr << "(%rbp), " << myLoc << "\n";
	}

	void LitOpd::genLoad(std::ostream &out, std::string regStr)
	{
		std::string output = "";
		if (this->getWidth() == BYTE)
		{
			if (this->valString() != "true" && this->valString() != "false")
			{
				char temp = val[1];
				int ascii = int(temp);
				output = to_string(ascii);
				out << "LitASCII " << valString();
				out << "movq $" << output << ", " << regStr << "\n";
			}
			else
				out << "movq $" << val << ", " << regStr << "\n";
		}
		else
			out << "movq $" << val << ". " << regStr << "\n";
	}

	void LitOpd::genStore(std::ostream &out, std::string regStr)
	{
		throw new InternalError("Cannot use literal as l-val");
	}

} // namespace holeyc
