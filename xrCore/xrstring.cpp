#include "stdafx.h"
#pragma hdrstop

#include "xrstring.h"

XRCORE_API	extern		str_container*	g_pStringContainer	= NULL;

#define		HEADER		8

str_value*	str_container::dock		(str_c value)
{
	if ((0==value) || (0==value[0]))	return 0;

	cs.Enter					();

	str_value*	result			= 0;

	// calc len
	u32		s_len				= xr_strlen(value);
	u32		s_len_with_zero		= (u32)s_len+1;
	VERIFY	(HEADER+s_len_with_zero < 4096);

	// setup find structure
	string4096	tempstorage;
	str_value*	sv				= (str_value*)tempstorage;
	sv->dwReference				= 0;
	sv->dwLength				= s_len;
	Memory.mem_copy				(sv->value,value,s_len_with_zero);
	
	// search
	cdb::iterator	I			= container.find	(sv);
	if (I!=container.end())		result	= *I;
	else {
		// Insert string
		result					= (str_value*)xr_malloc(HEADER+s_len_with_zero);
		Memory.mem_copy			(result,sv,HEADER+s_len_with_zero);
		container.insert		(result);
	}
	cs.Leave					();

	return	result;
}

void		str_container::clean	()
{
	cs.Enter	();
	cdb::iterator	it	= container.begin	();
	cdb::iterator	end	= container.end		();
	for (; it!=end; )
	{
		str_value*	sv = *it;
		if (0==sv->dwReference)	
		{
			cdb::iterator	i_current	= it;
			cdb::iterator	i_next		= ++it;
			xr_free			(sv);
			container.erase	(i_current);
			it							= i_next;
		} else {
			it++;
		}
	}
	cs.Leave	();
}
void		str_container::dump	()
{
	cs.Enter	();
	cdb::iterator	it	= container.begin	();
	cdb::iterator	end	= container.end		();
	for (; it!=end; it++)
		Msg	("%4d : %s",(*it)->dwReference,(*it)->value);
	cs.Leave	();
}
u32			str_container::stat_economy		()
{
	cs.Enter	();
	cdb::iterator	it		= container.begin	();
	cdb::iterator	end		= container.end		();
	s32				counter	= 0;
	for (; it!=end; it++)	{
		counter		+=		(*it)->dwReference * (*it)->dwLength;
		counter		-=		(*it)->dwLength;
	}
	cs.Leave	();

	return		u32(counter);
}
str_container::~str_container		()
{
	clean	();
	//R_ASSERT(container.empty());
}
