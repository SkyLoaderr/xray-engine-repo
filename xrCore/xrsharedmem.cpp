#include "stdafx.h"
#pragma hdrstop

using namespace std;

XRCORE_API	smem_container*	g_pSharedMemoryContainer	= NULL;

smem_value*			smem_container::dock			(u32 dwCRC, u32 dwLength, void* ptr)
{
	R_ASSERT					(dwCRC && dwLength && ptr);

	cs.Enter					();
	smem_value*		result		= 0;

	// search a place to insert
	u8				storage		[4*sizeof(u32)];
	smem_value*		value		= (smem_value*)storage;
	value->dwReference			= 0;
	value->dwCRC				= dwCRC;
	value->dwLength				= dwLength;
	cdb::iterator	it			= std::lower_bound	(container.begin(),container.end(),value,smem_search);
	cdb::iterator	saved_place	= it;
	if (container.end() != it)	{
		// supposedly found
		for (;;	it++)	{
			if (it==container.end())			break;
			if ((*it)->dwCRC	!= dwCRC)		break;
			if ((*it)->dwLength != dwLength)	break;
			if (0==memcmp((*it)->value,ptr,dwLength))
			{
				// really found
				result			= *it;
				break;
			}
		}
	}

	// if not found - create new entry
	if (0==result)
	{
		result					= (smem_value*)	xr_malloc	(4*sizeof(u32) + dwLength);
		result->dwReference		= 0;
		result->dwCRC			= dwCRC;
		result->dwLength		= dwLength;
		Memory.mem_copy			(result->value,ptr,dwLength);
		container.insert		(saved_place,result);
	}

	// exit
	cs.Leave					();
	return						result;
}

void				smem_container::clean			()
{
	cs.Enter		();
	cdb::iterator	it	= container.begin	();
	cdb::iterator	end	= container.end		();
	for (; it!=end; it++)	if (0==(*it)->dwReference)	xr_free	(*it);
	container.erase	(remove(container.begin(),container.end(),(smem_value*)0),container.end());
	cs.Leave		();
}

void				smem_container::dump			()
{
	cs.Enter		();
	cdb::iterator	it	= container.begin	();
	cdb::iterator	end	= container.end		();
	for (; it!=end; it++)
		Msg	("%4d : crc[%6x], %d bytes",(*it)->dwReference,(*it)->dwCRC,(*it)->dwLength);
	cs.Leave		();
}

u32					smem_container::stat_economy	()
{
	cs.Enter		();
	cdb::iterator	it		= container.begin	();
	cdb::iterator	end		= container.end		();
	s32				counter	= 0;
	for (; it!=end; it++)	{
		counter		+=		(*it)->dwReference * (*it)->dwLength;
		counter		-=		(*it)->dwLength;
	}
	cs.Leave		();

	return			u32(counter);
}

smem_container::~smem_container	()
{
	clean			();
}
