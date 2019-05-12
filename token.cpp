#include "token.hpp"

using std::string;

Token::Token() {
	type = "NULL";
	text = "";
	tokenID = NNULL;
}

Token::Token(string txt, string typ, tokenType id) {
	type = typ;
	text = txt;
	tokenID = id;
}
