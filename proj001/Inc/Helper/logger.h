/*
 * logger.h
 *
 *  Created on: Nov 16, 2021
 *      Author: ashwin pillai
 */

#ifndef HELPER_LOGGER_H_
#define HELPER_LOGGER_H_

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include "main.h"

void logError(char const * const format, ...);
void logInfo(char const * const format, ...);
void logDebug(char const * const format, ...);

#endif /* HELPER_LOGGER_H_ */
