// xrServer.cpp: implementation of the xrServer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xrServer.h"
#include "xrMessages.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

xrServer::xrServer()
{
	id_free.resize	(0xffff);
	u16 id			= 0;
	for (deque<u16>::iterator I=id_free.begin(); I!=id_free.end(); *I++ = id++);
}

xrServer::~xrServer()
{

}

//--------------------------------------------------------------------
xrClientData*	xrServer::ID_to_client		(DPNID ID)
{
	if (0==ID)			return 0;
	csPlayers.Enter		();
	for (DWORD client=0; client<net_Players.size(); client++)
	{
		if (net_Players[client]->ID==ID)	{
			csPlayers.Leave		();
			return (xrClientData*)net_Players[client];
		}
	}
	csPlayers.Leave		();
	return 0;
}

xrServerEntity*	xrServer::ID_to_entity		(u16 ID)
{
	if (0xffff==ID)				return 0;
	xrS_entities::iterator	I	= entities.find	(ID);
	if (I!=entities.end())		return I->second;
	else						return 0;
}

//--------------------------------------------------------------------
IClient*	xrServer::client_Create		()
{
	return new xrClientData;
}
void		xrServer::client_Replicate	()
{
}
void		xrServer::client_Destroy	(IClient* C)
{
	// Delete assosiated entity
	// xrClientData*	D = (xrClientData*)C;
	// xrServerEntity* E = D->owner;

	_DELETE			(C);
}

//--------------------------------------------------------------------
void xrServer::Update	()
{
	NET_Packet		Packet;
	u32				position;

	// spawn queue
	DWORD svT				= Device.TimerAsync();
	while (!(q_respawn.empty() || (svT<q_respawn.begin()->timestamp)))
	{
		// get
		svs_respawn	R		= *q_respawn.begin();
		q_respawn.erase		(q_respawn.begin());

		// 
		xrServerEntity* E	= ID_to_entity(R.phantom);
		E->Spawn_Write		(Packet,FALSE);
		u16					ID;
		Packet.r_begin		(ID);	R_ASSERT(M_SPAWN==ID);
		Process_spawn		(Packet,0xffff);
	}

	// 
	csPlayers.Enter		();
	for (DWORD client=0; client<net_Players.size(); client++)
	{
		// Initialize process and check for available bandwidth
		xrClientData*	Client	= (xrClientData*) net_Players	[client];
		if (!Client->net_Ready)		continue;
		if (!HasBandwidth(Client))	continue;

		// Send relevant entities to client
		// xrServerEntity*	Base	= Client->owner;

		Packet.w_begin	(M_UPDATE);

		// GameUpdate
		Client->game_replicate_id	++;
		u32		g_it				= (Client->game_replicate_id % client_Count());
		u32		g_id				= net_Players[g_it]->ID;
		game->net_Export_Update		(Packet,Client->ID,g_id);

		// Entities
		xrS_entities::iterator	I=entities.begin(),E=entities.end();
		for (; I!=E; I++)
		{
			xrServerEntity&	Test = *(I->second);

			if (0==Test.owner)							continue;	// Phantom(?)
			if (!Test.net_Ready)						continue;
			if (Test.owner == Client)					continue;	// Can't be relevant
			if (Test.s_flags&M_SPAWN_OBJECT_PHANTOM)	continue;	// Surely: phantom

			// write specific data
			{
				Packet.w_u16			(Test.ID		);
				Packet.w_chunk_open8	(position		);
				Test.UPDATE_Write		(Packet			);
				Packet.w_chunk_close8	(position		);
			}
		}
		if (Packet.B.count > 2)	
		{
			SendTo			(Client->ID,Packet,net_flags(FALSE,TRUE));
		}
	}
	csPlayers.Leave		();
}

DWORD xrServer::OnMessage(NET_Packet& P, DPNID sender)			// Non-Zero means broadcasting with "flags" as returned
{
	u16			type;
	P.r_begin	(type);

	switch (type)
	{
	case M_UPDATE:				Process_update		(P,sender);	return 0;		// No broadcast
	case M_SPAWN:				Process_spawn		(P,sender);	return 0;
	case M_EVENT:				Process_event		(P,sender); return 0;
	}

	return 0;
}

//--------------------------------------------------------------------
xrServerEntity*	xrServer::entity_Create		(LPCSTR name)
{
	return F_entity_Create(name);
}

void			xrServer::entity_Destroy	(xrServerEntity* P)
{
	R_ASSERT			(P);
	id_free.push_back	(P->ID);
	F_entity_Destroy	(P);
}

