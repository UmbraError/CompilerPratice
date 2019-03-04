#include <iostream>
#include <string>
#include "lexicalAnalyzer.hpp"
#include "token.hpp"

using std::string;
using std::cout;
using std::endl;

int main(int argc, char **argv){
	if(argc != 2){
		cout << "Usage " << argv[0] << " [file]" << endl;
		return 0;
	}

	LexicalAnalyzer myLexicalAnalyzer;

	myLexicalAnalyzer.init( (string)argv[1] );

	Token token;

	do{
		token = myLexicalAnalyzer.getToken();
		cout << token.type << ": " << token.text << endl;
	}while(token.type != "EOF");

	return 1;
}
