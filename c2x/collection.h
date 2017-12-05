#pragma once

struct collection
{
	virtual void*		add(const void* element = 0) = 0; // Add new element to collection
	virtual void		clear() = 0; // Clear all elements in collection
	virtual void*		get(int index) const = 0; // Get content of element with index
	virtual unsigned	getcount() const = 0; // Get count of elements in collection
	virtual int			indexof(const void* element) const = 0; // Get index of element (-1 if not in collection)
	virtual void		remove(int index, int count = 1) = 0; // Remove element from collection
	void				sort(int i1, int i2, int(*compare)(const void* p1, const void* p2, void* param), void* param);	// Sort collection
	virtual void		swap(int i1, int i2) = 0; // Swap elements
};