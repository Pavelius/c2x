#include "adat.h"
#include "aref.h"
#include "crt.h"
#include "evalue.h"
#include "files.h"

#pragma once

namespace c2 {
	enum message_s {
		ErrorUnexpectedSymbols, ErrorExpected1p,
		ErrorDivideByZero,
		ErrorInvalidEscapeSequence,
		ErrorInvalid1p2pIn3p,
		ErrorCantFind1pWithName2p,
		ErrorOperation1pNotUsedWithOperands2pAnd3p,
		ErrorNeedSymbol, ErrorNeedConstantExpression, ErrorNeedIndentifier, ErrorNeedFunctionMember, ErrorNeedPointerOrArray, ErrorNeedLValue,
		ErrorLongNotUseWithThisType, ErrorShortNotUseWithThisType, ErrorUnsignedNotUseWithThisType,
		Error1pDontUse2pTimes,
		Error1p2pAlreadyDefined,
		ErrorArrayOverflow, ErrorImportAlreadyHavePseudonim, ErrorModuleAlreadyImported,
		ErrorUnknownInstance, ErrorVoidReturnValue,
		ErrorKeyword1pUsedWithout2p,
		ErrorAssigmentWithoutEnumeratorMember, ErrorExpectedEnumeratorMember,
		ErrorSectionNumber,
		ErrorWrongParamNumber,
		ErrorFunctionMustReturnValue,
		ErrorNotImplement1p2p,
		ErrorNotFound1p2p,
		ErrorCastType1pTo2p,
		ErrorOptions, ErrorCompilator, ErrorLinker,
		FirstError = ErrorUnexpectedSymbols, LastError = ErrorCastType1pTo2p,
		StatusStartParse, StatusEndParse, StatusLink1p, StatusKeyword, StatusCompileMethod,
		StatusDeclare,
		FirstStatus = StatusStartParse, LastStatus = StatusDeclare,
	};
	enum flag_s : char {
		Public, Static, Readed, Writed, NoInitialized
	};
	enum register_s : char {
		Eax, Ebx, Ecx, Edx, Esi, Edi, Esp, Ebp,
		Const,
	};
	enum sectiontypes : char {
		Code, Data, DataStrings, DataUninitialized,
	};
	enum symbol_s : char {
		SymbolType, SymbolTypedef, SymbolConstant,
		SymbolMember, SymbolFunction,
		SymbolLocal, SymbolParameter,
	};
	struct state_state {
		bool			error_double_identifier;
	};
	struct symbol {
		const char*		id;
		symbol*			result;
		const char*		visibility;
		unsigned		size;
		unsigned		count;
		symbol*			parent;
		int				value;
		short unsigned	flags;
		symbol_s		type;
		//
		operator bool() const { return id != 0; }
		void			clear();
		symbol*			dereference();
		symbol*			getchild();
		symbol*			getnext(symbol* parent);
		unsigned		getsize() const { return size*count;}
		static void		initialize();
		bool			isforward() const;
		bool			isfunction() const;
		bool			islocal() const;
		bool			ismember() const;
		bool			ismethodparam() const;
		bool			isnumber() const;
		bool			ispointer() const;
		bool			isstatic() const;
		bool			istype() const;
		symbol*			reference();
		void			setfunction() { type = SymbolFunction; }
		void			setfunctionfw() { type = SymbolFunction; }
	};
	struct scope_state {
		scope_state*	parent;
		symbol*			member;
		const char*		visibility;
		scope_state();
		~scope_state();
	};
	struct evalue {
		struct plugin {
			const char*	progid;
			plugin*		next;
			static plugin*	first;
			static plugin*	current;
			//
			plugin(const char* progid);
			//
			virtual void	epilogue(symbol* module, symbol* member) = 0;
			static plugin*	find(const char* progid);
			virtual void	operation(evalue& e1, evalue& e2, char t1, char t2 = 0) = 0;
			virtual void	operation(evalue& e1, char t1) = 0;
			virtual void	retproc(symbol* member) = 0;
			virtual void	prologue(symbol* module, symbol* member) = 0;
			virtual	int		label(int a) = 0;
		};
		symbol*			result;
		register_s		reg;
		int				offset;
		symbol*			sym;
		evalue*			next;
		//
		evalue();
		evalue(evalue* e);
		//
		void			cast(symbol* need_result);
		void			clear();
		void			dereference();
		bool			isconst() const { return reg == Const; }
		bool			islvalue() const { return sym != 0; }
		static int		localalloc(unsigned size);
		register_s		getfree() const;
		void			getrvalue();
		void			load(register_s r);
		void			load() { load(getfree()); }
		void			set(int value);
		void			set(symbol* symbol);
		void			set(const evalue& e);
		void			setlvalue();
		void			xchange(evalue& e);
	};
	struct genstate {
		bool			code;
		bool			unique;
		bool			methods;
		genstate();
		~genstate();
	};
	struct segment {
		unsigned		rvabase;
		segment() : rvabase(0) {}
		virtual void	add(unsigned char a) = 0;
		virtual unsigned	get() = 0;
		virtual unsigned char*	getdata() = 0;
		virtual void	set(unsigned count) = 0;
	};
	symbol*				addsymbol(const char* id, symbol* result, symbol* parent, symbol_s type);
	extern segment*		segments[DataUninitialized + 1];
	extern evalue::plugin*	backend;
	void				compile(const char* url);
	void				error(message_s id, ...);
	extern int			errors;
	void				errorv(message_s m, const symbol* module, const symbol* member, const char* parameters);
	symbol*				findsymbol(const char* id, symbol_s type);
	symbol*				findsymbol(const char* id, const char* visibility, symbol_s type);
	symbol*				findtype(const char* id, unsigned modifier_unsigned);
	symbol*				findmodule(const char* url);
	extern genstate		gen;
	inline bool			is(unsigned flags, flag_s value) { return (flags & (1 << value)) != 0; }
	bool				isloaded(symbol* result);
	inline void			remove(unsigned& flags, flag_s value) { flags &= ~(1 << value); }
	inline void			set(unsigned& flags, flag_s value) { flags |= (1 << value); }
	extern scope_state	scope;
	extern state_state	state;
	void				status(message_s id, ...);

	extern symbol		types[]; // ������ �������� ����� ��� ����
	extern symbol		i8[]; // ���� �� ������
	extern symbol		i16[]; // ����� �� ������
	extern symbol		i32[]; // ������� ����� �� ������
	extern symbol		u8[]; // ���� ��� �����
	extern symbol		u16[]; // ����� ��� �����
	extern symbol		u32[]; // ������� ����� ��� �����
	extern symbol		v0[]; // ����� ��������
}