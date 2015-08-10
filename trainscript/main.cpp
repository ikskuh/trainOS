
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

	Method *scriptMain = module->method("main");
	if(scriptMain== nullptr) {
		printf("'main' method not found.\n");
		return 4;
	}

	printf("run...\n");
	Variable result = scriptMain->invoke({ mkvar(10) });
	if(result.type.usable()) {
		printf("done: ");
		result.printval();
		printf("\n");
	} else {
		printf("done.\n");
	}

	// Debug some stuff out:
	for(auto &var : module->variables) {
		printf("Variable: %s : %s = ",
			var.first.c_str(),
			typeName(var.second->type.id));
		switch(var.second->type.id) {
			case TypeID::Int: printf("%d", var.second->integer); break;
			case TypeID::Real: printf("%f", var.second->real); break;
			default: printf("???"); break;
		}
		printf("\n");
	}

	if(result.type == Type::Int) {
		return result.integer;
	} else {
		return 0;
	}
}





