#pragma once

template <class T, int granularity>
class	poolSS
{
private:
	union element
	{
		element*		next;
		T				data;
	};
	element*			list;
	vector<element*>	blocks;
private:
	void				block_create	()
	{
		// Allocate
		VERIFY				(0==list);
		list				= xr_malloc	(granularity*sizeof(T));
		blocks.push_back	(list);

		// Partition
		for (int it=0; it<(granularity-1); it++)
		{
			element*	E			= list+it;
			E->next					= E+1;
		}
		list[granularity-1].next	= NULL;
	}
public:
	poolSS()
	{
		list				= 0;
		blocks.reserve		(256);
	}
	~poolSS()
	{
		for (u32 b=0; b<blocks.size())
			_FREE(blocks[b]);
	}
	T*					create			()
	{
		if (0==list)	block_create();

		element* E		= list;
		list			= list->next;
		return			new (E) T();
	}
	void				destroy			(T* &P)
	{
		P->~P			();
		element* E		= static_cast<element*>(P);
		E->next			= list;
		list			= E;
		P				= NULL;
	}
};
