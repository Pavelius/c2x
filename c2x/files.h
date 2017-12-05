#pragma once

namespace c2 {
	namespace urls {
		extern const char*	library;
		extern const char*	project;
		extern const char*	platform;
		extern const char*	output;
	}
	char*					getfilename(char* result, const char* url); // Get valid name of file bt short path
	const char*				getfile(const char* id); // Get content of file with specified url's id like 'io.base' or 'acces.file'
}