#ifndef HOLEYC_CFG_DCE
#define HOLEYC_CFG_DCE

namespace holeyc{

class DeadCodeFacts{
public:
	DeadCodeFacts(){}
	DeadCodeFacts clone(){
		DeadCodeFacts facts;
		for (Opd * opd : liveOpds){ facts.liveOpds.insert(opd); }
		return facts;
	}
	void addFacts(DeadCodeFacts& other){ this->gen(other.liveOpds); }
	void gen(std::set<Opd *>& opds){
		for(auto o : opds){ liveOpds.insert(o); }
	}
	void kill(std::set<Opd *>& opds){
		for (auto o : opds){ liveOpds.erase(o); }
	}
	bool contains(std::set<Opd *>& opds){
		for (auto o : opds){
			auto itr = std::find(liveOpds.begin(), liveOpds.end(), o);
			if (itr == liveOpds.end()){
				return false;
			}
		}
		return true;
	}
	bool sameAs(DeadCodeFacts& other){
		if (!other.contains(this->liveOpds)){ return false; }
		if (!this->contains(other.liveOpds)){ return false; }
		return true;
	}
private:
	std::set<Opd *> liveOpds;
};

class DeadCodeElimination{
public:
	static bool run(ControlFlowGraph * cfg){
		DeadCodeElimination dce;
		return dce.runGraph(cfg);
	}
private:
	DeadCodeElimination() : effectful(false){}
	bool runGraph(ControlFlowGraph * cfg);
	bool runBlock(ControlFlowGraph * cfg, BasicBlock * block);
	bool immune(Quad * quad);

	bool effectful;
	std::map<BasicBlock *, DeadCodeFacts> outFacts;
	std::map<BasicBlock *, DeadCodeFacts> inFacts;
};

}

#endif
