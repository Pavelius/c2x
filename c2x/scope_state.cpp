#include "c2.h"

using namespace c2;

scope_state c2::scope;

scope_state::scope_state()
{
	*this = scope;
	scope.parent = this;
}

scope_state::~scope_state()
{
	scope = *this;
}