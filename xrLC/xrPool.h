#pragma once

template <class T, int granularity>
class	poolSS
{
private:
	T*					list;
	vector<T*>			blocks;
	HANDLE				hHeap;
private:
	T**					access			(T* P)	{ return (T**) LPVOID(P);	}
	void				block_create	()
	{
		// Allocate
		VERIFY				(0==list);
		list				= (T*)		HeapAlloc	(hHeap,0,granularity*sizeof(T));
		blocks.push_back	(list);

		// Partition
		for (int it=0; it<(granularity-1); it++)
		{
			T*		E			= list+it;
			*access(E)			= E+1;
		}
		*access(list+granularity-1)	= NULL;
	}
public:
	poolSS()
	{
		list				= 0;
		blocks.reserve		(256);
		hHeap				= HeapCreate(0,4*granularity*sizeof(T),0);
	}
	~poolSS()
	{
		for (u32 b=0; b<blocks.size(); b++)
			HeapFree(hHeap,0,blocks[b]);
		HeapDestroy			(hHeap);
	}
	T*					create			()
	{
		if (0==list)	block_create();

		T* E			= list;
		list			= *access(list);
		return			new (E) T();
	}
	void				destroy			(T* &P)
	{
		P->~T			();
		*access(P)		= list;
		list			= P;
		P				= NULL;
	}
};
