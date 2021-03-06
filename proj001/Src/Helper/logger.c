/*
 * logger.c
 *
 *  Created on: Nov 16, 2021
 *      Author: ashwin pillai
 */

#include "Helper/logger.h"

int _write(int file, char *ptr, int len)
{
  int i=0;
  for(i=0 ; i<len ; i++)
    ITM_SendChar((*ptr++));

  return len;
}



static void _log(char const * const logType, char const * const format, va_list args)
{
	printf("[%s] : ",logType);
	vfprintf(stdout,format,args);
	printf("\n");
}

void logError(char const * const format, ...)
{
	va_list args;
	va_start(args,format);
	_log("ERROR",format,args);
	va_end(args);
}

void logInfo(char const * const format, ...)
{
	va_list args;
		va_start(args,format);
		_log("INFO",format,args);
		va_end(args);
}

void logDebug(char const * const format, ...)
{
	va_list args;
		va_start(args,format);
		_log("DEBUG",format,args);
		va_end(args);
}
