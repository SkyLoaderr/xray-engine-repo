#include "stdafx.h"
#include "xrserver.h"
#include "xrserver_objects.h"

void	xrServer::Process_event_reject	(NET_Packet& P, DPNID sender, u32 time, u16 ID)
{
	DWORD		MODE		= net_flags(TRUE,TRUE);

	// Parse message
	u16					id_parent=ID,id_entity;
	P.r_u16				(id_entity);
	CSE_Abstract*		e_parent	= game->get_entity_from_eid	(id_parent);
	CSE_Abstract*		e_entity	= game->get_entity_from_eid	(id_entity);
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
			if (0xffff == e_entity->ID_Parent)
			{
				Msg						("! ERROR: can't detach independant object. entity[%s:%d], parent[%s:%d], section[%s]",
					e_entity->s_name_replace,id_entity,e_parent->s_name_replace,id_parent, e_entity->s_name);
			} else {
				R_ASSERT				(0xffff != e_entity->ID_Parent);
				e_entity->ID_Parent		= 0xffff;
				xr_vector<u16>& C			= e_parent->children;
				xr_vector<u16>::iterator c	= std::find	(C.begin(),C.end(),id_entity);
				VERIFY					(C.end()!=c);
				C.erase					(c);

				// Signal to everyone (including sender)
				SendBroadcast			(0xffffffff,P,MODE);
			}
		}
	}
}
