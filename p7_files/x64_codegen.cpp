#include <ostream>
#include "3ac.hpp"
#include "err.hpp"

namespace holeyc
{

	void IRProgram::allocGlobals()
	{
		TODO(Write out data section)
		// int i = 1;
		// for(auto  cur : globals){
		// cur.second->myLoc = "gbl_g" + to_string(i);
		// i++;
	}

	void IRProgram::datagenX64(std::ostream &out)
	{
		TODO(Write out data section)
		// call allocGlobals()

		/**
		* Write out data section with all .ascci and .quad
		* Do quad first then .asci
		* */

		// Put this directive after you write out strings
		// so that everything is aligned to a quadword value
		// again

		out << ".data";
		out << ".str_text:";
		for (auto cur : strings)
		{
			out << "\t.asciz \"" + cur.second + "\";\n";
		}
		out << ".align 8\n";
	}

	void IRProgram::toX64(std::ostream &out)
	{
		allocGlobals();
		for (auto proc : getProcs())
		{
			proc->toX64(out);
		}
	}

	void Procedure::allocLocals()
	{
		TODO(Implement me)
	}

	void Procedure::toX64(std::ostream &out)
	{
		//Allocate all locals
		allocLocals();
		// Report::fatal(3,4,"sdf");

		if (myName == "main")
		{

			out << "main:\n";
		}
		else
		{

			enter->codegenLabels(out);
		}

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
		{
			return;
		}

		size_t numLabels = labels.size();
		size_t labelIdx = 0;
		for (Label *label : labels)
		{
			out << label->toString() << ": ";
			if (labelIdx != numLabels - 1)
			{
				out << "\n";
			}
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
		dst->genStore(out, "rax");
		src->genLoad(out, "rax");
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
		TODO(Implement me)
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
			{
				out << "callq printInt\n";
			}
			else if (myArg->getWidth() == BYTE)
			{
				out << "callq printByte\n";
			}
			else
			{
				// If the argument is an ADDR,
				// assume it's a string
				out << "callq printString";
			}
			break;
		case INPUT:
			TODO("IMPLEMENT ME");
		}
	}

	void CallQuad::codegenX64(std::ostream &out)
	{
		TODO(Implement me)
	}

	void EnterQuad::codegenX64(std::ostream &out)
	{
		out << "Enter " << myProc->getName() << "\n";
		// myProc->toX64(out);
	}

	void LeaveQuad::codegenX64(std::ostream &out)
	{
		out << "Leave " << myProc->getName() << "\n";
		// myProc->toX64(out);
	}

	void SetArgQuad::codegenX64(std::ostream &out)
	{
		//  6 args with function
		if (index == 1)
		{
			out << "movq $" << opd->valString() << ", %rdi\n";
		}
		else if (index == 2)
		{
			out << "movq $" << opd->valString() << ", %rax\n";
		}
		else if (index == 3)
		{
			out << "movq $" << opd->valString() << ", %rsi\n";
		}
		else if (index == 4)
		{
			out << "movq $" << opd->valString() << ", %rdx\n";
		}
		else if (index == 5)
		{
			out << "movq $" << opd->valString() << ", %rcx\n";
		}
		else if (index == 6)
		{
			out << "movq $" << opd->valString() << ", %rsi\n";
		}
		else if (index == 7)
		{
			out << "movq $" << opd->valString() << ", %rsp\n";
		}
	}

	void GetArgQuad::codegenX64(std::ostream &out)
	{
		// nothing needed here
	}

	void SetRetQuad::codegenX64(std::ostream &out)
	{
		opd->genLoad(out,"%rax");
	}

	void GetRetQuad::codegenX64(std::ostream &out)
	{
		opd->genStore(out,"%rax");
	}

	void SymOpd::genLoad(std::ostream &out, std::string regStr)
	{
		out << "movq" << myLoc << ", " << regStr << "\n";
	}

	void SymOpd::genStore(std::ostream &out, std::string regStr)
	{
		out << "movq" << regStr << ", " << myLoc << "\n";
	}

	void AuxOpd::genLoad(std::ostream &out, std::string regStr)
	{
		out << "movq " << myLoc << "(%rbp), " << regStr << "\n";
	}

	void AuxOpd::genStore(std::ostream &out, std::string regStr)
	{
		out << "movq " << regStr << "(%rbp), " << myLoc << "\n";
	}

	void LitOpd::genLoad(std::ostream &out, std::string regStr)
	{
		out << "movq $" << val << ", " << regStr << "\n";
	}

	void LitOpd::genStore(std::ostream &out, std::string regStr)
	{
		throw new InternalError("Cannot use literal as l-val");
	}

} // namespace holeyc