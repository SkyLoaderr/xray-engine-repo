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
	_DELETE(C);
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
//		Packet.w_u32	(0);		// reserve place for "server_time"
		xrS_entities::iterator	I=entities.begin(),E=entities.end();
		for (; I!=E; I++)
		{
			xrServerEntity&	Test = *(*I);

			if (!Test.net_Ready)		continue;
			if (Test.owner == Client)	continue;	// Can't be relevant

			if (Test.RelevantTo(Base))	{
				Packet.w_u8	(Test.ID);
				Test.Write	(Packet);
			}
		}
		if (Packet.B.count > 2)	
		{
//			DWORD	server_time		= Device.TimerAsync();
//			Packet.w_seek	(0,&server_time,4);
			SendTo			(Client->ID,Packet,net_flags(FALSE,TRUE));
		}
	}
	csPlayers.Leave		();
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

//			if (entities.size()>1)	__asm int 3;
			
			// while has information
			while (!P.r_eof())
			{
				// find entity
				xrServerEntity* E	= ID_to_entity(P.r_u8());
				if (E)				{
					E->net_Ready	= TRUE;
					E->Read			(P);
				}
			}
		}
		return 0;		// No broadcast

	case M_FIRE_BEGIN:
	case M_FIRE_END:
	case M_FIRE_HIT:	// Simply broadcast this message to everyone except sender
		return net_flags(TRUE);
	case M_CL_SPAWN:
		{
			// read spawn information
			char	s_name[128];
			BYTE	s_team,	s_squad, s_group, s_rp;

			P.r_string(s_name);
			s_team	= P.r_u8();
			s_squad	= P.r_u8();
			s_group	= P.r_u8();
			s_rp	= P.r_u8();

			// generate/find new ID for entity
			BYTE ID = 0;
			if (!entities.empty())
			{
				ID = entities.back()->ID+1;
			}
			if (entities.size()>1)	
			{
				for (DWORD e=0; e<entities.size()-1; e++)
				{
					BYTE id_cur		= entities[e+0]->ID;
					BYTE id_next	= entities[e+1]->ID;
					if ((id_next-id_cur)>1)	{
						ID = id_cur+1;
						break;
					}
				}
			}
			
			// create server entity
			xrServerEntity*	E	= entity_Create(s_name);
			R_ASSERT			(E);
			char*				NameReplace="";
			xrClientData* CL	= ID_to_client(sender);
			if (0==CL->owner)	{
				CL->owner	= E;
				NameReplace	= CL->Name;

				// find empty squad (no leader)
				for (s_squad=0; s_squad<64; s_squad++ )
				{
					CSquad& S = Level().get_squad(s_team,s_squad);
					if (0==S.Leader)	break;
				}
			} else {
				// accept squad as is - it's AI or decorative entity
			}
			E->ID				= ID;
			E->owner			= CL;
			E->s_name			= strdup(s_name);
			E->s_name_replace	= strdup(NameReplace);
			E->s_team			= s_team;
			E->s_squad			= s_squad;
			E->s_group			= s_group;
			E->Spawn			(s_rp,entities);
			entities.push_back	(E);

			// log
			Level().HUD()->outMessage(0xffffffff,"SERVER","Spawning '%s'(%d,%d,%d) as #%d, on '%s'",
				E->s_name,s_team,s_squad,s_group,E->ID,CL->Name);

			// create packet and broadcast packet to everybody
			NET_Packet			Packet;

			E->msgSpawn			(Packet,TRUE);
			SendTo				(sender,Packet,net_flags(TRUE));

			E->msgSpawn			(Packet,FALSE);
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
		xrServerEntity*	Test = *I;

		Test->msgSpawn	(P,FALSE);
		SendTo			(CL->ID,P,net_flags(TRUE));
	}

	// Send "finished" signal
	P.w_begin		(M_SV_CONFIG_FINISHED);
	SendTo			(CL->ID,P,net_flags(TRUE));
}

void xrServer::OnCL_Disconnected	(IClient* CL)
{
	Level().HUD()->outMessage(0xffffffff,"SERVER","Player '%s' disconnected",CL->Name);
}
