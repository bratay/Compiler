#include "3ac.hpp"
#include "cfg.hpp"
#include "cfg_dce.hpp"

using namespace holeyc;

static void getUseDef(Quad * quad, std::set<Opd *>& uses, std::set<Opd *>& defs){
	uses.clear();
	defs.clear();
	if (auto q = dynamic_cast<BinOpQuad *>(quad)){
		uses.insert(q->getSrc1());
		uses.insert(q->getSrc2());
		defs.insert(q->getDst());
	} else if (auto q = dynamic_cast<UnaryOpQuad *>(quad)){
		uses.insert(q->getSrc());
		defs.insert(q->getDst());
	} else if (auto q = dynamic_cast<AssignQuad *>(quad)){
		uses.insert(q->getSrc());
		defs.insert(q->getDst());
	} else if (auto q = dynamic_cast<JmpIfQuad *>(quad)){
		uses.insert(q->getCnd());
	} else if (auto q = dynamic_cast<IntrinsicOutputQuad *>(quad)){
		uses.insert(q->getSrc());
	} else if (auto q = dynamic_cast<IntrinsicInputQuad *>(quad)){
		defs.insert(q->getDst());
	} else if (auto q = dynamic_cast<SetArgQuad *>(quad)){
		uses.insert(q->getSrc());
	} else if (auto q = dynamic_cast<GetArgQuad *>(quad)){
		defs.insert(q->getDst());
	} else if (auto q = dynamic_cast<SetRetQuad *>(quad)){
		defs.insert(q->getSrc());
	} else if (auto q = dynamic_cast<GetRetQuad *>(quad)){
		defs.insert(q->getDst());
	}
}

bool DeadCodeElimination::immune(Quad * quad){
	if (auto q = dynamic_cast<CallQuad *>(quad)){
		return true;
	} else if (auto q = dynamic_cast<SetRetQuad *>(quad)){
		return true;
	} else if (auto q = dynamic_cast<JmpQuad *>(quad)){
		return true;
	} else if (auto q = dynamic_cast<JmpIfQuad *>(quad)){
		return true;
	} else if (auto q = dynamic_cast<SetArgQuad *>(quad)){
		return true;
	} else if (auto q = dynamic_cast<IntrinsicInputQuad *>(quad)){
		return true;
	} else if (auto q = dynamic_cast<IntrinsicOutputQuad *>(quad)){
		return true;
	} else if (auto q = dynamic_cast<EnterQuad *>(quad)){
		return true;
	} else if (auto q = dynamic_cast<LeaveQuad *>(quad)){
		return true;
	} else if (auto q = dynamic_cast<NopQuad *>(quad)){
		return true;
	}
	return false;
}

bool DeadCodeElimination::runBlock(ControlFlowGraph * cfg, BasicBlock * block){
	std::list<Quad *> * quads = block->getQuads();

	auto quadItr = quads->rbegin();
	DeadCodeFacts facts = this->inFacts[block];
	std::set<Quad *> deadQuads;
	while (quadItr != quads->rend()){
		auto quad = *quadItr;

		std::set<Opd *> uses;
		std::set<Opd *> defs;
		getUseDef(quad, uses, defs);
		
		if (!facts.contains(defs) && !immune(quad)){
			deadQuads.insert(quad);
		} else {
			facts.kill(defs);
			facts.gen(uses);
		}
		quadItr++;
	}

	for (Quad * deadQuad : deadQuads){
		effectful = true;
		if (!cfg->removeQuad(deadQuad)){
			cfg->replaceWithNop(deadQuad);
		}
	}

	DeadCodeFacts oldOut = outFacts[block];
	if (!oldOut.sameAs(facts)){
		outFacts[block] = facts;
		return true;
	}
	return false;
}

bool DeadCodeElimination::runGraph(ControlFlowGraph * cfg){
	bool changed = true;
	for(BasicBlock * block : *cfg->getBlocks()){
		DeadCodeFacts empty;
		inFacts[block] = empty;
		outFacts[block] = empty;
	}
	IRProgram * prog = cfg->getProc()->getProg();
	std::set<Opd *> globalSyms = prog->globalSyms();

	while (changed){
		changed = false;
		for(BasicBlock * block : *cfg->getBlocks()){
			DeadCodeFacts in = inFacts[block];
			for (BasicBlock * block : cfg->blockSuccessors(block)){
				in.addFacts(outFacts[block]);
			}
			in.gen(globalSyms);
			inFacts[block] = in;
			bool blockChange = runBlock(cfg, block);
			if (blockChange){ 
				changed = true; 
			}
		}
	}
	return effectful;
}
