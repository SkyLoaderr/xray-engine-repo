#include "stdafx.h"

xrClientData* xrServer::SelectBestClientToMigrateTo	(xrServerEntity* E)
{
	// DUMB SELECTION
	if (E->owner)	return E->owner;
	else			return (xrClientData*)net_Players[::Random.randI(0,net_Players.size())];
}
