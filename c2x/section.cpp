#include "c2.h"

using namespace c2;

struct data_segment : section, public aref<unsigned char> {

	unsigned rvabase;

	data_segment() {
		initialize();
	}

	void add(unsigned char a) override {
		if(!gen.code)
			return;
		reserve(count + 1);
		data[count++] = a;
	}

	unsigned get() override {
		return count;
	}

	void set(unsigned count) override {
		if(!count && this->count == 0)
			return;
		reserve(count);
	}

	unsigned char* getdata() override {
		return data;
	}

	void clear() override {
		aref<unsigned char>::clear();
	}

	data_segment(const char* id) {
		this->id = id;
	}

};

struct bbs_segment : section {

	unsigned count;

	bbs_segment() : count(0) {
		id = section_bbs;
		rvabase = 0;
	}

	void add(unsigned char a) override {
		if(!gen.code)
			return;
		count++;
	}

	unsigned get() override {
		return count;
	}

	unsigned char* getdata() override {
		return 0;
	}

	void set(unsigned count) override {
		this->count = count;
	}

	void clear() override {
		count = 0;
	}

};
static data_segment	instance_code(section_code);
static data_segment	instance_data(section_data);
static data_segment	instance_strings(section_strings);
static bbs_segment	instance_bss;
static section*		predefined_sections[] = {&instance_code, &instance_data, &instance_strings, &instance_bss};

section* section::find(const char* id) {
	for(auto p : predefined_sections) {
		if(strcmp(p->id, id) == 0)
			return p;
	}
	return 0;
}

void segments_cleanup() {
	for(auto& e : predefined_sections)
		e->clear();
}