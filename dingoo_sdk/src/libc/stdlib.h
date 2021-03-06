#ifndef __stdlib_h__
#define __stdlib_h__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>
#include <malloc.h>
#include <string.h>

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

typedef struct {
	long int quot;
	long int rem;
} ldiv_t;

typedef struct {
	int quot;
	int rem;
} div_t;

extern void abort() __attribute__ ((noreturn));
extern void exit(int status) __attribute__ ((noreturn));
extern int  atexit(void (*func)(void));

extern int      abs(int n);
extern long int labs(long int n);

extern div_t  div(int numerator, int denominator);
extern ldiv_t ldiv(long int numerator, long int denominator);

#define RAND_MAX 0x7FFFFFFF

extern void     srand(unsigned int inSeed);
extern int      rand();

extern int       atoi(const char* str);
extern long      atol(const char* str);
extern long long atoll(const char* str);
extern double    atof(const char* str);

extern void qsort(void* base, size_t num, size_t size, int(* comparator) (const void*, const void*));

extern double strtod(const char *string, char **endPtr);

extern char * realpath(const char * file_name, char * resolved_name);

extern long strtol(const char *nptr, char **endptr, int ibase);
extern unsigned long strtoul(const char *nptr, char **endptr, int ibase);

#ifdef __cplusplus
}
#endif

#endif
