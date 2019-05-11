#ifndef __MACHINECODEGENERATOR_HPP__
#define __MACHINECODEGENERATOR_HPP__

#include <sys/mman.h>
#include "parser.hpp"
struct program {
	std::vector<char> buffer;
	int operator()() {
		auto exec = (char*)mmap(0, buffer.size(),
		                        PROT_EXEC | PROT_READ | PROT_WRITE,
		                        MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		int line = 0;
		for (auto op : buffer)
			exec[line++] = op;
		int ret = ((int (*)())(exec))();
		munmap(exec, buffer.size());
		return ret;
	}

	program() {}

	program(int byte) { buffer.push_back((char)(byte)); }
	program(std::vector<int> ops) {
		for (auto op : ops)
			buffer.push_back((char)op);
	}

	program operator+(program rhs) {
		program ret(*this);
		for (auto op : rhs.buffer)
			ret.buffer.push_back(op);
		return ret;
	}

	int size() { return buffer.size(); }
};

program machineCodeGenerator(Tree myTree);

#endif
