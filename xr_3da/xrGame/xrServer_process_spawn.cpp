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
		xrServerEntity* Phantom	=	entity_Create	(s_name); R_ASSERT(Phantom);
		Phantom->Spawn_Read		(P);
		Phantom->ID				=	PerformIDgen	(0xffff);
		Phantom->ID_Phantom		=	Phantom->ID;	// Self-linked to avoid phantom-breeding
		E->ID_Phantom			=	Phantom->ID;
		Phantom->s_flags		|=	M_SPAWN_OBJECT_PHANTOM;
		ids_used[Phantom->ID]	=	true;	
		entities.insert			(make_pair(Phantom->ID,Phantom));
	}
 
	// generate/find new ID for entity
	u16 ID				=	PerformIDgen(E->ID);
	E->ID				=	ID;
	E->owner			=	CL;

	// PROCESS NAME; Name this entity
	if (CL && (0 == CL->owner))	{
		CL->owner		= E;
		strcpy			(E->s_name_replace,CL->Name);
	}

	// PROCESS RP;	 3D position/orientation
	PerformRP			(E);
	E->s_RP				= 0xFE;	// Use supplied

	// REGISTER new ENTITY
	ids_used[ID]		= true;	
	entities.insert		(make_pair(ID,E));

	// log
	Level().HUD()->outMessage	(0xffffffff,"SERVER","Spawning '%s'(%d,%d,%d) as #%d, on '%s'", E->s_name_replace, E->g_team(), E->g_squad(), E->g_group(), E->ID, CL?CL->Name:"*SERVER*");

	// create packet and broadcast packet to everybody
	NET_Packet			Packet;

	if (CL) 
	{
		E->Spawn_Write		(Packet,TRUE	);
		SendTo				(sender,Packet,net_flags(TRUE));
	}

	if (GAME!=GAME_SINGLE)	E->s_flags		&=	~M_SPAWN_OBJECT_ASPLAYER;
	E->Spawn_Write			(Packet,FALSE	);
	SendBroadcast			(sender,Packet,net_flags(TRUE));
}
