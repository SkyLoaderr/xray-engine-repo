#include "stdafx.h"

void CLevel::net_Stop		()
{
	Msg							("- Disconnect");
	IGame_Level::net_Stop		();
	IPureClient::Disconnect		();
	if (Server)
	{
		Server->Disconnect		();
		xr_delete				(Server);
	}
}

void CLevel::ClientSend	()
{
	if (!net_HasBandwidth() || OnClient())	return;
	NET_Packet				P;
	P.w_begin				(M_UPDATE);
	Objects.net_Export		(&P);
//	Msg						("%6d : %d",timeServer(),P.w_tell());
	if (P.B.count>2)		Send(P,net_flags(FALSE));
}

void CLevel::Send		(NET_Packet& P, u32 dwFlags, u32 dwTimeout)
{
	// optimize the case when server located in our memory
	if (Server && game_configured && OnServer())	Server->OnMessage	(P,Game().local_svdpnid	);
	else							IPureClient::Send	(P,dwFlags,dwTimeout	);
}

void CLevel::net_Update	()
{
	// If we have enought bandwidth - replicate client data on to server
	Device.Statistic.netClient.Begin	();
	ClientSend							();
	Device.Statistic.netClient.End		();

	// If server - perform server-update
	if (Server)	{
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
