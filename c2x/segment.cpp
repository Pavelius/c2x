#include "segment.h"
#include "genstate.h"

using namespace c2;

segment* c2::segments[DataUninitialized + 1];

struct data_segment : segment, public aref<unsigned char>
{
	
	unsigned rvabase;

	data_segment()
	{
		initialize();
	}

	void add(unsigned char a) override
	{
		if(!gen.code)
			return;
		reserve(count + 1);
		data[count++] = a;
	}

	unsigned get() override
	{
		return count;
	}

	void set(unsigned count) override
	{
		if(!count && this->count == 0)
			return;
		reserve(count);
	}

	unsigned char* getdata() override
	{
		return data;
	}

};

struct bbs_segment : segment
{

	unsigned count;

	bbs_segment() : count(0)
	{
	}

	void add(unsigned char a) override
	{
		if(!gen.code)
			return;
		count++;
	}

	unsigned get() override
	{
		return count;
	}

	unsigned char* getdata() override
	{
		return 0;
	}

	void set(unsigned count) override
	{
		this->count = count;
	}

};

static data_segment data_initialized;
static data_segment data_string;
static data_segment code;
static bbs_segment data_uninitialized;

void segments_cleanup()
{
	segments[Code] = &code;
	segments[Data] = &data_initialized;
	segments[DataUninitialized] = &data_initialized;
	segments[DataStrings] = &data_string;
	// Clear all sections
	for(auto p : segments)
		p->set(0);
}