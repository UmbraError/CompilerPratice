#include "MachineCodeGenerator.hpp"
#include <cstdlib>
#include <iostream>
#include <string>
using namespace std::string_literals;

enum Reg { eax, ebx, ecx, edx };

program littleEndian(int num, int bytes) {
	if (bytes == 0) return program();
	return program(num & 0xFF) + littleEndian(num >> 8, bytes - 1);
}

program MOV(Reg reg, int num) {
	if (reg == eax) {
		return program(0xb8) + littleEndian(num, 4);
	}
}

program MOV(Reg reg1, Reg reg2) {
	if (reg1 == ecx)
		if (reg2 == eax) return program({0x89, 0xc1});
}

program ADD(Reg reg1, Reg reg2) {
	// http://shell-storm.org/online/Online-Assembler-and-Disassembler/
	if (reg1 == eax)
		if (reg2 == ecx) return program({0x01, 0xc8});
}

program SUB(Reg reg1, Reg reg2) {
	// http://shell-storm.org/online/Online-Assembler-and-Disassembler/
	if (reg1 == eax)
		if (reg2 == ecx) return program({0x29, 0xc8});
}

program PUSH(Reg reg) {
	if (reg == eax) return program(0x50);
}

program POP(Reg reg) {
	if (reg == ecx) return program(0x59);
}

program RET() { return program(0xC3); }

program nodeMachineCode(Tree myTree) {
	if (myTree.myType == constantInt)
		return MOV(eax, atol(myTree.myToken.text.c_str()));
	if (myTree.myType == addSub)
		if (myTree.myToken.text == "+")
			return nodeMachineCode(myTree.children[0]) + PUSH(eax) +
			       nodeMachineCode(myTree.children[1]) + POP(ecx) +
			       ADD(eax, ecx);
		else
			return nodeMachineCode(myTree.children[1]) + PUSH(eax) +
			       nodeMachineCode(myTree.children[0]) + POP(ecx) +
			       SUB(eax, ecx);
	throw "Unexpected Node Type: "s + std::to_string(myTree.myType);
}

program machineCodeGenerator(Tree myTree) {
	try {
		return nodeMachineCode(myTree) + RET();
	} catch (std::string error) {
		std::cout << "Code Generation ERROR: " << error << std::endl;
		return RET();
	}
}
