#include "stdafx.h"
#include "xrServer.h"

void xrServer::Process_update(NET_Packet& P, DPNID sender)
{
	xrClientData* CL		= ID_to_client(sender);
	if (CL)	CL->net_Ready	= TRUE;

	// while has information
	while (!P.r_eof())
	{
		// find entity
		u16				ID;
		u8				size;

		P.r_u16			(ID);
		P.r_u8			(size);
		xrServerEntity* E	= ID_to_entity(ID);
		if (E)				
		{
			E->net_Ready	= TRUE;
			E->UPDATE_Read	(P);
		} else {
			P.r_advance	(size);
		}
	}
}
