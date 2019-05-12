#ifndef __TOKEN_HPP__
#define __TOKEN_HPP__
#include <string>
enum tokenType {
	NNULL,         // Looking for state
	EEOF,          // Hint EOF charcter
	UNKNOWN,       // came across an unknown character
	INVALID,       // A set of known characters that don't meet the
	               // syntax requirments
	DEFINEDTOKEN,  // !, &, |, <, =, >, and etc.
	IDENT,         // name_Thingy5657
	INTEGER,       // 1,2,3,4,5,6,7,8,9,0
	REAL,          // 3.14, 654.9898, and etc.
	STRING,        // "String"
	LINECOMMENT,   // //Comment
	BLOCKCOMMENT   // /* Comment */
};
struct Token {
	std::string type;
	std::string text;
	tokenType tokenID;

	Token();
	Token(std::string txt, std::string typ, tokenType id);
};
#endif
