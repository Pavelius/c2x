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

namespace dasm
{
	enum op_s
	{
		NONE,
		// registers
		EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI,
		AL, CL, DL, BL, AH, CH, DH, BH,
		AX, CX, DX, BX, SP, BP, SI, DI,
		ES, CS, SS, DS, FS, GS,
		MM0, MM1, MM2, MM3, MM4, MM5, MM6, MM7,
		ST0, ST1, ST2, ST3, ST4, ST5, ST6, ST7,
		// opcodes
		ADD, OR, ADC, SBB, AND, SUB, XOR, CMP,
		ROL, ROR, RCL, RCR, SHL, SAL, SHR, SAR,
		INC, DEC, NOT, NEG, MUL, IMUL, DIV, IDIV,
		PUSH, POP, PUSHA, PUSHAD, POPA, POPAD, PUSHF, PUSHFD, POPF, POPFD,
		BOUND, ARPL, SAHF, LAHF, SALC,
		AAA, AAS, DAA, DAS, AAM, AAD, AMX, ADX,
		CDQ, CWD,
		JO, JNO, JB, JAE, JZ, JNZ, JBE, JA, JS, JNS, JP, JNP, JL, JGE, JLE, JG,
		REPZ, REPNZ, LOCK, JCXZ, LOOPNZ, LOOPZ, LOOP,
		LES, LDS, TEST, XCHG, MOV, LEA,
		NOP, WAIT, XLAT, IN, OUT,
		CALL, CALLF, JMP, JMPF, RET, RETF, LEAVE, ENTER, INT, INTO, IRET,
		CLD, STD, CLI, STI, CLC, STC, CMC, HLT,
		INSB, INSD, OUTSB, OUTSD, MOVSB, MOVSD, STOSB, STOSD, LODSB, LODSD, CMPSB, CMPSD, SCASB, SCASD,
		SLDT, LLDT, STR, VERR, VERW, SGDT, SIDT, LGDT, SMSW, LMSW, INVLPG, LAR, LSL, CLTS, INVD, WBINVD, LTR,
		LDTR, TR,
		SETO, SETNO, SETB, SETAE, SETZ, SETNZ, SETBE, SETA, SETS, SETNS, SETP, SETNP, SETL, SETGE, SETLE, SETG,
		// operation type
		PREFIX, C1, C3,
		MEM, ADDR,
		IMM8, IMM16, IMM32,
		REL8, REL16, REL32,
		R8, R16, R32, SEG,
		RM8, RM32,
	};
	struct oper
	{
		op_s		type;
		op_s		base;
		op_s		reg;
		int			scale;
		int			imm;
	};
	struct scanner
	{
		const unsigned char*	p;
		const unsigned char*	pe;
		//
		unsigned char			gbyte();
		char					gchar();
		unsigned 				gdword();
		unsigned short			gword();
		unsigned char			nbyte(); // next byte without reading it from stream
	};
	struct section
	{
		unsigned				image;
		const unsigned char*	code;
	};
	// Main class (how to use it looking in disassemle() function)
	struct parser : scanner
	{
		op_s					rep;
		op_s					seg;
		bool					pre_addr;
		bool					pre_size;
		unsigned char			byte_modrm;
		unsigned char			byte_sibb;
		op_s					c;
		oper					op[3];
		struct section			section;
		//
		void					next();
		void					tostring(char* w) const;
	private:
		void					load(oper& param, op_s type, int rg);
		void					tostring(char* w, const oper& op) const;
	};
};