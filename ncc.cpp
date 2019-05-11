#include <iostream>
#include <string>

#include "MachineCodeGenerator.hpp"
#include "lexicalAnalyzer.hpp"
#include "parser.hpp"
#include "token.hpp"

using std::cout;
using std::endl;
using std::string;

int main(int argc, char** argv) {
	if (argc != 2) {
		cout << "Usage " << argv[0] << " [file]" << endl;
		return 0;
	}

	// LexicalAnalyzer myLexicalAnalyzer;
	Parser myParser;
	myParser.init((string)argv[1]);

	Tree tree;
	while ((tree = myParser.getNext())) {
		cout << "Code tree:" << endl;
		cout << tree;
		program prog = machineCodeGenerator(tree);

		cout << "Code size: " << prog.size() << " bytes." << endl;
		cout << "Code execution:" << endl << prog() << endl << endl;
		// run the mechine code
	}

	//	Token token;

	//	do {
	// get next arthmerhic expression
	//		token = myLexicalAnalyzer.getToken();
	//		cout << token.type << ": " << token.text << endl;
	//	} while (token.type != "EOF");

	return 1;
}
