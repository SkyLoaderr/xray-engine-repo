// xrServer.cpp: implementation of the xrServer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xrServer.h"
#include "xrMessages.h"
#include "xrServer_Objects_ALife_All.h"
#include "level.h"
#include "game_cl_base.h"
#include "ai_space.h"

#include "script_space.h"
#include "../XR_IOConsole.h"
//#include "script_engine.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
xrClientData::xrClientData	():IClient(Device.GetTimerGlobal())
{
	ps			= Level().Server->game->createPlayerState();
	ps->clear();

	owner		= NULL;
	net_Ready	= FALSE;
	net_Accepted = FALSE;
}
xrClientData::~xrClientData()
{
	xr_delete(ps);
}


xrServer::xrServer():IPureServer(Device.GetTimerGlobal())
{
}

xrServer::~xrServer()
{
}

//--------------------------------------------------------------------
xrClientData*	xrServer::ID_to_client		(ClientID ID)
{
	if (0==ID.value())			return 0;
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
	// #pragma todo("??? to all : ID_to_entity - must be replaced to 'game->entity_from_eid()'")	
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

	VERIFY						(verify_entities());

	// game update
	game->ProcessDelayedEvent();
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
		ClientID clientID; clientID.set(0xffff);
		Process_spawn		(Packet,clientID);
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
//		u32		g_id				= net_Players[g_it]->ID;
		ClientID g_id 				= net_Players[g_it]->ID;
		game->net_Export_Update		(Packet,Client->ID,g_id);

//		if (!Client->flags.bLocal)	
			game->net_Export_GameTime(Packet);

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

	VERIFY						(verify_entities());

	csPlayers.Leave	();
}

u32 xrServer::OnMessage(NET_Packet& P, ClientID sender)			// Non-Zero means broadcasting with "flags" as returned
{
	u16			type;
	P.r_begin	(type);

	csPlayers.Enter			();

	VERIFY						(verify_entities());

	xrClientData* CL		= ID_to_client(sender);

	switch (type)
	{
	case M_UPDATE:	
		{
			Process_update			(P,sender);						// No broadcast
			VERIFY					(verify_entities());
		}break;
	case M_SPAWN:	
		{
//			R_ASSERT(CL->flags.bLocal);
			if (CL->flags.bLocal)
				Process_spawn		(P,sender);	

			VERIFY					(verify_entities());
		}break;
	case M_EVENT:	
		{
			Process_event			(P,sender);
			VERIFY					(verify_entities());
		}break;
	case M_EVENT_PACK:
		{
			NET_Packet	tmpP;
			while (!P.r_eof())
			{
				tmpP.B.count = P.r_u8();
				P.r(&tmpP.B.data, tmpP.B.count);

				OnMessage (tmpP, sender);
			};			
		}break;
	case M_CL_UPDATE:
		{
			xrClientData* CL		= ID_to_client	(sender);
			if (CL)	CL->net_Ready	= TRUE;
			//-------------------------------------------------------------------
			u32 ClientPing = CL->stats.getPing();
			P.w_seek(P.r_tell()+2, &ClientPing, 4);
			//-------------------------------------------------------------------
			if (SV_Client) SendTo	(SV_Client->ID, P, net_flags(TRUE, TRUE));
			VERIFY					(verify_entities());
		}break;
	case M_CL_INPUT:
		{
			xrClientData* CL		= ID_to_client	(sender);
			if (CL)	CL->net_Ready	= TRUE;
			if (SV_Client) SendTo	(SV_Client->ID, P, net_flags(TRUE, TRUE));
			VERIFY					(verify_entities());
		}break;
	case M_GAMEMESSAGE:
		{
			ClientID clientID;clientID.setBroadcast();
			SendBroadcast			(clientID,P,net_flags(TRUE,TRUE));
			VERIFY					(verify_entities());
		}break;
	case M_CLIENTREADY:
		{
			xrClientData* CL		= ID_to_client(sender);
			if (CL)	CL->net_Ready	= TRUE;
			game->signal_Syncronize	();
			VERIFY					(verify_entities());
		}break;
	case M_SWITCH_DISTANCE:
		{
			game->switch_distance	(P,sender);
			VERIFY					(verify_entities());
		}break;
	case M_CHANGE_LEVEL:
		{
			if (game->change_level(P,sender)){
				ClientID clientID;clientID.setBroadcast();
				SendBroadcast		(clientID,P,net_flags(TRUE,TRUE));
				}
			VERIFY					(verify_entities());
		}break;
	case M_SAVE_GAME:
		{
			game->save_game			(P,sender);
			VERIFY					(verify_entities());
		}break;
	case M_LOAD_GAME:
		{
			game->load_game			(P,sender);
			ClientID clientID;clientID.setBroadcast();
			SendBroadcast			(clientID,P,net_flags(TRUE,TRUE));
			VERIFY					(verify_entities());
		}break;
	case M_RELOAD_GAME:
		{
			ClientID clientID;clientID.setBroadcast();
			SendBroadcast			(clientID,P,net_flags(TRUE,TRUE));
			VERIFY					(verify_entities());
		}break;
	case M_SAVE_PACKET:
		{
			Process_save			(P,sender);
			VERIFY					(verify_entities());
		}break;
	case M_CLIENT_REQUEST_CONNECTION_DATA:
		{
			xrClientData* CL		= ID_to_client	(sender);
			OnCL_Connected(CL);
		}break;
	case M_CHAT_MESSAGE:
		{
			xrClientData *l_pC	= ID_to_client(sender);
			OnChatMessage(&P, l_pC);
		}break;
	case M_CHANGE_LEVEL_GAME:
		{
			ClientID CID; CID.set(0xffffffff);
			SendBroadcast		(CID,P,net_flags(TRUE,TRUE));
		}break;
	}

	VERIFY							(verify_entities());

	csPlayers.Leave					();

	return							(0);
}

void xrServer::SendTo_LL			(ClientID ID, void* data, u32 size, u32 dwFlags, u32 dwTimeout)
{
	if (SV_Client && SV_Client->ID==ID)
	{
		// optimize local traffic
		Level().OnMessage		(data,size);
	} else {
		IPureServer::SendTo_LL	(ID,data,size,dwFlags,dwTimeout);
	}
}

//--------------------------------------------------------------------
CSE_Abstract*	xrServer::entity_Create		(LPCSTR name)
{
	return F_entity_Create(name);
}

void			xrServer::entity_Destroy	(CSE_Abstract *&P)
{
	R_ASSERT					(P);
	P->owner = NULL;
	entities.erase				(P->ID);
	m_tID_Generator.vfFreeID	(P->ID,Device.TimerAsync());
	if (!ai().get_alife() || !P->m_bALifeControl)	
		F_entity_Destroy		(P);
}

//--------------------------------------------------------------------
void			xrServer::Server_Client_Check	( IClient* CL )
{
	clients_Lock	();
	/*
	if (SV_Client && SV_Client->ID != CL->ID)
	{
		clients_Unlock	();
		return;
	};
*/
	if (SV_Client && SV_Client->ID == CL->ID)
	{
		if (!CL->flags.bConnected)
		{
			SV_Client = NULL;
		};
		clients_Unlock	();
		return;
	};

	if (!CL->flags.bConnected) 
	{
		clients_Unlock();
		return;
	};

	IDirectPlay8Address* pAddr = NULL;
	CHK_DX(NET->GetClientAddress(CL->ID.value(), &pAddr, 0));

	if (pAddr) 
	{
		IBannedClient xClient;
		bool res = GetClientAddress(pAddr, xClient.HAddr);
		if (res)
		{
			char HostIP[4] = {0, 0, 0, 0};
			DWORD	NumAdresses = 0;
			NET->GetLocalHostAddresses(NULL, &NumAdresses, 0);

			IDirectPlay8Address* p_pAddr[256];
			Memory.mem_fill(p_pAddr, 0, sizeof(p_pAddr));

			NumAdresses = 256;
			R_CHK(NET->GetLocalHostAddresses(p_pAddr, &NumAdresses, 0));

			for (DWORD i=0; i<NumAdresses; i++)
			{
				if (!p_pAddr[i]) continue;

				if (!GetClientAddress(p_pAddr[i], HostIP)) continue;
				if (xClient == HostIP)
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

bool		xrServer::OnCL_QueryHost		() 
{
	if (game->Type() == GAME_SINGLE) return false;
	return (client_Count() != 0); 
};

CSE_Abstract*	xrServer::GetEntity			(u32 Num)
{
	xrS_entities::iterator	I=entities.begin(),E=entities.end();
	for (u32 C=0; I!=E; ++I,++C)
	{
		if (C == Num) return I->second;
	};
	return NULL;
};


void		xrServer::OnChatMessage(NET_Packet* P, xrClientData* CL)
{
//	string256 ChatMsg;
//	u16 PlayerID = P->r_u16();
	s16 team = P->r_s16();
//	P->r_stringZ(ChatMsg);
	if (!CL->net_Ready) return;
	game_PlayerState* Cps = CL->ps;
	for (u32 client=0; client<net_Players.size(); ++client)
	{
		// Initialize process and check for available bandwidth
		xrClientData*	Client		= (xrClientData*) net_Players	[client];
		game_PlayerState* ps = Client->ps;
		if (!Client->net_Ready) continue;
		if (team != 0 && ps->team != team) continue;
		if (Cps->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD) && !ps->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD))
			continue;
		SendTo(Client->ID, *P);
	};
};

void		xrServer::BannedList_Load		()
{
	Console->ExecuteScript(GetBannedListName());
}

#ifdef DEBUG

bool xrServer::verify_entities				() const
{
	xrS_entities::const_iterator		I = entities.begin();
	xrS_entities::const_iterator		E = entities.end();
	for ( ; I != E; ++I) {
		VERIFY2							((*I).first != 0xffff,"SERVER : Invalid entity id as a map key - 0xffff");
		VERIFY2							((*I).second,"SERVER : Null entity object in the map");
		VERIFY3							((*I).first == (*I).second->ID,"SERVER : ID mismatch - map key doesn't correspond to the real entity ID",(*I).second->name_replace());
		verify_entity					((*I).second);
	}
	return								(true);
}

void xrServer::verify_entity				(const CSE_Abstract *entity) const
{
	if (entity->ID_Parent != 0xffff) {
		xrS_entities::const_iterator	J = entities.find(entity->ID_Parent);
		VERIFY3							(J != entities.end(),"SERVER : Cannot find parent in the map",entity->name_replace());
		VERIFY3							((*J).second,"SERVER : Null entity object in the map",entity->name_replace());
		VERIFY3							((*J).first == (*J).second->ID,"SERVER : ID mismatch - map key doesn't correspond to the real entity ID",(*J).second->name_replace());
		VERIFY3							(std::find((*J).second->children.begin(),(*J).second->children.end(),entity->ID) != (*J).second->children.end(),"SERVER : Parent/Children relationship mismatch - Object has parent, but corresponding parent doesn't have children",(*J).second->name_replace());
	}

	xr_vector<u16>::const_iterator		I = entity->children.begin();
	xr_vector<u16>::const_iterator		E = entity->children.end();
	for ( ; I != E; ++I) {
		VERIFY3							(*I != 0xffff,"SERVER : Invalid entity children id - 0xffff",entity->name_replace());
		xrS_entities::const_iterator	J = entities.find(*I);
		VERIFY3							(J != entities.end(),"SERVER : Cannot find children in the map",entity->name_replace());
		VERIFY3							((*J).second,"SERVER : Null entity object in the map",entity->name_replace());
		VERIFY3							((*J).first == (*J).second->ID,"SERVER : ID mismatch - map key doesn't correspond to the real entity ID",(*J).second->name_replace());
		VERIFY3							((*J).second->ID_Parent == entity->ID,"SERVER : Parent/Children relationship mismatch - Object has children, but children doesn't have parent",(*J).second->name_replace());
	}
}

#endif // DEBUG