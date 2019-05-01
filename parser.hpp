#ifndef __PARSER_HPP__
#define __PARSER_HPP__

#include <string>
#include <vector>
//#include <iostream>
//#include <fstream>

#include "lexicalAnalyzer.hpp"
#include "token.hpp"

enum nodeType {
	empty,
	arthExp,
	addSub,
	multiDiv,
	ratExp,
	logExp,
	con,
	integar4
};

struct Node {
	nodeType myType;
	Token myToken;
	std::vector<Node> children;

	Node(nodeType aType, Token aToken,std::vector<Node> kids={}):
	  myType{aType),
	  myToken(aToken),
	  children(kids){
	}
};

	typedef Node Tree;

	/*
	 *
	 */
	class Parser {
	private:
		LexicalAnalyzer myLexicalAnalyzer;
		std::vector<std::string>> keywords;
		// std::vector<> symbolTable;

	public:
		/*
		 * Default Construtor
		 */
		Parser();

		/*
		 * startup
		 */
		void init(std::string filename);

		/*
		 * Get the next valid exp in order of: ...
		 */
		Tree getNext();
	};

#endif
