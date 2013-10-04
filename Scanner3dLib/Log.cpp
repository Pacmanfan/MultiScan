
#include "log.h"

#include <stdio.h>
#ifndef STDARG_H
#include <stdarg.h>
#endif

#ifndef TIME_H
#include <time.h>
#endif
#include <string.h>
static FILE *logfile=0;

static char szLogfile[256]="game.log"; // static so if dir change the log is still ok...
char *GetAppPath();
void SetLogPath(char *path){
	strcpy(&szLogfile[0],path);
	strcpy(&szLogfile[0] + strlen(&szLogfile[0]),"game.log");
}
void Log(char const *fmt,...)
	{
	static int readbefore=0;
	if (! readbefore)
		{
                //sprintf(szLogfile,"game.log");
                logfile=fopen(szLogfile,"wt");
		if (logfile){
			fprintf(logfile,"******************\n%9d : Log System initted\n",clock());
			fclose(logfile);
			}
		readbefore=1;
		}
	logfile=fopen(szLogfile,"at");
	if(logfile){
		fprintf(logfile,"%9d : ",clock());
		va_list arglist;
		va_start(arglist,fmt);
		vfprintf(logfile,fmt,arglist);
		fprintf(logfile,"\n");
		va_end(arglist);
		fclose(logfile);
	}
	}
