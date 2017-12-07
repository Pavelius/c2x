#include "c2.h"

using namespace c2;

static void			logical_or(evalue& e1);
static symbol*		parse_module(const char* id);
static symbol*		standart_types[] = {i8, i16, i32, u8, u16, u32, v0};
static void			statement(int* ct, int* br, int* cs, int* ds, evalue* cse = 0);
int					c2::errors;

static struct parser_state {
	const char*		p;
	symbol*			current_module;
	symbol*			current_member;
	parser_state() { memset(this, 0, sizeof(*this)); }
} ps;

static struct scopestate {
	scopestate*		parent;
	const char*		visibility;
	scopestate();
	~scopestate();
} scope;

scopestate::scopestate() : scopestate(scope) {
	if(this != &scope)
		scope.parent = this;
	scope.visibility = ps.p;
}

scopestate::~scopestate() {
	scope = *this;
}

static symbol* getmodule() {
	return ps.current_module;
}

static symbol* getmember() {
	return ps.current_member;
}

void c2::error(message_s id, ...) {
	if(getmodule())
		errorv(id, 0, getmodule(), xva_start(id));
	else
		errorv(id, 0, 0, xva_start(id));
}

void c2::status(message_s id, ...) {
}

static int jumpforward(int a = 0) {
	// Безусловный переход вперед
	return 0;
}

static void jumpback(int a) {
	// Переход назад
}

static int testcondition(int a, int b = 0) {
	// Если условия истина/ложь осуществляет переход
	return 0;
}

static void label(int i) {
	// Патчит назад все метки в секции кода
}

static int label() {
	// Создает метку в текущем месте
	//return segments[Code]->get();
	return 0;
}

static void addr32(int v, symbol* sym, section* sec) {
	auto s = 4;
	while(s > 0) {
		if(sec)
			sec->add((unsigned char)(v & 255));
		v >>= 8;
		s--;
	}
}

static void calling(symbol* sym, evalue* parameters, int count) {
}

static void prologue() {
	auto sym = getmember();
	if(!sym)
		return;
	//if(gen.code) {
	//	sym->value = segments[Code]->get();
	//	backend->prologue(scope.member, sym);
	//}
}

static void epilogue() {
	auto sym = getmember();
	if(!sym)
		return;
	//if(!ps.module || !sym)
	//	return;
	//if(gen.code)
	//	backend->epilogue(ps.module, sym);
}

static void retproc() {
	auto sym = getmember();
	if(!sym)
		return;
	//if(gen.code)
	//	backend->retproc(sym);
}

static void unary_operation(evalue& e2, char t1) {
	char temp[2];
	if(e2.isconst()) {
		switch(t1) {
		case '!': e2.offset = !e2.offset; return;
		case '-': e2.offset = -e2.offset; return;
		case '~': e2.offset = ~e2.offset; return;
		default:
			temp[0] = t1; temp[1] = 0;
			status(ErrorNotImplement1p2p, "constant unary operator", temp);
			break;
		}
	}
	//if(gen.code)
	//	backend->operation(e2, t1);
}

static void binary_operation(evalue& e2, char t1, char t2 = 0) {
	if(!e2.next) {
		status(ErrorExpected1p, "binary operation");
		return;
	}
	// Операция должна вытолкнуть один операнд из стека
	evalue& e1 = *e2.next;
	e2.next = 0;
	if(e1.result->ispointer()) {
		if(e2.result->isnumber()) {
			char opname[2];
			switch(t1) {
			case '+':
			case '-':
				break;
			default:
				opname[0] = t1; opname[1] = 0;
				status(ErrorOperation1pNotUsedWithOperands2pAnd3p, opname, "pointer", "number");
				break;
			}
			if(e1.isconst() && e2.isconst() && !e2.islvalue()) {
				switch(t1) {
				case '+': e1.offset += e1.result->result->size*e2.offset; return;
				case '-': e1.offset -= e1.result->result->size*e2.offset; return;
				}
			}
		}
	} else if(e1.isconst() && e2.isconst()) // Constant case
	{
		switch(t1) {
		case '.': e1.set(e2); return;
		case '&':
			if(t2 == t1)
				e1.offset = e1.offset && e2.offset;
			else
				e1.offset &= e2.offset;
			return;
		case '|':
			if(t2 == t1)
				e1.offset = e1.offset || e2.offset;
			else
				e1.offset |= e2.offset;
			return;
		case '+': e1.offset += e2.offset; return;
		case '-': e1.offset += e2.offset; return;
		case '*': e1.offset *= e2.offset; return;
		case '/':
			if(!e2.offset)
				status(ErrorDivideByZero);
			else
				e1.offset /= e2.offset;
			return;
		case '%':
			if(!e2.offset)
				status(ErrorDivideByZero);
			else
				e1.offset %= e2.offset;
			return;
		}
	}
	if(e1.islvalue() && e2.islvalue())
		e1.load();
	//if(gen.code)
	//	backend->operation(e1, e2, t1, t2);
}

static symbol* addsymbol(const char* id, symbol* result) {
	auto p = findsymbol(id, scope.visibility);
	if(p) {
		if(gen.unique)
			error(Error1p2pAlreadyDefined, "symbol", id);
		return p;
	}
	p = symbol::add();
	if(!p)
		return 0;
	p->clear();
	p->id = id;
	p->result = result;
	p->visibility = scope.visibility;
	p->declared = ps.p;
	return p;
}

static symbol* findsymbol(const char* id) {
	for(auto psc = &scope; psc; psc = psc->parent) {
		auto sym = findsymbol(id, psc->visibility);
		if(sym)
			return sym;
	}
	return 0;
}

static bool skipcm() {
	if(ps.p[0] == '/' && ps.p[1] == '/') {
		// Line comment
		ps.p += 2;
		while(*ps.p && *ps.p != '\n' && *ps.p != '\r')
			ps.p++;
		return true;
	} else if(ps.p[0] == '/' && ps.p[1] == '*') {
		// Comment
		ps.p += 2;
		while(*ps.p && !(ps.p[0] == '*' && ps.p[1] == '/'))
			ps.p++;
		return true;
	}
	return false;
}

static void next(const char* p1) {
	ps.p = p1;
	while(true) {
		// 1 - skip all white spaces
		ps.p = zskipspcr(ps.p);
		// 2 - skip all comments
		if(skipcm())
			continue;
		break;
	}
}

static void skip(char sym) {
	if(*ps.p == sym)
		next(ps.p + 1);
	else if(*ps.p) {
		char opname[] = {sym, 0};
		status(ErrorExpected1p, opname);
	}
}

static bool ischab(char sym) {
	return (sym >= 'A' && sym <= 'Z')
		|| (sym >= 'a' && sym <= 'z')
		|| (sym == '_');
}

static bool ischa(char sym) {
	return (sym >= '0' && sym <= '9')
		|| (sym >= 'A' && sym <= 'Z')
		|| (sym >= 'a' && sym <= 'z')
		|| (sym == '_');
}

static bool match(char sym) {
	if(*ps.p == sym) {
		next(ps.p + 1);
		return true;
	}
	return false;
}

static bool match(const char* name) {
	auto p1 = ps.p;
	auto p2 = name;
	while(name[0] && *p1) {
		if(*name++ != *p1++)
			return false;
	}
	if(name[0] || ischa(*p1))
		return false;
	next(p1);
	status(StatusKeyword, p2);
	return true;
}

static const char* identifier() {
	if(!ischab(*ps.p))
		return 0;
	static char temp[256];
	auto s = temp;
	while(ischa(*ps.p))
		*s++ = *ps.p++;
	*s++ = 0;
	next(ps.p);
	return temp;
}

static void expression(evalue& e1) {
	logical_or(e1);
	while(*ps.p == '?') {
		next(ps.p + 1);
		int i = jumpforward();
		expression(e1);
		skip(':');
		int f = testcondition(1);
		label(i);
		evalue e2;
		expression(e1);
		label(f);
	}
}

static void expression() {
	evalue e1;
	expression(e1);
}

static void expression_nocode() {
	genstate push;
	gen.code = false;
	evalue e1;
	expression(e1);
}

static int expression_const() {
	genstate push;
	gen.code = false;
	evalue e1;
	expression(e1);
	if(!e1.isconst())
		status(ErrorNeedConstantExpression);
	return e1.offset;
}

static symbol* expression_const_type() {
	genstate push;
	gen.code = false;
	evalue e1;
	expression(e1);
	if(!e1.isconst())
		status(ErrorNeedConstantExpression);
	auto result = e1.sym;
	if(!result || !result->istype())
		result = e1.result;
	return result;
}

static int test_01[] = {0, {2}};

static bool istype(symbol** declared, unsigned& flags) {
	auto e = i32;
	bool result = false;
	int m_public = 0;
	int m_static = 0;
	int m_unsigned = 0;
	const char* p1 = ps.p;
	while(*ps.p) {
		if(match("static")) {
			result = true;
			m_static++;
			continue;
		} else if(match("public")) {
			result = true;
			m_public++;
			continue;
		} else if(match("unsigned")) {
			result = true;
			m_unsigned++;
			continue;
		} else if(ischab(*ps.p)) {
			auto id = szdup(identifier());
			// Теперь надо найти тип, и убедиться, что это действительно определение
			bool change_back = true;
			auto parent_module = getmodule();
			if(isthis(id))
				e = getmodule();
			else
				e = findtype(id, flags);
			if(!e && parent_module) {
				e = findtype(id, parent_module->visibility);
				if(!e) {
					auto this_name = szext(parent_module->id);
					if(!this_name)
						this_name = parent_module->id;
					if(strcmp(id, this_name) == 0)
						e = parent_module;
				}
			}
			if(e) {
				if(*ps.p == '*' || ischa(*ps.p)) {
					change_back = false;
					result = true;
				}
			}
			if(change_back)
				ps.p = p1;
		}
		if(result) {
			if(m_static > 1)
				status(Error1pDontUse2pTimes, "static", m_static);
			if(m_public > 1)
				status(Error1pDontUse2pTimes, "public", m_public);
			if(m_unsigned > 1)
				status(Error1pDontUse2pTimes, "unsigned", m_unsigned);
			if(m_static)
				flags = setstatic(flags);
			if(m_public)
				flags = setpublic(flags);
			*declared = e;
		}
		break;
	}
	return result;
}

static void initialize(symbol* sym, section* sec) {
	if(sym->isarray() && *ps.p == '{') {
		next(ps.p + 1);
		if(sym->count == 0xFFFFFFFF) {
			// When size of array is unknown.
			sym->count = 0;
			while(true) {
				sym->count++;
				initialize(sym->result, sec);
				if(*ps.p == '}') {
					break;
				}
				skip(',');
				if(*ps.p == '}') {
					break;
				}
			}
		} else {
			// When size of array is known and we need add emphty elements to the end.
			unsigned i = 0;
			while(true) {
				if(i++ >= sym->count) {
					genstate push;
					gen.code = false;
					initialize(sym->result, sec);
				} else
					initialize(sym->result, sec);
				if(*ps.p == '}')
					break;
				skip(',');
				if(*ps.p == '}')
					break;
			}
			if(i >= sym->count)
				status(ErrorArrayOverflow, i, sym->count);
		}
		skip('}');
	} else if(sym->result && sym->result->ispointer()) {
		evalue e1;
		expression(e1);
		if(!e1.isconst())
			status(ErrorNeedConstantExpression);
		addr32(e1.offset, e1.sym, sec);
	} else if(sym->result) {
		initialize(sym->result, sec);
	} if(*ps.p == '{') {
		next(ps.p + 1);
		auto t = sym->getchild();
		while(t) {
			if(t->ismember() && !t->isfunction()) {
				initialize(t, sec);
				if(*ps.p == '}')
					break;
				skip(',');
				if(*ps.p == '}')
					break;
			}
			t = t->getnext();
		}
		skip('}');
	} else {
		// simple type
		int s = sym->size;
		int v = expression_const();
		while(s > 0) {
			if(sec)
				sec->add((unsigned char)(v & 255));
			v >>= 8;
			s--;
		}
	}
}

static int get_stack_frame(symbol* variable) {
	return 8;
}

static symbol* parse_pointer(symbol* declared) {
	while(*ps.p == '*') {
		next(ps.p + 1);
		declared = declared->reference();
	}
	return declared;
}

static bool direct_cast(evalue& e1) {
	symbol* declared;
	unsigned flags = 0;
	const char* p1 = ps.p;
	if(!istype(&declared, flags))
		return false;
	declared = parse_pointer(declared);
	if(*ps.p != ')') {
		ps.p = p1;
		return false;
	}
	next(ps.p + 1);
	e1.result = declared;
	return true;
}

static char next_string_symbol() {
	while(*ps.p) {
		if(*ps.p != '\\')
			return *ps.p++;
		ps.p++;
		switch(*ps.p++) {
		case 0:
			return 0;
		case 'n':
			return '\n';
		case 't':
			return '\t';
		case 'r':
			return '\r';
		case '\\':
			return '\\';
		case '\'':
			return '\'';
		case '\"':
			return '\"';
		case '\n':
			if(ps.p[0] == '\r')
				ps.p++;
			break;
		case '\r':
			if(ps.p[0] == '\n')
				ps.p++;
			break;
		default:
		{
			char temp[2] = {ps.p[-1], 0};
			status(ErrorInvalidEscapeSequence, temp);
		}
		break;
		}
	}
	return 0;
}

static int next_char() {
	char result[5];
	char* d1 = result;
	char* d2 = result + sizeof(result) / sizeof(result[0]) - 1;
	memset(result, 0, sizeof(result));
	while(*ps.p) {
		if(*ps.p == '\'') {
			next(ps.p + 1);
			break;
		}
		if(d1 < d2)
			*d1++ = next_string_symbol();
	}
	*d1++ = 0;
	return *((int*)&result);
}

static int get_string(const char* temp) {
	int result = section::find(section_strings)->get();
	for(auto p = temp; *p; p++)
		section::find(section_strings)->add((unsigned char)*p);
	section::find(section_strings)->add(0);
	return result;
}

static int next_string() {
	static char temp_buffer[256 * 256];
	auto pb = temp_buffer;
	auto pe = pb + sizeof(temp_buffer) - 2;
	while(*ps.p) {
		if(*ps.p == '\"') {
			next(ps.p + 1);
			if(*ps.p == '\"') {
				// if two string in row
				next(ps.p + 1);
				continue;
			} else if(*ps.p == '+' && ps.p[1] != '+') {
				// checking '+' between two strings
				const char* p1 = ps.p;
				next(ps.p + 1);
				if(*ps.p == '\"') {
					next(ps.p + 1);
					continue;
				}
				ps.p = p1;
			}
			break;
		}
		auto sym = next_string_symbol();
		if(pb < pe)
			*pb++ = sym;
	}
	*pb = 0;
	return get_string(temp_buffer);
}

static int next_number() {
	int num = 0;
	if(ps.p[0] == '0') {
		if(ps.p[1] == 'x') {
			ps.p += 2;
			while(true) {
				char s = *ps.p;
				if(s >= 'a' && s <= 'f')
					s = s - 'a' + 10;
				else if(s >= 'A' && s <= 'F')
					s = s - 'A' + 10;
				else if(s >= '0' && s <= '9')
					s = s - '0';
				else
					break;
				num = num * 16 + s;
				ps.p++;
			}
		} else {
			while(*ps.p >= '0' && *ps.p <= '7') {
				num = num * 8 + *ps.p - '0';
				ps.p++;
			}
		}
	} else {
		while(*ps.p >= '0' && *ps.p <= '9') {
			num = num * 10 + *ps.p - '0';
			ps.p++;
		}
	}
	if(*ps.p == 'f' || *ps.p == 'e')
		ps.p++;
	next(ps.p);
	return num;
}

static void function_call(evalue& e1) {
	auto sym = e1.sym;
	if(!sym->isfunction())
		status(ErrorNeedFunctionMember, sym->getname());
	skip('(');
	int	count = 0;
	evalue parameters[96];
	while(*ps.p) {
		if(*ps.p == ')') {
			next(ps.p + 1);
			break;
		}
		parameters[count].clear();
		parameters[count].next = (count ? &parameters[count - 1] : &e1);
		expression(parameters[count]);
		count++;
		if(*ps.p == ')') {
			next(ps.p + 1);
			break;
		}
		skip(',');
	}
	// default parameters
	// parameters back order
	for(int i = 0; i < count; i++) {
		//gen::param();
		//gen::pop();
	}
	//if(sym->getparametercount() != count)
	//	status(ErrorWrongParamNumber, sym->id, sym->getparametercount(), count);
	calling(sym, parameters, count);
	// function return value
	e1.reg = Eax;
	e1.offset = 0;
	e1.sym = 0;
}

static void postfix(evalue& e1) {
	while(*ps.p) {
		if(*ps.p == '(')
			function_call(e1);
		else if(*ps.p == '.') {
			next(ps.p + 1);
			const char* n = szdup(identifier());
			if(e1.result->ispointer())
				e1.dereference();
			e1.getrvalue();
			symbol* sym = 0;
			if(*ps.p == '(') {
				sym = findsymbol(n, e1.result->visibility, true);
				if(!sym) {
					sym = addsymbol(n, i32);
					// Forward declaration function in scope of expression result
					sym->visibility = e1.result->visibility;
				}
			} else
				sym = findsymbol(n, e1.result->visibility, false);
			evalue e2(&e1); e2.set(sym);
			binary_operation(e2, '.');
		} else if(*ps.p == '[') {
			next(ps.p + 1);
			if(e1.sym && e1.sym->isarray())
				e1.result = e1.result->reference();
			if(!e1.result->ispointer())
				status(ErrorNeedPointerOrArray);
			evalue e2(&e1);
			expression(e2);
			skip(']');
			binary_operation(e2, '+');
			e1.getrvalue();
		} else if(ps.p[0] == '-' && ps.p[1] == '-') {
			next(ps.p + 2);
			if(!e1.islvalue())
				status(ErrorNeedLValue);
			//gen::dup();
			//gen::rvalue(-1);
			evalue e2(&e1); e2.set(1);
			binary_operation(e2, '=', '-');
			//gen::pop();
		} else if(ps.p[0] == '+' && ps.p[1] == '+') {
			next(ps.p + 2);
			if(!e1.islvalue())
				status(ErrorNeedLValue);
			//gen::dup();
			//gen::rvalue(-1);
			evalue e2(&e1); e2.set(1);
			binary_operation(e2, '=', '+');
			//gen::pop();
		} else
			break;
	}
}

static void register_used_symbol(symbol* sym) {
	if(!gen.methods)
		return;
	//for(auto p : used_symbols) {
	//	if(p == sym)
	//		return;
	//}
	//used_symbols.reserve();
	//used_symbols.add(sym);
}

static void unary(evalue& e1) {
	switch(ps.p[0]) {
	case '-':
		ps.p++;
		if(ps.p[0] == '-') {
			next(ps.p + 1);
			unary(e1);
			evalue e2(&e1); e2.set(1);
			binary_operation(e2, '=', '-');
		} else {
			next(ps.p + 1);
			unary(e1);
			unary_operation(e1, '-');
		}
		break;
	case '+':
		ps.p++;
		if(ps.p[0] == '+') {
			next(ps.p + 1);
			unary(e1);
			evalue e2(&e1); e2.set(1);
			binary_operation(e2, '=', '+');
		} else {
			next(ps.p + 1);
			unary(e1);
		}
		break;
	case '!':
		next(ps.p + 1);
		unary(e1);
		unary_operation(e1, '!');
		break;
	case '*':
		next(ps.p + 1);
		unary(e1);
		unary_operation(e1, '*');
		break;
	case '&':
		next(ps.p + 1);
		unary(e1);
		unary_operation(e1, '&');
		break;
	case '(':
		if(!direct_cast(e1)) {
			next(ps.p + 1);
			evalue e2(&e1);
			expression(e2);
			skip(')');
		}
		break;
	case '\"':
		ps.p++;
		e1.set(next_string());
		e1.result = i8[0].reference();
		break;
	case '\'':
		ps.p++;
		e1.set(next_string());
		e1.result = i8;
		break;
	case '0':case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':
		e1.set(next_number());
		break;
	default:
		if(match("sizeof")) {
			skip('(');
			c2::symbol* sym = expression_const_type();
			e1.set(sym->size);
			skip(')');
		} else if(match("this"))
			e1.set(getmodule());
		else if(match("true"))
			e1.set(1);
		else if(match("false"))
			e1.set(0);
		else if(ischa(*ps.p)) {
			const char* n = szdup(identifier());
			c2::symbol* sym = findsymbol(n);
			if(!sym)
				status(ErrorNotFound1p2p, "identifier", n);
			else
				register_used_symbol(sym);
			e1.set(sym);
		}
		break;
	}
	postfix(e1);
}

static void multiplication(evalue& e1) {
	unary(e1);
	while((ps.p[0] == '*' || ps.p[0] == '/' || ps.p[0] == '%') && ps.p[1] != '=') {
		char s = ps.p[0];
		next(ps.p + 1);
		evalue e2(&e1);
		unary(e2);
		//gen::cast();
		binary_operation(e2, s);
	}
}

static void addiction(evalue& e1) {
	multiplication(e1);
	while((ps.p[0] == '+' || ps.p[0] == '-') && ps.p[1] != '=') {
		char s = ps.p[0];
		next(ps.p + 1);
		evalue e2(&e1);
		multiplication(e2);
		//gen::cast();
		binary_operation(e2, s);
	}
}

static void binary_cond(evalue& e1) {
	addiction(e1);
	while((ps.p[0] == '>' && ps.p[1] != '>')
		|| (ps.p[0] == '<' && ps.p[1] != '<')
		|| (ps.p[0] == '=' && ps.p[1] == '=')
		|| (ps.p[0] == '!' && ps.p[1] == '=')) {
		char t1 = *ps.p++;
		char t2 = 0;
		if(ps.p[0] == '=')
			t2 = *ps.p++;
		next(ps.p);
		evalue e2(&e1);
		addiction(e2);
		//gen::cast();
		binary_operation(e2, t1, t2);
	}
}

static void binary_and(evalue& e1) {
	binary_cond(e1);
	while(ps.p[0] == '&' && ps.p[1] != '&') {
		next(ps.p + 2);
		evalue e2(&e1);
		binary_cond(e2);
		//gen::cast();
		binary_operation(e2, '&');
	}
}

static void binary_xor(evalue& e1) {
	binary_and(e1);
	while(ps.p[0] == '^') {
		next(ps.p + 1);
		evalue e2(&e1);
		binary_and(e2);
		//gen::cast();
		binary_operation(e2, '^');
	}
}

static void binary_or(evalue& e1) {
	binary_xor(e1);
	while(ps.p[0] == '|' && ps.p[1] != '|') {
		next(ps.p + 1);
		evalue e2(&e1);
		binary_or(e2);
		//gen::cast();
		binary_operation(e2, '|');
	}
}

static void binary_shift(evalue& e1) {
	binary_or(e1);
	while((ps.p[0] == '>' && ps.p[1] == '>') || (ps.p[0] == '<' && ps.p[1] == '<')) {
		char t1 = ps.p[0];
		char t2 = ps.p[1];
		next(ps.p + 2);
		evalue e2(&e1);
		binary_or(e2);
		//gen::cast();
		binary_operation(e2, t1, t2);
	}
}

static void logical_and(evalue& e1) {
	binary_shift(e1);
	int t = 0;
	bool was = false;
	while(ps.p[0] == '&' && ps.p[1] == '&') {
		next(ps.p + 2);
		t = testcondition(1, t);
		evalue e2(&e1);
		binary_shift(e2);
		was = true;
	}
	//if(was)
	//	gen::pushl(FJmpI, t);
}

static void logical_or(evalue& e1) {
	logical_and(e1);
	int t = 0;
	bool was = false;
	while(ps.p[0] == '|' && ps.p[1] == '|') {
		next(ps.p + 2);
		t = testcondition(0, t);
		evalue e2(&e1);
		logical_and(e2);
		was = true;
	}
	//if(was)
	//	gen::pushl(FJmp, t);
}

static void assigment(evalue& e1) {
	expression(e1);
	if((ps.p[0] == '=' && ps.p[1] != '=')
		|| ((ps.p[0] == '+' || ps.p[0] == '-' || ps.p[0] == '/' || ps.p[0] == '*') && ps.p[1] == '=')) {
		char t2 = *ps.p++;
		char t1 = '=';
		if(t2 == '=')
			t2 = 0;
		next(ps.p + 1);
		evalue e2(&e1);
		assigment(e2);
		binary_operation(e2, t1, t2);
	}
}

static void assigment() {
	evalue e1;
	assigment(e1);
}

static unsigned align_symbol_offset(unsigned start, unsigned size, bool negative_rellocate, int align_size) {
	if(align_size > 1)
		size = ((size + 3) & 0xFFFFFFFC);
	if(negative_rellocate)
		return start - size;
	else
		return start + size;
}

static void instance(symbol* variable, bool runtime_assigment, bool allow_assigment, bool locale_variable, int align_size) {
	bool was_initialized = (*ps.p == '=');
	if(variable->isfunction()) {
		status(ErrorExpected1p, "variable instance");
		return;
	} else if(variable->isstatic()) {
		// Static members can be in initialized section or uninitialized.
		// Predict this by assigment passed after initialization.
		if(was_initialized)
			variable->section = section::find(section_data);
		else
			variable->section = section::find(section_bbs);
	} else {
		// This is not static member case. Can be local variable, function parameter or module member.
		// Get last element in this visible scope and add his size to calculate base of this element.
		// In case when there is no previous member initialize to zero.
		auto p = variable->getprevious();
		if(p && p != getmodule())
			variable->value = align_symbol_offset(p->value, p->getsize(), locale_variable, align_size);
		else
			variable->value = 0;
	}
	if(*ps.p == '=') {
		if(!allow_assigment)
			status(ErrorInvalid1p2pIn3p, "operator", "=", "after static variable");
		next(ps.p + 1);
		if(runtime_assigment && *ps.p != '{') {
			evalue e1; e1.set(variable);
			evalue e2(&e1);
			assigment(e2);
			binary_operation(e2, '=');
			return;
		}
		initialize(variable, variable->section);
	}
	// Calculate size of single module element. Get size from type of expression.
	variable->size = variable->result->size;
	if(variable->ismember() && !variable->isstatic() && !variable->isfunction()) {
		// Module member change size of his parent.
		auto m = getmodule();
		m->size += variable->getsize();
	}
}

static bool declaration(unsigned flags, bool allow_functions, bool allow_variables, bool runtime_isntance, bool allow_assigment, bool locale_variable, int align_size = 0) {
	symbol* declared;
	const char* p1 = ps.p;
	if(align_size == 0)
		align_size = 4;
	if(!istype(&declared, flags))
		return false;
	while(*ps.p) {
		auto result = parse_pointer(declared);
		auto id = szdup(identifier());
		if(*ps.p == '(' && !allow_functions) {
			ps.p = p1;
			return false;
		} else if(*ps.p != '(' && !allow_variables) {
			ps.p = p1;
			return false;
		}
		if(gen.unique) {
			// Если нашли совпадение выдаем ошибку
		}
		auto m2 = addsymbol(id, result);
		m2->flags = flags;
		m2->count = 1;
		status(StatusDeclare, m2->id);
		if(*ps.p == '(') {
			scopestate push;
			next(ps.p + 1);
			m2->count = 0;
			while(*ps.p) {
				symbol* result;
				unsigned pflags = 0;
				if(istype(&result, pflags)) {
					result = parse_pointer(result);
					auto id = szdup(identifier());
					result = addsymbol(id, result);
					instance(result, false, false, false, 4);
					m2->count++;
				}
				if(*ps.p == ')') {
					next(ps.p + 1);
					break;
				}
				skip(',');
			}
			if(*ps.p == ';') {
				// Forward declaration
				next(ps.p + 1);
			} else {
				scopestate push;
				auto ps_current_member = ps.current_member;
				ps.current_member = m2;
				prologue();
				statement(0, 0, 0, 0);
				retproc();
				epilogue();
				ps.current_member = ps_current_member;
			}
			return true;
		} else if(*ps.p == '[') {
			next(ps.p + 1);
			if(*ps.p == ']') {
				next(ps.p + 1);
				m2->count = -1;
				if(*ps.p != '=')
					m2->count = 0;
			} else {
				m2->count = expression_const();
				skip(']');
			}
		}
		instance(m2, runtime_isntance, allow_assigment, locale_variable, align_size);
		if(*ps.p == ';') {
			next(ps.p + 1);
			break;
		}
		skip(',');
	}
	return true;
}

static void statement(int* ct, int* br, int* cs, int* ds, evalue* cse) {
	if(match(';')) {
		// Empthy statement
	} else if(*ps.p == '{') {
		scopestate push;
		skip('{');
		while(*ps.p) {
			if(*ps.p == '}') {
				next(ps.p + 1);
				break;
			}
			auto psp = ps.p;
			statement(ct, br, cs, ds);
			if(psp == ps.p) {
				status(ErrorExpected1p, "keyword");
				break;
			}
		}
	} else if(match("break")) {
		if(!br)
			status(ErrorKeyword1pUsedWithout2p, "break", "loop");
		skip(';');
		if(br)
			*br = jumpforward(*br);
	} else if(match("continue")) {
		if(!ct)
			status(ErrorKeyword1pUsedWithout2p, "continue", "loop");
		skip(';');
		if(ct)
			jumpback(*ct);
	} else if(match("return")) {
		if(*ps.p == ';') {
			if(getmember() && getmember()->result != v0)
				status(ErrorFunctionMustReturnValue);
		} else {
			evalue e1;
			expression(e1);
			if(getmember())
				e1.cast(getmember()->result);
			skip(';');
		}
		retproc();
	} else if(match("if")) {
		int e = 0;
		while(true) {
			evalue e1;
			skip('(');
			expression(e1);
			skip(')');
			int b = testcondition(1);
			statement(ct, br, cs, ds);
			if(!match("else")) {
				label(b);
				break;
			}
			e = jumpforward(e);
			label(b);
			if(match("if"))
				continue;
			// Else statement - if anything not match
			statement(ct, br, cs, ds);
			break;
		}
		label(e);
	} else if(match("while")) {
		int label_continue = label();
		skip('(');
		expression();
		skip(')');
		int lable_break = testcondition(1);
		statement(&label_continue, &lable_break, 0, 0);
		jumpback(label_continue);
		label(lable_break);
	} else if(match("do")) {
		int label_continue = label();
		int label_break = 0;
		statement(&label_continue, &label_break, 0, 0);
		if(!match("while"))
			status(ErrorExpected1p, "while");
		skip('(');
		expression();
		skip(')');
		testcondition(0, label_continue);
		label(label_break);
	} else if(match("for")) {
		// For loop has his own scope when open braces
		scopestate push;
		skip('(');
		// Initialize loop
		if(!declaration(0, false, true, true, true, true)) {
			evalue e1;
			assigment(e1);
		}
		if(*ps.p == ';')
			skip(';');
		// Generate label continue and condition check
		int label_continue = label();
		int label_break = 0;
		expression();
		label_break = testcondition(1, label_break);
		skip(';');
		// Skip increment block
		const char* p_step = ps.p;
		expression_nocode();
		skip(')');
		// Генерируем блок тела цикла
		statement(&label_continue, &label_break, 0, 0);
		// Генерируем блок инкремента и переход на проверку условия
		const char* p_next = ps.p;
		next(p_step);
		expression();
		next(p_next);
		jumpback(label_continue);
		// Генерируем метку выхода из цикла
		label(label_break);
	} else if(match("switch")) {
		evalue e1;
		skip('(');
		expression(e1);
		skip(')');
		int break_label = 0;
		int case_label = 0;
		int default_label = 0;
		e1.load(Eax);
		statement(ct, &break_label, &case_label, &default_label, &e1);
		label(case_label);
		if(default_label)
			jumpback(default_label);
		label(break_label);
	} else if(match("case")) {
		//gen::dup();
		int v1 = expression_const();
		int v2 = v1;
		skip(':');
		if(cs)
			label(*cs);
		else
			status(ErrorKeyword1pUsedWithout2p, "case", "switch");
		int csm = 0;
		if(v1 == v2) {
			evalue e1(cse); e1.set(v1);
			binary_operation(e1, '=', '=');
			csm = testcondition(0, csm);
		}
		if(cs)
			*cs = csm;
	} else if(match("default")) {
		skip(':');
		if(ds)
			*ds = label();
		else
			status(ErrorKeyword1pUsedWithout2p, "default", "switch");
	} else if(!declaration(0, false, true, true, true, true)) {
		assigment();
		skip(';');
	}
}

static void skip_statement(int* ct, int* br, int* cs, int* ds, evalue* cse = 0) {
	genstate push;
	gen.code = false;
	statement(ct, br, cs, ds, cse);
}

static void block_declaration() {
	while(declaration(0, false, true, false, false, false));
}

static void block_function() {
	while(declaration(0, true, false, false, false, false));
}

static void block_enums() {
	while(match("enum")) {
		identifier();
		symbol* result = i32;
		skip('{');
		int num = 0;
		symbol* t = 0;
		while(*ps.p) {
			if(ischab(*ps.p)) {
				t = addsymbol(identifier(), result);
				t->value = num++;
				t->setpseudoname();
			} else if(*ps.p == '=') {
				next(ps.p + 1);
				num = expression_const();
				if(t)
					t->value = num;
				else
					status(ErrorAssigmentWithoutEnumeratorMember);
			} else if(*ps.p == ',') {
				t = 0;
				next(ps.p + 1);
			} else if(*ps.p == '}') {
				next(ps.p + 1);
				break;
			} else {
				status(ErrorExpectedEnumeratorMember);
				return;
			}
		}
		skip(';');
	}
}

static void block_imports() {
	// block_imports() has recursive calling.
	// Create 'temp' variable with static attribute will reduce stack memory usage.
	static char temp[260];
	while(match("import")) {
		temp[0] = 0;
		while(ps.p[0]) {
			auto pz = identifier();
			if(pz)
				zcat(temp, pz);
			if(*ps.p == '.') {
				zcat(temp, '.');
				next(ps.p + 1);
				continue;
			}
			if(match("as"))
				pz = identifier();
			auto id = szdup(pz);
			auto m = parse_module(temp);
			if(m) {
				m = addsymbol(id, m);
				m->setpseudoname();
			}
			skip(';');
			break;
		}
	}
}

static symbol* parse_module(const char* id) {
	id = szdup(id);
	auto sym = findmodule(id);
	if(sym)
		return sym;
	sym = symbol::addmodule();
	sym->id = id;
	sym->visibility = 0;
	if(!sym->visibility)
		sym->visibility = getfile(sym->id);
	if(!sym->visibility)
		status(ErrorCantFind1pWithName2p, "import module", sym->id);
	scopestate push;
	auto save_ps = ps;
	ps.current_module = sym;
	sym->declared = sym->visibility;
	scope.parent = 0;
	scope.visibility = sym->visibility;
	ps.p = scope.visibility;
	next(ps.p);
	if(!errors)
		block_imports();
	if(!errors) {
		status(StatusStartParse);
		block_enums();
	}
	if(!errors)
		block_declaration();
	if(!errors)
		block_function();
	if(!errors && *ps.p)
		status(ErrorUnexpectedSymbols);
	ps = save_ps;
	return sym;
}

void c2::compile(const char* id) {
	parse_module(id);
	gen.unique = false;
}