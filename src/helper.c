#include "../header/config.h"
#ifndef _H_HELPER
#include "../header/helper.h"
#include "../header/main.h"
#endif
#include<dirent.h>
#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>


struct hash *hashTable = NULL;
unsigned long eleCount = 0;
unsigned long contDigests1 = 0;
unsigned long contDigests2 = 0;
long long contForID=0;
long long contForNgram=0;
int log_open = 1;
unsigned int total_ids = 0;

void removeItensDIR(char *dirFiles) {
    DIR           *d;
    struct dirent *dir;
    
    d = opendir(dirFiles);
    if (d) {
		while ((dir = readdir(d)) != NULL) {
			char fullpath[120];
			strcpy(fullpath, dirFiles);
			strcat(fullpath, dir->d_name);
			remove(fullpath);
		}

        closedir(d);
    }
}
char* FindFileName(char *digest,int lookup){
	unsigned char *token;

	if(lookup){
		token =	strstr(digest,"t5/");
		token =	strtok(token,"/"); 
		return strtok(NULL,"\"");
	}

	else{
		token =	strstr(digest,"t5/");
		token =	strtok(token,"/"); 
		return strtok(NULL,"\"");
	}
}
void  createLogFile(char *filename){

	arqLog = fopen(filename, "w");
	if(!arqLog){

		fprintf(stderr, "create log operation failed %s\n", filename);
		perror(arqLog);
		return;
	}
	else		
	//	fprintf(arqLog, "LOG file opened!\n\n");
		
	fclose(arqLog);
	
	return;
}

int openLogFile(char *filename) {

	arqLog = fopen(filename, "a");
			
	if(!arqLog){

		printf("open log operation failed ????%s\n", filename);
		perror(arqLog);
		return 0;
	}
	
	return 1;
}


void writeLog(char *message, int newline, char *filename) {

	
	if (openLogFile(filename) == 1){
		if(!arqLog){
			fprintf(stderr, "Write log operation failed %s\n", filename);
			return;
		}
		else {
			
			if(newline == 0)
				fprintf(arqLog, "%s ", message);
			else
				fprintf(arqLog, "%s\n", message);
		}
			
		
	}
	fclose(arqLog);

	return;	
}

void writeIDlog(long long int id, int newline, char *filename){
	
	

	if (openLogFile(filename) == 1){
		if(!arqLog){
			fprintf(stderr, "write ID log operation failed %s\n", filename);
			return;
		}
		else {
		
			
			if(newline == 0)
				fprintf(arqLog, "%lld ", id);
			else
				fprintf(arqLog, "%lld\n", id);
		}
			
		fclose(arqLog);
	}
	return;	
}

void strreverse(char* begin, char* end) {
	
	char aux;
	
	while(end>begin)
	
		aux=*end, *end--=*begin, *begin++=aux;
}
	
void itoa(int value, char* str, int base) {
	
	//Code piece from: http://www.strudel.org.uk/itoa/
	
	static char num[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	
	char* wstr=str;
	
	int sign;
	
	div_t res;
		
	// Validate base
	if (base<2 || base>35){ *wstr='\0'; return; }
	
	// Take care of sign
	if ((sign=value) < 0) value = -value;
	
	// Conversion. Number is reversed.
	do {
	
		res = div(value,base);
		*wstr++ = num[res.rem];	
	}
	while((value=res.quot));
	
	if(sign<0) *wstr++='-';
	
	*wstr='\0';
	
	// Reverse string	
	strreverse(str,wstr-1);	
}   

unsigned long bin2int(const char *bin) {
	//https://stackoverflow.com/questions/12338584/binary-to-decimal-in-c	
	unsigned long i=0;
    int j=0;
    j = sizeof(int)*8;
    while ( (j--) && ((*bin=='0') || (*bin=='1')) ) {
        i <<= 1;
        if ( *bin=='1' ) i++;
        bin++;
    }
    return i;
}

int bin2hex(char *bin) {
	//https://stackoverflow.com/questions/5307656/how-do-i-convert-a-binary-string-to-hex-using-c	
	//char *bin="10010011";
	char *a = bin;
	int num = 0;
	do {
		int b = *a=='1'?1:0;
		num = (num<<1)|b;
		a++;
	} while (*a);
	
	return num;
}

unsigned long DecBase64(char *index, int base){
	
	char *e;
	//char *indexNgram = malloc((strlen(index)*6)+2);
	char *indexNgram=(char *)calloc(2, (strlen(index)*6)+1);
	
	//printf("\nINDEX: %s\n", index);
	
	for(int i=0; i<strlen(index);i++)
	{
		//strchr = searches for the first occurance of the character c in the string pointed to by the argument
		
		char p;
		
		p = index[i];
		
		//printf("letter: %c", p);
		
		e = strchr(b64, p);

		int n = ((int)(e - b64))+1;
		
		//printf(" (%d)\n", n);

		//printf("Posicao: %d\nBinary value: ", n);

		//Converting Decimal to binary
		
		int c, k;
		
		//char *piece = malloc(6);
			

		for (c = 5; c >= 0; c--)
		{
			//right shift
			k = n >> c;

			//bitwise AND
			if (k & 1){ 
				//printf("1");
				//piece[c]='1';
				strcat(indexNgram, "1");
			}
			else{
				//printf("0");
				//piece[c]='0';
				strcat(indexNgram, "0");
			}
			
			//appends the string pointed to by src to the end of the string pointed to by dest
			//strcat(result, piece);
		}
	}
	
	//printf("\n\nResult: %s \n\n", indexNgram);
	//printf("DECIMAL: %d\n", bin2int(result)); 
	//printf("HEX: %X\n", bin2hex(result));
	
	unsigned long ret=0;
	
	if(base == 2)
		ret = bin2int(indexNgram);
	else if(base == 16)
		ret = bin2hex(indexNgram);
		
	free(indexNgram);
	
	return ret;	
}


/*char *findIDline(long long int id){



}*/