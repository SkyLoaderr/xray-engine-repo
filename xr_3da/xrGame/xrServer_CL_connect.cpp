#include "stdafx.h"

void xrServer::OnCL_Connected		(IClient* _CL)
{
	xrClientData*	CL				= (xrClientData*)_CL;

	Level().HUD()->outMessage		(0xffffffff,"SERVER","Player '%s' connected",CL->Name);
	NET_Packet		P;

	// Game config
	P.w_begin		(M_SV_CONFIG_GAME);
	P.w_u8			(u8(GAME		));
	P.w_u16			(u16(g_fraglimit));
	P.w_u16			(u16(g_timelimit));
	SendTo			(CL->ID,P,net_flags(TRUE,TRUE));

	// Replicate current entities on to this client
	xrS_entities::iterator	I=entities.begin(),E=entities.end();
	for (; I!=E; I++)
	{
		xrServerEntity*		Test	= I->second;
		if (Test->s_flags & M_SPAWN_OBJECT_PHANTOM)	continue;

		if (0==Test->owner)	
		{
			// PROCESS NAME; Name this entity
			if (Test->s_flags & M_SPAWN_OBJECT_ASPLAYER)
			{
				CL->owner		= Test;
				strcpy			(Test->s_name_replace,CL->Name);
			}

			// Associate
			Test->owner			= CL;
			Test->Spawn_Write	(P,TRUE	);
		}
		else				
		{
			// Just inform
			Test->Spawn_Write	(P,FALSE);
		}
		SendTo				(CL->ID,P,net_flags(TRUE,TRUE));
	}

	// Send "finished" signal
	P.w_begin		(M_SV_CONFIG_FINISHED);
	SendTo			(CL->ID,P,net_flags(TRUE,TRUE));

	// Inform new client about already existed players
	csPlayers.Enter		();
	for (DWORD client=0; client<net_Players.size(); client++)
	{
		xrClientData*	data	= (xrClientData*)net_Players[client];
		if (data->ID==CL->ID)	continue;

		P.w_begin	(M_PLIST_ADD);
		P.w_u32		(data->ID);
		P.w_string	(data->Name);
		P.w_s16		(data->g_frags);
		SendTo		(CL->ID,P,net_flags(TRUE,TRUE));
	}
	csPlayers.Leave		();

	// Signal to everybody about connect
	P.w_begin		(M_PLIST_ADD);
	P.w_u32			(CL->ID);
	P.w_string		(CL->Name);
	P.w_s16			(CL->g_frags)
	SendBroadcast	(0xffffffff,P,net_flags(TRUE,TRUE));
}
