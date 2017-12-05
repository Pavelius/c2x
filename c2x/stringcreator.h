#pragma once

struct stringcreator
{
	// Custom print tokens set
	struct plugin
	{
		struct element
		{
			const char*		name;
			char*			(*proc)(char* result, int param);
			int*			param;
			//
			static int		compare(const void* p1, const void* p2);
		};
		element*			elements;
		int					count;
		plugin*				next;
		static plugin*		first;
		//
		static element*		find(const char* name);
		static char*		get(char* result, const char* name, int param);
		plugin(element* elements, int size);
	};
	const char*				parseformat(char* result, const char* result_max, const char* format, const char* format_param);
	virtual void			parseidentifier(char* result, const char* result_max, const char* identifier);
	virtual void			parsevariable(char* result, const char* result_max, const char** format);
	void					printv(char* result, const char* format, const char* format_param);
	void					print(char* result, const char* format, ...);
	void					printn(char* result, const char* format, ...);
	void					println(char* result, const char* format, ...);
};