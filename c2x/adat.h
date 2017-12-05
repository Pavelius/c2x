#include "collection.h"

#pragma once

// Use when we don't want use allocator and must have static data
// Beware - it's not have constructors and destructor
template<class T, int count_max = 128>
struct adat
{
	T						data[count_max];
	unsigned				count;
	//
	inline T&				operator[](int index) { return data[index]; }
	//
	T*						add() { if(count < count_max) return data + (count++); return 0; }
	void					add(const T& e) { if(count < count_max) data[count++] = e; }
	T*						begin() { return data; }
	const T*				begin() const { return data; }
	void					clear() { count = 0; }
	T*						end() { return data + count; }
	const T*				end() const { return data + count; }
	void					initialize() { count = 0; }
	template<class Z> T*	find(Z id) { auto e1 = data + count; for(T* e = data; e < e1; e++) { if(e->id == id) return e; } return 0; }
	inline int				getcount() const { return count; }
	int						indexof(const T* e) const { if(e >= data && e <= data + count) return e - data; return -1; }
	int						indexof(const T t) const { for(unsigned i = 0; i < count; i++) if(data[i] == t) return i; return -1; }
	bool					is(const T t) const { for(unsigned i = 0; i < count; i++) if(data[i] == t) return true; return false; }
	void					remove(int index, int remove_count = 1) { if(index < 0) return; if(index<int(count-1)) memcpy(data + index, data + index + 1, sizeof(data[0])*(count-index-1)); count--; }
	void					swap(int i1, int i2) { T e1 = data[i1]; data[i1] = data[i2]; data[i2] = e1; }
};
template<class T, int count_max = 128>
struct adatc : adat<T, count_max>, collection
{
	void					add(const T& e) { adat::add(e); }
	void*					add(const void* element = 0) override { if(count < count_max) data[count] = (T&)element; return data + count++; }
	void					clear() override { adat::clear(); }
	void*					get(int index) const override { return (T*)data + index; }
	unsigned				getcount() const { return adat::getcount(); }
	int						indexof(const void* element) const override { return adat::indexof((T*)element); }
	void					remove(int index, int count = 1) override { adat::remove(index, count); }
	void					swap(int i1, int i2) { adat::swap(i1, i2); }
};