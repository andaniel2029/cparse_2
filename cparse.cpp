#include <iostream>
#include <string>
#include <stack>
#include <queue>
#include <map>
#include <fstream>
#include <vector>
#include <regex>
#include "table.h"

using namespace std;


#define  FALSE	0
#define  TRUE	1

struct Handle
{
	unsigned int reduced;
	unsigned int length;
};

queue< pair<string, string> > q;

char Next(FILE* fp);
int  Word(char c);
int  Num(char c);
int  Symbol(char c);
int  print_reject_output();
int  print_accept_output(FILE* infp);

int  check_keyword = FALSE;
int  word_ing = FALSE;
int  num_ing = FALSE;
int  line_count = 1;

char symbol_type[20];
char whatis_now;
char check_word[20] = "";
char check_num[20] = "";
char int_keyword[] = "int";
char if_keyword[] = "if";
char then_keyword[] = "then";
char else_keyword[] = "else";
char while_keyword[] = "while";
char return_keyword[] = "return";
char eq_keyword[] = "==";
char buff[1];

stack<int> stacks;

int		ParsingTable[100][100];
Handle	GotoTable[7];


int print_accept_output(char * infp)
{
	if (word_ing == TRUE) {
		if ((strcmp(check_word, int_keyword)) == 0) {
			q.push({ int_keyword,"keyword" });
		}
		else if ((strcmp(check_word, if_keyword)) == 0) {
			q.push({ if_keyword,"keyword" });
		}
		else if ((strcmp(check_word, then_keyword)) == 0) {
			q.push({ then_keyword,"keyword" });
		}
		else if ((strcmp(check_word, else_keyword)) == 0) {
			q.push({ else_keyword,"keyword" });
		}
		else if ((strcmp(check_word, while_keyword)) == 0) {
			q.push({ while_keyword,"keyword" });
		}
		else if ((strcmp(check_word, return_keyword)) == 0) {
			q.push({ return_keyword,"keyword" });
		}
		else if ((strcmp(check_word, eq_keyword)) == 0) {
			q.push({ eq_keyword,"keyword" });
		}
		else {
			q.push({ check_word,"WORD" });
		}
	}

	else if (num_ing == TRUE) {
		q.push({ check_num,"NUM" });
	}

	if (Symbol(whatis_now) == TRUE) {
		if (whatis_now == '=') {
			infp++;
			if (whatis_now == '=') {
				strcpy(symbol_type, "EQ");
				q.push({ "==","keyword" });
			}
			else {
				whatis_now = '=';
				strcpy(symbol_type, "EQUAL");
				q.push({ "=","keyword" });
			}
		}
		if (whatis_now != '=') {
			buff[0] = whatis_now;
			q.push({ buff,"symbol_type" });
		}
	}

	strcpy(symbol_type, "");
	strcpy(check_num, "");
	strcpy(check_word, "");
	num_ing = FALSE;
	word_ing = FALSE;

	if (whatis_now == 32)
		return 0;
}

int scanner(string instring)
{
	int count = 0;
	int length = instring.length();

	char * pBuf = new char[length + 1];
	memset(pBuf, 0, length + 1);
	memcpy(pBuf, instring.c_str(), length);

	while (TRUE)
	{
		whatis_now = *pBuf;
		if (whatis_now <= 0)
		{
			if (word_ing)
				print_accept_output(pBuf);

			break;
		}

		if (whatis_now == 10 || whatis_now == 9) {
			line_count++;
			pBuf++;
			continue;
		}

		if (Word(whatis_now) == TRUE) {
			if (num_ing == TRUE)
				print_reject_output();

			check_word[count++] = whatis_now;
			word_ing = TRUE;
			pBuf++;
			continue;
		}
		else if (Num(whatis_now) == TRUE) {
			if (word_ing == TRUE)
				print_reject_output();

			check_num[count++] = whatis_now;
			num_ing = TRUE;
			pBuf++;
			continue;
		}
		else if (Symbol(whatis_now) == TRUE || whatis_now == 32) {
			if (word_ing == TRUE) {
				check_word[count] = '\0';
				print_accept_output(pBuf);
			}
			else if (num_ing == TRUE) {
				check_num[count] = '\0';
				print_accept_output(pBuf);
			}

			else
				print_accept_output(pBuf);

			word_ing = num_ing = FALSE;
			count = 0;
			memset(check_num, 0, sizeof(check_num));
			memset(symbol_type, 0, sizeof(symbol_type));
			memset(check_word, 0, sizeof(check_word));
			pBuf++;
		}
		else
		{
			print_reject_output();
			delete[] pBuf;
			exit(1);
		}
	}

	return 0;
}

int scanner(char* file) {
	int count = 0;
	FILE* input;
	char filename[50] = "";

	strcpy(filename, file);

	if ((input = fopen(filename, "r")) == NULL) {
		perror("input file open error!!\n");
	}

	while (TRUE)
	{
		bool bRet = Next(input);
		if (whatis_now <= 0)
		{
			if (word_ing)
				print_accept_output(input);

			break;
		}

		if (whatis_now == 10 || whatis_now == 9) {
			line_count++;
			continue;
		}

		if (Word(whatis_now) == TRUE) {
			if (num_ing == TRUE)
				print_reject_output();

			check_word[count++] = whatis_now;
			word_ing = TRUE;
			continue;
		}
		else if (Num(whatis_now) == TRUE) {
			if (word_ing == TRUE)
				print_reject_output();

			check_num[count++] = whatis_now;
			num_ing = TRUE;

			continue;
		}

		else if (Symbol(whatis_now) == TRUE || whatis_now == 32) {
			if (word_ing == TRUE) {
				check_word[count] = '\0';
				print_accept_output(input);
			}
			else if (num_ing == TRUE) {
				check_num[count] = '\0';
				print_accept_output(input);
			}

			else
				print_accept_output(input);

			word_ing = num_ing = FALSE;
			count = 0;
			memset(check_num, 0, sizeof(check_num));
			memset(symbol_type, 0, sizeof(symbol_type));
			memset(check_word, 0, sizeof(check_word));
		}
		else {
			print_reject_output();
			exit(1);
		}
	}

	fclose(input);
	return 0;
}

char Next(FILE * fp) {
	whatis_now = fgetc(fp);
	if (!feof(fp))
		return whatis_now;
	else
		return TRUE;
}

int Word(char c) {
	if (c >= 'a' && c <= 'z')
		return TRUE;
	else if (c >= 'A' && c <= 'Z')
		return TRUE;
	else
		return FALSE;
}

int Num(char c) {
	if (c >= '0' && c <= '9')
		return TRUE;
	else
		return FALSE;
}

int Symbol(char c) {
	switch (c) {
	case '{':
		strcpy(symbol_type, "BLOCK");
		return TRUE;
		break;
	case '}':
		strcpy(symbol_type, "BLOCK");
		return TRUE;
		break;
	case '(':
		strcpy(symbol_type, "PAREN");
		return TRUE;
		break;
	case ')':
		strcpy(symbol_type, "PAREN");
		return TRUE;
		break;
	case '>':
		strcpy(symbol_type, "GREATER_THAN");
		return TRUE;
		break;
	case '=':
		strcpy(symbol_type, "EQUAL");
		return TRUE;
		break;
	case '+':
		strcpy(symbol_type, "PLUS");
		return TRUE;
		break;
	case '*':
		strcpy(symbol_type, "MUL");
		return TRUE;
		break;
	case '-':
		strcpy(symbol_type, "SUB");
		return TRUE;
		break;
	case '/':
		strcpy(symbol_type, "DIV");
		return TRUE;
		break;
	case ';':
		strcpy(symbol_type, "SEMI");
		return TRUE;
		break;
	default:
		return FALSE;
		break;
	}
}


int print_reject_output()
{
	printf("Reject!! Line number : %d\n", line_count);
	exit(1);
}

int print_accept_output(FILE * infp) {
	if (word_ing == TRUE) {
		if ((strcmp(check_word, int_keyword)) == 0) {
			q.push({ int_keyword,"keyword" });
		}
		else if ((strcmp(check_word, if_keyword)) == 0) {
			q.push({ if_keyword,"keyword" });
		}
		else if ((strcmp(check_word, then_keyword)) == 0) {
			q.push({ then_keyword,"keyword" });
		}
		else if ((strcmp(check_word, else_keyword)) == 0) {
			q.push({ else_keyword,"keyword" });
		}
		else if ((strcmp(check_word, while_keyword)) == 0) {
			q.push({ while_keyword,"keyword" });
		}
		else if ((strcmp(check_word, return_keyword)) == 0) {
			q.push({ return_keyword,"keyword" });
		}
		else if ((strcmp(check_word, eq_keyword)) == 0) {
			q.push({ eq_keyword,"keyword" });
		}
		else {
			q.push({ check_word,"WORD" });
		}
	}

	else if (num_ing == TRUE) {
		q.push({ check_num,"NUM" });
	}

	if (Symbol(whatis_now) == TRUE) {
		if (whatis_now == '=') {
			Next(infp);
			if (whatis_now == '=') {
				strcpy(symbol_type, "EQ");
				q.push({ "==","keyword" });
			}
			else {
				whatis_now = '=';
				strcpy(symbol_type, "EQUAL");
				q.push({ "=","keyword" });
			}
		}
		if (whatis_now != '=') {
			buff[0] = whatis_now;
			q.push({ buff,"symbol_type" });
		}
	}

	strcpy(symbol_type, "");
	strcpy(check_num, "");
	strcpy(check_word, "");
	num_ing = FALSE;
	word_ing = FALSE;

	if (whatis_now == 32)	return 0;
}

// cparse
int tok_to_num(string token, string attribute)
{
	if (!attribute.compare("WORD"))return 4;
	if (!token.compare("=")) { return 0; }
	if (!token.compare("+"))return 0;
	if (!token.compare("*"))return 1;
	if (!token.compare("("))return 2;
	if (!token.compare(")"))return 3;
	if (!token.compare("i"))return 4;
	if (!token.compare("$"))return 5;
	if (!token.compare("&"))return 5;
}

void Shift()
{
	int i = stacks.top();
	auto temp = q.front();
	q.pop();

	stacks.push(tok_to_num(temp.first, temp.second));
	stacks.push(ParsingTable[i][tok_to_num(temp.first, temp.second)] - 100);

	queue< pair<string, string> > q_tem;
	stack<int> s_tem;
	s_tem = stacks;
	q_tem = q;

	for (int i = 0; i < stacks.size(); i++)
		s_tem.pop();

	for (int i = 0; i < q.size(); i++)
		q_tem.pop();
}

void Reduce()
{
	int i, j, go_to, cnt;
	int temp_node = stacks.top();
	stacks.pop();

	auto temp = q.front();

	i = ParsingTable[temp_node][tok_to_num(temp.first, temp.second)];

	cnt = GotoTable[i - 1000].length * 2 - 1;

	for (j = 0; j < cnt; j++)
		stacks.pop();

	go_to = ParsingTable[stacks.top()][GotoTable[i - 1000].reduced];
	cout << ">>REDUCE #" << i - 1000 << "\n";

	queue< pair<string, string> > q_tem;
	stack<int> s_tem;
	s_tem = stacks;
	q_tem = q;

	for (int i = 0; i < stacks.size(); i++)
		s_tem.pop();

	for (int i = 0; i < q.size(); i++)
		q_tem.pop();

	stacks.push(GotoTable[i - 1000].reduced);
	stacks.push(go_to);

	queue< pair<string, string> > q_tem2;
	stack<int> s_tem2;
	s_tem2 = stacks;
	q_tem2 = q;

	for (int i = 0; i < stacks.size(); i++)
		s_tem2.pop();

	for (int i = 0; i < q.size(); i++)
		q_tem2.pop();
}

int Act()
{
	stack<int> s_tem;
	s_tem = stacks;

	int i = stacks.top();
	auto temp = q.front();
	int k = tok_to_num(temp.first, temp.second);
	int j = ParsingTable[i][k];

	if (j == 0)
	{
		cout << "REJECT\n";
		return 2;
	}
	else if (j > 99 && j < 1000)
	{
		Shift();
		return 1;
	}
	else if (j < 10000)
	{
		Reduce();
		return 1;
	}
	else if (j == 10000)
	{
		cout << "Accept state reached\n";
		return 3;
	}
	else if (j < 0)
	{
		cout << "Error state on token ’&’ at state #.";
		return 0;
	}
	else
	{
		cout << "Stack overflow occurred.";
		return 0;
	}

	return 0;
}

void load_table()
{
	// e: 0, s: 100, r: 1000, a: 10000

	int i, j;
	for (i = 0; i < NUM_STATES; i++)
		memset(ParsingTable[i], 0, sizeof(ParsingTable[i]));

	// action & action_num
	for (i = 0; i < NUM_STATES; i++)
	{
		for (j = 0; j < NUM_TERMS + NUM_TERMS; j++)
		{
			if (j < NUM_TERMS)
			{
				if (action[i][j] == 'a')
					ParsingTable[i][j] = 10000;
				else if (action[i][j] == 'r')
					ParsingTable[i][j] = 1000;
				else if (action[i][j] == 's')
					ParsingTable[i][j] = 100;
				else
					ParsingTable[i][j] = 0;

				ParsingTable[i][j] += action_num[i][j];
			}
			else
			{
				ParsingTable[i][j] = go_to[i][j - NUM_TERMS];
			}
		}
	}

	for (i = 0; i < NUM_PRODS; i++)
	{
		GotoTable[i].reduced = reduce_lhs[i] + NUM_TERMS;
		GotoTable[i].length = reduce_num[i];
	}
}

int main(int argc, char **argv)
{
	string strIn;
	cout << "Please type following input to cparse program :	i * i + i\n>>";	
	getline(cin, strIn);

	load_table();

	int stat;
	char parsing[] = "D:\\sentence.txt";
	scanner(strIn);

	q.push({ "&", "&" });
	stat = 1;
	stacks.push(0);

	while (stat == 1)
		stat = Act();

	return 0;
}

