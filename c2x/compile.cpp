#include "c2.h"

void c2::error(message_s id, ...) {
	if(scope.member)
		errorv(id, scope.member, scope.member->parent, xva_start(id));
	else
		errorv(id, 0, 0, xva_start(id));
}

void c2::compile(const char* url) {
	auto p = symbol::find("main", url);
	if(!p)
		p = symbol::add("main", 0);
}