#include "collection.h"

#pragma once

// Standart reserve procedure
void* rmreserve(void* ptr, unsigned size, bool focre_to_size);
// Get total count of records.
inline int optimal(int need_count, int mc = 256 * 256)
{
	int m = 64;
	while(m < mc)
	{
		if(need_count < m)
			return m;
		m = m << 1;
	}
	return m;
}
// Simplest array - pointer and count. Used only for POD types.
// Not POD types NEED sublass clear(), reserve(), reserve(n).
template<class T>
struct aref
{
	T*						data;
	unsigned				count;
	//
	inline T&				operator[](int index) { return data[index]; }
	//
	T&						add() { return data[count++]; }
	T&						addr() { reserve(); return data[count++]; }
	void					add(const T& e) { data[count++] = e; }
	template<class Z> T*	addu(Z value) { auto e = find(value); if(e) return e; reserve(); return data + (count++); }
	T*						begin() { return data; }
	const T*				begin() const { return data; }
	void					initialize() { data = 0; count = 0; }
	void					clear() { delete data; initialize(); }
	T*						end() { return data + count; }
	const T*				end() const { return data + count; }
	template<class Z> T*	find(Z value) { for(auto& e : *this) { if(e.id == value) return &e; } return 0; }
	int						indexof(const T* t) const { if(t<data || t>data + count) return -1; return t - data; }
	int						indexof(const T t) const { for(int i = 0; i < count; i++) if(data[i] == t) return i; return -1; }
	void					reserve(int count) { data = (T*)rmreserve(data, sizeof(T)*optimal(count, 256 * 256 * 256), false); }
	void					reserve() { reserve(count + 1); }
	void					remove(int index, int elements_count = 1) { if(index < 0 || index >= count) return; count--; if(index >= count) return; memmove(data + index, data + index + elements_count, sizeof(data[0])*(count - index + elements_count - 1)); }
};
template <class T>
struct arefc : aref<T>, collection
{
	arefc() { initialize(); }
	~arefc() { aref<T>::clear(); }
	void*			add(const void* object) override { auto& m = addr(); if(object) m = *static_cast<const T*>(object); return &m; }
	void			clear() { aref<T>::clear(); }
	unsigned		getcount() const override { return count; }
	void*			get(int index) const override { return data + index; }
	int				indexof(const void* object) const override { return aref<T>::indexof(static_cast<const T*>(object)); }
	void			remove(int index, int elements_count) override { aref<T>::remove(index, elements_count); }
	void			swap(int i1, int i2) override { T a = data[i1]; data[i1] = data[i2]; data[i2] = a; }
};