#include "c2.h"

using namespace c2;

void c2::errorv(message_s m, const symbol* module, const symbol* member, const char* parameters) {
	char temp[4096];
	szprintv(temp, getstr(m), parameters);
	printcnf("Error: ");
	printcnf(temp);
	printcnf("\n");
}

int main(int argc, char *argv[]) {
	urls::project = "c2code/projects/anytest";
	urls::library = "c2code/library";
	backend = evalue::plugin::find("asm");
	symbol::initialize();
	compile(szdup("main"));
	section::find(section_code)->write("result.txt");
	return 0;
}