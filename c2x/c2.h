#include "adat.h"
#include "crt.h"
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
		Public, Static, Readed, Writed, NoInitialized, Function, Parameter,
	};
	struct status_state {
		bool			error_double_identifier;
	};
	struct parser_state {
		const char*		p;
	};
	struct symbol {
		const char*		name;
		symbol*			result;
		const char*		visibility;
		const char*		content;
		unsigned		size;
		unsigned		count;
		unsigned		flags;
		symbol*			parent;
		//
		operator bool() const { return name != 0; }
		static symbol*	add(const char* name, symbol* member = 0);
		void			clear();
		symbol*			dereference();
		static symbol*	find(const char* name);
		static symbol*	find(const char* name, const char* visibility);
		bool			islocal() const;
		bool			ismember() const;
		bool			ismethod() const;
		bool			ismethodparam() const;
		bool			isnumber() const;
		bool			ispointer() const;
		bool			istype() const;
		symbol*			reference();
	};
	struct scope_state {
		scope_state*	parent;
		symbol*			member;
		const char*		visibility;
		scope_state();
		~scope_state();
	};
	void				compile(const char* url);
	void				error(message_s id, ...);
	void				errorv(message_s m, const symbol* module, const symbol* member, const char* parameters);
	inline bool			is(unsigned flags, flag_s value) { return (flags & (1 << value)) != 0; }
	inline void			remove(unsigned& flags, flag_s value) { flags &= ~(1 << value); }
	extern scope_state	scope;
	inline void			set(unsigned& flags, flag_s value) { flags |= (1 << value); }
	extern status_state	status;

	extern symbol		i8[]; // Байт со знаком
	extern symbol		i16[]; // Слово со знаком
	extern symbol		i32[]; // Двойное слово со знаком
	extern symbol		u8[]; // Байт без знака
	extern symbol		u16[]; // Слово без знака
	extern symbol		u32[]; // Двойное слово без знака
	extern symbol		v0[]; // Путое значение
}