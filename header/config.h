#ifndef _H_CONFIG
#define _H_CONFIG

#include<dirent.h>
#include<stdio.h>

#define NGRAMSIZE 7
#define EKEY 4
#define INDEX 3
#define LINE 500
#define MAXLINEEXCEL 1000000

#define logFile "log.txt"
#define iDvsName "iDvsName.txt"
#define results "results.txt"
#define fileofDigests "digestFiles.txt" 
#define target "cb_target_set.txt"
#define databasepath "./database"

/*
 * Ligue este parâmetro ( = 1 ) para imprimir mensagens durante o processamento
 */
#define msgs 0

#define PATHRESULTSFILE "results.txt"

FILE *arqLog;
FILE *arqDigests;
FILE *summary_log;
FILE *iDvsNameHandle;

#endif //_H_CONFIG