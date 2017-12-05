#include "autogrow.h"
#include "type.h"

using namespace c2;

static autogrow<typeref, 128>	globals;

typeref* typeref::create(const char* id, struct type* type)
{
	auto p = globals.add();
	p->id = id;
	p->type = type;
	p->next = 0;
	return p;
}

void typeref::cleanup()
{
	globals.clear();
	memset(&globals, 0, sizeof(globals));
}

typeref* typeref::find(struct type* type)
{
	if(!this)
		return 0;
	for(auto p = this; p; p = p->next)
	{
		if(p->type == type)
			return p;
	}
	return 0;
}