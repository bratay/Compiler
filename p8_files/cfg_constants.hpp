#ifndef HOLEYC_CFG_CONSTANTS
#define HOLEYC_CFG_CONSTANTS

#include <map>
#include "cfg.hpp"
#include "3ac.hpp"

namespace holeyc{

enum ConstantValType {INTVAL, CHARVAL, BOOLVAL, TOPVAL};

/**
* This class represents the possible value that an Opd might take on at
* at some program point. Note that if the Opd might take on more than 
* one possible value at a program point, it is simply represented as
* TOPVAL (regardless of what the values are). This use of TOPVAL causes the 
* algorithm to reach saturation much faster. 
**/
class ConstantVal{
public:
	ConstantValType type;

	int intVal;
	char charVal;
	bool boolVal;
	void setInt(int val){ intVal = val; type = INTVAL; } 
	void setBool(bool val){ boolVal = val; type = BOOLVAL; } 
	void setChar(char val){ charVal = val; type = CHARVAL; } 
	void setTop(){ type = TOPVAL; } 


	void merge(ConstantVal other){
		if (other.type == TOPVAL) {
			setTop(); 
		} else if (other.type == INTVAL) { 
			if (intVal != other.intVal){ setTop(); }
		} else if (other.type == CHARVAL) { 
			if (charVal != other.charVal){ setTop(); }
		} else if (other.type == BOOLVAL) { 
			if (intVal != other.intVal){ setTop(); }
		}
	}
};

class ConstantsFacts{
public:
	ConstantsFacts(){}
	ConstantsFacts clone(){
		ConstantsFacts facts;
		return facts;
	}
	void addFacts(DeadCodeFacts& other){ this->gen(other.liveOpds); }
	void gen(Opd * opd, ConstantVal v){
		auto itr = vals.find(opd);
		if (itr == vals.end()){
			vals[opd] = v;
		} else {
			ConstantVal cur = itr->second;
			
		}
	}
	void kill(Opd * opd){
		vals.erase(opd);
	}
	//TODO: You'll probably want to add some extra convenience functions
	// to, for example, merge two constantsFacts objects, or determine if
	// a given set of values changes a fact set to determine if you've reached
	// saturation.
private:
	std::map<Opd *, ConstantVal> vals;
};

class ConstantsAnalysis{
public:
	static bool run(ControlFlowGraph * cfg){
		ConstantsAnalysis ca;
		return ca.runGraph(cfg);
	}
private:
	ConstantsAnalysis() : effectful(false){}
	bool runGraph(ControlFlowGraph * cfg); 
	bool runBlock(ControlFlowGraph * cfg, BasicBlock * block); 

	std::map<BasicBlock *, ConstantsFacts> outFacts;
	std::map<BasicBlock *, ConstantsFacts> inFacts;
	bool effectful;
};

}

#endif
