#include "token.hpp"

using std::string;

Token::Token(){
	type = "NULL";
	text = "NULL";
	tokenID = 0;
}

Token::Token(string txt, string typ, int id){
	type = typ;
	text = txt;
	tokenID = id;
}

