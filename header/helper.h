
#ifndef _H_HELPER
#define _H_HELPER
#include<stdlib.h>
struct hash *hashTable;
unsigned long eleCount;
unsigned long contDigests1;
unsigned long contDigests2;


int log_open;
long long contForID;
extern long long contForNgram;
static const char *b64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
unsigned int total_ids;


char* 			FindFileName(char *digest,int lookup);
void  			createLogFile(char *filename);
int				openLogFile(char *filename);
void 			writeLog(char *message, int newline,char *filename);
void 			writeIDlog(long long int id, int newline, char *filename);
void 			strreverse(char* begin, char* end);
void 			itoa(int value, char* str, int base);
unsigned long	 bin2int(const char *bin);
int 			bin2hex(char *bin);
unsigned long 	DecBase64(char *index, int base);
void 			removeItensDIR(char *dirFiles);

#endif //_H_HELPER
	