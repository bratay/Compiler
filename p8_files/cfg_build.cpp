#include "3ac.hpp"
#include "cfg.hpp"
#include <algorithm>

using namespace holeyc;
using namespace std;

ControlFlowGraph * CFGFactory::buildCFG(Procedure * procIn){
	CFGFactory f;
	f.proc = procIn;

	f.sequenceQuads();
	f.gatherQuadEdges();
	f.markLeadersAndTerminators();

	f.buildBlocks();
	f.buildCFGEdges();

	ControlFlowGraph * cfg  = new ControlFlowGraph(&f);
	return cfg;
}

void CFGFactory::sequenceQuads(){
	EnterQuad * enter = proc->getEnter();
	LeaveQuad * leave = proc->getLeave();

	if (proc->getQuads()->empty()){
		nextQuad.insert({enter,leave});
		prevQuad.insert({leave,enter});
		return;
	}

	Quad * trail = nullptr;
	for (auto lead : *(proc->getQuads())){
		if (trail == nullptr){
			trail = lead;
			nextQuad.insert({enter,lead});
			prevQuad.insert({lead,enter});
			continue;
		}

		nextQuad.insert({trail,lead});
		prevQuad.insert({lead,trail});
		trail = lead;
	}
	nextQuad.insert({trail,leave});
	prevQuad.insert({leave,trail});
}

void CFGFactory::gatherQuadEdges(){
	std::unordered_map<Label *, Quad *> labelToTgt;
	Quad * quad = proc->getEnter();
	while (true){
		if (Label * label = quad->getLabel()){
			labelToTgt[label] = quad;
		}
		if (quad == proc->getLeave()){ break; }
		quad = nextQuad[quad];
	}

	quad = proc->getEnter();
	while (true){
		if (JmpQuad * gotoQuad = dynamic_cast<JmpQuad*>(quad)){
			Quad * tgt = labelToTgt[gotoQuad->getLabel()];
			jmpEdges.insert({gotoQuad, tgt});
		} else if (JmpIfQuad * ifzQuad = dynamic_cast<JmpIfQuad*>(quad)){
			Quad * tgt = labelToTgt[ifzQuad->getLabel()];
			jmpEdges.insert({ifzQuad, tgt});
			fallthroughEdges.insert({quad,nextQuad[quad]});
		} else if (auto q = dynamic_cast<CallQuad *>(quad)){
			linkEdges.insert({quad, nextQuad[quad]});
		} else {
			fallthroughEdges.insert({quad,nextQuad[quad]});
		}
		if (quad == proc->getLeave()){ break; }
		quad = nextQuad[quad];
	}
}

void CFGFactory::markLeadersAndTerminators(){
	leaders.insert(proc->getEnter());
	terminators.insert(proc->getLeave());
	for (auto edge : jmpEdges){
		auto src = edge.first;
		auto tgt = edge.second;
		terminators.insert(src);
		leaders.insert(nextQuad[src]);

		leaders.insert(tgt);
		terminators.insert(prevQuad[tgt]);

		auto jmpFall = fallthroughEdges.find(src);
		if (jmpFall != fallthroughEdges.end()){
			auto jmpFallTgt = jmpFall->second;
			leaders.insert(jmpFallTgt);
		}
	}
	for (auto edge : linkEdges){
		auto src = edge.first;
		auto tgt = edge.second;
		terminators.insert(src);
		leaders.insert(tgt);
	}

	Quad * quad = proc->getEnter();
	while (quad != proc->getLeave()){
		if (std::find(leaders.begin(), leaders.end(), quad) != leaders.end()){
			terminators.insert(prevQuad[quad]);
		}
		if (std::find(terminators.begin(), terminators.end(), quad) != terminators.end()){
			leaders.insert(nextQuad[quad]);
		}
		quad = nextQuad[quad];
	}
}

void CFGFactory::buildBlocks(){
	this->entryBlock = nullptr;
	this->exitBlock = nullptr;
	Quad * quad = proc->getEnter();
	std::list<Quad *> * blockQuads = nullptr;
	blocks = new std::list<BasicBlock *>();
	
	Quad * leader = nullptr;
	Quad * terminator = nullptr;
	int num = 1;
	while(quad != proc->getLeave()){
		if (std::find(leaders.begin(), leaders.end(), quad) != leaders.end()){
			blockQuads = new std::list<Quad *>();
			leader = quad;
		}
		blockQuads->push_back(quad);
		if (std::find(terminators.begin(), terminators.end(), quad) != terminators.end()){
			terminator = quad;
			BasicBlock * block = new BasicBlock(num++, blockQuads, leader, terminator);
			if (entryBlock == nullptr){
				entryBlock = block;
			}
			blocks->push_back(block);
			blockQuads = nullptr;
		}

		quad = nextQuad[quad];
	}
	terminator = proc->getLeave();
	if (blockQuads == nullptr){ 
		leader = terminator;
		blockQuads = new std::list<Quad *>(); 
	}
	blockQuads->push_back(terminator);
	exitBlock = new BasicBlock(num++, blockQuads, leader, terminator);
	if (entryBlock == nullptr){
		entryBlock = exitBlock;
	}
	blocks->push_back(exitBlock);
}

void CFGFactory::buildCFGEdges(){
	edges = new std::list<CFGEdge *>();
	for (auto srcBlock : *blocks){
		auto srcQuad = srcBlock->getTerminator();
		auto jmpEdge = jmpEdges.find(srcQuad);
		auto fallEdge = fallthroughEdges.find(srcQuad);
		auto linkEdge = linkEdges.find(srcQuad);
		
		for (auto tgtBlock : *blocks){
			auto tgtQuad = tgtBlock->getLeader();
			if (jmpEdge != jmpEdges.end() && jmpEdge->second == tgtQuad){
				CFGEdge * edge = new CFGEdge(srcBlock, tgtBlock, JUMP);
				edges->push_back(edge);
			}
			if (fallEdge != fallthroughEdges.end() && fallEdge->second == tgtQuad){
				CFGEdge * edge = new CFGEdge(srcBlock, tgtBlock, FALL);
				edges->push_back(edge);
			}
			if (linkEdge != linkEdges.end() && linkEdge->second == tgtQuad){
				CFGEdge * edge = new CFGEdge(srcBlock, tgtBlock, LINK);
				edges->push_back(edge);
			}
		}
	}
}
