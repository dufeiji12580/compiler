#include <fstream>
#include <iostream>
#include <conio.h>
#include <stdlib.h>
#include <string>
#include "ParseAnalyzer.h"
#include "ConstDefine.h"
#include "ErrorDefine.h"
using namespace std;

/**构造函数，调用函数初始化action，goto表，以及栈底*/
parse :: parse()
{
    offset = 0;
    nownum = 0;
    eaxreg = false;
    ebxreg = false;
    int i,j;
    parsenode p;
    p.state = 0;
    p.symbol = -1;
    parsestack.push(p);

    for (i = 0; i < STATENUM; i++)
    {
        for (j = 0; j < ENDNUM; j++)
        {
            action[i][j].action = 'e';
            action[i][j].state = -1;
        }
    }
    for (i = 0; i < STATENUM; i++)
    {
        for(j = 0; j < NONENDNUM; j++)
        {
            go_to[i][j] = -1;
        }
    }
    readproduction();
    readlalr();
    out.open("out.txt");
    if (!out.is_open())
    {
        cout << "Open out.txt Faild" << endl;
        exit(1);
    }
}
/**读词法分析器输出进行语法分析*/
void parse::parse_analyze(Lexical &L)
{
    for (int i = 1; i < L.svector.size(); i++)
    {
        out << L.svector[i].words <<endl;
        lalr_analyze(L.svector[i]);
    }
    /**最后输入终结符编号*/
    spie s;
    s.label = ENDNUM - 1;
    lalr_analyze(s);
}
/**进行语法分析*/
void parse::lalr_analyze(spie s)
{
    char nextaction;
    int tempstate;
    tempparse.clear();
    parsenode stacktop = parsestack.top();
    nextaction = action[stacktop.state][s.label].action;
    tempstate = action[stacktop.state][s.label].state;
    /**移进*/
    if(nextaction == 's')
    {
        parsenode p;
        p.state = tempstate;
        p.symbol = s.words;
        parsestack.push(p);
    }
    /**归约*/
    else if(nextaction == 'r')
    {
        int length = reducelist[tempstate].tokenlist.size()-1;
        int symbol =  stringtonum(reducelist[tempstate].tokenlist[0]);
        for(int i = 0; i<length; i++)
        {
            tempparse.push_back(parsestack.top());
            parsestack.pop();
        }
        parsenode newstacktop = parsestack.top();
        parsenode p;
        p.state = go_to[newstacktop.state][symbol-51];
        p.symbol = reducelist[tempstate].tokenlist[0];
        parsestack.push(p);
        out << reducelist[tempstate].tokenlist[0] << " => ";
        for(int m = 1; m<reducelist[tempstate].tokenlist.size(); m++)
            out << reducelist[tempstate].tokenlist[m] << " ";
        out <<endl;
        semantic_analyze(tempstate);
        lalr_analyze(s);	//规约后再次查看
    }
    /**接受状态*/
    else if(nextaction == 'a')
    {
        out << "........................success.........................." <<endl;
    }
    else
    {
        out << "........................Error.........................." <<endl;
    }
}
/**除去HTM标签*/
string parse::ignorelabel(string str)
{
    int j;
    j = str.find(">");
    str = str.substr(j+1, str.size() - j-1);
    j = str.find("<");
    str = str.substr(0, j);
    return str;
}
/**读htm文件，产生goto表和action表*/
void parse ::  readlalr()
{
    ifstream fin;
    string str;
    fin.open("lalr.txt", ios_base::in);
    if (!fin.is_open())
    {
        cout << "Open lalr.txt Faild" << endl;
        exit(1);
    }
    for (int i = 0; i < STATENUM; i++)
    {
        getline(fin, str);
        getline(fin, str);
        int temp;
        for (int j = 0; j < ENDNUM; j++)
        {
            getline(fin, str);
            str = ignorelabel(str);
            if (str[0] == 's')
            {
                action[i][j].action = 's';
                temp = str.find(';');
                action[i][j].state = atoi(str.substr(temp + 1, str.size() - temp - 1).c_str());
            }
            else if (str[0] == 'r')
            {
                int x,y;
                bool rightnull = false;
                string tempstr;
                vector<string> rightlist;
                rightlist.clear();
                action[i][j].action = 'r';
                temp = str.find(";");//去掉reduce
                x = temp+1;
                y = x;
                while(isalpha(str[y]) || str[y] == '_')
                    y++;
                rightlist.push_back(str.substr(x, y-x));
                temp = str.find("->");
                if(!isalpha(str[temp+8]))//空产生式
                {
                    rightnull = true;
                }
                else
                {
                    y = temp+8;
                    x = y;
                    while(y<str.size())
                    {
                        while(str[y] != '&'&&y<str.size())
                            y++;
                        tempstr = str.substr(x,y-x);
                        rightlist.push_back(tempstr);
                        while(str[y] != ';' && y<str.size())
                            y++;
                        y++;
                        x = y;
                    }
                }
                for (int k = 0; k < reducelist.size(); k++)
                {
                    if (rightlist[0].compare(reducelist[k].tokenlist[0]) == 0)
                    {
                        if(rightnull == true && reducelist[k].tokenlist.size() == 1)
                        {
                            action[i][j].state = k;
                            break;
                        }
                        else if(rightlist.size() == reducelist[k].tokenlist.size())
                        {
                            int l;
                            for(l = 1; l<rightlist.size(); l++)
                            {
                                if(rightlist[l].compare(reducelist[k].tokenlist[l]) != 0)
                                    break;
                            }
                            if(l == rightlist.size())
                            {
                                action[i][j].state = k;
                                break;
                            }
                        }
                    }
                }
                rightlist.clear();
            }
            else if (str[0] == 'a')
            {
                action[i][j].state = -1;
                action[i][j].action = 'a';
            }
        }
        for (int j = 0; j < NONENDNUM; j++)
        {
            getline(fin, str);
            str = ignorelabel(str);
            if (str[0] == '&')
                go_to[i][j] = -1;
            else
                go_to[i][j] = atoi(str.c_str());
        }
        getline(fin, str);
    }
    fin.close();
}
/**读产生式*/
void parse :: readproduction()
{
    ifstream fin;
    string str;
    reduce r;
    fin.open("production.txt", ios_base::in);
    if (!fin.is_open())
    {
        cout << "Open production.txt Faild" <<endl;
        exit (1);
    }
    while (getline(fin, str))
    {
        int len = str.size();
        int num = 0;
        int x = 0, y = 0;
        string tempstr;
        while (y < len)
        {
            while (isspace(str[y]) && y < len)
                y++;
            if (y == len)
            {
                return;
            }
            x = y;
            while (isalpha(str[y]) || str[y] == '_')
                y++;
            r.tokenlist.push_back(str.substr(x, y-x));
            while (isspace(str[y]) && y < len)
                y++;
            if(str[y] == '=' && str[y+1] == '>')
                y+=2;
            while (isspace(str[y]) && y < len)
                y++;
            while(y<len)
            {
                x = y;
                while (isalpha(str[y]) || str[y] == '_')
                    y++;
                tempstr = str.substr(x,y-x);
                r.tokenlist.push_back(tempstr);
                num++;
                while (isspace(str[y]) && y < len)
                    y++;
            }
        }
        reducelist.push_back(r);
        r.tokenlist.clear();
    }
    fin.close();
}
/**返回非终结符的编号*/
int parse::stringtonum(string str)
{
    if(!str.compare("start"))
        return start;
    else if(!str.compare("statement"))
        return statement;
    else if(!str.compare("while_statement"))
        return while_statement;
    else if(!str.compare("if_statement"))
        return if_statement;
    else if(!str.compare("for_statement"))
        return for_statement;
    else if(!str.compare("compound_statement"))
        return compound_statement;
    else if(!str.compare("block_item_list"))
        return block_item_list;
    else if(!str.compare("primary_expression"))
        return primary_expression;
    else if(!str.compare("postfix_expression"))
        return postfix_expression;
    else if(!str.compare("multiplication_expression"))
        return multiplication_expression;
    else if(!str.compare("additive_expression"))
        return additive_expression;
    else if(!str.compare("relational_expression"))
        return relational_expression;
    else if(!str.compare("expression"))
        return expression;
    else if(!str.compare("assignment_expression"))
        return assignment_expression;
    else if(!str.compare("string_literal"))
        return string_literal;
    else if(!str.compare("declaration"))
        return declaration;
    else if(!str.compare("init_declarator"))
        return init_declarator;
    else if(!str.compare("init_declarator_list"))
        return init_declarator_list;
    else if(!str.compare("array_initializer"))
        return array_initializer;
    else if(!str.compare("initializer_list"))
        return initializer_list;
    else if(!str.compare("logical_AND_expression"))
        return logical_AND_expression;
    else if(!str.compare("logical_OR_expression"))
        return logical_OR_expression;
    else if(!str.compare("out_paralist"))
        return out_paralist;
    else if(!str.compare("out_statement"))
        return out_statement;
    else if(!str.compare("init_array_left"))
        return init_array_left;
    else if(!str.compare("A"))
        return A;
    else if(!str.compare("B"))
        return B;
    else if(!str.compare("C"))
        return C;
    else if(!str.compare("D"))
        return D;
    else if(!str.compare("E"))
        return E;
    else if(!str.compare("F"))
        return F;
    else if(!str.compare("G"))
        return G;
    else if(!str.compare("H"))
        return H;
}
/**添加符号表*/
void parse :: addform(string name,int flag)
{
    symform s;
    s.name = name;
    s.flag = flag;
    s.addr = offset;
    form.push_back(s);
}
/**产生标号*/
string parse :: randommake()
{
    char buf[10];
    itoa(nownum,buf,10);
    nownum++;
    return "L"+string(buf)+":\t";
}

int parse :: findeaxnum(string str)
{
    int i,num = 0;
    if(str.substr(0,4) == "eax,")
        num++;
    i = str.find(",");
    while(i != -1)
    {
        if(i+4 < str.size())
        {
            if(str.substr(i+1,4) == "eax,")
                num++;
        }
        else if(i+4 == str.size())
        {
            if(str.substr(i+1,3) == "eax")
                num++;
        }
        str = str.substr(i+1,str.size()-i-1);
        i = str.find(",");
    }
    return num;
}
/**目标代码的输出*/
void parse :: display()
{
    ofstream ooo;
    ooo.open("action.txt");
    for (int i = 0; i < STATENUM; i++)
    {
        for (int j = 0; j < ENDNUM; j++)
        {
            ooo << action[i][j].action << action[i][j].state << "\t";
        }
        for(int j = 0; j < NONENDNUM; j++)
        {
            ooo << go_to[i][j] <<"\t";
        }
        ooo<<endl;
    }
    ooo.close();
    char buf[10];
    asmout.open("asm.asm");
    if (!asmout.is_open())
    {
        cout << "Open asm.asm Faild" << endl;
        exit(1);
    }
    asmout <<
           ".686P\n"	<<
           ".MODEL flat, stdcall\n" <<
           ".STACK 4096\n" <<
           "option casemap:none\n\n" <<
           "printf          PROTO C :dword,:vararg\n" <<
           "ExitProcess	PROTO :DWORD\n\n"<<
           "INCLUDELIB C:\\Irvine\\USER32.LIB\n" <<
           "INCLUDELIB C:\\Irvine\\KERNEL32.LIB\n" <<
           "INCLUDELIB C:\\Irvine\\MSVCRT.LIB\n\n" <<
           "exit equ <INVOKE ExitProcess,0>\n\n" <<
           "chr$ MACRO any_text:vararg\n" <<
           "\tLOCAL textname\n" <<
           ".data\n" <<
           "\ttextname db any_text,0\n" <<
           "\tALIGN 4\n" <<
           ".code\n" <<
           "\tEXITM<OFFSET textname>\n" <<
           "ENDM\n\n" <<
           ".data" << endl;
    for(int i = 0; i<form.size(); i++)
    {
        if(form[i].flag == 0)
        {
            asmout << form[i].name + " dword ?" <<endl;
        }
        else
        {
            itoa(form[i+1].addr-form[i].addr,buf,10);
            asmout << form[i].name + " dword " + buf + " dup(?)"<<endl;
        }
    }
    asmout << endl;
    asmout << ".code" <<endl;
    asmout << "main PROC" <<endl;
    while(!codestack.empty())
    {
        for(int i = 0; i<codestack.top().code.size(); i++)
        {
            asmout << codestack.top().code[i] <<endl;
        }
        codestack.pop();
    }
    itoa(nownum,buf,10);
    asmout << "L"+string(buf)+":\texit\n";
    asmout << "main ENDP\n" << "END main";
}

/**语义的分析*/
void parse :: semantic_analyze(int label)
{
    codenode tempnode1,tempnode2,tempnode3,tempnode4,tempnode5,tempnode6;
    parsenode tempparsenode1,tempparsenode2;
    tempnode1.code.clear();
    tempnode2.code.clear();
    tempnode3.code.clear();
    tempnode4.code.clear();
    tempnode5.code.clear();
    tempnode6.code.clear();
    char buf[10];
    int i = 0,j = 0,k = 0,l = 0,eaxnum = 0;
    string tempstr,tempstr2,tempstr3;
    switch(label)
    {
    case 0://start => MAIN LR_BRAC RR_BRAC LB_BRAC block_item_list RB_BRAC

        break;
    case 1://statement => expression SEMIC
        eaxreg = false;
        break;
    case 2://statement => if_statement
        eaxreg = false;
        break;
    case 3://statement => while_statement
        eaxreg = false;
        break;
    case 4://statement => for_statement
        eaxreg = false;
        break;
    case 5://statement => declaration
        eaxreg = false;
        break;
    case 6://statement => out_statement
        eaxreg = false;
        break;
    case 7://while_statement => WHILE LR_BRAC D expression RR_BRAC E compound_statement
        tempnode4 = codestack.top();
        codestack.pop();
        tempnode3 = codestack.top();
        codestack.pop();
        tempnode2 = codestack.top();
        codestack.pop();
        tempnode1 = codestack.top();
        codestack.pop();
        tempnode4.code.push_back(randommake()+"jmp L" + tempnode1.result);
        itoa(nownum,buf,10);
        tempstr = tempnode3.code[tempnode3.code.size()-1];
        tempnode3.code[tempnode3.code.size()-1] = tempstr + string(buf);
        for(int i = 0; i < tempnode4.code.size(); i++)
        {
            tempnode3.code.push_back(tempnode4.code[i]);
        }
        for(int i = 0; i < tempnode3.code.size(); i++)
        {
            tempnode2.code.push_back(tempnode3.code[i]);
        }
        for(int i = 0; i < tempnode2.code.size(); i++)
        {
            tempnode1.code.push_back(tempnode2.code[i]);
        }
        codestack.push(tempnode1);
        break;
    case 8://for_statement => FOR LR_BRAC expression SEMIC F expression SEMIC G expression H RR_BRAC compound_statement
        tempnode6 = codestack.top();
        codestack.pop();
        tempnode5 = codestack.top();
        codestack.pop();
        tempnode4 = codestack.top();
        codestack.pop();
        tempnode3 = codestack.top();
        codestack.pop();
        tempnode2 = codestack.top();
        codestack.pop();
        tempnode1 = codestack.top();
        codestack.pop();
        tempnode6.code.push_back(randommake()+"jmp L" + tempnode3.result);
        itoa(nownum,buf,10);
        tempstr = tempnode3.code[tempnode3.code.size()-1];
        tempnode3.code[tempnode3.code.size()-1] = tempstr + string(buf);
        for(int i = 0; i < tempnode6.code.size(); i++)
        {
            tempnode5.code.push_back(tempnode6.code[i]);
        }
        for(int i = 0; i < tempnode5.code.size(); i++)
        {
            tempnode4.code.push_back(tempnode5.code[i]);
        }
        for(int i = 0; i < tempnode4.code.size(); i++)
        {
            tempnode3.code.push_back(tempnode4.code[i]);
        }
        for(int i = 0; i < tempnode3.code.size(); i++)
        {
            tempnode2.code.push_back(tempnode3.code[i]);
        }
        for(int i = 0; i < tempnode2.code.size(); i++)
        {
            tempnode1.code.push_back(tempnode2.code[i]);
        }
        tempnode2 = codestack.top();
        codestack.pop();
        for(int i = 0; i < tempnode1.code.size(); i++)
        {
            tempnode2.code.push_back(tempnode1.code[i]);
        }
        codestack.push(tempnode2);
        break;
    case 9://if_statement => IF LR_BRAC expression RR_BRAC A compound_statement
        tempnode2 = codestack.top();
        codestack.pop();
        tempnode1 = codestack.top();
        codestack.pop();
        itoa(nownum,buf,10);
        tempstr = tempnode1.code[tempnode1.code.size()-1];
        tempnode1.code[tempnode1.code.size()-1] = tempstr + string(buf);
        for(int i = 0; i < tempnode2.code.size(); i++)
        {
            tempnode1.code.push_back(tempnode2.code[i]);
        }
        tempnode2 = codestack.top();
        codestack.pop();
        for(int i = 0; i < tempnode1.code.size(); i++)
        {
            tempnode2.code.push_back(tempnode1.code[i]);
        }
        codestack.push(tempnode2);
        break;
    case 10://if_statement => IF LR_BRAC expression RR_BRAC A compound_statement B ELSE C compound_statement
        tempnode3 = codestack.top();
        codestack.pop();
        tempnode2 = codestack.top();
        codestack.pop();
        tempnode1 = codestack.top();
        codestack.pop();
        itoa(nownum,buf,10);
        tempstr = tempnode1.code[tempnode1.code.size()-1];
        tempnode1.code[tempnode1.code.size()-1] = tempstr + string(buf);
        for(int i = 0; i < tempnode3.code.size(); i++)
        {
            tempnode2.code.push_back(tempnode3.code[i]);
        }
        for(int i = 0; i < tempnode2.code.size(); i++)
        {
            tempnode1.code.push_back(tempnode2.code[i]);
        }
        tempnode2 = codestack.top();
        codestack.pop();
        for(int i = 0; i < tempnode1.code.size(); i++)
        {
            tempnode2.code.push_back(tempnode1.code[i]);
        }
        tempnode1 = codestack.top();
        codestack.pop();
        for(int i = 0; i < tempnode2.code.size(); i++)
        {
            tempnode1.code.push_back(tempnode2.code[i]);
        }
        tempnode2 = codestack.top();
        codestack.pop();
        for(int i = 0; i < tempnode1.code.size(); i++)
        {
            tempnode2.code.push_back(tempnode1.code[i]);
        }
        codestack.push(tempnode2);
        break;
    case 11://compound_statement => statement
        eaxreg = false;
        break;
    case 12://compound_statement => LB_BRAC block_item_list RB_BRAC
        eaxreg = false;
        break;
    case 13://block_item_list => statement
        eaxreg = false;
        break;
    case 14://block_item_list => statement block_item_list
        tempnode1 = codestack.top();
        codestack.pop();
        tempnode2 = codestack.top();
        codestack.pop();
        for(int i = 0; i < tempnode1.code.size(); i++)
        {
            tempnode2.code.push_back(tempnode1.code[i]);
        }
        codestack.push(tempnode2);
        break;
    case 15://primary_expression => ID
        tempnode1.isreg = false;
        tempnode1.isdig = false;
        tempnode1.result = tempparse[0].symbol;
        codestack.push(tempnode1);
        break;
    case 16://primary_expression => ID LS_BRAC expression RS_BRAC
        tempnode1 = codestack.top();
        codestack.pop();
        tempparsenode1 = parsestack.top();
        parsestack.pop();
        tempparsenode2 = parsestack.top();
        parsestack.push(tempparsenode1);
        if(tempparsenode2.symbol != "statement")
        {
            if(tempnode1.isdig == true)
            {
                if(eaxreg == true)
                {
                    tempnode1.code.push_back(randommake()+"push eax");
                    eaxreg = false;
                }
                tempnode1.code.push_back(randommake()+"mov eax, [" + tempparse[3].symbol+ "+" + tempnode1.result + "*4]");
                tempnode1.result = "eax";
                eaxreg = true;
            }
            else if(tempnode1.isreg == false)
            {
                if(eaxreg == true)
                {
                    tempnode1.code.push_back(randommake()+"push eax");
                    eaxreg = false;
                }
                tempnode1.code.push_back(randommake()+"mov esi, [" + tempnode1.result + "]");
                tempnode1.code.push_back(randommake()+"mov eax, [" + tempparse[3].symbol+ "+esi*4]");
                eaxreg = true;
            }
            else
            {
                if(eaxreg == false)
                    tempnode1.code.push_back(randommake()+"pop eax");
                tempnode1.code.push_back(randommake()+"mov esi, eax");
                tempnode1.code.push_back(randommake()+"mov eax, [" + tempparse[3].symbol+ "+esi*4]");
                eaxreg = true;
            }
            tempnode1.result = "eax";
            tempnode1.isreg = true;
            tempnode1.isdig = false;
            codestack.push(tempnode1);
        }
        else
        {
            tempnode1.result = tempparse[3].symbol + tempparse[2].symbol + tempnode1.result + tempparse[0].symbol;
            codestack.push(tempnode1);
        }
        break;
    case 17://postfix_expression => primary_expression

        break;
    case 18://postfix_expression => LR_BRAC expression RR_BRAC

        break;
    case 19://postfix_expression => DIG
        tempnode1.isreg = false;
        tempnode1.isdig = true;
        tempnode1.result = tempparse[0].symbol;
        codestack.push(tempnode1);
        break;
    case 20://postfix_expression => primary_expression PLUSPLUS
        tempnode1 = codestack.top();
        codestack.pop();
        tempstr = tempnode1.result;
        if(tempstr.find("[") == -1)
        {
            tempnode1.code.push_back(randommake()+"inc [" + tempnode1.result +"]");
            tempnode1.isdig = false;
            tempnode1.isreg = false;
            codestack.push(tempnode1);
        }
        else
        {
            i = tempstr.find("[");
            j = tempstr.find("]");
            tempstr2 = tempstr.substr(i+1,j-i-1);
            tempnode1.code.push_back(randommake()+"inc [" + tempstr.substr(0,i) + "+" + tempstr2 +"*4]");
            tempnode1.isdig = false;
            tempnode1.isreg = false;
            codestack.push(tempnode1);
        }
        break;
    case 21://postfix_expression => primary_expression MINUSMINUS
        tempnode1 = codestack.top();
        codestack.pop();
        tempstr = tempnode1.result;
        if(tempstr.find("[") == -1)
        {
            tempnode1.code.push_back(randommake()+"dec [" + tempnode1.result +"]");
            tempnode1.isdig = false;
            tempnode1.isreg = false;
            codestack.push(tempnode1);
        }
        else
        {
            i = tempstr.find("[");
            j = tempstr.find("]");
            tempstr2 = tempstr.substr(i+1,j-i-1);
            tempnode1.code.push_back(randommake()+"dec [" + tempstr.substr(0,i) + "+" + tempstr2 +"*4]");
            tempnode1.isdig = false;
            tempnode1.isreg = false;
            codestack.push(tempnode1);
        }
        break;
    case 22://multiplication_expression => postfix_expression

        break;
    case 23://multiplication_expression => multiplication_expression MULTI postfix_expression
        tempnode2 = codestack.top();
        codestack.pop();
        tempnode1 = codestack.top();
        codestack.pop();
        for(int i = 0; i < tempnode2.code.size(); i++)
        {
            tempnode1.code.push_back(tempnode2.code[i]);
        }
        if(tempnode2.isreg == false && tempnode1.isreg == false)
        {
            if(eaxreg == true)
            {
                tempnode1.code.push_back(randommake()+"push eax");
                eaxreg = false;
            }
            if(tempnode1.isdig == true)
                tempnode1.code.push_back(randommake()+"mov eax, " + tempnode1.result);
            else
                tempnode1.code.push_back(randommake()+"mov eax, [" + tempnode1.result + "]");
            if(tempnode2.isdig == true)
                tempnode1.code.push_back(randommake()+"mov ebx, " + tempnode2.result);
            else
                tempnode1.code.push_back(randommake()+"mov ebx, [" + tempnode2.result + "]");
            tempnode1.code.push_back(randommake()+"mul ebx");
            tempnode1.result = "eax";
            eaxreg = true;
        }
        else if(tempnode1.isreg == false && tempnode2.isreg == true)
        {
            if(eaxreg == false)
                tempnode1.code.push_back(randommake()+"pop ebx");
            else
            {
                tempnode1.code.push_back(randommake()+"mov ebx, eax");
                eaxreg = false;
            }
            if(tempnode1.isdig == true)
                tempnode1.code.push_back(randommake()+"mov eax, " + tempnode1.result);
            else
                tempnode1.code.push_back(randommake()+"mov eax, [" + tempnode1.result + "]");
            tempnode1.code.push_back(randommake()+"mul ebx");
            tempnode1.result = "eax";
            eaxreg = true;
        }
        else if(tempnode1.isreg == true && tempnode2.isreg == false)
        {
            if(eaxreg == false)
                tempnode1.code.push_back(randommake()+"pop eax");
            if(tempnode2.isdig == true)
                tempnode1.code.push_back(randommake()+"mov ebx, " + tempnode2.result);
            else
                tempnode1.code.push_back(randommake()+"mov ebx, [" + tempnode2.result + "]");
            tempnode1.code.push_back(randommake()+"mul ebx");
            tempnode1.result = "eax";
            eaxreg = true;
        }
        else
        {
            tempnode1.code.push_back(randommake()+"mov ebx, eax");
            tempnode1.code.push_back(randommake()+"pop eax");
            tempnode1.code.push_back(randommake()+"mul ebx");
            tempnode1.result = "eax";
            eaxreg = true;
        }
        tempnode1.isreg = true;
        tempnode1.isdig = false;
        codestack.push(tempnode1);
        break;
    case 24://multiplication_expression => multiplication_expression DIV postfix_expression
        tempnode2 = codestack.top();
        codestack.pop();
        tempnode1 = codestack.top();
        codestack.pop();
        for(int i = 0; i < tempnode2.code.size(); i++)
        {
            tempnode1.code.push_back(tempnode2.code[i]);
        }
        tempnode1.code.push_back(randommake()+"mov edx, 0");
        if(tempnode2.isreg == false && tempnode1.isreg == false)
        {
            if(eaxreg == true)
            {
                tempnode1.code.push_back(randommake()+"push eax");
                eaxreg = false;
            }
            if(tempnode1.isdig == true)
                tempnode1.code.push_back(randommake()+"mov eax, " + tempnode1.result);
            else
                tempnode1.code.push_back(randommake()+"mov eax, [" + tempnode1.result + "]");
            if(tempnode2.isdig == true)
                tempnode1.code.push_back(randommake()+"mov ebx, " + tempnode2.result);
            else
                tempnode1.code.push_back(randommake()+"mov ebx, [" + tempnode2.result + "]");
            tempnode1.code.push_back(randommake()+"div ebx");
            tempnode1.result = "eax";
            eaxreg = true;
        }
        else if(tempnode1.isreg == false && tempnode2.isreg == true)
        {
            if(eaxreg == false)
                tempnode1.code.push_back(randommake()+"pop ebx");
            else
            {
                tempnode1.code.push_back(randommake()+"mov ebx, eax");
                eaxreg = false;
            }
            if(tempnode1.isdig == true)
                tempnode1.code.push_back(randommake()+"mov eax, " + tempnode1.result);
            else
                tempnode1.code.push_back(randommake()+"mov eax, [" + tempnode1.result + "]");
            tempnode1.code.push_back(randommake()+"div ebx");
            tempnode1.result = "eax";
            eaxreg = true;
        }
        else if(tempnode1.isreg == true && tempnode2.isreg == false)
        {
            if(eaxreg == false)
                tempnode1.code.push_back(randommake()+"pop eax");
            if(tempnode2.isdig == true)
                tempnode1.code.push_back(randommake()+"mov ebx, " + tempnode2.result);
            else
                tempnode1.code.push_back(randommake()+"mov ebx, [" + tempnode2.result + "]");
            tempnode1.code.push_back(randommake()+"div ebx");
            tempnode1.result = "eax";
            eaxreg = true;
        }
        else
        {
            tempnode1.code.push_back(randommake()+"mov ebx, eax");
            tempnode1.code.push_back(randommake()+"pop eax");
            tempnode1.code.push_back(randommake()+"div ebx");
            tempnode1.result = "eax";
            eaxreg = true;
        }
        tempnode1.isreg = true;
        tempnode1.isdig = false;
        codestack.push(tempnode1);
        break;
    case 25://additive_expression => multiplication_expression

        break;
    case 26://additive_expression =>	additive_expression PLUS multiplication_expression
        tempnode2 = codestack.top();
        codestack.pop();
        tempnode1 = codestack.top();
        codestack.pop();
        for(int i = 0; i < tempnode2.code.size(); i++)
        {
            tempnode1.code.push_back(tempnode2.code[i]);
        }
        if(tempnode2.isreg == false && tempnode1.isreg == false)
        {
            if(eaxreg == true)
            {
                tempnode1.code.push_back(randommake()+"push eax");
                eaxreg = false;
            }
            if(tempnode1.isdig == true)
                tempnode1.code.push_back(randommake()+"mov eax, " + tempnode1.result);
            else
                tempnode1.code.push_back(randommake()+"mov eax, [" + tempnode1.result + "]");
            if(tempnode2.isdig == true)
                tempnode1.code.push_back(randommake()+"add eax, " + tempnode2.result);
            else
                tempnode1.code.push_back(randommake()+"add eax, [" + tempnode2.result + "]");
            tempnode1.result = "eax";
            eaxreg = true;
        }
        else if(tempnode1.isreg == false && tempnode2.isreg == true)
        {
            if(eaxreg == false)
                tempnode1.code.push_back(randommake()+"pop eax");
            if(tempnode1.isdig == true)
                tempnode1.code.push_back(randommake()+"add eax, " + tempnode1.result);
            else
                tempnode1.code.push_back(randommake()+"add eax, [" + tempnode1.result + "]");
            tempnode1.result = "eax";
            eaxreg = true;
        }
        else if(tempnode1.isreg == true && tempnode2.isreg == false)
        {
            if(eaxreg == false)
                tempnode1.code.push_back(randommake()+"pop eax");
            if(tempnode2.isdig == true)
                tempnode1.code.push_back(randommake()+"add eax, " + tempnode2.result);
            else
                tempnode1.code.push_back(randommake()+"add eax, [" + tempnode2.result + "]");
            tempnode1.result = "eax";
            eaxreg = true;
        }
        else
        {
            tempnode1.code.push_back(randommake()+"pop ebx");
            tempnode1.code.push_back(randommake()+"add eax,ebx");
            tempnode1.result = "eax";
            eaxreg = true;
        }
        tempnode1.isreg = true;
        tempnode1.isdig = false;
        codestack.push(tempnode1);
        break;
    case 27://additive_expression =>	additive_expression MINUS multiplication_expression
        tempnode2 = codestack.top();
        codestack.pop();
        tempnode1 = codestack.top();
        codestack.pop();
        for(int i = 0; i < tempnode2.code.size(); i++)
        {
            tempnode1.code.push_back(tempnode2.code[i]);
        }
        if(tempnode2.isreg == false && tempnode1.isreg == false)
        {
            if(eaxreg == true)
            {
                tempnode1.code.push_back(randommake()+"push eax");
                eaxreg = false;
            }
            if(tempnode1.isdig == true)
                tempnode1.code.push_back(randommake()+"mov eax, " + tempnode1.result);
            else
                tempnode1.code.push_back(randommake()+"mov eax, [" + tempnode1.result + "]");
            if(tempnode2.isdig == true)
                tempnode1.code.push_back(randommake()+"sub eax, " + tempnode2.result);
            else
                tempnode1.code.push_back(randommake()+"sub eax, [" + tempnode2.result + "]");
            tempnode1.result = "eax";
            eaxreg = true;
        }
        else if(tempnode1.isreg == false && tempnode2.isreg == true)
        {
            if(eaxreg == false)
                tempnode1.code.push_back(randommake()+"pop ebx");
            else
            {
                tempnode1.code.push_back(randommake()+"mov ebx, eax");
                eaxreg = false;
            }
            if(tempnode1.isdig == true)
                tempnode1.code.push_back(randommake()+"mov eax, " + tempnode1.result);
            else
                tempnode1.code.push_back(randommake()+"mov eax, [" + tempnode1.result + "]");
            tempnode1.code.push_back(randommake()+"sub eax, ebx");
            tempnode1.result = "eax";
            eaxreg = true;
        }
        else if(tempnode1.isreg == true && tempnode2.isreg == false)
        {
            if(eaxreg == false)
                tempnode1.code.push_back(randommake()+"pop eax");
            if(tempnode2.isdig == true)
                tempnode1.code.push_back(randommake()+"sub eax, " + tempnode2.result);
            else
                tempnode1.code.push_back(randommake()+"sub eax, [" + tempnode2.result + "]");
            eaxreg = false;
            tempnode1.result = "eax";
            eaxreg = true;
        }
        else
        {
            tempnode1.code.push_back(randommake()+"mov ebx, eax");
            tempnode1.code.push_back(randommake()+"pop eax");
            tempnode1.code.push_back(randommake()+"sub eax, ebx");
            tempnode1.result = "eax";
            eaxreg = true;
        }
        tempnode1.isreg = true;
        tempnode1.isdig = false;
        codestack.push(tempnode1);
        break;
    case 28://relational_expression => additive_expression

        break;
    case 29://relational_expression => relational_expression LT additive_expression
        tempnode2 = codestack.top();
        codestack.pop();
        tempnode1 = codestack.top();
        codestack.pop();
        for(int i = 0; i < tempnode2.code.size(); i++)
        {
            tempnode1.code.push_back(tempnode2.code[i]);
        }
        if(tempnode2.isreg == false && tempnode1.isreg == false)
        {
            if(eaxreg == true)
            {
                tempnode1.code.push_back(randommake()+"push eax");
                eaxreg = false;
            }
            if(tempnode1.isdig == true)
                tempnode1.code.push_back(randommake()+"mov eax, " + tempnode1.result);
            else
                tempnode1.code.push_back(randommake()+"mov eax, [" + tempnode1.result + "]");
            if(tempnode2.isdig == true)
                tempnode1.code.push_back(randommake()+"cmp eax, " + tempnode2.result);
            else
                tempnode1.code.push_back(randommake()+"cmp eax, [" + tempnode2.result + "]");
        }
        else if(tempnode1.isreg == false && tempnode2.isreg == true)
        {
            if(eaxreg == false)
                tempnode1.code.push_back(randommake()+"pop ebx");
            else
            {
                tempnode1.code.push_back(randommake()+"mov ebx, eax");
                eaxreg = false;
            }
            if(tempnode1.isdig == true)
                tempnode1.code.push_back(randommake()+"mov eax, " + tempnode1.result);
            else
                tempnode1.code.push_back(randommake()+"mov eax, [" + tempnode1.result + "]");
            tempnode1.code.push_back(randommake()+"cmp eax, ebx");
        }
        else if(tempnode1.isreg == true && tempnode2.isreg == false)
        {
            if(eaxreg == false)
                tempnode1.code.push_back(randommake()+"pop eax");
            if(tempnode2.isdig == true)
                tempnode1.code.push_back(randommake()+"cmp eax, " + tempnode2.result);
            else
                tempnode1.code.push_back(randommake()+"cmp eax, [" + tempnode2.result + "]");
        }
        else
        {
            tempnode1.code.push_back(randommake()+"mov ebx, eax");
            tempnode1.code.push_back(randommake()+"pop eax");
            tempnode1.code.push_back(randommake()+"cmp eax, ebx");
        }
        itoa(nownum+3,buf,10);
        tempnode1.code.push_back(randommake()+"jb L" + string(buf));
        tempnode1.code.push_back(randommake()+"mov eax, 0");
        itoa(nownum+2,buf,10);
        tempnode1.code.push_back(randommake()+"jmp L" + string(buf));
        tempnode1.code.push_back(randommake()+"mov eax, 1");
        tempnode1.result = "eax";
        eaxreg = true;
        tempnode1.isreg = true;
        tempnode1.isdig = false;
        codestack.push(tempnode1);
        break;
    case 30://relational_expression => relational_expression RT additive_expression
        tempnode2 = codestack.top();
        codestack.pop();
        tempnode1 = codestack.top();
        codestack.pop();
        for(int i = 0; i < tempnode2.code.size(); i++)
        {
            tempnode1.code.push_back(tempnode2.code[i]);
        }
        if(tempnode2.isreg == false && tempnode1.isreg == false)
        {
            if(eaxreg == true)
            {
                tempnode1.code.push_back(randommake()+"push eax");
                eaxreg = false;
            }
            if(tempnode1.isdig == true)
                tempnode1.code.push_back(randommake()+"mov eax, " + tempnode1.result);
            else
                tempnode1.code.push_back(randommake()+"mov eax, [" + tempnode1.result + "]");
            if(tempnode2.isdig == true)
                tempnode1.code.push_back(randommake()+"cmp eax, " + tempnode2.result);
            else
                tempnode1.code.push_back(randommake()+"cmp eax, [" + tempnode2.result + "]");
        }
        else if(tempnode1.isreg == false && tempnode2.isreg == true)
        {
            if(eaxreg == false)
                tempnode1.code.push_back(randommake()+"pop ebx");
            else
            {
                tempnode1.code.push_back(randommake()+"mov ebx, eax");
                eaxreg = false;
            }
            if(tempnode1.isdig == true)
                tempnode1.code.push_back(randommake()+"mov eax, " + tempnode1.result);
            else
                tempnode1.code.push_back(randommake()+"mov eax, [" + tempnode1.result + "]");
            tempnode1.code.push_back(randommake()+"cmp eax, ebx");
        }
        else if(tempnode1.isreg == true && tempnode2.isreg == false)
        {
            if(eaxreg == false)
                tempnode1.code.push_back(randommake()+"pop eax");
            if(tempnode2.isdig == true)
                tempnode1.code.push_back(randommake()+"cmp eax, " + tempnode2.result);
            else
                tempnode1.code.push_back(randommake()+"cmp eax, [" + tempnode2.result + "]");
        }
        else
        {
            tempnode1.code.push_back(randommake()+"mov ebx, eax");
            tempnode1.code.push_back(randommake()+"pop eax");
            tempnode1.code.push_back(randommake()+"cmp eax, ebx");
        }
        itoa(nownum+3,buf,10);
        tempnode1.code.push_back(randommake()+"ja L" + string(buf));
        tempnode1.code.push_back(randommake()+"mov eax, 0");
        itoa(nownum+2,buf,10);
        tempnode1.code.push_back(randommake()+"jmp L" + string(buf));
        tempnode1.code.push_back(randommake()+"mov eax, 1");
        tempnode1.result = "eax";
        eaxreg = true;
        tempnode1.isreg = true;
        tempnode1.isdig = false;
        codestack.push(tempnode1);
        break;
    case 31://relational_expression => relational_expression LQ additive_expression
        tempnode2 = codestack.top();
        codestack.pop();
        tempnode1 = codestack.top();
        codestack.pop();
        for(int i = 0; i < tempnode2.code.size(); i++)
        {
            tempnode1.code.push_back(tempnode2.code[i]);
        }
        if(tempnode2.isreg == false && tempnode1.isreg == false)
        {
            if(eaxreg == true)
            {
                tempnode1.code.push_back(randommake()+"push eax");
                eaxreg = false;
            }
            if(tempnode1.isdig == true)
                tempnode1.code.push_back(randommake()+"mov eax, " + tempnode1.result);
            else
                tempnode1.code.push_back(randommake()+"mov eax, [" + tempnode1.result + "]");
            if(tempnode2.isdig == true)
                tempnode1.code.push_back(randommake()+"cmp eax, " + tempnode2.result);
            else
                tempnode1.code.push_back(randommake()+"cmp eax, [" + tempnode2.result + "]");
        }
        else if(tempnode1.isreg == false && tempnode2.isreg == true)
        {
            if(eaxreg == false)
                tempnode1.code.push_back(randommake()+"pop ebx");
            else
            {
                tempnode1.code.push_back(randommake()+"mov ebx, eax");
                eaxreg = false;
            }
            if(tempnode1.isdig == true)
                tempnode1.code.push_back(randommake()+"mov eax, " + tempnode1.result);
            else
                tempnode1.code.push_back(randommake()+"mov eax, [" + tempnode1.result + "]");
            tempnode1.code.push_back(randommake()+"cmp eax, ebx");
        }
        else if(tempnode1.isreg == true && tempnode2.isreg == false)
        {
            if(eaxreg == false)
                tempnode1.code.push_back(randommake()+"pop eax");
            if(tempnode2.isdig == true)
                tempnode1.code.push_back(randommake()+"cmp eax, " + tempnode2.result);
            else
                tempnode1.code.push_back(randommake()+"cmp eax, [" + tempnode2.result + "]");
        }
        else
        {
            tempnode1.code.push_back(randommake()+"mov ebx, eax");
            tempnode1.code.push_back(randommake()+"pop eax");
            tempnode1.code.push_back(randommake()+"cmp eax, ebx");
        }
        itoa(nownum+3,buf,10);
        tempnode1.code.push_back(randommake()+"jbe L" + string(buf));
        tempnode1.code.push_back(randommake()+"mov eax, 0");
        itoa(nownum+2,buf,10);
        tempnode1.code.push_back(randommake()+"jmp L" + string(buf));
        tempnode1.code.push_back(randommake()+"mov eax, 1");
        tempnode1.result = "eax";
        eaxreg = true;
        tempnode1.isreg = true;
        tempnode1.isdig = false;
        codestack.push(tempnode1);
        break;
    case 32://relational_expression => relational_expression RQ additive_expression
        tempnode2 = codestack.top();
        codestack.pop();
        tempnode1 = codestack.top();
        codestack.pop();
        for(int i = 0; i < tempnode2.code.size(); i++)
        {
            tempnode1.code.push_back(tempnode2.code[i]);
        }
        if(tempnode2.isreg == false && tempnode1.isreg == false)
        {
            if(eaxreg == true)
            {
                tempnode1.code.push_back(randommake()+"push eax");
                eaxreg = false;
            }
            if(tempnode1.isdig == true)
                tempnode1.code.push_back(randommake()+"mov eax, " + tempnode1.result);
            else
                tempnode1.code.push_back(randommake()+"mov eax, [" + tempnode1.result + "]");
            if(tempnode2.isdig == true)
                tempnode1.code.push_back(randommake()+"cmp eax, " + tempnode2.result);
            else
                tempnode1.code.push_back(randommake()+"cmp eax, [" + tempnode2.result + "]");
        }
        else if(tempnode1.isreg == false && tempnode2.isreg == true)
        {
            if(eaxreg == false)
                tempnode1.code.push_back(randommake()+"pop ebx");
            else
            {
                tempnode1.code.push_back(randommake()+"mov ebx, eax");
                eaxreg = false;
            }
            if(tempnode1.isdig == true)
                tempnode1.code.push_back(randommake()+"mov eax, " + tempnode1.result);
            else
                tempnode1.code.push_back(randommake()+"mov eax, [" + tempnode1.result + "]");
            tempnode1.code.push_back(randommake()+"cmp eax, ebx");
        }
        else if(tempnode1.isreg == true && tempnode2.isreg == false)
        {
            if(eaxreg == false)
                tempnode1.code.push_back(randommake()+"pop eax");
            if(tempnode2.isdig == true)
                tempnode1.code.push_back(randommake()+"cmp eax, " + tempnode2.result);
            else
                tempnode1.code.push_back(randommake()+"cmp eax, [" + tempnode2.result + "]");
        }
        else
        {
            tempnode1.code.push_back(randommake()+"mov ebx, eax");
            tempnode1.code.push_back(randommake()+"pop eax");
            tempnode1.code.push_back(randommake()+"cmp eax, ebx");
        }
        itoa(nownum+3,buf,10);
        tempnode1.code.push_back(randommake()+"jae L" + string(buf));
        tempnode1.code.push_back(randommake()+"mov eax, 0");
        itoa(nownum+2,buf,10);
        tempnode1.code.push_back(randommake()+"jmp L" + string(buf));
        tempnode1.code.push_back(randommake()+"mov eax, 1");
        tempnode1.result = "eax";
        eaxreg = true;
        tempnode1.isreg = true;
        tempnode1.isdig = false;
        codestack.push(tempnode1);
        break;
    case 33://relational_expression => relational_expression EQEQ additive_expression
        tempnode2 = codestack.top();
        codestack.pop();
        tempnode1 = codestack.top();
        codestack.pop();
        for(int i = 0; i < tempnode2.code.size(); i++)
        {
            tempnode1.code.push_back(tempnode2.code[i]);
        }
        if(tempnode2.isreg == false && tempnode1.isreg == false)
        {
            if(eaxreg == true)
            {
                tempnode1.code.push_back(randommake()+"push eax");
                eaxreg = false;
            }
            if(tempnode1.isdig == true)
                tempnode1.code.push_back(randommake()+"mov eax, " + tempnode1.result);
            else
                tempnode1.code.push_back(randommake()+"mov eax, [" + tempnode1.result + "]");
            if(tempnode2.isdig == true)
                tempnode1.code.push_back(randommake()+"cmp eax, " + tempnode2.result);
            else
                tempnode1.code.push_back(randommake()+"cmp eax, [" + tempnode2.result + "]");
        }
        else if(tempnode1.isreg == false && tempnode2.isreg == true)
        {
            if(eaxreg == false)
                tempnode1.code.push_back(randommake()+"pop ebx");
            else
            {
                tempnode1.code.push_back(randommake()+"mov ebx, eax");
                eaxreg = false;
            }
            if(tempnode1.isdig == true)
                tempnode1.code.push_back(randommake()+"mov eax, " + tempnode1.result);
            else
                tempnode1.code.push_back(randommake()+"mov eax, [" + tempnode1.result + "]");
            tempnode1.code.push_back(randommake()+"cmp eax, ebx");
        }
        else if(tempnode1.isreg == true && tempnode2.isreg == false)
        {
            if(eaxreg == false)
                tempnode1.code.push_back(randommake()+"pop eax");
            if(tempnode2.isdig == true)
                tempnode1.code.push_back(randommake()+"cmp eax, " + tempnode2.result);
            else
                tempnode1.code.push_back(randommake()+"cmp eax, [" + tempnode2.result + "]");
        }
        else
        {
            tempnode1.code.push_back(randommake()+"mov ebx, eax");
            tempnode1.code.push_back(randommake()+"pop eax");
            tempnode1.code.push_back(randommake()+"cmp eax, ebx");
        }
        itoa(nownum+3,buf,10);
        tempnode1.code.push_back(randommake()+"jz L" + string(buf));
        tempnode1.code.push_back(randommake()+"mov eax, 0");
        itoa(nownum+2,buf,10);
        tempnode1.code.push_back(randommake()+"jmp L" + string(buf));
        tempnode1.code.push_back(randommake()+"mov eax, 1");
        tempnode1.result = "eax";
        eaxreg = true;
        tempnode1.isreg = true;
        tempnode1.isdig = false;
        codestack.push(tempnode1);
        break;
    case 34://relational_expression => relational_expression NEQ additive_expression
        tempnode2 = codestack.top();
        codestack.pop();
        tempnode1 = codestack.top();
        codestack.pop();
        for(int i = 0; i < tempnode2.code.size(); i++)
        {
            tempnode1.code.push_back(tempnode2.code[i]);
        }
        if(tempnode2.isreg == false && tempnode1.isreg == false)
        {
            if(eaxreg == true)
            {
                tempnode1.code.push_back(randommake()+"push eax");
                eaxreg = false;
            }
            if(tempnode1.isdig == true)
                tempnode1.code.push_back(randommake()+"mov eax, " + tempnode1.result);
            else
                tempnode1.code.push_back(randommake()+"mov eax, [" + tempnode1.result + "]");
            if(tempnode2.isdig == true)
                tempnode1.code.push_back(randommake()+"cmp eax, " + tempnode2.result);
            else
                tempnode1.code.push_back(randommake()+"cmp eax, [" + tempnode2.result + "]");
        }
        else if(tempnode1.isreg == false && tempnode2.isreg == true)
        {
            if(eaxreg == false)
                tempnode1.code.push_back(randommake()+"pop ebx");
            else
            {
                tempnode1.code.push_back(randommake()+"mov ebx, eax");
                eaxreg = false;
            }
            if(tempnode1.isdig == true)
                tempnode1.code.push_back(randommake()+"mov eax, " + tempnode1.result);
            else
                tempnode1.code.push_back(randommake()+"mov eax, [" + tempnode1.result + "]");
            tempnode1.code.push_back(randommake()+"cmp eax, ebx");
        }
        else if(tempnode1.isreg == true && tempnode2.isreg == false)
        {
            if(eaxreg == false)
                tempnode1.code.push_back(randommake()+"pop eax");
            if(tempnode2.isdig == true)
                tempnode1.code.push_back(randommake()+"cmp eax, " + tempnode2.result);
            else
                tempnode1.code.push_back(randommake()+"cmp eax, [" + tempnode2.result + "]");
        }
        else
        {
            tempnode1.code.push_back(randommake()+"mov ebx, eax");
            tempnode1.code.push_back(randommake()+"pop eax");
            tempnode1.code.push_back(randommake()+"cmp eax, ebx");
        }
        itoa(nownum+3,buf,10);
        tempnode1.code.push_back(randommake()+"jnz L" + string(buf));
        tempnode1.code.push_back(randommake()+"mov eax, 0");
        itoa(nownum+2,buf,10);
        tempnode1.code.push_back(randommake()+"jmp L" + string(buf));
        tempnode1.code.push_back(randommake()+"mov eax, 1");
        tempnode1.result = "eax";
        eaxreg = true;
        tempnode1.isreg = true;
        tempnode1.isdig = false;
        codestack.push(tempnode1);
        break;
    case 35://logical_AND_expression => relational_expression

        break;
    case 36://logical_AND_expression => logical_AND_expression AND relational_expression
        tempnode2 = codestack.top();
        codestack.pop();
        tempnode1 = codestack.top();
        codestack.pop();
        for(int i = 0; i < tempnode2.code.size(); i++)
        {
            tempnode1.code.push_back(tempnode2.code[i]);
        }
        if(tempnode2.isreg == false && tempnode1.isreg == false)
        {
            if(eaxreg == true)
            {
                tempnode1.code.push_back(randommake()+"push eax");
                eaxreg = false;
            }
            if(tempnode1.isdig == true)
                tempnode1.code.push_back(randommake()+"mov eax, " + tempnode1.result);
            else
                tempnode1.code.push_back(randommake()+"mov eax, [" + tempnode1.result + "]");
            if(tempnode2.isdig == true)
                tempnode1.code.push_back(randommake()+"and eax, " + tempnode2.result);
            else
                tempnode1.code.push_back(randommake()+"and eax, [" + tempnode2.result + "]");
        }
        else if(tempnode1.isreg == false && tempnode2.isreg == true)
        {
            if(eaxreg == false)
                tempnode1.code.push_back(randommake()+"pop ebx");
            else
            {
                tempnode1.code.push_back(randommake()+"mov ebx, eax");
                eaxreg = false;
            }
            if(tempnode1.isdig == true)
                tempnode1.code.push_back(randommake()+"mov eax, " + tempnode1.result);
            else
                tempnode1.code.push_back(randommake()+"mov eax, [" + tempnode1.result + "]");
            tempnode1.code.push_back(randommake()+"and eax, ebx");
        }
        else if(tempnode1.isreg == true && tempnode2.isreg == false)
        {
            if(eaxreg == false)
                tempnode1.code.push_back(randommake()+"pop eax");
            if(tempnode2.isdig == true)
                tempnode1.code.push_back(randommake()+"and eax, " + tempnode2.result);
            else
                tempnode1.code.push_back(randommake()+"and eax, [" + tempnode2.result + "]");
        }
        else
        {
            tempnode1.code.push_back(randommake()+"mov ebx, eax");
            tempnode1.code.push_back(randommake()+"pop eax");
            tempnode1.code.push_back(randommake()+"and eax, ebx");
        }
        itoa(nownum+3,buf,10);
        tempnode1.code.push_back(randommake()+"jnz L" + string(buf));
        tempnode1.code.push_back(randommake()+"mov eax, 0");
        itoa(nownum+2,buf,10);
        tempnode1.code.push_back(randommake()+"jmp L" + string(buf));
        tempnode1.code.push_back(randommake()+"mov eax, 1");
        tempnode1.result = "eax";
        eaxreg = true;
        tempnode1.isreg = true;
        tempnode1.isdig = false;
        codestack.push(tempnode1);
        break;
    case 37://logical_OR_expression => logical_AND_expression

        break;
    case 38://logical_OR_expression => logical_OR_expression OR logical_AND_expression
        tempnode2 = codestack.top();
        codestack.pop();
        tempnode1 = codestack.top();
        codestack.pop();
        for(int i = 0; i < tempnode2.code.size(); i++)
        {
            tempnode1.code.push_back(tempnode2.code[i]);
        }
        if(tempnode2.isreg == false && tempnode1.isreg == false)
        {
            if(eaxreg == true)
            {
                tempnode1.code.push_back(randommake()+"push eax");
                eaxreg = false;
            }
            if(tempnode1.isdig == true)
                tempnode1.code.push_back(randommake()+"mov eax, " + tempnode1.result);
            else
                tempnode1.code.push_back(randommake()+"mov eax, [" + tempnode1.result + "]");
            if(tempnode2.isdig == true)
                tempnode1.code.push_back(randommake()+"or eax, " + tempnode2.result);
            else
                tempnode1.code.push_back(randommake()+"or eax, [" + tempnode2.result + "]");
        }
        else if(tempnode1.isreg == false && tempnode2.isreg == true)
        {
            if(eaxreg == false)
                tempnode1.code.push_back(randommake()+"pop ebx");
            else
            {
                tempnode1.code.push_back(randommake()+"mov ebx, eax");
                eaxreg = false;
            }
            if(tempnode1.isdig == true)
                tempnode1.code.push_back(randommake()+"mov eax, " + tempnode1.result);
            else
                tempnode1.code.push_back(randommake()+"mov eax, [" + tempnode1.result + "]");
            tempnode1.code.push_back(randommake()+"or eax, ebx");
        }
        else if(tempnode1.isreg == true && tempnode2.isreg == false)
        {
            if(eaxreg == false)
                tempnode1.code.push_back(randommake()+"pop eax");
            if(tempnode2.isdig == true)
                tempnode1.code.push_back(randommake()+"or eax, " + tempnode2.result);
            else
                tempnode1.code.push_back(randommake()+"or eax, [" + tempnode2.result + "]");
        }
        else
        {
            tempnode1.code.push_back(randommake()+"mov ebx, eax");
            tempnode1.code.push_back(randommake()+"pop eax");
            tempnode1.code.push_back(randommake()+"or eax, ebx");
        }
        itoa(nownum+3,buf,10);
        tempnode1.code.push_back(randommake()+"jnz L" + string(buf));
        tempnode1.code.push_back(randommake()+"mov eax, 0");
        itoa(nownum+2,buf,10);
        tempnode1.code.push_back(randommake()+"jmp L" + string(buf));
        tempnode1.code.push_back(randommake()+"mov eax, 1");
        tempnode1.result = "eax";
        eaxreg = true;
        tempnode1.isreg = true;
        tempnode1.isdig = false;
        codestack.push(tempnode1);
        break;
    case 39://expression => assignment_expression
        break;
    case 40://expression => expression COMMA assignment_expression
        tempnode1 = codestack.top();
        codestack.pop();
        tempnode2 = codestack.top();
        codestack.pop();
        for(int i = 0; i < tempnode1.code.size(); i++)
        {
            tempnode2.code.push_back(tempnode1.code[i]);
        }
        codestack.push(tempnode2);
        break;
    case 41://assignment_expression => primary_expression EQ additive_expression
        tempnode2 = codestack.top();
        codestack.pop();
        tempnode1 = codestack.top();
        codestack.pop();
        tempstr = tempnode1.result;
        for(int i = 0; i < tempnode2.code.size(); i++)
        {
            tempnode1.code.push_back(tempnode2.code[i]);
        }
        if(tempstr.find("[") == -1)
        {
            if(tempnode2.isdig == true)
            {
                tempnode1.code.push_back(randommake()+"mov [" + tempnode1.result +"], " + tempnode2.result);
            }
            else if(tempnode2.isreg == false)
            {
                tempnode1.code.push_back(randommake()+"mov eax, [" + tempnode2.result + "]");
                tempnode1.code.push_back(randommake()+"mov [" + tempnode1.result + "], eax");
            }
            else
            {
                tempnode1.code.push_back(randommake()+"mov [" + tempnode1.result + "], " + tempnode2.result);
                if(tempnode2.result == "eax")
                    eaxreg = false;
                else
                    ebxreg = false;
            }
            tempnode1.isdig = false;
            tempnode1.isreg = false;
            codestack.push(tempnode1);
        }
        else
        {
            i = tempstr.find("[");
            j = tempstr.find("]");
            tempstr2 = tempstr.substr(i+1,j-i-1);
            if(tempnode2.isdig == true)
            {
                if(tempstr2 == "eax")
                {
                    if(eaxreg == false)
                        tempnode1.code.push_back(randommake()+"pop eax");
                    else
                        eaxreg = false;
                    tempnode1.code.push_back(randommake()+"mov esi, eax");
                    tempnode1.code.push_back(randommake()+"mov [" + tempstr.substr(0,i) +"+esi*4], " + tempnode2.result);
                }
                else if(isdigit(tempstr2[0]))
                {
                    tempnode1.code.push_back(randommake()+"mov [" + tempstr.substr(0,i) + "+" + tempstr2 +"*4], " + tempnode2.result);
                }
                else
                {
                    tempnode1.code.push_back(randommake()+"mov esi, [" + tempstr2 + "]");
                    tempnode1.code.push_back(randommake()+"mov [" + tempstr.substr(0,i) +"+esi*4], " + tempnode2.result);
                }
            }
            else if(tempnode2.isreg == false)
            {
                tempnode1.code.push_back(randommake()+"mov ebx, [" + tempnode2.result + "]");
                if(tempstr2 == "eax")
                {
                    if(eaxreg == false)
                        tempnode1.code.push_back(randommake()+"pop eax");
                    else
                        eaxreg = false;
                    tempnode1.code.push_back(randommake()+"mov esi, eax");
                    tempnode1.code.push_back(randommake()+"mov [" + tempstr.substr(0,i) +"+esi*4], ebx");
                }
                else if(isdigit(tempstr2[0]))
                {
                    tempnode1.code.push_back(randommake()+"mov [" + tempstr.substr(0,i) + "+" + tempstr2 +"*4], ebx");
                }
                else
                {
                    tempnode1.code.push_back(randommake()+"mov esi, [" + tempstr2 + "]");
                    tempnode1.code.push_back(randommake()+"mov [" + tempstr.substr(0,i) +"+esi*4], ebx");
                }
            }
            else
            {
                if(tempstr2 == "eax")
                {
                    if(eaxreg == false)
                    {
                        tempnode1.code.push_back(randommake()+"pop ebx");
                        tempnode1.code.push_back(randommake()+"pop eax");
                    }
                    else
                    {
                        tempnode1.code.push_back(randommake()+"mov ebx, eax");
                        tempnode1.code.push_back(randommake()+"pop eax");
                    }
                    tempnode1.code.push_back(randommake()+"mov esi, eax");
                    tempnode1.code.push_back(randommake()+"mov [" + tempstr.substr(0,i) +"+esi*4], ebx");
                }
                else if(isdigit(tempstr2[0]))
                {
                    tempnode1.code.push_back(randommake()+"mov [" + tempstr.substr(0,i) + "+" + tempstr2 +"*4], eax");
                }
                else
                {
                    tempnode1.code.push_back(randommake()+"mov esi, [" + tempstr2 + "]");
                    tempnode1.code.push_back(randommake()+"mov [" + tempstr.substr(0,i) + "+esi*4], eax");
                }
                eaxreg = false;
            }
            tempnode1.isdig = false;
            tempnode1.isreg = false;
            codestack.push(tempnode1);
        }
        break;
    case 42://assignment_expression => logical_OR_expression

        break;
    case 43://string_literal => DQ_MARKS STR DQ_MARKS
        tempnode1.result = tempparse[1].symbol;
        tempnode1.isreg = false;
        tempnode1.isdig = false;
        codestack.push(tempnode1);
        break;
    case 44://out_statement => PRINT LR_BRAC string_literal COMMA out_paralist RR_BRAC SEMIC
        tempnode2 = codestack.top();
        codestack.pop();
        tempnode1 = codestack.top();
        codestack.pop();
        for(int i = 0; i < tempnode2.code.size(); i++)
        {
            tempnode1.code.push_back(tempnode2.code[i]);
        }
        tempstr = tempnode1.result;
        k = findeaxnum(tempnode2.result);
        eaxnum = (k-1)*4;
        i = tempstr.find("%d");
        while(i != -1)
        {
            tempstr2 = tempstr.substr(0,i+2);
            j = tempstr2.find("\\n");
            while(j != -1)
            {
                tempnode1.code.push_back(randommake()+"invoke\tprintf,chr$(\""+tempstr2.substr(0,j)+"\",0dh,0ah)");
                tempstr2 = tempstr2.substr(j+2,tempstr2.size()-j-2);
                j = tempstr2.find("\\n");
            }
            l = tempnode2.result.find(",");
            if(l == -1)
            {
                l = tempnode2.result.size();
            }
            if(tempnode2.result.substr(0,l) == "eax")
            {
                itoa(eaxnum,buf,10);
                tempnode1.code.push_back(randommake()+"mov eax, [esp+" +buf+ "]");
                tempnode1.code.push_back(randommake()+"invoke\tprintf,chr$(\""+tempstr2+"\"),eax");
                eaxnum -= 4;
            }
            else
            {
                tempnode1.code.push_back(randommake()+"invoke\tprintf,chr$(\""+tempstr2+"\")," + tempnode2.result.substr(0,l));
            }
            if(l != tempnode2.result.size())
                tempnode2.result = tempnode2.result.substr(l+1,tempnode2.result.size()-l-1);
            tempstr = tempstr.substr(i+2,tempstr.size()-i-2);
            i = tempstr.find("%d");
        }
        if(!tempstr.empty())
        {
            j = tempstr.find("\\n");
            while(j != -1)
            {
                tempnode1.code.push_back(randommake()+"invoke\tprintf,chr$(\""+tempstr.substr(0,j)+"\",0dh,0ah)");
                tempstr = tempstr.substr(j+2,tempstr.size()-j-2);
                j = tempstr.find("\\n");
            }
            if(!tempstr.empty())
                tempnode1.code.push_back(randommake()+"invoke\tprintf,chr$(\""+tempstr+"\")");
        }
        if(k != 0)
        {
            itoa(k*4,buf,10);
            tempnode1.code.push_back(randommake()+"add esp, " + buf);
        }
        codestack.push(tempnode1);
        break;
    case 45://out_statement => PRINT LR_BRAC string_literal RR_BRAC SEMIC
        tempnode1 = codestack.top();
        codestack.pop();
        tempstr = tempnode1.result;
        i = tempstr.find("\\n");
        while(i != -1)
        {
            tempnode1.code.push_back(randommake()+"invoke\tprintf,chr$(\""+tempstr.substr(0,i)+"\",0dh,0ah)");
            tempstr = tempstr.substr(i+2,tempstr.size()-i-2);
            i = tempstr.find("\\n");
        }
        if(!tempstr.empty())
            tempnode1.code.push_back(randommake()+"invoke\tprintf,chr$(\""+tempstr+"\")");
        codestack.push(tempnode1);
        break;
    case 46://out_paralist => additive_expression
        break;
    case 47://out_paralist => out_paralist COMMA additive_expression
        tempnode2 = codestack.top();
        codestack.pop();
        tempnode1 = codestack.top();
        codestack.pop();
        for(int i = 0; i < tempnode2.code.size(); i++)
        {
            tempnode1.code.push_back(tempnode2.code[i]);
        }
        if(eaxreg == true)
        {
            tempnode1.code.push_back(randommake()+"push eax");
            eaxreg = false;
        }
        tempnode1.result = tempnode1.result + "," + tempnode2.result;
        codestack.push(tempnode1);
        break;
    case 48://declaration => INT init_declarator_list SEMIC
        break;
    case 49://init_declarator => ID
        tempnode1.result = "NULL";
        tempnode1.isreg = false;
        tempnode1.isdig = false;
        codestack.push(tempnode1);
        addform(tempparse[0].symbol,0);
        offset+=4;
        break;
    case 50://init_declarator => ID EQ DIG
        tempnode1.result = "NULL";
        tempnode1.isreg = false;
        tempnode1.isdig = false;
        tempnode1.code.push_back(randommake()+"mov [" + tempparse[2].symbol + "], " +  tempparse[0].symbol);
        codestack.push(tempnode1);
        addform(tempparse[2].symbol,0);
        offset += 4;
        break;
    case 51://init_declarator => init_array_left RS_BRAC EQ array_initializer
        tempnode1 = codestack.top();
        codestack.pop();
        tempnode2 = codestack.top();
        codestack.pop();
        for(int i = 0; i < tempnode1.code.size(); i++)
        {
            tempnode2.code.push_back(tempnode1.code[i]);
        }
        codestack.push(tempnode2);
        break;
    case 52://init_array_left => ID LS_BRAC DIG
        tempnode1.result = "NULL";
        tempnode1.isreg = false;
        tempnode1.isdig = false;
        codestack.push(tempnode1);
        addform(tempparse[2].symbol,1);
        offset += 4*atoi(tempparse[0].symbol.c_str());
        break;
    case 53://init_declarator_list => init_declarator

        break;
    case 54://init_declarator_list => init_declarator_list COMMA init_declarator
        tempnode1 = codestack.top();
        codestack.pop();
        tempnode2 = codestack.top();
        codestack.pop();
        for(int i = 0; i < tempnode1.code.size(); i++)
        {
            tempnode2.code.push_back(tempnode1.code[i]);
        }
        codestack.push(tempnode2);
        break;
    case 55://array_initializer => LB_BRAC initializer_list RB_BRAC

        break;
    case 56://initializer_list => DIG
        tempnode1.result = "NULL";
        tempnode1.isreg = false;
        tempnode1.isdig = false;
        tempnode1.code.push_back(randommake()+"mov ["+form.back().name + "], " + tempparse[0].symbol);
        codestack.push(tempnode1);
        break;
    case 57://initializer_list => initializer_list COMMA DIG
        char buf[10];
        itoa(codestack.top().code.size()*4,buf,10);
        tempnode1 = codestack.top();
        tempnode1.code.push_back(randommake()+"mov ["+form.back().name + "+" + string(buf) + "], " + tempparse[0].symbol);
        codestack.pop();
        codestack.push(tempnode1);
        break;
    case 58://A =>
        tempnode1 = codestack.top();
        codestack.pop();
        if(tempnode1.isdig == true)
        {
            if(eaxreg == true)
            {
                tempnode2.code.push_back(randommake()+"push eax");
                eaxreg = false;
            }
            tempnode2.code.push_back(randommake()+"mov eax, "+tempnode1.result);
            tempnode2.code.push_back(randommake()+"cmp eax, 0");
            tempnode2.code.push_back(randommake()+"jz L");
        }
        else if(tempnode1.isreg == true)
        {
            if(eaxreg == false)
                tempnode2.code.push_back(randommake()+"pop eax");
            tempnode2.code.push_back(randommake()+"cmp eax, 0");
            tempnode2.code.push_back(randommake()+"jz L");
            tempnode1.isreg = false;
            eaxreg = false;
        }
        else
        {
            if(eaxreg == true)
            {
                tempnode2.code.push_back(randommake()+"push eax");
                eaxreg = false;
            }
            tempnode2.code.push_back(randommake()+"mov eax, ["+tempnode1.result +"]");
            tempnode2.code.push_back(randommake()+"cmp eax, 0");
            tempnode2.code.push_back(randommake()+"jz L");
        }
        codestack.push(tempnode1);
        codestack.push(tempnode2);
        break;
    case 59://B =>
        tempnode1.code.push_back(randommake()+"jmp L");
        codestack.push(tempnode1);
        break;
    case 60://C =>
        tempnode3 = codestack.top();
        codestack.pop();
        tempnode2 = codestack.top();
        codestack.pop();
        tempnode1 = codestack.top();
        codestack.pop();
        itoa(nownum,buf,10);
        tempstr = tempnode1.code[tempnode1.code.size()-1];
        tempnode1.code[tempnode1.code.size()-1] = tempstr + string(buf);
        codestack.push(tempnode1);
        codestack.push(tempnode2);
        codestack.push(tempnode3);
        tempnode4.isdig = false;
        tempnode4.isreg = false;
        tempnode4.result = "NULL";
        codestack.push(tempnode4);
        break;
    case 61://D =>
        itoa(nownum,buf,10);
        tempnode1.result = buf;
        codestack.push(tempnode1);
        break;
    case 62://E =>
        tempnode1 = codestack.top();
        codestack.pop();
        if(tempnode1.isdig == true)
        {
            if(eaxreg == true)
            {
                tempnode2.code.push_back(randommake()+"push eax");
                eaxreg = false;
            }
            tempnode2.code.push_back(randommake()+"mov eax, "+tempnode1.result);
            tempnode2.code.push_back(randommake()+"cmp eax, 0");
            tempnode2.code.push_back(randommake()+"jz L");
        }
        else if(tempnode1.isreg == true)
        {
            if(eaxreg == false)
                tempnode2.code.push_back(randommake()+"pop eax");
            tempnode2.code.push_back(randommake()+"cmp eax, 0");
            tempnode2.code.push_back(randommake()+"jz L");
            tempnode1.isreg = false;
            eaxreg = false;
        }
        else
        {
            if(eaxreg == true)
            {
                tempnode2.code.push_back(randommake()+"push eax");
                eaxreg = false;
            }
            tempnode2.code.push_back(randommake()+"mov eax, ["+tempnode1.result +"]");
            tempnode2.code.push_back(randommake()+"cmp eax, 0");
            tempnode2.code.push_back(randommake()+"jz L");
        }
        codestack.push(tempnode1);
        codestack.push(tempnode2);
        break;
    case 63://F =>
        itoa(nownum,buf,10);
        tempnode1.result = buf;
        codestack.push(tempnode1);
        break;
    case 64://G =>
        tempnode1 = codestack.top();
        codestack.pop();
        if(tempnode1.isdig == true)
        {
            if(eaxreg == true)
            {
                tempnode2.code.push_back(randommake()+"push eax");
                eaxreg = false;
            }
            tempnode2.code.push_back(randommake()+"mov eax, "+tempnode1.result);
            tempnode2.code.push_back(randommake()+"cmp eax, 0");
            tempnode2.code.push_back(randommake()+"jnz L");
            tempnode2.code.push_back(randommake()+"jz L");
        }
        else if(tempnode1.isreg == true)
        {
            if(eaxreg == false)
                tempnode2.code.push_back(randommake()+"pop eax");
            tempnode2.code.push_back(randommake()+"cmp eax, 0");
            tempnode2.code.push_back(randommake()+"jnz L");
            tempnode2.code.push_back(randommake()+"jz L");
            tempnode1.isreg = false;
            eaxreg = false;
        }
        else
        {
            if(eaxreg == true)
            {
                tempnode2.code.push_back(randommake()+"push eax");
                eaxreg = false;
            }
            tempnode2.code.push_back(randommake()+"mov eax, ["+tempnode1.result +"]");
            tempnode2.code.push_back(randommake()+"cmp eax, 0");
            tempnode2.code.push_back(randommake()+"jnz L");
            tempnode2.code.push_back(randommake()+"jz L");
        }
        itoa(nownum,buf,10);
        tempnode2.result = buf;
        codestack.push(tempnode1);
        codestack.push(tempnode2);
        break;
    case 65://H =>
        tempnode4 = codestack.top();
        codestack.pop();
        tempnode3 = codestack.top();
        codestack.pop();
        tempnode2 = codestack.top();
        codestack.pop();
        tempnode1 = codestack.top();
        tempnode5.code.push_back(randommake()+"jmp L" + tempnode1.result);
        itoa(nownum,buf,10);
        tempstr = tempnode3.code[tempnode3.code.size()-2];
        tempnode3.code[tempnode3.code.size()-2] = tempstr + buf;
        codestack.push(tempnode2);
        codestack.push(tempnode3);
        codestack.push(tempnode4);
        codestack.push(tempnode5);
        break;
    }

}
