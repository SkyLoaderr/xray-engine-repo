#include "stdafx.h"
#include "xrServer.h"
#include "hudmanager.h"

void xrServer::Process_spawn(NET_Packet& P, DPNID sender, BOOL bSpawnWithClientsMainEntityAsParent)
{
	// read spawn information
	string64			s_name;
	P.r_string			(s_name);

	// create server entity
	xrClientData* CL	= ID_to_client	(sender);
	xrServerEntity*	E	= entity_Create	(s_name); R_ASSERT2(E,"Can't create entity.");
	E->Spawn_Read		(P);
	if (!((game->type==E->s_gameid)||(GAME_ANY==E->s_gameid)))
	{
		Msg				("- SERVER: Entity [%s] incompatible with current game type.",E->s_name);
		F_entity_Destroy(E);
		return;
	}

	// check if we can assign entity to some client
	if (0==CL && !net_Players.empty())
	{
		CL	= SelectBestClientToMigrateTo	(E);
	}

	// check for respawn-capability and create phantom as needed
	if (E->RespawnTime && (0xffff==E->ID_Phantom))
	{
		// Create phantom
		xrServerEntity* Phantom	=	entity_Create	(s_name); R_ASSERT(Phantom);
		Phantom->Spawn_Read		(P);
		Phantom->ID				=	PerformIDgen	(0xffff);
		Phantom->ID_Phantom		=	Phantom->ID;						// Self-linked to avoid phantom-breeding
		Phantom->owner			=	NULL;
		entities.insert			(make_pair(Phantom->ID,Phantom));

		Phantom->s_flags		|=	M_SPAWN_OBJECT_PHANTOM;

		// Spawn entity
		E->ID					=	PerformIDgen(E->ID);
		E->ID_Phantom			=	Phantom->ID;
		E->owner				=	CL;
		entities.insert			(make_pair(E->ID,E));
	} else {
		if (E->s_flags & M_SPAWN_OBJECT_PHANTOM)
		{
			// Clone from Phantom
			E->ID					=	PerformIDgen(0xffff);
			E->owner				=	CL		= SelectBestClientToMigrateTo	(E);
			E->s_flags				&=	~M_SPAWN_OBJECT_PHANTOM;
			entities.insert			(make_pair(E->ID,E));
		} else {
			// Simple spawn
			if (bSpawnWithClientsMainEntityAsParent)
			{
				R_ASSERT				(CL);
				xrServerEntity* P		= CL->owner;
				R_ASSERT				(P);
				E->ID_Parent			= P->ID;
			}
			E->ID					=	PerformIDgen(E->ID);
			E->owner				=	CL;
			entities.insert			(make_pair(E->ID,E));
		}
	}

	// PROCESS NAME; Name this entity
	if (CL && (E->s_flags&M_SPAWN_OBJECT_ASPLAYER))
	{
		CL->owner		= E;
		// strcpy		(E->s_name_replace,CL->Name);
	}

	// PROCESS RP;	 3D position/orientation
	PerformRP				(E);
	E->s_RP					= 0xFE;	// Use supplied

	// Parent-Connect
	if (E->ID_Parent!=0xffff)
	{
		xrServerEntity*		e_parent	= ID_to_entity(E->ID_Parent);
		R_ASSERT						(e_parent);
		e_parent->children.push_back	(E->ID);
	}

	// create packet and broadcast packet to everybody
	NET_Packet				Packet;
	if (CL) 
	{
		E->Spawn_Write		(Packet,TRUE	);
		SendTo				(CL->ID,Packet,net_flags(TRUE,TRUE));

		E->Spawn_Write		(Packet,FALSE	);
		SendBroadcast		(CL->ID,Packet,net_flags(TRUE,TRUE));
	} else {
		E->Spawn_Write		(Packet,FALSE	);
		SendBroadcast		(0,		Packet,net_flags(TRUE,TRUE));
	}

	// log
	Msg		("- SERVER: Spawning '%s'(%d,%d,%d) as #%d, on '%s'", E->s_name_replace, E->g_team(), E->g_squad(), E->g_group(), E->ID, CL?CL->Name:"*SERVER*");
}

/*
void spawn_WithPhantom
void spawn_FromPhantom
void spawn_Simple
*/
