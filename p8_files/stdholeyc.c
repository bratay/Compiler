#include "stdio.h"
#include "stdlib.h"

void printBool(char c){
	if (c == 0){ 
		fprintf(stdout, "false"); 
	} else{ 
		fprintf(stdout, "true"); 
	}
	fflush(stdout);
}

void printChar(char c){
	fprintf(stdout, "%c", c);
	fflush(stdout);
}

void printInt(long int num){
	fprintf(stdout, "%ld", num);
	fflush(stdout);
}

void printString(const char * str){
	fprintf(stdout, "%s", str);
	fflush(stdout);
}

int8_t getBool(){
	char c;
	scanf("%c", &c);
	getchar(); // Consume trailing newline
	if (c == '0'){
		return 0;
	} else {
		return 1;
	}
}

long int getInt(){
	char buffer[32];
	fgets(buffer, 32, stdin);
	long int res = atol(buffer);
	return res;
}

char getChar(){
	char c;
	c = getchar();
	if (c != '\n' && c != 0x10){
		char next = getchar(); // Consume trailing newline
		if (next != '\n' && next != 0x10){
			ungetc(next, stdin);
		}
	} else {
		//user didn't enter anything before newline
	}
	return c;
}
