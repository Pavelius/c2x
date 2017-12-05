#include "crt.h"
#include "evalue.h"
#include "segment.h"

using namespace c2;

static const char* reg_name32[] = {"eax", "ebx", "ecx", "edx", "esi", "edi", "esp", "ebp"};

struct backend_asm : public evalue::plugin
{

	void printz(sectiontypes type, const char* format)
	{
		for(auto p = format; *p; p++)
			segments[type]->add((unsigned char)*p);
	}

	void print(sectiontypes type, const char* format, ...)
	{
		char temp[4096];
		szprintv(temp, format, xva_start(format));
		printz(type, temp);
	}

	void operand(registers e1)
	{
		print(Code, reg_name32[e1]);
	}

	void linefeed()
	{
		printz(Code, "\n");
	}

	void operand(evalue& e1)
	{
		if(e1.sym)
		{
			print(Code, "[");
			if(e1.reg < Const)
			{
				operand(e1.reg);
				print(Code, "%+1i", e1.offset + e1.sym->value);
			}
			else
				print(Code, "%1i", e1.offset + e1.sym->value);
			print(Code, "]");
		}
		else if(e1.reg == Const)
			print(Code, "%1i", e1.offset);
		else
			operand(e1.reg);
	}

	void op(const char* name, evalue& e1, evalue& e2)
	{
		print(Code, name);
		print(Code, " ");
		operand(e1);
		print(Code, ", ");
		operand(e2);
		linefeed();
	}

	void op(const char* name, registers r1, evalue& e2)
	{
		print(Code, name);
		print(Code, " ");
		operand(r1);
		print(Code, ", ");
		operand(e2);
		linefeed();
	}

	void op(const char* name, evalue& e1)
	{
		print(Code, name);
		print(Code, " ");
		operand(e1);
		linefeed();
	}

	void retproc(type* member) override
	{
		if(member->size)
		{
			evalue e1;
			evalue e2;
			e1.set(Esp);
			e2.set(member->size);
			op("add", e1, e2);
		}
		print(Code, "ret");
		linefeed();
	}

	void epilogue(type* module, type* member) override
	{
		print(Code, "ENDP");
		linefeed();
		linefeed();
	}

	void prologue(type* module, type* member) override
	{
		print(Code, "%1 PROC", member->id);
		linefeed();
	}

	void operation(evalue& e1, evalue& e2, char t1, char t2) override
	{
		switch(t1)
		{
		case '=':
			op("mov", e1, e2);
			break;
		case '+':
			op("add", e1, e2);
			break;
		case '-':
			op("sub", e1, e2);
			break;
		}
	}

	void operation(evalue& e1, char t1)
	{
		switch(t1)
		{
		case '!':
			op("neg", e1);
			break;
		case 'd':
			op("mov", Eax, e1);
			break;
		}
	}

	int label(int a)
	{
		return a;
	}

	backend_asm() : plugin("asm")
	{
	}

};

static backend_asm instance;