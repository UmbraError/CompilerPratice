#include "parser.hpp"

using std::string;
using std::vector;
using namespace std::string_literals;

Parser::Parser() {}

void Parser::init(std::string filename) {
	myLexicalAnalyzer.init(filename);

	//	keywords.push_back("mod");
	//	keywords.push_back("int4");
	//	keywords.push_back("string");
}

// Would you like to return parse errors as
// A) error codes (The C way)
// B) Thrown exceptions (The OO way)

Tree Parser::parseIntegerConstant() {
	auto aToken = myLexicalAnalyzer.peekToken();
	if (aToken.type != "INTEGER")
		throw "Expected integer got: "s + aToken.type;
	myLexicalAnalyzer.getToken();
	return Node(constantInt, aToken, {});
}

Tree Parser::parseParenExper() {
	auto aToken = myLexicalAnalyzer.peekToken();
	if (aToken.type != "LPAREN")
		throw "Expected LPAREN got: "s + aToken.type;
	myLexicalAnalyzer.getToken();
	auto insideParen = parseExpression();
	aToken = myLexicalAnalyzer.peekToken();
	if (aToken.type != "RPAREN")
		throw "Expected RPAREN got: "s + aToken.type;
	myLexicalAnalyzer.getToken();
	return insideParen;
}

Tree Parser::parseNumericValue() {
	// check if int or ( and then call
	auto aToken = myLexicalAnalyzer.peekToken();
	if (aToken.type == "LPAREN")
		return parseParenExper();
	else if (aToken.type == "INTEGER")
		return parseIntegerConstant();
	else
		throw "Expected an int or a '(' got: "s + aToken.type;
}

Tree Parser::parseAddAndSubtractPrime(Tree accumulator) {
	// + number
	// - number
	//
	// 4 - 3 - 2 - 1
	//
	// -(- (4 3) 2)

	// if + or - get value and make tree with accumlat
	auto aToken = myLexicalAnalyzer.peekToken();
	if (aToken.type != "PLUS" && aToken.type != "MINUS") return accumulator;
	myLexicalAnalyzer.getToken();
	auto rightOperand = parseNumericValue();
	return parseAddAndSubtractPrime(
	    Node(addSub, aToken, {accumulator, rightOperand}));
}

Tree Parser::parseAddAndSubtract() {
	// 3  prime
	auto leftOperand = parseNumericValue();
	return parseAddAndSubtractPrime(leftOperand);
}

Tree Parser::parseExpression() { return parseAddAndSubtract(); }

std::ostream& operator<<(std::ostream& o, Node node) {
	static int depth = 0;
	for (int i = 0; i < depth; i++) {
		o << "  ";
	}
	o << node.myToken.text << std::endl;  // print self
	depth++;
	for (auto& kid : node.children)
		o << kid;
	// print kids
	depth--;
	return o;
}
