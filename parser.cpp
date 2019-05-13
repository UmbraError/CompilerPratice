#include "parser.hpp"

using std::string;
using std::vector;
using namespace std::string_literals;

Parser::Parser() {}

void Parser::init(std::string filename) { myLexicalAnalyzer.init(filename); }

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
	auto insideParen = parseArithmeticExpression();
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
	else if (aToken.type == "IDENT") {
		myLexicalAnalyzer.getToken();
		return Node(variable, aToken, {});
	} else
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

Tree Parser::parseArithmeticExpression() { return parseAddAndSubtract(); }

Tree Parser::parseBooleanValue() {
	auto aToken = myLexicalAnalyzer.peekToken();
	if (aToken.type == "IDENT" &&
	    (aToken.text == "true" || aToken.text == "false")) {
		myLexicalAnalyzer.getToken();
		return Node(boolean, aToken, {});
	}
	throw "Expected true or false got: "s + aToken.type + " " + aToken.text;
}

Tree Parser::parseBooleanExpression() {
	auto aToken = myLexicalAnalyzer.peekToken();
	if (aToken.type == "LPAREN") {
		auto inside = parseMysteryParenths();
		if (inside.myType == logExp || inside.myType == boolean)
			return inside;
		inside = parseAddAndSubtractPrime(parseMultAndDivPrime(inside));
		auto expressionOperator = myLexicalAnalyzer.peekToken();
		if (expressionOperator.type != "LESS" &&
		    expressionOperator.type != "LESS_EQ" &&
		    expressionOperator.type != "GREATER_EQ" &&
		    expressionOperator.type != "GREATER" &&
		    expressionOperator.type != "EQUAL" &&
		    expressionOperator.type != "NOT_EQUAL")
			throw "Expected realational operator in parse bool got: "s+expressionOperator.type;
		myLexicalAnalyzer.getToken();
		auto expressionRight = parseArithmeticExpression();
		return Node(boolean, expressionOperator,
		            {inside, expressionRight});
	}
	if (aToken.type == "NOT") {
		myLexicalAnalyzer.getToken();
		return Node(logExp, aToken, {parseBooleanExpression()});
	}
	if (aToken.type == "IDENT" &&
	    (aToken.text == "true" || aToken.text == "false"))
		return parseBooleanValue();
	auto expressionLeft = parseArithmeticExpression();
	auto expressionOperator = myLexicalAnalyzer.peekToken();
	if (expressionOperator.type != "LESS" &&
	    expressionOperator.type != "LESS_EQ" &&
	    expressionOperator.type != "GREATER_EQ" &&
	    expressionOperator.type != "GREATER" &&
	    expressionOperator.type != "EQUAL" &&
	    expressionOperator.type != "NOT_EQUAL")
		throw "Expected boolean realational operator got: "s +
		    expressionOperator.type;
	myLexicalAnalyzer.getToken();
	auto expressionRight = parseArithmeticExpression();
	return Node(boolean, expressionOperator,
	            {expressionLeft, expressionRight});
}

Tree Parser::parseBooleanAndPrime(Tree accumulator) {
	auto andToken = myLexicalAnalyzer.peekToken();
	if (andToken.type != "AND") return accumulator;
	myLexicalAnalyzer.getToken();
	auto boolExpression = parseBooleanExpression();
	return parseBooleanAndPrime(
	    Node(logExp, andToken, {accumulator, boolExpression}));
}

Tree Parser::parseBooleanAnd() {
	return parseBooleanAndPrime(parseBooleanExpression());
}

Tree Parser::parseBooleanOrPrime(Tree accumulator) {
	auto oarToken = myLexicalAnalyzer.peekToken();
	// now we can row around our island
	if (oarToken.type != "OR") return accumulator;
	myLexicalAnalyzer.getToken();
	auto andExpression = parseBooleanAnd();
	return parseBooleanOrPrime(
	    Node(logExp, oarToken, {accumulator, andExpression}));
}

Tree Parser::parseBooleanOr() { return parseBooleanOrPrime(parseBooleanAnd()); }

Tree Parser::parseBoolean() { return parseBooleanOr(); }

Tree Parser::parseMysteryParenths() {
	// (a+3)<4
	// (a<4) & b = 2
	auto lparenToken = myLexicalAnalyzer.peekToken();
	if (lparenToken.type != "LPAREN")
		throw "Expected '(' got: "s + lparenToken.type;
	myLexicalAnalyzer.getToken();
	auto inside = parseBooleanOrArithmeticExpression();
	auto rparenToken = myLexicalAnalyzer.peekToken();
	if (rparenToken.type != "RPAREN")
		throw "Expected ')' got: "s + rparenToken.type;
	myLexicalAnalyzer.getToken();
	return inside;
}

Tree Parser::parseBooleanOrArithmeticExpression() {
	// true
	// a+3
	// a<b
	// a<3 OR b>4
	auto aToken = myLexicalAnalyzer.peekToken();
	if (aToken.type == "LPAREN") {
		auto inside = parseMysteryParenths();
		if (inside.myType == logExp || inside.myType == boolean)
			return parseBooleanOrPrime(
			    parseBooleanAndPrime(inside));
		// parse thing in ()s
		// still deal with & and | after it
		auto expressionOperator = myLexicalAnalyzer.peekToken();
		if (expressionOperator.type != "LESS" &&
		    expressionOperator.type != "LESS_EQ" &&
		    expressionOperator.type != "GREATER_EQ" &&
		    expressionOperator.type != "GREATER" &&
		    expressionOperator.type != "EQUAL" &&
		    expressionOperator.type != "NOT_EQUAL")
			return parseAddAndSubtractPrime(
			    parseMultAndDivPrime(inside));
		myLexicalAnalyzer.getToken();
		auto expressionRight = parseArithmeticExpression();
		auto logicalOperator = myLexicalAnalyzer.peekToken();
		auto relationNode = Node(boolean, expressionOperator,
		                         {inside, expressionRight});
		if (logicalOperator.type == "AND" ||
		    logicalOperator.type == "OR")
			return parseBooleanOrPrime(
			    parseBooleanAndPrime(relationNode));
		return relationNode;
	}
	if (aToken.type == "NOT") return parseBoolean();
	if (aToken.type == "IDENT" &&
	    (aToken.text == "true" || aToken.text == "false"))
		return parseBooleanValue();
	auto expressionLeft = parseArithmeticExpression();
	auto expressionOperator = myLexicalAnalyzer.peekToken();
	if (expressionOperator.type != "LESS" &&
	    expressionOperator.type != "LESS_EQ" &&
	    expressionOperator.type != "GREATER_EQ" &&
	    expressionOperator.type != "GREATER" &&
	    expressionOperator.type != "EQUAL" &&
	    expressionOperator.type != "NOT_EQUAL")
		return expressionLeft;
	myLexicalAnalyzer.getToken();
	auto expressionRight = parseArithmeticExpression();
	auto logicalOperator = myLexicalAnalyzer.peekToken();
	auto relationNode = Node(boolean, expressionOperator,
	                         {expressionLeft, expressionRight});
	if (logicalOperator.type == "AND" || logicalOperator.type == "OR")
		return parseBooleanOrPrime(parseBooleanAndPrime(relationNode));
	return relationNode;
}

Tree Parser::parseFunctionPrint() {
	// "print" ( parseArithmeticExpresion ) ;
	auto printToken = myLexicalAnalyzer.peekToken();
	if (printToken.type != "IDENT" || printToken.text != "print")
		throw "Expected print statement got: "s + printToken.type +
		    " : " + printToken.text;
	myLexicalAnalyzer.getToken();
	auto lparenToken = myLexicalAnalyzer.peekToken();
	if (lparenToken.type != "LPAREN")
		throw "Expected '(' in print got: "s + lparenToken.type;
	vector<Tree> parameters;
	Token aToken;
	do {
		myLexicalAnalyzer.getToken();
		aToken = myLexicalAnalyzer.peekToken();
		if (aToken.type == "STRING") {
			parameters.push_back(Node(pstring, aToken, {}));
			myLexicalAnalyzer.getToken();
		} else
			parameters.push_back(
			    parseBooleanOrArithmeticExpression());

		aToken = myLexicalAnalyzer.peekToken();
	} while (aToken.type == "COMMA");
	if (aToken.type != "RPAREN")
		throw "Expected ')' in print got: "s + aToken.type;
	myLexicalAnalyzer.getToken();
	auto semiToken = myLexicalAnalyzer.peekToken();
	if (semiToken.type != "SEMICOLON")
		throw "Expected ';' in print got: "s + semiToken.type;
	myLexicalAnalyzer.getToken();
	return Node(builtinFunc, printToken, parameters);
}

Tree Parser::parseFunctionRead() {
	auto readToken = myLexicalAnalyzer.peekToken();
	// read
	if (readToken.type != "IDENT" || readToken.text != "read")
		throw "Expected read got: "s + readToken.type + " " +
		    readToken.text;
	myLexicalAnalyzer.getToken();
	//(
	auto lparenToken = myLexicalAnalyzer.peekToken();
	if (lparenToken.type != "LPAREN")
		throw "Expected '(' got: "s + lparenToken.type;
	myLexicalAnalyzer.getToken();

	// varname
	auto varToken = myLexicalAnalyzer.peekToken();
	if (varToken.type != "IDENT")
		throw "Expected variable name got: "s + varToken.type;
	myLexicalAnalyzer.getToken();
	//)
	auto rparenToken = myLexicalAnalyzer.peekToken();
	if (rparenToken.type != "RPAREN")
		throw "Expected ')' got: "s + rparenToken.type;
	myLexicalAnalyzer.getToken();

	//;
	auto semiToken = myLexicalAnalyzer.peekToken();
	if (semiToken.type != "SEMICOLON")
		throw "Expected ';' got: "s + semiToken.type;
	myLexicalAnalyzer.getToken();
	return Node(builtinFunc, readToken, {Node(variable, varToken, {})});
}

Tree Parser::parseDeclaration() {
	auto typeToken = myLexicalAnalyzer.peekToken();
	if (typeToken.type != "IDENT" || typeToken.text != "int4")
		throw "Expected int4 in declaration got: "s + typeToken.type +
		    " " + typeToken.text;
	myLexicalAnalyzer.getToken();
	// IDENT: aname
	auto varToken = myLexicalAnalyzer.peekToken();
	if (varToken.type != "IDENT")
		throw "Expected IDENT in declaration got: "s + varToken.type;
	myLexicalAnalyzer.getToken();
	// SEMICOLON
	auto semiToken = myLexicalAnalyzer.peekToken();
	if (semiToken.type != "SEMICOLON")
		throw "Expected ';' int declaration got: "s + typeToken.type;
	myLexicalAnalyzer.getToken();

	return Node(declare, varToken, {});
}

Tree Parser::parseAssignment() {
	// a <- arimet_value ;
	auto varToken = myLexicalAnalyzer.peekToken();
	if (varToken.type != "IDENT")
		throw "Exptected variable name got: "s + varToken.type;
	myLexicalAnalyzer.getToken();
	auto assignToken = myLexicalAnalyzer.peekToken();
	if (assignToken.type != "ASSIGN")
		throw "Expected '<-' got: "s + assignToken.type;
	myLexicalAnalyzer.getToken();
	auto valTree = parseArithmeticExpression();
	auto semiToken = myLexicalAnalyzer.peekToken();
	if (semiToken.type != "SEMICOLON")
		throw "Expected ';' got: "s + semiToken.type;
	myLexicalAnalyzer.getToken();
	return Node(assignment, assignToken,
	            {Node(variable, varToken, {}), valTree});
}

Tree Parser::parseControlStructureIf() {
	// if ( bool )
	auto ifToken = myLexicalAnalyzer.peekToken();
	if (ifToken.type != "IDENT" || ifToken.text != "if")
		throw "Expected 'if' got: "s + ifToken.type + " " +
		    ifToken.text;
	myLexicalAnalyzer.getToken();
	auto lparenToken = myLexicalAnalyzer.peekToken();
	if (lparenToken.type != "LPAREN")
		throw "Expected '(' got: "s + lparenToken.type;
	myLexicalAnalyzer.getToken();
	auto check = parseBoolean();
	auto rparenToken = myLexicalAnalyzer.peekToken();
	if (rparenToken.type != "RPAREN")
		throw "Expected ')' got: "s + rparenToken.type;
	myLexicalAnalyzer.getToken();
	auto areCode = parseBlockOrStatement();
	return Node(controlStructure, ifToken, {check, areCode});
}

Tree Parser::parseStatement() {
	auto aToken = myLexicalAnalyzer.peekToken();
	if (aToken.type == "IDENT" && aToken.text == "print")
		return parseFunctionPrint();
	if (aToken.type == "IDENT" && aToken.text == "int4")
		return parseDeclaration();
	if (aToken.type == "IDENT" && aToken.text == "read")
		return parseFunctionRead();
	if (aToken.type == "IDENT" && aToken.text == "if")
		return parseControlStructureIf();
	if (aToken.type == "IDENT")  // we don't know the var to check
		return parseAssignment();
	throw "Expected print, declaration, or read got: "s + aToken.type +
	    " " + aToken.text;
}

Tree Parser::parseGlobalBlock() {
	vector<Node> statements;
	while (myLexicalAnalyzer.peekToken().type != "EOF")
		statements.push_back(parseStatement());
	return Node(block, {"statement block", "", NNULL}, statements);
}

Tree Parser::parseBlockOrStatement() {
	auto squiggleBrace = myLexicalAnalyzer.peekToken();
	if (squiggleBrace.type != "LBRACE") return parseStatement();
	myLexicalAnalyzer.getToken();
	vector<Node> statements;
	while (myLexicalAnalyzer.peekToken().type != "RBRACE")
		statements.push_back(parseStatement());
	return Node(block, squiggleBrace, statements);
}

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
