#include "c2.h"

using namespace c2;

genstate c2::gen;

genstate::genstate() : genstate(gen)
{
}

genstate::~genstate()
{
	gen = *this;
}