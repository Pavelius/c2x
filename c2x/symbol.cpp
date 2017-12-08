#include "c2.h"
#include "crt.h"

using namespace c2;

enum flag_s : char {
	Public, Static, Readed, Writed, Pseudoname
};

static symbol* standart_types[] = {i8, i16, i32, u8, u16, u32, v0};
static unsigned	pointer_size = 4;
symbol c2::i8[1]; // Байт со знаком
symbol c2::i16[1]; // Слово со знаком
symbol c2::i32[1]; // Двойное слово со знаком
symbol c2::u8[1]; // Байт без знака
symbol c2::u16[1]; // Слово без знака
symbol c2::u32[1]; // Двойное слово без знака
symbol c2::v0[1]; // Путое значение
symbol c2::types[1];
symbolset c2::symbols;
symbolset c2::pointers;
symbolset c2::modules;

void symbol::clear() {
	memset(this, 0, sizeof(*this));
}

bool symbol::istype() const {
	return this && !result;
}

bool symbol::islocal() const {
	return this && result && !section;
}

bool symbol::istypesimple() const {
	return this && !result && visibility == 0;
}

bool symbol::ismember() const {
	return this && result && !ispointer();
}

bool symbol::ispointer() const {
	return this && (id[0] == '*' && id[1] == 0);
}

bool symbol::isfunction() const {
	// Function and function parameters must have same 'declared' and 'visiblity'
	return this && declared && declared[0] == '(';
}

bool symbol::isfunctionparam() const {
	return this && visibility && visibility[0] == '(';
}

bool symbol::isfunctionparam(symbol* function) const {
	return visibility == function->declared;
}

bool symbol::isstatic() const {
	return (flags & (1 << Static)) != 0;
}

bool symbol::isnumber() const {
	return this == i8
		|| this == u8
		|| this == i16
		|| this == u16
		|| this == i32
		|| this == u32;
}

bool symbol::ispseudoname() const {
	return this && result && (flags & (1 << Pseudoname)) != 0;
}

void symbol::setpseudoname() {
	flags |= (1 << Pseudoname);
}

symbol* symbol::add() {
	return symbols.add();
}

symbol* symbol::addmodule() {
	return modules.add();
}

static symbol* addp() {
	return pointers.add();
}

symbol* symbol::reference() {
	if(!this)
		return 0;
	for(auto& e : pointers) {
		if(e.result == this && e.ispointer())
			return &e;
	}
	auto p = addp();
	p->clear();
	p->result = this;
	p->id = szdup("*");
	p->size = pointer_size;
	return p;
}

symbol* symbol::dereference() {
	if(!ispointer())
		return 0;
	return result;
}

symbol*	symbol::getchild() const {
	auto pe = symbols.end();
	auto pp = declared;
	if(istype()) {
		for(auto p = symbols.begin(); p < pe; p++) {
			if(p->visibility == visibility)
				return (symbol*)p;
		}
	} else {
		for(auto p = this + 1; p < pe; p++) {
			if(p->visibility == pp)
				return (symbol*)p;
		}
	}
	return 0;
}

symbol*	symbol::getnext() const {
	auto pe = symbols.end();
	auto pp = visibility;
	for(auto p = this + 1; p < pe; p++) {
		if(p->visibility == pp)
			return (symbol*)p;
	}
	return 0;
}

symbol*	symbol::getprevious() const {
	auto pe = symbols.begin();
	auto pp = visibility;
	for(auto p = this - 1; p >= pe; p--) {
		if(p->visibility == pp)
			return (symbol*)p;
	}
	return 0;
}

bool c2::isthis(const char* id) {
	return id[0] == 't'
		&& id[1] == 'h'
		&& id[2] == 'i'
		&& id[3] == 's'
		&& id[4] == 0;
}

bool c2::isstatic(unsigned flags) {
	return (flags & (1 << Static)) != 0;
}

unsigned c2::setstatic(unsigned flags) {
	return flags | (1 << Static);
}

unsigned c2::setpublic(unsigned flags) {
	return flags |= (1 << Public);
}

symbol* c2::findsymbol(const char* name, const char* visibility) {
	for(auto& e : symbols) {
		if(e.id == name && e.visibility == visibility)
			return &e;
	}
	return 0;
}

symbol* c2::findsymbol(const char* name, const char* visibility, bool is_function) {
	for(auto& e : symbols) {
		if(e.id == name && e.visibility == visibility && e.isfunction() == is_function)
			return &e;
	}
	return 0;
}

symbol* c2::findmodule(const char* id) {
	for(auto& e : modules) {
		if(e.id == id)
			return &e;
	}
	return 0;
}

symbol* c2::findmodulebv(const char* visiblility) {
	for(auto& e : modules) {
		if(e.visibility == visiblility)
			return &e;
	}
	return 0;
}

symbol* c2::findtype(const char* id, unsigned modifier_unsigned) {
	// Найдем стандартный тип
	for(auto e : standart_types) {
		if(e->id == id)
			return e;
	}
	// Маленькие типы
	if(strcmp(id, "bool") == 0 || strcmp(id, "char") == 0) {
		if(modifier_unsigned)
			return u8;
		return i8;
	}
	// Короткое число
	if(strcmp(id, "short") == 0) {
		if(modifier_unsigned)
			return u16;
		return i16;
	}
	// Целое число
	if(strcmp(id, "int") == 0) {
		if(modifier_unsigned)
			return u32;
		return i32;
	}
	return 0;
}

symbol* c2::findtype(const char* id, const char* visibility) {
	for(auto& e : symbols) {
		if(e.visibility == visibility && e.id == id && e.ispseudoname() && !e.result->istypesimple())
			return &e;
	}
	return 0;
}

void basetype(symbol* p, const char* name, unsigned size) {
	p->id = szdup(name);
	p->size = size;
}

void symbol::initialize() {
	basetype(i8, "int8", 1);
	basetype(i16, "int16", 2);
	basetype(i32, "int32", 4);
	basetype(u8, "uint8", 1);
	basetype(u16, "uint16", 2);
	basetype(u32, "uint32", 4);
	basetype(v0, "void", 0);
}