#include "stdafx.h"
#include "hudmanager.h"

void xrServer::OnCL_Disconnected	(IClient* CL)
{
	csPlayers.Enter			();
	Level().HUD()->outMessage(0xffffffff,"SERVER","Player '%s' disconnected",CL->Name);

	NET_Packet			P;
	u32				mode			= net_flags(TRUE,TRUE);

	// Game config (all, info excludes deleted player)
	game->OnPlayerDisconnect(CL->ID);
	game->signal_Syncronize	();

	// Migrate entities
	xrS_entities::iterator	I=entities.begin(),E=entities.end();
	for (; I!=E; I++)
	{
		xrServerEntity*	E	= I->second;
		if (E->owner == CL)		PerformMigration	(E,(xrClientData*)CL,SelectBestClientToMigrateTo(E));
	}
	csPlayers.Leave			();
}
