#include "stdafx.h"

xrClientData* xrServer::SelectBestClientToMigrateTo	(xrServerEntity* E)
{
	// DUMB SELECTION
	return E->owner;
}
