#include "lexicalAnalyzer.hpp"

#include <iostream>
using std::cout;

using std::string;
using std::cerr;
using std::endl;

/*
 * Default Constructor
 */
LexicalAnalyzer::LexicalAnalyzer(){
	currentState = NNULL;

	//Even is the text, Odd/+one is the Type 
	definedTokens.push_back("+"); definedTokens.push_back("PLUS");
	definedTokens.push_back("-"); definedTokens.push_back("MINUS");
	definedTokens.push_back("*"); definedTokens.push_back("MULT");
	definedTokens.push_back("/"); definedTokens.push_back("DIV");
	definedTokens.push_back("^"); definedTokens.push_back("EXP");
	definedTokens.push_back("<"); definedTokens.push_back("LESS");
	definedTokens.push_back("<="); definedTokens.push_back("LESS_EQ");
	definedTokens.push_back(">"); definedTokens.push_back("GREATER");
	definedTokens.push_back(">="); definedTokens.push_back("GREATER_EQ");
	definedTokens.push_back("="); definedTokens.push_back("EQUAL");
	definedTokens.push_back("~="); definedTokens.push_back("NOT_EQUAL");
	definedTokens.push_back("<-"); definedTokens.push_back("ASSIGN");
	definedTokens.push_back("!"); definedTokens.push_back("NOT");
	definedTokens.push_back("("); definedTokens.push_back("LPAREN");
	definedTokens.push_back(")"); definedTokens.push_back("RPAREN");
	definedTokens.push_back("{"); definedTokens.push_back("LBRACE");
	definedTokens.push_back("}"); definedTokens.push_back("RBRACE");
	definedTokens.push_back("["); definedTokens.push_back("LBRACKET");
	definedTokens.push_back("]"); definedTokens.push_back("RBRACKET");
	definedTokens.push_back("&"); definedTokens.push_back("AND");
	definedTokens.push_back("|"); definedTokens.push_back("OR");
	definedTokens.push_back("."); definedTokens.push_back("DOT");
	definedTokens.push_back("@"); definedTokens.push_back("AT");
	definedTokens.push_back(":"); definedTokens.push_back("COLON");
	definedTokens.push_back(";"); definedTokens.push_back("SEMICOLON");
	definedTokens.push_back(","); definedTokens.push_back("COMMA");
}

/*
 * Startup the token reader state
 */
void LexicalAnalyzer::init(string filename){
	inFile.open(filename);
	if(!inFile)
		cerr << "Unable to open file: " << filename << endl;
}

/*
 * Get the next token
 */
Token LexicalAnalyzer::getToken(){
	char temp = '\0';
	string currentToken = "";
	currentState = NNULL;
	
	if(storageStack.empty())
		inFile >> temp;
	else{
		temp = storageStack.back();
		storageStack.pop_back();
	}

	currentToken += temp;

	//Remove white space
	while( currentToken == "\r" || currentToken == "\n" || currentToken == "\t" || currentToken == " " ){
		if(storageStack.empty())
			inFile >> temp;
		else{
			temp = storageStack.back();
			storageStack.pop_back();
		}
		currentToken = temp;
	}


	// Start of State primer 
	if(temp <= '9' && temp >= '0')
		currentState = INTEGER;
	else if(currentToken == ".")
		currentState = REAL;
	else if(currentToken == "\"")
		currentState = STRING;
	else if(currentToken == "#")
		currentState = LINECOMMENT;
	else if(currentToken == "<"){ //Buggy? related to having stuff on storage when checking syntax
		inFile >> temp;
		currentToken += temp;
		if ( temp == '<'){
			storageStack.push_back(temp);
			inFile >> temp;
			if (temp == '-'){
				currentState = BLOCKCOMMENT;
				currentToken += storageStack.back();
				storageStack.pop_back();
				currentToken += temp;
			}
			else{
				storageStack.push_back(temp);
			}
		}
		else{
			storageStack.push_back(temp);
		}
	}
	else if(((temp >= 'a') && (temp <= 'z')) || (temp == '_') || ((temp >='A') && (temp <= 'Z')))
		currentState = IDENT;
	else if(checkIfDefinedToken(currentToken))
		currentState = DEFINEDTOKEN;
	else if(temp == inFile.eof())
		currentState = EEOF;
	else 
		currentState = UNKNOWN;

	cout << "got here" << endl;
	//Start of State Machine 
	while(true){
		switch(currentState){
		case EEOF:
			return Token(currentToken, "EOF", currentState);
			break;
		case UNKNOWN:
			return Token(currentToken, "UNKNOWN", currentState);
			break;
		case INVALID:
			return Token(currentToken, "INVALID", INVALID);
			break;
		case DEFINEDTOKEN:
		{
			int duplicate = 0;
			int lastMatch = -1;
			while(currentState == DEFINEDTOKEN){
				for(int i = 0; i <= definedTokens.size(); i++){
					if( checkIfContains(currentToken, definedTokens[i]) ){
						duplicate++;
						lastMatch = i;
					}
					if(duplicate >= 2)
						i = definedTokens.size();
				}
				if(duplicate >= 2){
					inFile >> temp;
					duplicate = 0;
				}
				else if(duplicate == 1){
					return Token(currentToken, definedTokens[lastMatch], currentState);
				}
				else if(duplicate == 0){
					storageStack.push_back(temp);
					currentState = INVALID; 
				}
				else if(inFile.eof()){
					currentState = INVALID;
				}
				else{
					currentState = UNKNOWN;
				}
			}
		}
			break;
		case IDENT:
			while(currentState == IDENT){
				inFile >> temp;
				if(((temp >= 'a') && (temp <= 'z')) || (temp == '_') 
				   || ((temp >='A') && (temp <= 'Z')) || ((temp >= '0' && (temp <= '9')))){
					;//Do Nothing
				}
				else if ( inFile.eof() )
					return Token(currentToken, "IDENT", currentState);
				else{
					storageStack.push_back(temp);
					currentState = INVALID; 
				}
			}
			break;
		case INTEGER:
			//Error <------------------------------------------------
			// 1e+q == Int Ident Operator Ident
			// check here for case above???
			while(currentState == INTEGER){
				inFile >> temp;
				if(temp >= '0' && temp <= '9'){
					;//Do Nothing
				}
				else if(temp == '.'){
					currentToken += temp;
					currentState = REAL;
				}
				else if( inFile.eof() )
					return Token(currentToken, "INTEGER", currentState);
				else{
					storageStack.push_back(temp);
					currentState = INVALID; 
				}
			}
			break;
		case REAL:
			//Error <------------------------------------------------
			// 1e+1 == REAL
			//already has the .
			//eof
			break;
		case STRING:
			while(currentState == STRING){
				inFile >> temp;
				if(temp == '\"'){
					currentToken += temp;
					return Token(currentToken, "STRING", currentState);
				}
				else if( temp == '\\'){
					inFile >> temp;
					if( temp == '\\' )
						currentToken += "\\" ;
					else if( temp == 'n' )
						currentToken += "\n";
					else if( temp == 't' )
						currentToken += "\t";
					else if( temp == 'r' )
						currentToken += "\r";
					else if( temp == 'a' )
						currentToken += "\a";
					else if( temp == 'b' )
						currentToken += "\b";
					else if( temp == 'u' ){
						;//uF6F6F6
					}
					else{
						currentToken += "\\";
						currentToken += temp;
					}
				}
				else if( inFile.eof() )
					return Token(currentToken, "STRING", currentState);
				else
					currentToken += temp;
			}
			break;
		case LINECOMMENT:
			while(currentState == LINECOMMENT){
				inFile >> temp;
				if(temp == '\n' || inFile.eof())
					return Token(currentToken, "LINECOMMENT", currentState);
				else
					currentToken += temp;
			}
			break;
		case BLOCKCOMMENT:
			while(currentState == BLOCKCOMMENT){
				inFile >> temp;
				if( temp == '-' ){
					inFile >> temp;
					if( temp == '>'){
						inFile >> temp;
						if(temp == '>'){
							currentToken += "->>";
							return Token(currentToken, "BLOCKCOMMENT", currentState);
						}
						else
							currentToken += "->";
					}
					else
						currentToken += "-";
					
					if( inFile.eof() )
						return Token(currentToken, "BLOCKCOMMENT", currentState);
					else
						currentToken += temp;
				}
				else if ( inFile.eof() )
					return Token(currentToken, "BLOCKCOMMENT", currentState);
				else
					currentToken += temp;
			}
			break;
		default:
			// Nothing???
			break;
		}	
	}
}

/*
 * Check if the string contans part of the current token
 * which the restriction of this copy being concatanated at
 * the front of the defined token.
 * Example: if cT was % and dT was *% it returns false but
 *          if cT was % and dt was %* it returns true. 
 */
bool LexicalAnalyzer::checkIfContains(string currentToken, string definedToken){
	for(int i = 0; i <= currentToken.length(); i++)
		if( currentToken[i] != definedToken[i])
			return false;
	return true;
}

/*
 * Check if the current token could be a defined token
 */
bool LexicalAnalyzer::checkIfDefinedToken(string token){
	for(int i = 0; i <= definedTokens.size(); i++)
		if( checkIfContains(token, definedTokens[i]))
			return true;
	return false;
}
