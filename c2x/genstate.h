#pragma once

namespace c2
{
	struct genstate
	{
		bool			code;
		bool			unique;
		bool			methods;
		//bool			size;
		genstate();
		~genstate();
	};
	extern genstate		gen;
}