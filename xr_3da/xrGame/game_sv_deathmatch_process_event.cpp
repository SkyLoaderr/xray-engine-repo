#include "stdafx.h"
#include "game_sv_deathmatch.h"
#include "xrServer.h"
#include "xrMessages.h"

void	game_sv_Deathmatch::OnEvent (NET_Packet &P, u16 type, u32 time, ClientID sender )
{

	switch	(type)
	{


	case GAME_EVENT_PLAYER_CHANGE_SKIN: //dm only
		{
			xrClientData *l_pC = m_server->ID_to_client(sender);
			u8 l_skin;
			P.r_u8(l_skin);
			OnPlayerChangeSkin(l_pC->ID, l_skin);

/*			game_event e; e.create();
			e.w->w_u32( l_pC->ID );
			e.w->w_u8(P.r_u8());//skin
			m_event_registry.AddEvent(eGamePropertyHavePlayerChangeSkin,e);*/

		}break;

	case GAME_EVENT_PLAYER_KILL: //dm only  (g_kill)
		{
			xrClientData *l_pC = m_server->ID_to_client(sender);
			KillPlayer(l_pC->ID);
//			OnPlayerWantsDie(l_pC->ID);

/*			game_event e; e.create();
			e.w->w_u32( l_pC->ID );
			m_event_registry.AddEvent(eGamePropertyHavePlayerKilled,e);*/
		}break;


	case GAME_EVENT_PLAYER_BUY_FINISHED: // dm only
		{
			xrClientData *l_pC = m_server->ID_to_client(sender);
			OnPlayerBuyFinished(l_pC->ID, P);

/*			game_event e; e.create();
			e.w->w_u32( l_pC->ID );
			m_event_registry.AddEvent(eGamePropertyHavePlayerBuyFinished,e);*/
		}break;

	default:
		inherited::OnEvent(P, type, time, sender);
	};//switch
}


