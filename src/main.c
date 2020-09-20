#include"../header/helper.h"
#include"../header/config.h"
#include"../header/index.h"
#include"../header/main.h"


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <dirent.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

int main() {
    int n,ch, threshold;
    char digest[100];
    unsigned int index = 0;

	/*
     *  Cria os arquivos de log e de iDvsName
     */
    createLogFile(logFile);
    createLogFile(iDvsName); 
	contForID=0;
	contForNgram=0;
	
    /*
     * O usuário insere o número de bits n para a função de endereçamento desejados 
     * Uma hash table é criada com esse número n de buckets com o mesmo tamanho da struct hash
     */

    printf("Enter the number of bits used by the address function: ");
    //scanf("%d", &n);
    
    // #####
    n = 24;
    // #####
    
    n = pow(2,n); // Número máximo de endereços do index
    printf("Num: %d", n); 
    eleCount = n;
    hashTable = (struct hash *) calloc(eleCount, sizeof(struct hash));
    
    // ######
  //  ch = 1; 
    // ######
    
    while (1) {
        printf("\n\n1. Insert\t\t2. Search (w. list)\n");
        printf("3. Search (w. digest)\t4. Search (w. index)\n5. Display\t\t6. Exit\n");
        printf("Enter your choice:");
        scanf("%d", &ch);

        switch (ch) {

        case 1:;  //Insertion       

            createLogFile(results);
            
            /*
             *  Cria o arquivo que irá conter os nomes dos caminhos para os digests a serem processados (CreateListOfDigestFiles), 
             *  e em seguida processa cada um desses arquivos da lista criada com ProcessingDigestFile
             */ 
            
            CreateListOfDigestFiles(databasepath);
            ProcessingDIgestFile();
            if(msgs) printf("Number of n-grams inserted: %lld\n", contForNgram);
            printf("Files inserted successfully!\n");
            
            // #####
            //exit(0);
            //ch = 2 ;
            // #####
            break;

        case 2:; 

            printf("Enter the number of minimum common n-grams of a match:\n ");
            //scanf("%d",&threshold);
            
            // #####
            threshold = 1;
            // #####
            
            writeLog("Printing Results for threshold =", 0, results);
            writeIDlog(threshold, 1, results);

			printf("Searching for digests listed in cb_known_set.txt\n");
            
            /*
             *  Abre o arquivo contendo os digests dos arquivos alvo, e os busca na tabela com a função searchInHash
             */

            FILE *arq;
			char linha[LINE];
			arq = fopen(target, "r");

			if(!arq){
				fprintf(stderr, "Couldn't open %s\n", fileofDigests);
				writeLog("\nCouldn't open digest", 1, logFile);
				return 0;
			
            }

			fgets (linha, LINE, arq); //exclui a primeira linha que contém informações dos digests
			while (!feof(arq))
			{		
				if( fgets (linha, LINE, arq)!=NULL ) 
				{
					char *pos;
					if ((pos=strchr(linha, '\n')) != NULL)
						*pos = '\0';				
					searchInHash(linha, 0, threshold);
				}
			}
			fclose(arq);
            printf("All files searched successfully! Check results.txt for the results\n");
            // #####
             exit(0);
            // #####
            break;



        case 3: //Searching (digest)
            printf("Enter the number of minimum common n-grams of a match: ");
            scanf("%d",&threshold);
            printf("Enter the digest, or piece of it, to search for: ");
            scanf("%s", &digest[0]);
            searchInHash(digest, 0, threshold);
            break;



        case 4://Searching (index)
            printf("Enter the number of minimum common n-grams of a match: ");
            scanf("%d",&threshold);
            printf("Enter the index to search for: ");
            scanf("%d", &index);
            searchInHash(NULL, index, threshold);
            break;



        case 5://Display
			printf("\nLoading...:");
            display();
            break;


        case 6://Exit
            exit(0);


        default:
            printf("U have entered wrong option!!\n");
            break;
        }
    }
    
   fclose(results);
   free(IDtable);
   free(nametable);


    return 0;

}
