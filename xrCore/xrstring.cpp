#include "stdafx.h"
#include "xrstring.h"

XRCORE_API	extern		str_container*	g_pStringContainer	= NULL;

str_value*	str_container::dock		(str_c value)
{
	cs.Enter					();

	str_value*	result			= 0;

	// calc len
	u32		s_len_with_zero		= strlen(value)+1;
	VERIFY	(4+s_len_with_zero < 4096);

	// setup find structure
	string4096	tempstorage;
	str_value*	sv				= (str_value*)tempstorage;
	sv->dwReference				= 0;
	Memory.mem_copy				(sv->value,value,s_len_with_zero);
	
	// search
	cdb::iterator	I			= container.find	(sv);
	if (I!=container.end())		result	= *I;
	else {
		// Insert string
		result					= (str_value*)xr_malloc(4+s_len_with_zero);
		Memory.mem_copy			(result,sv,4+s_len_with_zero);
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

str_container::~str_container		()
{
	clean	();
	R_ASSERT(container.empty());
}
