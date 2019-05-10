#ifndef __MACHINECODEGENERATOR_HPP__
#define __MACHINECODEGENERATOR_HPP__

#include "parser.hpp"

struct program {
	std::vector<char> buffer;
	int operator() {
		auto exec = (char*)mmap(0, buffer.size(),
		                        PROT_EXEC | PROT_READ | PROT_WRITE,
		                        MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		int line = 0;
		for (auto op : buffer)
			exec[line++] = op;
		int ret = ((int (*)())(exec))() munmap(program, buffer.size());
		return ret;
	}
};

program machineCodeGenerator(Tree myTree);

#endif
