#include "crt.h"
#include "io.h"
#include "files.h"
#include "segment.h"
#include "type.h"

using namespace c2;

void write_to_text(sectiontypes type, const char* url)
{
	io::file file(url, StreamWrite);
	if(!file)
		return;
	auto ps = segments[type];
	auto count = ps->get();
	auto pdata = ps->getdata();
	file.write(pdata, count);
}

void c2::type::link(const char* id)
{
	char temp[260];
	szurl(temp, c2::urls::output, id, "txt");
	write_to_text(Code, temp);
}