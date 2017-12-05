#include "crt.h"
#include "io.h"

unsigned io::stream::getsize()
{
	unsigned s = seek(0, SeekCur);
	unsigned r = seek(0, SeekEnd);
	seek(s, SeekSet);
	return r;
}

void* loadb(const char* url, int* size, int additional)
{
	void* p = 0;
	if(size)
		*size = 0;
	if(!url || url[0] == 0)
		return 0;
	io::file file(url, StreamRead);
	if(!file)
		return 0;
	int s = file.seek(0, SeekEnd) + additional;
	file.seek(0, SeekSet);
	p = new char[s];
	memset(p, 0, s);
	file.read(p, s);
	if(size)
		*size = s;
	return p;
}

char* loadt(const char* url, int* size)
{
	int s1;
	if(size)
		*size = 0;
	unsigned char* p = (unsigned char*)loadb(url, &s1, 1);
	if(!p)
		return 0;
	if(p[0] == 0xEF && p[1] == 0xBB && p[2] == 0xBF)
	{
		// UTF8
		// Перекодируем блок через декодировщик. Может быть только меньше,
		// так как системная кодировка ANSI
		szencode((char*)p, s1, metrics::code, (char*)p + 3, s1, CPUTF8);
	}
	if(size)
		*size = s1;
	return (char*)p;
}