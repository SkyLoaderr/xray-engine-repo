#include "stdafx.h"
#include "Level.h"
#include "Level_Bullet_Manager.h"
#include "xrserver.h"
#include "xrmessages.h"
#include "game_cl_base.h"
#include "PHCommander.h"
#include "net_queue.h"
#include "MainUI.h"

void CLevel::net_Stop		()
{
	Msg							("- Disconnect");
	
	if (OnServer())
		Server->SLS_Clear		();
	
	if (OnClient())
		ClearAllObjects			();

	BOOL b_stored = psDeviceFlags.test(rsDisableObjectsAsCrows);
	for (int i=0; i<6; ++i) {
		psDeviceFlags.set(rsDisableObjectsAsCrows,TRUE);
		ClientReceive			();
		ProcessGameEvents		();
		Objects.Update			();
	}
	
	IGame_Level::net_Stop		();
	IPureClient::Disconnect		();

	BulletManager().Clear		();
	ph_commander().clear		();
	ph_commander_scripts().clear();
	if (Server) {
		Server->Disconnect		();
		xr_delete				(Server);
	}
	psDeviceFlags.set(rsDisableObjectsAsCrows, b_stored);

}

BOOL	g_bCalculatePing = FALSE;
void CLevel::ClientSend	()
{
	if (GameID() == GAME_SINGLE || OnClient())
	{
		if (!net_HasBandwidth()) return;
	};
	if (g_bCalculatePing) SendPingMessage();

	NET_Packet				P;
	u32						start	= 0;
	//----------- for E3 -----------------------------
	if (OnClient()) 
	{
//		if (!(Game().local_player) || Game().local_player->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD)) return;
		if (!CurrentControlEntity()) return;
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
		start	= Objects.net_Export	(&P, start, NET_ObjectsPerPacket);
		if (P.B.count>2)				Send	(P, net_flags(FALSE));
		else							break	;
	}
	//-------------------------------------------------
}

u32	CLevel::Objects_net_Save	(NET_Packet* _Packet, u32 start, u32 count)
{
	NET_Packet& Packet	= *_Packet;
	u32			position;
	for (; start<Objects.o_count(); start++)	{
		CObject		*_P = Objects.o_get_by_iterator(start);
		CGameObject *P = smart_cast<CGameObject*>(_P);
		if (P && !P->getDestroy() && P->net_SaveRelevant())	{
			Packet.w_u16			(u16(P->ID())	);
			Packet.w_chunk_open16	(position);
			P->net_Save				(Packet);
#ifdef DEBUG
			u32 size				= u32		(Packet.w_tell()-position)-sizeof(u16);
			if				(size>=65536)			{
				Debug.fatal	("Object [%s][%d] exceed network-data limit\n size=%d, Pend=%d, Pstart=%d",
					*P->cName(), P->ID(), size, Packet.w_tell(), position);
			}
#endif
			Packet.w_chunk_close16	(position);
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
		
		start		= Objects_net_Save(&P, start, NET_ObjectsPerPacket);

		if (P.B.count>2)
			Send	(P, net_flags(FALSE));
		else
			break;
	}
}

extern		 float		phTimefactor;
void CLevel::Send		(NET_Packet& P, u32 dwFlags, u32 dwTimeout)
{
	// optimize the case when server located in our memory
	if (Server && game_configured && OnServer()){
		Server->OnMessage	(P,Game().local_svdpnid	);
	}else											IPureClient::Send	(P,dwFlags,dwTimeout	);
	if (g_pGameLevel && Level().game && GameID() != GAME_SINGLE)		{
		// anti-cheat
		phTimefactor		= 1.f					;
		psDeviceFlags.set	(rsConstantFPS,FALSE)	;	
	}
}

void CLevel::net_Update	()
{
	if(game_configured){
		// If we have enought bandwidth - replicate client data on to server
		Device.Statistic->netClient.Begin	();
		ClientSend							();
		Device.Statistic->netClient.End		();
	}
	// If server - perform server-update
	if (Server && OnServer())	{
		Device.Statistic->netServer.Begin();
		Server->Update					();
		Device.Statistic->netServer.End	();
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

BOOL			CLevel::Connect2Server				(LPCSTR options)
{
	NET_Packet					P;
	m_bConnectResultReceived	= false	;
	m_bConnectResult			= true	;
	if (!Connect(options))		return	FALSE;
	//---------------------------------------------------------------------------
	while	(!m_bConnectResultReceived)		{ 
		ClientReceive	();
		Sleep			(5); 
		if(Server)
			Server->Update()	;
	}
	Msg							("%c client : connection %s - <%s>", m_bConnectResult ?'*':'!', m_bConnectResult ? "accepted" : "rejected", m_sConnectResult.c_str());
	if		(!m_bConnectResult) 
	{
		OnConnectRejected	();	
		Disconnect		()	;
		return FALSE		;
	};

	while (!net_IsSyncronised()) {
	};

	//---------------------------------------------------------------------------
	P.w_begin	(M_CLIENT_REQUEST_CONNECTION_DATA);
	Send		(P);
	//---------------------------------------------------------------------------
	return TRUE;
};

void			CLevel::OnBuildVersionChallenge		()
{
	NET_Packet P;
	P.w_begin				(M_CL_AUTH);
	u64 auth = FS.auth_get();
	Msg			("auth - %d", auth);
	P.w_u64					(auth);
	Send					(P);
};

void			CLevel::OnConnectResult				(NET_Packet*	P)
{
	// multiple results can be sent during connection they should be "AND-ed"
	m_bConnectResultReceived	= true;
	u8	result					= P->r_u8();
	if (!result)				m_bConnectResult	= false			;	// and
	string128 ResultStr			;
	P->r_stringZ(ResultStr)		;
	m_sConnectResult			= ResultStr;
};

void			CLevel::SendPingMessage				()
{
	u32 CurTime = timeServer_Async();
	if (CurTime < (m_dwCL_PingLastSendTime + m_dwCL_PingDeltaSend)) return;
	u32 m_dwCL_PingLastSendTime = CurTime;
	NET_Packet P;
	P.w_begin		(M_CL_PING_CHALLENGE);
	P.w_u32			(m_dwCL_PingLastSendTime);
	P.w_u32			(m_dwCL_PingLastSendTime);
	P.w_u32			(m_dwRealPing);
	Send	(P, net_flags(FALSE));
};

void			CLevel::ClearAllObjects				()
{
	u32 CLObjNum = Level().Objects.o_count();

	bool ParentFound = true;
	
	while (ParentFound)
	{	
		ParentFound = false;
		for (u32 i=0; i<CLObjNum; i++)
		{
			CObject* pObj = Level().Objects.o_get_by_iterator(i);
			if (!pObj->H_Parent()) continue;
			//-----------------------------------------------------------
			NET_Packet			GEN;
			GEN.w_begin			(M_EVENT);
			//---------------------------------------------		
			GEN.w_u32			(Level().timeServer());
			GEN.w_u16			(GE_OWNERSHIP_REJECT);
			GEN.w_u16			(pObj->H_Parent()->ID());
			GEN.w_u16			(u16(pObj->ID()));
			game_events->insert	(GEN);
			if (g_bDebugEvents)	ProcessGameEvents();
			//-------------------------------------------------------------
			ParentFound = true;
			//-------------------------------------------------------------
#ifdef DEBUG
			Msg ("Rejection of %s[%d] from %s[%d]", *(pObj->cNameSect()), pObj->ID(), *(pObj->H_Parent()->cNameSect()), pObj->H_Parent()->ID());
#endif
		};
		ProcessGameEvents();
	};

	for (u32 i=0; i<CLObjNum; i++)
	{
		CObject* pObj = Level().Objects.o_get_by_iterator(i);
		R_ASSERT(pObj->H_Parent()==NULL);
		//-----------------------------------------------------------
		NET_Packet			GEN;
		GEN.w_begin			(M_EVENT);
		//---------------------------------------------		
		GEN.w_u32			(Level().timeServer());
		GEN.w_u16			(GE_DESTROY);
		GEN.w_u16			(u16(pObj->ID()));
		game_events->insert	(GEN);
		if (g_bDebugEvents)	ProcessGameEvents();
		//-------------------------------------------------------------
		ParentFound = true;
		//-------------------------------------------------------------
#ifdef DEBUG
		Msg ("Destruction of %s[%d]", *(pObj->cNameSect()), pObj->ID());
#endif
	};
	ProcessGameEvents();
};

void				CLevel::OnInvalidHost			()
{
	IPureClient::OnInvalidHost();
	UI()->OnInvalidHost();
};

void				CLevel::OnInvalidPassword		()
{
	IPureClient::OnInvalidPassword();
	UI()->OnInvalidPass();
};

void				CLevel::OnSessionFull			()
{
	IPureClient::OnSessionFull();
	UI()->OnSessionFull();
}

void				CLevel::OnConnectRejected		()
{
	IPureClient::OnConnectRejected();
	UI()->OnServerReject();
};