#include "stdafx.h"
#include "xrServer.h"
#include "xrServer_Objects.h"

void xrServer::Process_update(NET_Packet& P, ClientID sender)
{
	xrClientData* CL		= ID_to_client(sender);
	if (!CL)
	{
		return;
	}
//	if (CL)	CL->net_Ready	= TRUE;

	R_ASSERT(CL->flags.bLocal);
	// while has information
	while (!P.r_eof())
	{
		// find entity
		u16				ID;
		u8				size;

		P.r_u16			(ID);
		P.r_u8			(size);
		CSE_Abstract	*E	= ID_to_entity(ID);
		
		if (E) {
			E->net_Ready	= TRUE;
			E->UPDATE_Read	(P);
		}
		else
			P.r_advance	(size);
	}
}

void xrServer::Process_save(NET_Packet& P, ClientID sender)
{
	xrClientData* CL		= ID_to_client(sender);
	if (CL)	CL->net_Ready	= TRUE;

	R_ASSERT(CL->flags.bLocal);
	// while has information
	while (!P.r_eof())
	{
		// find entity
		u16				ID;
		u16				size;

		P.r_u16			(ID);
		P.r_u16			(size);
		u32				_pos_start	= P.r_tell	();
		CSE_Abstract	*E	= ID_to_entity(ID);

		if (E) {
			E->net_Ready = TRUE;
			E->load		(P);
		}
		else
			P.r_advance	(size);
		u32				_pos_end	= P.r_tell	();
		VERIFY3			(size == (_pos_end-_pos_start),"load/save mismatch",E?E->s_name.c_str():"unknown");
	}
}
