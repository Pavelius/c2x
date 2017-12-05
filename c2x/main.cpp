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
	c2::compile("C:/Users/p.chistyakov/Source/Repos/c2/c2code/projects/anytest/main.c2");
	return 0;
}