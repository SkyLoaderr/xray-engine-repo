#include "stdafx.h"

void xrServer::Process_event_ownership(NET_Packet& P, DPNID sender, u32 time, u16 ID)
{
	DWORD				MODE		= net_flags		(TRUE,TRUE);

	// Parse message
	u16					id_parent=ID,id_entity;
	P.r_u16				(id_entity);
	xrServerEntity*		e_parent	= ID_to_entity	(id_parent);
	xrServerEntity*		e_entity	= ID_to_entity	(id_entity);
	if (0		==	e_entity)				return;
	if (0xffff	!=	e_entity->ID_Parent)	return;
	xrClientData*		c_parent	= e_parent->owner;
	xrClientData*		c_entity	= e_entity->owner;
	xrClientData*		c_from		= ID_to_client	(sender);
	R_ASSERT			(c_parent == c_from);		// assure client only send request for local units

	switch (GAME)
	{
	case GAME_SINGLE:
	case GAME_DEATHMATCH:
		{
			// Perform migration if needed
			if (c_parent != c_entity)		PerformMigration(e_entity,c_entity,c_parent);

			// Rebuild parentness
			e_entity->ID_Parent	= id_parent;

			// Signal to everyone (including sender)
			SendBroadcast		(0xffffffff,P,MODE);
		}
		break;
	case GAME_ASSAULT:
		{
			// Check if it's special game-play item
			xrSE_Target_Assault*	T	= dynamic_cast<xrSE_Target_Assault*> (e_entity);
			if (T && (0==T->g_team()) && (1==e_parent->g_team()))
			{
				// Task acomplished

				// ???????????????????????????????????????????
			} else {
				// It's some generic item - perform migration if needed
				if (c_parent != c_entity)		PerformMigration(e_entity,c_entity,c_parent);

				// Rebuild parentness
				e_entity->ID_Parent	= id_parent;

				// Signal to everyone (including sender)
				SendBroadcast		(0xffffffff,P,MODE);
			}
		}
		break;
	}
}
