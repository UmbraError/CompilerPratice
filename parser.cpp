#include "parser.hpp"

Parser::Parser(){
}

Parser::init( std::string filename ){
	myLexicalAnalyzer.init( filename );

	keywords.push_back("mod");
	keywords.push_back("int4");
	keywords.push_back("string");
}

/*
 * Note: need to run init before this
 * Descrpition: Get next arthmatic Expression
 */
Node* Parser::getNextArthExp(){
	*Node myNode(arthExp);
	Token myToken = myLexicalAnalyzer.getToken();

	if( )
	;
	if(EEOF)
		//terminate;
	;
	if(STRING)
		//break?
		;
	if("+" || "-")
		//add, subtract
		;
	if(IDENT || "/" || "*")
		//check if keyword
		//if not check if declared
		//
		;
	if()
		//check for negation
		;
	if(REAL || INTEGER)
		;
	if("(" || ")") 
		;
	if()
		return getNextArthExp();
	if()
		return myNode;
}
