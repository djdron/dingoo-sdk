#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <malloc.h>
#include <setjmp.h>

extern int main(int, char**);

extern char* __to_locale_ansi(wchar_t*);

extern bool _memsys_init();
extern void _memsys_cleanup();

/*char  __to_locale_ansi_string[256];

char* __to_locale_ansi(wchar_t* inInput) {
	uintptr_t i;
	char* tempString = (char*)inInput;
	for(i = 0; (i < 255) && (tempString[i] != '\0'); i += 2)
		__to_locale_ansi_string[i] = tempString[i];
	tempString[i] = '\0';
	return tempString;
}*/

char* _app_path = NULL;

jmp_buf _libc_exit_jmp_buf;
int     _libc_exit_status;

typedef void (*_atexit_func_t)(void);
extern uintptr_t       _atexit_count;
extern _atexit_func_t* _atexit_table;

extern int _fread(void* ptr, size_t size, size_t count, FILE* stream);
extern int _fwrite(const void* ptr, size_t size, size_t count, FILE* stream);
extern int _fseek(FILE* stream, long int offset, int origin);

extern void* _memcpy(void* outDest, const void* inSrc, size_t inLength);
extern void* _memset(void* outDest, int inValue, size_t inLength);

//extern void* _malloc(size_t size);
//extern void* _realloc(void* ptr, size_t size);
//extern void  _free(void* ptr);

extern void _abort();

void *__fread, *__fwrite, *__fseek;
void *__memcpy, *__memset;
//void *__malloc, *__realloc, *__free;
void *__abort;

void* dl_patch(void* inOld, void* inNew) {
	if(((uintptr_t)inNew & 3) != 0)
		return NULL;
	if(((uintptr_t)inNew & 0xF0000000) != ((uintptr_t)inOld & 0xF0000000))
		return NULL;
	void* tempOld = (void*)(((*((uint32_t*)inOld) << 2) & 0x0FFFFFFF) | ((uint32_t)inOld & 0xF0000000));
	*((uint32_t*)inOld) = (0x08000000 | (((uint32_t)inNew >> 2) & 0x03FFFFFF));
	return tempOld;
}

char* _app_path_init(const char* inPath) {
	uintptr_t i, j;
	for(i = 0, j = 0; inPath[i] != '\0'; i++) {
		if((inPath[i] == '\\') || (inPath[i] == '/'))
			j = i + 1;
	}
	_app_path = (char*)malloc(j + 1);
	if(_app_path == NULL)
		return NULL;
	strncpy(_app_path, inPath, j);
	return _app_path;
}

int GameMain(char* respath) {
	// Patch functions to use our own fixed versions
	__fread  = dl_patch(&fread, &_fread);
	__fwrite = dl_patch(&fwrite, &_fwrite);
	__fseek  = dl_patch(&fseek, &_fseek);

	__memcpy = dl_patch(&memcpy, &_memcpy);
	__memset = dl_patch(&memset, &_memset);

	//__malloc  = dl_patch(&malloc, &_malloc);
	//__realloc = dl_patch(&realloc, &_realloc);
	//__free    = dl_patch(&free, &_free);

	__abort = dl_patch(&abort, &_abort);

	if(!_memsys_init())
		return EXIT_FAILURE;

	char* tempAnsiPath = __to_locale_ansi((wchar_t*)respath);
	uintptr_t tempAnsiLen;
	for(tempAnsiLen = 0; tempAnsiPath[tempAnsiLen] != '\0'; tempAnsiLen++);
	char* tempPath = malloc(tempAnsiLen + 1);
	if(tempPath == NULL)
		return EXIT_FAILURE;
	strcpy(tempPath, __to_locale_ansi((wchar_t*)respath));
	_app_path_init(tempPath);

	int tempOut;
	if(!setjmp(_libc_exit_jmp_buf))
		tempOut = main(1, &tempPath);
	else
		tempOut = _libc_exit_status;

	// Do atexit stuff.
	uintptr_t i;
	if(_atexit_table != NULL) {
		for(i = _atexit_count; i != 0; i--)
			_atexit_table[i - 1]();
		_atexit_count = 0;
		free(_atexit_table);
		_atexit_table = NULL;
	}

	free(_app_path);
	free(tempPath);
	_memsys_cleanup();

	// Revert patches, probably un-needed.
	dl_patch(&fread, __fread);
	dl_patch(&fwrite, __fwrite);
	dl_patch(&fseek, __fseek);

	dl_patch(&memcpy, __memcpy);
	dl_patch(&memset, __memset);

	//dl_patch(&malloc, __malloc);
	//dl_patch(&realloc, __realloc);
	//dl_patch(&free, __free);

	dl_patch(&abort, __abort);

	return tempOut;
}
