#include "type.h"

#pragma once

namespace c2
{
	enum registers : char {
		Eax, Ebx, Ecx, Edx, Esi, Edi, Esp, Ebp,
		Const,
	};
	struct evalue
	{
		struct plugin
		{
			const char*		progid;
			plugin*			next;
			static plugin*	first;
			static plugin*	current;
			//
			plugin(const char* progid);
			//
			virtual void	epilogue(type* module, type* member) = 0;
			static plugin*	find(const char* progid);
			virtual void	operation(evalue& e1, evalue& e2, char t1, char t2 = 0) = 0;
			virtual void	operation(evalue& e1, char t1) = 0;
			virtual void	retproc(type* member) = 0;
			virtual void	prologue(type* module, type* member) = 0;
			virtual	int		label(int a) = 0;
		};
		type*				result;
		registers			reg;
		int					offset;
		type*				sym;
		evalue*				next;
		//
		evalue();
		evalue(evalue* e);
		//
		void				cast(type* need_result);
		void				clear();
		void				dereference();
		bool				isconst() const { return reg == Const; }
		bool				islvalue() const { return sym != 0; }
		static int			localalloc(unsigned size);
		registers			getfree() const;
		void				getrvalue();
		void				load(registers r);
		void				load() { load(getfree()); }
		void				set(int value);
		void				set(type* symbol);
		void				set(const evalue& e);
		void				setlvalue();
		void				xchange(evalue& e);
	};
	extern evalue::plugin*	backend;
}
