#include "stdafx.h"
#include "xrServer.h"

#include "hudmanager.h"

void xrServer::Process_spawn(NET_Packet& P, DPNID sender)
{
	// read spawn information
	u16					s_respawntime;
	string64			s_name;
	P.r_string			(s_name);

	// create server entity
	xrClientData* CL	= ID_to_client	(sender);
	xrServerEntity*	E	= entity_Create	(s_name); R_ASSERT(E);
	E->Spawn_Read		(P);
	if (!((GAME==E->s_gameid)||(GAME_ANY==E->s_gameid)))
	{
		Msg				("- SERVER: Entity [%s] incompatible with current game type.",E->s_name);
		F_entity_Destroy(E);
		return;
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
		ids_used[Phantom->ID]	=	true;	
		entities.insert			(make_pair(Phantom->ID,Phantom));

		Phantom->s_flags		|=	M_SPAWN_OBJECT_PHANTOM;

		// Spawn entity
		E->ID					=	PerformIDgen(E->ID);
		E->ID_Phantom			=	Phantom->ID;
		E->owner				=	CL;
		ids_used[E->ID]			=	true;	
		entities.insert			(make_pair(E->ID,E));
	} else {
		if (E->s_flags & M_SPAWN_OBJECT_PHANTOM)
		{
			// Clone from Phantom
			E->ID					=	PerformIDgen(0xffff);
			E->owner				=	CL		= SelectBestClientToMigrateTo	(E);
			ids_used[E->ID]			=	true;
			entities.insert			(make_pair(E->ID,E));
		} else {
			// Simple spawn
			E->ID					=	PerformIDgen(E->ID);
			E->owner				=	CL;
			ids_used[E->ID]			=	true;
			entities.insert			(make_pair(E->ID,E));
		}
	}

	// PROCESS NAME; Name this entity
	if (CL && (E->s_flags&M_SPAWN_OBJECT_ASPLAYER))
	{
		CL->owner		= E;
		strcpy			(E->s_name_replace,CL->Name);
	}

	// PROCESS RP;	 3D position/orientation
	PerformRP				(E);
	E->s_RP					= 0xFE;	// Use supplied

	// create packet and broadcast packet to everybody
	NET_Packet				Packet;
	if (CL) 
	{
		E->Spawn_Write		(Packet,TRUE	);
		SendTo				(sender,Packet,net_flags(TRUE));
	}

	E->Spawn_Write			(Packet,FALSE	);
	SendBroadcast			(sender,Packet,net_flags(TRUE));

	// log
	Level().HUD()->outMessage	(0xffffffff,"SERVER","Spawning '%s'(%d,%d,%d) as #%d, on '%s'", E->s_name_replace, E->g_team(), E->g_squad(), E->g_group(), E->ID, CL?CL->Name:"*SERVER*");
}

/*
void spawn_WithPhantom
void spawn_FromPhantom
void spawn_Simple
*/
