#ifndef xrMemory_POOLh
#define xrMemory_POOLh
#pragma once

class	MEMPOOL
{
private:
	xrCriticalSection	cs;
	u32					s_sector;		// large-memory sector size
	u32					s_element;		// element size, for example 32
	u32					s_count;		// element count = [s_sector/s_element]
	u32					s_offset;		// header size
	u8*					list;
private:
	void**				access			(void* P)	{ return (void**) LPVOID(P);	}
	void				block_create	();
public:
	void				_initialize		(u32 _element, u32 _sector, u32 _header);
	
	IC u32				get_element		()	{ return s_element; }

	IC void*			create			()
	{
		cs.Enter		();
		if (0==list)	block_create();

		void* E			= list;
		list			= (u8*)*access(list);
		cs.Leave		();
		return			E;
	}
	IC void				destroy			(void* &P)
	{
		cs.Enter		();
		*access(P)		= list;
		list			= (u8*)P;
		cs.Leave		();
	}
};
#endif
