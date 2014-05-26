#ifndef ConstDefine_H_INCLUDED
#define ConstDefine_H_INCLUDED

using namespace std;

#define STATENUM 133
#define NONENDNUM 33
#define ENDNUM 35//有个#

#define MAIN 0		//main
#define INT 1
#define IF 2
#define ELSE 3
#define WHILE 4
#define FOR 5
#define PLUS 6		//+
#define MINUS 7		//-
#define MULTI 8		//*
#define DIV 9		// /
#define EQ 10		//=
#define LT 11		//<
#define RT 12		//>
#define LQ 13		//<=
#define RQ 14		//>=
#define EQEQ 15		//==
#define LR_BRAC 16	//(
#define RR_BRAC 17	//)
#define LS_BRAC 18	//[
#define RS_BRAC 19	//]
#define LB_BRAC 20	//{
#define RB_BRAC 21	//}
#define PLUSPLUS 22	//++
#define MINUSMINUS 23	//--
#define SEMIC 24    //;
#define COMMA 25    //,
#define DQ_MARKS 26  //"
#define NEQ 27		//!=
#define PRINT  28   //printf
#define DIG 29		//数字
#define ID 30		//标识符
#define STR 31		//字符串
#define AND	32		//&&
#define OR 33		//||
#define RETURN 34
#define ERROR 100


#define start 51
#define statement 52
#define while_statement 53
#define if_statement 54
#define for_statement 55
#define compound_statement 56
#define block_item_list 57
#define primary_expression 58
#define postfix_expression 59
#define multiplication_expression 60
#define additive_expression 61
#define relational_expression 62
#define expression 63
#define assignment_expression 64
#define string_literal 65
#define declaration 66
#define init_declarator 67
#define init_declarator_list 68
#define array_initializer 69
#define initializer_list 70
#define logical_AND_expression 71
#define logical_OR_expression 72
#define out_paralist 73
#define out_statement 74
#define init_array_left 75
#define A 76
#define B 77
#define C 78
#define D 79
#define E 80
#define F 81
#define G 82
#define H 83

/*
0:	_start => start
1:	start => MAIN LR_BRAC RR_BRAC compound_statement
2:	statement => compound_statement
3:	statement => expression SEMIC
4:	statement => if_statement
5:	statement => while_statement
6:	statement => for_statement
7:	statement => declaration
8:	statement => out_statement
9:	while_statement => WHILE LR_BRAC expression RR_BRAC statement
10:	for_statement => FOR LR_BRAC expression SEMIC expression SEMIC expression RR_BRAC statement
11:	if_statement => IF LR_BRAC expression RR_BRAC  statement
12:	if_statement => IF LR_BRAC expression RR_BRAC  statement ELSE statement
13:	compound_statement => LB_BRAC block_item_list RB_BRAC
14:	block_item_list => statement
15:	block_item_list => block_item_list statement
16:	primary_expression => ID
17:	primary_expression => ID LS_BRAC expression RS_BRAC
18:	postfix_expression => primary_expression
19:	postfix_expression => LR_BRAC expression RR_BRAC
20:	postfix_expression => DIG
21:	postfix_expression => primary_expression PLUSPLUS
22:	postfix_expression => primary_expression MINUSMINUS
23:	multiplication_expression => postfix_expression
24:	multiplication_expression => multiplication_expression MULTI postfix_expression
25:	multiplication_expression => multiplication_expression DIV postfix_expression
26:	additive_expression => multiplication_expression
27:	additive_expression =>	additive_expression PLUS multiplication_expression
28:	additive_expression =>	additive_expression MINUS multiplication_expression
29:	relational_expression => additive_expression
30:	relational_expression => relational_expression LT additive_expression
31:	relational_expression => relational_expression RT additive_expression
32:	relational_expression => relational_expression LQ additive_expression
33:	relational_expression => relational_expression RQ additive_expression
34:	relational_expression => relational_expression EQEQ additive_expression
35:	relational_expression => relational_expression NEQ additive_expression
36:	logical_AND_expression => relational_expression
37:	logical_AND_expression => logical_AND_expression AND relational_expression
38:	logical_OR_expression => logical_AND_expression
39:	logical_OR_expression => logical_OR_expression OR logical_AND_expression
40:	expression => assignment_expression
41:	expression => expression COMMA assignment_expression
42:	assignment_expression => primary_expression EQ assignment_expression
43:	assignment_expression => logical_OR_expression
44:	string_literal => DQ_MARKS STR DQ_MARKS
45:	out_statement => PRINT LR_BRAC string_literal out_paralist RR_BRAC
46:	out_statement => PRINT LR_BRAC string_literal RR_BRAC
47:	out_paralist => COMMA additive_expression
48:	out_paralist => out_paralist COMMA additive_expression
49:	declaration => INT init_declarator_list SEMIC
50:	init_declarator => ID
51:	init_declarator => ID EQ DIG
52: init_declarator => ID LS_BRAC DIG RS_BRAC EQ array_initializer
53:	init_declarator_list => init_declarator
54:	init_declarator_list => init_declarator_list COMMA init_declarator
55:	array_initializer => LB_BRAC initializer_list RB_BRAC
56:	initializer_list => DIG
57:	initializer_list => initializer_list COMMA DIG
*/


/*
0:	start'
1:	start
2:	statement
3:	while_statement
4:	if_statement
5:	for_statement
6:	compound_statement
7:	block_item_list
8:	primary_expression
9;	postfix_expression
10:	multiplication_expression
11:	additive_expression
12:	relational_expression
13:	expression
14:	assignment_expression
15:	string_literal
16:	declaration
17:	init_declarator
18:	init_declarator_list
19:	array_initializer
20:	initializer_list
21:	logical_AND_expression
22:	logical_OR_expression
23:	out_paralist
24:	out_statement
*/


#endif
