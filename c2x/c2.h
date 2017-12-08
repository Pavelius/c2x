#include "adat.h"
#include "aref.h"
#include "crt.h"
#include "files.h"

#pragma once

#define section_code ".code"
#define section_data ".data"
#define section_bbs ".bbs"
#define section_strings ".strings"

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
	enum register_s : char {
		Eax, Ebx, Ecx, Edx, Esi, Edi, Esp, Ebp,
		Const,
	};
	struct section {
		const char*		id;
		unsigned		rvabase;
		section*		next;
		virtual void	add(unsigned char a) = 0;
		virtual void	clear() = 0;
		static section*	find(const char* id);
		virtual unsigned get() = 0;
		virtual unsigned char* getdata() = 0;
		virtual void	set(unsigned count) = 0;
		bool			write(const char* url);
	};
	struct symbol {
		const char*		id;
		symbol*			result;
		const char*		visibility; // For module or module member this value equal start of text file. Local variables and parameters have it own visibility.
		const char*		declared; // Line where symbol declared.
		const char*		content;
		unsigned		size;
		unsigned		count; // Count of array elements. For non array this member is zero.
		int				value;
		unsigned		flags;
		struct section*	section; // Local variable and types has no section.
		//
		operator bool() const { return id != 0; }
		static symbol*	add();
		static symbol*	addmodule();
		void			clear();
		symbol*			dereference();
		symbol*			getchild() const;
		symbol*			getnext() const;
		const char*		getname() const { return (this ? id : "none"); }
		symbol*			getprevious() const;
		unsigned		getsize() const { return size;}
		static void		initialize();
		bool			isarray() const { return count != 0; }
		bool			isfunction() const;
		bool			isfunctionparam() const;
		bool			isfunctionparam(symbol* function) const;
		bool			islocal() const;
		bool			ismember() const;
		bool			isnumber() const;
		bool			ispointer() const;
		bool			ispseudoname() const;
		bool			isstatic() const;
		bool			istype() const;
		bool			istypesimple() const;
		symbol*			reference();
		void			setpseudoname();
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
		bool			usedsymbols;
		genstate();
		~genstate();
	};
	typedef adat<symbol, 8096> symbolset;
	extern symbolset	symbols;
	extern symbolset	pointers;
	extern symbolset	modules;
	extern evalue::plugin* backend;
	void				compile(const char* url);
	void				error(message_s id, ...);
	extern int			errors;
	void				errorv(message_s m, const symbol* module, const symbol* member, const char* parameters);
	symbol*				findmodule(const char* id);
	symbol*				findmodulebv(const char* visiblility);
	symbol*				findsymbol(const char* id, const char* visibility);
	symbol*				findsymbol(const char* id, const char* visibility, bool function);
	symbol*				findtype(const char* id, unsigned flags);
	symbol*				findtype(const char* id, const char* visibility);
	symbol*				findmodule(const char* url);
	extern genstate		gen;
	bool				isstatic(unsigned flags);
	bool				isthis(const char* id);
	void				link(const char* id);
	unsigned			setpublic(unsigned flags);
	unsigned			setstatic(unsigned flags);
	void				status(message_s id, ...);

	extern symbol		types[]; // Такого родителя имеют все типы
	extern symbol		i8[]; // Байт со знаком
	extern symbol		i16[]; // Слово со знаком
	extern symbol		i32[]; // Двойное слово со знаком
	extern symbol		u8[]; // Байт без знака
	extern symbol		u16[]; // Слово без знака
	extern symbol		u32[]; // Двойное слово без знака
	extern symbol		v0[]; // Путое значение
}