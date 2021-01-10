#include <fstream>
#include <string.h>

#include "errors.hpp"
#include "scanner.hpp"
#include "ast.hpp"
#include "name_analysis.hpp"
#include "type_analysis.hpp"
#include "3ac.hpp"
#include "cfg.hpp"

using namespace std;
using namespace holeyc;

static void usageAndDie(){
	std::cerr << "Usage: holeycc <infile> <options>"
	<< " [-t <tokensFile>]"
	<< " [-p]"
	<< " [-u <unparseFile>]"
	<< " [-n <nameFile>]"
	<< " [-c]"
	<< " [-a <3ACFile>]"
	<< " [-o <ASMFile>]"
	<< " [-z]"
	<< " [-d <CFGDir>]"
	<< "\n"
	;
	std::cout << std::flush;
	std::cerr << std::flush;
	exit(1);
}

static std::ifstream * openInput(const char * inputPath){
	if (inputPath == nullptr){ usageAndDie(); }

	std::ifstream * input = new std::ifstream(inputPath);
	if (input == NULL){ usageAndDie(); }
	if (!input->good()){
		std::cerr << "Bad path " <<  inputPath << std::endl;
		usageAndDie();
	}
	return input;
}

static void doTokenization(const char * inputPath, const char * outPath){
	std::ifstream * input = openInput(inputPath);

	holeyc::Scanner scanner(input);
	if (strcmp(outPath, "--") == 0){
		scanner.outputTokens(std::cout);
	} else {
		std::ofstream outStream(outPath);
		if (!outStream.good()){
			std::string msg = "Bad output file ";
			msg += outPath;
			throw new holeyc::InternalError(msg.c_str());
		}
		scanner.outputTokens(outStream);
	}
}

static holeyc::ProgramNode * syntacticAnalysis(const char * inputPath){
	std::ifstream * input = openInput(inputPath);
	if (input == nullptr){
		return nullptr;
	}

	holeyc::ProgramNode * root = nullptr;

	holeyc::Scanner scanner(input);
	#if 1
	holeyc::Parser parser(scanner, &root);
	#else
	holeyc::Parser parser(scanner);
	#endif

	int errCode = parser.parse();
	if (errCode != 0) { 
		return nullptr; 
	}
	
	return root;
}

static void outputAST(ASTNode * ast, const char * outPath){
	if (strcmp(outPath, "--") == 0){
		ast->unparse(std::cout, 0);
	} else {
		std::ofstream outStream(outPath);
		if (!outStream.good()){
			std::string msg = "Bad output file ";
			msg += outPath;
			throw new holeyc::InternalError(msg.c_str());
		}
		ast->unparse(outStream, 0);
	}
}

static bool doUnparsing(const char * inputPath, const char * outPath){
	holeyc::ProgramNode * ast = syntacticAnalysis(inputPath);
	if (ast == nullptr){ 
		std::cerr << "No AST built\n";
		return false;
	}

	outputAST(ast, outPath);
	return true;
}

static holeyc::NameAnalysis * doNameAnalysis(const char * inputPath){
	holeyc::ProgramNode * ast = syntacticAnalysis(inputPath);
	if (ast == nullptr){ return nullptr; }

	return holeyc::NameAnalysis::build(ast);
}

static holeyc::TypeAnalysis * doTypeAnalysis(const char * inputPath){
	holeyc::NameAnalysis * nameAnalysis = doNameAnalysis(inputPath);
	if (nameAnalysis == nullptr){ return nullptr; }

	return holeyc::TypeAnalysis::build(nameAnalysis);
}

static void write3AC(holeyc::IRProgram * prog, const char * outPath){
	if (outPath == nullptr){
		throw new InternalError("Null 3AC flat file given");
	}
	std::string flatProg = prog->toString();
	if (strcmp(outPath, "--") == 0){
		std::cout << flatProg << std::endl;
	} else {
		std::ofstream outStream(outPath);
		outStream << flatProg << std::endl;
		outStream.close();
	}
}

static list<ControlFlowGraph *> * getCFGs(IRProgram * prog){
	std::list<ControlFlowGraph *> * cfgs;
	cfgs = new std::list<ControlFlowGraph *>();
	for (auto proc : *prog->getProcs()){
		ControlFlowGraph * cfg = CFGFactory::buildCFG(proc);
		cfgs->push_back(cfg);
	}
	return cfgs;
}

static IRProgram * do3AC(const char * inputPath){
	holeyc::TypeAnalysis * typeAnalysis = doTypeAnalysis(inputPath);
	if (typeAnalysis == nullptr){ return nullptr; }
	
	IRProgram * prog = typeAnalysis->ast->to3AC(typeAnalysis);
	return prog;
}

static void writeCFGs(std::list<ControlFlowGraph *> *cfgs, const char * cfgDir){
	std::ostream * o = &std::cout;
	for (auto cfg : *cfgs){
		if (strncmp(cfgDir, "--", 2) != 0){
			std::string path = cfgDir;
			path += "/fn_" + cfg->getProcName() + ".dot";
			std::cout << "path: " << path << "\n";
			o = new std::ofstream(path);
		}
		cfg->toDot(*o);
	}
}

int main(int argc, char * argv[]){
	if (argc <= 1){ usageAndDie(); }
	const char * input = argv[1];

	const char * tokensFile = nullptr; // Output file if 
	                                   // printing tokens
	bool checkParse = false;	   // Flag set if doing 
					   // syntactic analysis
	const char * unparseFile = NULL;   // Output file if 
	                                   // unparsing
	const char * nameFile = NULL;	   // Output file if doing
					   // name analysis
	bool checkTypes = false;	   // Flag set if doing
					   // type analysis
	const char * threeACFile = NULL;   // Output file for
					   // 3AC representation
	const char * asmFile = NULL;       // Output file for
					   // X64 representation
	bool doOptimize = false;           // 
	const char * cfgDir = NULL;        // 
	
	bool useful = false; // Check whether the command is 
                         // a no-op
	for (int i = 1; i < argc; i++){
		if (argv[i][0] == '-'){
			if (argv[i][1] == 't'){
				i++;
				if (i >= argc){ usageAndDie(); }
				tokensFile = argv[i];
				useful = true;
			} else if (argv[i][1] == 'p'){
				checkParse = true;
				useful = true;
			} else if (argv[i][1] == 'u'){
				i++;
				if (i >= argc){ usageAndDie(); }
				unparseFile = argv[i];
				useful = true;
			} else if (argv[i][1] == 'n'){
				i++;
				if (i >= argc){ usageAndDie(); }
				nameFile = argv[i];
				useful = true;
			} else if (argv[i][1] == 'c'){
				checkTypes = true;
				useful = true;
			} else if (argv[i][1] == 'a'){
				i++;
				if (i >= argc){ usageAndDie(); }
				threeACFile = argv[i];
				useful = true;
			} else if (argv[i][1] == 'o'){
				i++;
				if (i >= argc){ usageAndDie(); }
				else { asmFile = argv[i]; }
				useful = true;
			} else if (argv[i][1] == 'z'){
				doOptimize = true;
			} else if (argv[i][1] == 'd'){
				i++;
				if (i >= argc){ usageAndDie(); }
				else { cfgDir = argv[i]; }
				useful = true;
			} else {
				std::cerr << "Unknown option"
				  << " " << argv[i] << "\n";
				usageAndDie();
			}
		}
	}

	if (useful == false){
		std::cerr << "You didn't specify an operation to do!\n";
		usageAndDie();
	}


	try {
		if (tokensFile != nullptr){
			doTokenization(input, tokensFile);
		}
		if (checkParse){
			if (!syntacticAnalysis(input)){
				std::cerr << "Parse failed";
			}
		}
		if (unparseFile != nullptr){
			doUnparsing(input, unparseFile);
		}
		if (nameFile){
			holeyc::NameAnalysis * na;
			na = doNameAnalysis(input);
			if (na == nullptr){
				std::cerr << "Name Analysis Failed\n";
				return 1;
			}
			outputAST(na->ast, nameFile);
		}
		if (checkTypes){
			holeyc::TypeAnalysis * ta;
			ta = doTypeAnalysis(input);
			if (ta == nullptr){
				std::cerr << "Type Analysis Failed\n";
				return 1;
			}
		}

		if (threeACFile != NULL){
			auto prog = do3AC(input);
			if (prog == nullptr){ return 1; }
			if (doOptimize){
				auto cfgs = getCFGs(prog);
				for(auto cfg : *cfgs){
					cfg->optimize();
				}
			}
			write3AC(prog, threeACFile);
		}

		if (asmFile != NULL){
			auto prog = do3AC(input);
			if (prog == nullptr){ return 1; }
			if (doOptimize){
				auto cfgs = getCFGs(prog);
				for(auto cfg : *cfgs){
					cfg->optimize();
				}
			}
			std::cerr << "This option is not available for this project";
			std::cerr << " however, you may import your solution from";
			std::cerr << " the previous project to ensure your optimized";
			std::cerr << " code works." << std::endl;
			return 1;
		}

		if (cfgDir != NULL){
			IRProgram * prog = do3AC(input);
			if (prog == nullptr){ return 1; }
			auto cfgs = getCFGs(prog);
			if (doOptimize){
				for(auto cfg : *cfgs){
					cfg->optimize();
				}
			}
			writeCFGs(cfgs, cfgDir);
		}
	} catch (holeyc::ToDoError * e){
		std::cerr << "ToDoError: " << e->msg() << "\n";
		return 1;
	} catch (holeyc::InternalError * e){
		std::cerr << "InternalError: " << e->msg() << "\n";
		return 1;
	}

	return 0;
}
