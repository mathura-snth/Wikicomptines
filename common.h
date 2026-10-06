/* Mathura Santhalingam
 * Je déclare qu'il s'agit de mon propre travail. */

#ifndef __COMMON_H
#define __COMMON_H

#include <errno.h>

// Stringification
#define _STR(x) #x     // 2. On va stringifier le résultat.
#define STR(x) _STR(x) // 1. On va étendre notre macro grâce à un premier appel.

// Primitive color codes
#define BOLD "\e[1m"
#define CLR "\e[0m"

// Debug color codes
#define DBG "\e[34m"
#define INF "\e[32m"
#define WRN "\e[33m"
#define ERR "\e[31m"
#define FTL BOLD ERR
#define LOG "\e[37m"
#define WHT "\e[97m"

// Debug info
#define _DBG(type, name, file_color, str_color, str) type name CLR "\t " file_color __FILE__ ":" STR(__LINE__) CLR str_color "\t > " str "\n" CLR
#define DEBUG(str) _DBG(DBG, "DEBUG", LOG, LOG, str)
#define INFO(str) _DBG(INF, "INFO", LOG, WHT, str)
#define ERROR(str) _DBG(ERR, "ERROR", WHT, WHT, str)
#define FATAL(str) _DBG(FTL, "FATAL", FTL, FTL, str)

// Debug macros
#define _PRTDBG(str, ...) fprintf(stderr, DEBUG(str), ##__VA_ARGS__)
#define _PRTINF(str, ...) fprintf(stderr, INFO(str), ##__VA_ARGS__)
#define _PRTERR(str, ...) fprintf(stderr, ERROR(str), ##__VA_ARGS__)
#define _PRTFTL(str, ...) fprintf(stderr, FATAL(str), ##__VA_ARGS__)
#define _ERRNO(str) _PRTERR(str ": %s", strerror(errno))
#define _ERRNOFTL(str) _PRTFTL(str ": %s", strerror(errno))

// Log levels
enum log_level {
	QUIET = 0,
	FATAL = 1,
	ERROR = 2,
	INFO = 3,
	DEBUG = 4,
};

#define PRTDBG(str, ...) if (loglvl >= DEBUG) _PRTDBG(str, ##__VA_ARGS__)
#define PRTINF(str, ...) if (loglvl >= INFO) _PRTINF(str, ##__VA_ARGS__)
#define PRTERR(str, ...) if (loglvl >= ERROR) _PRTERR(str, ##__VA_ARGS__)
#define PRTFTL(str, ...) if (loglvl >= FATAL) _PRTFTL(str, ##__VA_ARGS__)
#define ERRNO(str) if (loglvl >= ERROR) _ERRNO(str)
#define ERRNOFTL(str) if (loglvl >= FATAL) _ERRNOFTL(str)

// Test macros
#define BGNTEST(n) printf("TEST " n " : ")
#define TEST(t) if (t) printf("."); else printf(ERR "F" CLR)
#define ENDTEST printf("\n\n")

#endif
