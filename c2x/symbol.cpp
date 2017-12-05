#include "c2.h"
#include "crt.h"

using namespace c2;

adat<symbol, 8096> symbols;
symbol	c2::i8[1]; // Байт со знаком
symbol	c2::i16[1]; // Слово со знаком
symbol	c2::i32[1]; // Двойное слово со знаком
symbol	c2::u8[1]; // Байт без знака
symbol	c2::u16[1]; // Слово без знака
symbol	c2::u32[1]; // Двойное слово без знака
symbol	c2::v0[1]; // Путое значение
static symbol* standart_types[] = {i8, i16, i32, u8, u16, u32, v0};
static symbol types[1];
static symbol types_ref[1];
static symbol types_platform[1];
static unsigned pointer_size = 4;

void symbol::clear() {
	memset(this, 0, sizeof(*this));
}

bool symbol::istype() const {
	return this && (parent == types || parent == types_platform || parent == types_ref);
}

bool symbol::ismember() const {
	return this && !(parent == types || parent == types_platform || parent == types_ref);
}

bool symbol::ispointer() const {
	return this && (parent == types_ref);
}

bool symbol::ismethod() const {
	return ismember() && is(flags, Function);
}

bool symbol::ismethodparam() const {
	return this && is(flags, Parameter);
}

bool symbol::islocal() const {
	return this && !parent;
}

bool symbol::isnumber() const {
	return this == i8
		|| this == u8
		|| this == i16
		|| this == u16
		|| this == i32
		|| this == u32;
}

symbol* symbol::find(const char* name, const char* visibility) {
	for(auto& e : symbols) {
		if(e.name == name && e.visibility == visibility)
			return &e;
	}
	return 0;
}

symbol* symbol::find(const char* name) {
	for(auto psc = &scope; psc; psc = psc->parent) {
		auto sym = find(name, psc->visibility);
		if(sym)
			return sym;
	}
	return 0;
}

static symbol* addsymbol() {
	for(auto& e : symbols) {
		if(!e)
			return &e;
	}
	return symbols.add();
}

symbol* symbol::add(const char* name, symbol* member) {
	auto p = find(name, scope.visibility);
	if(p) {
		if(status.error_double_identifier)
			error(Error1p2pAlreadyDefined, "symbol", name);
		return p;
	}
	p = addsymbol();
	if(!p)
		return 0;
	if(!member)
		member = scope.member;
	p->clear();
	p->name = name;
	p->parent = member;
	p->visibility = scope.visibility;
	return p;
}

symbol* symbol::reference() {
	if(!this)
		return 0;
	for(auto& e : symbols) {
		if(e.result == this && e.parent == types_ref)
			return &e;
	}
	auto p = addsymbol();
	p->clear();
	p->result = this;
	p->name = szdup("*");
	p->parent = types_ref;
	p->size = pointer_size;
	p->count = 1;
	return p;
}

symbol* symbol::dereference() {
	if(!ispointer())
		return 0;
	return result;
}