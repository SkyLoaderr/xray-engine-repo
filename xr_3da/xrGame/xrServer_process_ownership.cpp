#include "stdafx.h"

void xrServer::Process_ownership(NET_Packet& P, DPNID sender)
{
	// Parse message
	u16					id_parent,id_entity;
	P.r_u16				(id_parent);
	P.r_u16				(id_entity);
	xrServerEntity*		e_parent	= ID_to_entity	(id_parent);
	xrServerEntity*		e_entity	= ID_to_entity	(id_entity);
	xrClientData*		c_parent	= e_parent->owner;
	xrClientData*		c_entity	= e_entity->owner;
	xrClientData*		c_from		= ID_to_client	(sender);
	R_ASSERT			(c_parent == c_from);		// assure client only send request for local units

	// Perform migration if needed
	if (c_parent != c_entity)		PerformMigration(e_entity,c_entity,c_parent);

	// Rebuild parentness
	R_ASSERT			(0xffff == e_entity->ID_Parent);
	e_entity->ID_Parent	= id_parent;

	// Signal to everyone (including sender)
	SendBroadcast		(0xffffffff,P,net_flags(TRUE,TRUE));
}
