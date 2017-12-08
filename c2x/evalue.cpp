#include "c2.h"

using namespace c2;

// Различные способы адресации
// Число 12 - reg(Const), offset(12), sym(0)
// Переменная А - reg(Const), offset(0), sym(A)
// Переменная А[5] - reg(Const), offset(5*sizeof(A)), sym(A)
// Локальная переменная В - reg(Ebp), offset(0), sym(B)
// Локальная переменная В.field - reg(Ebp), offset(&field), sym(B)
// Временная локальная переменная х - reg(Ebp), offset(смещение), sym(lvalue_type)
// Значение локальной переменной В.field - reg(Eax), offset(0), sym(0)

// Формула адресации
// [reg + sym.offset + offset]

static symbol		lvalue_type[2];
evalue::plugin*		c2::backend;

evalue::evalue() : result(i32), sym(0), offset(0), reg(Const), next(0) {
}

evalue::evalue(evalue* e) : evalue() {
	next = e;
}

void evalue::set(int value) {
	this->result = i32;
	this->offset = value;
	this->sym = 0;
	this->reg = Const;
}

void evalue::set(symbol* value) {
	if(!value)
		set(0);
	else if(value->istype()) {
		this->result = value;
		this->offset = 0;
		this->sym = 0;
		this->reg = Const;
	}
	//else if(value->isconstant())
	//{
	//	set(value->value);
	//	this->result = value->result;
	//}
	else {
		this->result = value->result;
		this->offset = 0;
		this->sym = value;
		this->reg = Const;
		if(value->islocal())
			this->reg = Ebp;
	}
}

void evalue::set(const evalue& e) {
	sym = e.sym;
	result = e.result;
	offset = e.offset;
	reg = e.reg;
}

void evalue::setlvalue() {
	sym = lvalue_type;
}

void evalue::clear() {
	evalue();
}

register_s evalue::getfree() const {
	static register_s valid_registers[] = {Eax, Ebx, Ecx, Edx, Esi, Edi};
	for(auto result : valid_registers) {
		bool all_correct = true;
		for(auto p = this; p; p = p->next) {
			if(p->reg == result) {
				all_correct = false;
				break;
			}
		}
		if(all_correct)
			return result;
	}
	return Const;
}

int	evalue::localalloc(unsigned size) {
	return 0;
}

void evalue::load(register_s r) {
	if(reg != r || sym) {
		evalue e2(this);
		e2.reg = r;
		if(gen.code && backend)
			backend->operation(e2, *this, '=');
	}
	reg = r;
	offset = 0;
	sym = 0;
}

void evalue::dereference() {
	if(result->ispointer()) {
		getrvalue();
		evalue e2(this);
		e2.sym = lvalue_type;
		if(gen.code && backend)
			backend->operation(*this, e2, '=');
		offset = 0;
		sym = 0;
		result = result->dereference();
	}
}

void evalue::getrvalue() {
	if(islvalue())
		load(getfree());
}

void evalue::xchange(evalue& e) {
	iswap(reg, e.reg);
	iswap(offset, e.offset);
	iswap(sym, e.sym);
	iswap(result, e.result);
}

void evalue::cast(symbol* need_type) {
	if(result == need_type)
		return;
	if(result->ispointer() && need_type->ispointer()) {
		// Error;
	} else if(result->isnumber() && need_type->isnumber()) {
		result = need_type;
	}
	if(result != need_type)
		status(ErrorCastType1pTo2p, result ? result->id : "none", need_type ? need_type->id : "none");
}