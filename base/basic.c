#include "basic.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int isNum(char c){
	return (c >= 48 && c <= 57);
}

int isAlpha(char c){
	return (c >= 65 && c <= 90) || (c >= 97 && c <= 122);
}

int strIsNum(char* str){
	int i;
	char c;
	for (i=0; str[i] != '\0'; ++i)
	{
		if (!isNum(str[i]))
		{
			return 0;
		}
	}

	return 1;
}

int char2int(char ch){
	int c = (int) ch;

	if (c >= 48 && c <= 57)
	{
		return c -48;
	}

	return -1;
}

int str2int(char* str){
	int result = 0;
	int multiplier = 1;
	int tmp = 0;

	int i;
	for (i = 0; str[i] != '\0'; ++i)
	{
		if (str[i] == '-')
		{
			multiplier = -1;
			continue;
		}

		tmp = char2int(str[i]);
		if (tmp == -1)
		{
			return 0;
		}
		
		result = (result * 10) + tmp;
	}

	return result * multiplier;
}

int strIsAlpha(char* str){
	int i;
	char c;
	for (i=0; str[i] != '\0'; ++i)
	{
		if (!isAlpha(str[i]))
		{
			return 0;
		}
	}

	return 1;
}

int alnum(char* str){
	int i;
	char c;
	for (i=0; str[i] != '\0'; ++i)
	{
		c = str[i];
		if (!isAlpha(c) && !isNum(c) && c == 32){
			return 0;
		}
	}

	return 1;
}

void substr(char* str, int offset, int length, char** target){
	int i;
	int z=0;
	for (i = offset; (z < length || !length) && str[i] != '\0'; ++i, ++z){
		(*target)[z] = str[i];
	}
	
	(*target)[z] = '\0';
}

char* concat(char* str1, char* str2, char* res){
	int len1 = strlen(str1);
	int len2 = strlen(str2);
	int max = len1 + len2;

	int i;
	int y;
	char c;
	for (i = y = 0; i < max; y=++i)
	{
		if (i > len1){
			y = i - len1;
			c = str2[y];
		}
		else{
			c = str1[y];
		}

		res[i] = c;
	}

	res[i] = '\0';
	return res;
}


int charCnt(char c, char* str, int limit){
	int i;
	int cnt;
	for (i=cnt=0; str[i] != '\0' && cnt < limit; ++i) {
		if (str[i] == c)
		{
			cnt ++;
		}
	}

	return cnt;
}

int strpos(char c, char* str, int offset){
	int i;
	for (i = offset; str[i] != '\0'; ++i){
		if (str[i] == c){
			return i;
		}
	}

	return -1;
}

void explode(char c, char* str, int offset, int limit, char* res[]){
	int cnt = charCnt(c, str, limit);
	if (!cnt){
		res[0] = str;
		res[1] = '\0';
		return;
	}

	int i=0;
	int pos0 = 0;
	int pos1 = 0;

	if (str[0] == c){
		i=1;
		pos0=1;
		res[0] = "";
	}


	for (i = i; i < cnt && (pos1 = strpos(c, str, pos0)) > -1; ++i){
		res[i] = malloc(((pos1 - (pos0)) * sizeof(char)) +1);
		substr(str, pos0, pos1 - pos0, &res[i]);
		pos0 = pos1 +1;
	}

	pos1 = strlen(str) - pos0;
	res[i] = malloc((pos1 +1) * sizeof(char) +1);

	substr(str, pos0, 0, &res[i]);
	res[i+1] = '\0';
}