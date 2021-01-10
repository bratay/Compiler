#include "3ac.hpp"
#include "cfg.hpp"
#include <set>
#include <algorithm>
#include <unordered_map>

//#include "cfg_dce.hpp"
#include "cfg_constants.hpp"

using namespace holeyc;
using namespace std;

ControlFlowGraph::ControlFlowGraph(CFGFactory * b){
	proc = b->proc;
	exit = b->exitBlock;
	entry = b->entryBlock;
	blocks = b->blocks;
	edges = b->edges;

	assert(exit != nullptr);
	assert(entry != nullptr);
}

void ControlFlowGraph::setEntryBlock(BasicBlock * block){
	assert(entry == nullptr);
	entry = block;
}

BasicBlock * ControlFlowGraph::getEntryBlock(){
	return entry;
}

void ControlFlowGraph::setExitBlock(BasicBlock * block){
	assert(exit == nullptr);
	exit = block;
}

BasicBlock * ControlFlowGraph::getExitBlock(){
	return exit;
}

void ControlFlowGraph::removeBlock(BasicBlock * block){
	blocks->remove(block);

	for (Quad * quad : *block->getQuads()){
		proc->getQuads()->remove(quad);
	}

	auto edgeItr = edges->begin();
	while  (edgeItr != edges->end()){
		auto edge = *edgeItr;
		if (edge->src == block){
			edgeItr = edges->erase(edgeItr);
		} else if (edge->tgt == block){
			edges->remove(edge);
		} else {
			edgeItr++;
		}
	}
}

BasicBlock * ControlFlowGraph::getBlock(Quad * quad){
	for (BasicBlock * block : *blocks){
		std::list<Quad *> * quads = block->getQuads();
		auto qItr = std::find(quads->begin(), quads->end(), quad);
		if (qItr != quads->end()){
			return block;
		}
	}
	return nullptr;
}

std::list<BasicBlock *> * ControlFlowGraph::getBlocks(){ return blocks; }

bool ControlFlowGraph::removeQuad(Quad * quad){
	std::list<Quad *> * allQuads = proc->getQuads();

	BasicBlock * block = getBlock(quad);
	std::list<Quad *> * blockQuads = block->getQuads();

	if (blockQuads->size() == 1){
		return false;
	}

	allQuads->remove(quad);
	blockQuads->remove(quad);
	if (block->getLeader() == quad){
		Label * lbl = quad->getLabel();

		Quad * front = blockQuads->front();
		front->clearLabels();
		front->addLabel(lbl);
		block->setLeader(front);
	}
	if (block->getTerminator() == quad){
		block->setTerminator(blockQuads->back());
	}
	return true;
}

void ControlFlowGraph::replaceWithNop(Quad * quad){
	NopQuad * nop = new NopQuad();
	{
	BasicBlock * block = getBlock(quad);
	std::list<Quad *> * blockQuads = block->getQuads();
	auto pos = std::find(blockQuads->begin(), blockQuads->end(), quad);
	blockQuads->insert(pos, nop);
	blockQuads->erase(pos);

	if (block->getLeader() == quad){
		block->setLeader(nop);
		if (Label * label = quad->getLabel()){
			nop->addLabel(label);
		}
	}
	if (block->getTerminator() == quad){
		block->setTerminator(nop);
	}
	}

	{
	std::list<Quad *> * procQuads = proc->getQuads();
	auto pos = std::find(procQuads->begin(), procQuads->end(), quad);
	procQuads->insert(pos, nop);
	procQuads->erase(pos);
	}
}

std::string ControlFlowGraph::getProcName(){
	return proc->getName();
}

void ControlFlowGraph::optimize(){
	// The dead code elimination pass is left in the codebase
	// for your reference, but you should not run it as part
	// of your project
	// bool dceEffect = DeadCodeElimination::run(this);

	// TODO: implement this code
	bool constantEffect = ConstantsAnalysis::run(this);
}

std::set<BasicBlock *> ControlFlowGraph::blockSuccessors(BasicBlock * block){
	std::set<BasicBlock *> res;
	for (auto edge : *edges){
		if (edge->src == block){
			res.insert(edge->tgt);
		}
	}
	return res;
}

std::set<BasicBlock *> ControlFlowGraph::blockPredecessors(BasicBlock * block){
	std::set<BasicBlock *> res;
	for (auto edge : *edges){
		if (edge->tgt == block){
			res.insert(edge->src);
		}
	}
	return res;
}

static void replaceAllSubstrs(std::string& str, std::string from, std::string to){
	size_t pos = str.find(from);
	while( pos != std::string::npos){
		str.replace(pos, from.size(), to);
		pos = str.find(from, pos + to.size());
	}
}

static void dottifyString(std::string& str){
	replaceAllSubstrs(str, "\"", "\\\"");
	replaceAllSubstrs(str, "[", "\\[");
	replaceAllSubstrs(str, "]", "\\]");
	replaceAllSubstrs(str, "\n", "\\n");
}

void ControlFlowGraph::toDot(std::ostream& out){
	out << "digraph G {\n";
	for (auto block : *blocks){
		out << "blk" << block->getNum() << " [";
		std::string res = block->toString();
		dottifyString(res);
		out << "shape=record ";
		out << "label=\"" << res << "\"";
		out << "]\n";
	}
	for (auto edge : *edges){
		std::string srcNum = std::to_string(edge->src->getNum());
		std::string tgtNum = std::to_string(edge->tgt->getNum());
		std::string edgeLbl = "";
		switch (edge->type){
			case FALL:
				edgeLbl = "FALL";
				break;
			case JUMP:
				edgeLbl = "JUMP";
				break;
			case LINK:
				edgeLbl = "LINK";
				break;
		}
		if (edge->type == FALL){
			edgeLbl = "FALL";
		} else {
		}
		out << "blk" << srcNum << " -> " << "blk" << tgtNum 
		    << "[label=" << edgeLbl << "]" << "\n";
	}

	out << "}\n";
	out << std::flush;
}
