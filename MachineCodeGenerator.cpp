#include "MachineCodeGenerator.hpp"
#include <cstdlib>

enum Reg { eax, ebx, ecx, edx };

program littleEndian(int num, int bytes) {
	if (bytes == 0) return program();
	return program(num & 0xFF) + littleEndian(num >> 8, bytes - 1);
}

program MOV(Reg reg, int num) {
	if (reg == eax) {
		return program(0xb8) + littleEndian(num, 4);
	}
};

program RET() { return program(0xC3); }

program machineCodeGenerator(Tree myTree) {
	if (myTree.myType == constantInt)
		return MOV(eax, atol(myTree.myToken.text.c_str())) + RET();
}
