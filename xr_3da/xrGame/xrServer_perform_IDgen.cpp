#include "stdafx.h"

u16		xrServer::PerformIDgen		(u16 ID)
{
	if (0xffff==ID)		
	{
		// Find
		if (ids_used.size())	
		{
			for (vector<bool>::iterator I=ids_used.begin(); I!=ids_used.end(); I++)
			{
				if (!(*I))	{ ID = u16(I-ids_used.begin()); break; }
			}
			if (0xffff==ID)	{
				ID			= u16(ids_used.size	());
				ids_used.push_back			(false);
			}
		} else {
			ID		= 0;
			ids_used.push_back	(false);
		}
	} else {
		// Try to use supplied ID
		if (ID<ids_used.size())
		{
			R_ASSERT		(false==ids_used[ID]);
			ids_used[ID]	= true;
		} else {
			ids_used.resize	(ID+1);
			ids_used[ID]	= true;
		}
	}
	return ID;
}
