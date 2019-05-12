#ifndef __LEXICALANALYZER_HPP__
#define __LEXICALANALYZER_HPP__

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "token.hpp"

class LexicalAnalyzer {
       private:
	typedef tokenType state;
	std::ifstream inFile;
	state currentState;
	std::vector<char> storageStack;
	std::vector<std::string> definedTokens;
	std::vector<Token> peekStack;

       public:
	/*
	 * Default Constructor
	 */
	LexicalAnalyzer();

	/*
	 * Startup the token reader state
	 */
	void init(std::string filename);

	/*
	 * Get the next token
	 */
	Token getToken();

	/*
	 * Look at the next token
	 */
	Token peekToken();

	/*
	 * Check the valid Token list for if there could be another longer valid
	 * token
	 */
	bool checkIfStarts(std::string currentToken, std::string definedToken);

	/*
	 * Check if the current token could be a defined token
	 */
	bool checkIfDefinedToken(std::string token);
};

#endif
