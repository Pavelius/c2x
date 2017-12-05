#include "autogrow.h"
#include "io.h"
#include "files.h"

using namespace c2;

static const char*	valid_extensions[] = {"c2"};
const char*			urls::library;
const char*			urls::project;
const char*			urls::platform;
const char*			urls::output;

struct filecash {
	const char*		id;
	const char*		content;

	operator bool() const {
		return content != 0;
	}

	void clear() {
		if(content)
			delete content;
		content = 0;
		id = 0;
	}

};
static autogrow<filecash, 64>	files;

static filecash* findfile(const char* id) {
	for(auto p = &files; p; p = p->next) {
		for(auto& e : files) {
			if(e.id == id)
				return &e;
		}
	}
	return 0;
}

static char* zreplace(char* result, char s1, char s2) {
	for(int i = 0; result[i]; result++) {
		if(result[i] == '.')
			result[i] = '/';
	}
	return result;
}

static bool ispath(const char* url) {
	return zchr(url, ':') != 0;
}

static char* findfile(char* result, const char* base, const char* url, const char* ext) {
	result[0] = 0;
	if(!ispath(base)) {
		io::file::getdir(result, 260);
		zcat(result, "/");
	}
	szprint(zend(result), "%1/", base);
	auto p = zend(result);
	zcpy(p, url);
	zreplace(p, '.', '/');
	szprint(zend(p), ".%1", ext);
	return result;
}

static char* findfile(char* result, const char* base, const char* url) {
	if(!base)
		return 0;
	for(auto e : valid_extensions) {
		findfile(result, base, url, e);
		if(io::file::exist(result))
			return szurlc(result);
	}
	return 0;
}

char* c2::getfilename(char* result, const char* url) {
	if(findfile(result, urls::project, url))
		return result;
	if(findfile(result, urls::library, url))
		return result;
	return 0;
}

const char* c2::getfile(const char* id) {
	auto p = findfile(id);
	if(!p) {
		char temp[260];
		if(!getfilename(temp, id))
			return 0;
		p = files.add();
		p->id = id;
		p->content = loadt(temp);
	}
	return p->content;
}

void files_cleanup() {
	for(auto pe = &files; pe; pe = pe->next) {
		for(auto& e : files) {
			if(e)
				e.clear();
		}
	}
	files.clear();
}