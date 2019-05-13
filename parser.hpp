#ifndef __PARSER_HPP__
#define __PARSER_HPP__

#include <iostream>
#include <string>
#include <vector>
//#include <fstream>

#include "lexicalAnalyzer.hpp"
#include "token.hpp"

enum nodeType {
	empty,            // no token yet
	arthExp,          // Arthemetic expression
	ratExp,           // Rational expression
	logExp,           // Logical expression
	unary,            // negative or postive sign
	addSub,           // Plus or subtraction
	multiDiv,         // Multiple, Divide, Mode
	powers,           // exsponets, example: 5^2 = 25
	constantInt,      // Consant Integar: example: 5
	variable,         // ???
	builtinFunc,      // hmmmmm?
	block,            // PROFIT!
	pstring,          // some of my phavorite things!
	declare,          // my independence!
	assignment,       // number 3 is a bit late
	boolean,          // logic is defined on and and xor, not or
	controlStructure  // THE AGENTS OF THE STATE KNOW NO BOUNDS!
};

/*
 * The Node sturture for building a tree
 * For this class it is meant for building a code tree
 * Which will be used for ordered the generation of mechine code on: Intel x84
 */
struct Node {
	nodeType myType;
	Token myToken;
	std::vector<Node> children;

	Node() : myType(empty), myToken(), children({}) {}
	Node(nodeType aType, Token aToken, std::vector<Node> kids = {})
	    : myType(aType), myToken(aToken), children(kids) {}

	operator bool() { return myType != empty; }
};

/*
 * Giving a second name to Node.
 * New name = Tree
 */
typedef Node Tree;

std::ostream& operator<<(std::ostream&, Tree);

/*
 * This class is for tieing together all methods for parsing data out of
 * a tokenized file.
 */
class Parser {
       private:
	LexicalAnalyzer myLexicalAnalyzer;
	std::vector<std::string> keywords;
	// std::vector<> symbolTable;
	Tree parseIntegerConstant();
	Tree parseParenExper();
	Tree parseNumericValue();
	Tree parseUnaryOperator();
	Tree parsePow();
	Tree parseMultAndDivPrime(Tree);
	Tree parseMultAndDiv();
	Tree parseAddAndSubtractPrime(Tree);
	Tree parseAddAndSubtract();
	Tree parseArithmeticExpression();
	Tree parseBooleanValue();
	Tree parseBooleanExpression();
	Tree parseBooleanAndPrime(Tree);
	Tree parseBooleanAnd();
	Tree parseBooleanOrPrime(Tree);
	Tree parseBooleanOr();
	Tree parseBoolean();
	Tree parseMysteryParenths();
	Tree parseBooleanOrArithmeticExpression();
	Tree parseFunctionPrint();
	Tree parseFunctionRead();
	Tree parseDeclaration();
	Tree parseAssignment();
	Tree parseControlStructureIf();
	Tree parseStatement();
	Tree parseGlobalBlock();
	Tree parseBlockOrStatement();

       public:
	/*
	 * Default Construtor
	 */
	Parser();

	/*
	 * startup
	 */
	void init(std::string filename);

	/*
	 * Get the next valid exp in order of: ...
	 */
	Tree getNext() {
		if (myLexicalAnalyzer.peekToken().type == "EOF") return Tree();
		try {
			return parseGlobalBlock();
		} catch (std::string error) {
			std::cout << "Parse ERROR: " << error << std::endl;
			return Tree();
		}
	}
};

#endif
