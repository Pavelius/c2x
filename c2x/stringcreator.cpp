#include "crt.h"
#include "stringcreator.h"

stringcreator::plugin* stringcreator::plugin::first;

int	stringcreator::plugin::element::compare(const void* p1, const void* p2)
{
	return strcmp(((element*)p1)->name, ((element*)p2)->name);
}

stringcreator::plugin::plugin(element* elements, int size) : elements(elements), count(size / sizeof(element))
{
	seqlink(this);
	qsort(elements, count, sizeof(element), element::compare);
}

stringcreator::plugin::element* stringcreator::plugin::find(const char* name)
{
	element e;
	e.name = name;
	e.param = 0;
	for(auto n = first; n; n = n->next)
	{
		auto p = (element*)bsearch(&e, n->elements, n->count,
			sizeof(element), element::compare);
		if(p)
			return p;
	}
	return 0;
}

void stringcreator::parseidentifier(char* result, const char* result_max, const char* identifier)
{
	if(!plugin::first)
		return;
	auto p = plugin::find(identifier);
	if(p)
		p->proc(result, p->param ? *p->param : 0);
	else
	{
		// Fix error command
		zcat(result, "[-");
		zcat(result, identifier);
		zcat(result, "]");
	}
}

void stringcreator::parsevariable(char* result, const char* result_max, const char** format)
{
	char temp[260];
	auto src = *format;
	int s = 0;
	if(*src == '(')
	{
		*format += 1;
		while(*src && *src!=')')
			src++;
		s = src - *format;
		src++;
	}
	else
	{
		while(*src)
		{
			const char* s1 = src;
			unsigned ch = szget(&src);
			if(!ischa(ch) && !isnum(ch) && ch != '_')
			{
				src = s1;
				break;
			}
		}
		s = src - *format;
	}
	if(s != 0 && s<int(sizeof(temp) - 1))
	{
		memcpy(temp, *format, s);
		temp[s] = 0;
	}
	*format = src;
	parseidentifier(result, result_max, temp);
}

const char* stringcreator::parseformat(char* dst, const char* result_max, const char* src, const char* vl)
{
	if(*src == '%')
	{
		*dst++ = *src++;
		*dst = 0;
		return src;
	}
	*dst = 0;
	bool prefix_plus = false;
	if(*src == '+')
	{
		prefix_plus = true;
		src++;
	}
	if(*src >= '0' && *src <= '9')
	{
		// ≈сли число, просто подставим нужный параметр
		int pn = 0, pnp = 0;
		while(isnum(*src))
			pn = pn * 10 + (*src++) - '0';
		if(src[0] == '.' && isnum(src[1]))
		{
			src++;
			while(isnum(*src))
				pnp = pnp * 10 + (*src++) - '0';
		}
		if(*src == 'i')
		{
			src++;
			auto value = ((int*)vl)[pn - 1];
			if(prefix_plus && value >= 0)
				*dst++ = '+';
			sznum(dst, value, 0, 0, 10);
		}
		else if(*src == 'f')
		{
			src++;
			auto value = ((float*)vl)[pn - 1];
			if(prefix_plus && value >= 0)
				*dst++ = '+';
			sznum(dst, value, pnp);
		}
		else if(*src == 'h')
		{
			src++;
			sznum(dst, ((int*)vl)[pn - 1], 8, "00000000", 16);
		}
		else
		{
			if(((char**)vl)[pn - 1])
				zcpy(dst, ((char**)vl)[pn - 1]);
		}
	}
	else
		parsevariable(dst, result_max, &src);
	return src;
}

void stringcreator::printv(char* result, const char* src, const char* vl)
{
	if(!result)
		return;
	if(!src)
	{
		// Error: No source string
		result[0] = 0;
		return;
	}
	while(true)
	{
		switch(*src)
		{
		case 0:
			*result = 0;
			return;
		case '%':
			src = parseformat(result, 0, src+1, vl);
			result = zend(result);
			break;
		default:
			*result++ = *src++;
			break;
		}
	}
}

void stringcreator::print(char* result, const char* src, ...)
{
	printv(result, src, xva_start(src));
}

void stringcreator::printn(char* result, const char* src, ...)
{
	printv(result, src, xva_start(src));
	if(result[0])
	{
		szupper(result, 1);
		zcat(result, " ");
	}
}

void stringcreator::println(char* result, const char* src, ...)
{
	printv(result, src, xva_start(src));
	if(result[0])
	{
		szupper(result, 1);
		zcat(result, "\n");
	}
}

char* szprintv(char* result, const char* src, const char* vl)
{
	stringcreator e;
	e.printv(result, src, vl);
	return result;
}

char* szprint(char* result, const char* src, ...)
{
	stringcreator e;
	e.printv(result, src, xva_start(src));
	return result;
}