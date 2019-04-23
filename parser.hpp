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
	Node* leftChild;
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
	 * Default Construtor
	 */
	Parser();
	
	/*
	 * startup
	 */
	void init(string filename);

	/*
	 * Get the next valid exp in order of: ...
	 */
	Node* getNextExp();

	/*
	 * Get next arthimetic expreesion
	 */
	Node* getNextArthExp();

	/*
	 *
	 */
	Node* getNextRatExp();

	/*
	 *
	 */
	Node* getNextLogExp();

	/*
	 * dealing with addition, subraction
	 */

};

#endif
