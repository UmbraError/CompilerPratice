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

//	do{
	for(int i = 0; i < 10; i++){
		token = myLexicalAnalyzer.getToken();
		cout << token.text << " " << token.type << endl;
	}
//	}while(token.type != "EOF");

return 1;
}
