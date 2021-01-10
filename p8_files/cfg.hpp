#ifndef HOLEYC_CFG
#define HOLEYC_CFG

#include "3ac.hpp"
#include <set>
#include <algorithm>
#include <unordered_map>

namespace holeyc{

class CFGFactory;
class CFGEdge;

enum CFGEdgeType{
	FALL, JUMP, LINK
};

class BasicBlock{
public:
	BasicBlock(int numIn, std::list<Quad *> * quadsIn, Quad * leaderIn, Quad * terminatorIn) 
	: num(numIn), quads(quadsIn), leader(leaderIn), terminator(terminatorIn){
	}
	Quad * getLeader() { return leader; }
	void setLeader(Quad * quad) { leader = quad; }
	Quad * getTerminator() { return terminator; }
	void setTerminator(Quad * quad) { terminator = quad; }
	std::list<Quad *> * getQuads() { return quads; }
	std::string toString();
	int getNum(){ return num; }

	void optimize();

private:
	const int num;
	std::list<Quad *> * quads;
	Quad * leader;
	Quad * terminator;
	std::string id;
};

class CFGEdge{
public:
	CFGEdge(BasicBlock * srcIn, BasicBlock * tgtIn, CFGEdgeType typeIn)
	: src(srcIn), tgt(tgtIn), type(typeIn) {}
	BasicBlock * src;
	BasicBlock * tgt;
	CFGEdgeType type;
};

class ControlFlowGraph{
public:
	ControlFlowGraph(CFGFactory * g);
	void setEntryBlock(BasicBlock * block);
	void setExitBlock(BasicBlock * block);
	BasicBlock * getEntryBlock();
	BasicBlock * getExitBlock();
	BasicBlock * getBlock(Quad * quad);
	std::list<BasicBlock *> * getBlocks();
	void toDot(std::ostream& out);
	Procedure * getProc(){ return proc; }
	std::string getProcName();
	void removeBlock(BasicBlock * block);
	bool removeQuad(Quad * quad);
	void replaceWithNop(Quad * quad);
	void removeUnreachableBlocks();
	void cutJmpToNext();
	std::set<BasicBlock *> blockSuccessors(BasicBlock * block);
	std::set<BasicBlock *> blockPredecessors(BasicBlock * block);

	void optimize();
	void deadCodeElimination();
private:
	std::list<BasicBlock *> * blocks;
	std::list<CFGEdge *> * edges;
	BasicBlock * entry = nullptr;
	BasicBlock * exit = nullptr;
	Procedure * proc;
};

class CFGFactory{
public:
	static ControlFlowGraph * buildCFG(Procedure * procIn);
private:
	CFGFactory(){}
	void sequenceQuads();
	void gatherQuadEdges();
	void markLeadersAndTerminators();
	void buildBlocks();
	void buildCFGEdges();

	std::unordered_map<Quad *, Quad *> jmpEdges;
	std::unordered_map<Quad *, Quad *> fallthroughEdges;
	std::unordered_map<Quad *, Quad *> linkEdges;

	std::unordered_map<Quad *, Quad *> prevQuad;
	std::unordered_map<Quad *, Quad *> nextQuad;

	std::list<CFGEdge *> * edges;
	std::list<BasicBlock *> * blocks;
	BasicBlock * entryBlock = nullptr;
	BasicBlock * exitBlock = nullptr;

	std::set<Quad *> leaders;
	std::set<Quad *> terminators;

	Procedure * proc;
	
friend ControlFlowGraph;
};

}

#endif
