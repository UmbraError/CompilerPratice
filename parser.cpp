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
