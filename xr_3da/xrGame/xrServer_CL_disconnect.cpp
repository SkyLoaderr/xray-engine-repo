#include "stdafx.h"
#include "hudmanager.h"
#include "xrserver.h"
#include "game_sv_single.h"
#include "alife_simulator.h"
#include "xrserver_objects.h"

void xrServer::OnCL_Disconnected	(IClient* CL)
{
	csPlayers.Enter			();
///	if (!CL->flags.bLocal)
//	{			
//		HUD().outMessage		(0xffffffff,"SERVER","Player '%s' disconnected",game->get_option_s(CL->Name,"name",CL->Name));
//	};

	// Game config (all, info includes deleted player now, excludes at the next cl-update)
	NET_Packet P;
	P.B.count = 0;
	P.w_clientID(CL->ID);
	P.r_pos = 0;
	ClientID clientID;clientID.set(0);
//	game->OnEvent(P,GAME_EVENT_PLAYER_DISCONNECTED, 0, clientID);
	game->AddDelayedEvent(P,GAME_EVENT_PLAYER_DISCONNECTED, 0, clientID);
//	if (SV_Client) game->OnPlayerDisconnect(CL->ID);
//	game->signal_Syncronize	();

	//
	xrS_entities::iterator	I=entities.begin(),E=entities.end();
	if (client_Count()>1 && !CL->flags.bLocal)
	{
		// Migrate entities
		for (; I!=E; ++I)
		{
			CSE_Abstract*	entity		= I->second;
//			R_ASSERT(entity->owner->flags.bLocal);
			if (entity->owner == CL)	PerformMigration	(entity,(xrClientData*)CL,SelectBestClientToMigrateTo(entity,TRUE));
		}
	} else {
		// Destroy entities
		while (!entities.empty())		{
			CSE_Abstract*	entity		= entities.begin()->second;
			entity_Destroy	(entity);
		}
	}
	csPlayers.Leave			();

	Server_Client_Check(CL);
}
