// xrServer.cpp: implementation of the xrServer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xrServer.h"
#include "xrMessages.h"

#include "hudmanager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

xrServer::xrServer()
{
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
	xrClientData*	D = (xrClientData*)C;
	xrServerEntity* E = D->owner;

	_DELETE			(C);
}

//--------------------------------------------------------------------
void xrServer::Update	()
{
	csPlayers.Enter		();
	for (DWORD client=0; client<net_Players.size(); client++)
	{
		// Initialize process and check for available bandwidth
		NET_Packet		Packet;
		xrClientData*	Client	= (xrClientData*) net_Players	[client];
		if (!Client->net_Ready)		continue;
		if (!HasBandwidth(Client))	continue;

		// Send relevant entities to client
		xrServerEntity*	Base	= Client->owner;

		Packet.w_begin	(M_UPDATE);
		xrS_entities::iterator	I=entities.begin(),E=entities.end();
		for (; I!=E; I++)
		{
			xrServerEntity&	Test = *(I->second);

			if (!Test.net_Ready)		continue;
			if (Test.owner == Client)	continue;	// Can't be relevant

//			if (Test.RelevantTo(Base))	{
				Packet.w_u16		(Test.ID);
				Test.UPDATE_Write	(Packet	);
//			}
		}
		if (Packet.B.count > 2)	
		{
			SendTo			(Client->ID,Packet,net_flags(FALSE,TRUE));
		}
	}
	csPlayers.Leave		();
}

BOOL xrServer::ProcessRP	(xrServerEntity* EEE)
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
		}
		break;
	}

	// Perform spawn
	Fvector4&			P = Level().Teams[EEE->g_team()].RespawnPoints[selected];
	EEE->o_Position.set	(P.x,P.y,P.z);
	EEE->o_Angle.set	(0,P.w,0);
	return TRUE;
}

DWORD xrServer::OnMessage(NET_Packet& P, DPNID sender)	// Non-Zero means broadcasting with "flags" as returned
{
	u16 type;
	P.r_begin	(type);

	switch (type)
	{
	case M_UPDATE:
		{
			xrClientData* CL		= ID_to_client(sender);
			CL->net_Ready			= TRUE;

			// while has information
			while (!P.r_eof())
			{
				// find entity
				u16		ID;
				P.r_u16	(ID);
				xrServerEntity* E	= ID_to_entity(ID);
				if (E)				{
					E->net_Ready	= TRUE;
					E->UPDATE_Read	(P);
				}
			}
		}
		return 0;		// No broadcast

	case M_FIRE_BEGIN:
	case M_FIRE_END:
	case M_FIRE_HIT:	// Simply broadcast this message to everyone except sender
		return net_flags(TRUE);
	case M_SPAWN:
		{
			// read spawn information
			string64	s_name;
			P.r_string	(s_name);
			
			// generate/find new ID for entity
			u16 ID		= 0xffff;
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
			
			// create server entity
			xrClientData* CL	= ID_to_client	(sender);
			xrServerEntity*	E	= entity_Create	(s_name);
			R_ASSERT			(E);
			E->Spawn_Read		(P);
			
			// ID, owner, etc
			E->ID				= ID;
			E->owner			= CL;

			// PROCESS NAME; Name this entity
			LPCSTR				NameReplace = 0;
			if (0 == CL->owner)	{
				CL->owner		= E;
				strcpy			(E->s_name_replace,CL->Name);
			}
			
			// PROCESS RP;	 3D position/orientation
			ProcessRP			(E);
			
			// REGISTER new ENTITY
			entities.insert		(make_pair(ID,E));
			ids_used[ID]		= true;	
			
			// log
			Level().HUD()->outMessage	(0xffffffff,"SERVER","Spawning '%s'(%d,%d,%d) as #%d, on '%s'", E->s_name_replace, E->g_team(), E->g_squad(), E->g_group(), E->ID, CL->Name);

			// create packet and broadcast packet to everybody
			NET_Packet			Packet;

			E->Spawn_Write		(Packet,TRUE	);
			SendTo				(sender,Packet,net_flags(TRUE));

			E->Spawn_Write		(Packet,FALSE	);
			SendBroadcast		(sender,Packet,net_flags(TRUE));
		}
		return 0;
	}

	return 0;
}

void xrServer::OnCL_Connected		(IClient* CL)
{
	Level().HUD()->outMessage(0xffffffff,"SERVER","Player '%s' connected",CL->Name);
	
	// Replicate current entities on to this client
	NET_Packet		P;
	xrS_entities::iterator	I=entities.begin(),E=entities.end();
	for (; I!=E; I++)
	{
		xrServerEntity*	Test = I->second;

		Test->Spawn_Write	(P,FALSE);
		SendTo				(CL->ID,P,net_flags(TRUE));
	}

	// Send "finished" signal
	P.w_begin		(M_SV_CONFIG_FINISHED);
	SendTo			(CL->ID,P,net_flags(TRUE));
}

void xrServer::OnCL_Disconnected	(IClient* CL)
{
	Level().HUD()->outMessage(0xffffffff,"SERVER","Player '%s' disconnected",CL->Name);

	// Collect entities
	svector<u16,256>	IDs;
	xrS_entities::iterator	I=entities.begin(),E=entities.end();
	for (; I!=E; I++)
	{
		xrServerEntity*	E = I->second;
		if (E->owner == CL)	{
			IDs.push_back	(E->ID);
			entity_Destroy	(E);
		}
	}

	// Destroy from the map and create message
	NET_Packet			P;
	P.w_begin			(M_DESTROY);
	P.w_u16				(IDs.size());
	for (int i=0; i<IDs.size(); i++)
	{
		u16		ID			= IDs[i];
		P.w_u16				(ID);
		entities.erase		(ID);
	}

	// Send this to all other clients
	SendBroadcast		(CL->ID,P,net_flags(TRUE));
}
