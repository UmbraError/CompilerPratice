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

Tree Parser::parseUnaryOperator() {
	// - 4
	// + 4
	// 4
	auto aToken = myLexicalAnalyzer.peekToken();
	if ((aToken.type == "MINUS") || (aToken.type == "PLUS")) {
		myLexicalAnalyzer.getToken();
		return Node(unary, aToken, {parseNumericValue()});
	} else
		return parseNumericValue();
}

Tree Parser::parsePow() {
	// 4 ^ 5
	// 4 ^ 5 ^ 6
	auto leftOperand = parseUnaryOperator();
	auto aToken = myLexicalAnalyzer.peekToken();
	if (aToken.type != "EXP") return leftOperand;
	myLexicalAnalyzer.getToken();
	auto rightOperand = parsePow();
	return Node(powers, aToken, {leftOperand, rightOperand});
}

Tree Parser::parseMultAndDivPrime(Tree accumulator) {
	auto aToken = myLexicalAnalyzer.peekToken();
	if ((aToken.type != "MULT") && (aToken.type != "DIV") &&
	    (aToken.type != "IDENT" || aToken.text != "mod"))
		return accumulator;
	myLexicalAnalyzer.getToken();
	auto rightOperand = parsePow();
	return parseMultAndDivPrime(
	    Node(multiDiv, aToken, {accumulator, rightOperand}));
}

Tree Parser::parseMultAndDiv() {
	auto leftOperand = parsePow();
	return parseMultAndDivPrime(leftOperand);
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
	if ((aToken.type != "PLUS") && (aToken.type != "MINUS"))
		return accumulator;
	myLexicalAnalyzer.getToken();
	auto rightOperand = parseMultAndDiv();
	return parseAddAndSubtractPrime(
	    Node(addSub, aToken, {accumulator, rightOperand}));
}

Tree Parser::parseAddAndSubtract() {
	// 3  prime
	auto leftOperand = parseMultAndDiv();
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
