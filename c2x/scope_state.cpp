#include "c2.h"

using namespace c2;

scope_state c2::scope;

scope_state::~scope_state() {
	scope = *this;
}

symbol* scope_state::getmodule() const {
	for(auto p = this; p; p = p->parent) {
		if(p->member && p->member->istype())
			return p->member;
	}
	return 0;
}