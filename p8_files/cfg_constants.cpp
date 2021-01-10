#include "cfg_constants.hpp"

using namespace holeyc;

bool ConstantsAnalysis::runGraph(ControlFlowGraph *cfg)
{
	//This function should iterate over each block, much like the
	// dead code elimination pass. However, constant propagation
	// is a FORWARD analysis, so the inFacts will be made up
	// of the union of PREDECESSORS, rather than the union of SUCCESSORS
	// std::list<Quad *> *quads = block->getQuads();

	bool changed = true;
	for (BasicBlock *block : *cfg->getBlocks())
	{
		ConstantsFacts empty;
		inFacts[block] = empty;
		outFacts[block] = empty;
	}
	IRProgram *prog = cfg->getProc()->getProg();
	std::set<Opd *> globalSyms = prog->globalSyms();

	while (changed)
	{
		changed = false;
		for (BasicBlock *block : *cfg->getBlocks())
		{
			ConstantsFacts in = inFacts[block];
			for (BasicBlock *block : cfg->blockPredecessors(block))
			{
				in.addFacts(outFacts[block]);
			}
			in.gen(globalSyms);
			inFacts[block] = in;
			bool blockChange = runBlock(cfg, block);
			if (blockChange)
			{
				changed = true;
			}
		}
	}
	return effectful;
}

bool ConstantsAnalysis::runBlock(ControlFlowGraph *cfg, BasicBlock *block)
{
	//This function should iterate over each quad in the block,
	// much like the dead code elimination pass, except that this
	// is a forward analysis, so you should iterate over the block
	// from the first quad to the last, collecting the known constants

	// There are two things you should do for each quad:
	// 1) Constant Folding
	// if all source operands are constants (i.e. can cast to LitOpd),
	// evaluate the result and replace the quad with a simple assignment
	// 2) Constant Propagation
	// if the source operand of an assignment is a constant, record
	// that fact and keep rolling
	// if the source operand is a variable (SymOpd or AuxOpd), but that
	// variable is known to be constant, replace the

	auto quads = block->getQuads();

	bool effectful = false;
	auto quadItr = quads->begin();
	std::map<std::string, std::string> constants;

	while (quadItr != quads->end())
	{
		auto quad = *quadItr;
		std::cout << quad->toString() << "\n";
		if (auto q = dynamic_cast<AssignQuad *>(quad))
		{
			auto src = q->getSrc();
			auto dst = q->getDst();

			if (auto l = dynamic_cast<LitOpd *>(src))
			{
				constants[dst->valString()] = src->valString();
				Opd *res = new LitOpd(l->valString(), QUADWORD);
				q->setDst(res);
			}
			if (auto l = dynamic_cast<SymOpd *>(src))
			{
				if (constants.count(src->valString()) > 0)
				{
					Opd *res = new LitOpd(constants[src->valString()], QUADWORD);
					q->setSrc(res);
					effectful = true;
				}
			}
			if (auto l = dynamic_cast<AuxOpd *>(src))
			{
				if (constants.count(src->valString()) > 0)
				{
					Opd *res = new LitOpd(constants[src->valString()], QUADWORD);
					q->setSrc(res);
					effectful = true;
				}
			}
		}
		quadItr++;
	}

	return effectful;
}
