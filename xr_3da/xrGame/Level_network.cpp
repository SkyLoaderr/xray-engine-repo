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
	if (P.B.count>2)		Send(P,net_flags(FALSE));
}

void CLevel::Send		(NET_Packet& P, u32 dwFlags, u32 dwTimeout)
{
	// optimize the case when server located in our memory
	if (Server)			Server->OnMessage	(P,Game().local_svdpnid	);
	else				IPureClient::Send	(P,dwFlags,dwTimeout	);
}
