#include "c2.h"

using namespace c2;

evalue::plugin* evalue::plugin::first;
evalue::plugin* evalue::plugin::current;

evalue::plugin::plugin(const char* progid) : progid(progid)
{
	seqlink(this);
}

evalue::plugin* evalue::plugin::find(const char* progid)
{
	for(auto p = first; p; p = p->next)
	{
		if(strcmp(p->progid, progid) == 0)
			return p;
	}
	return 0;
}