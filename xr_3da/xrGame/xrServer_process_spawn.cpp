#include "stdafx.h"
#include "xrServer.h"

#include "hudmanager.h"

void xrServer::Process_spawn(NET_Packet& P, DPNID sender)
{
	// read spawn information
	string64	s_name;
	P.r_string	(s_name);

	// create server entity
	xrClientData* CL	= ID_to_client	(sender);
	xrServerEntity*	E	= entity_Create	(s_name);
	R_ASSERT			(E);
	E->Spawn_Read		(P);

#pragma todo("SE, Spawn: remove auto-activate")
	E->s_flags			|= M_SPAWN_OBJECT_ACTIVE;

	// generate/find new ID for entity
	u16 ID		=		E->ID;
	if (0xffff==ID)		
	{
		// Find
		if (ids_used.size())	
		{
			for (vector<bool>::iterator I=ids_used.begin(); I!=ids_used.end(); I++)
			{
				if (!(*I))	{ ID = I-ids_used.begin(); break; }
			}
			if (0xffff==ID)	{
				ID			= ids_used.size	();
				ids_used.push_back			(false);
			}
		} else {
			ID		= 0;
			ids_used.push_back	(false);
		}
	} else {
		// Try to use supplied ID
		if (ID<ids_used.size())
		{
			R_ASSERT		(false==ids_used[ID]);
			ids_used[ID]	= true;
		} else {
			ids_used.resize	(ID+1);
			ids_used[ID]	= true;
		}
	}

	// ID, owner, etc
	E->ID				= ID;
	E->owner			= CL;

	// PROCESS NAME; Name this entity
	LPCSTR				NameReplace = 0;
	if (CL && (0 == CL->owner))	{
		CL->owner		= E;
		strcpy			(E->s_name_replace,CL->Name);
	}

	// PROCESS RP;	 3D position/orientation
	PerformRP			(E);

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

	E->Spawn_Write		(Packet,FALSE	);
	SendBroadcast		(sender,Packet,net_flags(TRUE));
}
