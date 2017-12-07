#include "crt.h"

#define DLL extern "C"
#define WINAPI __stdcall
#define STD_OUTPUT_HANDLE -11

DLL void* WINAPI	GetStdHandle(unsigned nStdHandle);
DLL int WINAPI		WriteFile(void* hFile, const void* lpBuffer, unsigned nNumberOfBytesToWrite, unsigned* lpNumberOfBytesWritten, void* lpOverlapped);

void printcnf(const char* text) {
	unsigned result;
	WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), text, zlen(text), &result, 0);
}

void printc(const char* format, ...) {
	char temp[4096 * 4];
	szprintv(temp, format, xva_start(format));
	printcnf(temp);
}