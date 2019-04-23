#ifndef __PARSER_HPP__
#define __PARSER_HPP__

#include <string>

//#include <vector>
//#include <iostream>
//#include <fstream>

#include "token.hpp"
#include "lexicalAnalyzer.hpp"

enum nodeType {empty, arthExp, ratExp, logExp, con, integar, integar4, };

struct Node{
	nodeType myType;
	Token myToken;
	Node* leftChild; // a vector of kids would be better here.
	Node* rightChild;

	//default
	Node(nodeType aType = empty, Token aToken = Token(), Node* left = nullptr, Node* right = nullptr){
		if(aType == empty)
			myType = aType;
		if(aToken.text == "NULL")
			myToken = aToken;
		if(left == nullptr)
			leftChild = left;
		if(right == nullptr)
			rightChild = right;
	};
};
typedef Node code_tree;

/*
 * 
 */
class Parser{ 
	private:

	LexicalAnalyzer myLexicalAnalyzer;
	std::vector<std::string>> keywords;
	//std::vector<> symbolTable;

	public:
	
	/*
	 * startup
	 */
	void init(string filename);

	/*
	 * Get the file's code tree.
	 */
	code_tree getCodeTree();
};

#endif
