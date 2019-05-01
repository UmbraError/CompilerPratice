#ifndef __TOKEN_HPP__
#define __TOKEN_HPP__
#include <string>

struct Token {
	std::string type;
	std::string text;
	int tokenID;

	Token();
	Token(std::string txt, std::string typ, int id);
};
#endif
