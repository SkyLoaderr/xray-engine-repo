#include "stdafx.h"
#include "xrServer.h"

#include "hudmanager.h"

void xrServer::Process_spawn(NET_Packet& P, DPNID sender)
{
	// read spawn information
	u16					s_respawntime;
	string64			s_name;
	P.r_u16				(s_respawntime);
	P.r_string			(s_name);

	// create server entity
	xrClientData* CL	= ID_to_client	(sender);
	xrServerEntity*	E	= entity_Create	(s_name);
	R_ASSERT			(E);
	E->Spawn_Read		(P);
	if (!((GAME==E->s_gameid)||(GAME_ANY==E->s_gameid)))
	{
		Msg				("- SERVER: Entity [%s] incompatible with current game type.",E->s_name);
		F_entity_Destroy(E);
		return;
	}

	// check for respawn-capability
 
	// generate/find new ID for entity
	u16 ID				=	PerformIDgen(E->ID);
	E->ID				=	ID;
	E->owner			=	CL;

	// PROCESS NAME; Name this entity
	// LPCSTR				NameReplace = 0;
	if (CL && (0 == CL->owner))	{
		CL->owner		= E;
		strcpy			(E->s_name_replace,CL->Name);
	}

	// PROCESS RP;	 3D position/orientation
	PerformRP			(E);
	E->s_RP				= 0xFE;	// Use supplied

	// REGISTER new ENTITY
	entities.insert		(make_pair(ID,E));
	ids_used[ID]		= true;	

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
