#include "stdafx.h"
#include "hudmanager.h"

void xrServer::OnCL_Disconnected	(IClient* CL)
{
	csPlayers.Enter			();
	HUD().outMessage		(0xffffffff,"SERVER","Player '%s' disconnected",CL->Name);

	// Game config (all, info includes deleted player now, excludes at the next cl-update)
	game->OnPlayerDisconnect(CL->ID);
	game->signal_Syncronize	();

	//
	xrS_entities::iterator	I=entities.begin(),E=entities.end();
	if (client_Count()>1)
	{
		// Migrate entities
		for (; I!=E; I++)
		{
			CAbstractServerObject*	entity		= I->second;
			if (entity->owner == CL)	PerformMigration	(entity,(xrClientData*)CL,SelectBestClientToMigrateTo(entity,TRUE));
		}
	} else {
		// Destroy entities
		for (; I!=E; I++)
		{
			CAbstractServerObject*	entity		= I->second;
			entities.erase	(entity->ID);
			entity_Destroy	(entity);
		}
	}
	csPlayers.Leave			();
}
