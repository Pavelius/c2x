#include "type.h"
#include "evalue.h"

using namespace c2;
using namespace gen;

// Generate a modrm reference - second byte in most intel instructions.
// 'op_reg' contains the addtionnal 3 opcode bits
static void modrm(int op_reg, evalue& e)
{
	//r8(/r)                 	   AL    CL    DL    BL    AH    CH    DH    BH
	//r16(/r)                	   AX    CX    DX    BX    SP    BP    SI    DI
	//r32(/r)                	   EAX   ECX   EDX   EBX   ESP   EBP   ESI   EDI
	///digit (Opcode)        	   0     1     2     3     4     5     6     7
	//REG =                  	   000   001   010   011   100   101   110   111
	//
	//	[EAX]                000   00    08    10    18    20    28    30    38
	//	[ECX]                001   01    09    11    19    21    29    31    39
	//	[EDX]                010   02    0A    12    1A    22    2A    32    3A
	//	[EBX]                011   03    0B    13    1B    23    2B    33    3B
	//	[--] [--]        00  100   04    0C    14    1C    24    2C    34    3C
	//	disp32               101   05    0D    15    1D    25    2D    35    3D
	//	[ESI]                110   06    0E    16    1E    26    2E    36    3E
	//	[EDI]                111   07    0F    17    1F    27    2F    37    3F
	//
	//	disp8[EAX]           000   40    48    50    58    60    68    70    78
	//	disp8[ECX]           001   41    49    51    59    61    69    71    79
	//	disp8[EDX]           010   42    4A    52    5A    62    6A    72    7A
	//	disp8[EPX];          011   43    4B    53    5B    63    6B    73    7B
	//	disp8[--] [--]   01  100   44    4C    54    5C    64    6C    74    7C
	//	disp8[ebp]           101   45    4D    55    5D    65    6D    75    7D
	//	disp8[ESI]           110   46    4E    56    5E    66    6E    76    7E
	//	disp8[EDI]           111   47    4F    57    5F    67    6F    77    7F
	//
	//	disp32[EAX]          000   80    88    90    98    A0    A8    B0    B8
	//	disp32[ECX]          001   81    89    91    99    A1    A9    B1    B9
	//	disp32[EDX]          010   82    8A    92    9A    A2    AA    B2    BA
	//	disp32[EBX]          011   83    8B    93    9B    A3    AB    B3    BB
	//	disp32[--] [--]  10  100   84    8C    94    9C    A4    AC    B4    BC
	//	disp32[EBP]          101   85    8D    95    9D    A5    AD    B5    BD
	//	disp32[ESI]          110   86    8E    96    9E    A6    AE    B6    BE
	//	disp32[EDI]          111   87    8F    97    9F    A7    AF    B7    BF
	//
	//	EAX/AX/AL            000   C0    C8    D0    D8    E0    E8    F0    F8
	//	ECX/CX/CL            001   C1    C9    D1    D9    E1    E9    F1    F9
	//	EDX/DX/DL            010   C2    CA    D2    DA    E2    EA    F2    FA
	//	EBX/BX/BL            011   C3    CB    D3    DB    E3    EB    F3    FB
	//	ESP/SP/AH        11  100   C4    CC    D4    DC    E4    EC    F4    FC
	//	EBP/BP/CH            101   C5    CD    D5    DD    E5    ED    F5    FD
	//	ESI/SI/DH            110   C6    CE    D6    DE    E6    EE    F6    FE
	//	EDI/DI/BH            111   C7    CF    D7    DF    E7    EF    F7    FF
	op_reg = op_reg << 3;
	int r = e.group();
	if(e.is(LValue))
	{
		if(r==Const)
		{
			add(0x05 | op_reg);
			add(e.sym, e.value);
		}
		else if(e.value == 0 && r!=Ebp)
			add((0x00 + r) | op_reg);
		else if(e.value == (char)e.value)
		{
			// short reference
			add((0x40 + r) | op_reg);
			add(e.value);
		}
		else
		{
			add((0x80 + r) | op_reg);
			add32(e.value);
		}
	}
	else
		add(0xC0|op_reg|(r&7));
}

// load 'r' from value 'e'
void c2::gen::load(int r, evalue& e)
{
	tokens g = e.group();
	// Если тот же самый регистр
	if(!e.is(LValue) && g==r)
		return;
	if(!e.is(LValue) && g==Const)
	{
		add(0xB8 + r); // mov reg, XXXX
		add(e.sym, e.value);
	}
	else if(!e.is(LValue) && g<Const && e.value)
	{
		add(0x8D);
		e.set(LValue);
		modrm(r, e);
		e.del(LValue);
	}
	else if(g==FCmp)
	{
		add(0x0F);
		add(0x90 + e.value);
		add(0xC0+r);
	}
	else if(e.result->size == 1)
	{
		add(0x0F); // movsb
		if(e.result == &type::uchar)
			add(0xB6);
		else
			add(0xBE);
		modrm(r, e);
	}
	else if(e.result->size == 2)
	{
		add(0x0F); // movsw
		if(e.result == &type::usint)
			add(0xB7);
		else
			add(0xBF);
		modrm(r, e);
	}
	else
	{
		add(0x8B); // mov
		modrm(r, e);
	}
	//switch(v)
	//{
	//case FJmp:
	//case FJmpI:
	//	//t = v & 1;
	//	//oad(0xb8 + r, t); /* mov $1, r */
	//	//o(0x05eb); /* jmp after */
	//	//gsym(fc);
	//	//oad(0xb8 + r, t ^ 1); /* mov $0, r */
	//	break;
	//}
	e.set(r);
	e.value = 0;
}

// Save register 'r' in value 'e'
void c2::gen::load(evalue& e, int r)
{
	if(e.result->size == 1)
		add(0x88);
	else
		add(0x89);
	modrm(r, e);
}

void c2::gen::savereg(int r)
{
	int offset = 0;
	for(evalue* p = c2::stack; p <= top; p++)
	{
		if(p->group()==r)
		{
			if(!offset)
			{
				// Если 'sym'=0 то это локальная переменная
				// относительно регистра [ebp]
				type sr;
				sr.clear();
				sr.set(Auto);
				sr.result = &type::xint;
				instance(&sr, false);
				offset = sr.value;
				p->set(Ebp);
				p->set(LValue);
				p->value = offset;
				p->sym = 0;
				load(*p, r);
			}
			else
			{
				p->set(Ebp);
				p->set(LValue);
				p->value = offset;
				p->sym = 0;
			}
		}
	}
}

static evalue* find_value(int r, int m)
{
	if(!top)
		return 0;
	for(evalue* p = c2::stack; p <= top; p++)
	{
		if(p->group()==r)
			return p;
	}
	return 0;
}

static int getreg()
{
	for(int i = Eax; i <= Edx; i++)
	{
		evalue* p = find_value(i, FTypeMask);
		if(!p)
			return i;
	}
	savereg(Eax);
	return Eax;
}

// Move one value into another
void c2::gen::move(int n1, int n2)
{
	evalue& e1 = top[n1];
	evalue& e2 = top[n2];
	if(!e2.is(LValue) && e2.group()==Const)
	{
		if(e1.result->size == 1)
		{
			add(0xC6);
			modrm(0, e1);
			add((unsigned char)e2.value);
		}
		else
		{
			add(0xC7);
			modrm(0, e1);
			add(e2.sym, e2.value);
		}
	}
	else if(e2.group()==FCmp)
	{
		load(getreg(), e2);
		load(e1, e2.group());
	}
	else
	{
		rvalue(n2);
		load(e1, e2.group());
	}
}

void c2::gen::rvalue(int n)
{
	evalue& e = top[n];
	if(e.is(LValue) || (e.group()<Const && e.value))
		load(getreg(), e);
}

int get_jmp_opcode(char t1, char t2)
{
	//JO=0,JNO=1,JB= 2,JAE= 3,JZ= 4,JNZ= 5,JBE= 6,JA= 7,
	//JS=8,JNS=9,JP=10,JNP=11,JL=12,JGE=13,JLE=14,JG=15,
	switch(t1)
	{
	case '>':
		if(t2=='=')
			return 13;
		if(t2==0)
			return 15;
		break;
	case '<':
		if(t2=='=')
			return 14;
		if(t2==0)
			return 12;
		break;
	case '=':
		if(t2=='=')
			return 4;
		break;
	case '!':
		if(t2=='=')
			return 5;
		break;
	}
	return 0;
}

int c2::gen::test(int inv, int bp)
{
	int create_label = -1;
	tokens g = top[0].group();
	// Оптимизация выражения && и ||
	if(g==FJmp || g==FJmpI)
	{
		if((g-FJmp)==inv)
		{
			// Пройдемся до самого первого условия в списке меток
			// и поставим указанную метку в начало списка
			// затем заменик текущую метку на метку из стэка (хвост)
			int p = top[0].value;
			while(p && *((int*)(section::code.data+p)))
				p = *((int*)(section::code.data+p));
			if(p)
				*((int*)(section::code.data+p)) = bp;
			bp = top[0].value;
		}
		else
		{
			// Надо создать метку на конец этого блока условий
			// Учитывая что ниже будет генериться код, надо создать метку после его генерации
			create_label = top[0].value;
		}
		gen::pop();
		g = top[0].group();
	}
	if(g == FCmp)
	{
		// Простой случай: можем перейти непосредственно
		// так как флаги уже установлены и куда перейти мы знаем
		int i = top[0].value^inv;
		add(0x0F);
		add(0x80 + i);
		bp = sym(bp);
	}
	else if(!top[0].is(LValue) && top[0].isconst())
	{
		// Если выражением является константа,
		// в случае истины не генерим никагого кода
		// в случае лжи генерим переход в конец условия (и получаем кусок мертвого кода)
		if((top[0].value!=0) != inv)
			bp = jumpf(bp);
	}
	else
	{
		// Проверить результат выражения а равен ли он нулю.
		// Если он равен нулю выполнить переход.
		rvalue(0);
		int r = top[0].group();
		add(0x85); // test
		add(0xC0 + r*8 + r);
		add(0x0F); // jz/jnz
		add(inv ? 0x84 : 0x85);
		bp = sym(bp);
	}
	pop();
	if(create_label!=-1)
		gen::label(create_label);
	return bp;
}

int c2::gen::jumpf(int sym)
{
	if(disable::ret)
		return sym;
	add(0xE9);
	return gen::sym(sym);
}

int c2::gen::label()
{
	disable::ret = false;
	return ptr();
}

void c2::gen::label(int bp)
{
	if(disable::code)
		return;
	disable::ret = false;
	int a = ptr();
	while(bp)
	{
		unsigned* ptr = (unsigned*)&section::code.data[bp];
		int n = *ptr; // next value
		*ptr = a - bp - 4;
		bp = n;
	}
}

void c2::gen::jumpb(int sym)
{
	int r = sym - ptr() - 2;
	if(r == (char)r)
	{
		add(0xEB);
		add(r);
	}
	else
	{
		add(0xE9);
		add32(sym - ptr() - 5);
	}
}

static int op_group(char t1, char t2)
{
	switch(t1)
	{
	case '+':
		if(t2=='+')
			return 0;
		return 0;
	case '.':
		return 0;
	case '|':
		return 1;
	case '&':
		return 4;
	case '-':
		if(t2=='-')
			return 1;
		return 5;
	case '^':
		return 6;
	case '<':
		if(t2=='<')
			return 4;
		break;
	case '>':
		if(t2=='>')
			return 5;
		break;
	default:
		break;
	}
	return 7;
}

static bool is_compare(char t1, char t2)
{
	switch(t1)
	{
	case '>':
		return t2=='=' || t2==0;
	case '<':
		return t2=='=' || t2==0;
	case '!':
	case '=':
		return t2=='=';
	default:
		return false;
	}
}

void c2::gen::machine(char t1, char t2, char t3)
{
	if(t1=='=' && t2!='=')
	{
		rvalue(0);
		switch(t2)
		{
		case 0:
			move(-1, 0);
			break;
		case '/':
		case '*':
			swap();
			dup();
			roll(3);
			oper(t2);
			machine('=');
			break;
		default:
			machine(t2, t3, '=');
			break;
		}
	}
	else if(((t1=='+' || t1=='-' || t1=='&' || t1=='|' || t1=='^') && t2==0)
		|| is_compare(t1, t2)
		|| (t1=='>' && t2=='>')
		|| (t1=='<' && t2=='<'))
	{
		if(t3!='=')
		{
			// Левый операнд может быть LValue только в случае
			// когда у нас идет операция с присваиванием.
			// Таким образом, операция '+=' приобразуется в операцию '+'
			// где в качестве левого операнда будет левостороннее значение.
			if(top[-1].is(LValue) || top[-1].isconst() || (top[-1].group()<Const && top[-1].value))
				load(getreg(), top[-1]);
		}
		int opc = op_group(t1, t2);
		if(!top[0].is(LValue) && top[0].group()==Const) // Операция rm1, const
		{
			int c = top[0].value;
			if((t1=='>' && t2=='>') || (t1=='<' && t2=='<'))
			{
				// Константа при операции сдвигов
				add(0xC1); // shl/shr/sar $xxx, r
				modrm(opc, top[-1]);
				add(top[0].value&0xFF);
			}
			else if(c==1 && (t1=='+' || t1=='-'))
			{
				// Быстрый инкремент или декремент
				if(!top[-1].is(LValue) && top[-1].group() < Const)
					add(((t1=='+') ? 0x40 : 0x48) + top[-1].group());
				else
				{
					// Инкремент или декремент значения в памяти
					add(0xFF);
					modrm((t1=='+' ? 0 : 1), top[-1]);
				}
			}
			else if(top[-1].group() == Eax) // Eax register optimization
			{
				add((opc << 3) | 0x05);
				add(top->sym, c);
			}
			else if(c == (char)c)
			{
				add(0x83);
				modrm(opc, top[-1]);
				add(c);
			}
			else
			{
				add(0x81);
				modrm(opc, top[-1]);
				add32(c);
			}
		}
		else if((t1=='>' && t2=='>') || (t1=='<' && t2=='<'))
		{
			// Создадим сдвиг в Ecx
			savereg(Ecx);
			load(Ecx, top[0]);
			add(0xD3); // shl/shr/sar %cl, r */
			modrm(opc, top[-1]);
		}
		else // Операция r1, rm2
		{
			add((opc << 3) | 0x03);
			modrm(top[-1].group(), top[0]);
		}
		// Если сравнение укажем какое именно сравнение
		if(is_compare(t1, t2))
		{
			top[-1].set(FCmp);
			top[-1].value = get_jmp_opcode(t1, t2);
		}
	}
	else if(t1=='*')
	{
		if(!top[0].is(LValue) && top[0].group()==Const)
		{
			savereg(Eax);
			add(0x69);
			modrm(Eax, top[-1]);
			add(top[0].sym, top[0].value);
			top[-1].set(Eax);
		}
		else if(top[-1].is(LValue))
		{
			add(0x0F);
			add(0xAF); // imul fr, r
			modrm(top[0].group(), top[-1]);
		}
		else
		{
			add(0x0F);
			add(0xAF); // imul fr, r
			modrm(top[-1].group(), top[0]);
		}
	}
	else if(t1=='/' || t1=='%') // Деление
	{
		// Операнд должен быть в eax.
		// Также при операции будет задействован Edx.
		savereg(Eax);
		savereg(Edx);
		load(Eax, top[-1]);
		add(0x99); // cdq
		add(0xF7); // div
		modrm(6, top[0]);
		if(t1 == '%')
			top[-1].set(Edx);
		else
			top[-1].set(Eax);
	}
	else if(t1=='.')
		top[-1] = top[0];
}

void c2::gen::param()
{
	if(!top[0].is(LValue) && top[0].value && top[0].group()!=Const)
		load(getreg(), top[0]);
	if(top[0].is(LValue))
	{
		add(0xFF);
		modrm(6, top[0]);
	}
	else if(top[0].group()==Const)
	{
		int c = top[0].value;
		if(top[0].sym || c!=(char)c)
		{
			add(0x68);
			add(top[0].sym, c);
		}
		else
		{
			add(0x6A);
			add(c);
		}
	}
	else if(top[0].group()<Const)
		add(0x50+top[0].group());
}

void c2::gen::begin(type* t)
{
	proc::local = 0;
	proc::localmax = proc::local;
	proc::param = 8; // return address, ebp
	t->value = label();
}

static bool header(type* t)
{
	return t->size || t->child || t->parent->group()==Class;
}

void c2::gen::body(type* t)
{
	if(header(t))
	{
		add(0x55); // push ebp
		add(0x89); // mov ebp,esp
		add(0xE5);
	}
	if(t->size>0)
	{
		add(0x81); // sub esp, stack size
		add(0xEC);
		add32(t->size);
	}
	if(t->parent->group() == Class)
	{
		add(0x53);	// push ebx
		add(0x8B); // mov ebx, [ebp+8]
		add(0x5D);
		add(0x08);
	}
}

void c2::gen::result(type* t)
{
	if(disable::ret)
		return;
	if(!header(t))
	{
		add(0xC3); // ret
		return;
	}
	int r = t->csize(4);;
	if(t->parent->group() == Class)
		add(0x5B); // pop ebx
	add(0xC9); // leave
	if(r && (t->flags&FreeParams) == 0)
	{
		add(0xC2); // ret N
		add16(r);
	}
	else
		add(0xC3); // ret
	disable::ret = true;
}

void c2::gen::end(type* t)
{
	t->size = proc::localmax;
}

void c2::gen::call(type* t, int count)
{
	t->set(RValue);
	if(t->parent && t->parent->parent == &type::platform)
	{
		add(0xFF);
		add(0x15);
		add(t, 0);
	}
	else
	{
		add(0xE8);
		add((type*)0, t->value - label() - type::xint.size);
	}
	if(t->flags&FreeParams)
	{
		int size = count * 4;
		if(size == (char)size)
		{
			add(0x83);
			add(0xC4);
			add(size);
		}
		else
		{
			add(0x81);
			add(0xC4);
			add32(size);
		}
	}
}