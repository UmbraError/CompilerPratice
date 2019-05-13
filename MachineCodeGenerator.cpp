#include "MachineCodeGenerator.hpp"
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
using namespace std::string_literals;

namespace Fun {
void print_int(int x) { std::cout << x; }
void print_str(char* s) { std::cout << s; }
void print_bool(bool b) { std::cout << (b ? "true" : "false"); }
int read_int() {
	int x;
	std::cin >> x;
	return x;
}
}  // namespace Fun

enum Reg { eax, ebx, ecx, edx, rsi, rdi, rax };

program littleEndian(long long int num, int bytes) {
	if (bytes == 0) return program();
	return program(num & 0xFF) + littleEndian(num >> 8, bytes - 1);
}

program MOV(Reg reg, int num) {
	// std::cout << "In MOV(r,i) i="<<num<<std::endl;
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

template <typename T, typename... ARGS>
program MOV(Reg reg, T (*fp)(ARGS...)) {
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

program STORAGE(int location, Reg reg) {
	switch (reg) {
		case eax:
			return {{0x89, 0x45, 0xff & (-4 * (location + 1))}};
		default:
			throw "Unknown register in STORAGE(i,r)"s;
	}
	// mov eax,[ebp-4*d]
}

program GET_VAR(Reg reg, int location) {
	switch (reg) {
		case eax:
			return {{0x8b, 0x45, 0xff & (-4 * (location + 1))}};

		default:
			throw "Unknown register in GET_VAR(r,i)"s;
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

program CMP(Reg reg1, Reg reg2) {
	// flargelnarg
	switch (reg1) {
		case eax:
			switch (reg2) {
				case eax:
					return {{0x39, 0xc0}};
				case ecx:
					return {{0x39, 0xc8}};
				default:
					throw "Unexpceted register 2 in CMP(r,r)"s;
			}
		default:
			throw "Unexpceted register 1 in CMP(r,r)"s;
	}
}

program EXTENTIONS_EXCLAMATIONPOINT() {
	// JUST THREE EASY PAYMENTS OF 0xffff
	return {{0x66, 0x98, 0x98}};
}

program BOOL_LESS() {
	return program({0x0f, 0x9c, 0xc0})  // setl al
	       + EXTENTIONS_EXCLAMATIONPOINT();
}
program BOOL_LESS_EQ() {
	return program({0x0f, 0x9e, 0xc0})  // setle al
	       + EXTENTIONS_EXCLAMATIONPOINT();
}
program BOOL_GREATER_EQ() {
	return program({0x0f, 0x9d, 0xc0})  // setge al
	       + EXTENTIONS_EXCLAMATIONPOINT();
}
program BOOL_GREATER() {
	return program({0x0f, 0x9f, 0xc0})  // setg al
	       + EXTENTIONS_EXCLAMATIONPOINT();
}
program BOOL_EQ() {
	return program({0x0f, 0x94, 0xc0})  // sete al
	       + EXTENTIONS_EXCLAMATIONPOINT();
}
program BOOL_NOT_EQ() {
	return program({0x0f, 0x95, 0xc0})  // setne al
	       + EXTENTIONS_EXCLAMATIONPOINT();
}

program AND(Reg reg1, Reg reg2) {
	switch (reg1) {
		case eax:
			switch (reg2) {
				case ecx:
					return {{0x21, 0xc8}};
				default:
					throw "Unexpected Register 2 for AND(r,r)"s;
			}
		default:
			throw "Unexpected register 1 for AND(r,r)"s;
	}
}
program OR(Reg reg1, Reg reg2) {
	switch (reg1) {
		case eax:
			switch (reg2) {
				case ecx:
					return {{0x09, 0xc8}};
				default:
					throw"Unexpected register 2 for OR(r,r)"s;
			}
		default:
			throw "Unexpected register 1 for OR(r,r)"s;
	}
}
program NOT(Reg reg) {
	switch (reg) {
		case eax:
			return {{0xf7, 0xd0}};
		default:
			throw "Unknown register in NOT(r)"s;
	}
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

program JE(int distance) {
	return program({0x0f, 0x84}) + littleEndian(distance, 4);
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

program READ_INT() { return MOV(rsi, Fun::read_int) + CALL(rsi); }
program PRINT_INT() {
	return MOV(rdi, rax) + MOV(rsi, Fun::print_int) + CALL(rsi);
}
program PRINT_BOOL() {
	return MOV(rdi, rax) + MOV(rsi, Fun::print_bool) + CALL(rsi);
}
program PRINT_STRING() { return MOV(rsi, Fun::print_str) + CALL(rsi); }

program RET() { return program(0xC3); }

struct Symbol {
	// std::string type; // always int4
	std::string name;
	int location;  // offset from the basepointer
};

struct Enviroment {
	std::vector<Symbol> symbolTable;
	program cleanup() {
		program ret;
		for (auto symbol : symbolTable)
			ret += POP(ecx);
		return ret;
	}
	int find(std::string varName) {
		for (auto symbol : symbolTable)
			if (symbol.name == varName) return symbol.location;
		throw "Unknown variable name was read: "s + varName;
	}
};

program nodeMachineCode(Tree myTree, Enviroment& island) {
	// storage of strings so that the pointer does not disappear.
	static std::vector<std::unique_ptr<std::string>> stringTable;

	if (myTree.myType == constantInt)
		return MOV(eax, atol(myTree.myToken.text.c_str()));
	else if (myTree.myType == pstring) {
		stringTable.push_back(
		    std::make_unique<std::string>(myTree.myToken.text));
		return MOV(rdi, stringTable.back()->c_str());
	} else if (myTree.myType == boolean)
		if (myTree.myToken.text == "true")
			return MOV(eax, 1);
		else if (myTree.myToken.text == "false")
			return MOV(eax, 0);
		else {
			auto /*m*/ lp =
			    nodeMachineCode(myTree.children[0], island);
			auto rp = nodeMachineCode(myTree.children[1], island);
			if (myTree.myToken.type == "LESS")
				return rp + PUSH(eax) + lp + POP(ecx) +
				       CMP(eax, ecx) + BOOL_LESS();
			else if (myTree.myToken.type == "LESS_EQ")
				return rp + PUSH(eax) + lp + POP(ecx) +
				       CMP(eax, ecx) + BOOL_LESS_EQ();
			else if (myTree.myToken.type == "GREATER_EQ")
				return rp + PUSH(eax) + lp + POP(ecx) +
				       CMP(eax, ecx) + BOOL_GREATER_EQ();
			else if (myTree.myToken.type == "GREATER")
				return rp + PUSH(eax) + lp + POP(ecx) +
				       CMP(eax, ecx) + BOOL_GREATER();
			else if (myTree.myToken.type == "EQUAL")
				return rp + PUSH(eax) + lp + POP(ecx) +
				       CMP(eax, ecx) + BOOL_EQ();
			else  // if myTree.myToken.type == "NOT_EQUAL")
				return rp + PUSH(eax) + lp + POP(ecx) +
				       CMP(eax, ecx) + BOOL_NOT_EQ();
		}
	else if (myTree.myType == logExp)
		if (myTree.myToken.type == "NOT")
			return nodeMachineCode(myTree.children[0], island) +
			       NOT(eax);
		else if (myTree.myToken.type == "AND")
			return nodeMachineCode(myTree.children[0], island) +
			       PUSH(eax) +
			       nodeMachineCode(myTree.children[1], island) +
			       POP(ecx) + AND(eax, ecx);
		else  // if (myTree.myTolen.type == "OR" )
			return nodeMachineCode(myTree.children[0], island) +
			       PUSH(eax) +
			       nodeMachineCode(myTree.children[1], island) +
			       POP(ecx) + OR(eax, ecx);
	else if (myTree.myType == variable)
		return GET_VAR(eax, island.find(myTree.myToken.text));
	else if (myTree.myType == declare) {
		island.symbolTable.push_back(
		    {myTree.myToken.text, (int)island.symbolTable.size()});
		return MOV(eax, 0) + PUSH(eax);  // make space on stack
		// TODO: we assume there are to temps on the stack
	} else if (myTree.myType == addSub)
		if (myTree.myToken.text == "+")
			return nodeMachineCode(myTree.children[0], island) +
			       PUSH(eax) +
			       nodeMachineCode(myTree.children[1], island) +
			       POP(ecx) + ADD(eax, ecx);
		else
			return nodeMachineCode(myTree.children[1], island) +
			       PUSH(eax) +
			       nodeMachineCode(myTree.children[0], island) +
			       POP(ecx) + SUB(eax, ecx);
	else if (myTree.myType == unary)
		if (myTree.myToken.text == "-")
			return nodeMachineCode(myTree.children[0], island) +
			       NEG(eax);
		else
			return nodeMachineCode(myTree.children[0], island);
	else if (myTree.myType == multiDiv)
		if (myTree.myToken.text == "*")
			return nodeMachineCode(myTree.children[0], island) +
			       PUSH(eax) +
			       nodeMachineCode(myTree.children[1], island) +
			       POP(ecx) + MUL(eax, ecx);
		else if (myTree.myToken.text == "/")
			return nodeMachineCode(myTree.children[1], island) +
			       PUSH(eax) +
			       nodeMachineCode(myTree.children[0], island) +
			       POP(ecx) + DIV(eax, ecx);
		else  // if (myTree.myToken.text == "mod")
			return nodeMachineCode(myTree.children[1], island) +
			       PUSH(eax) +
			       nodeMachineCode(myTree.children[0], island) +
			       POP(ecx) + MOD(eax, ecx);
	else if (myTree.myType == powers)
		return nodeMachineCode(myTree.children[0], island);
	else if (myTree.myType == builtinFunc)
		if (myTree.myToken.text == "print") {
			program ret;
			for (auto kid : myTree.children)
				if (kid.myType == pstring)
					ret += nodeMachineCode(kid, island) +
					       PRINT_STRING();
				else if (kid.myType == boolean ||
				         kid.myType == logExp)
					ret += nodeMachineCode(kid, island) +
					       PRINT_BOOL();
				else
					ret += nodeMachineCode(kid, island) +
					       PRINT_INT();
			return ret;
		} else if (myTree.myToken.text == "read") {
			auto varlocation =
			    island.find(myTree.children[0].myToken.text);
			return READ_INT() + STORAGE(varlocation, eax);
		} else
			throw "Unknown builtin "s + myTree.myToken.text;
	else if (myTree.myType == assignment) {
		auto varlocation = island.find(myTree.children[0].myToken.text);
		return nodeMachineCode(myTree.children[1], island) +
		       STORAGE(varlocation, eax);
	} else if (myTree.myType == block) {
		// enter new scope
		// run all the code!
		program ret;
		Enviroment innerEnviroment;
		for (auto kid : myTree.children)
			ret += nodeMachineCode(kid, innerEnviroment);
		ret += innerEnviroment.cleanup();
		return ret;
	} else if (myTree.myType == controlStructure)
	// if(myTree.myToken.text=="if")
	{
		auto code = nodeMachineCode(myTree.children[1], island);
		return nodeMachineCode(myTree.children[0], island) +
		       CMP(eax, eax) + JE(code.size()) + code;
	} else
		throw "Unexpected Node Type: "s + std::to_string(myTree.myType);
}

program machineCodeGenerator(Tree myTree) {
	try {
		Enviroment globalEnviroment;  // better be carefuly not to cause
		                              // global warming!
		return nodeMachineCode(myTree, globalEnviroment) +
		       globalEnviroment.cleanup() + RET();
	} catch (std::string error) {
		std::cout << "Code Generation ERROR: " << error << std::endl;
		return RET();
	}
}
