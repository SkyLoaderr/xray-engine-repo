#pragma once

#include "xrMemory_pso.h"

class XRCORE_API	xrMemory
{
public:
	void			_initialize	();
	void			_destroy	();
public:
	u32				mem_usage	();
	void			mem_compact ();

	void*			mem_alloc	(size_t	size							);
	void			mem_free	(void*	p								);
	void*			mem_realloc	(void*	p, size_t size					);

	pso_MemCopy*	mem_copy;
	pso_MemFill*	mem_fill;
	pso_MemFill32*	mem_fill32;
};
extern XRCORE_API	xrMemory	Memory;

#undef ZeroMemory
#undef CopyMemory
#undef FillMemory
#define ZeroMemory(a,b)	Memory.mem_fill(a,0,b)
#define CopyMemory(a,b,c) Memory.mem_copy(a,b,c)
#define FillMemory(a,b,c) Memory.mem_fill(a,c,b)

// new(0)
template <class T>
IC	T*		xr_new		()
{
	T* ptr	= (T*)Memory.mem_alloc(sizeof(T));
	return new (ptr) T();
}
// new(1)
template <class T, class P1>
IC	T*		xr_new		(P1 p1) {
	T* ptr	= (T*)Memory.mem_alloc(sizeof(T));
	return new (ptr) T(p1);
}
// new(2)
template <class T, class P1, class P2> 
IC	T*		xr_new		(P1 p1, P2 p2) {
	T* ptr	= (T*)Memory.mem_alloc(sizeof(T));
	return new (ptr) T(p1,p2);
}
// new(3)
template <class T, class P1, class P2, class P3>
IC	T*		xr_new		(P1 p1, P2 p2, P3 p3) {
	T* ptr	= (T*)Memory.mem_alloc(sizeof(T));
	return new (ptr) T(p1,p2,p3);
}
// new(4)
template <class T, class P1, class P2, class P3, class P4>
IC	T*		xr_new		(P1 p1, P2 p2, P3 p3, P4 p4) {
	T* ptr	= (T*)Memory.mem_alloc(sizeof(T));
	return new (ptr) T(p1,p2,p3,p4);
}
// new(5)
template <class T, class P1, class P2, class P3, class P4, class P5>
IC	T*		xr_new		(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) {
	T* ptr	= (T*)Memory.mem_alloc(sizeof(T));
	return new (ptr) T(p1,p2,p3,p4,p5);
}
// new(6)
template <class T, class P1, class P2, class P3, class P4, class P5, class P6>
IC	T*		xr_new		(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6) {
	T* ptr	= (T*)Memory.mem_alloc(sizeof(T));
	return new (ptr) T(p1,p2,p3,p4,p5,p6);
}
// new(7)
template <class T, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
IC	T*		xr_new		(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7) {
	T* ptr	= (T*)Memory.mem_alloc(sizeof(T));
	return new (ptr) T(p1,p2,p3,p4,p5,p6,p7);
}

// delete
template <class T>
IC	void	xr_delete	(T* &ptr)
{
	if (ptr) 
	{
		ptr->~T();
		ptr = NULL;
	}
}

// generic "C"-like allocations/deallocations
template <class T>
IC T*		xr_alloc	(u32 count)				{	return  (T*)Memory.mem_alloc(count*sizeof(T));	}

template <class T>
IC void		xr_free		(T* &P)					{	if (P) { Memory.mem_free((void*)P); P=NULL;	};	}

IC void*	xr_malloc	(size_t size)			{	return	Memory.mem_alloc(size);					}

IC void*	xr_realloc	(void* P, size_t size)	{	return Memory.mem_realloc(P,size);				}

XRCORE_API char*		xr_strdup	(const char* string);
