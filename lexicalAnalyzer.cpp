#include "lexicalAnalyzer.hpp"

#include <iostream>
using std::cout;

using std::cerr;
using std::endl;
using std::string;

/*
 * Default Constructor
 */
LexicalAnalyzer::LexicalAnalyzer() {
	currentState = NNULL;

	// Even is the text, Odd/+one is the Type
	definedTokens.push_back("+");
	definedTokens.push_back("PLUS");
	definedTokens.push_back("-");
	definedTokens.push_back("MINUS");
	definedTokens.push_back("*");
	definedTokens.push_back("MULT");
	definedTokens.push_back("/");
	definedTokens.push_back("DIV");
	definedTokens.push_back("^");
	definedTokens.push_back("EXP");
	definedTokens.push_back("<");
	definedTokens.push_back("LESS");
	definedTokens.push_back("<=");
	definedTokens.push_back("LESS_EQ");
	definedTokens.push_back(">");
	definedTokens.push_back("GREATER");
	definedTokens.push_back(">=");
	definedTokens.push_back("GREATER_EQ");
	definedTokens.push_back("=");
	definedTokens.push_back("EQUAL");
	definedTokens.push_back("<-");
	definedTokens.push_back("ASSIGN");
	definedTokens.push_back("~=");
	definedTokens.push_back("NOT_EQUAL");
	definedTokens.push_back("~");
	definedTokens.push_back("Squigle");
	definedTokens.push_back("!");
	definedTokens.push_back("NOT");
	definedTokens.push_back("(");
	definedTokens.push_back("LPAREN");
	definedTokens.push_back(")");
	definedTokens.push_back("RPAREN");
	definedTokens.push_back("{");
	definedTokens.push_back("LBRACE");
	definedTokens.push_back("}");
	definedTokens.push_back("RBRACE");
	definedTokens.push_back("[");
	definedTokens.push_back("LBRACKET");
	definedTokens.push_back("]");
	definedTokens.push_back("RBRACKET");
	definedTokens.push_back("&");
	definedTokens.push_back("AND");
	definedTokens.push_back("|");
	definedTokens.push_back("OR");
	definedTokens.push_back(".");
	definedTokens.push_back("DOT");
	definedTokens.push_back("@");
	definedTokens.push_back("AT");
	definedTokens.push_back(":");
	definedTokens.push_back("COLON");
	definedTokens.push_back(";");
	definedTokens.push_back("SEMICOLON");
	definedTokens.push_back(",");
	definedTokens.push_back("COMMA");
}

/*
 * Startup the token reader state
 */
void LexicalAnalyzer::init(string filename) {
	inFile.open(filename);
	if (!inFile) cerr << "Unable to open file: " << filename << endl;
}

/*
 * Get the next token
 */
Token LexicalAnalyzer::getToken() {
	char temp = '\0';
	string currentToken = "";
	currentState = NNULL;

	if (storageStack.empty())
		inFile >> temp;
	else {
		temp = storageStack.back();
		storageStack.pop_back();
	}

	currentToken += temp;

	// Remove white space
	while (currentState != EEOF &&
	       (currentToken == "\r" || currentToken == "\n" ||
	        currentToken == "\t" || currentToken == " ")) {
		if (storageStack.empty()) {
			if (inFile.eof()) currentState = EEOF;
			inFile >> temp;
		} else {
			temp = storageStack.back();
			storageStack.pop_back();
		}
		currentToken = temp;
	}

	// Start of State primer
	if (temp <= '9' && temp >= '1')
		currentState = INTEGER;
	else if (currentToken == "0")
		return Token(currentToken, "INTEGER", INTEGER);
	else if (currentToken == "\"")
		currentState = STRING;
	else if (currentToken == "#")
		currentState = LINECOMMENT;
	else if (currentToken == "<") {
		if (storageStack.empty())
			inFile >> temp;
		else {
			temp = storageStack.back();
			storageStack.pop_back();
		}
		if (temp == '<') {
			if (storageStack.empty())
				inFile >> temp;
			else {
				temp = storageStack.back();
				storageStack.pop_back();
			}
			if (temp == '-') {
				currentState = BLOCKCOMMENT;
				currentToken += "<-";
			} else {
				storageStack.push_back(temp);
				storageStack.push_back('<');
				temp = '<';
				currentState = DEFINEDTOKEN;
			}
		} else {
			storageStack.push_back(temp);
			temp = '<';
			currentState = DEFINEDTOKEN;
		}
	} else if (((temp >= 'a') && (temp <= 'z')) || (temp == '_') ||
	           ((temp >= 'A') && (temp <= 'Z')))
		currentState = IDENT;
	else if (checkIfDefinedToken(currentToken))
		currentState = DEFINEDTOKEN;
	else if (inFile.eof())
		currentState = EEOF;
	else
		currentState = UNKNOWN;

	// Start of State Machine
	while (true) {
		switch (currentState) {
			case EEOF:
				return Token(currentToken, "EOF", currentState);
				break;
			case UNKNOWN:
				return Token(currentToken, "UNKNOWN", currentState);
				break;
			case INVALID:
				return Token(currentToken, "INVALID", INVALID);
				break;
			case DEFINEDTOKEN: {
				if (temp == '.') {
					if (storageStack.empty())
						inFile >> temp;
					else {
						temp = storageStack.back();
						storageStack.pop_back();
					}

					if (temp >= '0' && temp <= '9') {
						storageStack.push_back(temp);
						temp = '.';
						currentState = REAL;
					} else {
						storageStack.push_back(temp);
						return Token(".", "DOT", currentState);
					}
				}

				int lastPerfectMatch = -1;

				while (currentState == DEFINEDTOKEN) {
					int duplicate = 0;

					for (int i = 0; i <= definedTokens.size(); i++) {
						if (checkIfStarts(currentToken, definedTokens[i])) {
							duplicate++;
						}
						if (0 == currentToken.compare(definedTokens[i]))
							lastPerfectMatch = i;
					}

					if (duplicate >= 1) {
						if (storageStack.empty())
							inFile >> temp;
						else {
							temp = storageStack.back();
							storageStack.pop_back();
						}

						if (!checkIfDefinedToken(currentToken + temp)) {
							storageStack.push_back(temp);
							return Token(currentToken, definedTokens[lastPerfectMatch + 1],
							             currentState);
						} else
							currentToken += temp;

					} else if (inFile.eof()) {
						if (lastPerfectMatch >= 0)
							return Token(currentToken, definedTokens[lastPerfectMatch + 1],
							             currentState);
						else
							currentState = INVALID;
					}
				}
			} break;
			case IDENT:
				while (currentState == IDENT) {
					if (storageStack.empty())
						inFile.get(temp);
					else {
						temp = storageStack.back();
						storageStack.pop_back();
					}

					if (inFile.eof())
						return Token(currentToken, "IDENT", currentState);
					else if (((temp >= 'a') && (temp <= 'z')) || (temp == '_') ||
					         ((temp >= 'A') && (temp <= 'Z')) ||
					         ((temp >= '0') && (temp <= '9'))) {
						currentToken += temp;
					}

					else {
						storageStack.push_back(temp);
						return Token(currentToken, "IDENT", currentState);
					}
				}
				break;
			case INTEGER:
				while (currentState == INTEGER) {
					inFile.get(temp);
					if (temp >= '0' && temp <= '9') {
						currentToken += temp;
					} else if (temp == '.' || temp == 'e' || temp == 'E') {
						if (temp == '.') currentToken += temp;
						currentState = REAL;
					} else if (inFile.eof())
						return Token(currentToken, "INTEGER", currentState);
					else {
						storageStack.push_back(temp);
						return Token(currentToken, "INTEGER", currentState);
					}
				}
				break;
			case REAL:
				if (temp == '.') {
					if (storageStack.empty())
						inFile.get(temp);
					else {
						temp = storageStack.back();
						storageStack.pop_back();
					}

					if (temp >= '0' && temp <= '9') {
						while (temp >= '0' && temp <= '9') {
							currentToken += temp;
							inFile.get(temp);
						}
					} else {
						storageStack.push_back(temp);
						return Token(currentToken, "REAL", DEFINEDTOKEN);
					}
				}

				if (temp == 'e' || temp == 'E') {
					char eTemp = temp;
					inFile.get(temp);
					char oTemp = temp;
					if (temp == '+' || temp == '-') {
						inFile.get(temp);
						if (temp >= '1' && temp <= '9') {
							currentToken += eTemp;
							;
							currentToken += oTemp;
							currentToken += temp;
						} else {
							storageStack.push_back(temp);
							storageStack.push_back(oTemp);
							storageStack.push_back(eTemp);
							return Token(currentToken, "REAL", currentState);
						}
					} else {
						if (temp >= '1' && temp <= '9') {
							currentToken += eTemp;
							currentToken += temp;
						} else {
							storageStack.push_back(oTemp);
							storageStack.push_back(eTemp);
							return Token(currentToken, "REAL", currentState);
						}
					}

					inFile.get(temp);
					while (temp >= '0' && temp <= '9') {
						currentToken += temp;
						inFile.get(temp);
					}
				}

				return Token(currentToken, "REAL", currentState);

				break;
			case STRING:
				while (currentState == STRING) {
					if (storageStack.empty())
						inFile.get(temp);
					else {
						temp = storageStack.back();
						storageStack.pop_back();
					}

					if (temp == '\"') {
						currentToken += temp;
						return Token(currentToken, "STRING", currentState);
					} else if (temp == '\\') {
						inFile.get(temp);
						if (temp == '\\')
							currentToken += "\\";
						else if (temp == 'n')
							currentToken += "\n";
						else if (temp == 't')
							currentToken += "\t";
						else if (temp == 'r')
							currentToken += "\r";
						else if (temp == 'a')
							currentToken += "\a";
						else if (temp == 'b')
							currentToken += "\b";
						else if (temp == 'u') {
							;  // uF6F6F6
						} else {
							currentToken += "\\";
							currentToken += temp;
						}
					} else if (inFile.eof())
						currentState = INVALID;
					else
						currentToken += temp;
				}
				break;
			case LINECOMMENT:
				while (currentState == LINECOMMENT) {
					if (storageStack.empty())
						inFile.get(temp);
					else {
						temp = storageStack.back();
						storageStack.pop_back();
					}

					if (temp == '\n' || inFile.eof())
						return getToken();
					else
						currentToken += temp;
				}
				break;
			case BLOCKCOMMENT:
				while (currentState == BLOCKCOMMENT) {
					if (storageStack.empty())
						inFile.get(temp);
					else {
						temp = storageStack.back();
						storageStack.pop_back();
					}

					if (temp == '-') {
						inFile.get(temp);
						if (temp == '>') {
							inFile.get(temp);
							if (temp == '>') {
								currentToken += "->>";
								return getToken();
							} else
								currentToken += "->";
						} else
							currentToken += "-";

						if (inFile.eof())
							currentState = INVALID;
						else
							currentToken += temp;
					} else if (inFile.eof())
						currentState = INVALID;
					else
						currentToken += temp;
				}
				break;
			default:
				currentState = UNKNOWN;
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
bool LexicalAnalyzer::checkIfStarts(string currentToken, string definedToken) {
	for (int i = 0; i < currentToken.length(); i++)
		if (currentToken[i] != definedToken[i]) return false;
	return true;
}

/*
 * Check if the current token could be a defined token
 */
bool LexicalAnalyzer::checkIfDefinedToken(string token) {
	for (int i = 0; i < definedTokens.size(); i++)
		if (checkIfStarts(token, definedTokens[i])) return true;
	return false;
}

/*
 *
 */
string hexToUnicode(string hex) { return "Unicode Char"; }
