#ifndef LEXICALANALYZER_H_INCLUDED
#define LEXICALANALYZER_H_INCLUDED

#include <string>
#include <vector>
using namespace std;

struct spie
{
	string words;
	int label;
	int line;
};

class Lexical
{
public:
	Lexical()
	{
		isannotation = false;
	};
	~Lexical(){}
	void analyze(string str,int line);
    vector<spie> svector;
private:
	int checkiskeyword(string str);
	spie s;
	bool isannotation;
};


#endif
