#include <iostream>
#include <fstream>
#include <sstream>
#include <ctype.h>
#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <iterator>
#include <iomanip>

using namespace std;

class DFA
{
	private:
		vector<string> finalStates;
		vector<string> alphabet;
		vector<map<string, string> > transitions;
		vector<string> transitionTokens;
		ifstream file;
		string start;
	public:
		DFA(string inputFile);
		~DFA();
		void getToken(string word, int &errorPos) const;
		void test();
};

class Token
{
	private:
		int type, value;
	public:
		static const int TYPE_OPERATOR = 0, TYPE_IDENTIFIER = 1,
						TYPE_KEYWORD = 2, TYPE_INTEGER = 3, TYPE_FLOAT = 4,
						TYPE_STRING = 5, TYPE_CHAR = 6, TYPE_SEPARATOR = 7,
						TYPE_SYMBOL = 8, TYPE_ERROR = 9;
		Token() {}
		Token(int type, int value)
		{
			this->type = type;
			this->value = value;
		}	
		~Token() {}
		int getType() { return this->type; }
		int getValue() { return this->value; }
};

vector<Token*> tokens;
vector<string> tableTokens;
void tokenize(string line, vector<string> &tokens, bool alpha = false);
bool isKeyword(string token);
string trim(string& line);
void stripComments(ifstream &fin, ofstream &fout);

DFA::DFA(string inputFile) 
{
	file.open(inputFile);
	if (file.is_open()) {
		string line, alpha, from, to;
		int index;

		getline(file, start);
		getline(file, line);
		tokenize(line, finalStates);
		getline(file, line);
		tokenize(line, alphabet, true);
		transitions.resize(alphabet.size());
		while (getline(file, line)) {
			transitionTokens.clear();
			tokenize(line, transitionTokens);
			from = transitionTokens[0];
			alpha = transitionTokens[1];
			to = transitionTokens[2];

			for (int i = 0; i < alphabet.size(); i++) {
				if (alphabet[i] == alpha) {
					index = i;
					break;
				}
			}

			transitions[index].insert(pair<string, string>(from, to));
		}
		/* only for whitespace */
		index = find(alphabet.begin(), alphabet.end(), " ") - alphabet.begin();
		transitions[index].insert(pair<string, string>("0", "50"));
		transitions[index].insert(pair<string, string>("37", "37"));
		/* end whitespace */
		file.close();
	} else {
		cout<<"Unable to open "<<inputFile<<" file";
	}
}

DFA::~DFA()
{
}

void DFA::getToken(string word, int &errorPos) const 
{
	bool valid = true;
	int i, index = 0, indexToken;
	string currentState = start;

	for (i = 0; i < word.length(); i++) {
		if (find(alphabet.begin(), alphabet.end(), string(1, word[i])) != alphabet.end()) {
			index = find(alphabet.begin(), alphabet.end(), string(1, word[i])) - alphabet.begin();
			if (transitions[index].find(currentState) != transitions[index].end()) {
				currentState = transitions[index].at(currentState);
			} else {
				break;
			}
		} else {
			break;
		}
	}
	//cout<<i<<" "<<currentState<<" "<<endl;
	
	if (find(tableTokens.begin(), tableTokens.end(), word.substr(0, i)) == tableTokens.end()) {
		tableTokens.push_back(word.substr(0, i));
	}
	indexToken = find(tableTokens.begin(), tableTokens.end(), word.substr(0, i)) - tableTokens.begin();
	errorPos += word.substr(0, i).length();
	switch (stoi(currentState)) {
		case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8: case 9: case 10: case 11:
		case 12: case 13: case 14: case 15: case 16: case 17: case 18: case 19: case 20: case 21:
		case 22: case 23: case 24: case 25: case 26: case 27: case 28: case 29: case 30: case 31:
		case 32:
			tokens.push_back(new Token(Token::TYPE_OPERATOR, indexToken));
			break;
		case 33:
			if (isKeyword(word.substr(0, i))) {
				tokens.push_back(new Token(Token::TYPE_KEYWORD, indexToken));
			} else {
				tokens.push_back(new Token(Token::TYPE_IDENTIFIER, indexToken));
			}
			break;
		case 34:
			tokens.push_back(new Token(Token::TYPE_INTEGER, indexToken));
			break;
		case 35: case 36:
			tokens.push_back(new Token(Token::TYPE_FLOAT, indexToken));
			break;
		case 38:
			tokens.push_back(new Token(Token::TYPE_STRING, indexToken));
			break;
		case 41:
			tokens.push_back(new Token(Token::TYPE_CHAR, indexToken));
			break;
		case 42: case 43:
			tokens.push_back(new Token(Token::TYPE_SEPARATOR, indexToken));
			break;
		case 44: case 45: case 46: case 47: case 48: case 49:
			tokens.push_back(new Token(Token::TYPE_SYMBOL, indexToken));
			break;
		case 50:
			break;	
		default: 
			tokens.push_back(new Token(Token::TYPE_ERROR, errorPos));
			i = word.end() - word.begin();
			break;									
	}
		//cout<<word.substr(i + 1, word.end() - word.begin());
		//exit(0);
	if (i <= (word.end() - word.begin() - 1)) {
		//cout<<"here"<<" "<<i<<" "<<(word.end() - word.begin() - 1)<<endl;
		DFA::getToken(word.substr(i, word.end() - word.begin() - 1), errorPos);
	}
	
}

void DFA::test()
{
	/*vector<string>::iterator iterator;
	for (iterator = alphabet.begin(); iterator < alphabet.end(); iterator++) {
		cout<<(iterator-alphabet.begin())<<*iterator<<"h"<<endl;
	}*/
	int index;
	index = find(alphabet.begin(), alphabet.end(), string(1, ' ')) - alphabet.begin();
	if (transitions[index].find("0") != transitions[index].end()) {
		cout<<transitions[index].at("0");
	} else {
		cout<<false;
	}
}

void tokenize(string line, vector<string> &tokens, bool alpha)
{
	istringstream ss(line);
	while(!ss.eof()) {
		string token;
		getline(ss, token, ' ');
		tokens.push_back(token);
	}
	if (alpha == true) {
		tokens.push_back(" ");
	}
}

bool isKeyword(string token) {
	if (token == "for" || token == "while" || token == "if" || token == "else" || token == "int" ||
		token == "float" || token == "short" || token == "do" || token == "char" || token == "return" ||
		token == "auto" || token == "struct" || token == "union" || token == "break" || token == "long" ||
		token == "double" || token == "const" || token == "unsigned" || token == "switch" || token == "continue" || 
		token == "signed" || token == "void" || token == "case" || token == "enum" || token == "register" ||
		token == "typedef" || token == "default" || token == "goto" || token == "extern" || token == "static" ||
		token == "main" || token == "printf" || token == "scanf") {
		return true;
	} else {
		return false;
	}
}

void trim(string& line, char t) {
	string::size_type pos = line.find_last_not_of(t);
	if (pos != string::npos) {
		line.erase(pos+1);
		pos = line.find_first_not_of(t);
		if (pos != string::npos) {
			line.erase(0, pos);
		}
	} else {
		line.erase(line.begin(), line.end());
	}
}

void stripComments(ifstream &fin, ofstream &fout)
{
	if (fin.is_open()) {
		char char1, char2;
		bool isInsideComment = false;
		fin.get(char1);
		while (!fin.eof()) {
			if (char1 == '/' && !isInsideComment) {
				fin.get(char2);
				if (char2 == '*') {
					isInsideComment = true;
				} else if (char2 == '/') {
					isInsideComment = true;
				} else {
					fout.put(char1);
					fout.put(char2);
				}
			} else if (char1 == '*' && isInsideComment) {
				fin.get(char2);
				if (char2 == '/') {
					isInsideComment = false;
				}
			} else if (!isInsideComment) {
				fout.put(char1);
			}
			fin.get(char1);
		}
		fin.close();
		fout.close();
	} else {
		cout<<"Unable to open input file for strip";
	}
}

int main()
{
	string input, output, finalOutput;
	int errorPos;
	cout<<"Enter a text file for lexical analysis: "<<endl;
	cin>>input;

	ifstream fin(input);
	output = input.insert(input.length()-4, "_stripped");
	finalOutput = input.insert(input.length()-4, "_output");
	ofstream fout(output);
	stripComments(fin, fout);

	ifstream file(output);
	ofstream out(finalOutput);

	if (file.is_open()) {
		string line;
		string token;
		char lookahead;

		DFA dfa("dfa.txt");
		while (getline(file, line)) {
			trim(line, '\t');
			trim(line, ' ');
			if (line != "") {
				errorPos = 0;
				dfa.getToken(line, errorPos);
				if (tokens.at(tokens.end() - tokens.begin() - 1)->getType() == Token::TYPE_ERROR) {
					break;
				}
			}
		}
		file.close();

		for (vector<Token*>::iterator it = tokens.begin(); it < tokens.end(); it++) {
			switch((*it)->getType()) {
				case 0:
					out<<"OPERATOR"<<" "<<tableTokens.at((*it)->getValue())<<endl;
					break;
				case 1:
					out<<"IDENTIFIER"<<" "<<tableTokens.at((*it)->getValue())<<endl;
					break;
				case 2:
					out<<"KEYWORD"<<" "<<tableTokens.at((*it)->getValue())<<endl;
					break;
				case 3:
					out<<"INTEGER"<<" "<<tableTokens.at((*it)->getValue())<<endl;
					break;
				case 4:
					out<<"FLOAT"<<" "<<tableTokens.at((*it)->getValue())<<endl;
					break;
				case 5:
					out<<"STRING"<<" "<<tableTokens.at((*it)->getValue())<<endl;
					break;
				case 6:
					out<<"CHAR"<<" "<<tableTokens.at((*it)->getValue())<<endl;
					break;
				case 7:
					out<<"SEPARATOR"<<" "<<tableTokens.at((*it)->getValue())<<endl;
					break;
				case 8:
					out<<"SYMBOL"<<" "<<tableTokens.at((*it)->getValue())<<endl;
					break;
				case 9:
					out<<"ERROR"<<" "<<(*it)->getValue()<<endl;
					break;
				default:
					break;
			}
		}
		out.close();
	} else {
		cout << "Unable to open file " << input << endl;
	}

	return 0;
}