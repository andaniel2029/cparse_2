#include<iostream>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include <vector>
#include <regex>

using namespace std;

int no_t;
int no_nt;
char terminals[100]={};
char non_terminals[100]={};
char goto_table[100][100];
char reduce[20][20];
char follow[20][20];
char fo_co[20][20];
char first[20][20];

struct state
{
    int prod_count;
    char prod[100][100];
};

void add_dots(state *I){
    for(int i=0;i<I->prod_count;i++){
        for (int j=99;j>3;j--)
            I->prod[i][j] = I->prod[i][j-1];
        I->prod[i][3]='@';
    }
}

void augument(state *S,state *I){
    //if(I->prod[0][0]=='\'')
    //    strcpy(S->prod[0],"Z->@\'");
    //else{
        strcpy(S->prod[0],"\'->@");
        S->prod[0][4]=I->prod[0][0];
    //}
    S->prod_count++;
}

bool isExistElement(vector<char>& vArr, char ch)
{
	int count = (int)vArr.size();
	for (int i = 0; i < count; i++)
	{
		if (vArr[i] == ch)
			return true;
	}

	return false;
}

string replaceString(string subject, const string& search, const string& replace) 
{
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != string::npos)
	{
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}

	return subject;
}

void getProdValues(vector<string>& vProd, vector<char>& vnt, vector<char>& vt)
{
	string strContent = "";
	int count = (int)vProd.size();
	for (int i = 0; i < count; i++)
	{
		string strLine = vProd.at(i);		
		char ch = strLine.at(0);
		if (!isExistElement(vnt, ch))
			vnt.push_back(ch);

		strContent += strLine;
	}

	// Remove Letters
	strContent = replaceString(strContent, "->", "");

	for (int i = 0; i < vnt.size(); i++)
	{
		char sz[4];
		memset(sz, 0, sizeof(sz));
		sz[0] = vnt.at(i);
		strContent = replaceString(strContent, sz, "");        
	}

    strContent = replaceString(strContent, "\r", "");

	for (int i = 0; i < strContent.length(); i++)
	{
		char ch = strContent.at(i);
		if (!isExistElement(vt, ch))
			vt.push_back(ch);
	}
}

bool get_prods(state *I, char * filepath)
{
	FILE * fp = fopen(filepath, "r");
	if (fp == NULL)
		return false;

	char line[128];
	bool bArguS = false;
	bool bArguT = false;

	vector<string> vProd;

	while (true)
	{
		memset(line, 0, sizeof(line));
		if (fgets(line, sizeof(line), fp) == NULL)
			break;

		string strLine = line;

		size_t found = strLine.find("\n");
		if (found != -1)
			strLine = strLine.substr(0, found);

		found = strLine.find("\'");
		if (found != -1)
			continue;

		if (strLine.empty())
			continue;

		found = strLine.find("Augmented Grammar");
		if (found != -1)
		{
			bArguS = true;
			continue;
		}

		found = strLine.find("Follows");
		if (found != -1)
		{
			bArguT = true;
			break;
		}

		if (bArguS)
		{
			found = strLine.find("->");
			if (found != -1)
            {
                strLine = replaceString(strLine, "\r", "");
                vProd.push_back(strLine);
            }
		}
	}

	fclose(fp);

	size_t size = vProd.size();
	if (size == 0)
		return false;

	vector<char> vnt;
	vector<char> vt;
	getProdValues(vProd, vnt, vt);

	I->prod_count = (int)vProd.size();
	for (int i = 0; i < I->prod_count; i ++)
		strcpy(I->prod[i], vProd[i].c_str());

	no_nt = (int)vnt.size();
	for (int i = 0; i < no_nt; i++)
		non_terminals[i] = vnt[i];

	no_t = (int)vt.size();
	for (int i = 0; i < no_t; i++)
		terminals[i] = vt[i];

	return true;
}

bool is_non_terminal(char a){
   if (a >= 'A' && a <= 'Z')
        return true;
    else
        return false;
}

bool in_state(state *I,char *a){
    for(int i=0;i<I->prod_count;i++){
        if(!strcmp(I->prod[i],a))
            return true;
    }
    return false;
}

char char_after_dot(char a[100])
{
    char b;
	for (int i = 0; i < strlen(a); i++)
	{
		if (a[i] == '@')
		{
			b = a[i + 1];
			return b;
		}
	}

	return '\0';
}

char* move_dot(char b[100],int len){
	static char a[100];
	memset(a, 0, sizeof(a));

    strcpy(a,b);
    for(int i=0;i<len;i++){
        if(a[i]=='@'){
            swap(a[i],a[i+1]);
            break;
        }
    }
    return &a[0];
}

bool same_state(state *I0,state *I){

    if (I0->prod_count != I->prod_count)
        return false;

    for (int i=0; i<I0->prod_count; i++)
    {
        int flag = 0;
        for (int j=0; j<I->prod_count; j++)
            if (strcmp(I0->prod[i], I->prod[j]) == 0)
                flag = 1;
        if (flag == 0)
            return false;
    }
    return true;

}

void closure(state *I,state *I0){
    char a={};
    for(int i=0;i<I0->prod_count;i++){
        a=char_after_dot(I0->prod[i]);
        if(is_non_terminal(a)){
            for(int j=0;j<I->prod_count;j++){
                if(I->prod[j][0]==a){
                    if(!in_state(I0,I->prod[j])){
                        strcpy(I0->prod[I0->prod_count],I->prod[j]);
                        I0->prod_count++;
                    }
                }
            }
        }
    }
}

void goto_state(state *I,state *S,char a){
    int time=1;
    for(int i=0;i<I->prod_count;i++){
        if(char_after_dot(I->prod[i])==a){
                if(time==1){
                    time++;
                }
                strcpy(S->prod[S->prod_count], move_dot(I->prod[i], (int)strlen(I->prod[i])));
                S->prod_count++;
        }
    }
}

void print_prods(state *I){
    for(int i=0;i<I->prod_count;i++)
        printf("%s\n",I->prod[i]);
    cout << endl;
}

bool in_array(char a[20],char b){
    for(int i=0;i<strlen(a);i++)
        if(a[i]==b)
            return true;
    return false;
}

char* chars_after_dots(state *I){
    static char a[20]={};
    for(int i=0;i<I->prod_count;i++){
        if(!in_array(a,char_after_dot(I->prod[i]))){
                a[strlen(a)]=char_after_dot(I->prod[i]);
            }
    }
    return &a[0];
}

void cleanup_prods(state * I){
    char a[100]={};
    for(int i=0;i<I->prod_count;i++)
        strcpy(I->prod[i],a);
    I->prod_count=0;
}

int return_index(char a)
{
    for(int i=0;i<no_t;i++)
        if(terminals[i]==a)
            return i;

	for(int i=0;i<no_nt;i++)
        if(non_terminals[i]==a)
            return no_t+i;

	return 0;
}

int get_index(char c,char *a)
{
	for (int i = 0; i < strlen(a); i++)
	{
		if (a[i] == c)
			return i;
	}

	return 0;
}

void add_dot_at_end(state* I){
    for(int i=0;i<I->prod_count;i++){
        strcat(I->prod[i],"@");
    }
}

void add_to_first(int n,char b){
   for(int i=0;i<strlen(first[n]);i++)
        if(first[n][i]==b)
            return;
    first[n][strlen(first[n])]=b;
}

void add_to_first(int m,int n){
    for(int i=0;i<strlen(first[n]);i++){
            int flag=0;
        for(int j=0;j<strlen(first[m]);j++){
            if(first[n][i]==first[m][j])
                flag=1;
        }
        if(flag==0)
            add_to_first(m,first[n][i]);
    }
}

void add_to_follow(int n,char b){
   for(int i=0;i<strlen(follow[n]);i++)
        if(follow[n][i]==b)
            return;
    follow[n][strlen(follow[n])]=b;
}

void add_to_follow(int m,int n){
    for(int i=0;i<strlen(follow[n]);i++){
            int flag=0;
        for(int j=0;j<strlen(follow[m]);j++){
            if(follow[n][i]==follow[m][j])
                flag=1;
        }
        if(flag==0)
            add_to_follow(m,follow[n][i]);
    }
}

void add_to_follow_first(int m,int n){
    for(int i=0;i<strlen(first[n]);i++){
            int flag=0;
        for(int j=0;j<strlen(follow[m]);j++){
            if(first[n][i]==follow[m][j])
                flag=1;
        }
        if(flag==0)
            add_to_follow(m,first[n][i]);
    }
}

void find_first(state *I){
    for(int i=0;i<no_nt;i++){
        for(int j=0;j<I->prod_count;j++){
            if(I->prod[j][0]==non_terminals[i]){
                if(!is_non_terminal(I->prod[j][3])){
                    add_to_first(i,I->prod[j][3]);
                    }

            }
        }
    }
}

void find_follow(state *I){
    for(int i=0;i<no_nt;i++){
        for(int j=0;j<I->prod_count;j++){
            for(int k=3;k<strlen(I->prod[j]);k++){
                if(I->prod[j][k]==non_terminals[i]){
                    if(I->prod[j][k+1]!='\0'){
                        if(!is_non_terminal(I->prod[j][k+1])){
                            add_to_follow(i,I->prod[j][k+1]);
                        }
                    }
                }
            }
        }
    }
}

int get_index(int *arr,int n){
    for(int i=0;i<no_t;i++){
        if(arr[i]==n)
            return i;
    }
    return -1;
}

void make_shift_table(int state_count, char **& action, int **& action_num, int **& goto_num)
{
	for (int i = 0; i < state_count; i++) 
	{
		int arr[100] = { -1 };
		for (int j = 0; j < state_count; j++) {
			if (goto_table[i][j] != '~') {
				arr[return_index(goto_table[i][j])] = j;
			}
		}

		for (int j = 0; j < no_t; j++) 
		{
			action_num[i][j] = (arr[j] == -1) ? 0 : arr[j];

			if (i == 1 && j == no_t - 1)
			{
				action[i][j] = 'a';
				continue;
			}

			if (arr[j] == -1 || arr[j] == 0)
				action[i][j] = 'e';
			else
				action[i][j] = 's';
		}
	}

	for (int i = 0; i < state_count; i++)
	{
		int arr[100] = { -1 };
		for (int j = 0; j < state_count; j++) {
			if (goto_table[i][j] != '~') {
				arr[return_index(goto_table[i][j])] = j;
			}
		}

		for (int j = 0; j < no_nt; j++)
		{
			if (arr[j + no_t] == -1 || arr[j + no_t] == 0)
				goto_num[i][j] = 0;
			else
				goto_num[i][j] = arr[j + no_t];
		}
	}
}

void make_reduce_table(int state_count,int *no_re,state *temp1, char **& action, int **& action_num)
{
	int arr[100][100] = { -1 };

	for(int i=0;i<temp1->prod_count;i++)
	{
		int n = no_re[i];
		for (int j = 0; j < strlen(follow[return_index(temp1->prod[i][0]) - no_t]); j++) {
			for (int k = 0; k < no_t; k++) {
				if (follow[return_index(temp1->prod[i][0]) - no_t][j] == terminals[k])
					arr[i][k] = i + 1;
			}
		}

		for (int j = 0; j < no_t; j++) 
		{
			if (arr[i][j] != -1 && arr[i][j] != 0 && arr[i][j] < state_count)
			{
				action_num[n][j] = arr[i][j];
				action[n][j] = 'r';
			}
		}
	}
}


bool write_table_header(int state_count, int no_nt, int no_t, int prod_count, char **& action, int **& action_num, int **& goto_num)
{
	FILE * fp = fopen("table.h", "wb");
	if (fp == NULL)
		return false;

	char header[] = "#ifndef TABLE_HEADER\r\n#define TABLE_HEADER\r\n\r\n";
	fwrite(header, strlen(header), 1, fp);

	char szLine[128];

	memset(szLine, 0, sizeof(szLine));
        sprintf(szLine, "#define  NUM_STATES\t%d\r\n", state_count);
	fwrite(szLine, strlen(szLine), 1, fp);

        sprintf(szLine, "#define  NUM_TERMS\t%d\r\n", no_t);
	fwrite(szLine, strlen(szLine), 1, fp);

	sprintf(szLine, "#define  NUM_NONTERMS\t%d\r\n", no_nt);
	fwrite(szLine, strlen(szLine), 1, fp);

        sprintf(szLine, "#define  NUM_PRODS\t%d\r\n\r\n", prod_count);
	fwrite(szLine, strlen(szLine), 1, fp);

	sprintf(szLine, "static char action[NUM_STATES][NUM_TERMS] = {\r\n\t/*\t+\t *\t  (\t   )\ti\t $\t*/\r\n");
	fwrite(szLine, strlen(szLine), 1, fp);

	int i = 0;
	bool bWrote = false;

	for (i = 0; i < state_count; i++)
	{
		memset(szLine, 0, sizeof(szLine));

		for (int j = 0; j < no_t; j++)
		{
			if (j == 0)
			{
				sprintf(szLine, "\t{  \'%c\', ", action[i][j]);
				fwrite(szLine, strlen(szLine), 1, fp);
			}
			else if (j == no_t - 1)
			{
				sprintf(szLine, "\'%c\'  }, /* %d */", action[i][j], i);
				fwrite(szLine, strlen(szLine), 1, fp);
			}
			else
			{
				sprintf(szLine, "\'%c\', ", action[i][j]);
				fwrite(szLine, strlen(szLine), 1, fp);
			}
		}

		sprintf(szLine, "\r\n");
		fwrite(szLine, strlen(szLine), 1, fp);
		bWrote = true;
	}

	if (bWrote)
	{
		sprintf(szLine, "};\r\n\r\n");
		fwrite(szLine, strlen(szLine), 1, fp);
		bWrote = false;
	}

	sprintf(szLine, "static int action_num[NUM_STATES][NUM_TERMS] = {\r\n\t/*  +   *   (   )   i   $\t*/\r\n");
	fwrite(szLine, strlen(szLine), 1, fp);
	
	for (i = 0; i < state_count; i++)
	{
		memset(szLine, 0, sizeof(szLine));

		for (int j = 0; j < no_t; j++)
		{
			if (j == 0)
			{
				sprintf(szLine, "\t{  %2d, ", action_num[i][j]);
				fwrite(szLine, strlen(szLine), 1, fp);
			}
			else if (j == no_t - 1)
			{
				sprintf(szLine, "%2d  },\t/* %2d */", action_num[i][j], i);
				fwrite(szLine, strlen(szLine), 1, fp);
			}
			else
			{
				sprintf(szLine, "%2d, ", action_num[i][j]);
				fwrite(szLine, strlen(szLine), 1, fp);
			}
		}

		sprintf(szLine, "\r\n");
		fwrite(szLine, strlen(szLine), 1, fp);
		bWrote = true;
	}

	if (bWrote)
	{
		sprintf(szLine, "};\r\n\r\n");
		fwrite(szLine, strlen(szLine), 1, fp);
		bWrote = false;
	}

	sprintf(szLine, "static int go_to[NUM_STATES][NUM_NONTERMS] = {\r\n");
	fwrite(szLine, strlen(szLine), 1, fp);

	for (i = 0; i < state_count; i++)
	{
		memset(szLine, 0, sizeof(szLine));

		for (int j = 0; j < no_nt; j++)
		{
			if (j == 0)
			{
				sprintf(szLine, "\t{  %2d, ", goto_num[i][j]);
				fwrite(szLine, strlen(szLine), 1, fp);
			}
			else if (j == no_nt - 1)
			{
				sprintf(szLine, "%2d  },\t/* %2d */", goto_num[i][j], i);
				fwrite(szLine, strlen(szLine), 1, fp);
			}
			else
			{
				sprintf(szLine, "%2d, ", goto_num[i][j]);
				fwrite(szLine, strlen(szLine), 1, fp);
			}
		}

		sprintf(szLine, "\r\n");
		fwrite(szLine, strlen(szLine), 1, fp);
		bWrote = true;
	}

	if (bWrote)
	{
		sprintf(szLine, "};\r\n\r\n");
		fwrite(szLine, strlen(szLine), 1, fp);
		bWrote = false;
	}

	sprintf(szLine, "static char reduce_num[NUM_PRODS] = \r\n");
	fwrite(szLine, strlen(szLine), 1, fp);

	for (i = 0; i < prod_count; i++)
	{
		if (i == 0)
		{
			sprintf(szLine, "\t/*%2d ", i + 1);
			fwrite(szLine, strlen(szLine), 1, fp);
		}
		else if (i == prod_count - 1)
		{
			sprintf(szLine, "%2d */", i + 1);
			fwrite(szLine, strlen(szLine), 1, fp);
		}
		else
		{
			sprintf(szLine, "%2d ", i + 1);
			fwrite(szLine, strlen(szLine), 1, fp);
		}
	}

	sprintf(szLine, "\r\n\t{  1, 3, 1, 3, 1, 3, 1  };\r\n");
	fwrite(szLine, strlen(szLine), 1, fp);
	
	sprintf(szLine, "\r\n\r\nstatic char reduce_lhs[NUM_PRODS] = \r\n");
	fwrite(szLine, strlen(szLine), 1, fp);

	for (i = 0; i < prod_count; i++)
	{
		if (i == 0)
		{
			sprintf(szLine, "\t/*%2d ", i + 1);
			fwrite(szLine, strlen(szLine), 1, fp);
		}
		else if (i == prod_count - 1)
		{
			sprintf(szLine, "%2d */", i + 1);
			fwrite(szLine, strlen(szLine), 1, fp);
		}
		else
		{
			sprintf(szLine, "%2d ", i + 1);
			fwrite(szLine, strlen(szLine), 1, fp);
		}
	}

	sprintf(szLine, "\r\n\t{  0, 0, 0, 1, 1, 2, 2  };\r\n");
	fwrite(szLine, strlen(szLine), 1, fp);

	sprintf(szLine, "\r\n\r\nstatic char tokens[NUM_TERMS] = \r\n");
	fwrite(szLine, strlen(szLine), 1, fp);

	for (i = 0; i < no_t; i++)
	{
		if (i == 0)
		{
			sprintf(szLine, "\t/* %2d   ", i + 1);
			fwrite(szLine, strlen(szLine), 1, fp);
		}
		else if (i == no_t - 1)
		{
			sprintf(szLine, "%2d */", i + 1);
			fwrite(szLine, strlen(szLine), 1, fp);
		}
		else
		{
			sprintf(szLine, "%2d   ", i + 1);
			fwrite(szLine, strlen(szLine), 1, fp);
		}
	}

	//sprintf(szLine, "\r\n\t{  '+', '*', '(', ')', 'i', '$'  };\r\n");
	//fwrite(szLine, strlen(szLine), 1, fp);	
	
	for (i = 0; i < strlen(terminals); i++)
	{
		if (i == 0)
		{
			sprintf(szLine, "\r\n\t{  \'%c\', ", terminals[i]);
			fwrite(szLine, strlen(szLine), 1, fp);
		}
		else if (i == strlen(terminals) - 1)
		{
			sprintf(szLine, "\'%c\'  };\r\n", terminals[i]);
			fwrite(szLine, strlen(szLine), 1, fp);
		}
		else
		{
			sprintf(szLine, "\'%c\', ", terminals[i]);
			fwrite(szLine, strlen(szLine), 1, fp);
		}
	} 

	char footer[] = "\r\n\r\n#endif //TABLE_HEADER\r\n";
	fwrite(footer, strlen(footer), 1, fp);

	fclose(fp);

	return true;
}

int main(int argc, char **argv)
{
    if (argc < 2)
	{
		cout << "[Usage] gentable GrammarFilePath\n";
		return 0;
    }

    state init;
    if (!get_prods(&init, argv[1]))
	{
		cout << "Failed to read file <" << argv[1] << ">\n";
		return 0;
	}

    state temp;
	state temp1;
    int state_count=1;
    temp=init;
    temp1=temp;
    add_dots(&init);

    for(int i=0;i<100;i++)
        for(int j=0;j<100;j++)
            goto_table[i][j]='~';

    state I[50];
	for (int i = 0; i < 50; i++)
		memset(&I[i], 0, sizeof(state));

    augument(&I[0],&init);
    closure(&init,&I[0]);    

    char characters[20]={};
    for(int i=0;i<state_count;i++)
	{
        char characters[20]={};
		for (int z = 0; z < I[i].prod_count; z++)
		{
			if (!in_array(characters, char_after_dot(I[i].prod[z])))
				characters[strlen(characters)] = char_after_dot(I[i].prod[z]);
		}

        for(int j=0;j<strlen(characters);j++){
            goto_state(&I[i],&I[state_count],characters[j]);
            closure(&init,&I[state_count]);
            int flag=0;
            for(int k=0;k<state_count-1;k++){
                if(same_state(&I[k],&I[state_count])){
                    cleanup_prods(&I[state_count]);flag=1;
                    goto_table[i][k]=characters[j];;
                    break;
                }
            }
            if(flag==0){
                state_count++;
                goto_table[i][state_count-1]=characters[j];
            }
        }
    }

    int no_re[50]={-1};
    terminals[no_t]='$';no_t++;

    add_dot_at_end(&temp1);
    for(int i=0;i<state_count;i++){
        for(int j=0;j<I[i].prod_count;j++)
            for(int k=0;k<temp1.prod_count;k++)
                 if(in_state(&I[i],temp1.prod[k]))
                        no_re[k]=i;
    }

    find_first(&temp);
    for(int l=0;l<no_nt;l++){
    for(int i=0;i<temp.prod_count;i++){
        if(is_non_terminal(temp.prod[i][3])){
            add_to_first(return_index(temp.prod[i][0])-no_t,return_index(temp.prod[i][3])-no_t);
        }
    }}

    find_follow(&temp);
    add_to_follow(0,'$');
    for(int l=0;l<no_nt;l++)
	{
        for(int i=0;i<temp.prod_count;i++)
		{
            for(int k=3;k<strlen(temp.prod[i]);k++)
			{
                if(temp.prod[i][k]==non_terminals[l])
				{
                    if(is_non_terminal(temp.prod[i][k+1]))
                        add_to_follow_first(l,return_index(temp.prod[i][k+1])-no_t);

                    if(temp.prod[i][k+1]=='\0')
                        add_to_follow(l,return_index(temp.prod[i][0])-no_t);
                }
            }
        }
    }

	int NUM_STATES = state_count;
	int NUM_TERMS = no_t;
	int NUM_NONTERMS = no_nt;
	int NUM_PRODS = I->prod_count;

	char **action = NULL;
	action = new char *[NUM_STATES];
	
	int i, j;
	for (i = 0; i < NUM_STATES; i++)
	{
		action[i] = new char[NUM_TERMS];
		for (j = 0; j < NUM_TERMS; j++)
			action[i][j] = '\0';
	}

	int ** action_num = NULL;
	action_num = new int *[NUM_STATES];
	for (i = 0; i < NUM_STATES; i++)
	{
		action_num[i] = new int[NUM_TERMS];
		for (j = 0; j < NUM_TERMS; j++)
			action_num[i][j] = 0;
	}

	int ** goto_num = NULL;
	goto_num = new int *[NUM_STATES];
	for (i = 0; i < NUM_STATES; i++)
	{
		goto_num[i] = new int[NUM_NONTERMS];
		for (j = 0; j < NUM_NONTERMS; j++)
			goto_num[i][j] = 0;
	}

    make_shift_table(state_count, action, action_num, goto_num);
	make_reduce_table(state_count, &no_re[0], &temp1, action, action_num);

	if (write_table_header(NUM_STATES, NUM_NONTERMS, NUM_TERMS, NUM_PRODS, action, action_num, goto_num))
		cout << "Save Done!\n\n";
	else
		cout << "Save Failed.\n\n";

	for (i = 0; i < NUM_STATES; i++)
	{
		delete[] action[i];
		delete[] action_num[i];
		delete[] goto_num[i];
	}

	delete[] action;
	delete[] action_num;
	delete[] goto_num;
}







