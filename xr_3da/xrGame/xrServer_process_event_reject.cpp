#include "stdafx.h"

void	xrServer::Process_event_reject	(NET_Packet& P, DPNID sender, u32 time, u16 ID)
{
	DWORD		MODE		= net_flags(TRUE,TRUE);

	// Parse message
	u16					id_parent=ID,id_entity;
	P.r_u16				(id_entity);
	CAbstractServerObject*		e_parent	= game->get_entity_from_eid	(id_parent);
	CAbstractServerObject*		e_entity	= game->get_entity_from_eid	(id_entity);
	if (e_parent && e_entity) 
	{
		xrClientData*		c_parent	= e_parent->owner;
		xrClientData*		c_entity	= e_entity->owner;
		// xrClientData*	c_from		= ID_to_client	(sender);
		// R_ASSERT			(c_entity == c_parent);
		// R_ASSERT			(c_parent == c_from);		// assure client only send request for local units

		if (game->OnDetach	(id_parent,id_entity))
		{
			// Perform migration if needed
			xrClientData*		c_dest		= SelectBestClientToMigrateTo		(e_entity);
			if (c_dest	!= c_entity)		PerformMigration					(e_entity,c_parent,c_dest);

			// Rebuild parentness
			R_ASSERT				(0xffff != e_entity->ID_Parent);
			e_entity->ID_Parent		= 0xffff;
			xr_vector<u16>& C			= e_parent->children;
			xr_vector<u16>::iterator c	= std::find	(C.begin(),C.end(),id_entity);
			VERIFY					(c!=C.end());
			C.erase					(c);

			// Signal to everyone (including sender)
			SendBroadcast			(0xffffffff,P,MODE);
		}
	}
}
