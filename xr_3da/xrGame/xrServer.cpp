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
		Process_spawn		(Packet,0);
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

BOOL xrServer::PerformRP	(xrServerEntity* EEE)
{
	// Get list of respawn points
	if (EEE->g_team() >= int(Level().Teams.size()))	return FALSE;
	svector<Fvector4,maxRP>&	RP					= Level().Teams[EEE->g_team()].RespawnPoints;
	if (RP.empty())									return FALSE;
	
	DWORD	selected	= 0;
	switch (EEE->s_RP)	{
	case 0xFE:	// Use supplied coords
		return TRUE;
	default:	// Use specified RP
		if (EEE->s_RP>=RP.size())	Msg("! ERROR: Can't spawn entity at RespawnPoint #%d.", DWORD(EEE->s_RP));
		selected = DWORD(EEE->s_RP);
		break;
	case 0xFF:	// Search for best RP for this entity
		{
			selected	= ::Random.randI	(0,RP.size());

			/*
			float	best		= -1;
			for (DWORD id=0; id<RP.size(); id++)
			{
				Fvector4&	P = RP[id]; 
				Fvector		POS;
				POS.set		(P.x,P.y,P.z);
				float		cost	=0;
				DWORD		count	=0;
				
				for (xrS_entities::iterator o_it=entities.begin(); o_it!=entities.end(); o_it++) 
				{
					// Get entity & it's position
					xrServerEntity*	E	= o_it->second;
					if	(E->s_flags & M_SPAWN_OBJECT_PHANTOM)			continue;

					float	dist		= POS.distance_to(E->o_Position);
					float	e_cost		= 0;
					
					if (EEE->g_team() == E->g_team())	{
						// same teams, try to spawn near them, but not so near
						if (dist<5)		e_cost += 3*(5-dist);
					} else {
						// different teams, try to avoid them
						if (dist<30)	e_cost += 3*(30-dist);
					}
					
					cost	+= dist;
					count	+= 1;
				}
				
				if (0==count)	{ selected = id; break; }
				cost /= float(count);
				if (cost>best)	{ selected = id; best = cost; }
			}
			*/
		}
		break;
	}

	// Perform spawn
	Fvector4&			P = Level().Teams[EEE->g_team()].RespawnPoints[selected];
	EEE->o_Position.set	(P.x,P.y,P.z);
	EEE->o_Angle.set	(0,P.w,0);
	return TRUE;
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

