#include<stdlib.h>
#include<stdio.h>
#include<string.h>
void main(int argc, char** argv){
	if(argc != 2)
		fprintf(stderr, "Error: The program expect one command-line argument which is the length of randomly generated string\n");
	int n = atoi(argv[1]);
	srand(time(NULL));
	long l = 0;
	while(l++ < n)
		putchar('a'+rand()%26);
}
