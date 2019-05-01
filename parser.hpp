#ifndef __PARSER_HPP__
#define __PARSER_HPP__

#include <string>

//#include <vector>
//#include <iostream>
//#include <fstream>

#include "token.hpp"
#include "lexicalAnalyzer.hpp"

enum nodeType {empty,
	       arthExp,
	       addSub,
	       multiDiv,	
	       ratExp, 
	       logExp, 
	       con, 
	       integar4, };

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
	 * All of these will be local to the cpp file
	/*
	 * Get the next valid exp in order of: ...
	 * /
	Node* getNextExp();

	/*
	 * Get next arthimetic expreesion
	 * /
	Node* getNextArthExp();

	//addition_and_subtraction
	Node* checkAddSub();
	//multi_div_mod
	Node* checkMultiDiv();
	//powers
	Node* checkPower();
	//Negation
	Node* checkSign();
	//int
	Node* checkNum();
	//group, (,)
	Node* checkForGrouping();


	/*
	 *
	 * /
	Node* getNextRatExp();

	/*
	 *
	 * /
	Node* getNextLogExp();
	*/
};

#endif
