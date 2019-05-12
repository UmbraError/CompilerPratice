#include "MachineCodeGenerator.hpp"
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
using namespace std::string_literals;

namespace Fun {
void print_int(int x) { std::cout << x; }
void print_str(char* s) { std::cout << s; }
}  // namespace Fun

enum Reg { eax, ebx, ecx, edx, rsi, rdi, rax };

program littleEndian(long long int num, int bytes) {
	if (bytes == 0) return program();
	return program(num & 0xFF) + littleEndian(num >> 8, bytes - 1);
}

program MOV(Reg reg, int num) {
	switch (reg) {
		case eax:
			return program(0xb8) + littleEndian(num, 4);
		case edx:
			return program(0xba) + littleEndian(num, 4);
		default:
			throw "Unknown Register for MOV(r,imm)"s;
	}
}

// return MOV(rax, myTree.myToken.text.c_str());
program MOV(Reg reg, char const* cstr) {
	switch (reg) {
		case rdi:
			return program({0x48, 0xbf}) +
			       littleEndian((long long int)cstr, 8);
		default:
			throw "Unknown Register for MOV(r,cstr)"s;
	}
}

template <typename T>
program MOV(Reg reg, void (*fp)(T)) {
	switch (reg) {
		case rsi:
			return program({0x48, 0xbe}) +
			       littleEndian((long long int)fp, 8);
		default:
			throw "Unknown Register for MOV(r,*fp)"s;
	}
}

program MOV(Reg reg1, Reg reg2) {
	if (reg1 == reg2) return program();
	switch (reg1) {
		case ecx:
			switch (reg2) {
				case eax:
					return program({0x89, 0xc1});
				default:
					throw "Unknown Register 2 for MOV(r,r)"s;
			}
		case eax:
			switch (reg2) {
				case edx:
					return program({0x89, 0xd0});
				default:
					throw "Unknown Register 2 for MOV(r,r)"s;
			}
		case rdi:
			switch (reg2) {
				case rax:
					return {{0x89, 0xc7}};
				default:
					throw "Unknown Register 2 for MOV(r,r)"s;
			}
		default:
			throw "Unknown Register 1 for MOV(r,r)"s;
	}
}

program ADD(Reg reg1, Reg reg2) {
	// http://shell-storm.org/online/Online-Assembler-and-Disassembler/
	switch (reg1) {
		case eax:
			switch (reg2) {
				case ecx:
					return program({0x01, 0xc8});
				default:
					throw "Unknown register 2 for ADD(r,r)"s;
			}
		default:
			throw "Unknown register 1 for ADD(r,r)"s;
	}
}

program NEG(Reg reg) {
	switch (reg) {
		case eax:
			return program({0xf7, 0xd8});
		default:
			throw "Unknown register for REG(r)"s;
	}
}

program SUB(Reg reg1, Reg reg2) {
	// http://shell-storm.org/online/Online-Assembler-and-Disassembler/
	switch (reg1) {
		case eax:
			switch (reg2) {
				case ecx:
					return program({0x29, 0xc8});
				default:
					throw "Unknown register 2 for SUB(r,r)"s;
			}
		default:
			throw "Unknown register 1 for SUB(r,r)"s;
	}
}
program MUL(Reg reg1, Reg reg2) {
	switch (reg1) {
		case eax:
			switch (reg2) {
				case ecx:
					return program({0x0f, 0xaf, 0xc1});
				default:
					throw "Unknown register 2 for MUL(r,r)"s;
			}
		default:
			throw "Unknown register 1 for MUL(r,r)"s;
	}
}

program DIV(Reg reg1, Reg reg2) {
	switch (reg1) {
		case eax:
			switch (reg2) {
				case ecx:
					return MOV(edx, 0) +
					       program({0xf7, 0xf9});
				default:
					throw "Unknown register 2 for DIV(r,r)"s;
			}
		default:
			throw "Unknown register 1 for DIV(r,r)"s;
	}
}

program MOD(Reg reg1, Reg reg2) {
	// same as DIV but Quotent is in EDX
	return DIV(reg1, reg2) + MOV(eax, edx);
}

program PUSH(Reg reg) {
	switch (reg) {
		case eax:
			return program(0x50);
		default:
			throw "Unknown register for PUSH(r)"s;
	}
}

program POP(Reg reg) {
	switch (reg) {
		case ecx:
			return program(0x59);
		default:
			throw "Unknown register for POP(r)"s;
	}
}

program CALL(Reg reg) {
	switch (reg) {
		case rsi:
			return {{0xff, 0xd6}};
			// program({0xff,0xd6});
		default:
			throw "Unknown register for Call(r)"s;
	}
}

program PRINT_INT() {
	return MOV(rdi, rax) + MOV(rsi, Fun::print_int) + CALL(rsi);
}

program PRINT_STRING() { return MOV(rsi, Fun::print_str) + CALL(rsi); }

program RET() { return program(0xC3); }

program nodeMachineCode(Tree myTree) {
	// storage of strings so that the pointer does not disappear.
	static std::vector<std::unique_ptr<std::string>> stringTable;

	if (myTree.myType == constantInt)
		return MOV(eax, atol(myTree.myToken.text.c_str()));
	else if (myTree.myType == pstring) {
		stringTable.push_back(
		    std::make_unique<std::string>(myTree.myToken.text));
		return MOV(rdi, stringTable.back()->c_str());
	} else if (myTree.myType == addSub)
		if (myTree.myToken.text == "+")
			return nodeMachineCode(myTree.children[0]) + PUSH(eax) +
			       nodeMachineCode(myTree.children[1]) + POP(ecx) +
			       ADD(eax, ecx);
		else
			return nodeMachineCode(myTree.children[1]) + PUSH(eax) +
			       nodeMachineCode(myTree.children[0]) + POP(ecx) +
			       SUB(eax, ecx);
	else if (myTree.myType == unary)
		if (myTree.myToken.text == "-")
			return nodeMachineCode(myTree.children[0]) + NEG(eax);
		else
			return nodeMachineCode(myTree.children[0]);
	else if (myTree.myType == multiDiv)
		if (myTree.myToken.text == "*")
			return nodeMachineCode(myTree.children[0]) + PUSH(eax) +
			       nodeMachineCode(myTree.children[1]) + POP(ecx) +
			       MUL(eax, ecx);
		else if (myTree.myToken.text == "/")
			return nodeMachineCode(myTree.children[1]) + PUSH(eax) +
			       nodeMachineCode(myTree.children[0]) + POP(ecx) +
			       DIV(eax, ecx);
		else  // if (myTree.myToken.text == "mod")
			return nodeMachineCode(myTree.children[1]) + PUSH(eax) +
			       nodeMachineCode(myTree.children[0]) + POP(ecx) +
			       MOD(eax, ecx);
	else if (myTree.myType == powers)
		return nodeMachineCode(myTree.children[0]);
	else if (myTree.myType == builtinFunc)
		if (myTree.myToken.text == "print") {
			program ret;
			for (auto kid : myTree.children)
				if (kid.myType == pstring)
					ret += nodeMachineCode(kid) +
					       PRINT_STRING();
				else
					ret +=
					    nodeMachineCode(kid) + PRINT_INT();
			return ret;
		} else
			throw "Unknown builtin "s + myTree.myToken.text;
	else if (myTree.myType == block) {
		// enter new scope
		// run all the code!
		program ret;
		for (auto kid : myTree.children)
			ret += nodeMachineCode(kid);
		return ret;
	} else
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
