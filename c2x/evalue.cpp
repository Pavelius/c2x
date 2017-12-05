#include "crt.h"
#include "evalue.h"
#include "genstate.h"

using namespace c2;

// ��������� ������� ���������
// ����� 12 - reg(Const), offset(12), sym(0)
// ���������� � - reg(Const), offset(0), sym(A)
// ���������� �[5] - reg(Const), offset(5*sizeof(A)), sym(A)
// ��������� ���������� � - reg(Ebp), offset(0), sym(B)
// ��������� ���������� �.field - reg(Ebp), offset(&field), sym(B)
// ��������� ��������� ���������� � - reg(Ebp), offset(��������), sym(lvalue_type)
// �������� ��������� ���������� �.field - reg(Eax), offset(0), sym(0)

// ������� ���������
// [reg + sym.offset + offset]

static type			lvalue_type[2];
evalue::plugin*		c2::backend;

evalue::evalue() : result(type::i32), sym(0), offset(0), reg(Const), next(0)
{
}

evalue::evalue(evalue* e) : evalue()
{
	next = e;
}

void evalue::set(int value)
{
	this->result = type::i32;
	this->offset = value;
	this->sym = 0;
	this->reg = Const;
}

void evalue::set(type* value)
{
	if(!value)
		set(0);
	else if(value->istype())
	{
		this->result = value;
		this->offset = 0;
		this->sym = 0;
		this->reg = Const;
	}
	else if(value->isconstant())
	{
		set(value->value);
		this->result = value->result;
	}
	else
	{
		this->result = value->result;
		this->offset = 0;
		this->sym = value;
		this->reg = Const;
		if(value->islocal())
			this->reg = Ebp;
	}
}

void evalue::set(const evalue& e)
{
	sym = e.sym;
	result = e.result;
	offset = e.offset;
	reg = e.reg;
}

void evalue::setlvalue()
{
	sym = lvalue_type;
}

void evalue::clear()
{
	evalue();
}

registers evalue::getfree() const
{
	static registers valid_registers[] = {Eax, Ebx, Ecx, Edx, Esi, Edi};
	for(auto result : valid_registers)
	{
		bool all_correct = true;
		for(auto p = this; p; p = p->next)
		{
			if(p->reg == result)
			{
				all_correct = false;
				break;
			}
		}
		if(all_correct)
			return result;
	}
	return Const;
}

int	evalue::localalloc(unsigned size)
{
	return 0;
}

void evalue::load(registers r)
{
	if(reg != r || sym)
	{
		evalue e2(this);
		e2.reg = r;
		if(gen.code)
			backend->operation(e2, *this, '=');
	}
	reg = r;
	offset = 0;
	sym = 0;
}

void evalue::dereference()
{
	if(result->ispointer())
	{
		getrvalue();
		evalue e2(this);
		e2.sym = lvalue_type;
		if(gen.code)
			backend->operation(*this, e2, '=');
		offset = 0;
		sym = 0;
		result = result->dereference();
	}
}

void evalue::getrvalue()
{
	if(islvalue())
		load(getfree());
}

void evalue::xchange(evalue& e)
{
	iswap(reg, e.reg);
	iswap(offset, e.offset);
	iswap(sym, e.sym);
	iswap(result, e.result);
}

void evalue::cast(type* need_type)
{
	if(result == need_type)
		return;
	if(result->ispointer() && need_type->ispointer())
	{
		// Error;
	}
	else if(result->isnumber() && need_type->isnumber())
	{
		result = need_type;
	}
	if(result != need_type)
		status(ErrorCastType1pTo2p, result->id, need_type->id);
}