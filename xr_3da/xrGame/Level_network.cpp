#include "stdafx.h"

void CLevel::net_Stop		()
{
	Msg							("- Disconnect");
	IPureClient::Disconnect		();
	if (Server)
	{
		Server->Disconnect	();
		_DELETE				(Server);
	}
}

void CLevel::ClientSend	()
{
	if (!net_HasBandwidth())	return;

	NET_Packet				P;
	P.w_begin				(M_UPDATE);
	Objects.net_Export		(&P);
	if (P.B.count>2)		Send(P,net_flags(FALSE));
}

