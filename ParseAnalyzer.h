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
    char action; //r 规约  s 移进 a accept e error
    int state; //现在状态，如果是规约动作，则保存规约式编号
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
struct symform	//符号表
{
    string name;
    int addr;
    int flag; //0 变量 1 数组
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
    /**语义*/
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
