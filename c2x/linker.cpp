#include "c2.h"
#include "io.h"

using namespace c2;

bool section::write(const char* url)
{
	io::file file(url, StreamWrite | StreamText);
	if(!file)
		return false;
	auto count = get();
	auto pdata = getdata();
	file.write(pdata, count);
	return true;
}

void c2::link(const char* id)
{
	char temp[260];
	auto ps = section::find(section_code);
	if(!ps)
		return;
	ps->write(szurl(temp, c2::urls::output, id, "txt"));
}