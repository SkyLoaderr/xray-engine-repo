#include "stdafx.h"
#include "xrServer.h"

void xrServer::Process_update(NET_Packet& P, DPNID sender)
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
