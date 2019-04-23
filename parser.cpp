#include "parser.hpp"

Parser::init( std::string filename ){
	myLexicalAnalyzer.init( filename );

	keywords.push_back("mod");
	keywords.push_back("int4");
	keywords.push_back("string");
}

class Error{
  std::string message;
  Error()=default;
  Error(std::string m):message(m){}
  bool(){return message=="";}
};

Error parse_string(code_tree&,Lexicalanalyzer&){
	if(STRING)
		//break?
		;
}

Error parse_add_sub(code_tree&,Lexicalanalyzer&){
	if("+" || "-")
		//add, subtract
		;
}

Error parse_mull_div(code_tree&,Lexicalanalyzer&){
	// why check for ident?
	if(IDENT || "/" || "*")
		//check if keyword
		//if not check if declared
		//
		;
}

Error parse_unary_minus(code_tree&,Lexicalanalyzer&){
	if()
		//check for negation
		;
}

Error parse_number(code_tree&,Lexicalanalyzer&){
	// we only have ints in these assignments
	if(REAL || INTEGER)
		;
}

Error parse_parenthisis(code_tree&,Lexicalanalyzer&){
	if("(" || ")") 
		;
}


code_tree Parser::getCodeTree(){
  // TODO: Check that init has been called.
}
