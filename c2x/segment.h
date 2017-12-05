#include "aref.h"

#pragma once

namespace c2
{
	enum sectiontypes {
		Code, Data, DataStrings, DataUninitialized,
	};
	struct segment
	{
		unsigned			rvabase;
		segment() : rvabase(0) {}
		virtual void			add(unsigned char a) = 0;
		virtual unsigned		get() = 0;
		virtual unsigned char*	getdata() = 0;
		virtual void			set(unsigned count) = 0;
	};
	extern segment*			segments[DataUninitialized + 1];
}
