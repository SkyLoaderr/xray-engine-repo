#include "stdafx.h"

void xrServer::Process_event_ownership(NET_Packet& P, DPNID sender, u32 time, u16 ID)
{
	u32				MODE		= net_flags		(TRUE,TRUE);

	// Parse message
	u16					id_parent=ID,id_entity;
	P.r_u16				(id_entity);
	xrServerEntity*		e_parent	= ID_to_entity	(id_parent);
	xrServerEntity*		e_entity	= ID_to_entity	(id_entity);
	if (0		==	e_parent)				return;
	if (0		==	e_entity)				return;
	if (0xffff	!=	e_entity->ID_Parent)	return;
	xrClientData*		c_parent	= e_parent->owner;
	xrClientData*		c_entity	= e_entity->owner;
	xrClientData*		c_from		= ID_to_client	(sender);
	R_ASSERT			(c_parent == c_from);		// assure client only send request for local units


	if (game->OnTargetTouched(sender,id_entity))
	{
		// Game allows ownership of entity

		// Perform migration if needed
		if (c_parent != c_entity)		PerformMigration(e_entity,c_entity,c_parent);

		// Rebuild parentness
		e_entity->ID_Parent			= id_parent;
		e_parent->chields.push_back	(id_entity);

		// Signal to everyone (including sender)
		SendBroadcast		(0xffffffff,P,MODE);
	}
}
