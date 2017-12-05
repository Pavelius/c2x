#pragma once

namespace c2
{
	struct typeref
	{
		const char*		id;
		struct type*	type;
		typeref*		next;
		//
		static typeref*	create(const char* id, struct type* type);
		static void		cleanup();
		typeref*		find(struct type* type);
	};
}
