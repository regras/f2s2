#include"../header/config.h"
#include"../header/helper.h"
#include"../header/index.h"
#include<stdio.h>
#include<string.h>
#include<dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

int name_index = 0;

struct nodeBucket * createNodeBucket(long long id) {
	struct nodeBucket *newnode;
	newnode= (struct nodeBucket *) malloc(sizeof(struct nodeBucket) + 2);
	
	newnode->id = id;
    newnode->next = NULL;
    
	return newnode;
}
 
struct nodeNgram * createNodeNgram(char *index, char *ekey, long long id) {
    
    struct nodeNgram *newnode;  
    newnode = (struct nodeNgram *) malloc(sizeof(struct nodeNgram) + 2);
    
    if(newnode == NULL) {	
		printf("Error! memory not allocated for E-KEY.");
		return NULL;		
    }
    
    strcpy(newnode->index, index);
    strcpy(newnode->ekey, ekey);
    newnode->next = NULL;
    newnode->count=1;
    
    struct nodeBucket *nBck = createNodeBucket(id);
    nBck->id=id;
    nBck->next=NULL;
    
    newnode->head=nBck;
    
    //printf("\t\tCreating node: index: %s, ekey: %s, id: %d\n", index, ekey, id);
    //printf("\t\tCreating node (NEW): index: %s, ekey: %s, id: %d\n", newnode->index, newnode->ekey, newnode->id);
    
    return newnode;
}

// Ordem de processamento: InsertToHash->InsertToDigest->InsertingElementHashTable
void insertingElementHashTable(char *index, char *ekey, long long id) {
    if(index == NULL) {
        printf("Index NULL");
        writeLog("Index NULL", 1,logFile);
        return;
    }
    
    if(ekey == NULL){
        printf("E-key NULL");
        writeLog("E-key NULL", 1,logFile);
        return;
    }
    
    int indexNgram = 0;
    unsigned long hashIndex=0;
    
    indexNgram = DecBase64(index, 2);
    hashIndex = indexNgram % eleCount;
    
    if(msgs == 1)printf("\nInserting n-gram: %s from Digest: %lld", ekey, id);
     
    /* head of list for the bucket with index "hashIndex" */
    if (!hashTable[hashIndex].head) {       
        struct nodeNgram *newnode = createNodeNgram(index, ekey, id);
        hashTable[hashIndex].head = newnode;
        hashTable[hashIndex].count = 1; 
        
        return;
    }
    
    /* adding new node to the list */
    struct nodeNgram *nodesList = hashTable[hashIndex].head;
    int insertNgram=1;
    int insertId=1;
    char ngram[7];
    strcpy(ngram, index);
    strcat(ngram, ekey);

    /* Cheching if any element in the node already contain the n-gram */
    while (nodesList != NULL) {
        if(msgs == 1) printf("\n Comparing hash table n-gram: %s with queried one: %s", nodesList->index, ngram);
        if ( (strcmp(nodesList->index, ngram) == 0) ) {
            
            insertNgram = 0;
            
            struct nodeBucket *nodeBkt = nodesList->head;
                
            while (nodeBkt != NULL) {
                
                if(msgs == 1) printf("\nNode bucket ID: %lld and queried ID: %lld", nodeBkt->id, id);
                
                if(nodeBkt->id == id){
                    insertId = 0;
                    nodeBkt=NULL;
                }
                else
                    nodeBkt = nodeBkt->next;
            }
            
            if(insertId > 0) {
                
                //update the head of the list
                
                struct nodeBucket *tempNodeBkt = createNodeBucket(id);
                
                tempNodeBkt->next = nodesList->head;
                nodesList->head = tempNodeBkt;          
                nodesList->count++;
            }           
        }
        
        nodesList = nodesList->next;
    }   
    
    if(insertNgram > 0){
        
        struct nodeNgram *newnode = createNodeNgram(index, ekey, id);
    
        if(msgs == 1) {
            printf(" \tIndex: %s (%d)\t ekey: %s\n", index, indexNgram, ekey);
            printf("Index NODE: %s\t Ekey: %s\n", newnode->index, newnode->ekey);
        }
        
        newnode->next = (hashTable[hashIndex].head);
    
        //update the head of the list
        hashTable[hashIndex].head = newnode;
        hashTable[hashIndex].count++;
    }
    
    //free(newnode);
    
    return;
}

// aqui sim iremos inserir o id e as assinaturas, até então apenas quebramos o digest nesses pedaços
void insertDigest(char *digest, long long id, int sig) {

    if(digest == NULL) {
        printf("Digest NULL!\n");
        writeLog("Digest NULL", 1,logFile);
        return;
    }
    
    int lenDigest=strlen(digest);
    int lenNgrams = lenDigest-NGRAMSIZE+1;
    
    if(msgs == 1){
        printf("\nExtracting n-grams from ID %lld\n", id);
        printf("Digest:%s\n", digest);
        printf("Size of digest:%d\n", lenDigest);
        printf("\nNumber of n-grams: %d\n", lenNgrams);
    }
    
    if(lenNgrams <= 0) {
        if(msgs)printf("Digest too short to insert in the hash table. Only %d caracters (min.: %d)!\n", lenDigest, NGRAMSIZE);
        return;
    }
   // Conta quantos assinaturas 1 e 2 foram inseridas 
    if(sig == 1)
        contDigests1++;
    else
        contDigests2++;
        
    contForNgram = contForNgram + lenNgrams;
    
    for (int i=0; i < lenNgrams; i++) {
        char * ngram = NULL;
        char * index = NULL;
        char * ekey = NULL;
    
        ngram=(char *)calloc(2, NGRAMSIZE * sizeof(*ngram)+1);
        
        if(ngram != NULL) {
            memcpy(ngram, digest+i, NGRAMSIZE);
        }

        else{
            printf("ERRO MEMORY ALLOCATION AT NGRAM");
            writeLog("ERRO MEMORY ALLOCATION AT NGRAM", 1,logFile);
            return;
        }
        
        if(msgs == 1) {
            printf("\tN-gram %d: %s\n", i+1, ngram);
            printf("\tInserting n-gram %d: %s", i+1, ngram);
        }
        
        index = (char *) calloc(2, INDEX * sizeof(*index)+1);
        
        if(index == NULL) {
            printf("ERRO MEMORY ALLOCATION AT INDEX");
            writeLog("ERRO MEMORY ALLOCATION AT INDEX", 1,logFile);
            return;
        }
        
        memcpy(index, ngram, INDEX);
        ekey = (char *) calloc(2, EKEY * sizeof(*ekey)+1);
        
        if(ekey == NULL) {
            printf("ERRO MEMORY ALLOCATION AT E-KEY");
            writeLog("ERRO MEMORY ALLOCATION AT E-KEY", 1,logFile);
            return;
        }
        
        memcpy(ekey, ngram+INDEX, EKEY);
        
        insertingElementHashTable(index, ekey, id);
        
        free(ngram);
        free(index);
        free(ekey);
    }
    
    if(msgs == 1)
        printf("Insertion completed!\n");

}
void insertToHash(char *digest) {

    char linha[LINE];
    char name[LINE];    
    FILE *arq;
    arq = fopen(digest, "r");

    /*
     * Para que possamos relacionar os IDs com os nomes dos arquivos 
     * devemos registrar em memória todos os IDS e os nomes para referencia futura 
     * Para isso devemos saber quantos digests iremos ter:
     */
    
    while (!feof(arq)){       
        if( fgets (linha, LINE, arq)!=NULL )total_ids++;
    }

    /*
     * Com o número total de digests calculado, podemos agora alocar memória para uma tabela 
     * contendo IDs e nomes de arquivos
     */

    IDtable = calloc(total_ids+1,sizeof(long long int*));
    nametable = calloc(total_ids+1,sizeof(char*));
    fseek(arq,0,SEEK_SET);


    if(!arq){
        fprintf(stderr, "Couldn't open %s\n", digest);
        writeLog("\nCouldn't open digest\n", 1,logFile);
        return;
    }
    
    printf("\n\nReading file...\n\n");
    writeLog("\nReading file: ", 0,logFile);
    writeLog(digest, 1,logFile);
    
    contForID=0;
    
    

    while (!feof(arq))
    {       
        // Lê uma linha (inclusive com o '\n')
        if( fgets (linha, LINE, arq)!=NULL ) // o 'fgets' lê até 199 caracteres ou até o '\n'
        {
            //printf("\nResult: %s", linha);
            
            char temp[LINE];
            strcpy(temp, linha);
            /*Used to allow us to print the inserted files*/
            strcpy(name, linha);
            //printf("\nLength: %ld", strlen(linha));
            
            contForID++;
                        
            //LER DIGESTS DO ARQUIVO
            if(strcmp("ssdeep,1.1--blocksize:hash:hash,filename\n", linha) != 0) {
                
                //variable to allow only valid insertions (Block size: SIG1: SIG2: name)
                //Value 0: invalid      Value 1: Valid
                int insert = 1;
                
                //antes o valor de calloc era 1, ao inves de 0. Mudei para 0 para diminuir a quantidade de memória vazada
                //char *token = (char *)calloc(0, sizeof(*token)+1);
                //Comentei linha acima: não vaza mais memória. Fazer testes...
                char *token = strtok(linha, ":");
                char *sig1 = (char*) malloc(64*sizeof(char*) + 2);
    
                if(sig1 == NULL) {
                    printf("Erro memory allocation: SIG1");
                    writeLog("\nErro memory allocation: SIG1\n", 1,logFile);
                    insert=0;
                }
                
                char *sig2 = (char*) malloc(32*sizeof(char*) + 2);

                if( sig2 == NULL) {
                    printf("Erro memory allocation: SIG2");
                    writeLog("\nErro memory allocation: SIG2\n", 1,logFile);
                    insert=0;
                }
                
                long long id = 0;
                long p1 = 0;
                long p2 = 0;
                //Formação do ID com base no path para o arquivo
                while(token && insert > 0) {
                    
                    if(msgs == 1)                   
                        printf("BLOCK size: %s\n", token);
                        
                    //Foward to the next element, sig1...
                    token = strtok(NULL, ":");
                    
                    if(token == NULL || strcmp(token, "") == 0) {
                        insert=0;
                        continue;
                    }
                    
                    strcpy(sig1, token);
                    
                    if(msgs == 1)                   
                        printf("SIG1: %s\n", sig1);
                    
                    //Foward to the last element, sig2 + NAME
                    token = strtok(NULL, ":");
                    
                    //Getting sig2
                    char *token2 = strtok(token, ",");
                    
                    if(token2 == NULL || strcmp(token2, "") == 0){
                        insert=0;
                        continue;
                    }
                        
                    strcpy(sig2, token2);
                    
                    //Getting NAME
                    token2 = strtok(NULL, ",");
                   
                    
                    if(msgs == 1) {
                        printf("SIG2: %s\n", sig2);
                        printf("Name: %s\n", token2);
                    }
                    
                    //Processing NAME -> transform it to ID (numeric value)
                    
                    if(token2 == NULL || strcmp(token2, "") == 0) {
                        insert=0;
                       
                        continue;
                    }
                    
                    //Dealing with NIST dataset and names with path
                    char *token3 = strtok(token2, "/");
                    
                    if(token3 == NULL || strcmp(token3, "") == 0)
                        continue;
                        
                    //Adding left 0 to allow integer transformation
                    token3[0]='0';                  
                    p1=atoi(token3);
                    
                    if(msgs == 1)
                        printf("P1 value: %ld\n", p1);
                    
                    token3 = strtok(NULL, "/");
                    
                    //IF name is NOT a Numerical sequence...
                    if(token3 != NULL){
                        
                        //Dealing with Path NAMES format ("/boot/grub/locale/vi.mo")
                        //Choosing a numeric sequence as default for each folder + conter
                        
                        //FOLDER /boot/ (LINUX)
                        if(strcmp(token3, "boot") == 0) {
                            
                            //Format boot: 999900000000000
                            p1=999;
                            p2=9;
                        
                            id = contForID + (p1*1000000000000) + (p2*100000000000);
                        }
                        //FOLDER /bin/ (LINUX)
                        else if(strcmp(token3, "bin") == 0) {
                            //Format bin: 980900000000000
                            p1=980;
                            p2=9;
                        
                            id = contForID + (p1*1000000000000) + (p2*100000000000);
                        }
                        //FOLDER /dev/ (LINUX)
                        else if(strcmp(token3, "dev") == 0) {
                            //Format dev: 970900000000000
                            p1=970;
                            p2=9;
                        
                            id = contForID + (p1*1000000000000) + (p2*100000000000);
                        }
                        //FOLDER /etc/ (LINUX)
                        else if(strcmp(token3, "etc") == 0) {
                            //Format etc: 960900000000000
                            p1=960;
                            p2=9;
                        
                            id = contForID + (p1*1000000000000) + (p2*100000000000);
                        }
                        //FOLDER /lib/ (LINUX)
                        else if(strcmp(token3, "lib") == 0) {
                            //Format lib: 950900000000000
                            p1=950;
                            p2=9;
                        
                            id = contForID + (p1*1000000000000) + (p2*100000000000);
                        }
                        //FOLDER /lib32/ (LINUX)
                        else if(strcmp(token3, "lib32") == 0) {
                            //Format etc: 940900000000000
                            p1=940;
                            p2=9;
                        
                            id = contForID + (p1*1000000000000) + (p2*100000000000);
                        }
                        //FOLDER /lib64/ (LINUX)
                        else if(strcmp(token3, "lib64") == 0) {
                            //Format lib64: 930900000000000
                            p1=930;
                            p2=9;
                        
                            id = contForID + (p1*1000000000000) + (p2*100000000000);
                        }
                        //FOLDER /mnt/ (LINUX)
                        else if(strcmp(token3, "mnt") == 0) {
                            //Format mnt: 920900000000000
                            p1=920;
                            p2=9;
                        
                            id = contForID + (p1*1000000000000) + (p2*100000000000);
                        }
                        //FOLDER /opt/ (LINUX)
                        else if(strcmp(token3, "opt") == 0) {
                            //Format opt: 910900000000000
                            p1=910;
                            p2=9;
                        
                            id = contForID + (p1*1000000000000) + (p2*100000000000);
                        }
                        //FOLDER /run/ (LINUX)
                        else if(strcmp(token3, "run") == 0) {
                            //Format run: 900900000000000
                            p1=900;
                            p2=9;
                        
                            id = contForID + (p1*1000000000000) + (p2*100000000000);
                        }
                        //FOLDER /sbin/ (LINUX)
                        else if(strcmp(token3, "sbin") == 0) {
                            //Format sbin: 909900000000000
                            p1=909;
                            p2=9;
                        
                            id = contForID + (p1*1000000000000) + (p2*100000000000);
                        }
                        //FOLDER /srv/ (LINUX)
                        else if(strcmp(token3, "srv") == 0) {
                            //Format srv: 908900000000000
                            p1=908;
                            p2=9;
                        
                            id = contForID + (p1*1000000000000) + (p2*100000000000);
                        }
                        //FOLDER /sys/ (LINUX)
                        else if(strcmp(token3, "sys") == 0) {
                            //Format sys: 907900000000000
                            p1=907;
                            p2=9;
                        
                            id = contForID + (p1*1000000000000) + (p2*100000000000);
                        }
                        //FOLDER /usr/ (LINUX)
                        else if(strcmp(token3, "usr") == 0) {
                            //Format usr: 906900000000000
                            p1=906;
                            p2=9;
                        
                            id = contForID + (p1*1000000000000) + (p2*100000000000);
                        }
                        //FOLDER /media/vitormoia/Novo volume/ (partition)                      
                        else if(strcmp(token3, "home") == 0) {
                            
                            token3 = strtok(NULL, "/");
                            token3 = strtok(NULL, "/");
                            
                            if(token3 != NULL) {
                                
                           
                                if(strcmp(token3, "686A50F06A50BD0E") == 0) {
                                    
                                    //Format bin: 800900000000000
                                     p1=800;
                                    p2=9;
                                
                                    id = contForID + (p1*1000000000000) + (p2*100000000000);
                                }
                                //FOLDER /media/vitormoia/Novo volume/ (partition)                      
                                else /*if(strcmp(token3, "") == 0)*/ 
                                {
                                    //Format bin: 900900000000000
                                    p1=900;
                                    p2=9;
                                
                                    id = contForID + (p1*1000000000000) + (p2*100000000000);
                                }
                            }
                        }
                        else{
                            
                            //Dealing with NIST NAME format ("000/000/000000000")
                            
                            p2=atoi(token3);
                                
                            token3 = strtok(NULL, "/");
                            
                            if(token3 == NULL)
                                continue;
                            
                            id = atoi(token3) + (p1*1000000000000) + (p2*1000000000);
                        }
                        
                        printf("ID: %lld \t P1: %ld \t P2: %ld\n", id, p1, p2);
                        
                    }
                    else {
                        //NAME is a numerical sequence
                        id = p1;
                        
                        if(msgs == 1)printf("ID: %lld\n", id);
                    }

                    char *FileName;
                    FileName = FindFileName(name,0);
                    if(msgs)if(!FileName) printf("Error getting file name\n");
                    iDvsNameHandle = fopen(iDvsName,"a");
                    if(msgs)if(!iDvsNameHandle)printf("Error opening iDvsName.txt :( \n");
                    fprintf(iDvsNameHandle,"%s\t%lld\n", FileName, id);
                    fclose(iDvsNameHandle);
                    
                    //token3 = strtok(NULL, "/");
                    //token2 = strtok(NULL, ",");
                    //token = strtok(NULL, ":");
                    
                    token3=NULL;
                    token2=NULL;
                    token=NULL;
                    free(token2);
                    free(token3);

                
                }
                    
                    /*
                     * Como as strings ocupam muito espaço não podemos alocar os nomes
                     *  dos arquivos para relacionar com os ids, então, podemos utilizar 
                     * um arquivo auxiliar para tal
                     */
                 
                    
                  //  IDtable[name_index] = id;
                    //Aqui temos o id do arquivo pront já, então podemos coloca-lo no arquivo que contém a
                    // relação iDvsNome

                 // Até aqui só criamos os valores  serem inseridos, id, etc
                //INSERTING DIGESTS....
                
                if(insert > 0) {
                
                    if(sig1 != NULL && (strcmp(sig1, "") != 0 && (strcmp(sig1, ",\"000/000/000000000\"\n") != 0))) {
                    
                        if(msgs == 1)
                            printf("\nInserting n-grams Digest 1... %s\t\tID: %lld\n\n", sig1, id);
                            
                        insertDigest(sig1, id, 1);
                    }
                    
                    if(sig2 != NULL && strcmp(sig2, "") != 0 && (strcmp(sig2, ",\"000/000/000000000\"\n") != 0)) {
                        
                        if(msgs == 1)
                            printf("\nInserting n-grams Digest 2...%s\t\tID: %lld\n\n", sig2, id);
                            
                        insertDigest(sig2, id, 2);  

                    }

                
                }

                else {
                    
                    char buffer[LINE];
                    snprintf(buffer, LINE, "Digest not inserted: %s\tLine: %lld", temp, contForID);
                    writeLog(buffer,1,logFile);     
                    token=NULL;         
                    
                }
                
                free(sig1);
                free(sig2);
                free(token);
                id=0;
                
                //printf("\nEnd of the file...\n\n");
                
            }
        }
    }
    
    writeLog("Finished! OK...", 1,logFile);
    printf("Done!\n");
    
    fclose(arq);
    
    return;
}
void deleteFromHash(int key) {
    /*
    // find the bucket using hash index 
    int hashIndex = key % eleCount, flag = 0;
    struct nodeNgram *temp, *myNode;
    // get the list head from current bucket
    myNode = hashTable[hashIndex].head;
    if (!myNode) {
        printf("Given data is not present in hash Table!!\n");
        return;
    }
    temp = myNode;
    while (myNode != NULL) {
        // delete the node with given key
        if (myNode->key == key) {
            flag = 1;
            if (myNode == hashTable[hashIndex].head)
                hashTable[hashIndex].head = myNode->next;
            else
                temp->next = myNode->next;
 
            hashTable[hashIndex].count--;
            free(myNode);
            break;
        }
        temp = myNode;
        myNode = myNode->next;
    }
    if (flag)
        printf("Data deleted successfully from Hash Table\n");
    else
        printf("Given data is not present in hash Table!!!!\n");
    return;
    
    */
    
    return;
}
void searchInHash(char *digest, int indexParam, int threshold) {    
    if(msgs)printf("++++++++++++++++++++++++++++++++\n");

    char *FileName;
    FileName = FindFileName(digest,1);
    if(!results){
        printf("Couldn't open %s\n", results);
        return;
        }

    if(digest != NULL) {
 
        //fprintf(results,"|%s|",FileName);

        int lenDigest=strlen(digest);
        int lenNgrams = lenDigest-NGRAMSIZE+1;
               

    /*
     * Alocação de memória para a array contendo os ids e o quanto se repetem para um mesmo arquivo dentro do index
     */
     
        // Variável que indica a posição do último 
        int last_inserted = 0;
        // Ponteiro para a array que contém a relação entre o iD que tem possível relação e quantas vezes esse iD foi encontrado
        long long int **resultado;
        //  Aloca 2 posições, uma para o iD e uma para quantas vezes o iD é encontrado
        resultado = calloc(2,sizeof(*resultado));
        // Expansão para o total de N-grams encontrados
        for (int i =0; i<2;i++) resultado[i]=calloc(lenNgrams,sizeof*(resultado[i]));

        if(msgs == 1){  
            printf("\nExtracting n-grams...\n");
            printf("Digest: %s \n", digest);
            printf("Size of digest: %d\n", lenDigest);
            printf("Number of n-grams: %d\n", lenNgrams);
        }
        
        if(lenNgrams <= 0) {
           if(msgs)printf("Digest too short to lookup in the hash table. Only %d caracters (min.: %d)!\n", lenDigest, NGRAMSIZE);      
            return;
        }        

        //N-gram extraction
        unsigned int ngramsfound = 0;
        for (int i=0; i < lenNgrams; i++) {
        
            char * ngram = NULL;
            char * index = NULL;
            char * ekey = NULL;
        
            //ngram = (char*)malloc(NGRAMSIZE * sizeof(*ngram) + 2);
            ngram =(char *)calloc(2, NGRAMSIZE * sizeof(*ngram)+1);
            index = (char *) calloc(2, INDEX * sizeof(*index)+1);
            ekey = (char *) calloc(2, EKEY * sizeof(*ekey)+1);
            
            if(index == NULL || ngram == NULL || ekey == NULL){
                if(ngram == NULL)printf("Error! memory not allocated for the N-GRAM.");
                else if(index == NULL) printf("Error! memory not allocated for INDEX.");
                else printf("Error! memory not allocated for E-KEY.");
                return;
            }
            
            memcpy(ngram, digest+i, NGRAMSIZE);
            memcpy(index, ngram, INDEX);
            memcpy(ekey, ngram+INDEX, EKEY);    
            
            int indexNgram = 0;
            indexNgram = DecBase64(index, 2);   
                
            unsigned long hashIndex = indexNgram % eleCount;
            int flag=0;
            
            struct nodeNgram *myNode = hashTable[hashIndex].head;

            while (myNode != NULL) {
                
                if (strcmp(myNode->ekey, ekey) == 0) {
                    
                    if(msgs == 1){
                        printf("\t\tN-gram: %s", myNode->index);
                        printf("\t\tE-key: %s", myNode->ekey);
                        printf("\tEqual n-grams: %d\n", myNode->count);
                        printf("\t\t\t\t\t\t\tList of IDs: \n");
                    }
                    struct nodeBucket *idNgram = myNode->head;
                    ngramsfound++;


                    while (idNgram != NULL) {
                        
                       if(msgs == 1) printf("\t\t\t\t\t\t\t\t\tIDs que tem este n-gram: %lld\n", idNgram->id);
/*
 *  Checamos se o id já foi inserido no vetor alocado para ponteiros, 
 *  se ja tiver sido ele simplesmente incrementa a posição apontada pelo iD
 *  se nao tiver sido inserido ainda, ele zera a posição e soma 1
 */

                        for(int j = 0;j<=last_inserted;j++){
/* 
 * Se todo o vetor for percorrido e não for encontrado nenhuma ocorrência anterior daquele iD,
 * colocamos o iD no vetor e incrementamos seu número de ocorrências     
 */
                            if(resultado[0][j] == 0){
                               if(msgs == 1) printf("NEW ID FOUND!\n");
                                resultado[0][j] = idNgram->id;
                                resultado[1][j] = 1;
                                last_inserted++;
                                break;
                            }
/*
 * Se encontrar uma ocorrência anterior deste iD, basta incrementar o número de ocorrências e prosseguir para a próxima n-gram
 */
                            else if(resultado[0][j] == idNgram->id){
                                resultado[1][j]++;
                                break;
                            }
                        }
                         // Pula para o próximo iD encontrado com a mesma N-gram
                         idNgram = idNgram->next;
                    }
        
                    flag = 1;
                }
                myNode = myNode->next;

              
            }

            free(ngram);
            free(index);
            free(ekey);
            free(myNode);
            
            if (!flag);
                if(msgs ==1)printf("\t\t\tSearch element unavailable in hash table\n"); 

        }
        /*
         *  Print dos arquivos relacionados
         */

            writeLog(FileName,0,results);
            writeLog("\t", 0, results);

            int tab = 0;
            /*
             *  Percorremos todo o vetor alocado com os possíveis iDs relacionados
             */

            for(int j = 0;j<=lenNgrams;j++){

                if(resultado[0][j]==0){ // Se chegarmos ao final dos iDs...
                    if(j == 0){ // E não tiverem sido encontrados possíveis iDs relacionados
                        if(msgs) printf("NO N-GRAMS FOUND IN THE INDEX!\n");
                        writeLog("\tNo N-grams found in the index",1,results);
                    }
                    break;
            
                }
                    if(resultado[1][j] >= threshold){             
                            if(msgs)printf("ID:%lld -> %lld\n",resultado[0][j],resultado[1][j]);
                            writeLog("\t", 0, results);
                            if(tab) writeLog("\t", 0, results);
                            writeIDlog(resultado[0][j], 0, results);
                            writeLog("\t", 0, results);
                            writeIDlog(resultado[1][j], 0, results);
                            writeLog("\t", 0, results);
                            writeIDlog((long long int)lenNgrams, 1, results);
                            tab = 1;
                    }
                    
                }
               
   
    free(resultado);
    }

    // usado para busca com parâmetro
    else if(indexParam > 0){
        printf("Looking for Index: %d\n", indexParam);
            
        unsigned long hashIndex = indexParam % eleCount;
        int flag=0;
        
        struct nodeNgram *myNode = hashTable[hashIndex].head;
        
        printf("\t\tResult: \n");
        
        printf("\t\t\tDifferent n-grams in this bucket: %d\n\n", hashTable[hashIndex].count);

        while (myNode != NULL) {
                
            printf("\t\t\tN-gram: %s", myNode->index);
            printf("\t\tE-key: %s", myNode->ekey);
            printf("\tEqual n-grams: %d\n", myNode->count);
            printf("\t\t\t\t\t\t\tList of IDs: \n");
            
            struct nodeBucket *idNgram = myNode->head;
            
            while (idNgram != NULL) {
                
                printf("\t\t\t\t\t\t\t\t\tId: %lld\n", idNgram->id);
                
                idNgram = idNgram->next;
            }
            
            if(msgs ==1)printf("\t\t\t................................................................\n");
            
            flag = 1;
            myNode = myNode->next;
        }
        
        free(myNode);
        
        if (!flag)
           if(msgs ==1) printf("\t\t\tSearch element unavailable in hash table\n"); 
    }
    else {
        printf("Erro in the lookup procedure! Digest and INDEX NULL!");
        return;
    }
  
    return;
}

void display() {
    
    int cont=0;             /* Counter for the number of different n-grams */ 
    int contEqual=0;        /* Counter for the total number of n-grams (different + equal) */ 
    int contEmpty=0;        /* Counter for the number of empty slots in the hash table */ 
    //int contArqResult=0;  /* Counter for identifying the result files, sequentially */ 
    
    FILE *arqDisplay;
    
    char fileDisplayResults[120];
    char str[8];
    
    //Removing all files in the directory where the results will be stored
    //removeItensDIR(PATHRESULTS);
    
    strcpy(fileDisplayResults, PATHRESULTSFILE);
    //sprintf(str, "%d.txt", contArqResult);
    sprintf(str, ".txt");
    strcat(fileDisplayResults, str);
            
    arqDisplay = fopen(fileDisplayResults, "w");
    
    fprintf(arqDisplay, "%s \t\t %s \t\t %s \n", "INDEX", "Cont", "Cont Equal Files");
        
    for (unsigned int j = 0; j < eleCount; j++) {
        
        int numElements = hashTable[j].count;
        
        if(msgs == 1)
            printf("\nNumber of elements: %d", numElements);
        
        if (numElements > 0) {
                    
            cont = cont + numElements;
            
            struct nodeNgram *myNode = hashTable[j].head;
            
            if (myNode == NULL) {
                free(myNode);
                continue;
            }
            
            fprintf(arqDisplay, "%d \t\t %d \t\t", j, numElements);
            
            while (myNode != NULL) {
                
                fprintf(arqDisplay, " %d", myNode->count);
                contEqual = contEqual+myNode->count;
                
                myNode = myNode->next;
            }
            
            free(myNode);
            fprintf(arqDisplay, "\n");
            
        }
        else {
            
            fprintf(arqDisplay, "%d \t\t %d \t\t %d\n", j, 0, 0);
            contEmpty++;
        }
            
    }
    
    fprintf(arqDisplay, "\n%s \t\t%d\n", "Contador final de ngrams (diferentes): ", cont);
    fprintf(arqDisplay, "\n%s \t\t%d\n", "Contador final de ngrams (total): ", contEqual);
    fprintf(arqDisplay, "\n%s \t%d\n", "Contador final ngrams (empty slots): ", contEmpty);
    fprintf(arqDisplay, "\n%s \t%ld\n", "Contador final de digests (sig1): ", contDigests1);
    fprintf(arqDisplay, "\n%s \t%ld\n", "Contador final de digests (sig2): ", contDigests2);
    
    fclose(arqDisplay);
    
    printf("\n\nResult:");
    
    printf("\nContador de ngrams (diferentes): %d", cont);
    printf("\nContador de ngrams (total): %d", contEqual);
    printf("\nContador de ngrams (empty slots): %d\n", contEmpty);
    printf("\nContador final de digests (sig1): %ld\n", contDigests1);  
    printf("\nContador final de digests (sig2): %ld\n", contDigests2);
    
    return;
}

void InsertSetDigests(char *path) {
    //abre a pasta, Digests, que contém os arquivos com os hashes
    arqDigests = fopen(fileofDigests, "w");
    if(!logFile){
        fprintf(stderr, "Couldn't open file %s\n", fileofDigests);
        return;
    }
    else{
        struct dirent **namelist;
        int m = scandir(path, &namelist, 0, alphasort);
        
        if (m < 0)
            perror("scandir");
        else {
            while(m--) {
                // lista o arquivo com o hash seguinte
                printf("\nArquivo para processar: %s\n", namelist[m]->d_name);
                //apenas printa os diretórios . e .. 
                if((strcmp(namelist[m]->d_name, ".") == 0)){
                    printf("Arquivo: %s\n", namelist[m]->d_name);
                    continue; 
                }
                
                if (strcmp(namelist[m]->d_name, "..") == 0){
                    printf("Arquivo: %s\n", namelist[m]->d_name);
                    continue; 
                }
                //forma o nome do caminho
                char pathDir[strlen(path)+20];
                
                strcpy(pathDir, path);
                
                strcat(pathDir, "/");
                
                struct stat path_stat;           
                // escreve o caminho até os digests no log
                writeLog("\nPath to take the digests: ", 0, logFile);
                writeLog(pathDir, 1,logFile);
                
                strcat(pathDir, namelist[m]->d_name);
                
                printf("Path: %s\n", pathDir);
                
                int status = stat(pathDir, &path_stat);
                // detecção de diretório e arquivos
                if (status != 0) {
                    printf ("Error, errno = %d\n", errno);
                    writeLog("\nError in path status, errno", 1,logFile);
                    return;
                }
                
                if (S_ISREG (path_stat.st_mode)) {
                    printf ("%s is a regular file.\n", pathDir);
                    
                    //Wrting on file
                    fprintf(arqDigests, "%s\n", pathDir);
                }
                else if (S_ISDIR (path_stat.st_mode)) {
                    printf ("is a directory.\n");
                    
                    InsertSetDigests(pathDir);
                }
                
                free(namelist[m]);
            }
            free(namelist);
        }
        
        return; 
    }
}

void CreateListOfDigestFiles(char *path) {
    //abre digestsFiles.txt
    arqDigests = fopen(fileofDigests, "w");
    if(!logFile){
        fprintf(stderr, "Couldn't open file %s\n", fileofDigests);
        return;
    }
    else
        // apenas cria um log com o caminho até os arquivos
        InsertSetDigests(path);
    
    fclose(arqDigests);
    
    return;
}

void ProcessingDIgestFile(){
    FILE *arq;
    char linha[LINE];
    arq = fopen(fileofDigests, "r");
    //printf("%s\n",fileofDigests);
    if(!arq){
        fprintf(stderr, "Couldn't open %s\n", fileofDigests);
        writeLog("\nCouldn't open digest file", 1,logFile);
        return;
    }  
    
    while (!feof(arq))
    {       
        // Lê uma linha (inclusive com o '\n')
        if( fgets (linha, LINE, arq)!=NULL ) // o 'fgets' lê até 199 caracteres ou até o '\n'
        {
            char *pos;
            if ((pos=strchr(linha, '\n')) != NULL)
                *pos = '\0';
            insertToHash(linha);
        }
    }
    
    fclose(arq);

    return;
}


