#include "typeref.h"

#pragma once

namespace c2
{
	enum messages
	{
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
	enum typeflags : char {
		Public, Static, Readed, Writed, NoInitialized, Function, Parameter,
	};
	struct type
	{
		const char*		id; // name identifiers ('this' name for base type)
		unsigned		size; // size of type/requisit (total lenght is multiplied by count), 0 for method
		int				count; // 0 for type, 1+ for requisit, parameters count for method
		int				value; // Offset in section
		type*			parent; // types, plaform types, reference
		type*			result; // result of requisit or type pointer
		type*			child; // child requisits, list of params
		type*			next; // next element in chain
		typeref*		refs; // pseudonime and types
		unsigned		flags; // any flags
		const char*		content; // code source
		//
		operator bool() const { return id != 0; }
		//
		static type		i8[1];
		static type		i16[1];
		static type		i32[1];
		static type		u8[1];
		static type		u16[1];
		static type		u32[1];
		static type		v0[1];
		//
		static const char* id_this;
		//
		static bool		compile(const char* module_id, const char* proc_name = 0);
		static type*	create(const char*  id);
		type*			create(const char*  id, type* result, unsigned flags);
		static type*	createloc(const char* id, type* result, unsigned flags);
		static void		cleanup();
		void			clear();
		type*			dereference();
		type*			find(const char* id);
		type*			findmembers(const char* id);
		type*			findmembertype(const char* id, int modifier_unsigned = 0);
		static type*	findtype(const char* id);
		unsigned		getparametercount() const { return ismethod() ? count : 0; }
		type*			getprevious();
		bool			is(typeflags value) const { return (flags&(1 << value)) != 0; }
		bool			isarray() const { return count > 1; }
		bool			isconstant() const { return count == -1 && size == 0; }
		bool			isforward() const { return ismethod() && !content; }
		bool			islocal() const;
		bool			ismember() const;
		bool			ismethod() const;
		bool			ismethodparam() const;
		bool			isnumber() const;
		bool			isplatform() const;
		bool			ispointer() const;
		bool			istype() const;
		static void		link(const char* id);
		type*			reference();
		void			set(typeflags value) { flags |= (1 << value); }
		void			setconstant(int value);
		void			setmethod();
		void			setmethodparam();
	};
	void				status(messages m, ...);
	extern void(*errorproc)(messages m, const type* module, const type* member, const char* parameters);
	extern void(*statusproc)(messages m, const type* module, const type* member, const char* parameters);
}