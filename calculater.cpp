// calculater.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <stdio.h>
#include <stdlib.h>
#include <stack>
#define MAX 500

using namespace std;

typedef struct token {
	int TokenType;
	char value[32];
}Token;

typedef struct var {
	int type = -1;
	char name[32] = {};
	int int_val = 'a';
	float float_val = 'a';
}Var;

int namecmp(char*, char*);
int opcmp(Token*, int);
int GetInput(char*, char*);
int Scaner(char*, int, Token*);
int Reader(Token*, int, Var*, int*);
int calculator(char*, int, int, Token*, Var*, int);
void calc(Var*, int*, int, Token*, int);
void Output(Token*, Var*, int, int, int*);


int main(int argc, char* argv[]) {
	char input[MAX] = {};
	Token TokenList[MAX/2] = {};
	int CMDList[MAX/4] = {};
	Var VarList[20] = {};//变量少于20个
	
	int InputLength = GetInput(input, argv[1]);//将输入存入数组，并得到具体的输入长度
	int TokenLength = Scaner(input, InputLength, TokenList);//将输入分割成单词，并得到单词数目
	int VarNum = Reader(TokenList, TokenLength, VarList, CMDList);//分出句子，并生成变量列表
	calc(VarList, CMDList, VarNum, TokenList, TokenLength);//计算
	Output(TokenList, VarList, TokenLength, VarNum, CMDList);//输出
	
	//debug
	/*printf("%d\n", TokenLength);
	for (int i = 0; i < TokenLength; i++) {
		printf(TokenList[i].value);
		printf("\n%d\n", TokenList[i].TokenType);
	}*/

	/*printf("%d\n", VarNum);
	for (int i = 0; i < VarNum; i++) {
		printf(VarList[i].name);
		printf("%d\n", VarList[i].type);
	}

	for (int i = 0; CMDList[i] != 'a'; i++) {
		printf("%d\n", CMDList[i]);
	}*/

	return 0;
}

int namecmp(char* a, char* b) {
	for (int i = 0; i < 32; i++) {
		if (a[i] == b[i])
			continue;
		else
			return 0;
	}

	return 1;
}

int opcmp(Token* TokenList, int op) {
	int pr = 0;
	switch (TokenList[op].TokenType) {
	case 1: pr = 4; break;
	case 2: pr = 4; break;
	case 3: pr = 5; break;
	case 4: pr = 5; break;
	}
	return pr;
}

//接收输入，将输入存入数组
int GetInput(char* input, char* file) {
	int i = 0;
	bool flag = 0;
	FILE* fp = NULL;
	errno_t err;
	char c;

	err = fopen_s(&fp, file, "r");
	if (!err && fp)
	{
		c = fgetc(fp);
		while (i < MAX && feof(fp) == 0) {
			if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == ' ' ||
				c == ';' || c == '+' || c == '-' || c == '*' || c == '/' || c == '=' || c == '(' || c == ')') {
				input[i] = c;
				i++;
				flag = 0;
			}
			else if (c == '.') {
				input[i] = c;
				i++;
				flag = 1;
			}
			c = fgetc(fp);
		}
		input[i - 1] = ';';
	}
	else
		perror("打开文件失败");

	fclose(fp);
	return i;
}

//词法分析，生成TokenList
int Scaner(char* input, int length, Token* TokenList) {
	enum TokenType {
		PLUS = 1, MINUS = 2, MULTI = 3, DIV = 4, LPAREN = 5, RPAREN = 6, INT = 7, FLOAT = 8, ARG = 9, CMD = 10, EAQLE = 11, END = 12
	};
	int j = 0;
	bool minus = 0;

	for (int i = 0; i < length; i++) {
		switch (input[i]) {
			case '+':
				TokenList[j].TokenType = PLUS;
				TokenList[j].value[0] = '+';
				j++;
				break;
			case '-':
				if (TokenList[j - 1].TokenType == 11 || TokenList[j - 1].TokenType == 5 || TokenList[j - 1].TokenType == 1 || TokenList[j - 1].TokenType == 2 || TokenList[j - 1].TokenType == 3 || TokenList[j - 1].TokenType == 4) {
					minus = 1;
				}
				else {
					TokenList[j].TokenType = MINUS;
					TokenList[j].value[0] = '-';
					j++;
				}
				break;
			case '*':
				TokenList[j].TokenType = MULTI;
				TokenList[j].value[0] = '*';
				j++;
				break;
			case '/':
				TokenList[j].TokenType = DIV;
				TokenList[j].value[0] = '/';
				j++;
				break;
			case '(':
				TokenList[j].TokenType = LPAREN;
				TokenList[j].value[0] = '(';
				j++;
				break;
			case ')':
				TokenList[j].TokenType = RPAREN;
				TokenList[j].value[0] = ')';
				j++;
				break;
			case '=':
				TokenList[j].TokenType = EAQLE;
				TokenList[j].value[0] = '=';
				j++;
				break;
			case ';':
				TokenList[j].TokenType = END;
				TokenList[j].value[0] = ';';
				j++;
				break;
		}
	
		if ((input[i] >= 'a' && input[i] <= 'z') || (input[i] >= 'A' && input[i] <= 'Z')) {
			int k = 0;
			char temp[32] = {};//变量名不超过32字节

			while ((input[i + 1] >= 'a' && input[i + 1] <= 'z') || (input[i + 1] >= 'A' && input[i + 1] <= 'Z') || (input[i + 1] >= '0' && input[i + 1] <= '9')) {
				temp[k] = input[i];
				i++;
				k++;
			}
			temp[k] = input[i];

			if (temp[0] == 'i' && temp[1] == 'n' && temp[2] == 't' ||
				temp[0] == 'f' && temp[1] == 'l' && temp[2] == 'o' && temp[3] == 'a' && temp[4] == 't' ||
				temp[0] == 'w' && temp[1] == 'r' && temp[2] == 'i' && temp[3] == 't' && temp[4] == 'e') {
				TokenList[j].TokenType = CMD;
				for (int t = 0; temp[t] != '\0'; t++) {
					TokenList[j].value[t] = temp[t];
				}
				j++;
			}
			else {
				TokenList[j].TokenType = ARG;
				for (int t = 0; temp[t] != '\0'; t++) {
					TokenList[j].value[t] = temp[t];
				}
				j++;
			}
		}

		else if (input[i] >= '0' && input[i] <= '9') {
			int k = 0;
			bool flag = 0;
			char temp[32] = {};//数字不超过32字节

			while ((input[i + 1] >= '0' && input[i + 1] <= '9') || input[i + 1] == '.') {
				if (input[i + 1] == '.')
					flag = 1;
				temp[k] = input[i];
				i++;
				k++;
			}
			temp[k] = input[i];

			if (flag) {
				TokenList[j].TokenType = FLOAT;
				if (minus) {
					for (int t = 0; temp[t] != '\0'; t++) {
						TokenList[j].value[t + 1] = temp[t];
						TokenList[j].value[0] = '-';
					}
					minus = 0;
				}
				else {
					for (int t = 0; temp[t] != '\0'; t++) {
						TokenList[j].value[t] = temp[t];
					}
				}
				j++;
			}
			else {
				TokenList[j].TokenType = INT;
				if (minus) {
					for (int t = 0; temp[t] != '\0'; t++) {
						TokenList[j].value[t + 1] = temp[t];
						TokenList[j].value[0] = '-';
					}
					minus = 0;
				}
				else {
					for (int t = 0; temp[t] != '\0'; t++) {
						TokenList[j].value[t] = temp[t];
					}
				}
				j++;
			}
		}
	}

	return j;
}

//语法分析,分析成语句并创建变量表
int Reader(Token* TokenList, int length, Var* VarList, int* CMDList) {
	int j = 0, k = 0;
	int flag = 0;
	for (int i = 0; i < length; i++) {
		if (TokenList[i].TokenType == 9)//arg
		{
			if (flag == 1) {
				for (int t = 0; TokenList[i].value[t] != '\0'; t++) {
					VarList[j].name[t] = TokenList[i].value[t];
				}
				VarList[j].type = 7;
				flag = 0;
				j++;
			}
			else if (flag == 2) {
				for (int t = 0; TokenList[i].value[t] != '\0'; t++) {
					VarList[j].name[t] = TokenList[i].value[t];
				}
				VarList[j].type = 8;
				flag = 0;
				j++;
			}

			if (TokenList[i - 1].TokenType == 12) {
				CMDList[k] = i;
				k++;
			}
		}
		else if (TokenList[i].TokenType == 10)//cmd
		{
			if (TokenList[i].value[0] == 'i' && TokenList[i].value[1] == 'n' && TokenList[i].value[2] == 't') {
				//syntax error
				if (TokenList[i + 1].TokenType != 9 || TokenList[i + 2].TokenType != 12) {
					printf("syntax error(Line %d)", i + 1);
					exit(-1);
				}
				flag = 1;
			}
			else if (TokenList[i].value[0] == 'f' && TokenList[i].value[1] == 'l' && TokenList[i].value[2] == 'o' && TokenList[i].value[3] == 'a' && TokenList[i].value[4] == 't') {
				//syntax error
				if (TokenList[i + 1].TokenType != 9 || TokenList[i + 2].TokenType != 12) {
					printf("syntax error(Line %d)", i + 1);
					exit(-1);
				}
				flag = 2;
			}
	
			CMDList[k] = i;
			k++;
		}
	}

	CMDList[k] = 'a';

	return j;
}

//计算只有数字和运算符的中缀表达式
int calculator(char* var, int start, int end, Token* TokenList, Var* VarList, int VarNum) {
	stack<int> st1, st3;
	stack<float> st2;
	int temp[100] = {};
	int j = 0;

	//syntax error
	stack<int>st;
	for (int i = start; i <= end; i++) {
		if (TokenList[i].TokenType == 5) {
			if (TokenList[i + 1].TokenType != 7 && TokenList[i + 1].TokenType != 8 && TokenList[i + 1].TokenType != 5) {
				return 1;
			}
			st.push(i);
		}
		else if (TokenList[i].TokenType == 6) {
			if (TokenList[i - 1].TokenType != 7 && TokenList[i - 1].TokenType != 8 && TokenList[i - 1].TokenType != 6) {
				return 1;
			}

			if (!st.empty()) {
				st.pop();
			}
		}
		else {
			if (i == end && TokenList[i].TokenType != 7 && TokenList[i].TokenType != 8) {
				return 1;
			}
			else if (i != end &&
				((TokenList[i].TokenType == 7 || TokenList[i].TokenType == 8) && (TokenList[i + 1].TokenType == 7 || TokenList[i + 1].TokenType == 8)) &&
				((TokenList[i].TokenType == 1 || TokenList[i].TokenType == 2 || TokenList[i].TokenType == 3 || TokenList[i].TokenType == 4) && (TokenList[i + 1].TokenType == 1 || TokenList[i + 1].TokenType == 2 || TokenList[i + 1].TokenType == 3 || TokenList[i + 1].TokenType == 4))) {
				return 1;
			}
		}
	}
	if (!st.empty()) {
		return 1;
	}

	for (int i = start; i <= end; i++)//中缀转后缀
	{
		if (TokenList[i].TokenType == 7 || TokenList[i].TokenType == 8) {
			st3.push(i);
		}
		else if (TokenList[i].TokenType == 1 || TokenList[i].TokenType == 2 || TokenList[i].TokenType == 3 || TokenList[i].TokenType == 4) {
			while (true) {
				if (st1.empty() || TokenList[st1.top()].TokenType == 5) {
					st1.push(i);
					break;
				}
				else if (opcmp(TokenList, i) > opcmp(TokenList, st1.top())) {
					st1.push(i);
					break;
				}
				else {
					int cc = st1.top();
					st1.pop();
					st3.push(cc);
				}
			}
		}
		else {
			if (TokenList[i].TokenType == 5) {
				st1.push(i);
			}
			else {
				while (TokenList[st1.top()].TokenType != 5) {
					int ccc = st1.top();
					st1.pop();
					st3.push(ccc);
				}
				st1.pop();
			}
		}
	}
	while (!st1.empty()) {
		int cccc = st1.top();
		st3.push(cccc);
		st1.pop();
	}

	while (!st3.empty()) {
		int c = st3.top();
		st1.push(c);
		st3.pop();
	}
	while (!st1.empty()) {
		temp[j] = st1.top();
		st1.pop();
		j++;
	}

	temp[j] = 'a';
	
	for (int k = 0; temp[k] != 'a'; k++)//计算后缀
	{
		if (TokenList[temp[k]].TokenType == 7) {
			int buf;
			if (sscanf_s(TokenList[temp[k]].value, "%d", &buf))
				st2.push(buf);
		}
		else if (TokenList[temp[k]].TokenType == 8) {
			float buf;
			if (sscanf_s(TokenList[temp[k]].value, "%f", &buf))
				st2.push(buf);
		}
		else {
			float op1, op2, val;

			op1 = st2.top();
			st2.pop();
			op2 = st2.top();
			st2.pop();

			switch (TokenList[temp[k]].TokenType) {
			case 1:
				val = op1 + op2;
				break;
			case 2:
				val = op2 - op1;
				break;
			case 3:
				val = op1 * op2;
				break;
			case 4:
				val = op2 / op1;
				break;
			}
			st2.push(val);
		}
	}

	for (int t = 0; t < VarNum; t++) {
		if (namecmp(var, VarList[t].name)) {
			if (VarList[t].type == 7)
				VarList[t].int_val = (int)st2.top();
			else
				VarList[t].float_val = st2.top();
		}
	}

	return 0;
}

//计算,先将语句中的变量换成数字，然后通过后缀表达式计算出结果
void calc(Var* VarList, int* CMDList, int VarNum, Token* TokenList, int TokenLength) {
	for (int i = 0; CMDList[i] != 'a'; i++) {
		if (TokenList[CMDList[i]].TokenType == 9) {
			//syntax error
			if (TokenList[CMDList[i] + 1].TokenType != 11) {
				printf("syntax error(Line %d)", i + 1);
				exit(-1);
			}

			bool flag = 0;
			for (int j = CMDList[i]; j < CMDList[i + 1] && j < TokenLength; j++)//先变换成数字
			{
				if (TokenList[j].TokenType == 9) {
					//undefined identifier
					bool undefine = 1;
					for (int b = 0; b < VarNum; b++) {
						if (namecmp(TokenList[j].value, VarList[b].name)) {
							undefine = 0;
						}
					}
					if (undefine) {
						printf("undefined identifier(Line %d)\n", i + 1);
						exit(-1);
					}

					if (flag) {
						//uninitialized identifier
						bool unini = 0;
						for (int t = 0; t < VarNum; t++) {
							if (namecmp(TokenList[j].value, VarList[t].name)) {
								if (VarList[t].float_val == 'a' && VarList[t].int_val == 'a') {
									unini = 1;
								}
								//divided by zero
								else if (VarList[t].float_val == 0 && VarList[t].int_val == 0) {
									if (TokenList[j - 1].TokenType == 4) {
										printf("divided by zero(Line %d)\n", i + 1);
										exit(-1);
									}
								}

								if (VarList[t].type == 7) {
									sprintf_s(TokenList[j].value, "%d", VarList[t].int_val);
								}
								else {
									_gcvt_s(TokenList[j].value, 32, VarList[t].float_val, 30);
								}
								TokenList[j].TokenType = VarList[t].type;
							}
						}
						if (unini) {
							printf("uninitialized identifier(Line %d)\n", i + 1);
							exit(-1);
						}
					}
				}

				else if (TokenList[j].TokenType == 11) {
					if (flag) {
						printf("syntax error(Line %d)\n", i + 1);
						exit(-1);
					}
					flag = 1;
				}

				//divided by zero
				else if (TokenList[j].TokenType == 4) {
					if (TokenList[j + 1].TokenType == 7) {
						int buf;
						if (sscanf_s(TokenList[j + 1].value, "%d", &buf)) {
							if (buf == 0) {
								printf("divided by zero(Line %d)\n", i + 1);
								exit(-1);
							}
						}
					}
					else if (TokenList[j + 1].TokenType == 8) {
						float buf;
						if (sscanf_s(TokenList[j + 1].value, "%f", &buf)) {
							if (buf == 0) {
								printf("divided by zero(Line %d)\n", i + 1);
								exit(-1);
							}
						}
					}
				}
			}

			int end = 0;
			if (CMDList[i + 1] == 'a')
				end = TokenLength - 2;
			else
				end = CMDList[i + 1] - 2;

			if (calculator(TokenList[CMDList[i]].value, CMDList[i] + 2, end, TokenList, VarList, VarNum)) {
				printf("syntax error(Line %d)", i + 1);
				exit(-1);
			}
		}
	}
}

//输出函数
void Output(Token* TokenList, Var* VarList, int TokenLength, int VarNum, int* CMDList){
	for (int i = 0; CMDList[i] != 'a'; i++) {
		if (TokenList[CMDList[i]].TokenType == 10 &&
			TokenList[CMDList[i]].value[0] == 'w' && TokenList[CMDList[i]].value[1] == 'r' && TokenList[CMDList[i]].value[2] == 'i' && TokenList[CMDList[i]].value[3] == 't' && TokenList[CMDList[i]].value[4] == 'e') {
			if (TokenList[CMDList[i] + 1].TokenType == 5 && TokenList[CMDList[i] + 2].TokenType == 9 && TokenList[CMDList[i] + 3].TokenType == 6) {
				bool undefine = 1;
				for (int j = 0; j < VarNum; j++) {
					if (namecmp(TokenList[CMDList[i] + 2].value, VarList[j].name)) {
						undefine = 0;
						//uninitialized identifier
						if (VarList[j].int_val == 'a' && VarList[j].float_val == 'a') {
							printf("uninitialized identifier %s(Line %d)\n", VarList[j].name, i + 1);
							exit(-1);
						}

						if (VarList[j].type == 7) {
							printf("%s = %d\n", VarList[j].name, VarList[j].int_val);
						}
						else {
							printf("%s = %f\n", VarList[j].name, VarList[j].float_val);
						}
					}
				}
				//undefined identifier
				if (undefine) {
					printf("undefined identifier(Line %d)\n", i + 1);
					exit(-1);
				}
			}
			//syntax error
			else {
				printf("syntax error(Line %d)", i + 1);
				exit(-1);
			}
		}
	}
}