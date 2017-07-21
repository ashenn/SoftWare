#ifndef __BASIC_C__
#define __BASIC_C__

int isNum(char c);
int isAlpha(char c);
int strIsNum(char* str);
int char2int(char ch);
int str2int(char* str);
int strIsAlpha(char* str);
int alnum(char* str);
void substr(char* str, int offset, int length, char** target);

#endif