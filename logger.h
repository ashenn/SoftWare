#ifndef __LOGGER_H__
#define __LOGGER_H__

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#define DEBUG 0
#define INFO 1
#define WARNING 2
#define ERROR 3

#include "libParse.h"

typedef struct Log{
	short lvl;	// Output min lvl
	short isInit;

	char* file;	// LogFile Path
	int f;
	
	void* init;
	void* log;

	void* (*dbg)(char* msg, ...);
	void* (*inf)(char* msg, ...);
	void* (*err)(char* msg, ...);
	void* (*war)(char* msg, ...);
	void* (*close)();

	char** lvls; // Existing debug lvl
	char** lvlColors; // Existing debug lvl colors

	ListManager* args;
} Log;


Log* logger;

Log* getLogger();
Log* initLogger();
void* closeLogger();
void* setLogLvl(int lvl);
void logg(short lvl, char* msg, va_list* args);
#endif