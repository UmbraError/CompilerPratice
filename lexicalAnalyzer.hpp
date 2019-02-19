#ifndef __LEXICALANALYZER_HPP__
#define __LEXICALANALYZER_HPP__

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "token.hpp"

class LexicalAnalyzer{ 
	private:
		enum state { NNULL,         // Looking for state 
		     EEOF,          // Hint EOF charcter
		     UNKNOWN,       // came across an unknown character
		     INVALID,       // A set of known characters that don't meet the syntax requirments
		     DEFINEDTOKEN,  // !, &, |, <, =, >, and etc. 
		     IDENT,         // name_Thingy5657
	             INTEGER,       // 1,2,3,4,5,6,7,8,9,0
		     REAL,          // 3.14, 654.9898, and etc.
		     STRING,        // "String"
		     LINECOMMENT,   // //Comment
	             BLOCKCOMMENT   // /* Comment */
	         	   };        

		std::ifstream inFile;
		state currentState;
		std::vector<char> storageStack;		
		std::vector<std::string> definedTokens;
	
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
		 * Check the valid Token list for if there could be another longer valid token
		 */
		bool checkIfContains(std::string currentToken, std::string definedToken);
		
		/*
		 * Check if the current token could be a defined token
		 */
		bool checkIfDefinedToken(std::string token);
		
};

#endif
