#include "stdafx.h"
#include "xrServer.h"
#include "game_sv_single.h"
#include "ai_alife.h"
#include "xrserver_objects.h"

void xrServer::Process_event	(NET_Packet& P, DPNID sender)
{
	u32			timestamp;
	u16			type;
	u16			destination;
	u32			MODE			= net_flags(TRUE,TRUE);

//	xrClientData *l_pC = ID_to_client(sender);

	// correct timestamp with server-unique-time (note: direct message correction)
	P.r_u32		(timestamp	);
	/*
	xrClientData*	c_sender	= ID_to_client	(sender);
	if (c_sender)
	{
		u32			sv_timestamp	= Device.TimerAsync	() - (c_sender->stats.getPing()/2);		// approximate time this message travels
		timestamp					= (timestamp+sv_timestamp)/2;								// approximate timestamp with both client and server time
		CopyMemory	(&P.B.data[P.r_pos-4], &timestamp, 4);
	}
	*/

	// read generic info
	P.r_u16		(type		);
	P.r_u16		(destination);

	CSE_Abstract*	receiver	= game->get_entity_from_eid	(destination);
	if (receiver)	receiver->OnEvent						(P,type,timestamp,sender);

	switch		(type)
	{
	case GEG_PLAYER_CHANGE_TEAM:
		{
			xrClientData *l_pC = ID_to_client(sender);
			s16 l_team; P.r_s16(l_team); P.r_s16(l_team);
			game->OnPlayerChangeTeam(l_pC->ID, l_team);
		}
		break;
	case GEG_PLAYER_KILL:
		{
			xrClientData *l_pC = ID_to_client(sender);
			game->OnPlayerWantsDie(l_pC->ID);
		}break;
	case GEG_PLAYER_READY:
		{
			CSE_Abstract*		E			= game->get_entity_from_eid	(destination);
			if (E) {
				xrClientData*		C			= E->owner;
				if (C && (C->owner == E))
				{
					game->OnPlayerReady		(C->ID);
				}
			}
		}
		break;
	case GE_INFO_TRANSFER:
		SendBroadcast			(0xffffffff,P,MODE);
		break;
	case GE_PDA:
		SendBroadcast			(0xffffffff,P,MODE);
		break;
	case GE_INV_ACTION:
		{
			xrClientData* CL		= ID_to_client(sender);
			if (CL)	CL->net_Ready	= TRUE;
			if (SV_Client) SendTo(SV_Client->ID, P, net_flags(TRUE, TRUE));
		}break;
	case GE_BUY:
		{
			string64			i_name;
			P.r_string			(i_name);
			CSE_Abstract*		E			= game->get_entity_from_eid	(destination);
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
			CSE_Abstract*		E	= game->get_entity_from_eid	(destination);
			if (E) 
			{
				R_ASSERT			(E->s_flags.is(M_SPAWN_OBJECT_PHANTOM));

				svs_respawn			R;
				R.timestamp			= timestamp	+ E->RespawnTime*1000;
				R.phantom			= destination;
				q_respawn.insert	(R);
			}
		}
		break;
	case GE_WPN_STATE_CHANGE:
		SendBroadcast			(0xffffffff,P,MODE);
		break;
	case GE_TRADE_BUY:
	case GE_OWNERSHIP_TAKE:
		Process_event_ownership	(P,sender,timestamp,destination);
		break;
	case GE_TRADE_SELL:
	case GE_OWNERSHIP_REJECT:
		Process_event_reject	(P,sender,timestamp,destination);
		break;
	case GE_TRANSFER_AMMO:
		{
			u16					id_parent=destination,id_entity;
			P.r_u16				(id_entity);
			CSE_Abstract*		e_parent	= game->get_entity_from_eid	(id_parent);	// ��� �������� (��� ����� ����)
			CSE_Abstract*		e_entity	= game->get_entity_from_eid	(id_entity);	// ��� ������
			if (!e_entity)		break;
			if (0xffff != e_entity->ID_Parent)	break;						// this item already taken
			xrClientData*		c_parent	= e_parent->owner;
			xrClientData*		c_from		= ID_to_client	(sender);
			R_ASSERT			(c_from == c_parent);						// assure client ownership of event

			// Signal to everyone (including sender)
			SendBroadcast		(0xffffffff,P,MODE);

			// Perfrom real destroy
			entity_Destroy		(e_entity	);
		}
		break;
	case GE_HIT:
		{
			// Parse message
			u16					id_dest		=	destination, id_src;
			P.r_u16				(id_src);
			/*CSE_Abstract*	e_dest		= game->get_entity_from_eid	(id_dest);*/	// ��� ����������
			CSE_Abstract*		e_src		= game->get_entity_from_eid	(id_src	); if(!e_src) break; // @@@ WT		// ��������� ����
//			xrClientData*		c_src		= e_src->owner;
//			xrClientData*		c_from		= ID_to_client	(sender);
//			R_ASSERT			(c_src == c_from);							// assure client ownership of event

//			CSE_Abstract*		e_hitter = e_src;
//			CSE_Abstract*		e_hitted = receiver;

			game->OnHit(id_src, id_dest, P);
			
			// Signal just to destination (����, ��� ����������)
			SendBroadcast		(0xffffffff,P,MODE);
		}
		break;
	case GE_DIE:
		{
			// Parse message
			u16					id_dest		=	destination, id_src;
			P.r_u16				(id_src);


			xrClientData *l_pC = ID_to_client(sender);
			Msg		("* [%2d] killed by [%2d] - sended by [%s:%2d]", id_dest, id_src, game->get_option_s(l_pC->Name,"name","Player"), l_pC->owner->ID);

			CSE_Abstract*		e_dest		= game->get_entity_from_eid	(id_dest);	// ��� ����
			Msg		("* [%2d] is [%s:%s]", id_dest, e_dest->s_name, e_dest->s_name_replace);
			CSE_Abstract*		e_src		= game->get_entity_from_eid	(id_src	);	// ��� ����
			R_ASSERT2			(e_dest && e_src, "Killer or/and being killed are offline or not exist at all :(");
			Msg		("* [%2d] is [%s:%s]", id_src, e_src->s_name, e_src->s_name_replace);

			xrClientData*		c_dest		= e_dest->owner;			// ������, ��� ���� ����
			xrClientData*		c_src		= e_src->owner;				// ������, ��� ���� ����
//			xrClientData*		c_from		= ID_to_client	(sender);	// ������, ������ ������ �������
//			R_ASSERT2			(c_dest == c_from, "Security error (SSU :)");// assure client ownership of event

			//
			
			if (e_src->s_flags.is(M_SPAWN_OBJECT_ASPLAYER) && e_dest->s_flags.is(M_SPAWN_OBJECT_ASPLAYER)) {
				game->OnPlayerKillPlayer	(c_src->ID,c_dest->ID);
			}

			if (c_src->owner->ID == id_src) {
				// Main unit
				P.w_begin			(M_EVENT);
				P.w_u32				(timestamp);
				P.w_u16				(type);
				P.w_u16				(destination);
				P.w_u16				(id_src);
				P.w_u32				(c_src->ID);
			}

			SendBroadcast		(0xffffffff,P,MODE);
		}
		break;
	case GE_DESTROY:
		{
			// Parse message
			u16					id_dest		=	destination;
			CSE_Abstract*		e_dest		=	game->get_entity_from_eid	(id_dest);	// ��� ������ ���� ���������
			if (0==e_dest)		Msg			("SV:ge_destroy: [%d]",id_dest);

			R_ASSERT			(e_dest			);
			xrClientData*		c_dest		=	e_dest->owner;				// ������, ��� ����
			R_ASSERT		(c_dest			);
			xrClientData*		c_from		=	ID_to_client	(sender);	// ������, ��� �������
			R_ASSERT			(c_dest == c_from);							// assure client ownership of event

			
			SendBroadcast		(0xffffffff,P,MODE);
			// Parent-signal
			if (0xffff != e_dest->ID_Parent)
			{
				NET_Packet			P2;
				P2.w_begin			(M_EVENT);
				P2.w_u32			(timestamp);
				P2.w_u16			(GE_OWNERSHIP_REJECT);
				P2.w_u16			(e_dest->ID_Parent);
				P2.w_u16			(id_dest);
				SendBroadcast		(0xffffffff,P2,MODE);
			}

			// Everything OK, so perform entity-destroy
			if (e_dest->m_bALifeControl) {
				game_sv_Single *tpGame = dynamic_cast<game_sv_Single*>(game);
				VERIFY(tpGame);
//				if (e_dest->ID_Parent == 0xffff) {
					tpGame->m_tpALife->vfReleaseObject(e_dest,false);
//				}
			}
			entity_Destroy		(e_dest);
		}
		break;
	case GE_GRENADE_EXPLODE:
		{
			SendBroadcast		(0xffffffff,P,MODE);
		}break;
	default:
		R_ASSERT2	(0,"Game Event not implemented!!!");
		break;
	}
}
