#include "stdafx.h"
#include "Level.h"
#include "Level_Bullet_Manager.h"
#include "xrserver.h"
#include "xrmessages.h"
#include "game_cl_base.h"

void CLevel::net_Stop		()
{
	Msg							("- Disconnect");
	IGame_Level::net_Stop		();
	IPureClient::Disconnect		();

	BulletManager().Clear		();

	
	if (Server)
	{
		Server->Disconnect		();
		xr_delete				(Server);
	}
}

void CLevel::ClientSend	()
{
	if (GameID() == GAME_SINGLE || OnClient())
	{
		if (!net_HasBandwidth()) return;
	};

	NET_Packet				P;
	u32						start	= 0;
	//----------- for E3 -----------------------------
	if (OnClient()) 
	{
		if (!CurrentEntity()) return;
		CObject* pObj = CurrentControlEntity();
		if (pObj->getDestroy() || !pObj->net_Relevant()) return;

		P.w_begin		(M_CL_UPDATE);
		
		P.w_u16			(u16(pObj->ID())	);
		P.w_u32			(0);	//reserved place for client's ping

		pObj->net_Export			(P);
		
		if (P.B.count>9)				Send	(P, net_flags(FALSE));
		return;
	};
	//-------------------------------------------------

	while (1)				{
		P.w_begin						(M_UPDATE);
		start	= Objects.net_Export	(&P, start, 48);
		if (P.B.count>2)				Send	(P, net_flags(FALSE));
		else							break	;
	}
}

u32	CLevel::Objects_net_Save	(NET_Packet* _Packet, u32 start, u32 count)
{
	NET_Packet& Packet	= *_Packet;
	u32			position;
	for (; start<Objects.objects.size(); start++)	{
		CObject		*_P = Objects.objects[start];
		CGameObject *P = dynamic_cast<CGameObject*>(_P);
		if (P && !P->getDestroy() && P->net_SaveRelevant())	{
			Packet.w_u16			(u16(P->ID())	);
			Packet.w_chunk_open8	(position);
			P->net_Save				(Packet);
#ifdef DEBUG
			u32 size				= u32		(Packet.w_tell()-position)-sizeof(u8);
			if				(size>=256)			{
				Debug.fatal	("Object [%s][%d] exceed network-data limit\n size=%d, Pend=%d, Pstart=%d",
					*P->cName(), P->ID(), size, Packet.w_tell(), position);
			}
#endif
			Packet.w_chunk_close8	(position);
			if (0==(--count))		break;
		}
	}
	return	start;
}

void CLevel::ClientSave	()
{
	NET_Packet		P;
	u32				start	= 0;

	for (;;) {
		P.w_begin	(M_SAVE_PACKET);
		
//		start		= Objects.net_Save(&P, start, 48);
		start		= Objects_net_Save(&P, start, 48);

		if (P.B.count>2)
			Send	(P, net_flags(FALSE));
		else
			break;
	}
}

void CLevel::Send		(NET_Packet& P, u32 dwFlags, u32 dwTimeout)
{
	// optimize the case when server located in our memory
	if (Server && game_configured && OnServer()){
		
		Server->OnMessage	(P,Game().local_svdpnid	);
	}else											IPureClient::Send	(P,dwFlags,dwTimeout	);
}

void CLevel::net_Update	()
{
	// If we have enought bandwidth - replicate client data on to server
	Device.Statistic.netClient.Begin	();
	ClientSend							();
	Device.Statistic.netClient.End		();

	// If server - perform server-update
	if (Server && OnServer())	{
		Device.Statistic.netServer.Begin();
		Server->Update					();
		Device.Statistic.netServer.End	();
	}
}

struct _NetworkProcessor	: public pureFrame
{
	virtual void OnFrame	( )
	{
		if (g_pGameLevel)	g_pGameLevel->net_Update();
	}
}	NET_processor;

pureFrame*	g_pNetProcessor	= &NET_processor;
