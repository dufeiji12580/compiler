#ifndef PARSEANALYZER_H_INCLUDED
#define PARSEANALYZER_H_INCLUDED

#include <string>
#include <vector>
#include <fstream>
#include <stack>
#include "ConstDefine.h"
#include "LexicalAnalyzer.h"
using namespace std;

struct actionform
{
    char action; //r ��Լ  s �ƽ� a accept e error
    int state; //����״̬������ǹ�Լ�������򱣴��Լʽ���
};
struct parsenode
{
    int state;
    string symbol;
};
struct codenode
{
    vector<string> code;
    string result;
    bool isreg;
    bool isdig;
};
struct reduce
{
    vector<string> tokenlist;
};
struct symform	//���ű�
{
    string name;
    int addr;
    int flag; //0 ���� 1 ����
};
class parse
{
public:
    parse();
    ~parse()
    {
        out.close();
        asmout.close();
    };
    void parse_analyze(Lexical &L);
    void lalr_analyze(spie s);
    void semantic_analyze(int label);
    void display();
private:
    actionform action[STATENUM][ENDNUM];
    int go_to[STATENUM][NONENDNUM];
    stack<parsenode> parsestack;
    vector<parsenode> tempparse;
    void readproduction();
    void readlalr();
    string ignorelabel(string str);
    int stringtonum(string str);
    vector<reduce> reducelist;
    ofstream out,asmout;
    /**����*/
    vector<symform> form;
    stack<codenode> codestack;
    int offset;
    void addform(string name,int flag);
    int findeaxnum(string str);
    string randommake();
    int nownum;
    bool eaxreg;
    bool ebxreg;
};

#endif // PARSEANALYZER_H_INCLUDED
