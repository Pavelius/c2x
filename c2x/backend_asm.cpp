#include "c2.h"

using namespace c2;

static const char* reg_name32[] = {"eax", "ebx", "ecx", "edx", "esi", "edi", "esp", "ebp"};

struct backend_asm : public evalue::plugin {

	void printz(section* sec, const char* format) {
		for(auto p = format; *p; p++)
			sec->add((unsigned char)*p);
	}

	void print(const char* section_name, const char* format, ...) {
		char temp[4096];
		szprintv(temp, format, xva_start(format));
		printz(section::find(section_name), temp);
	}

	void operand(register_s e1) {
		print(section_code, reg_name32[e1]);
	}

	void linefeed() {
		printz(section::find(section_code), "\n");
	}

	void operand(evalue& e1) {
		if(e1.sym) {
			print(section_code, "[");
			if(e1.reg < Const) {
				operand(e1.reg);
				print(section_code, "%+1i", e1.offset + e1.sym->value);
			} else
				print(section_code, "%1i", e1.offset + e1.sym->value);
			print(section_code, "]");
		} else if(e1.reg == Const)
			print(section_code, "%1i", e1.offset);
		else
			operand(e1.reg);
	}

	void op(const char* name, evalue& e1, evalue& e2) {
		print(section_code, name);
		print(section_code, " ");
		operand(e1);
		print(section_code, ", ");
		operand(e2);
		linefeed();
	}

	void op(const char* name, register_s r1, evalue& e2) {
		print(section_code, name);
		print(section_code, " ");
		operand(r1);
		print(section_code, ", ");
		operand(e2);
		linefeed();
	}

	void op(const char* name, evalue& e1) {
		print(section_code, name);
		print(section_code, " ");
		operand(e1);
		linefeed();
	}

	void retproc(symbol* member) override {
		if(member->size) {
			evalue e1;
			evalue e2;
			e1.set(Esp);
			e2.set(member->size);
			op("add", e1, e2);
		}
		print(section_code, "ret");
		linefeed();
	}

	void epilogue(symbol* module, symbol* member) override {
		print(section_code, "ENDP");
		linefeed();
		linefeed();
	}

	void prologue(symbol* module, symbol* member) override {
		print(section_code, "%1 PROC", member->id);
		linefeed();
	}

	void operation(evalue& e1, evalue& e2, char t1, char t2) override {
		switch(t1) {
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

	void operation(evalue& e1, char t1) {
		switch(t1) {
		case '!':
			op("neg", e1);
			break;
		case 'd':
			op("mov", Eax, e1);
			break;
		}
	}

	int label(int a) {
		return a;
	}

	backend_asm() : plugin("asm") {
	}

};

static backend_asm instance;