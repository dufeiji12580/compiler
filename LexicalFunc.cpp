#include <iostream>
#include "LexicalAnalyzer.h"
#include "ConstDefine.h"
#include "ErrorDefine.h"
using namespace std;
void Lexical::analyze(string str,int line)
{
	int x = 0, y = 0;
	int len = str.length();
	while (y < len)
	{
		if (isannotation == true)
		{
			while (y < len - 1 && !(str[y] == '*' && str[y + 1] == '/'))
				y++;
			if (y == len - 1)
			{
				return;
			}
			else
			{
				isannotation = false;
				y += 2;
				x = y;
			}
		}
		while (isspace(str[y]) && y < len)
			y++;
		x = y;
		if (y == len)
		{
			return;
		}
		if (isalpha(str[y]) || str[y] == '_')
		{
			int result;
			while (isalnum(str[y]) || str[y] == '_')
				y++;
			s.words = str.substr(x, y-x);
			if ((result = checkiskeyword(s.words)) != -1)
            {
                if(result == 34)
                    return;
                s.label = result;
            }
			else
				s.label = ID;
			s.line = line;
			svector.push_back(s);
			x = y;
		}
		else if (isdigit(str[y]))
		{
			while (isdigit(str[y]))
				y++;
			s.words = str.substr(x, y - x);
			s.label = DIG;
			s.line = line;
			svector.push_back(s);
			x = y;
		}
		else
		{
			switch (str[y])
			{
			case '*':
				y++;
				s.label = MULTI;
				s.words = str.substr(x, y - x);
				s.line = line;
				svector.push_back(s);
				x = y;
				break;
			case '<':
				y++;
				if (str[y] == '=')
				{
					y++;
					s.label = LQ;
				}
				else
					s.label = LT;
				s.words = str.substr(x, y - x);
				s.line = line;
				svector.push_back(s);
				x = y;
				break;
			case '=':
				y++;
				if (str[y] == '=')
				{
					y++;
					s.label = EQEQ;
				}
				else
					s.label = EQ;
				s.words = str.substr(x, y - x);
				s.line = line;
				svector.push_back(s);
				x = y;
				break;
			case '>':
				y++;
				if (str[y] == '=')
				{
					y++;
					s.label = RQ;
				}
				else
					s.label = RT;
				s.words = str.substr(x, y - x);
				s.line = line;
				svector.push_back(s);
				x = y;
				break;
			case '+':
				y++;
				if (str[y] == '+')
				{
					y++;
					s.label = PLUSPLUS;
				}
				else
					s.label = PLUS;
				s.words = str.substr(x, y - x);
				s.line = line;
				svector.push_back(s);
				x = y;
				break;
			case '-':
				y++;
				if (str[y] == '-')
				{
					y++;
					s.label = MINUSMINUS;
				}
				else
					s.label = MINUS;
				s.words = str.substr(x, y - x);
				s.line = line;
				svector.push_back(s);
				x = y;
				break;
			case '/':
				y++;
				if (str[y] == '/')
				{
					return;
				}
				else if (str[y] == '*')
				{
					y++;
					while (y < len - 1 && !(str[y] == '*' && str[y + 1] == '/'))
						y++;
					if (y == len - 1)
					{
						isannotation = true;
						return;
					}
					y += 2;
					x = y;
					break;
				}
				else
					s.label = DIV;
				s.words = str.substr(x, y - x);
				s.line = line;
				svector.push_back(s);
				x = y;
				break;
			case '!':
				y++;
				if (str[y] == '=')
				{
					y++;
					s.label = NEQ;
				}
				else
				{
					cout << "line: " << line << "  " << UNRECOGNIZED << " " << str[y] << endl;
					return;
				}
				s.words = str.substr(x, y - x);
				s.line = line;
				svector.push_back(s);
				x = y;
				break;
			case '"':
				y++;
				s.words = str.substr(x, y - x);
				s.label = DQ_MARKS;
				s.line = line;
				svector.push_back(s);
				x = y;
				while (y<len && str[y] != '"')
				{
					y++;
				}
				if (y == len)
				{
					s.words = str.substr(x, y - x);
					s.label = ERROR;
					s.line = line;
					svector.push_back(s);
					cout << "line: " << line << "  " << DQ_MARKS_ERROR << endl;
					return;
				}
				s.words = str.substr(x, y - x);
				s.label = STR;
				s.line = line;
				svector.push_back(s);
				x = y;
				y++;
				s.words = str.substr(x, y - x);
				s.label = DQ_MARKS;
				s.line = line;
				svector.push_back(s);
				x = y;
				break;
			case '[':
				y++;
				s.words = str.substr(x, y - x);
				s.label = LS_BRAC;
				s.line = line;
				svector.push_back(s);
				x = y;
				break;
			case ']':
				y++;
				s.words = str.substr(x, y - x);
				s.label = RS_BRAC;
				s.line = line;
				svector.push_back(s);
				x = y;
				break;
			case '(':
				y++;
				s.words = str.substr(x, y - x);
				s.label = LR_BRAC;
				s.line = line;
				svector.push_back(s);
				x = y;
				break;
			case ')':
				y++;
				s.words = str.substr(x, y - x);
				s.label = RR_BRAC;
				s.line = line;
				svector.push_back(s);
				x = y;
				break;
			case '{':
				y++;
				s.words = str.substr(x, y - x);
				s.label = LB_BRAC;
				s.line = line;
				svector.push_back(s);
				x = y;
				break;
			case '}':
				y++;
				s.words = str.substr(x, y - x);
				s.label = RB_BRAC;
				s.line = line;
				svector.push_back(s);
				x = y;
				break;
			case ',':
				y++;
				s.words = str.substr(x, y - x);
				s.label = COMMA;
				s.line = line;
				svector.push_back(s);
				x = y;
				break;
			case ';':
				y++;
				s.words = str.substr(x, y - x);
				s.label = SEMIC;
				s.line = line;
				svector.push_back(s);
				x = y;
				break;
			case '&':
				y++;
				if (str[y] == '&')
				{
					y++;
					s.label = AND;
				}
				else
				{
					cout << "line: " << line << "  " << UNRECOGNIZED << " " << str[y] << endl;
					return;
				}
				s.words = str.substr(x, y - x);
				s.line = line;
				svector.push_back(s);
				x = y;
				break;
			case '|':
				y++;
				if (str[y] == '|')
				{
					y++;
					s.label = OR;
				}
				else
				{
					cout << "line: " << line << "  " << UNRECOGNIZED << " " << str[y] << endl;
					return;
				}
				s.words = str.substr(x, y - x);
				s.line = line;
				svector.push_back(s);
				x = y;
				break;
			case '#':
				return;
				break;
			default:
				cout << "line: " << line << "  " << UNRECOGNIZED << " " << str[y] << endl;
				return;
				break;

			}
		}
	}
}

int Lexical:: checkiskeyword(string str)
{
	if (str == "int")
		return INT;
	else if (str == "if")
		return IF;
	else if (str == "else")
		return ELSE;
	else if (str == "else")
		return ELSE;
	else if (str == "while")
		return WHILE;
	else if (str == "for")
		return FOR;
	else if (str == "and")
		return AND;
	else if (str == "OR")
		return OR;
	else if (str == "main")
		return MAIN;
	else if (str == "printf")
		return PRINT;
    else if (str == "return")
		return RETURN;
	else
		return -1;
}
