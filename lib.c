#include <stdlib.h> 
#include <stdio.h>

char num2char(int i){
	char c;
	if (i > 9 ||  i < 0)
	{
		i = -1;
	}

	c = 48+i;
	return c;
}

int strLen(char* str){
	if (str == NULL)
	{
		return 0;
	}
	int i;
	i=0;
	while (str[i] != '\0')
	{
		++i;
	}

	return i;
}

int arrayLen(int* str){
	int i;
	i=0;

	if (str == NULL)
	{
		return 0;
	}
	
	while (str[i] != '\0')
	{
		++i;
	}

	return i;
}

int intLength(int num){
	int i;
	num = (num < 0) ? -num : num;

	for (i = 0; num > 0; ++i)
	{
		num = num / 10;
	}

	return i;
}

int lastDigit(int num){
	int d;
	double tmp;

	d = num / 10;
	
	tmp = ((double)num) / 10;
	tmp -= d;

	d = tmp *10;
	
	return d;
}

char* int2str(int num, int length){
	short inNeg;
	int i;
	int j;

	length = (length <= 0) ? intLength(num) : length;

	char* str = malloc(length);

	for (i = 0, j = length-1; i <= length; ++i, j--)
	{
		if (i == length-1)
		{
			str[j] = '\0';
		}
		if (num > 0)
		{
			str[j] = num2char(lastDigit(num));
		}
		else{
			str[j] = '0';
		}

		num = num /10;
	}

	return str;
}

char* strConcat(char* str1, char* str2){
	int i;
	int l;
	int l1;
	int l2;

	l1 = strLen(str1);
	l2 = strLen(str2);
	l = l1 + l2;

	char* str3 = malloc(l);

	for (i = 0; i < l; ++i)
	{
		if (i < l1)
		{
			str3[i] = str1[i];
		}
		else if(i < l1 + l2){
			str3[i] = str2[i-l1];
		}
		else{
			str3[i] = '\0';
		}
	}

	return str3;
}

int strComp(char* str1, char* str2){
	int i;
	int l1;
	int l2;

	l1 = strLen(str1);
	l2 = strLen(str2);

	if (l1 != l2)
	{
		return 0;
	}

	for (i = 0; i < l1; ++i)
	{
		if (str1[i]!=str2[i])
		{
			return 0;
		}
	}

	return 1;
}

int inArray(int val, int* arr){
	int i;
	int len;

	len = arrayLen(arr);
	for (i = 0; i < len; ++i)
	{
		if (arr[i] == val)
		{
			return 1;
		}
	}

	return 0;
}

void print(char* str){
	write(1, str, strLen(str));
}

int strCopy(char* str, char* str2){
	int i;
	int len;
	int len2;

	len = strLen(str);
	len2 = strLen(str2);
	if (len2 < len)
	{
		return 0;
	}

	for (i = 0; str[i] != '\0' && i < len; ++i)
	{
		str2[i] = str[i];
	}

	str2[i+1] = '\0';

	return 1;
}