#include "stdafx.h"

void xrServer::OnCL_Connected		(IClient* CL)
{
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
			xrClientData*	C	= (xrClientData*)CL;
			if (Test->s_flags & M_SPAWN_OBJECT_ASPLAYER)
			{
				C->owner		= Test;
				strcpy			(Test->s_name_replace,C->Name);
			}

			// Associate
			Test->owner			= C;
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

	// Signal to everybody about connect
	P.w_begin		(M_PLIST_ADD);
	P.w_u32			(CL->ID);
	P.w_string		(CL->Name);
	SendBroadcast	(0xffffffff,P,net_flags(TRUE,TRUE));
}
