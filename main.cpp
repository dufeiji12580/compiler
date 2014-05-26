#include <iostream>
#include <fstream>
#include <string>
#include <conio.h>
#include <stdlib.h>
#include "LexicalAnalyzer.h"
#include "ParseAnalyzer.h"
using namespace std;

int main()
{
	ifstream fin;
	Lexical L;
	parse p;
	int i = 0,j = 0;
	fin.open("test.c", ios_base::in);
	if (!fin.is_open())
	{
		cout << "Open Faild" <<endl;
	}
	else
	{
		string str;
		while (getline(fin, str))
		{
			j++;
			L.analyze(str,j);
		}
		for (i = 0; i < L.svector.size(); i++)
		{
			cout << L.svector[i].words << " ";
			cout << L.svector[i].label <<"\t";
		}
		fin.close();
	}
    p.parse_analyze(L);
    p.display();
	return 0;
}
