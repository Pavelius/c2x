///////////////////////////////////////////////////////////////
// SIMLE DISASSEMBLER
//
// Copyright (c) 2012 Pavel Chistiakov
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include "crt.h"
#include "dasm.h"

namespace dasm
{

	struct icode
	{
		op_s		code;
		op_s		p1;
		op_s		p2;
		op_s		p3;
		icode*		group;
		//
		bool		modrm() const { return p1 >= R8 || p2 >= R8 || p3 >= R8; }
		bool		prefix() const { return p2 == PREFIX; }
		int			rm() const { return (p1 == RM32 || p1 == RM8) ? 0 : (p2 == RM32 || p2 == RM8) ? 1 : 2; }
		op_s		size() const { return (p1 == R8 || p2 == R8 || p3 == R8) ? R8 : R32; }
	};

	static const char* op_names[] =
	{
		"<none>",
		"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi",
		"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh",
		"ax", "cx", "dx", "bx", "sp", "bp", "si", "di",
		"es", "cs", "ss", "ds", "fs", "gs",
		"mm0", "mm1", "mm2", "mm3", "mm4", "mm5", "mm6", "mm7",
		"st0", "st1", "st2", "st3", "st4", "st5", "st6", "st7",
		// opcodes
		"add", "or", "adc", "sbb", "and", "sub", "xor", "cmp",
		"rol", "ror", "rcl", "rcr", "shl", "sal", "shr", "sar",
		"inc", "dec", "not", "neg", "mul", "imul", "div", "idiv",
		"push", "pop", "pusha", "pushad", "popa", "popad", "pushf", "pushfd", "popf", "popd",
		"bound", "arpl", "sahf", "lahf", "salc",
		"aaa", "aas", "daa", "das", "aam", "aad", "amx", "adx",
		"cdq", "cwd",
		"jo", "jno", "jb", "jae", "jz", "jnz", "jbe", "ja", "js", "jns", "jp", "jnp", "jl", "jge", "jle", "jg",
		"repz", "repnz", "lock", "jecxz", "loopnz", "loopz", "loop",
		"lds", "les", "test", "xchg", "mov", "lea",
		"nop", "wait", "xlatb", "in", "out",
		"call", "call far", "jmp", "jmp far", "ret", "retf", "leave", "enter", "int", "into", "iret",
		"cld", "std", "cli", "sti", "clc", "stc", "cmc", "hlt",
		"insb", "insd", "outsb", "outsd", "movsb", "movsd", "stosb", "stosd", "lodsb", "lodsd", "cmpsb", "cmpsâ", "cmpsb", "cmpsd",
		"sldt", "lldt", "str", "verr", "verw", "sgdt", "sidt", "lgdt", "smsw", "lmsw", "invlpg", "lar", "lsl", "clts", "invd", "wbinvd", "ltr",
		"ldtr", "tr",
		"seto", "setno", "setb", "setae", "setz", "setnz", "setbe", "seta", "sets", "setns", "setp", "setnp", "setl", "setge", "setle", "setg",
	};

	static icode top00[8] = {
		{SLDT, RM32, LDTR},
		{STR, RM32, TR},
		{LLDT, LDTR, RM32},
		{LTR, TR, RM32},
		{VERR, RM32},
		{VERW, RM32},
	};
	static icode top01[8] = {
		{SLDT, RM32, LDTR},
		{STR, RM32, TR},
		{LLDT, LDTR, RM32},
		{LTR, TR, RM32},
		{VERR, RM32},
		{VERW, RM32},
	};
	static icode opcodex[256] = {
		{R16, NONE, NONE, NONE, top00}, // 0x00
		{R16, NONE, NONE, NONE, top01},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE}, // 0x08
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE}, // 0x10
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE}, // 0x18
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE}, // 0x20
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE}, // 0x28
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE}, // 0x30
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE}, // 0x38
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE}, // 0x40
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE}, // 0x48
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE}, // 0x50
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE}, // 0x58
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE}, // 0x60
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE}, // 0x68
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE}, // 0x70
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE}, // 0x78
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{JO, REL32}, // 0x80
		{JNO, REL32},
		{JB, REL32},
		{JAE, REL32},
		{JZ, REL32},
		{JNZ, REL32},
		{JBE, REL32},
		{JA, REL32},
		{JS, REL32}, // 0x88
		{JNS, REL32},
		{JP, REL32},
		{JNP, REL32},
		{JL, REL32},
		{JGE, REL32},
		{JLE, REL32},
		{JG, REL32},
		{SETO, RM8}, // 0x90
		{SETNO, RM8},
		{SETB, RM8},
		{SETAE, RM8},
		{SETZ, RM8},
		{SETNZ, RM8},
		{SETBE, RM8},
		{SETA, RM8},
		{SETS, RM8}, // 0x98
		{SETNS, RM8},
		{SETP, RM8},
		{SETNP, RM8},
		{SETL, RM8},
		{SETGE, RM8},
		{SETLE, RM8},
		{SETG, RM8},
		{NONE}, // 0xA0
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE}, // 0xA8
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE}, // 0xB0
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE}, // 0xB8
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{MOVSB, R32, RM8},
		{NONE},
	};
	static icode group1[8] = {
		{ADD},
		{OR},
		{ADC},
		{SBB},
		{AND},
		{SUB},
		{XOR},
		{CMP},
	};
	static icode	group2[8] = {
		{ROL},
		{ROR},
		{RCL},
		{RCR},
		{SHL},
		{SHR},
		{SAL},
		{SAR},
	};
	static icode	group3[8] = {
		{TEST},
		{TEST},
		{NOT},
		{NEG},
		{MUL},
		{IMUL},
		{DIV},
		{IDIV},
	};
	static icode	group4[8] = {
		{INC},
		{DEC},
		{CALL},
		{CALLF},
		{JMP},
		{JMPF},
		{PUSH},
		{NONE},
	};
	static icode	opcodes[256] = {
		{ADD, RM8, R8}, // 0x00
		{ADD, RM32, R32},
		{ADD, R8, RM8},
		{ADD, R32, RM32},
		{ADD, AL, IMM8},
		{ADD, EAX, IMM32},
		{PUSH, ES},
		{POP, ES},
		{OR, RM8, R8}, // 0x08
		{OR, RM32, R32},
		{OR, R8, RM8},
		{OR, R32, RM32},
		{OR, AL, IMM8},
		{OR, EAX, IMM32},
		{PUSH, CS},
		{IMM8, NONE, NONE, NONE, opcodex},
		{ADC, RM8, R8}, // 0x10
		{ADC, RM32, R32},
		{ADC, R8, RM8},
		{ADC, R32, RM32},
		{ADC, AL, IMM8},
		{ADC, EAX, IMM32},
		{PUSH, SS},
		{POP, SS},
		{SBB, RM8, R8}, // 0x18
		{SBB, RM32, R32},
		{SBB, R8, RM8},
		{SBB, R32, RM32},
		{SBB, AL, IMM8},
		{SBB, EAX, IMM32},
		{PUSH, DS},
		{POP, DS},
		{AND, RM8, R8}, // 0x20
		{AND, RM32, R32},
		{AND, R8, RM8},
		{AND, R32, RM32},
		{AND, AL, IMM8},
		{AND, EAX, IMM32},
		{SEG, ES},
		{DAA},
		{SUB, RM8, R8}, // 0x28
		{SUB, RM32, R32},
		{SUB, R8, RM8},
		{SUB, R32, RM32},
		{SUB, AL, IMM8},
		{SUB, EAX, IMM32},
		{SEG, CS, PREFIX},
		{DAS},
		{XOR, RM8, R8}, // 0x30
		{XOR, RM32, R32},
		{XOR, R8, RM8},
		{XOR, R32, RM32},
		{XOR, AL, IMM8},
		{XOR, EAX, IMM32},
		{SEG, SS, PREFIX},
		{AAA},
		{CMP, RM8, R8}, // 0x38
		{CMP, RM32, R32},
		{CMP, R8, RM8},
		{CMP, R32, RM32},
		{CMP, AL, IMM8},
		{CMP, EAX, IMM32},
		{SEG, DS, PREFIX},
		{AAS},
		{INC, EAX}, // 0x40
		{INC, ECX},
		{INC, EDX},
		{INC, EBX},
		{INC, ESP},
		{INC, EBP},
		{INC, ESI},
		{INC, EDI},
		{DEC, EAX}, // 0x48
		{DEC, ECX},
		{DEC, EDX},
		{DEC, EBX},
		{DEC, ESP},
		{DEC, EBP},
		{DEC, ESI},
		{DEC, EDI},
		{PUSH, EAX}, // 0x50
		{PUSH, ECX},
		{PUSH, EDX},
		{PUSH, EBX},
		{PUSH, ESP},
		{PUSH, EBP},
		{PUSH, ESI},
		{PUSH, EDI},
		{POP, EAX}, // 0x58
		{POP, ECX},
		{POP, EDX},
		{POP, EBX},
		{POP, ESP},
		{POP, EBP},
		{POP, ESI},
		{POP, EDI},
		{PUSHAD}, // 0x60
		{POPAD},
		{BOUND, IMM16, IMM32},
		{ARPL, RM32, R32},
		{SEG, FS, PREFIX},
		{SEG, GS, PREFIX},
		{PREFIX, NONE, NONE},
		{PREFIX, NONE, NONE},
		{PUSH, IMM32}, // 0x68
		{IMUL, R32, RM32, IMM32},
		{PUSH, IMM8},
		{IMUL, R32, RM32, IMM8},
		{INSB},
		{INSD},
		{OUTSB},
		{OUTSD},
		{JO, REL8}, // 0x70
		{JNO, REL8},
		{JB, REL8},
		{JAE, REL8},
		{JZ, REL8},
		{JNZ, REL8},
		{JBE, REL8},
		{JA, REL8},
		{JS, REL8}, // 0x78
		{JNS, REL8},
		{JP, REL8},
		{JNP, REL8},
		{JL, REL8},
		{JGE, REL8},
		{JLE, REL8},
		{JG, REL8},
		{R8, RM8, IMM8, NONE, group1}, // 0x80
		{R8, RM32, IMM32, NONE, group1},
		{R8, RM8, IMM8, NONE, group1},
		{R8, RM32, IMM8, NONE, group1},
		{TEST, RM8, R8},
		{TEST, RM32, R32},
		{XCHG, RM8, R8},
		{XCHG, RM32, R32},
		{MOV, RM8, R8}, // 0x88
		{MOV, RM32, R32},
		{MOV, R8, RM8},
		{MOV, R32, RM32},
		{MOV, R32, SEG},
		{LEA, R32, RM32},
		{MOV, SEG, R32},
		{POP, RM32},
		{NOP}, // 0x90
		{XCHG, EAX, ECX},
		{XCHG, EAX, EDX},
		{XCHG, EAX, EBX},
		{XCHG, EAX, ESP},
		{XCHG, EAX, EBP},
		{XCHG, EAX, ESI},
		{XCHG, EAX, EDI},
		{CWD}, // 0x98
		{CDQ},
		{CALL, ADDR},
		{WAIT},
		{PUSHFD},
		{POPFD},
		{SAHF},
		{LAHF},
		{MOV, AL, MEM}, // 0xA0
		{MOV, EAX, MEM},
		{MOV, MEM, AL},
		{MOV, MEM, EAX},
		{MOVSB},
		{MOVSD},
		{CMPSB},
		{CMPSD},
		{TEST, AL, IMM8}, // 0xA8
		{TEST, EAX, IMM32},
		{STOSB},
		{STOSD},
		{LODSB},
		{LODSD},
		{SCASB},
		{SCASD},
		{MOV, AL, IMM8}, // 0xB0
		{MOV, CL, IMM8},
		{MOV, DL, IMM8},
		{MOV, BL, IMM8},
		{MOV, AH, IMM8},
		{MOV, CH, IMM8},
		{MOV, DL, IMM8},
		{MOV, BL, IMM8},
		{MOV, EAX, IMM32}, // 0xB8
		{MOV, ECX, IMM32},
		{MOV, EDX, IMM32},
		{MOV, EBX, IMM32},
		{MOV, ESP, IMM32},
		{MOV, EBP, IMM32},
		{MOV, ESI, IMM32},
		{MOV, EDI, IMM32},
		{R8, RM8, IMM8, NONE, group2}, // 0xC0
		{R8, RM32, IMM8, NONE, group2},
		{RET, IMM16},
		{RET},
		{LES, R8, RM8},
		{LDS, R32, RM32},
		{MOV, RM8, IMM8},
		{MOV, RM32, IMM32},
		{ENTER, IMM16, IMM8}, // 0xC8
		{LEAVE},
		{RETF, IMM16},
		{RETF},
		{INT, C3},
		{INT, IMM8},
		{INTO},
		{IRET},
		{R8, RM8, C1, NONE, group2}, // 0xD0
		{R8, RM32, C1, NONE, group2},
		{R8, RM8, CL, NONE, group2},
		{R8, RM32, CL, NONE, group2},
		{AMX, IMM8},
		{ADX, IMM8},
		{SALC},
		{XLAT},
		{NONE}, // 0xD8
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{NONE},
		{LOOPNZ, REL8}, // 0xE0
		{LOOPZ, REL8},
		{LOOP, REL8},
		{JCXZ, REL8},
		{IN, AL, IMM8},
		{IN, EAX, IMM8},
		{OUT, AL, IMM8},
		{OUT, EAX, IMM8},
		{CALL, REL32}, // 0xE8
		{JMP, REL32},
		{JMPF, REL32},
		{JMP, REL8},
		{IN, AL, DX},
		{IN, EAX, DX},
		{OUT, AL, DX},
		{OUT, EAX, DX},
		{LOCK, NONE, PREFIX}, // 0xF0
		{INT, C1},
		{REPNZ, NONE, PREFIX},
		{REPZ, NONE, PREFIX},
		{HLT},
		{CMC},
		{R8, RM8, NONE, NONE, group3},
		{R8, RM32, NONE, NONE, group3},
		{CLC}, // 0xF8
		{STC},
		{CLI},
		{STI},
		{CLD},
		{STD},
		{R8, RM8, NONE, NONE, group4},
		{R8, RM32, NONE, NONE, group4},
	};

	unsigned char scanner::gbyte()
	{
		if(p < pe)
			return *p++;
		return 0x90;
	}

	char scanner::gchar()
	{
		if(p < pe)
			return *((int*)p++);
		return 0;
	}

	unsigned scanner::gdword()
	{
		unsigned char pd[4];
		for(int i = 0; i < 4; i++)
			pd[i] = gbyte();
		return *((unsigned*)pd);
	}

	unsigned short scanner::gword()
	{
		unsigned char pd[2];
		for(int i = 0; i < 2; i++)
			pd[i] = gbyte();
		return *((unsigned short*)pd);
	}

	static void sibb(struct parser& parser, oper& op, op_s mod)
	{
		unsigned char m = parser.gbyte();
		int sc = m >> 6;
		int id = (m >> 3) & 7;
		int bs = m & 7;
		op.type = MEM;
		if(bs == 5)
			op.base = mod;
		else
			op.base = (op_s)(EAX + bs);
		if(id != 4)
			op.reg = (op_s)(EAX + id);
		switch(sc)
		{
		case 1:
			op.scale = 2;
			break;
		case 2:
			op.scale = 4;
			break;
		case 3:
			op.scale = 8;
			break;
		}
	}

	static int modrm(struct parser& parser, oper& op1, op_s size)
	{
		unsigned char m = parser.gbyte();
		int mod = (m >> 6) & 3;
		int r1 = m & 7;
		int r2 = (m >> 3) & 7;
		switch(mod)
		{
		case 0:
			switch(r1)
			{
			case 4:
				sibb(parser, op1, NONE);
				break;
			case 5:
				op1.type = MEM;
				if(parser.pre_addr)
					op1.imm = parser.gword();
				else
					op1.imm = parser.gdword();
				break;
			default:
				op1.type = MEM;
				op1.base = (op_s)(EAX + r1);
				break;
			}
			break;
		case 1:
			switch(r1)
			{
			case 4:
				sibb(parser, op1, EBP);
				op1.imm = parser.gbyte();
				break;
			default:
				op1.type = MEM;
				op1.base = (op_s)(EAX + r1);
				op1.imm = parser.gchar();
				break;
			}
			break;
		case 2:
			switch(r1)
			{
			case 4:
				sibb(parser, op1, EBP);
				op1.imm = parser.gdword();
				break;
			default:
				op1.type = MEM;
				op1.base = (op_s)(EAX + r1);
				op1.imm = parser.gdword();
				break;
			}
			break;
		case 3:
			op1.type = size;
			switch(size)
			{
			case R8:
				op1.base = (op_s)(AL + r1);
				break;
			default:
				if(parser.pre_addr)
					op1.base = (op_s)(AX + r1);
				else
					op1.base = (op_s)(EAX + r1);
				break;
			}
			break;
		}
		return r2;
	}

	void parser::load(oper& param, op_s type, int rg)
	{
		int		ix;
		char 	cx;
		short	sx;
		switch(type)
		{
		case IMM8:
			param.type = type;
			param.imm = gbyte();
			if(c == PUSH || c == POP)
				param.type = IMM32;
			break;
		case IMM16:
			param.type = type;
			param.imm = gword();
			if(c == PUSH || c == POP)
				param.type = IMM32;
			break;
		case IMM32:
			param.type = type;
			param.imm = gdword();
			break;
		case EAX: case EBX:
		case ECX: case EDX:
		case EBP: case ESP:
		case ESI: case EDI:
			param.type = R32;
			param.base = type;
			break;
		case AX: case BX:
		case CX: case DX:
		case BP: case SP:
		case SI: case DI:
			param.type = R16;
			param.base = type;
			break;
		case AH: case AL:
		case CH: case CL:
		case BH: case BL:
		case DH: case DL:
			param.type = R8;
			param.base = type;
			break;
		case ES: case DS:
		case CS: case SS:
		case FS: case GS:
			param.type = SEG;
			param.base = type;
			break;
		case RM32:
		case RM8:
			// already set, just break
			break;
		case R8:
			param.type = R8;
			param.base = (op_s)(AL + rg);
			break;
		case R32:
			param.type = R32;
			if(pre_addr)
				param.base = (op_s)(AX + rg);
			else
				param.base = (op_s)(EAX + rg);
			break;
		case REL8:
			cx = (char)gbyte();
			param.type = ADDR;
			param.imm = cx;
			param.imm = (p - section.code) + cx;
			break;
		case REL16:
			sx = (short)gword();
			param.type = ADDR;
			param.imm = gword();
			param.imm = (p - section.code) + sx;
			break;
		case REL32:
			ix = (int)gdword();
			param.type = ADDR;
			param.imm = (p - section.code) + ix;
			break;
		case MEM:
			param.type = type;
			param.imm = gdword();
			break;
		default:
			param.type = type;
			break;
		}
	}

	void parser::next()
	{
		pre_addr = false;
		pre_size = false;
		rep = NONE;
		seg = NONE;
		c = NONE;
		memset(&op, 0, sizeof(op));
		icode		ic = opcodes[gbyte()];
		// prefix
		while(ic.prefix())
		{
			switch(ic.code)
			{
			case SEG:
				seg = ic.p1;
				break;
			default:
				rep = ic.code;
				break;
			}
			ic = opcodes[gbyte()];
		}
		// operation
		int	rg = 0;
		while(true)
		{
			switch(ic.code)
			{
			case IMM8:
				ic = ic.group[gbyte()];
				continue;
			case R8:
				rg = modrm(*this, op[ic.rm()], ic.size());
				ic.code = ic.group[rg].code;
				break;
			case R16:
				rg = modrm(*this, op[ic.rm()], ic.size());
				ic = ic.group[rg];
				break;
			default:
				if(ic.modrm())
					rg = modrm(*this, op[ic.rm()], ic.size());
				break;
			}
			break;
		}
		c = ic.code;
		// parameters
		load(op[0], ic.p1, rg);
		load(op[1], ic.p2, rg);
		load(op[2], ic.p3, rg);
	}

	void parser::tostring(char* w, const oper& op) const
	{
		w[0] = 0;
		switch(op.type)
		{
		case R8:
		case R16:
		case R32:
		case SEG:
			zcpy(w, op_names[op.base]);
			break;
		case MEM:
			zcpy(w, "[");
			if(op.base != NONE)
				zcat(w, op_names[op.base]);
			if(op.reg != NONE)
			{
				zcat(w, "+");
				zcat(w, op_names[op.reg]);
				if(op.scale > 1)
					szprint(zend(w), "*%1i", op.scale);
			}
			if(op.imm)
			{
				char c = (char)op.imm;
				if(op.imm == c)
					szprint(zend(w), "%+1i", op.imm);
				else
				{
					if(w[1] != 0)
					{
						if(op.imm > 0)
							zcat(w, "+");
						sznum(zend(w), op.imm);
					}
					else
						sznum(zend(w), op.imm, 8, 0, 16);
				}
			}
			zcat(w, "]");
			break;
		case IMM32:
			sznum(w, op.imm, 8, 0, 16);
			break;
		case IMM16:
			sznum(w, op.imm, 4, 0, 16);
			break;
		case IMM8:
			sznum(w, op.imm, 2, 0, 16);
			break;
		case ADDR:
			sznum(w, op.imm + section.image, 8, 0, 16);
			break;
		default:
			break;
		}
	}

	void parser::tostring(char* w) const
	{
		w[0] = 0;
		if(c == NONE)
			szprint(w, "nimpl %02Xh", op[0].imm);
		else
		{
			if(rep != NONE)
				zcat(w, op_names[rep]);
			if(w[0] != 0)
				zcat(w, " ");
			int i = zlen(op_names[c]);
			zcat(w, op_names[c]);
			for(; i < 4; i++)
				zcat(w, " ");
			if(op[0].type != NONE)
			{
				zcat(w, " ");
				tostring(zend(w), op[0]);
			}
			if(op[1].type != NONE)
			{
				zcat(w, ",");
				tostring(zend(w), op[1]);
			}
			if(op[2].type != NONE)
			{
				zcat(w, ",");
				tostring(zend(w), op[2]);
			}
		}
	}

}