#include "stdafx.h"

xrClientData* xrServer::SelectBestClientToMigrateTo	(xrServerEntity* E, BOOL bForceAnother)
{
	if (bForceAnother)
	{
		// DUMB SELECTION - NOT THE CURRENT OWNER
		for (u32 it=0; it<net_Players.size(); it++)
		{
			if (E->owner!=net_Players[it])	(xrClientData*)return net_Players[it];
		}
	} else {
		// DUMB SELECTION
		if (E->owner)	return E->owner;
		else			return (xrClientData*)net_Players[::Random.randI(0,net_Players.size())];
	}
}
