
#include "common.h"

#include "tsvm.hpp"
#include <string>
#include <fstream>
#include <vector>

using namespace std;
using namespace trainscript;

bool filedata(string name, vector<char> &buffer)
{
	std::ifstream file(name.c_str(), std::ios::binary);
	file.seekg(0, std::ios::end);
	std::streamsize len = file.tellg();
	file.seekg(0, std::ios::beg);

	if(len == -1)
		return false;

	printf("size: %d\n", len);

	buffer.resize(len);

	file.read(buffer.data(), buffer.size());

	return true;
}

int main(int argc, char** argv)
{
	if(argc < 2) {
		printf("tsvm <file>\n");
		return 1;
	}
	vector<char> buffer;
	if(filedata(argv[1], buffer) == false) {
		printf("File not found\n");
		return 2;
	}

	Module *module = VM::load(buffer.data(), buffer.size());
	if(module == nullptr) {
		printf("Could not compile file.\n");
		return 3;
	}

	// This should be replaced by parsing.... :P
	{
		Block *block = new Block(module);

		block->instructions.push_back(new DebugInstruction(module, "hello world!"));
		block->instructions.push_back(new DebugVariableInstruction(module, "x"));

		Method *m = new Method(module, block);
		m->arguments.push_back({"x", Variable(TypeID::Int)});
		module->methods.insert({"main", m});
	}

	Method *scriptMain = module->method("main");
	if(scriptMain== nullptr) {
		printf("'main' method not found.\n");
		return 4;
	}

	scriptMain->invoke({ Variable(15) });

	return 0;
}





