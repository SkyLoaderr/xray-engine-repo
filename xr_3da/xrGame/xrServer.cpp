// xrServer.cpp: implementation of the xrServer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xrServer.h"
#include "xrMessages.h"
#include "xrServer_Objects_ALife_All.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

xrServer::xrServer():IPureServer(Device.GetTimerGlobal())
{
	SV_Client = NULL;
}

xrServer::~xrServer()
{

}

//--------------------------------------------------------------------
xrClientData*	xrServer::ID_to_client		(DPNID ID)
{
	if (0==ID)			return 0;
	csPlayers.Enter		();
	for (u32 client=0; client<net_Players.size(); ++client)
	{
		if (net_Players[client]->ID==ID)	{
			csPlayers.Leave		();
			return (xrClientData*)net_Players[client];
		}
	}
	csPlayers.Leave		();
	return 0;
}

CSE_Abstract*	xrServer::ID_to_entity		(u16 ID)
{
#pragma todo("??? to all : ID_to_entity - must be replaced to 'game->entity_from_eid()'")	
	if (0xffff==ID)				return 0;
	xrS_entities::iterator	I	= entities.find	(ID);
	if (entities.end()!=I)		return I->second;
	else						return 0;
}

//--------------------------------------------------------------------
IClient*	xrServer::client_Create		()
{
	return xr_new<xrClientData> ();
}
void		xrServer::client_Replicate	()
{
}
void		xrServer::client_Destroy	(IClient* C)
{
	// Delete assosiated entity
	// xrClientData*	D = (xrClientData*)C;
	// CSE_Abstract* E = D->owner;

	xr_delete			(C);
}

//--------------------------------------------------------------------
void xrServer::Update	()
{
	NET_Packet		Packet;
	u32				position;
	csPlayers.Enter	();

	// game update
	game->Update	();

	// spawn queue
	u32 svT				= Device.TimerAsync();
	while (!(q_respawn.empty() || (svT<q_respawn.begin()->timestamp)))
	{
		// get
		svs_respawn	R		= *q_respawn.begin();
		q_respawn.erase		(q_respawn.begin());

		// 
		CSE_Abstract* E	= ID_to_entity(R.phantom);
		E->Spawn_Write		(Packet,FALSE);
		u16					ID;
		Packet.r_begin		(ID);	R_ASSERT(M_SPAWN==ID);
		Process_spawn		(Packet,0xffff);
	}

	// 
	for (u32 client=0; client<net_Players.size(); ++client)
	{
		// Initialize process and check for available bandwidth
		xrClientData*	Client		= (xrClientData*) net_Players	[client];
		if (!Client->net_Ready)		continue;		
		if (!HasBandwidth(Client))	continue;

		// Send relevant entities to client
		// CSE_Abstract*	Base	= Client->owner;

		Packet.w_begin	(M_UPDATE);

		// GameUpdate
		++(Client->game_replicate_id);
		u32		g_it				= (Client->game_replicate_id % client_Count());
		u32		g_id				= net_Players[g_it]->ID;
		game->net_Export_Update		(Packet,Client->ID,g_id);
		if (!Client->flags.bLocal)	game->net_Export_GameTime(Packet);

		if (!Client->flags.bLocal || client_Count() == 1)
		{
			// Entities
			xrS_entities::iterator	I=entities.begin(),E=entities.end();
			for (; I!=E; ++I)
			{
				CSE_Abstract&	Test = *(I->second);

				if (0==Test.owner)							continue;	// Phantom(?)
				if (!Test.net_Ready)						continue;
				if (Test.owner == Client && Client->flags.bLocal)					continue;	// Can't be relevant

				if (Test.s_flags.is(M_SPAWN_OBJECT_PHANTOM))	continue;	// Surely: phantom

				// write specific data
				{
					Packet.w_u16			(Test.ID		);
					Packet.w_chunk_open8	(position		);
					Test.UPDATE_Write		(Packet			);
					Packet.w_chunk_close8	(position		);
				}
			}
		};

		if (Packet.B.count > 2)	
		{
			SendTo			(Client->ID,Packet,net_flags(FALSE,TRUE));
		}
	}

	if (game->sv_force_sync)	Perform_game_export();

	csPlayers.Leave	();
}

u32 xrServer::OnMessage(NET_Packet& P, DPNID sender)			// Non-Zero means broadcasting with "flags" as returned
{
	u16			type;
	P.r_begin	(type);

	csPlayers.Enter			();
	xrClientData* CL		= ID_to_client(sender);

	switch (type)
	{
	case M_UPDATE:	Process_update		(P,sender);	break;		// No broadcast
	case M_SPAWN:	
		{
//			R_ASSERT(CL->flags.bLocal);
			if (CL->flags.bLocal)
				Process_spawn		(P,sender);	
		}break;
	case M_EVENT:	Process_event		(P,sender); break;	
	case M_CL_INPUT:
		{
			xrClientData* CL		= ID_to_client(sender);
			if (CL)	CL->net_Ready	= TRUE;
			if (SV_Client) SendTo(SV_Client->ID, P, net_flags(TRUE, TRUE));
		}break;
	case M_CLIENTREADY:
		{
			xrClientData* CL		= ID_to_client(sender);
			if (CL)	CL->net_Ready	= TRUE;
			game->signal_Syncronize();
///			SendConnectionData(CL);
//			game->OnPlayerReady		(CL->ID);			
		}break;
	case M_CHANGE_LEVEL:
		{
			if (game->change_level(P,sender))
				SendBroadcast		(0xffffffff,P,net_flags(TRUE,TRUE));
		}break;
	}
	csPlayers.Leave				();

	return 0;
}

//--------------------------------------------------------------------
CSE_Abstract*	xrServer::entity_Create		(LPCSTR name)
{
	return F_entity_Create(name);
}

void			xrServer::entity_Destroy	(CSE_Abstract *&P)
{
	R_ASSERT					(P);
	entities.erase				(P->ID);
	m_tID_Generator.vfFreeID	(P->ID,Device.TimerAsync());
	if (!P->m_bALifeControl)	
		F_entity_Destroy		(P);
}

//--------------------------------------------------------------------
void			xrServer::Server_Client_Check	( IClient* CL )
{
	clients_Lock();
	
	if (SV_Client && SV_Client->ID != CL->ID)
	{
		clients_Unlock();
		return;
	};

	if (SV_Client && SV_Client->ID == CL->ID)
	{
		if (!CL->flags.bConnected)
		{
			SV_Client = NULL;
		};
		clients_Unlock();
		return;
	};

	if (!CL->flags.bConnected) 
	{
		clients_Unlock();
		return;
	};

	IDirectPlay8Address* pAddr = NULL;
	CHK_DX(NET->GetClientAddress(CL->ID, &pAddr, 0));

	if (pAddr) {

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
		if (ClientIP && ClientIP[0]) {

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
				if (!stricmp(ServerIP, ClientIP))
				{
					CL->flags.bLocal = 1;
					SV_Client = (xrClientData*)(CL);
					break;
				}
				else
				{
					CL->flags.bLocal = 0;
				};
			};	
		};
	};

	clients_Unlock();
};