#include "cfg.hpp"
#include <set>
#include <algorithm>
#include <unordered_map>

using namespace holeyc;
using namespace std;

std::string BasicBlock::toString(){
	std::string res = "";
	for (auto quad : *quads){
		res += quad->toString() + "\n";
	}
	return res;
}


void BasicBlock::optimize(){
	TODO(Block local and instruction-level optimizations)
}
