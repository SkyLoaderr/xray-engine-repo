#include "stdafx.h"
#include "xrServer.h"

void xrServer::Process_event	(NET_Packet& P, DPNID sender)
{
	u32			timestamp;
	u16			type;
	u16			destination;

	P.r_u32		(timestamp);
	P.r_u16		(type);
	P.r_u16		(destination);

	switch		(type)
	{
	case GE_RESPAWN:
		{
			xrServerEntity*		E	= ID_to_entity	(destination);
			if (E) 
			{
				R_ASSERT			(E->s_flags&M_SPAWN_OBJECT_PHANTOM);

				svs_respawn			R;
				R.timestamp			= timestamp	+ E->RespawnTime*1000;
				R.phantom			= destination;
				q_respawn.insert	(R);
			}
		}
		break;
	case GE_OWNERSHIP_TAKE:
		{
			// Parse message
			u16					id_parent=destination,id_entity;
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
		break;
	case GE_OWNERSHIP_REJECT:
		{
			// Parse message
			u16					id_parent=destination,id_entity;
			P.r_u16				(id_entity);
			xrServerEntity*		e_parent	= ID_to_entity	(id_parent);
			xrServerEntity*		e_entity	= ID_to_entity	(id_entity);
			xrClientData*		c_parent	= e_parent->owner;
			xrClientData*		c_entity	= e_entity->owner;
			xrClientData*		c_from		= ID_to_client	(sender);
			R_ASSERT			(c_entity == c_parent);
			R_ASSERT			(c_parent == c_from);		// assure client only send request for local units

			// Perform migration if needed
			xrClientData*		c_dest		= SelectBestClientToMigrateTo		(e_entity);
			if (c_dest	!= c_entity)		PerformMigration(e_entity,c_parent,c_dest);

			// Rebuild parentness
			R_ASSERT			(0xffff != e_entity->ID_Parent);
			e_entity->ID_Parent	= 0xffff;

			// Signal to everyone (including sender)
			SendBroadcast		(0xffffffff,P,net_flags(TRUE,TRUE));
		}
		break;
	case GE_TRANSFER_AMMO:
		{
			// Signal to everyone (including sender)
			SendBroadcast		(0xffffffff,P,net_flags(TRUE,TRUE));
		}
		break;
	case GE_HIT:
		{
			// Parse message
			u16					id_dest		=	destination, id_src;
			P.r_u16				(id_src);
			xrServerEntity*		e_dest		= ID_to_entity	(id_dest);	// кто повредился
			xrServerEntity*		e_src		= ID_to_entity	(id_src	);	// благодаря кому
			xrClientData*		c_dest		= e_dest->owner;
			xrClientData*		c_src		= e_src->owner;
			xrClientData*		c_from		= ID_to_client	(sender);
			R_ASSERT			(c_src == c_from);		// assure client ownership of event

			// Signal just to destination (тому, кто повредился)
			SendBroadcast		(0xffffffff,P,net_flags(TRUE,TRUE));
		}
		break;
	case GE_DIE:
		{
			// Parse message
			u16					id_dest		=	destination, id_src;
			P.r_u16				(id_src);
			xrServerEntity*		e_dest		= ID_to_entity	(id_dest);	// кто умер
			xrServerEntity*		e_src		= ID_to_entity	(id_src	);	// кто убил

			xrClientData*		c_dest		= e_dest->owner;			// клиент, чей юнит умер
			xrClientData*		c_src		= e_src->owner;				// клиент, чей юнит убил
			xrClientData*		c_from		= ID_to_client	(sender);	// клиент, откуда пришла мессага
			R_ASSERT			(c_dest == c_from);		// assure client ownership of event

			//
			if (c_src->owner->ID == id_src)
			{
				// Main unit
				c_src->g_frags		++;

				P.w_begin			(M_EVENT);
				P.w_u32				(timestamp);
				P.w_u16				(type);
				P.w_u16				(destination);
				P.w_u16				(id_src);
				P.w_u32				(c_src->ID);
			}
			SendBroadcast		(0xffffffff,P,net_flags(TRUE,TRUE));
		}
		break;
	default:
		R_ASSERT2	(0,"Game Event not implemented!!!");
		break;
	}
}
