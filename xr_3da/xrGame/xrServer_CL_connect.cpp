#include "stdafx.h"
#include "hudmanager.h"

void xrServer::Perform_connect_spawn(CSE_Abstract* E, xrClientData* CL, NET_Packet& P)
{
	if (E->net_Processed)						return;
	if (E->s_flags.is(M_SPAWN_OBJECT_PHANTOM))	return;

	// Connectivity order
	CSE_Abstract* Parent = ID_to_entity	(E->ID_Parent);
	if (Parent)		Perform_connect_spawn	(Parent,CL,P);

	// Process
	if (0==E->owner)	
	{
		// PROCESS NAME; Name this entity
		if (E->s_flags.is(M_SPAWN_OBJECT_ASPLAYER))
		{
			CL->owner		= E;
			strcpy			(E->s_name_replace,CL->Name);
		}

		// Associate
		E->owner		= CL;
		E->Spawn_Write	(P,TRUE	);
	}
	else				
	{
		// Just inform
		E->Spawn_Write	(P,FALSE);
	}
	SendTo				(CL->ID,P,net_flags(TRUE,TRUE));
	E->net_Processed	= TRUE;
}

void xrServer::OnCL_Connected		(IClient* _CL)
{
	csPlayers.Enter					();
	xrClientData*	CL				= (xrClientData*)_CL;

	//. HUD().outMessage			(0xffffffff,"SERVER","Player '%s' connected",CL->Name);
	NET_Packet		P;
	u32			mode				= net_flags(TRUE,TRUE);

	// Game config (all, info includes _new_ player)
	Perform_game_export				();

	// Replicate current entities on to this client
	xrS_entities::iterator	I=entities.begin(),E=entities.end();
	for (; I!=E; ++I)						I->second->net_Processed	= FALSE;
	for (I=entities.begin(); I!=E; ++I)		Perform_connect_spawn		(I->second,CL,P);

	// Send "finished" signal
	P.w_begin						(M_SV_CONFIG_FINISHED);
	SendTo							(CL->ID,P,mode);

	// 
	game->OnPlayerConnect			(CL->ID);
	csPlayers.Leave					();

	Find_Server_Client();

	/*
	IDirectPlay8Address* pAddr = NULL;
	CHK_DX(NET->GetClientAddress(_CL->ID, &pAddr, 0));

	if (pAddr)
	{
		string256	aaaa;
		DWORD		aaaa_s			= sizeof(aaaa);
		R_CHK		(pAddr->GetURLA(aaaa,&aaaa_s));
		aaaa_s		= xr_strlen(aaaa);

		LPSTR ClientIP = NULL;
		if (strstr(aaaa, "hostname="))
		{
			ClientIP = strstr(aaaa, "hostname=")+ xr_strlen("hostname=");
			if (strstr(ClientIP, ";")) strstr(ClientIP, ";")[0] = 0;
		};
		if (!ClientIP || !ClientIP[0]) return;

		DWORD	NumAdresses = 0;
		NET->GetLocalHostAddresses(NULL, &NumAdresses, 0);
		
		IDirectPlay8Address* p_pAddr[256];
		Memory.mem_fill(p_pAddr, 0, sizeof(p_pAddr));

		NumAdresses = 256;
		R_CHK(NET->GetLocalHostAddresses(p_pAddr, &NumAdresses, 0));

		for (DWORD i=0; i<NumAdresses; i++)
		{
			if (!p_pAddr[i]) continue;
			
			string256	bbbb;
			DWORD		bbbb_s			= sizeof(bbbb);
			R_CHK		(p_pAddr[i]->GetURLA(bbbb,&bbbb_s));
			bbbb_s		= xr_strlen(bbbb);

			LPSTR ServerIP = NULL;
			if (strstr(aaaa, "hostname="))
			{
				ServerIP = strstr(bbbb, "hostname=")+ xr_strlen("hostname=");
				if (strstr(ServerIP, ";")) strstr(ServerIP, ";")[0] = 0;
			};
			if (!ServerIP || !ServerIP[0]) continue;
			if (!strcmp(ServerIP, ClientIP))
			{
				CL->flags.bLocal = 1;
				break;
			};
		};
	};
	*/
}
