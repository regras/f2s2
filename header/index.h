#ifndef _H_INDEX
#define _H_INDEX
#include"helper.h"
#include"config.h"

/*struct nodeBucket;
struct hash;
struct nodeNgram;*/


 /* Struct que contem o id de um bucket e um ponteiro para o id seguinte */
struct nodeBucket {

    long long id;

    struct nodeBucket *next;    /*Points to the next ID (Linked list)*/
};


/*   Struct que contém uma array de tamanho INDEX e outra de tamanho EKEY, 
 *   contador (indicando quantos arquivos diferentes tem o mesmo n-gram)
 *   um ponteiro para a struct do próximo n-gram e um ponteiro para uma lista de ids           
 */

struct nodeNgram {
    char index[INDEX], ekey[EKEY];
    int count;                  /*Count the number of equal n-grams of different digests*/
    struct nodeNgram *next;     /*Points to the next n-gram (Linked list)*/
    struct nodeBucket *head;    /*Points to a linked list of digest IDs*/
};

 /*Struct que contém um ponteiro para a structure que contém a ekey e o index nodeNgram */

struct hash {
    struct nodeNgram *head;
    int count;
};
unsigned int total_ids;

long long int *IDtable;
char **nametable;
int name_index;

struct nodeBucket   *createNodeBucket(long long id);
struct nodeNgram    *createNodeNgram(char *index, char *ekey, long long id);
void 				insertingElementHashTable(char *index, char *ekey, long long id);
void 				insertDigest(char *digest, long long id, int sig);
void 				insertToHash(char *digest);
void 				deleteFromHash(int key);
void 				searchInHash(char *digest, int indexParam, int threshold);
void 				InsertSetDigests(char *path);
void 				CreateListOfDigestFiles(char *path);
void 				ProcessingDIgestFile();
void                display();

#endif //_H_INDEX