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
Node* Parser::getNextArthExp(Node* myNode){

	myNode.leftChild = Node* aChild();
	checkAddSub(myNode.leftChild);

		
/*
	//Node myNode(arthExp);
	//myNode.myToken = myLexicalAnalyzer.getToken();

	//set default?
	//myNode.myType = arthExp;


	//Check addition
	if(myNode.myToken.type == "ADD" || myNode.myToken.type == "MINUS")
		//myNode.leftChild = Node* leftNode();
		;
	//Check *,/,mod
	else if()
		;
	//Check powers
	else if()
		;
	//Check negation
	else if()
		;
	//Check integar
	else if()
		;	
	//Check (, )
	else if()
		;
	//Check eof
	else if()
		;
	//throw error
	else 
		;

*/








	myNode.rightChild =

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

	if(myNode.myToken == EEOF)
		return myNode;
}
