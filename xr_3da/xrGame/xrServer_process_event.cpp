#include "stdafx.h"
#include "xrServer.h"

void xrServer::Process_event	(NET_Packet& P, DPNID sender)
{
	u32			timestamp;
	u16			type;
	u16			destination;
	u32			MODE			= net_flags(TRUE,TRUE);

	P.r_u32		(timestamp);
	P.r_u16		(type);
	P.r_u16		(destination);

	switch		(type)
	{
	case GEG_PLAYER_READY:
		{
			xrServerEntity*		E			= ID_to_entity	(destination);
			if (E) {
				xrClientData*		C			= E->owner;
				if (C && (C->owner == E))
				{
					game->OnPlayerReady		(C->ID);
				}
			}
		}
		break;
	case GE_BUY:
		{
			string64			i_name;
			P.r_string			(i_name);
			xrServerEntity*		E			= ID_to_entity	(destination);
			if (E) {
				xrClientData*		C			= E->owner;
				if (C && (C->owner == E))
				{
					game->OnPlayerBuy		(C->ID,destination,i_name);
				}
			}
		}
		break;
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
		Process_event_ownership	(P,sender,timestamp,destination);
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
			// R_ASSERT			(c_entity == c_parent);
			// R_ASSERT			(c_parent == c_from);		// assure client only send request for local units

			if (game->OnTargetDetouched(sender,id_entity))
			{
				// Perform migration if needed
				xrClientData*		c_dest		= SelectBestClientToMigrateTo		(e_entity);
				if (c_dest	!= c_entity)		PerformMigration					(e_entity,c_parent,c_dest);

				// Rebuild parentness
				R_ASSERT			(0xffff != e_entity->ID_Parent);
				e_entity->ID_Parent	= 0xffff;

				// Signal to everyone (including sender)
				SendBroadcast		(0xffffffff,P,MODE);
			}
		}
		break;
	case GE_TRANSFER_AMMO:
		{
			u16					id_parent=destination,id_entity;
			P.r_u16				(id_entity);
			xrServerEntity*		e_parent	= ID_to_entity	(id_parent);	// кто забирает (для своих нужд)
			xrServerEntity*		e_entity	= ID_to_entity	(id_entity);	// кто отдает
			if (!e_entity)		break;
			if (0xffff != e_entity->ID_Parent)	break;						// this item already taken
			xrClientData*		c_parent	= e_parent->owner;
			xrClientData*		c_from		= ID_to_client	(sender);
			R_ASSERT			(c_from == c_parent);						// assure client ownership of event

			// Signal to everyone (including sender)
			SendBroadcast		(0xffffffff,P,MODE);

			// Perfrom real destroy
			entity_Destroy		(e_entity	);
			entities.erase		(id_entity	);
		}
		break;
	case GE_HIT:
		{
			// Parse message
			u16					/*id_dest		=	destination,*/ id_src;
			P.r_u16				(id_src);
			/*xrServerEntity*	e_dest		= ID_to_entity	(id_dest);*/	// кто повредился
			xrServerEntity*		e_src		= ID_to_entity	(id_src	);		// благодаря кому
			xrClientData*		c_src		= e_src->owner;
			xrClientData*		c_from		= ID_to_client	(sender);
			R_ASSERT			(c_src == c_from);							// assure client ownership of event

			// Signal just to destination (тому, кто повредился)
			SendBroadcast		(0xffffffff,P,MODE);
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
			R_ASSERT			(c_dest == c_from);						// assure client ownership of event

			//
			if (c_src->owner->ID == id_src)
			{
				game->OnPlayerKillPlayer	(c_src->ID,c_dest->ID);

				// Main unit
				P.w_begin					(M_EVENT);
				P.w_u32						(timestamp);
				P.w_u16						(type);
				P.w_u16						(destination);
				P.w_u16						(id_src);
				P.w_u32						(c_src->ID);
			}
			SendBroadcast		(0xffffffff,P,MODE);
		}
		break;
	case GE_DESTROY:
		{
			// Parse message
			u16					id_dest		=	destination;
			xrServerEntity*		e_dest		=	ID_to_entity	(id_dest);	// кто должен быть уничтожен
			R_ASSERT			(e_dest			);
			xrClientData*		c_dest		=	e_dest->owner;				// клиент, чей юнит
			R_ASSERT			(c_dest			);
			xrClientData*		c_from		=	ID_to_client	(sender);	// клиент, кто прислал
			R_ASSERT			(c_dest == c_from);							// assure client ownership of event

			// Parent-signal
			if (0xffff != e_dest->ID_Parent)
			{
				NET_Packet			P2;
				P2.w_begin			(M_EVENT);
				P2.w_u32			(timestamp-1);
				P2.w_u16			(GE_OWNERSHIP_REJECT);
				P2.w_u16			(e_dest->ID_Parent);
				P2.w_u16			(id_dest);
				SendBroadcast		(0xffffffff,P2,MODE);
			}

			// Everything OK, so perform entity-destroy
			entity_Destroy		(e_dest);
			entities.erase		(id_dest);
			SendBroadcast		(0xffffffff,P,MODE);
		}
		break;
	default:
		R_ASSERT2	(0,"Game Event not implemented!!!");
		break;
	}
}
